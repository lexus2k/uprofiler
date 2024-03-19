/**
 * MIT License
 * 
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


