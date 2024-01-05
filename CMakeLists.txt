cmake_minimum_required(VERSION 3.21)
project(lab3 VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -Wmissing-prototypes -Wstrict-prototypes -fsanitize-address-use-after-scope -g")
set(PARSER_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/parser")
set(PARSER_AST "${CMAKE_CURRENT_LIST_DIR}/parser/ast")

file(GLOB_RECURSE PARSER_SOURCES ${PARSER_SRC_DIR}/*.c ${PARSER_SRC_DIR}/*.h)
file(GLOB_RECURSE DATABASE_SOURCES ${CMAKE_CURRENT_LIST_DIR}/database/src/*.c ${CMAKE_CURRENT_LIST_DIR}/database/include/*.h)

find_program(FLEX_EXECUTABLE flex)
find_program(BISON_EXECUTABLE bison)

find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})

set(LEXER_OUT "${PARSER_AST}/tokens.c")
set(PARSER_OUT "${PARSER_AST}/grammar.c" database/src/xml/from_xml_server.c)

execute_process(
        COMMAND ${FLEX_EXECUTABLE} -o ${LEXER_OUT} ${PARSER_AST}/tokens.l
        WORKING_DIRECTORY ${PARSER_AST}
)
execute_process(
        COMMAND ${BISON_EXECUTABLE} --graph --report=itemset -d -o ${PARSER_OUT} ${PARSER_AST}/grammar.y
        WORKING_DIRECTORY ${PARSER_AST}
)

add_executable(client "parser/xml_client.c" ${LEXER_OUT} ${PARSER_OUT} ${PARSER_SOURCES})
target_link_libraries(client PRIVATE ${LIBXML2_LIBRARIES})
target_include_directories(client PRIVATE ${INCLUDE_DIR})


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -Wmissing-prototypes -Wstrict-prototypes -fsanitize-address-use-after-scope -g")

include_directories(${LIBXML2_INCLUDE_DIR})

add_executable(server "database/src/xml_server.c" ${DATABASE_SOURCES})

target_link_libraries(server PRIVATE ${LIBXML2_LIBRARIES})
target_include_directories(server PRIVATE ${INCLUDE_DIR})
