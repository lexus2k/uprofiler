/*
    Copyright 2024 (C) Alexey Dynda

    This file is part of uProfiler.

    GNU General Public License Usage

    Protocol Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Protocol Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Protocol Library.  If not, see <http://www.gnu.org/licenses/>.

    Commercial License Usage

    Licensees holding valid commercial uProfiler licenses may use this file in
    accordance with the commercial license agreement provided in accordance with
    the terms contained in a written agreement between you and Alexey Dynda.
    For further information contact via email on github account.
*/

#include "stddef.h"
#include "uprofiler.h"
#include "stdint.h"
#include "string.h"

#if defined(__AVR__)
#include <util/atomic.h>
#include <Arduino.h>
#endif

#define NOT_FOUND 0xFFFF
#define EMPTY_SLOT 0

typedef struct {
    uint16_t id;
    uint32_t start_timestamp;
    uint32_t total_duration;
    union
    {
        uint32_t count;
        uint32_t real_id;
    };
} uprof_tag_t;

typedef struct {
    uprof_config_t config;
    uprof_tag_t *tags;
    uint16_t tag_count;
    uint16_t max_count;
} uprof_t;

static uprof_t s_data = {{0, 0, 0}, NULL, 0};

#if defined(__AVR__)
// && defined(ATOMIC_BLOCK)

uint8_t static inline __avr_atomic_compare_exchange(uint16_t *p, uint16_t *c, uint16_t n)
{
    uint8_t f = 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if (*c == *p)
        {
            *c = *p;
            *p = n;
            f = 1;
        }
    }
    return f;
}

uint16_t static inline __avr_atomic_load(const uint16_t *p)
{
    uint16_t v = 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        v = *p;
    }
    return v;
}

void static inline __avr_atomic_store(uint16_t *p, uint16_t v)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        *p = v;
    }
}

uint32_t static inline __avr_atomic_fetch_add(uint32_t *p, uint32_t v)
{
    uint32_t n;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        n = *p;
        *p += v;
    }
    return n;
}

#define uprof_atomic_compare_exchange_n(p, c, n) __avr_atomic_compare_exchange(p, c, n)
#define uprof_atomic_load_n(p) __avr_atomic_load(p)
#define uprof_atomic_store_n(p,v) __avr_atomic_store(p,v)
#define uprof_atomic_fetch_add(p,v) __avr_atomic_fetch_add(p,v)

#else

#define uprof_atomic_compare_exchange_n(p, c, n)  __atomic_compare_exchange_n(p, c, n, 0, __ATOMIC_RELAXED, __ATOMIC_RELAXED)
#define uprof_atomic_load_n(p) __atomic_load_n(p, __ATOMIC_RELAXED)
#define uprof_atomic_store_n(p,v) __atomic_store_n(p, v, __ATOMIC_RELAXED)
#define uprof_atomic_fetch_add(p,v) __atomic_fetch_add(p, v, __ATOMIC_RELAXED)

#endif


static uint16_t __uprof_find_slot_and_acquire(uint16_t id)
{
    uint16_t start_index = id % s_data.tag_count;
    uint16_t index = start_index;
    do {
        uint16_t s_empty = EMPTY_SLOT;
        uint8_t success = uprof_atomic_compare_exchange_n(&s_data.tags[index].id, &s_empty, id + 1);
        if (success) {
            // new slot is found, counter must be zero at this point
            return index;
        } else if ( uprof_atomic_load_n(&s_data.tags[index].id) == id + 1) {
            return index;
        }
        index = (index + 1) % s_data.tag_count;
    } while (index != start_index);
    return NOT_FOUND;
}

static uint16_t __uprof_find_slot(uint16_t id) {
    uint16_t start_index = id % s_data.tag_count;
    uint16_t index = start_index;
    do {
        if (uprof_atomic_load_n(&s_data.tags[index].id) == id + 1) {
            return index;
        }
        index = (index + 1) % s_data.tag_count;
    } while (index != start_index);
    return NOT_FOUND;
}

void uprof_init(const uprof_config_t *config, void *buffer, int size)
{
    s_data.config = *config;
    s_data.tags = (uprof_tag_t *)buffer;
    s_data.max_count = size / sizeof(uprof_tag_t);
    s_data.tag_count = s_data.max_count - s_data.config.max_concurrent_calls;
    memset(s_data.tags, 0, size);
}

int uprof_calculate_size(uint16_t num_tags)
{
    return (int)num_tags * sizeof(uprof_tag_t);
}

//
// Single threaded implementation
//

void uprof_begin_tag(uint16_t id)
{
    uint16_t index = __uprof_find_slot_and_acquire(id);
    if (index == NOT_FOUND) {
        return;
    }
    s_data.tags[index].start_timestamp = s_data.config.get_time();
    return;
}

void uprof_end_tag(uint16_t id)
{
    uint16_t index = __uprof_find_slot(id);
    if (index == NOT_FOUND) {
        return;
    }
    uint32_t end_timestamp = s_data.config.get_time();
    s_data.tags[index].total_duration += end_timestamp - s_data.tags[index].start_timestamp;
    s_data.tags[index].count++;
}

//
// multi-threaded implementation
//

static uint16_t __uprof_find_slot_and_acquire_multi(uint16_t id)
{
    if (s_data.max_count == s_data.tag_count) {
        return NOT_FOUND;
    }
    uint16_t start_index = s_data.tag_count + (id % (s_data.max_count - s_data.tag_count));
    uint16_t index = start_index;
    do {
        uint16_t s_empty = EMPTY_SLOT;
        // Find any empty slot
        uint8_t success = uprof_atomic_compare_exchange_n(&s_data.tags[index].id, &s_empty, NOT_FOUND);
        if (success) {
            s_data.tags[index].real_id = id;
            return index;
        }
        index++;
        if (index >= s_data.max_count) {
            index = s_data.tag_count;
        };
    } while (index != start_index);
    return NOT_FOUND;
}


uint16_t uprof_begin_multi_tag(uint16_t id)
{
    uint16_t index = __uprof_find_slot_and_acquire_multi(id);
    if (index == NOT_FOUND) {
        return NOT_FOUND;
    }
    s_data.tags[index].start_timestamp = s_data.config.get_time();
    return index;
}

void uprof_end_multi_tag(uint16_t id)
{
    if (id == NOT_FOUND || id >= s_data.max_count) {
        return;
    }
    uint16_t real_id = s_data.tags[id].real_id;
    uint16_t stat_slot = __uprof_find_slot_and_acquire(real_id);
    if (stat_slot != NOT_FOUND) {
        uprof_atomic_fetch_add(&s_data.tags[stat_slot].total_duration, s_data.config.get_time() - s_data.tags[id].start_timestamp);
        uprof_atomic_fetch_add(&s_data.tags[stat_slot].count, 1);
    }

    memset(&s_data.tags[id].start_timestamp, 0, sizeof(uprof_tag_t) - offsetof(uprof_tag_t, start_timestamp));
    uprof_atomic_store_n(&s_data.tags[id].id, EMPTY_SLOT);
}

int uprof_get_stat(uint16_t id, uprof_tag_stat_t *stat)
{
    uint16_t index = __uprof_find_slot(id);
    if (index == NOT_FOUND) {
        return -1;
    }
    stat->total_time = s_data.tags[index].total_duration;
    stat->count = s_data.tags[index].count;
    return 0;
}

