cmake_minimum_required(VERSION 3.6.0)

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
else()
    add_definitions(-DSTM32FLASH_HAVE_GETOPT)
endif()

if (HAVE_SSIZE_T)
    add_definitions(-DSTM32FLASH_HAVE_SSIZE_T)
endif()

if(MSVC)
    add_compile_definitions(__WIN32__)
endif()

add_executable(${PROJECT_NAME} ${SRC_FILES})
