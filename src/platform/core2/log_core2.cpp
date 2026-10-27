#include <M5Unified.h>
#include "platform/log.h"
#include <cstdarg>          //gives "va_list" (the "..." tools)
#include <cstdio>           //gives "vsnprintf"

const int LOG_MAX_LENGTH = 256; //cuts off too long messages

void show_debug_message(const char* fmt, ...){
    char buf[LOG_MAX_LENGTH];    //character buffer, an empty space to build the finished text in
    va_list args;              // a handle on the "..." values
    va_start(args, fmt);        //start reading the values passed
    vsnprintf(buf, sizeof(buf), fmt, args); //fill in the %d type variables and write the result into buffer
    va_end(args);       //done reading, needs to exist with va_start
    Serial.println(buf);    //print buffer contents to console. this line is core2 specific, others aren't.
}