/*******************************************************************************
 * Copyright (c) 2015 Matthijs Kooijman
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * This the HAL to run LMIC on top of the Arduino environment.
 *******************************************************************************/

#include "../lmic.h"
#include "hal.h"
#include <stdio.h>
#include "mbed.h"
#include <InterruptIn.h>
// -----------------------------------------------------------------------------
// I/O

extern void radio_irq_handler( u1_t dio );

static DigitalOut nss( PTD0 );
static SPI spi(PTD2, PTD3, PTD1); // mosi, miso, sclk
static DigitalInOut rst( PTC7 );//dio3
static DigitalOut rxtx( PTC0 ); 
static InterruptIn dio0( PTA12 );
static InterruptIn dio1( PTA4 );
static InterruptIn dio2( PTA5 ); 

// static DigitalOut nss( PTD0 );
// static SPI spi(PTD2, PTD3, PTD1); // mosi, miso, sclk
// static DigitalInOut rst( PTC8 );//dio3
// static DigitalOut rxtx( PTC4 ); 
// static InterruptIn dio0( PTC7 );
// static InterruptIn dio1( PTC9 );
// static InterruptIn dio2( PTC6); 


static void dio0Irq( void ) {
  //  printf("\r---- 0 ----\n");
    radio_irq_handler( 0 );
}

static void dio1Irq( void ) {
   // printf("\r---- 1 ----\n");
    radio_irq_handler( 1 );
}

static void dio2Irq( void ) {
    //printf("\r---- 2 ----\n");
    radio_irq_handler( 2 );
}


static u1_t irqlevel = 0;
static u4_t ticks = 0;

static Timer timer;
static Ticker ticker;

static void reset_timer( void ) {
    ticks += timer.read_us( ) >> 6;
    timer.reset( );
}
static void hal_io_init ( void ) {
     __disable_irq( );
     irqlevel = 0;
    // configure input lines
    //dio0.mode( PullDown );
    dio0.rise( dio0Irq );
    dio0.enable_irq( );
   // dio1.mode( PullDown );   
    dio1.rise( dio1Irq );
    dio1.enable_irq( );
   // dio2.mode( PullDown );
    dio2.rise( dio2Irq );
    dio2.enable_irq( );
    // configure reset line
    rst.input();
    __enable_irq( );

}

// val == 1  => tx 1
void hal_pin_rxtx( u1_t val ) {
   // printf("\r---- rtx ----\n");
    rxtx = !val;
}

// set radio RST pin to given value (or keep floating!)
void hal_pin_rst( u1_t val ) {
    if( val == 0 || val == 1 )
    { // drive pin
        rst.output( );
        rst = val;
    } 
    else
    { // keep pin floating
        rst.input( );
    }
}
// -----------------------------------------------------------------------------
// SPI

//static const SPISettings settings(10E6, MSBFIRST, SPI_MODE0);

static void hal_spi_init () {
    // configure spi
    spi.frequency( 10E6 );
    spi.format( 8, 0 );
    nss = 1;
}
// perform SPI transaction with radio
u1_t hal_spi( u1_t out ) {
    return spi.write( out );
}

void hal_pin_nss( u1_t val ) {
    nss = val;
}

// -----------------------------------------------------------------------------
// TIME

static void hal_time_init () {
 // configure timer
    timer.start( );
    ticker.attach_us( &reset_timer, 10000000 ); // reset timer every 10sec
 }

u4_t hal_ticks () {
    // Because micros() is scaled down in this function, micros() will
    // overflow before the tick timer should, causing the tick timer to
    // miss a significant part of its values if not corrected. To fix
    // this, the "overflow" serves as an overflow area for the micros()
    // counter. It consists of three parts:
    //  - The US_PER_OSTICK upper bits are effectively an extension for
    //    the micros() counter and are added to the result of this
    //    function.
    //  - The next bit overlaps with the most significant bit of
    //    micros(). This is used to detect micros() overflows.
    //  - The remaining bits are always zero.
    //
    // By comparing the overlapping bit with the corresponding bit in
    // the micros() return value, overflows can be detected and the
    // upper bits are incremented. This is done using some clever
    // bitwise operations, to remove the need for comparisons and a
    // jumps, which should result in efficient code. By avoiding shifts
    // other than by multiples of 8 as much as possible, this is also
    // efficient on AVR (which only has 1-bit shifts).
    static uint8_t overflow = 0;

    // Scaled down timestamp. The top US_PER_OSTICK_EXPONENT bits are 0,
    // the others will be the lower bits of our return value.
    uint32_t scaled = ticks + (timer.read_us() >> US_PER_OSTICK_EXPONENT);
    // Most significant byte of scaled
    uint8_t msb = scaled >> 24;
    // Mask pointing to the overlapping bit in msb and overflow.
    const uint8_t mask = (1 << (7 - US_PER_OSTICK_EXPONENT));
    // Update overflow. If the overlapping bit is different
    // between overflow and msb, it is added to the stored value,
    // so the overlapping bit becomes equal again and, if it changed
    // from 1 to 0, the upper bits are incremented.
    overflow += (msb ^ overflow) & mask;

    // Return the scaled value with the upper bits of stored added. The
    // overlapping bit will be equal and the lower bits will be 0, so
    // bitwise or is a no-op for them.
    return scaled | ((uint32_t)overflow << 24);

    // 0 leads to correct, but overly complex code (it could just return
    // micros() unmodified), 8 leaves no room for the overlapping bit.
    static_assert(US_PER_OSTICK_EXPONENT > 0 && US_PER_OSTICK_EXPONENT < 8, "Invalid US_PER_OSTICK_EXPONENT value");
}

// Returns the number of ticks until time. Negative values indicate that
// time has already passed.
static s4_t delta_time(u4_t time) {
    return (s4_t)(time - hal_ticks());
}

void hal_waitUntil (u4_t time) {
    s4_t delta = delta_time(time);
    // From delayMicroseconds docs: Currently, the largest value that
    // will produce an accurate delay is 16383.
    while (delta > (16000 / US_PER_OSTICK)) {
        wait_ms(16);
        delta -= (16000 / US_PER_OSTICK);
    }
    if (delta > 0)
        wait_us(delta * US_PER_OSTICK);
}

// check and rewind for target time
u1_t hal_checkTimer (u4_t time) {
    // No need to schedule wakeup, since we're not sleeping
    return delta_time(time) <= 0;
}

void hal_disableIRQs( void ) {
    __disable_irq( );
    irqlevel++;
}

void hal_enableIRQs( void ) {
    if( --irqlevel == 0 ) {
        __enable_irq( );
    }
}

void hal_sleep () {
    // Not implemented
}

// -----------------------------------------------------------------------------

void hal_init () {
    // configure radio I/O and interrupt handler
    hal_io_init();
    // configure radio SPI
    hal_spi_init();
    // configure timer and interrupt handler
    hal_time_init();
}

void hal_failed (const char *file, u2_t line) {
    hal_disableIRQs();
    while(1);
}
