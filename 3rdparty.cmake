set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(3RDPARTY
    3rdparty/http-parser/http_parser.c
    3rdparty/http-parser/http_parser.h
)

add_library(3rdparty
    STATIC
    ${3RDPARTY}
)
