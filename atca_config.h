/* Auto-generated config file atca_config.h */
#ifndef ATCA_CONFIG_H
#define ATCA_CONFIG_H

/* Included HALS */

#define ATCA_HAL_I2C

/* Included device support */
#define ATCA_ATECC608_SUPPORT

/** Device Override - Library Assumes ATECC608B support in checks */
#define ATCA_ATECC608A_SUPPORT

/* \brief How long to wait after an initial wake failure for the POST to
 *         complete.
 * If Power-on self test (POST) is enabled, the self test will run on waking
 * from sleep or during power-on, which delays the wake reply.
 */
#ifndef ATCA_POST_DELAY_MSEC
#define ATCA_POST_DELAY_MSEC 25
#endif

/***************** Diagnostic & Test Configuration Section *****************/

/** Enable debug messages */
#define ATCA_PRINTF


/******************** Features Configuration Section ***********************/

/** Define certificate templates to be supported. */
#define ATCA_TNGTLS_SUPPORT


/** Define Software Crypto Library to Use - if none are defined use the
    cryptoauthlib version where applicable */

#define ATCA_OPENSSL

/** Define to build atcab_ functions rather that defining them as macros */
#define ATCA_USE_ATCAB_FUNCTIONS

/******************** Platform Configuration Section ***********************/


/** Define platform malloc/free */
#define ATCA_PLATFORM_MALLOC    malloc
#define ATCA_PLATFORM_FREE      free

#define atca_delay_ms   hal_delay_ms
#define atca_delay_us   hal_delay_us

#endif // ATCA_CONFIG_H
