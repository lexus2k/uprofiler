/*
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
    along with uProfiler Library.  If not, see <http://www.gnu.org/licenses/>.

    Commercial License Usage

    Licensees holding valid commercial uProfiler licenses may use this file in
    accordance with the commercial license agreement provided in accordance with
    the terms contained in a written agreement between you and Alexey Dynda.
    For further information contact via email on github account.
*/

#include <CppUTest/TestHarness.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <chrono>
#include <thread>
#include "uprofiler.h"

static uint32_t getTime()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

TEST_GROUP(BASE){
    void setup(){
        // ...
    }

    void teardown(){
        // ...
    }
};


TEST(BASE, single_call_check)
{
    uprof_config_t config = 
    {
        .logger_output = NULL,
        .on_buffer_ready = NULL,
        .get_time = getTime
    };
    uint8_t buffer[uprof_calculate_size(10)];
    uprof_init(&config, buffer, sizeof(buffer));
    uprof_begin_tag(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    uprof_end_tag(1);
    uprof_tag_stat_t stat;
    CHECK_EQUAL(0, uprof_get_stat(1, &stat));
    CHECK_COMPARE(stat.total_time, >=, 100);
    CHECK_COMPARE(stat.total_time, <=, 110);
    CHECK_EQUAL(1, stat.count);
}

TEST(BASE, many_calls_check)
{
    uprof_config_t config = 
    {
        .logger_output = NULL,
        .on_buffer_ready = NULL,
        .get_time = getTime
    };
    uint8_t buffer[uprof_calculate_size(10)];
    uprof_init(&config, buffer, sizeof(buffer));
    for (int i = 0; i < 10; i++)
    {
        uprof_begin_tag(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        uprof_end_tag(1);
    }
    uprof_tag_stat_t stat;
    CHECK_EQUAL(0, uprof_get_stat(1, &stat));
    CHECK_COMPARE(stat.total_time, >=, 100);
    CHECK_COMPARE(stat.total_time, <=, 112);
    CHECK_EQUAL(10, stat.count);   
}

TEST(BASE, single_thread_not_enough_slots)
{
    uprof_config_t config = 
    {
        .logger_output = NULL,
        .on_buffer_ready = NULL,
        .get_time = getTime,
        .max_concurrent_calls = 0
    };
    uint8_t buffer[uprof_calculate_size(1)];
    uprof_init(&config, buffer, sizeof(buffer));
    uprof_begin_tag(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    uprof_end_tag(1);
    uprof_begin_tag(2);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    uprof_end_tag(2);
    uprof_tag_stat_t stat;
    CHECK_EQUAL(0, uprof_get_stat(1, &stat));
    CHECK_COMPARE(stat.total_time, >=, 50);
    CHECK_COMPARE(stat.total_time, <=, 80);
    CHECK_EQUAL(1, stat.count);
    CHECK_EQUAL(-1, uprof_get_stat(2, &stat));
}

TEST(BASE, many_tags_check)
{
    uprof_config_t config = 
    {
        .logger_output = NULL,
        .on_buffer_ready = NULL,
        .get_time = getTime
    };
    uint8_t buffer[uprof_calculate_size(20)];
    uprof_init(&config, buffer, sizeof(buffer));
    for (int i = 0; i < 20; i++)
    {
        uprof_begin_tag(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(10+i));
        uprof_end_tag(i);
    }
    for (int i = 0; i < 20; i++)
    {
        uprof_tag_stat_t stat;
        CHECK_EQUAL(0, uprof_get_stat(i, &stat));
        CHECK_COMPARE(stat.total_time, >=, 10 + (uint32_t)i);
        CHECK_COMPARE(stat.total_time, <=, 16 + (uint32_t)i);
        CHECK_EQUAL(1, stat.count);
    }
}

TEST(BASE, muitl_thread_test)
{
    uprof_config_t config = 
    {
        .logger_output = NULL,
        .on_buffer_ready = NULL,
        .get_time = getTime,
        .max_concurrent_calls = 2,
    };
    uint8_t buffer[uprof_calculate_size(24)];
    uprof_init(&config, buffer, sizeof(buffer));
    std::thread t1([](){
        for (int i = 0; i < 20; i++)
        {
            auto id = uprof_begin_multi_tag(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(10+i));
            uprof_end_multi_tag(id);
        }
    });
    std::thread t2([](){
        for (int i = 0; i < 20; i++)
        {
            auto id = uprof_begin_multi_tag(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(10+i));
            uprof_end_multi_tag(id);
        }
    });
    t1.join();
    t2.join();
    for (int i = 0; i < 20; i++)
    {
        uprof_tag_stat_t stat;
        CHECK_EQUAL(0, uprof_get_stat(i, &stat));
        CHECK_COMPARE(20 + (uint32_t)i * 2, <= , stat.total_time);
        CHECK_COMPARE(30 + (uint32_t)i * 2, >= , stat.total_time);
        CHECK_EQUAL(2, stat.count);
    }
}

#if 0
#define UPROF_TAG_ID   __func__ ## tag_id

#define UPROF_FUNC_ID  &UPROF_TAG_ID

TEST(BASE, macro_test)
{
    uprof_config_t config = 
    {
        .max_concurrent_calls = 1,
        .logger_output = NULL,
        .on_buffer_ready = NULL,
        .get_time = getTime
    };
    uint8_t buffer[uprof_calculate_size(3)];
    uprof_init(&config, buffer, sizeof(buffer));
    uint16_t UPROF_TAG_ID = uprof_begin_multi_tag(UPROF_FUNC_ID);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    uprof_end_multi_tag(UPROF_TAG_ID);
}
#endif
