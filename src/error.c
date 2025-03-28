#include "error.h"


void pkfft_error(const char *errmsg, const char *file, const char *func, int line, ...)
{
    /*
     * Print an error message
     */

    /* No Error Message to Print */
    if (errmsg == NULL)
      {
        /* Print where the Error was found */
        printf("\t%s : %s (line : %d)\n", file, func, line);
        return;
      }

    printf("PKFFT_ERROR : \n");

    /* Print the Error Message */
    va_list args;
    va_start(args, line);

    vprintf(errmsg, args);

    va_end(args);

    printf("\n");

    /* Print where the Error was found */
    printf("%s : %s (line : %d)\n", file, func, line);
}
