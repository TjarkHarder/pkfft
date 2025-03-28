#ifndef __PKFFT_PKUTILS_H__
#define __PKFFT_PKUTILS_H__


#include "common.h"
#include "error.h"

#include <gsl/gsl_spline.h>


/* ================================================================ */
/* #############   Store the linear Power Spectrum   ############## */
/* ================================================================ */


/* #################   Typedef Change Struct   #################### */

typedef struct
{
    /*
     * Struct for keeping Track of Changes in the Data Struct
     */

    /* Overall Change */
    int version;

    /* Change in the Data Size */
    int sizeVersion;

    /* Change in the Data */
    int dataVersion;

} pkfft_pkutils_data_change_t;


/* ##################   Typedef Data Struct   ##################### */

typedef struct
{
    /*
     * Struct for linear Power Spectrum Data
     */

    /* Keep Track of Changes */
	pkfft_pkutils_data_change_t *change;

    /* Data */
    size_t size;
    double *k;
    double *pk;

} pkfft_pkutils_data_t;


/* ################   Create / Free Data Struct   ################# */

/* Create new Pk Data Struct */
pkfft_pkutils_data_t *pkfft_pkutils_data_alloc(void);

/* Copy a Pk Data Struct */
void pkfft_pkutils_data_cp(pkfft_pkutils_data_t *data, pkfft_pkutils_data_t *dataCp);

/* Free Pk Data Struct */
void pkfft_pkutils_data_free(pkfft_pkutils_data_t *data);


/* ####################   Set Dependencies   ###################### */

void pkfft_pkutils_data_set(pkfft_pkutils_data_t *data, double *k, double *pk, size_t size);


/* ###################   Read Data from File   #################### */

/* Read the linear Power Spectrum from CLASS */
#define PKFFT_PKUTILS_DATA_READ_CLASS_KEYWORD_SIZE "wavenumber"
#define PKFFT_PKUTILS_DATA_READ_CLASS_KEYCHAR_HEADER '#'
pkfft_pkutils_data_t *pkfft_pkutils_data_read_class(const char *file);


/* ================================================================ */
/* ###########   Interpolate the linear Power Spectrum   ########## */
/* ================================================================ */


/* #################   Typedef Change Struct   #################### */

typedef struct
{
    /*
     * Struct for keeping Track of Changes in the Spline Struct
     */

    /* Overall Change */
    int version;

    /* Change in the Input Data */
    pkfft_pkutils_data_change_t *dataChange;

} pkfft_pkutils_spline_change_t;


/* #################   Typedef Spline Struct   #################### */

typedef struct
{
    /*
     * Struct to store the Cubic Spline
     */

    /* Keep Track of Changes */
	pkfft_pkutils_spline_change_t *change;


    /**  Input Parameters  **/

    /* Data */
    pkfft_pkutils_data_t *data;


    /**  Calculated from Input Parameters  **/

    /* Extrapolation Parameters */
    double *as;
    double *ns;

    /* Spline */
    gsl_spline *spline;
    gsl_interp_accel *accel;

} pkfft_pkutils_spline_t;


/* ###############   Create / Free Spline Struct   ################ */

/* Create new Spline Struct */
pkfft_pkutils_spline_t *pkfft_pkutils_spline_alloc(void);

/* Copy Spline Struct */
void pkfft_pkutils_spline_cp(pkfft_pkutils_spline_t *spline, pkfft_pkutils_spline_t *splineCp);

/* Free Spline Struct */
void pkfft_pkutils_spline_free(pkfft_pkutils_spline_t *spline);


/* #####################   Set Dependencies   ##################### */

/* Set the Data Struct */
void pkfft_pkutils_spline_set_data(pkfft_pkutils_spline_t *spline, pkfft_pkutils_data_t *data);


/* #####################   Get Dependencies   ##################### */

/* Get the Data Struct */
pkfft_pkutils_data_t *pkfft_pkutils_spline_get_data(pkfft_pkutils_spline_t *spline);


/* ######################   Update Spline   ####################### */

void pkfft_pkutils_spline_update(pkfft_pkutils_spline_t *spline);


/* #####################   Evaluate Spline   ###################### */

double pkfft_pkutils_spline_eval(double k, pkfft_pkutils_spline_t *spline);


/* ================================================================ */
/* #############   Sample the linear Power Spectrum   ############# */
/* ================================================================ */


/* ##################   Logarithmic Sampling   #################### */

void pkfft_pkutils_data_sample_log10(pkfft_pkutils_data_t *data, pkfft_pkutils_spline_t *spline, double l10kmin, double l10kmax, size_t size, double bias);


#endif // __PKFFT_PKUTILS_H__
