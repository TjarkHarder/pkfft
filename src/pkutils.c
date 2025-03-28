#include "pkutils.h"


/* ============================================================================================== */
/* -----------------------------  Store the linear Power Spectrum  ------------------------------ */
/* ============================================================================================== */


/* ---------------------------------------------------------------------------------------------- */
/* ----------------------------------  Keep Track of Changes  ----------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


static pkfft_pkutils_data_change_t *_pkfft_pkutils_data_change_alloc(void)
{
    /*
     * Create a new Change Struct for the Data Struct
     */

    /* Allocate Memory */
    pkfft_pkutils_data_change_t *change = malloc(sizeof(pkfft_pkutils_data_change_t));


    /**  Initialise Elements  **/

    /* Overall Changes */
    change -> version = -1;

    /* Change in the Data Size */
    change -> sizeVersion = -1;

    /* Change in the Data */
    change -> dataVersion = -1;

    return change;
}


static void _pkfft_pkutils_data_change_free(pkfft_pkutils_data_change_t *change)
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
/* --------------------------------  Create / Free Data Struct  --------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


pkfft_pkutils_data_t *pkfft_pkutils_data_alloc(void)
{
    /*
     * Create a new Data Struct
     */

    /* Allocate Memory */
    pkfft_pkutils_data_t *data = malloc(sizeof(pkfft_pkutils_data_t));

    /* Keep Track of Changes */
    data -> change = _pkfft_pkutils_data_change_alloc();

    /* Data */
    data -> size = 0;
    data -> k = NULL;
    data -> pk = NULL;

    return data;
}


void pkfft_pkutils_data_cp(pkfft_pkutils_data_t *data, pkfft_pkutils_data_t *dataCp)
{
    /*
     * Copy a Data Struct
     */

    pkfft_pkutils_data_set(dataCp, data -> k, data -> pk, data -> size);
}


void pkfft_pkutils_data_free(pkfft_pkutils_data_t *data)
{
    /*
     * Free a Data Struct
     */

    /* Check for NULL */
    if (data == NULL) return;

    /* Free Memory */
    _pkfft_pkutils_data_change_free(data -> change);

    free(data -> k);
    free(data -> pk);

    free(data);
}


/* ---------------------------------------------------------------------------------------------- */
/* -------------------------------------  Set Dependencies  ------------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


void pkfft_pkutils_data_set(pkfft_pkutils_data_t *data, double *k, double *pk, size_t size)
{
    /*
     * Set size, k, and pk in the Data Struct
     */

    /* Set Change Flag(s) */
    bool sizeChange = data -> size != size;
    bool dataChange = false;

    /* Update the Size */
    if (sizeChange)
      {
        /* Set the Size */
        data -> size = size;

        /* Need to Reallocate Memory if Sizes are different */
        data -> k = realloc(data -> k, sizeof(double) * data -> size);
        data -> pk = realloc(data -> pk, sizeof(double) * data -> size);
      }

    /* Update the Data */
    for (size_t i = 0; i < data -> size; i++)
      {
        /* Update Change Flag */
        dataChange |= data -> k[i] != k[i] || data -> pk[i] != pk[i];

        /* Copy the Data */
        data -> k[i] = k[i];
        data -> pk[i] = pk[i];
      }

    /* Update Versions */
    data -> change -> sizeVersion += sizeChange;
    data -> change -> dataVersion += dataChange;

    data -> change -> version += sizeChange || dataChange;
}


/* ---------------------------------------------------------------------------------------------- */
/* ------------------------------  Read the linear Power Spectrum  ------------------------------ */
/* ---------------------------------------------------------------------------------------------- */


/* ##############################  Static Function Declarations  ################################ */
static int _pkfft_pkutils_data_read_find_keyword(FILE *fptr, const char *file, const char *keyword);
static int _pkfft_pkutils_data_read_find_number(FILE *fptr, const char *file, size_t *num);
static int _pkfft_pkutils_data_read_find_header_length(FILE *fptr, const char *file, char header, size_t *length);
/* ############################################################################################## */


pkfft_pkutils_data_t *pkfft_pkutils_data_read_class(const char *file)
{
    /*
     * Read the linear Power Spectrum as output by CLASS
     */

    /* Open the file */
    FILE *fptr = fopen(file, "r");

    if (fptr == NULL)
      {
        PKFFT_ERROR("Could not open the file '%s'.\n", file);
      }

    /* Find the length of the Header */
    size_t length;
    PKFFT_CALL(_pkfft_pkutils_data_read_find_header_length(fptr, file, PKFFT_PKUTILS_DATA_READ_CLASS_KEYCHAR_HEADER, &length));

    fclose(fptr);

    /* Find Length of Data */
    size_t size;
    fptr = fopen(file, "r");

    PKFFT_CALL(_pkfft_pkutils_data_read_find_keyword(fptr, file, PKFFT_PKUTILS_DATA_READ_CLASS_KEYWORD_SIZE)); // Find the keyword
    PKFFT_CALL(_pkfft_pkutils_data_read_find_number(fptr, file, &size));

    fclose(fptr);

    /* Skip the Header */
    fptr = fopen(file, "r");

    for (size_t i = 0; i < length; i++)
      {
        fgetc(fptr);
      }

    /* Get the Data */
    pkfft_pkutils_data_t *data = pkfft_pkutils_data_alloc();

    data -> size = size;
    data -> k = malloc(sizeof(double) * size);
    data -> pk = malloc(sizeof(double) * size);

    data -> change -> version = 0; // First change was applied

    for (size_t i = 0; i < size; i++)
      {
        int status = fscanf(fptr, "%lf %lf", &data -> k[i], &data -> pk[i]);
        (void) status;
      }

    /* Close the File */
    fclose(fptr);

    return data;
}


/* ###################################  Static Functions  ####################################### */

static int _pkfft_pkutils_data_read_find_keyword(FILE *fptr, const char *file, const char *keyword)
{
    /*
     * Parse a File until a Keyword is found
     */

    /* Index used to parse the Keyword */
    size_t i = 0;

    /* Store a single Character from the File */
    char c;

    do
      {
        /* Get the next Character */
        c = (char) fgetc(fptr);

        /* Update the Index */
        i = (c != keyword[i]) ? 0 : i + 1;

        /* Keyword was found */
        if (keyword[i] == '\0')
            break;
      }

    while (c != EOF);

    /* Keyword was not found */
    if (c == EOF)
      {
        _PKFFT_ERROR("Could not find the keyword '%s' in the file '%s'.\n", keyword, file);
      }

    return PKFFT_SUCCESS;
}


static int _pkfft_pkutils_data_read_find_number(FILE *fptr, const char *file, size_t *num)
{
    /*
     * Find the first Integer in a File
     */

    /* Initialise the Number */
    *num = 0;

    /* Store a single Character from the File */
    char c;

    do
      {
        /* Get the next Character */
        c = (char) fgetc(fptr);

        /* Update the Number */
        if (isdigit(c))
          {
            *num = *num * 10 + ((size_t) (c - '0'));
          }

        /* Break if Number was changed once */
        else if (*num)
          {
            break;
          }
      }

    while (c != EOF);

    /* Number was not found */
    if (c == EOF)
      {
        _PKFFT_ERROR("Could not find a number in the file '%s'.\n", file);
      }

    return PKFFT_SUCCESS;
}


static int _pkfft_pkutils_data_read_find_header_length(FILE *fptr, const char *file, char header, size_t *length)
{
    /*
     * Find the length of the Header
     */

    (void) file;

    /* Initialise the Length */
    *length = 0;

    /* Store a single Character from the File */
    char c;

    /* Check for the Header char */
    bool check = true;

    do
      {
        /* Get the next Character */
        c = (char) fgetc(fptr);

        /* Increment the Length */
        *length += 1;

        /* Line Break */
        if (c == '\n' || c == '\r')
          {
            check = true; // Can check first char again

            continue;
          }

        /* Only check first char of line */
        if (check)
          {
            /* Successful check -> no need to check rest of line */
            if (c == header)
              {
                check = false;
                continue;
              }

            /* Uncuccessful check -> found the end of the header */
            *length -= 1;
            break;
          }
      }

    while (c != EOF);

    return PKFFT_SUCCESS;
}

/* ############################################################################################## */



/* ============================================================================================== */
/* --------------------------  Interpolate the linear Power Spectrum  --------------------------- */
/* ============================================================================================== */


/* ---------------------------------------------------------------------------------------------- */
/* ----------------------------------  Keep Track of Changes  ----------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


static pkfft_pkutils_spline_change_t *_pkfft_pkutils_spline_change_alloc(void)
{
    /*
     * Create a new Change Struct for the Spline Struct
     */

    /* Allocate Memory */
    pkfft_pkutils_spline_change_t *change = malloc(sizeof(pkfft_pkutils_spline_change_t));


    /**  Initialise Elements  **/

    /* Overall Changes */
    change -> version = -1;

    /* Change in the Data */
    change -> dataChange = _pkfft_pkutils_data_change_alloc();

    return change;
}


static void _pkfft_pkutils_spline_change_free(pkfft_pkutils_spline_change_t *change)
{
    /*
     * Free a Change Struct for the Spline Struct
     */

    /* Check for NULL */
    if (change == NULL) return;

    /* Free Changes in the Data */
    _pkfft_pkutils_data_change_free(change -> dataChange);

    /* Free the Struct itself */
    free(change);
}


/* ---------------------------------------------------------------------------------------------- */
/* -------------------------------  Create / Free Spline Struct  -------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


pkfft_pkutils_spline_t *pkfft_pkutils_spline_alloc(void)
{
    /*
     * Create a new Spline Struct
     */

    /* Allocate Memory */
    pkfft_pkutils_spline_t *spline = malloc(sizeof(pkfft_pkutils_spline_t));

    /* Keep Track of Changes */
    spline -> change = _pkfft_pkutils_spline_change_alloc();

    /* Data */
    spline -> data = pkfft_pkutils_data_alloc();

    /* Extrapolation Parameters */
    spline -> ns = calloc(2, sizeof(double));
    spline -> as = calloc(2, sizeof(double));

    /* Spline */
    spline -> spline = NULL;
    spline -> accel = gsl_interp_accel_alloc();

    return spline;
}


void pkfft_pkutils_spline_cp(pkfft_pkutils_spline_t *spline, pkfft_pkutils_spline_t *splineCp)
{
    /*
     * Copy a Spline Struct
     */

    /* Copy Data */
    pkfft_pkutils_data_cp(spline -> data, splineCp -> data);

    /* Update */
    pkfft_pkutils_spline_update(splineCp);
}


void pkfft_pkutils_spline_free(pkfft_pkutils_spline_t *spline)
{
    /*
     * Free a Spline Struct
     */

    /* Check for NULL */
    if (spline == NULL) return;

    /* Free Change Struct */
    _pkfft_pkutils_spline_change_free(spline -> change);

    /* Free Data */
    pkfft_pkutils_data_free(spline -> data);

    /* Free Extrapolation Parameters */
    free(spline -> as);
    free(spline -> ns);

    /* Free Spline */
    gsl_interp_accel_free(spline -> accel);
    gsl_spline_free(spline -> spline);

    /* Free Spline Struct */
    free(spline);
}


/* ---------------------------------------------------------------------------------------------- */
/* -------------------------------------  Set Dependencies  ------------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


void pkfft_pkutils_spline_set_data(pkfft_pkutils_spline_t *spline, pkfft_pkutils_data_t *data)
{
    /*
     * Set the Data Struct in the Spline Struct
     */

    pkfft_pkutils_data_cp(data, spline -> data);
}


/* ---------------------------------------------------------------------------------------------- */
/* -------------------------------------  Get Dependencies  ------------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


pkfft_pkutils_data_t *pkfft_pkutils_spline_get_data(pkfft_pkutils_spline_t *spline)
{
    /*
     * Get the Data Struct from the Spline Struct
     */

    return spline -> data;
}


/* ---------------------------------------------------------------------------------------------- */
/* -------------------------------------  Update the Spline  ------------------------------------ */
/* ---------------------------------------------------------------------------------------------- */


void pkfft_pkutils_spline_update(pkfft_pkutils_spline_t *spline)
{
    /*
     * Update the Spline Struct
     */

    /* Change Struct */
    pkfft_pkutils_spline_change_t *change = spline -> change;

    /* Data Struct */
    pkfft_pkutils_data_t *data = spline -> data;

    /* Spline is up to Date */
    if (change -> dataChange -> version == data -> change -> version) return;

    /* Size Changed */
    if (change -> dataChange -> sizeVersion != data -> change -> sizeVersion)
      {
        /* Need to Reallocate Memory */
        gsl_spline_free(spline -> spline);
        spline -> spline = gsl_spline_alloc(gsl_interp_cspline, data -> size);
      }

    /* Extrapolation Parameters */
    spline -> ns[0] = (data -> pk[0] == 0.) ? 0. : (log(data -> pk[1] / data -> pk[0])
                                                    / log(data -> k[1] / data -> k[0])); // d log P(k) / d log k
    spline -> as[0] = (data -> pk[0] == 0.) ? 0. : data -> pk[0] * pow(data -> k[0], -spline -> ns[0]); // P(k) / k^ns

    spline -> ns[1] = (data -> pk[data -> size - 1] == 0.) ? 0. : (log(data -> pk[data -> size - 1] / data -> pk[data -> size - 2])
                                                                    / log(data -> k[data -> size - 1] / data -> k[data -> size - 2])); // d log P(k) / d log k
    spline -> as[1] = (data -> pk[data -> size - 1] == 0.) ? 0. : data -> pk[data -> size - 1] * pow(data -> k[data -> size - 1], -spline -> ns[1]); // P(k) / k^ns

    /* Interpolate the Power Spectrum */
    gsl_spline_init(spline -> spline, data -> k, data -> pk, data -> size);

    /* Update Versions */
    change -> dataChange -> sizeVersion = data -> change -> sizeVersion;
    change -> dataChange -> dataVersion = data -> change -> dataVersion;
    change -> dataChange -> version = data -> change -> version;

    spline -> change -> version++;
}


/* ---------------------------------------------------------------------------------------------- */
/* ------------------------------------  Evaluate the Spline  ----------------------------------- */
/* ---------------------------------------------------------------------------------------------- */


double pkfft_pkutils_spline_eval(double k, pkfft_pkutils_spline_t *spline)
{
    /*
     * Evaluate the Spline
     */

    if (k < spline -> data -> k[0])
      {
        return spline -> as[0] * pow(k, spline -> ns[0]);
      }

    else if (k > spline -> data -> k[spline -> data -> size - 1])
      {
        return spline -> as[1] * pow(k, spline -> ns[1]);
      }

    return gsl_spline_eval(spline -> spline, k, spline -> accel);
}


/* ============================================================================================== */
/* -----------------------------  Sample the linear Power Spectrum  ----------------------------- */
/* ============================================================================================== */


/* ---------------------------------------------------------------------------------------------- */
/* ---------------------  Sample the linear Power Spectrum Logarithmically  --------------------- */
/* ---------------------------------------------------------------------------------------------- */


void pkfft_pkutils_data_sample_log10(pkfft_pkutils_data_t *data, pkfft_pkutils_spline_t *spline, double l10kmin, double l10kmax, size_t size, double bias)
{
    /*
     * Sample and Bias the linear Power Spectrum linearly in log10 Space
     */

    /* Check Input */
    if (l10kmin > l10kmax)
      {
        double buf = l10kmin;
        l10kmin = l10kmax;
        l10kmax = buf;
      }

    if (size == 0 || (size == 1 && l10kmin != l10kmax))
      {
        PKFFT_ERROR("Cannot sample Pk from log(kmin) = %e to log(kmax) = %e in %ld steps.\n", l10kmin, l10kmax, size);
      }

    /* Set Change Flags */
    bool sizeChange = data -> size != size;
    bool dataChange = false;

    /* Need to Reallocate Memory if Sizes are different */
    if (sizeChange)
      {
        data -> size = size;
        data -> k = realloc(data -> k, sizeof(double) * data -> size);
        data -> pk = realloc(data -> pk, sizeof(double) * data -> size);
      }

    /* Sample the linear Power Spectrum */
    for (size_t i = 0; i < data -> size; i++)
      {
        /* log_{10}(k) + k */
        double lk = l10kmin + (l10kmax - l10kmin) / ((double) (size - 1)) * ((double) i);
        double k = pow(10., lk);

        /* P(k) * k^{-\nu} */
        double pk = pkfft_pkutils_spline_eval(k, spline) * pow(10., -bias * lk);

        /* Track Changes */
        dataChange |= data -> k[i] != k || data -> pk[i] != pk; // Change occurs when Data is different

        /* Store the Data */
        data -> k[i] = k;
        data -> pk[i] = pk;
      }

    /* Update Versions */
    data -> change -> sizeVersion += sizeChange;
    data -> change -> dataVersion += dataChange;

    data -> change -> version += sizeChange || dataChange;
}
