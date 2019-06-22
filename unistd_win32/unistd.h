/*******************************************************************************
*  file    : unistd.h
*  created : 05.01.2019
*  author  : Slyshyk Oleksiy (alexslyshyk@gmail.com)
*******************************************************************************/

#ifndef __UNISTD_CUSTOM_H
#define __UNISTD_CUSTOM_H

#include <stdint.h>

#ifdef _WIN32
#include <Windows.h>
#include <io.h>
#endif

#ifndef STM32FLASH_HAVE_GETOPT
#include "getopt.h"
#endif

#ifndef STM32FLASH_HAVE_SSIZE_T
typedef int ssize_t;
#endif

#ifdef __cplusplus
extern "C"
#endif
void usleep(unsigned long waitTime);

#endif //__UNISTD_CUSTOM_H
