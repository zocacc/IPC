#include "json_output.h"
#include <stdio.h>
#include <time.h>

void print_json_status(const char* module, const char* status, const char* message, int pid) {
    time_t now;
    time(&now);
    if (pid > 0) {
        printf("{\"type\":\"status\",\"module\":\"%s\",\"status\":\"%s\",\"message\":\"%s\",\"pid\":%d,\"timestamp\":%ld}\n", 
               module, status, message, pid, now);
    } else {
        printf("{\"type\":\"status\",\"module\":\"%s\",\"status\":\"%s\",\"message\":\"%s\",\"timestamp\":%ld}\n", 
               module, status, message, now);
    }
    fflush(stdout);
}

void print_json_data(const char* module, const char* data, const char* source, int pid) {
    time_t now;
    time(&now);
    if (pid > 0) {
        printf("{\"type\":\"data\",\"module\":\"%s\",\"data\":\"%s\",\"source\":\"%s\",\"pid\":%d,\"timestamp\":%ld}\n", 
               module, data, source, pid, now);
    } else {
        printf("{\"type\":\"data\",\"module\":\"%s\",\"data\":\"%s\",\"source\":\"%s\",\"timestamp\":%ld}\n", 
               module, data, source, now);
    }
    fflush(stdout);
}

void print_json_error(const char* module, const char* error, int pid) {
    time_t now;
    time(&now);
    if (pid > 0) {
        printf("{\"type\":\"error\",\"module\":\"%s\",\"error\":\"%s\",\"pid\":%d,\"timestamp\":%ld}\n", 
               module, error, pid, now);
    } else {
        printf("{\"type\":\"error\",\"module\":\"%s\",\"error\":\"%s\",\"timestamp\":%ld}\n", 
               module, error, now);
    }
    fflush(stdout);
}
