#include "platform/clock.h"
#include <esp_timer.h>

uint64_t current_time() {return (esp_timer_get_time()/1000);}