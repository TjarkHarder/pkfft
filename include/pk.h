#ifndef __PKFFT_PK_H__
#define __PKFFT_PK_H__

#include "common.h"
#include "error.h"

#include "fft.h"

#include "pkutils.h"


/* ########################   Definitions   ####################### */

#define PKFFT_LOG10 2.30258509299404568401799145468436



/* ================================================================ */
/* ###################   Keep Track of Changes   ################## */
/* ================================================================ */


/* ###################   Typedef Change Struct   ################## */

typedef struct
{
    /*
     * Struct for keeping Track of Changes in the linear Power Spectrum Struct
     */

    /* Overall Change */
    int version;

    /* Change in Input Data */
    int dataVersion;

    /* Change in the Size of the Logarithmic Sampling */
    int sizeVersion;

    /* Change in the Bounds of the Logarithmic Sampling */
    int boundsVersion;

    /* Change in linear Power Spectrum Bias */
    int biasVersion;

} pkfft_pk_change_t;


/* ###############   Create / Free Spline Struct   ################ */

/* Create new Change Struct */
pkfft_pk_change_t *pkfft_pk_change_alloc(void);

/* Copy Change Struct */
void pkfft_pk_change_cp(pkfft_pk_change_t *change, pkfft_pk_change_t *changeCp);

/* Free Change Struct */
void pkfft_pk_change_free(pkfft_pk_change_t *change);



/* ================================================================ */
/* #############   Configure Linear Power Spectrum   ############## */
/* ================================================================ */


/* ###################   Typedef Config Struct   ################## */

typedef struct
{
    /*
     * Struct for the Configuration Parameters of the linear Power Spectrum
     */

    /* Keep Track of Changes */
    pkfft_pk_change_t *change;

    /* Linear Power Spectrum Input Data / Spline Interpolation */
    pkfft_pkutils_data_t *data; // Pointer to spline -> data
    pkfft_pkutils_spline_t *spline;

    /* Size of Logarithmic Sampling */
    size_t size;

    /* Logarithmic Bounds in Base 10 */
    double l10kmin;
    double l10kmax;

    /* Bias */
    double bias;

} pkfft_pk_config_t;


/* ###################   Create / Free Struct   ################### */

/* Create a new Config Struct */
pkfft_pk_config_t *pkfft_pk_config_alloc(void);

/* Copy a Config Struct */
void pkfft_pk_config_cp(pkfft_pk_config_t *config, pkfft_pk_config_t *configCp);

/* Free a Config Struct */
void pkfft_pk_config_free(pkfft_pk_config_t *config);


/* #####################   Set Dependencies   ##################### */

/* Set the Data */
void pkfft_pk_config_set_data(pkfft_pk_config_t *config, pkfft_pkutils_data_t *data);

/* Set the Logarithmic Bounds (Base 10) */
void pkfft_pk_config_set_bounds(pkfft_pk_config_t *config, double l10kmin, double l10kmax);

/* Set the Size of the Logarithmic Sampling */
void pkfft_pk_config_set_size(pkfft_pk_config_t *config, size_t size);

/* Set the Bias */
void pkfft_pk_config_set_bias(pkfft_pk_config_t *config, double bias);


/* #####################   Get Dependencies   ##################### */

/* Get the Data */
pkfft_pkutils_data_t *pkfft_pk_config_get_data(pkfft_pk_config_t *config);

/* Get the Logarithmic Bounds (Base 10) */
void pkfft_pk_config_get_bounds(pkfft_pk_config_t *config, double *l10kmin, double *l10kmax);

/* Get the Size of the Logarithmic Sampling */
size_t pkfft_pk_config_get_size(pkfft_pk_config_t *config);

/* Get the Bias */
double pkfft_pk_config_get_bias(pkfft_pk_config_t *config);


/* ######################   Update Config   ####################### */

void pkfft_pk_config_update(pkfft_pk_config_t *config);



/* ================================================================ */
/* ##################   Linear Power Spectrum   ################### */
/* ================================================================ */


/* ###########   Typedef Linear Power Spectrum Struct   ########### */

typedef struct
{
    /*
     * Struct for a linear Power Spectrum
     */

    /* Keep Track of Changes */
    pkfft_pk_change_t *change;

    /* Config Struct */
    pkfft_pk_config_t *config;

    /* Logarithmic Bounds in Base e */
    double lkmin;
    double lkmax;

    /* Logarithmic Differences (Base e) */
    double Dlk; // Absolute Difference
    double dlk; // Relative Difference

    /* Linear Power Spectrum Sample / Linear Power Spectrum FFT Coefficients */
    pkfft_pkutils_data_t *dataFFT;

    /* FFT of Linear Power Spectrum */
    pkfft_fft_t *fft;

} pkfft_pk_t;


/* ###################   Create / Free Struct   ################### */

/* Create a new linear Power Spectrum Struct */
pkfft_pk_t *pkfft_pk_alloc(void);

/* Copy a linear Power Spectrum Struct */
void pkfft_pk_cp(pkfft_pk_t *pk, pkfft_pk_t *pkCp);

/* Free a linear Power Spectrum Struct */
void pkfft_pk_free(pkfft_pk_t *pk);


/* #####################   Set Dependencies   ##################### */

/* Set the Config Struct */
void pkfft_pk_set_config(pkfft_pk_t *pk, pkfft_pk_config_t *config);


/* #####################   Get Dependencies   ##################### */

/* Get the Config Struct */
pkfft_pk_config_t *pkfft_pk_get_config(pkfft_pk_t *pk);


/* #############   Update the linear Power Spectrum   ############# */

void pkfft_pk_update(pkfft_pk_t *pk);


/* ############   Evaluate the linear Power Spectrum   ############ */

/* Evaluate the Spline Interpolation */
double pkfft_pk_eval_spline(double k, pkfft_pk_t *pk);

/* Evaluate the FFT */
double pkfft_pk_eval_fft(double k, pkfft_pk_t *pk);


#endif // __PKFFT_PK_H__
