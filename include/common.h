#ifndef __PKFFT_COMMON_H__
#define __PKFFT_COMMON_H__


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>


/* Success + Failure */
#define PKFFT_SUCCESS 1
#define PKFFT_FAILURE 0

/* Passes status (return value of _pkfft... functions) to a pointer */
#define PKFFT_CALL_PASS_STATUS(status, ptr, ...) {        \
    ptr != NULL ? *((int*) ptr) = status : (void)(0);     \
  }

/* Calls _PKFFT_ERROR on error -> does not terminate program */
#define _PKFFT_CALL(function, ...) {                              \
    int __pkfft_status = function;                                \
    if (__pkfft_status == PKFFT_FAILURE)                          \
      {                                                           \
        _PKFFT_ERROR(NULL);                                       \
      }                                                           \
    PKFFT_CALL_PASS_STATUS(__pkfft_status, ##__VA_ARGS__, NULL);  \
  }

/* Calls PKFFT_ERROR on error -> terminates program */
#define PKFFT_CALL(function, ...) {                               \
    int __pkfft_status = function;                                \
    if (__pkfft_status == PKFFT_FAILURE)                          \
      {                                                           \
        PKFFT_ERROR(NULL);                                        \
      }                                                           \
    PKFFT_CALL_PASS_STATUS(__pkfft_status, ##__VA_ARGS__, NULL);  \
  }


#endif // __PKFFT_COMMON_H__
