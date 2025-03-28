#include "pkfft.h"


#define TEST_FFT 0
#define TEST_PKUTILS 0
#define TEST_PK 1


int test_fft()
{
    /* Generate Pulse */
    size_t size = 256;
    double data[size], diff = 0.;

    for (size_t i = 0; i < size; i++)
      {
        data[i] = 0.0;
      }

    for (size_t i = size / 3; i < 2 * size / 3; i++)
      {
        data[i] = 1.0;
      }

    /* Create FFT Structs */
    pkfft_fft_t *fftR2HC = pkfft_fft_alloc();
    pkfft_fft_t *fftHC2R = pkfft_fft_alloc();

    /* Set Types */
    pkfft_fft_set_type(fftR2HC, "r2hc");
    pkfft_fft_set_type(fftHC2R, "hc2r");

    /* Set Sizes */
    pkfft_fft_set_size(fftR2HC, size);
    pkfft_fft_set_size(fftHC2R, size);

    /* Start the clock */
    struct timespec startClock_;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &startClock_);

    /* Plan the FFT */
    pkfft_fft_plan(fftR2HC);
    pkfft_fft_plan(fftHC2R);

    /* End the clock */
    struct timespec endClock_;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endClock_);

    printf("%e\n", 1.e-9 * (double) (endClock_.tv_nsec - startClock_.tv_nsec));

    /* Start the clock */
    struct timespec startClock;
    clock_gettime(CLOCK_MONOTONIC_RAW, &startClock);

    /* Set the Data for the FFT */
    pkfft_fft_set_data(fftR2HC, data);

    /* Perform the FFT */
    pkfft_fft(fftR2HC);

//    for (size_t i = 0; i < size; i++)
//      {
//        printf("%ld : %g %g\n", i, fftR2HC -> dataIn[i], fftR2HC -> dataOut[i]);
//      }
//
//    printf("\n");

    /* Set the Data for the Inverse FFT */
    pkfft_fft_set_data(fftHC2R, fftR2HC -> dataOut);

    /* Perform the Inverse FFT */
    pkfft_fft(fftHC2R);

//    for (size_t i = 0; i < size; i++)
//      {
//        printf("%ld : %g %g\n", i, fftHC2R -> dataIn[i], fftHC2R -> dataOut[i]);
//      }
//
//    printf("\n");

    /* Compare */
    for (size_t i = 0; i < size; i++)
      {
        diff += fabs(fftHC2R -> dataOut[i] / fftHC2R -> normOut - fftR2HC -> dataIn[i]);
      }

    printf("%e\n", diff);

    /* End the clock */
    struct timespec endClock;
    clock_gettime(CLOCK_MONOTONIC_RAW, &endClock);

    printf("%e\n", 1.e-9 * (double) (endClock.tv_nsec - startClock.tv_nsec));

    /* Free Memory */
    pkfft_fft_free(fftR2HC);
    pkfft_fft_free(fftHC2R);

    return 0;
}


int test_pkutils()
{
    /* Read the Power Spectrum */
    pkfft_pkutils_data_t *dataIn = pkfft_pkutils_data_read_class("pk.dat");

    printf("Input data size : %ld\n", dataIn -> size);

    /* Interpolate the Power Spectrum */
    pkfft_pkutils_spline_t *spline = pkfft_pkutils_spline_alloc();
    pkfft_pkutils_spline_set_data(spline, dataIn);
    pkfft_pkutils_spline_update(spline);

    /* Start the clock */
    struct timespec startClock;
    clock_gettime(CLOCK_MONOTONIC_RAW, &startClock);

    /* Sample the Power Spectrum logarithmically */
    double bias = 0.;
    double lkmin = -4.; // 1.e-4
    double lkmax =  2.; // 1.e+2
    size_t size = 256;

    pkfft_pkutils_data_t *data = pkfft_pkutils_data_alloc();
    pkfft_pkutils_data_sample_log10(data, spline, lkmin, lkmax, size, bias);

    /* End the clock */
    struct timespec endClock;
    clock_gettime(CLOCK_MONOTONIC_RAW, &endClock);

    printf("%e\n", 1.e-9 * (double) (endClock.tv_nsec - startClock.tv_nsec));

    /* Free Memory */
    pkfft_pkutils_data_free(data);
    pkfft_pkutils_spline_free(spline);
    pkfft_pkutils_data_free(dataIn);

    return 0;
}


int test_pk()
{
    /* Read the Power Spectrum */
    pkfft_pkutils_data_t *dataIn = pkfft_pkutils_data_read_class("pk.dat");

    printf("Input data size : %ld\n", dataIn -> size);

    /* Create linear Power Spectrum Struct */
    pkfft_pk_t *pk = pkfft_pk_alloc();

    /* First Update Call Plans the FFT and Performs an FFT for Default Data */
    pkfft_pk_update(pk);

    /* Get the Config Struct */
    pkfft_pk_config_t *config = pkfft_pk_get_config(pk);

    /* Set Input Data */
    pkfft_pk_config_set_data(config, dataIn);

    /* Set (logarithmic) Sampling Parameters */
    pkfft_pk_config_set_bounds(config, -4., 2.); // Sample from 10^{-4} to 10^2
    pkfft_pk_config_set_size(config, 256); // Sample 256 points
    pkfft_pk_config_set_bias(config, -1.6); // Bias the linear Power Spectrum with k^{-1.6}

    /* Start the clock */
    struct timespec startClock;
    clock_gettime(CLOCK_MONOTONIC_RAW, &startClock);

    /* Update the linear Power Spectrum */
    pkfft_pk_update(pk);

    /* End the clock */
    struct timespec endClock;
    clock_gettime(CLOCK_MONOTONIC_RAW, &endClock);

    printf("%e\n", 1.e-9 * (double) (endClock.tv_nsec - startClock.tv_nsec));

    /* Compute some values */
    double k = 0.1;
    double pkfft = pkfft_pk_eval_fft(k, pk);
    double pkspline = pkfft_pk_eval_spline(k, pk);

    printf("%e : %e (%e) -> error : %e\n", k, pkfft, pkspline, fabs(pkspline - pkfft) / pkspline);

    /* Free Memory */
    pkfft_pk_free(pk);
    pkfft_pkutils_data_free(dataIn);

    return 0;
}


int main()
{

    if (TEST_FFT)
      {
        test_fft();
      }

    if (TEST_PKUTILS)
      {
        test_pkutils();
      }

    if (TEST_PK)
      {
        test_pk();
      }

    return 0;
}
