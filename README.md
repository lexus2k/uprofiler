
![Tiny Protocol](.travis/tinylogo.svg)<br>
<!-- [![Build Status](https://circleci.com/gh/lexus2k/tinyproto.svg?style=svg)](https://circleci.com/gh/lexus2k/tinyproto) -->
![Github actions](https://github.com/lexus2k/tinyproto/actions/workflows/main.yml/badge.svg)
[![Coverage Status](https://coveralls.io/repos/github/lexus2k/tinyproto/badge.svg?branch=master)](https://coveralls.io/github/lexus2k/tinyproto?branch=master)
[![Documentation](https://codedocs.xyz/lexus2k/tinyproto.svg)](https://codedocs.xyz/lexus2k/tinyproto/)
![License](https://img.shields.io/badge/license-GPLv3-blue)
![License](https://img.shields.io/badge/license-Commercial-blue)


[tocstart]: # (toc start)

  * [Introduction](#introduction)
  * [Key Features](#key-features)
  * [Supported platforms](#supported-platforms)
  * [Easy to use](#easy-to-use)
    * [Cpp](#cpp)
    * [Python](#python)
  * [Setting up](#setting-up)
  * [How to buid](#how-to-build)
  * [Using tiny_loopback tool](#using-tiny_loopback-tool)
  * [License](#license)

[tocend]: # (toc end)

## Introduction

Tiny Protocol is **LAYER 2** protocol. It is intended to be used for the systems with low resources.
It is also can be compiled for desktop Linux system, and it can be built for Windows.
Using this library you can easy implement data transfer between 2 microcontrollers or between microcontroller and pc via UART, SPI,
I2C or any other communication channels.
You don't need to think about data synchronization between points. The library use no dynamic allocation of memory.

TinyProto is based on RFC 1662. Tiny Protocol supports 2 HDLC modes: ABM (for peer to peer connections) and NRM (for
multi-drop connections).

Tiny Protocol is NOT an application layer protocol, although, for example, it can be used with Protocol Buffers. The protocol supports ABM and NRM modes.


## Key Features

Main features:
 * Hot plug/unplug support for ABM (peer to peer).
 * Connection autorecover for Full duplex (both for ABM and NRM modes) and Light protocols (with enabled crc)
 * Error detection: Simple 8-bit checksum (sum of bytes), FCS16 (CCITT-16), FCS32 (CCITT-32)
 * Platform independent hdlc framing implementation (hdlc low level API: hdlc_ll_xxxx)
 * Easy to use Light protcol - analogue of a SLIP protcol (tiny_light_xxxx API, see examples)
 * Full-duplex protocol (tiny_fd_xxxx true RFC 1662 implementation, supports confirmation, frames retransmissions: ABM and NRM modes )
 * one to one and one to many modes
 * Frames of maximum 32K or 2G size (payload limit depends on platfrom).
 * Low SRAM consumption (starts at 60 bytes).
 * Low Flash consumption (starts at 1.2KiB, features can be disabled and enabled at compilation time)
 * No dynamic memory allocation!
 * Special serial loopback tool for debug purposes and performance testing (ABM mode only)

## Supported platforms

 * Any platform, where C/C++ compiler is available (C99, C++11)

| **Platform** | **Examples** |
| :-------- |:---------|
| ESP32 | [IDF](examples/esp32_idf) |
| Cortex M0 | [Zero](examples/arduino_zero_m0) |
| Linux | [Linux](examples/linux/loopback) |
| Windows | [Win32](examples/win32/loopback) |
| Other | Any platform with implemented HAL |

### What if my platform is not yet supported?

 That's not a problem. Just implement abstraction layer for your platform (timing and mutex functions). Please
go through the steps below:

 * add TINY_CUSTOM_PLATFORM define to your compilation flags.
 * Implement HAL functions and call `tiny_hal_init()` to pass your platform functions to the library
 * add CONFIG_TINYHAL_THREAD_SUPPORT define to your compilation flags if your platform supports standard c++ thread library

Refer to `tiny_hal_init()` function. To understand HAL implementation refer to
[Linux](https://github.com/lexus2k/tinyproto/blob/master/src/hal/linux/linux_hal.inl) and
[ESP32](https://github.com/lexus2k/tinyproto/blob/master/src/hal/esp32/esp32_hal.inl) examples in
 [HAL abstraction layer](https://github.com/lexus2k/tinyproto/tree/master/src/hal).
You may use template code [platform_hal.c](tools/hal_template_functions/platform_hal.c)

## Easy to use

### Cpp

Usage of light Tiny Protocol in C++ can look like this:
```.cpp
#include "tinyproto.h"

tinyproto::Light  proto;
tinyproto::Packet<256> packet;

void setup() {
    ...
    proto.beginToSerial();
}

void loop() {
    if (Serial.available()) {
        int len = proto.read( packet );
        if (len > 0) {
            /* Send message back */
            proto.write( packet );
        }
    }
}
```

Example of using full duplex Tiny Protocol in C++ is a little bit bigger, but it is still simple:
```.cpp
#include "tinyproto.h"

tinyproto::Fd<FD_MIN_BUF_SIZE(64,4)>  proto;

void onReceive(void *udata, tinyproto::IPacket &pkt) {
    // Process message here, you can do with the message, what you need
    // Let's send it back to the sender ;)
    if ( proto.write(pkt) == TINY_ERR_TIMEOUT ) {
        // Do what you need to do if looping back failed on timeout.
        // But never use blocking operations inside callback
    }
}

void setup() {
    ...
    // Here we say FD protocol object, which callback to call once new msg is received
    proto.setReceiveCallback( onReceive );
    proto.begin();
}

void loop() {
    if (Serial.available()) {
        uint8_t byte = Serial.read();
        proto.run_rx( &byte, 1 ); // run FD protocol parser to process data received from the channel
    }
    uint8_t byte;
    if ( proto.run_tx( &byte, 1 ) == 1 ) // FD protocol fills buffer with data, we need to send to the channel
    {
        while ( Serial.write( byte ) == 0 ); // Just send the data
    }
}
```

### Python

```.py
import tinyproto

p = tinyproto.Hdlc()
def on_read(a):
    print("Received bytes: " + ','.join( [ "{:#x}".format(x) for x in a ] ) )

# setup protocol
p.on_read = on_read
p.begin()

# provide rx bytes to the protocol, obtained from hardware rx channel
p.rx( bytearray([ 0x7E, 0xFF, 0x3F, 0xF3, 0x39, 0x7E  ]) )
```

## How to build

### Linux
```.txt
make
# === OR ===
mkdir build
cd build
cmake -DEXAMPLES=ON ..
make
```

### Windows
```.txt
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -DEXAMPLES=ON ..
```

### ESP32
Just place the library to your project components folder.

## Setting up

 * Arduino Option 1 (with docs and tools)
   * Download source from https://github.com/lexus2k/tinyproto
   * Put the downloaded library content to Arduino/libraries/tinyproto folder
   * Restart the Arduino IDE
   * You will find the examples in the Arduino IDE under File->Examples->tinyproto

 * Arduino Option 2 (only library without docs)
   * Go to Arduino Library manager
   * Find and install tinyproto library
   * Restart the Arduino IDE
   * You will find the examples in the Arduino IDE under File->Examples->tinyproto

 * ESP32 IDF
   * Download sources from https://github.com/lexus2k/tinyproto and put to components
     folder of your project
   * Run `make` for your project

 * Linux
   * Download sources from https://github.com/lexus2k/tinyproto
   * Run `make` command from tinyproto folder, and it will build library and tools for you

 * Plain AVR
   * Download sources from https://github.com/lexus2k/tinyproto
   * Install avr gcc compilers
   * Run `make ARCH=avr`

 * Python
   * Download sources from https://github.com/lexus2k/tinyproto
   * Run `python setup.py install`

## Using tiny_loopback tool

 * Connect your Arduino board to PC
 * Run your sketch or tinylight_loopback
 * Compile tiny_loopback tool
 * Run tiny_loopback tool: `./bld/tiny_loopback -p /dev/ttyUSB0 -t light -g -c 8 -a -r`

 * Connect your Arduino board to PC
 * Run your sketch or tinyfd_loopback
 * Compile tiny_loopback tool
 * Run tiny_loopback tool: `./bld/tiny_loopback -p /dev/ttyUSB0 -t fd -c 8 -w 3 -g -a -r`

For more information about this library, please, visit https://github.com/lexus2k/tinyproto.
Doxygen documentation can be found at [Codedocs xyz site](https://codedocs.xyz/lexus2k/tinyproto).
If you found any problem or have any idea, please, report to Issues section.
If you find the library useful and want to [support future development](https://www.paypal.me/lexus2k), you may contact me.

| Paypal | Bitcoin | Etherium |
| ------ | ------- | -------- |
| Not available  |  <center>![BTC](.travis/btc_segwit.png)<br/>[3CtUY6Ag2zsvm1JyqeeKeK8kjdG7Tnjr5W](bitcoin:3CtUY6Ag2zsvm1JyqeeKeK8kjdG7Tnjr5W)</center> | <center>![ETH](.travis/eth.png)<br/>[0x20608A71470Bc84a3232621819f578Fb9C02A460](etherium:0x20608A71470Bc84a3232621819f578Fb9C02A460)</center> |

## License

The project is released under dual license: GPLv3, or Commercial license.

Copyright 2016-2022 (C) Alexey Dynda

This file is part of Tiny Protocol Library.

GNU General Public License Usage

Protocol Library is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Protocol Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Protocol Library.  If not, see <http://www.gnu.org/licenses/>.

Commercial License Usage

Licensees holding valid commercial Tiny Protocol licenses may use this file in
accordance with the commercial license agreement provided in accordance with
the terms contained in a written agreement between you and Alexey Dynda.
For further information contact via email on github account.
