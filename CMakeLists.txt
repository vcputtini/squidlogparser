cmake_minimum_required(VERSION 3.14)
project(squidlogparser LANGUAGES CXX VERSION 1.0 DESCRIPTION "Squid-cache Log Parser")

#set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_definitions("-Wall -Wextra -pedantic")

# These macros make the 'parser' functions print the values of the retrieved
# fields to stdout.
#
# Important: Only activate these macros for testing. In production environment
# they must be disabled.

# Uncomment to activate.
#add_definitions("-DDEBUG_PARSER_SQUID")
#add_definitions("-DDEBUG_PARSER_COMMON")
#add_definitions("-DDEBUG_PARSER_COMBINED")
#add_definitions("-DDEBUG_PARSER_REFERRER")
#add_definitions("-DDEBUG_PARSER_USERAGENT")

file(WRITE "${CMAKE_SOURCE_DIR}/QtCreatorDeployment.txt" "<deployment/prefix>\n")

add_library(squidlogparser SHARED
	squidlogparser.h
	squidlogparser.cc
)

find_package(tinyxml2 REQUIRED)

set_target_properties(squidlogparser PROPERTIES VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER squidlogparser.h)

target_include_directories(squidlogparser PRIVATE .)
set_target_properties(squidlogparser PROPERTIES PUBLIC_HEADER squidlogparser.h)

target_link_libraries(squidlogparser PRIVATE -ltinyxml2 -lpthread)
target_compile_definitions(squidlogparser PRIVATE squidlogparser_LIBRARY)

# Uncomment to install (root)
#install(TARGETS squidlogparser DESTINATION /lib64)
#install(FILES squidlogparser.h DESTINATION /usr/include)
