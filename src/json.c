#include "json.h"

#include <regex.h>
#include <stdlib.h>
#include <string.h>

typedef enum json_token_type_t {
    // Tokens that correspond to JSON values
    JSON_TOKEN_TYPE_NULL = 0,
    JSON_TOKEN_TYPE_FALSE = 1,
    JSON_TOKEN_TYPE_TRUE = 2,
    JSON_TOKEN_TYPE_NUMBER = 3,
    JSON_TOKEN_TYPE_STRING = 4,
    JSON_TOKEN_TYPE_OPEN_BRACE = 5,
    JSON_TOKEN_TYPE_OPEN_BRACKET = 6,
    JSON_TOKEN_TYPE_VALUE_IDENTIFIER = 7,

    // Tokens that do not directly correspond to a JSON value
    JSON_TOKEN_TYPE_COLON = 8,
    JSON_TOKEN_TYPE_COMMA = 9,
    JSON_TOKEN_TYPE_CLOSE_BRACE = 10,
    JSON_TOKEN_TYPE_CLOSE_BRACKET = 11,
    JSON_TOKEN_TYPE_WHITESPACE = 12,
    JSON_TOKEN_TYPE_INVALID = 13
} json_token_type_t;

typedef struct json_token_t {
    json_token_type_t type;
    const char * string;
    uint32_t children;
    uint32_t scope;
} json_token_t;

typedef struct json_token_regex_t {
    json_token_type_t type;
    const char * regex_string;
    regex_t regex;
} json_token_regex_t;

json_token_regex_t regexes[JSON_TOKEN_TYPE_INVALID] = {
    { .type = JSON_TOKEN_TYPE_TRUE, .regex_string = "^true" },
    { .type = JSON_TOKEN_TYPE_INVALID }
};

json_token_t * check_scope(json_token_t * tokens, uint32_t scope, json_token_t * parent, json_token_t * end){
    for (; tokens < end; tokens++){
        tokens->scope = scope;
        
        if (parent != nullptr){
            parent->children++;
        }
        
        if (*tokens == JSON_TOKEN_TYPE_OPEN_BRACE || *tokens == JSON_TOKEN_TYPE_OPEN_BRACKET){
            tokens = check_scope(tokens + 1, scope + 1, tokens);
        } else if (*tokens == JSON_TOKEN_TYPE_CLOSE_BRACE || *tokens == JSON_TOKEN_TYPE_CLOSE_BRACKET){
            return tokens;
        }
    }

    return nullptr;
}

void count_tokens(json_token_t * tokens, uint32_t * value_count, uint32_t * key_pair_count, uint32_t * object_count){
    *value_count = 0;
    *key_pair_count = 0;
    *object_count = 0;

    for (uint32_t i = 0; tokens[i].type != JSON_TOKEN_TYPE_INVALID; i++){
        if (tokens[i].type < JSON_TOKEN_TYPE_VALUE_IDENTIFIER){
            (*value_count)++;
            
            if (tokens[i].type == JSON_TOKEN_TYPE_OPEN_BRACE){
                (*object_count)++;
            } 
        } else if (tokens[i].type == JSON_TOKEN_TYPE_COLON){
            (*key_pair_count)++;
        }
    }
}

bool json_compile_regular_expressions(){
    for (uint32_t i = 0; regexes[i].type != JSON_TOKEN_TYPE_INVALID; i++){
        if (regcomp(&regexes[i].regex, regexes[i].regex_string, REG_EXTENDED) != 0){
            return false;
        }
    }

    return true;
}

bool tokenize(const char * string, json_token_t * tokens){
    while (*string != '\0'){
        for (uint32_t i = 0;; i++){
            if (regexes[i].type == JSON_TOKEN_TYPE_INVALID){
                return false;
            }
            
            regmatch_t match;
            int result = regexec(&regexes[i].regex, string, 1, &match, 0);
            if (result == 0){
                if (regexes[i].type != JSON_TOKEN_TYPE_WHITESPACE){
                    *tokens = (json_token_t){
                        .type = regexes[i].type,
                        .string = string,
                    };
                    tokens++;
                }
                string += match.rm_eo;
                break;
            }
        }
    }

    *tokens = (json_token_t){ .type = JSON_TOKEN_TYPE_INVALID };
    return true;
}

uint64_t hash(const char * string) {
    // see: http://www.cs.yorku.ca/~oz/hash.html
    uint64_t hash = 5381;

    for (; *string != '\0'; string++){
        hash = ((hash << 5) + hash) + *string;
    }

    return hash;
}

json_value_t * json_object_get(const json_object_t * object, const char * string){
    uint64_t index = hash(string) % JSON_OBJECT_BUCKETS_COUNT;
    json_bucket_t bucket = object->buckets[index];
    for (uint32_t i = 0; i < bucket.length; i++){
        if (strcmp(string, bucket.key_pairs[i].key) == 0){
            return bucket.key_pairs[i].value;
        }
    }
    return nullptr;
}

json_value_t parse_json_value(json_token_t ** tokens, json_value_t ** values){
    json_token_t token = **token;
    (*tokens)++;

    uint32_t length = token.children;
    json_value_t * elements = *values;
    (*values) += length;

    if (token.type == JSON_TOKEN_TYPE_NULL){
        return (json_value_t){ .type = JSON_TYPE_NULL };
    } else if (token.type == JSON_TOKEN_TYPE_FALSE){
        return (json_value_t){ .type = JSON_TYPE_BOOLEAN, .boolean = false };
    } else if (token.type == JSON_TOKEN_TYPE_TRUE){
        return (json_value_t){ .type = JSON_TYPE_BOOLEAN, .boolean = true };
    } else if (token.type == JSON_TOKEN_TYPE_NUMBER){
        return (json_value_t){ .type = JSON_TYPE_NUMBER, .number = atof(token.string) };
    } else if (token.type == JSON_TOKEN_TYPE_STRING){
        // TODO
    } else if (token.type == JSON_TOKEN_TYPE_OPEN_BRACE){
        // TODO
    } else if (tokens->type == JSON_TOKEN_TYPE_OPEN_BRACKET){
        for (uint32_t i = 0; i < length; i++){
            elements[i] = parse_json_value(tokens, values);
            if (elements[i].type == JSON_TYPE_INVALID){
                return elements[i];
            }

            token = **token;
            (*tokens)++;
            if (
                (i < array_length - 1 && token.type != JSON_TOKEN_TYPE_COMMA) ||
                (i == array_length - 1 && token.type != JSON_TOKEN_TYPE_CLOSE_BRACKET) 
            ){
                return false;
            }
        }
        
        return (json_value_t){ 
            .type = JSON_TYPE_ARRAY, 
            .array = (json_array_t){
                .elements = elements,
                .length = length,
            },
        };
    } 

    return (json_value_t){ .type = JSON_TYPE_INVALID };

bool json_document_parse(const char * string, json_document_t * document){
    size_t string_length = strlen(string);
    json_token_t * tokens = malloc(string_length * sizeof(json_token_t));

    if (tokens == nullptr){
        return false;
    }

    bool result = tokenize(string, tokens);
    if (!result){
        free(tokens);
        return false;
    }

    json_token_t * scope_check_result = check_scope(tokens, 0, nullptr, tokens + string_length);
    if (scope_check_result == nullptr){
        free(tokens);
        return false;
    }

    uint32_t value_count;
    uint32_t key_pair_count;
    uint32_t object_count;
    count_tokens(tokens, &value_count, &key_pair_count, &object_count); 

    void * data_pointer = malloc(
        value_count * sizeof(json_value_t) +
        key_pair_count * sizeof(json_key_pair_t) +
        object_count * sizeof(json_object_t)
    );

    if (data_pointer == nullptr){
        free(tokens);
        return false;
    }

    json_value_t * values = data_pointer;
    json_key_pair_t * key_pairs = (json_key_pair_t *)(values + value_count);
    json_object_t * objects = (json_object_t *)(key_pairs + key_pair_count);

    json_value_t root = parse_json_value(tokens, values);
    free(tokens);

    if (root.type == JSON_TYPE_INVALID){
        free(data_pointer);
        return false;
    }
    
    *document = (json_document_t){
        .data_pointer = data_pointer,
        .root = root
    };

    return true;
}
