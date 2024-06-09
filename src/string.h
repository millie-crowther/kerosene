#ifndef STRING_H
#define STRING_H

typedef struct string {
    const char * chars;
    uint32_t length;
} string;

bool string_equals(const string a, const string b);
bool string_starts_with(const string string, const string prefix);
bool string_contains_character(const string string, const char character);
string string_tail(const string string);
char string_head(const string string);

#endif
