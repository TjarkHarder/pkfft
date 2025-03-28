#include "fft.h"


/* ---------------------------------------------------------------------------------------------- */
/* ----------------------------------  Keep Track of Changes  ----------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


static pkfft_fft_change_t *_pkfft_fft_change_alloc(void)
{
    /*
     * Create a new Change Struct for the FFT Struct
     */

    /* Allocate Memory */
    pkfft_fft_change_t *change = malloc(sizeof(pkfft_fft_change_t));


    /**  Initialise Elements  **/

    /* Overall Changes */
    change -> change = true;
    change -> version = -1;

    /* Change in the Input Data */
    change -> dataChange = true;
    change -> dataVersion = -1;

    /* Change in the Size of the Input Data */
    change -> sizeChange = true;
    change -> sizeVersion = -1;

    /* Change in the Type of the FFT */
    change -> typeChange = true;
    change -> typeVersion = -1;

    /* Change in the FFT Plan */
    change -> planChange = true;
    change -> planVersion = -1;

    return change;
}


static void _pkfft_fft_change_free(pkfft_fft_change_t *change)
{
    /*
     * Free a Change Struct
     */

    /* Check for NULL */
    if (change == NULL) return;

    /* Free the Struct */
    free(change);
}


/* ---------------------------------------------------------------------------------------------- */
/* -----------------------------------  Create / Free Struct  ----------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


pkfft_fft_t *pkfft_fft_alloc(void)
{
    /*
     * Create a New FFT Struct
     */

    /* Allocate Memory */
    pkfft_fft_t *fft = malloc(sizeof(pkfft_fft_t));

    /* Keep Track of Changes */
    fft -> change = _pkfft_fft_change_alloc();

    /* Data Size */
    fft -> size = 0;

    /* Input Data */
    fft -> dataIn = NULL;

    /* FFT Type */
    fft -> type = PKFFT_FFT_R2HC; // Default is Real to Halfcomplex

    /* FFTW Plan */
    fft -> plan = NULL;
    fft -> kind = FFTW_R2HC; // Default is Real to Halfcomplex

	/* Output Data */
    fft -> dataOut = NULL;

    /* Output Normalisation */
    fft -> normOutPtr = NULL;
    fft -> normOut = 0.;

    return fft;
}


void pkfft_fft_cp(pkfft_fft_t *fft, pkfft_fft_t *fftCp)
{
    /*
     * Copy a FFT Struct
     */

	/* Copy Size */
	pkfft_fft_set_size(fftCp, fft -> size);

	/* Copy Type */
	pkfft_fft_set_type(fftCp, fft -> type);

	/* Copy Data */
	pkfft_fft_set_data(fftCp, fft -> dataIn);

    /* FFTW Plan */
    pkfft_fft_plan(fftCp);
}


void pkfft_fft_free(pkfft_fft_t *fft)
{
    /*
     * Free a FFT Struct
     */

    /* Check for NULL */
    if (fft == NULL) return;

    /**  Free Memory  **/

    /* Free Change Struct */
    _pkfft_fft_change_free(fft -> change);

    /* Free Plan */
    if (fft -> plan != NULL)
      {
        fftw_destroy_plan(fft -> plan);
      }

    /* Free Data */
    free(fft -> dataIn);
    free(fft -> dataOut);

    /* Free Struct itself */
    free(fft);
}


/* ---------------------------------------------------------------------------------------------- */
/* -------------------------------------  Set Dependencies  ------------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


void pkfft_fft_set_type(pkfft_fft_t *fft, const char *type)
{
    /*
     * Set the Type of the FFT
     */

    /* Real to Halfcomplex */
    if (!strcmp(type, PKFFT_FFT_R2HC))
      {
        fft -> change -> typeChange = strcmp(fft -> type, PKFFT_FFT_R2HC);
        fft -> type = PKFFT_FFT_R2HC;
        fft -> kind = FFTW_R2HC;
        fft -> normOutPtr = NULL;
      }

    /* Halfcomplex to Real */
    else if (!strcmp(type, PKFFT_FFT_HC2R))
      {
        fft -> change -> typeChange = strcmp(fft -> type, PKFFT_FFT_HC2R);
        fft -> type = PKFFT_FFT_HC2R;
        fft -> kind = FFTW_HC2R;
        fft -> normOutPtr = &fft -> size;
      }

    else
      {
        PKFFT_ERROR("Unsupported FFT type '%s'... The FFT is only supported for the '%s' and '%s' types.\n", type, PKFFT_FFT_R2HC, PKFFT_FFT_HC2R);
      }
}


void pkfft_fft_set_size(pkfft_fft_t *fft, size_t size)
{
    /*
     * Set the Size for the FFT
     */

    /* Need to Reallocate Memory if Sizes are Different */
    if (fft -> size != size)
      {
        fft -> dataIn = realloc(fft -> dataIn, sizeof(double) * size);
        fft -> dataOut = realloc(fft -> dataOut, sizeof(double) * size);

        for (size_t i = fft -> size; i < size; i++)
          {
            fft -> dataIn[i] = 0.;
          }
      }

    /* Size of Data */
    fft -> change -> sizeChange = fft -> size != size;
    fft -> size = size;
    fft -> odd = size % 2;
    fft -> sizehalf = (size - fft -> odd) / 2;
}


void pkfft_fft_set_data(pkfft_fft_t *fft, double *data)
{
    /*
     * Set the Input Data for the FFT
     */

    /* Initialise Change Flag */
    fft -> change -> dataChange = false;

    /* Set Data */
    for (size_t i = 0; i < fft -> size; i++)
      {
        fft -> change -> dataChange |= fft -> dataIn[i] != data[i];
        fft -> dataIn[i] = data[i];
      }
}


/* ---------------------------------------------------------------------------------------------- */
/* -------------------------------------  Get Dependencies  ------------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


const char *pkfft_fft_get_type(pkfft_fft_t *fft)
{
    /*
     * Get the Type of the FFT
     */

    return fft -> type;
}


size_t pkfft_fft_get_size(pkfft_fft_t *fft)
{
    /*
     * Get the Size of the FFT
     */

    return fft -> size;
}


/* ---------------------------------------------------------------------------------------------- */
/* ---------------------------------------  Plan the FFT  --------------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


void pkfft_fft_plan(pkfft_fft_t *fft)
{
    /*
     * Plan the FFT
     */

    /* Plan is already up to Date */
    if (!fft -> change -> sizeChange && !fft -> change -> typeChange)
      {
        fft -> change -> planChange = false;
        return;
      }

    /* Destroy existing Plan */
    if (fft -> plan != NULL)
      {
        fftw_destroy_plan(fft -> plan);
      }

    /* Create Plan */
    fft -> plan = fftw_plan_r2r_1d((int) fft -> size, fft -> dataIn, fft -> dataOut, fft -> kind, FFTW_MEASURE);

    /* Set the Normalisation */
    fft -> normOut = (fft -> normOutPtr == NULL) ? 1. : (double) (fft -> size);

    /* Flag Changed Plan */
    fft -> change -> planChange = true;

	/* Keep Track of Changes */
	fft -> change -> sizeVersion += fft -> change -> sizeChange;
	fft -> change -> typeVersion += fft -> change -> typeChange;

    /* Reset Flags */
    fft -> change -> typeChange = false;
    fft -> change -> sizeChange = false;
}


/* ---------------------------------------------------------------------------------------------- */
/* -------------------------------------  Perform the FFT  -------------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


void pkfft_fft(pkfft_fft_t *fft)
{
    /*
     * Perform the FFT
     */

    /* Check Input */
    if (fft -> change -> sizeChange || fft -> change -> typeChange)
      {
        PKFFT_ERROR("Must first call 'pkfft_fft_plan' after 'pkfft_fft_set_size' or 'pkfft_fft_set_type'.\n");
      }

	/* Set Change Flag */
	fft -> change -> change = fft -> change -> planChange || fft -> change -> dataChange;

    /* FFT is up to Date */
    if (!fft -> change -> planChange && !fft -> change -> dataChange) return;

    /* Perform the FFT */
    fftw_execute(fft -> plan);

	/* Keep Track of Changes */
	fft -> change -> planVersion += fft -> change -> planChange;
	fft -> change -> dataVersion += fft -> change -> dataChange;

	fft -> change -> version++;

    /* Reset Flags */
    fft -> change -> planChange = false;
    fft -> change -> dataChange = false;
}


/* ---------------------------------------------------------------------------------------------- */
/* --------------------------------  Custom FFT Implementation  --------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


void pkfft_fft_r2hc_n2(double *data, size_t size)
{
    /*
     * Perform the FFT Algorithm for real to halfcomplex Data using a custom, slow O(n^2) Algorithm
     */

    bool odd = size % 2;
    size_t sizehalf = (size - odd) / 2;

    double *dataFFT = calloc(size, sizeof(double));

    for (size_t i = 0; i < size; i++)
      {
        dataFFT[0] += data[i];

        for (size_t j = 1; j < sizehalf + odd; j++)
          {
            dataFFT[j] += data[i] * cos(2. * M_PI * (double) (i * j) / (double) size);
            dataFFT[size - j] -= data[i] * sin(2. * M_PI * (double) (i * j) / (double) size);
          }

        if (!odd)
          {
            dataFFT[sizehalf] += data[i] * cos(2. * M_PI * (double) (i * sizehalf) / (double) size);
          }
      }

    memcpy(data, dataFFT, sizeof(double) * size);
    free(dataFFT);
}


void pkfft_fft_hc2r_n2(double *data, size_t size)
{
    /*
     * Perform the Inverse FFT Algorithm for halfcomplex to real Data using a custom, slow O(n^2) Algorithm
     */

    bool odd = size % 2;
    size_t sizehalf = (size - odd) / 2;

    double *dataFFT = malloc(sizeof(double) * size);

    for (size_t i = 0; i < size; i++)
      {
        dataFFT[i] = data[0];

        for (size_t j = 1; j < sizehalf + odd; j++)
          {
            size_t j1 = j;
            size_t j2 = size - j;

            dataFFT[i] += data[j1] * (cos(2. * M_PI * (double) (j1 * i) / (double) size) + cos(2. * M_PI * (double) (j2 * i) / (double) size))
                            - data[j2] * (sin(2. * M_PI * (double) (j1 * i) / (double) size) - sin(2. * M_PI * (double) (j2 * i) / (double) size));
          }

        if (!odd)
          {
            dataFFT[i] += data[sizehalf] * cos(2. * M_PI * (double) (sizehalf * i) / (double) size);
          }

        dataFFT[i] /= ((double) size);
      }

    memcpy(data, dataFFT, sizeof(double) * size);
    free(dataFFT);
}
