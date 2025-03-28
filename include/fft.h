#ifndef __PKFFT_FFT_H__
#define __PKFFT_FFT_H__

#include "common.h"
#include "error.h"

#include <fftw3.h>


/* #######################   Definitions   ######################## */

#define PKFFT_FFT_R2HC "r2hc"
#define PKFFT_FFT_HC2R "hc2r"


/* ###################   Typedef Change Struct   ################## */

typedef struct
{
    /*
     * Struct for keeping Track of Changes in the FFT Struct
     */

    /* Overall Change */
    bool change;
    int version;

    /* Change in the Input Data */
    bool dataChange;
    int dataVersion;

    /* Change in the Size of the Input Data */
    bool sizeChange;
	int sizeVersion;

    /* Change in the Type of the FFT */
    bool typeChange;
	int typeVersion;

    /* Change in the FFT Plan */
    bool planChange;
    int planVersion;

} pkfft_fft_change_t;


/* ####################   Typedef FFT Struct   #################### */

typedef struct
{
    /*
     * Struct for FFTW's FFT
     */

    /* Keep Track of Changes */
    pkfft_fft_change_t *change;


    /**  Input Parameters  **/

    /* Size of Data */
    size_t size;

	/* Input Data */
	double *dataIn;

    /* Type of FFT */
    const char *type;


	/**  Plan the FFT from Input Parameters  **/

    /* FFTw Kind + Plan */
    fftw_r2r_kind kind;
    fftw_plan plan;


    /**  Calculate the FFT from Input Parameters and Plan  **/

	/* Flag Odd Size */
    bool odd;

    /* Half of the Size of Data */
    size_t sizehalf;

    /* Output Data */
    double *dataOut;

    /* Normalisation of the Output Data */
    size_t *normOutPtr; // R2HC : NULL Pointer ; HC2R : Pointer to Size (Arbitrary Choice)
    double normOut; // Actual Normalisation (1 for R2HC and Size for HC2R)

} pkfft_fft_t;


/* ###################   Create / Free Struct   ################### */

/* Create a New FFT Struct */
pkfft_fft_t *pkfft_fft_alloc(void);

/* Copy a FFT Struct */
void pkfft_fft_cp(pkfft_fft_t *fft, pkfft_fft_t *fftCp);

/* Free a FFT Struct */
void pkfft_fft_free(pkfft_fft_t *fft);


/* #####################   Set Dependencies   ##################### */

/* Set the Type of the FFT */
void pkfft_fft_set_type(pkfft_fft_t *fft, const char *type);

/* Set the Size */
void pkfft_fft_set_size(pkfft_fft_t *fft, size_t size);

/* Set the Input Data */
void pkfft_fft_set_data(pkfft_fft_t *fft, double *data);


/* #####################   Get Dependencies   ##################### */

/* Get the Type of the FFT */
const char *pkfft_fft_get_type(pkfft_fft_t *fft);

/* Get the Size of the FFT */
size_t pkfft_fft_get_size(pkfft_fft_t *fft);


/* #######################   Plan the FFT   ####################### */

void pkfft_fft_plan(pkfft_fft_t *fft);


/* ######################   Perform the FFT   ##################### */

void pkfft_fft(pkfft_fft_t *fft);


/* ################   Custom FFT Implementations   ################ */

void pkfft_fft_r2hc_n2(double *data, size_t size);
void pkfft_fft_hc2r_n2(double *data, size_t size);


#endif // __PKFFT_FFT_H__
