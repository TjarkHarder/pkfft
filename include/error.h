#ifndef __PKFFT_ERROR_H__
#define __PKFFT_ERROR_H__


#include "common.h"


/* Print Error Message */
void pkfft_error(const char *errmsg, const char *file, const char *func, int line, ...);

/* Called in _pkfft... functions (prints error message and returns PKFFT_FAILURE) */
#define _PKFFT_ERROR(errmsg, ...) do {                                \
    pkfft_error(errmsg, __FILE__, __func__, __LINE__, ##__VA_ARGS__); \
    return PKFFT_FAILURE;                                             \
    } while(0)

/* Called in pkfft... functions (prints error message and aborts program) */
#define PKFFT_ERROR(errmsg, ...) do {                                 \
    pkfft_error(errmsg, __FILE__, __func__, __LINE__, ##__VA_ARGS__); \
    abort();                                                          \
    } while(0)


#endif // __PKFFT_ERROR_H__


