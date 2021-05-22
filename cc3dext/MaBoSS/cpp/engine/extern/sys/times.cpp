#include "times.h"

clock_t times(struct tms *_buffer) {
    _buffer->tms_utime = clock();
    _buffer->tms_stime = 0;
    _buffer->tms_cstime = 0;
    _buffer->tms_cutime = 0;
    return _buffer->tms_utime;
}