#ifndef JSON_OUTPUT_H
#define JSON_OUTPUT_H

void print_json_status(const char* module, const char* status, const char* message);
void print_json_data(const char* module, const char* data, const char* source);
void print_json_error(const char* module, const char* error);

#endif