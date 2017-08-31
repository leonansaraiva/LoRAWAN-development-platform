# Copyright 2015 Adam Green (http://mbed.org/users/AdamGreen/)
#  the gcc4mbed compressed archive from https://github.com/adamgreen/gcc4mbed/zipball/master
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
PROJECT         := lora
DEVICES         := KL25Z \
                   KL27Z 
GCC4MBED_DIR    := /home/leonan/gcc4mbed

NO_FLOAT_SCANF  := 1
NO_FLOAT_PRINTF := 0


#MBED_LIBS := rtos

include $(GCC4MBED_DIR)/build/gcc4mbed.mk
