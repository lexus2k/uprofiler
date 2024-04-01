# uProfiler

<!-- [![Build Status](https://circleci.com/gh/lexus2k/uprofiler.svg?style=svg)](https://circleci.com/gh/lexus2k/uprofiler) -->
![Github actions](https://github.com/lexus2k/uprofiler/actions/workflows/main.yml/badge.svg)
[![Coverage Status](https://coveralls.io/repos/github/lexus2k/uprofiler/badge.svg?branch=main)](https://coveralls.io/github/lexus2k/uprofiler?branch=main)
[![Documentation](https://codedocs.xyz/lexus2k/uprofiler.svg)](https://codedocs.xyz/lexus2k/uprofiler/)
![License](https://img.shields.io/badge/license-GPLv3-blue)
![License](https://img.shields.io/badge/license-Commercial-blue)


[tocstart]: # (toc start)

  * [Introduction](#introduction)
  * [Key Features](#key-features)
  * [Easy to use](#easy-to-use)
  * [Setting up](#setting-up)
  * [How to buid](#how-to-build)
  * [License](#license)

[tocend]: # (toc end)

## Introduction

uProfiler is a lightweight and efficient library designed for profiling applications, with a focus on microcontrollers commonly found in production devices. The primary objective of uProfiler is to gather valuable statistics from a fleet of devices. The library offers support for both single-threaded and multi-threaded modes, making it adaptable to various application architectures. Notably, uProfiler is lock-free, enabling its usage in interrupt handlers.


## Key Features

Key features:

 * Lock-free design: uProfiler is built with a lock-free architecture, ensuring that it can operate seamlessly in multi-threaded and interrupt-handling scenarios. This design choice minimizes the impact on the overall system performance and eliminates the possibility of contention issues.

 * Small footprint: Designed with resource-constrained environments in mind, uProfiler has a small memory and code footprint. It minimizes the utilization of system resources, making it ideal for deployment on microcontrollers with limited storage and processing capabilities.


## Easy to use

### C

Usage of light uProfiler in C++ can look like this:
```.cpp
#include "uprofiler.h"

void my_function_to_profile(void)
{
    uprof_begin_tag(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    uprof_end_tag(1);
}

static uint32_t my_get_time()
{
    // for Arduino
    return millis();
    // For C++ applications 
    // return std::chrono::duration_cast<std::chrono::milliseconds>(
    //    std::chrono::steady_clock::now().time_since_epoch()).count();
}

...
    uprof_config_t config = 
    {
        .logger_output = NULL,
        .on_buffer_ready = NULL,
        .get_time = my_get_time
    };
    uint8_t buffer[uprof_calculate_size(10)];
    uprof_init(&config, buffer, sizeof(buffer));

    my_function_to_profile();
    my_function_to_profile();

    uprof_tag_stat_t stat;
    if ( uprof_get_stat(1, &stat) == 0)
    {
        // print the stats here
    }
```

## How to build

### Linux
```.txt
make
# === OR ===
mkdir build
cd build
cmake ..
make
```

### Windows
```.txt
mkdir build
cd build
cmake -G "Visual Studio 16 2019" ..
```

### ESP32
Just place the library to your project components folder.

## Setting up

 * Arduino Option 1 (with docs and tools)
   * Download source from https://github.com/lexus2k/uprofiler
   * Put the downloaded library content to Arduino/libraries/uprofiler folder
   * Restart the Arduino IDE

 * ESP32 IDF
   * Download sources from https://github.com/lexus2k/uprofiler and put to components
     folder of your project
   * Run `make` for your project

 * Linux
   * Download sources from https://github.com/lexus2k/uprofiler
   * Run `make` command from uprofiler folder, and it will build library and tools for you

 * Plain AVR
   * Download sources from https://github.com/lexus2k/uprofiler
   * Install avr gcc compilers
   * Run `make ARCH=avr`

For more information about this library, please, visit https://github.com/lexus2k/uprofiler.
Doxygen documentation can be found at [Codedocs xyz site](https://codedocs.xyz/lexus2k/uprofiler).
If you found any problem or have any idea, please, report to Issues section.
If you find the library useful and want to [support future development](https://www.paypal.me/lexus2k), you may contact me.

| Paypal | Bitcoin | Etherium |
| ------ | ------- | -------- |
| Not available  |  <center>![BTC](.travis/btc_segwit.png)<br/>[3CtUY6Ag2zsvm1JyqeeKeK8kjdG7Tnjr5W](bitcoin:3CtUY6Ag2zsvm1JyqeeKeK8kjdG7Tnjr5W)</center> | <center>![ETH](.travis/eth.png)<br/>[0x20608A71470Bc84a3232621819f578Fb9C02A460](etherium:0x20608A71470Bc84a3232621819f578Fb9C02A460)</center> |

## License

The project is released under dual license: GPLv3, or Commercial license.

Copyright 2024 (C) Alexey Dynda

This file is part of uProfiler Library.

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

Licensees holding valid commercial uProfiler licenses may use this file in
accordance with the commercial license agreement provided in accordance with
the terms contained in a written agreement between you and Alexey Dynda.
For further information contact via email on github account.
