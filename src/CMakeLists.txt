cmake_minimum_required(VERSION 3.21)
project (simorgh C)

# add_compile_options(-Wall -Wextra -pedantic -Werror)


set(SOURCES
    main.c
    json.c
)

add_executable(simorgh ${SOURCES})
set_property(TARGET simorgh PROPERTY C_STANDARD 23)
