#include "pk.h"


/* ============================================================================================== */
/* ----------------------------------  Keep Track of Changes  ----------------------------------- */
/* ============================================================================================== */


/* ---------------------------------------------------------------------------------------------- */
/* -----------------------------------  Create / Free Struct  ----------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


pkfft_pk_change_t *pkfft_pk_change_alloc(void)
{
    /*
     * Create a new Change Struct for the linear Power Spectrum Struct
     */

    /* Allocate Memory */
    pkfft_pk_change_t *change = malloc(sizeof(pkfft_pk_change_t));


    /**  Initialise Elements  **/

    /* Overall Changes */
    change -> version = -1;

    /* Changes in the Input Data */
    change -> dataVersion = -1;

    /* Changes in the Size of the Logarithmic Sampling Size */
    change -> sizeVersion = -1;

    /* Changes in the Bounds of the Logarithmic Sampling */
    change -> boundsVersion = -1;

    /* Changes in the linear Power Spectrum Bias */
    change -> biasVersion = -1;

    return change;
}


void pkfft_pk_change_cp(pkfft_pk_change_t *change, pkfft_pk_change_t *changeCp)
{
    /*
     * Copy a Change Struct
     */

    /* Overall Changes */
    changeCp -> version = change -> version;

    /* Changes in the Input Data */
    changeCp -> dataVersion = change -> dataVersion;

    /* Changes in the Size of the Logarithmic Sampling Size */
    changeCp -> sizeVersion = change -> sizeVersion;

    /* Changes in the Bounds of the Logarithmic Sampling */
    changeCp -> boundsVersion = change -> boundsVersion;

    /* Changes in the linear Power Spectrum Bias */
    changeCp -> biasVersion = change -> biasVersion;
}


void pkfft_pk_change_free(pkfft_pk_change_t *change)
{
    /*
     * Free a Change Struct
     */

    /* Check for NULL */
    if (change == NULL) return;

    /* Free the Struct */
    free(change);
}



/* ============================================================================================== */
/* ----------------------------  Configure the Linear Power Spectrum  --------------------------- */
/* ============================================================================================== */


/* ---------------------------------------------------------------------------------------------- */
/* -------------------------------  Default Struct Dependencies  -------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


/* Default Linear Power Spectrum */
static const size_t _dataSize = 100;
static double _dataK[100] = {1.000000000000e-04, 1.123324032978e-04, 1.261856883066e-04, 1.417474162927e-04, 1.592282793341e-04, 1.788649529057e-04, 2.009233002565e-04, 2.257019719634e-04, 2.535364493970e-04, 2.848035868436e-04,
                             3.199267137797e-04, 3.593813663805e-04, 4.037017258597e-04, 4.534878508129e-04, 5.094138014816e-04, 5.722367659350e-04, 6.428073117284e-04, 7.220809018385e-04, 8.111308307897e-04, 9.111627561155e-04,
                             1.023531021899e-03, 1.149756995398e-03, 1.291549665015e-03, 1.450828778496e-03, 1.629750834621e-03, 1.830738280295e-03, 2.056512308349e-03, 2.310129700083e-03, 2.595024211400e-03, 2.915053062825e-03,
                             3.274549162878e-03, 3.678379771829e-03, 4.132012400115e-03, 4.641588833613e-03, 5.214008288000e-03, 5.857020818057e-03, 6.579332246576e-03, 7.390722033526e-03, 8.302175681320e-03, 9.326033468832e-03,
                             1.047615752790e-02, 1.176811952435e-02, 1.321941148466e-02, 1.484968262254e-02, 1.668100537200e-02, 1.873817422860e-02, 2.104904144512e-02, 2.364489412645e-02, 2.656087782947e-02, 2.983647240283e-02,
                             3.351602650939e-02, 3.764935806792e-02, 4.229242874389e-02, 4.750810162103e-02, 5.336699231206e-02, 5.994842503189e-02, 6.734150657751e-02, 7.564633275546e-02, 8.497534359086e-02, 9.545484566618e-02,
                             1.072267222010e-01, 1.204503540259e-01, 1.353047774580e-01, 1.519911082953e-01, 1.707352647471e-01, 1.917910261672e-01, 2.154434690032e-01, 2.420128264794e-01, 2.718588242733e-01, 3.053855508833e-01,
                             3.430469286315e-01, 3.853528593711e-01, 4.328761281083e-01, 4.862601580065e-01, 5.462277217684e-01, 6.135907273413e-01, 6.892612104350e-01, 7.742636826811e-01, 8.697490026178e-01, 9.770099572992e-01,
                             1.097498765493e+00, 1.232846739442e+00, 1.384886371394e+00, 1.555676143930e+00, 1.747528400008e+00, 1.963040650040e+00, 2.205130739903e+00, 2.477076355992e+00, 2.782559402207e+00, 3.125715849688e+00,
                             3.511191734215e+00, 3.944206059438e+00, 4.430621457584e+00, 4.977023564332e+00, 5.590810182512e+00, 6.280291441834e+00, 7.054802310719e+00, 7.924828983539e+00, 8.902150854450e+00, 1.000000000000e+01};
static double _dataPk[100] = {4.294604757180e+02, 4.804909310731e+02, 5.375759282597e+02, 6.014301019853e+02, 6.728509460480e+02, 7.527278323534e+02, 8.420517861548e+02, 9.419259966458e+02, 1.053577007240e+03, 1.178366485935e+03,
                              1.317803424070e+03, 1.473556554621e+03, 1.647466723739e+03, 1.841558895732e+03, 2.058053419191e+03, 2.299376105355e+03, 2.568166503218e+03, 2.867283378623e+03, 3.199805677386e+03, 3.569026162050e+03,
                              3.978433842691e+03, 4.431681203486e+03, 4.932533851959e+03, 5.484802275711e+03, 6.092253630251e+03, 6.758494944916e+03, 7.486819033704e+03, 8.280016746393e+03, 9.140150713233e+03, 1.006830216344e+04,
                              1.106424253670e+04, 1.212605719769e+04, 1.324972623707e+04, 1.442868551731e+04, 1.565335379047e+04, 1.691057690958e+04, 1.818306683913e+04, 1.944889494299e+04, 2.068091662620e+04, 2.184634542412e+04,
                              2.290646183845e+04, 2.381667793149e+04, 2.452717711967e+04, 2.498456983135e+04, 2.513506270001e+04, 2.492978851205e+04, 2.433281325399e+04, 2.333182679098e+04, 2.195035724756e+04, 2.025840724596e+04,
                              1.837615768057e+04, 1.646414986142e+04, 1.469511518250e+04, 1.320814082831e+04, 1.205299535477e+04, 1.114066351268e+04, 1.023709283040e+04, 9.072117086505e+03, 7.588234192000e+03, 6.105991116218e+03,
                              5.065351391726e+03, 4.508319718218e+03, 3.950890520897e+03, 3.116173990411e+03, 2.475958296203e+03, 2.155159790116e+03, 1.710028975182e+03, 1.365363955354e+03, 1.122384518494e+03, 8.798449978110e+02,
                              7.059241690615e+02, 5.584084996630e+02, 4.376777446508e+02, 3.435316825501e+02, 2.683242200145e+02, 2.086194463391e+02, 1.615928079779e+02, 1.247419981384e+02, 9.597286474243e+01, 7.360761311221e+01,
                              5.628774691937e+01, 4.287720486244e+01, 3.264529765560e+01, 2.473609210272e+01, 1.874267965752e+01, 1.413477885308e+01, 1.066344519721e+01, 8.007803957526e+00, 6.017274872673e+00, 4.501411723837e+00,
                              3.370374831830e+00, 2.512578613530e+00, 1.875101144976e+00, 1.393445718583e+00, 1.036856574628e+00, 7.684130460011e-01, 5.699240531491e-01, 4.207360898323e-01, 3.128243259212e-01, 2.331258696529e-01};

static pkfft_pkutils_data_t _data = {.size = _dataSize, .k = _dataK, .pk = _dataPk};

/* Default Logarithmic Sampling */
static const size_t _size = 256;
static const double _l10kmin = -4.; // 10^{-4}
static const double _l10kmax =  2.; // 10^{2}

/* Default Bias */
static const double _bias = -1.6;


/* ---------------------------------------------------------------------------------------------- */
/* -----------------------------------  Create / Free Struct  ----------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


pkfft_pk_config_t *pkfft_pk_config_alloc(void)
{
    /*
     * Create a new Config Struct for the linear Power Spectrum
     */

    /* Allocate Memory */
    pkfft_pk_config_t *config = malloc(sizeof(pkfft_pk_config_t));

    /* Keep Track of Changes */
	config -> change = pkfft_pk_change_alloc();

    /* Linear Power Spectrum Input Data / Spline Interpolation */
    config -> spline = pkfft_pkutils_spline_alloc();
    config -> data = config -> spline -> data;
    pkfft_pkutils_data_cp(&_data, config -> data);

    /* Size of Logarithmic Sampling */
    config -> size = _size;

    /* Logarithmic Bounds */
    config -> l10kmin = _l10kmin;
    config -> l10kmax = _l10kmax;

    /* Bias */
    config -> bias = _bias;

    return config;
}


void pkfft_pk_config_cp(pkfft_pk_config_t *config, pkfft_pk_config_t *configCp)
{
    /*
     * Copy a linear Power Spectrum Config Struct
     */

    /* Copy the Data Struct */
    pkfft_pk_config_set_data(configCp, config -> data);

    /* Copy Size of Logarithmic Sampling */
    pkfft_pk_config_set_size(configCp, config -> size);

    /* Copy Logarithmic Bounds */
    pkfft_pk_config_set_bounds(configCp, config -> l10kmin, config -> l10kmax);

    /* Copy Bias */
    pkfft_pk_config_set_bias(configCp, config -> bias);

    /* Update */
    pkfft_pk_config_update(configCp);
}


void pkfft_pk_config_free(pkfft_pk_config_t *config)
{
    /*
     * Free a linear Power Spectrum Config Struct
     */

    /* Check for NULL */
    if (config == NULL) return;

    /* Free Changes */
	pkfft_pk_change_free(config -> change);

    /* Free Spline Interpolation */
    pkfft_pkutils_spline_free(config -> spline);

    /* Free Struct itself */
    free(config);
}


/* ---------------------------------------------------------------------------------------------- */
/* -------------------------------------  Set Dependencies  ------------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


void pkfft_pk_config_set_data(pkfft_pk_config_t *config, pkfft_pkutils_data_t *data)
{
    /*
     * Set the linear Power Spectrum Input Data
     */

    // TODO: Check if data points to config -> data

    pkfft_pkutils_data_cp(data, config -> data);
}


void pkfft_pk_config_set_bounds(pkfft_pk_config_t *config, double l10kmin, double l10kmax)
{
    /*
     * Set the Logarithmic Bounds in Base 10
     */

    /* Set Change Flag */
    bool boundsChange = config -> l10kmin != l10kmin || config -> l10kmax != l10kmax;

    /* Config is up to Date */
    if (!boundsChange) return;

    /* Update the Bounds */
    config -> l10kmin = l10kmin;
    config -> l10kmax = l10kmax;

    /* Update Versions */
    config -> change -> boundsVersion++;
    config -> change -> version++;
}


void pkfft_pk_config_set_size(pkfft_pk_config_t *config, size_t size)
{
    /*
     * Set the Size of the Logarithmic Sampling
     */

    /* Set Change Flag */
    bool sizeChange = config -> size != size;

    /* Config is up to Date */
    if (!sizeChange) return;

    /* Update the Size */
    config -> size = size;

    /* Update Versions */
    config -> change -> sizeVersion++;
    config -> change -> version++;
}


void pkfft_pk_config_set_bias(pkfft_pk_config_t *config, double bias)
{
    /*
     * Set the bias of the linear Power Spectrum FFT
     */

    /* Set Change Flag */
    bool biasChange = config -> bias != bias;

    /* Config is up to Date */
    if (!biasChange) return;

    /* Update the Size */
    config -> bias = bias;

    /* Update Versions */
    config -> change -> biasVersion++;
    config -> change -> version++;
}


/* ---------------------------------------------------------------------------------------------- */
/* -------------------------------------  Get Dependencies  ------------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


pkfft_pkutils_data_t *pkfft_pk_get_data(pkfft_pk_config_t *config)
{
    /*
     * Get the linear Power Spectrum Input Data
     */

    return config -> data;
}


void pkfft_pk_config_get_bounds(pkfft_pk_config_t *config, double *l10kmin, double *l10kmax)
{
    /*
     * Get the Logarithmic Bounds in Base 10
     */

    *l10kmin = config -> l10kmin;
    *l10kmax = config -> l10kmax;
}


size_t pkfft_pk_config_get_size(pkfft_pk_config_t *config)
{
    /*
     * Get the Size of the Logarithmic Sampling
     */

    return config -> size;
}


double pkfft_pk_config_get_bias(pkfft_pk_config_t *config)
{
    /*
     * Get the bias of the linear Power Spectrum FFT
     */

    return config -> bias;
}


/* ---------------------------------------------------------------------------------------------- */
/* -----------------------------------------  Update  ------------------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


void pkfft_pk_config_update(pkfft_pk_config_t *config)
{
    /*
     * Update the linear Power Spectrum Config Struct
     */

    /* Set Change Flag */
    bool dataChange = config -> data -> change -> version != config -> change -> dataVersion;

    /* Config is up to Date */
    if (!dataChange) return;

    /* Update the Spline Interpolation */
    pkfft_pkutils_spline_update(config -> spline);

    /* Update Versions */
    config -> change -> dataVersion = config -> data -> change -> version;
    config -> change -> version++;
}



/* ============================================================================================== */
/* ----------------------------------  Linear Power Spectrum  ----------------------------------- */
/* ============================================================================================== */


/* ---------------------------------------------------------------------------------------------- */
/* -----------------------------------  Create / Free Struct  ----------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


pkfft_pk_t *pkfft_pk_alloc(void)
{
    /*
     * Create a new linear Power Spectrum Struct
     */

    /* Allocate Memory */
    pkfft_pk_t *pk = malloc(sizeof(pkfft_pk_t));

    /* Keep Track of Changes */
	pk -> change = pkfft_pk_change_alloc();

    /* Config Struct */
    pk -> config = pkfft_pk_config_alloc();

    /* Linear Power Spectrum Sample / Linear Power Spectrum FFT Coefficients */
    pk -> dataFFT = pkfft_pkutils_data_alloc();

    /* FFT of Linear Power Spectrum */
    pk -> fft = pkfft_fft_alloc(); // Plan will be created on first call of pkfft_pk_update
    pkfft_fft_set_type(pk -> fft, PKFFT_FFT_R2HC);

    return pk;
}


void pkfft_pk_cp(pkfft_pk_t *pk, pkfft_pk_t *pkCp)
{
    /*
     * Copy a linear Power Spectrum Struct
     */

    /* Copy the Config Struct */
    pkfft_pk_set_config(pkCp, pk -> config);

    /* Update */
    pkfft_pk_update(pkCp);
}


void pkfft_pk_free(pkfft_pk_t *pk)
{
    /*
     * Free a linear Power Spectrum Struct
     */

    /* Check for NULL */
    if (pk == NULL) return;

    /* Free Changes */
	pkfft_pk_change_free(pk -> change);

    /* Free Config */
    pkfft_pk_config_free(pk -> config);

    /* Free Sample / FFT Coefficients */
    pkfft_pkutils_data_free(pk -> dataFFT);

    /* Free FFT */
    pkfft_fft_free(pk -> fft);

    /* Free Struct */
    free(pk);
}


/* ---------------------------------------------------------------------------------------------- */
/* -------------------------------------  Set Dependencies  ------------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


void pkfft_pk_set_config(pkfft_pk_t *pk, pkfft_pk_config_t *config)
{
    /*
     * Set the linear Power Spectrum Config Struct
     */

    pkfft_pk_config_cp(config, pk -> config);
}


/* ---------------------------------------------------------------------------------------------- */
/* -------------------------------------  Get Dependencies  ------------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


pkfft_pk_config_t *pkfft_pk_get_config(pkfft_pk_t *pk)
{
    /*
     * Get the linear Power Spectrum Config Struct
     */

    return pk -> config;
}


/* ---------------------------------------------------------------------------------------------- */
/* ----------------------------  Update the Linear Power Spectrum  ------------------------------ */
/* ---------------------------------------------------------------------------------------------- */


void pkfft_pk_update(pkfft_pk_t *pk)
{
    /*
     * Update the linear Power Spectrum
     */

    /* First Call of this Function */
    bool ini = pk -> fft -> change -> version == -1;

    /* Change Struct */
    pkfft_pk_change_t *change = pk -> change;

    /* Config Struct */
    pkfft_pk_config_t *config = pk -> config;

    /* Update the Config Struct */
    pkfft_pk_config_update(config);

    /* Set Change Flags */
    bool dataChange = config -> change -> dataVersion != change -> dataVersion || ini;
    bool sizeChange = config -> change -> sizeVersion != change -> sizeVersion || ini;
    bool boundsChange = config -> change -> boundsVersion != change -> boundsVersion || ini;
    bool biasChange = config -> change -> biasVersion != change -> biasVersion || ini;

    bool fftChange = dataChange || sizeChange || boundsChange || biasChange;

    /* Update Logarithmic Bounds */
    if (boundsChange)
      {
        pk -> lkmin = config -> l10kmin * PKFFT_LOG10; // From Base 10 to Base e
        pk -> lkmax = config -> l10kmax * PKFFT_LOG10; // From Base 10 to Base e
        pk -> Dlk = pk -> lkmax - pk -> lkmin;
      }

    /* Update the Size of the FFT */
    if (sizeChange)
      {
        /* Set the Size */
        pkfft_fft_set_size(pk -> fft, config -> size);

        /* Plan the FFT */
        pkfft_fft_plan(pk -> fft);
      }

    /* Update Relative Difference */
    if (sizeChange || boundsChange)
      {
        pk -> dlk = pk -> Dlk / ((double) (config -> size - 1));
      }

    /* Update the FFT */
    if (fftChange)
      {
        /* Sample the linear Power Spectrum */
        pkfft_pkutils_data_sample_log10(pk -> dataFFT, config -> spline, config -> l10kmin, config -> l10kmax, config -> size, config -> bias); // TODO: Fairly expensive (could get to 1e-6s instead of 1e-5s...)

        /* Insert the Data into the FFT Struct */
        pkfft_fft_set_data(pk -> fft, pk -> dataFFT -> pk);

        /* Perform the FFT */
        pkfft_fft(pk -> fft);
      }

    /* Update Change Struct */
    pkfft_pk_change_cp(config -> change, change);
}


/* ---------------------------------------------------------------------------------------------- */
/* ----------------------------  Evaluate the Linear Power Spectrum  ---------------------------- */
/* ---------------------------------------------------------------------------------------------- */


double pkfft_pk_eval_spline(double k, pkfft_pk_t *pk)
{
    /*
     * Evaluate the linear Power Spectrum at a given Scale from its Spline Interpolation
     */

    return pkfft_pkutils_spline_eval(k, pk -> config -> spline);
}


double pkfft_pk_eval_fft(double k, pkfft_pk_t *pk)
{
    /*
     * Evaluate the linear Power Spectrum at a given Scale from its FFT
     *
     * Note: In contrast to the inverse FFT (e.g. pkfft_fft_hc2r_n2)
     *
     *          P(k) = 1/N \sum_{m = 0}^{N - 1} c_m k^\nu (k / k_{min})^{i 2\pi m (N - 1) / (N * \log k_{max}/k_{min})}
     *
     *      this function calculates the FFT via the positive and negative frequency decomposition
     *
     *          P(k) = 1/N \sum_{m = -\ceil{N/2} + 1}^{\floor{N/2}} \tilde{c}_m k^\nu (k / k_{min})^{i 2\pi m (N - 1) / (N * log(k_{max} / k_{min}))}
     *
     *      with
     *                          /
     *                         | c_m         for m \in [0, \floor{N/2}]
     *          \tilde{c}_m = <
     *                         | (c_{-m})^*  for m \in [-\ceil{N/2} + 1, -1]
     *                          \
     *
     *      The two are equivalent only if \log k/k_{min} is a multiple of \log k_{max}/k_{min}. The
     *      second, however, is essentially a discrete version of the Fourier transform and thus
     *      provides a much better approximation of P(k) when k is general.
     */

    /* FFT Struct */
    pkfft_fft_t *fft = pk -> fft;

    /* Base Angle */
    double theta0 = 2. * M_PI * (log(k) - pk -> lkmin) / pk -> Dlk * (1. - 1. / ((double) fft -> size));

    /* "Undo" Bias */
    double bias = pow(k, pk -> config -> bias);

    /* Initialise the linear Power Spectrum */
    double val = fft -> dataOut[0];

    /* Upper Bound of Loop */
    size_t size = fft -> sizehalf + fft -> odd; // Need one extra iteration for N % 2 = 1 due to inclusion of positive and negative \floor{N/2} frequencies

    for (size_t i = 1; i < size; i++)
      {
        double theta = theta0 * (double) i;
        val += 2. * (fft -> dataOut[i] * cos(theta) - fft -> dataOut[fft -> size - i] * sin(theta));
      }

    if (!fft -> odd) // If N % 2 = 0 the Nyquist frequency N/2 only enters as +N/2 but not as -N/2, such that it cannot be included in the loop above
      {
        val += fft -> dataOut[fft -> sizehalf] * cos(theta0 * (double) (fft -> sizehalf)); // Nyquist frequency has a real coefficient
      }

    return val * bias / (double) fft -> size; // Acount for bias and the fact that the inverse FFT must be divided by N
}
