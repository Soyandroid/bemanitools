#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "util/log.h"
#include "util/time.h"

static const uint8_t _MAX_LEN_16SEG = 9;
static const uint32_t _SEQ_CYCLE_TIME_MS = 500;

static bool _enabled;

static uint64_t _time_counter;
static size_t _text_pos;

static char _text[1024 + 1];
static size_t _text_len;

static void _create_display_string_with_wrap_around(char* out_16seg)
{
    size_t cur_text_pos = _text_pos;

    for (uint8_t i = 0; i < _MAX_LEN_16SEG; i++) {
        if (cur_text_pos >= _text_len) {
            cur_text_pos = 0;
        }
        
        out_16seg[i] = _text[cur_text_pos];
        cur_text_pos++;
    }
}

void vigem_iidxio_cab_16seg_sequencer_init(const char* text)
{
    log_assert(text);

    _text_len = strlen(text);

    if (_text_len + 1 > sizeof(_text)) {
        log_warning("Truncating input text as it exceeds the max size");
        strncpy(_text, text, sizeof(_text) - 1);
    } else {
        strcpy(_text, text);
    }

    _time_counter = time_get_counter();
    _enabled = true;

    log_info("Initialized");
}

void vigem_iidxio_cab_16seg_sequencer_update(char* out_16seg)
{
    log_assert(out_16seg);

    if (!_enabled) {
        memset(out_16seg, ' ', _MAX_LEN_16SEG);
        return;
    }

    uint64_t counter_now = time_get_counter();
    uint32_t cycle_time_elapsed_ms = time_get_elapsed_ms(counter_now - _time_counter);

    if (cycle_time_elapsed_ms >= _SEQ_CYCLE_TIME_MS) {
        _time_counter = counter_now;
        _text_pos++;
    
        if (_text_pos >= _text_len) {
            _text_pos = 0;
        }
    }

    _create_display_string_with_wrap_around(out_16seg);
}