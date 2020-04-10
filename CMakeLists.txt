cmake_minimum_required(VERSION 3.6.0)

#----------------------------------------------------------------------
function(glob_src VARNAME src_path)
    file(GLOB_RECURSE _glob_src
            ${src_path}/*.cpp
            ${src_path}/*.cc
            ${src_path}/*.c
            ${src_path}/*.s
            ${src_path}/*.S
            ${src_path}/*.h
            ${src_path}/*.hpp
        )

    set(${VARNAME} ${_glob_src} PARENT_SCOPE)
endfunction(glob_src)

macro(reject_file_pattern pattern input_list)
  foreach(src_file ${${input_list}})
    get_filename_component(base_name ${src_file} NAME)
    if(${base_name} MATCHES ${pattern})
      list(REMOVE_ITEM ${input_list} "${src_file}")
    endif()
  endforeach()
endmacro()
#----------------------------------------------------------------------

project(stm32flash C)

set(SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dev_table.c
    ${CMAKE_CURRENT_SOURCE_DIR}/i2c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/init.h
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port.h
    ${CMAKE_CURRENT_SOURCE_DIR}/serial.h
    ${CMAKE_CURRENT_SOURCE_DIR}/serial_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/serial_platform.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.h

    ${CMAKE_CURRENT_SOURCE_DIR}/parsers/binary.c
    ${CMAKE_CURRENT_SOURCE_DIR}/parsers/binary.h
    ${CMAKE_CURRENT_SOURCE_DIR}/parsers/hex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/parsers/hex.h
    ${CMAKE_CURRENT_SOURCE_DIR}/parsers/parser.h

    )

include(CheckIncludeFile)
CHECK_INCLUDE_FILE(unistd.h STM32FLASH_HAVE_UNISTD_H)
if (STM32FLASH_HAVE_UNISTD_H)
    add_definitions(-DSTM32FLASH_HAVE_UNISTD_H)
else()
    list(APPEND SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/unistd_win32/unistd.h)
    list(APPEND SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/unistd_win32/unistd.c)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/unistd_win32)
endif()

include(CheckCSourceCompiles)
check_c_source_compiles("
    #include <getopt.h>
    int main(int argc, char* argv[]) {
      int c;
      while ((c = getopt(argc, argv, \"a:b\")) != -1){}
      return 0;
    }
  "
  HAVE_GETOPT
)

check_c_source_compiles("
    #include <stdint.h>
    #include <sys/types.h>
    int main(void) {
      ssize_t c;
      (void)c;
      return 0;
    }
  "
  HAVE_SSIZE_T
)

if( NOT HAVE_GETOPT)
    list(APPEND SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/unistd_win32/getopt.h)
    list(APPEND SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/unistd_win32/getopt.c)
endif()

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        $<$<BOOL:${HAVE_GETOPT}>:STM32FLASH_HAVE_GETOPT>
        $<$<BOOL:${HAVE_SSIZE_T}>:STM32FLASH_HAVE_SSIZE_T>
        $<$<C_COMPILER_ID:MSVC>:__WIN32__>
)


#======================= clang-format ==========================================
set(FORMAT_OBJECTS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/parsers
    ${CMAKE_CURRENT_SOURCE_DIR}/unistd_win32
)

if (NOT DEFINED CLANG_FORMAT)
    find_program(CLANG_FORMAT "clang-format"
                     HINTS
                     "d:/projects/qt/libclang_80/libclang/bin"
                     "d:/bin/msys64/mingw64/bin"
                 )
endif()

if(CLANG_FORMAT)
    execute_process(COMMAND ${CLANG_FORMAT} --version OUTPUT_VARIABLE _clang_format_out)

    string(REGEX REPLACE "\n$" "" _clang_format_out "${_clang_format_out}")

    STRING(REGEX MATCH "([0-9]*)\\.([0-9]*)\\.([0-9]*)" FMT_VERSION "${_clang_format_out}" )
    STRING(REGEX REPLACE "([0-9]*)\\.([0-9]*)\\.(.*)" "\\1" FMT_VERSION_MAJOR "${FMT_VERSION}")

    if(NOT ${FMT_VERSION_MAJOR} EQUAL "8" )
        set(CLANG_FORMAT "")
        message(STATUS "Found clang-format ...${FMT_VERSION_MAJOR}...")
        message(WARNING "Wrong version of clang-format (${FMT_VERSION}). Expected ver. 8.x.x")
    endif()

endif()

if(CLANG_FORMAT)
    message("clang-format: ${CLANG_FORMAT}")
    foreach(_obj ${FORMAT_OBJECTS})
        if(EXISTS ${_obj})
            if(IS_DIRECTORY ${_obj})
                glob_src(_src_list  ${_obj})
                reject_file_pattern( \\\\.S _src_list)
                reject_file_pattern( \\\\.s _src_list)
                add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
                                   COMMAND ${CLANG_FORMAT} -i -style=file -fallback-style=none ${_src_list})
            else ()
                add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
                                   COMMAND ${CLANG_FORMAT} -i -style=file -fallback-style=none ${_obj})
            endif()
        else()
            message(WARNING "File/dir ${_obj} not exist.")
        endif()
    endforeach()
else()
    message(WARNING "clang-format not found.")
endif()
