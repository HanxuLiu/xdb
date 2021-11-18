cmake_minimum_required (VERSION 3.0)

project (xdb)

add_compile_options(-std=c++14)

include_directories(deps/libelfin deps/linenoise include)

add_executable(xdb src/xdb.cpp deps/linenoise/linenoise.c)

add_custom_target(
   libelfin
   COMMAND make
   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/deps/libelfin
)

target_link_libraries(xdb
                      ${PROJECT_SOURCE_DIR}/deps/libelfin/dwarf/libdwarf++.so
                      ${PROJECT_SOURCE_DIR}/deps/libelfin/elf/libelf++.so)

add_dependencies(xdb libelfin)
