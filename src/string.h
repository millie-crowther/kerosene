#ifndef STRING_H
#define STRING_H

typedef struct string {
    const char * chars;
    uint32_t length;
} string;

bool string_equals(const string a, const string b);
bool string_starts_with(const string string, const string prefix);

#endif
