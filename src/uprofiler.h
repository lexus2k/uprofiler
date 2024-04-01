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
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * The configuration of the profiler.
 *
 * @param logger_output The function to output the log.
 * @param on_buffer_ready The function to be called when the buffer is ready.
 * @param get_time The function to get the current time.
 */
typedef struct {
    uint16_t max_concurrent_calls;
    void (*logger_output)(const char *msg);
    void (*on_buffer_ready)(void *buffer, int size);
    uint32_t (*get_time)(void);
} uprof_config_t;

/**
 * The statistics of a tag.
 * 
 * @param id The tag id.
 * @param count The number of times the tag is called.
 * @param total_time The total time of the tag. 
 */
typedef struct {
    uint16_t id;
    uint32_t count;
    uint32_t total_time;
} uprof_tag_stat_t;

/**
 * Initialize the profiler.
 * 
 * @param config The configuration of the profiler.
 * @param buffer The buffer to store the statistics.
 * @param size The size of the buffer.
 * 
 * @return void
 */
void uprof_init(uprof_config_t *config, void *buffer, int size);

/**
 * Calculate the size of the required buffer.
 * 
 * @param num_tags The number of tags.
 * 
 * @return The size of the required buffer.
 */
int uprof_calculate_size(uint16_t num_tags);

/**
 * Collects statistics before function execution
 * 
 * @param id the tag id
 * 
 * @return void
 */
void uprof_begin_tag(uint16_t id);

/**
 * Collects statistics after function execution
 * 
 * @param id the tag id
 * 
 * @return void
 */
void uprof_end_tag(uint16_t id);

/**
 * Collects statistics before function execution
 * 
 * @param id the tag id
 * 
 * @return id to use with uprof_end_multi_tag
 */
uint16_t uprof_begin_multi_tag(uint16_t id);

/**
 * Collects statistics after function execution
 * 
 * @param id the tag id
 * 
 * @return void
 */
void uprof_end_multi_tag(uint16_t id);

/**
 * Get the statistics of a tag.
 * 
 * @param id The tag id.
 * @param stat The statistics of the tag.
 * 
 * @return 0 if the tag exists, -1 if the tag does not exist.
 */
int uprof_get_stat(uint16_t id, uprof_tag_stat_t *stat);

void uprof_flush(void);

#ifdef __cplusplus
}
#endif


