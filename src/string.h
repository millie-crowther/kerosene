#ifndef STRING_H
#define STRING_H

#include <stdint.h>

typedef struct string {
    const char * chars;
    uint32_t length;
} string;

string str(const char * literal);

bool string_equals(const string a, const string b);
bool string_starts_with(const string s, const string prefix);
bool string_contains_character(const string s, const char character);
string string_tail(const string s);
char string_head(const string s);
bool string_is_empty(const string s);

#endif
