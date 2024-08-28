cmake_minimum_required(VERSION 3.21)
project (kerosene C)

# add_compile_options(-Wall -Wextra -pedantic -Werror)


set(SOURCES
    main.c
    json.c
)

add_executable(kerosene ${SOURCES})
set_property(TARGET kerosene PROPERTY C_STANDARD 23)
