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
    for (; tokens != nullptr && tokens < end; tokens++){
        tokens->scope = scope;
        
        if (parent != nullptr && tokens->type < JSON_TOKEN_TYPE_VALUE_IDENTIFIER){
            parent->children++;
        }
        
        if (tokens->type == JSON_TOKEN_TYPE_OPEN_BRACE || tokens->type == JSON_TOKEN_TYPE_OPEN_BRACKET){
            tokens = check_scope(tokens + 1, scope + 1, tokens, end);
        } else if (tokens->type == JSON_TOKEN_TYPE_CLOSE_BRACE || tokens->type == JSON_TOKEN_TYPE_CLOSE_BRACKET){
            return tokens;
        }
    }

    return nullptr;
}

bool json_compile_regular_expressions(){
    for (uint32_t i = 0; regexes[i].type != JSON_TOKEN_TYPE_INVALID; i++){
        if (regcomp(&regexes[i].regex, regexes[i].regex_string, REG_EXTENDED) != 0){
            return false;
        }
    }

    return true;
}

int tokenize(const char * string, json_token_t * tokens){
    int token_count = 0;
    while (*string != '\0'){
        for (uint32_t i = 0;; i++){
            if (regexes[i].type == JSON_TOKEN_TYPE_INVALID){
                return -1;
            }
            
            regmatch_t match;
            int result = regexec(&regexes[i].regex, string, 1, &match, 0);
            if (result == 0){
                if (regexes[i].type != JSON_TOKEN_TYPE_WHITESPACE){
                    tokens[token_count] = (json_token_t){
                        .type = regexes[i].type,
                        .string = string,
                    };
                    token_count++;
                }
                string += match.rm_eo;
                break;
            }
        }
    }

    return token_count;
}

uint64_t hash(const json_value_t * object, const char * string) {
    // see: http://www.cs.yorku.ca/~oz/hash.html
    uint64_t hash = 5381;

    for (; *string != '\0'; string++){
        hash = ((hash << 5) + hash) + *string;
    }

    return hash ^ (uint64_t)(object);
}

json_value_t * json_object_get(const json_value_t * object, const char * string){
    json_hash_map_t * map = object->hash_map;
    json_key_pair_t * start = map->buckets[hash(object, string) % map->length];
    
    for (json_key_pair_t * key_pair = start; key_pair != nullptr; key_pair = key_pair->next){
        if (key_pair->object == object && strcmp(string, key_pair->key) == 0){
            return key_pair->value;
        }
    }
    
    return nullptr;
}

json_value_t * parse_json_value(json_token_t ** tokens, json_value_t ** values){
    json_token_t token = **tokens;
    (*tokens)++;

    uint32_t length = token.children;
    if (token.type == JSON_TOKEN_TYPE_OPEN_BRACE && length % 2 != 0){
        return nullptr;
    }
    json_value_t * elements = *values;
    (*values) += length;

    json_value_t result;
    
    if (token.type == JSON_TOKEN_TYPE_NULL){
        result = (json_value_t){ .type = JSON_TYPE_NULL };
    } else if (token.type == JSON_TOKEN_TYPE_FALSE || token.type == JSON_TOKEN_TYPE_TRUE){
        result = (json_value_t){ 
            .type = JSON_TYPE_BOOLEAN,
            .boolean = token.type == JSON_TOKEN_TYPE_TRUE,
        };
    } else if (token.type == JSON_TOKEN_TYPE_NUMBER){
        result = (json_value_t){ .type = JSON_TYPE_NUMBER, .number = atof(token.string) };
    } else if (token.type == JSON_TOKEN_TYPE_STRING){
        // TODO
    } else if (token.type == JSON_TOKEN_TYPE_OPEN_BRACE){
        // TODO
    } else if (token.type == JSON_TOKEN_TYPE_OPEN_BRACKET){
        for (uint32_t i = 0; i < length; i++){
            json_value_t * element = parse_json_value(tokens, values);
            if (element == nullptr){
                return nullptr;
            }
            elements[i] = *element;

            token = **tokens;
            (*tokens)++;
            if (
                (i < length - 1 && token.type != JSON_TOKEN_TYPE_COMMA) ||
                (i == length - 1 && token.type != JSON_TOKEN_TYPE_CLOSE_BRACKET) 
            ){
                return nullptr;
            }
        }
        
        result = (json_value_t){ 
            .type = JSON_TYPE_ARRAY, 
            .array = (json_array_t){
                .elements = elements,
                .length = length,
            },
        };
    } else {
        return nullptr;
    }

    **values = result;
    return *values;
}

bool json_document_parse(const char * string, json_document_t * document){
    size_t string_length = strlen(string);
    json_token_t * tokens = malloc(string_length * sizeof(json_token_t));

    if (tokens == nullptr){
        return false;
    }

    int token_count = tokenize(string, tokens);
    if (token_count < 0){
        free(tokens);
        return false;
    }

    json_token_t * scope_check_result = check_scope(tokens, 0, nullptr, tokens + string_length);
    if (scope_check_result == nullptr){
        free(tokens);
        return false;
    }

    uint32_t value_count = token_count;
    uint32_t object_count = token_count / 2;
    void * data_pointer = malloc(
        value_count * sizeof(json_value_t) + object_count * sizeof(json_object_t)
    );

    if (data_pointer == nullptr){
        free(tokens);
        return false;
    }

    json_value_t * values = data_pointer;
    json_object_t * objects = (json_object_t *)(values + value_count);

    json_token_t * token_pointer = tokens;
    json_value_t * root = parse_json_value(&token_pointer, &values);
    free(tokens);

    if (root == nullptr){
        free(data_pointer);
        return false;
    }
    
    *document = (json_document_t){
        .data_pointer = data_pointer,
        .root = root
    };

    return true;
}
