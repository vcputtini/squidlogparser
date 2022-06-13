cmake_minimum_required(VERSION 3.14)
project(squidlogparser LANGUAGES CXX VERSION 1.0 DESCRIPTION "Squid-cache Log Parser")

#set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_definitions("-Wall -Wextra -pedantic")

# -----------------------------------------------------------------------------
# Enable this extension allows the parsed data to be recorded
# directly on tables in MariaDB(tm)
#
# Note: See README_SLPDatabase.md for more details.
#
# Note: MySQL Connector C++ must be compiled and installed.
# https://mariadb.com/docs/connect/programming-languages/
# https://github.com/mariadb-corporation/mariadb-connector-cpp
#
# Enabled: ON
# Disabled: OFF (DEFAULT)
option(DATABASE_EXTENSION "Database Extension" OFF)
# -----------------------------------------------------------------------------

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

if(DATABASE_EXTENSION)
    add_definitions("-DDATABASE_EXTENSION")
    target_link_libraries(squidlogparser PRIVATE -ltinyxml2 -lboost_regex -lmariadbcpp -lpthread)
else()
    target_link_libraries(squidlogparser PRIVATE -ltinyxml2 -lboost_regex -lpthread)
endif(DATABASE_EXTENSION)
unset(DATABASE_EXTENSION CACHE)

target_compile_definitions(squidlogparser PRIVATE squidlogparser_LIBRARY)

# Uncomment to install (root)
#install(TARGETS squidlogparser DESTINATION /lib64)
#install(FILES squidlogparser.h DESTINATION /usr/include)
