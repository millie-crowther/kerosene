#include "string.h"

#include <string.h>

bool string_equals(const string a, const string b){
    return a.length == b.length && strncmp(a.chars, b.chars, a.length);
}
