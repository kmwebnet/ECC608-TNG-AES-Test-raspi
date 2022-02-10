#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "cryptoauthlib.h"
#include "tng_atcacert_client.h"
#include "hal/atca_hal.h"

#include "host/atca_host.h"


#define AES_CONFIG_ENABLE_BIT_MASK   (uint8_t)0x01


typedef struct
{
    uint32_t text_size;
    uint8_t  plaintext[32];
    uint8_t  ciphertext[32];
    uint32_t aad_size;
    uint8_t  aad[32];
    uint8_t  tag[16];
} aes_gcm_partial_block_vectors;

const aes_gcm_partial_block_vectors test_vector =
{
    16,
    { 0x9f, 0xee, 0xbb, 0xdf, 0x16, 0x0f, 0x96, 0x52, 0x53, 0xd9, 0x99, 0x58, 0xcc, 0xb1, 0x76, 0xdf },
    { 0xA6, 0x97, 0x10, 0x3A, 0x70, 0x29, 0x7A, 0xAA, 0xCD, 0x25, 0x9E, 0x1A, 0x85, 0x36, 0xA7, 0xDC },
    16,
    { 0x47, 0x6b, 0x48, 0x80, 0xf5, 0x93, 0x33, 0x14, 0xdc, 0xc2, 0x3d, 0xf5, 0xdc, 0xb0, 0x09, 0x66 },
//    { 0x48, 0x6b, 0x48, 0x80, 0xf5, 0x93, 0x33, 0x14, 0xdc, 0xc2, 0x3d, 0xf5, 0xdc, 0xb0, 0x09, 0x66 },
    { 0xE8, 0x8C, 0x95, 0x9A, 0xBC, 0x1E, 0x75, 0x93, 0xA0, 0x3E, 0xF0, 0x34, 0x84, 0x64, 0xF2, 0xD5 }
};

ATCA_STATUS test_aes_enabled(void)
{
    ATCA_STATUS status;
    uint8_t aes_enable;

    status = atcab_read_bytes_zone(ATCA_ZONE_CONFIG, 0, 13, &aes_enable, 1);
    if ((aes_enable & AES_CONFIG_ENABLE_BIT_MASK) == 0)
    {
        printf("AES is not enabled in config zone");
        return ATCA_BAD_PARAM;
    }
    return status;
}


void aestest (void)
{
    ATCA_STATUS status;
    uint16_t key_id = 9; // slot 9 is for AES key
    uint8_t aes_key_block = 0;
    uint8_t ciphertext[32];
    uint8_t plaintext[32];
    uint8_t tag[AES_DATA_SIZE];
    bool is_verified;
    atca_aes_gcm_ctx_t ctx;
    uint8_t key[] = { 0xb7, 0xcf, 0x6c, 0xf5, 0xe7, 0xf3, 0xca, 0x22, 0x3c, 0xa7, 0x3c, 0x81, 0x9d, 0xcd, 0x62, 0xfe,
                    0xb7, 0xcf, 0x6c, 0xf5, 0xe7, 0xf3, 0xca, 0x22, 0x3c, 0xa7, 0x3c, 0x81, 0x9d, 0xcd, 0x62, 0xfe };
    uint8_t iv[] = { 0xa4, 0x13, 0x60, 0x09, 0xc0, 0xa7, 0xfd, 0xac, 0xfe, 0x53, 0xf5, 0x07 };

    printf ("AES Encrypt test.\n");

    // load AES key to Slot
    
    status = atcab_write_bytes_zone(ATCA_ZONE_DATA, key_id, 0, key, sizeof(key));
    if (status != ATCA_SUCCESS)
    {
        printf ("load AES Key to slot failed:%d\n", status);
        return;
    }
    printf( "Key loaded.\n" );
    


    //Initialize gcm ctx with IV
    status = atcab_aes_gcm_init(&ctx, key_id, aes_key_block, iv, sizeof(iv));
    if (status != ATCA_SUCCESS)
    {
        printf ("atcab_aes_gcm_init failed\n");
        return;
    }
    //Add aad to gcm
    status = atcab_aes_gcm_aad_update(&ctx, test_vector.aad, test_vector.aad_size);
    if (status != ATCA_SUCCESS)
    {
        printf ("atcab_aes_gcm_aad_update failed\n");
        return;
    }
    //Encrypt data
    status = atcab_aes_gcm_encrypt_update(&ctx, test_vector.plaintext, test_vector.text_size, ciphertext);
    if (status != ATCA_SUCCESS)
    {
        printf ("atcab_aes_gcm_encrypt_update failed\n");
        return;
    }
    //Calculate authentication tag
    status = atcab_aes_gcm_encrypt_finish(&ctx, tag, sizeof(tag));
    if (status != ATCA_SUCCESS)
    {
        printf ("atcab_aes_gcm_encrypt_finish failed\n");
        return;
    }

    printf("result of ciphertext:\n");
    for (int i = 0; i < 1; i++){
        for(int j = 0; j < 16; j++){
	        printf("0x%02x ", ciphertext[i * 16 + j]);
        }
	    printf("\n");
    }

    if (memcmp(&test_vector.ciphertext, ciphertext, test_vector.text_size) == 0)
    {
        printf ("Results are match as expected.\n");
    }
    else
    {
        printf ("Results are not match.\n");
    }

    printf("result of tag:\n");
    for (int i = 0; i < 1; i++){
        for(int j = 0; j < 16; j++){
	        printf("0x%02x ", tag[i * 16 + j]);
        }
	    printf("\n");
    }

    if (memcmp(&test_vector.tag, tag, sizeof(tag)) == 0)
    {
        printf ("Results are match as expected.\n");
    }
    else
    {
        printf ("Results are not match.\n");
    }

    printf ("AES Decrypt test.\n");

    //Initialize gcm ctx with IV
    status = atcab_aes_gcm_init(&ctx, key_id, aes_key_block, iv, sizeof(iv));
    if (status != ATCA_SUCCESS)
    {
        printf ("atcab_aes_gcm_init failed\n");
        return;
    }
    //Add aad to gcm
    status = atcab_aes_gcm_aad_update(&ctx, test_vector.aad, test_vector.aad_size);
    if (status != ATCA_SUCCESS)
    {
        printf ("atcab_aes_gcm_aad_update failed\n");
        return;
    }
    //Encrypt data
    status = atcab_aes_gcm_decrypt_update(&ctx, test_vector.ciphertext, test_vector.text_size, plaintext);
    if (status != ATCA_SUCCESS)
    {
        printf ("atcab_aes_gcm_decrypt_update failed\n");
        return;
    }
    //Calculate authentication tag
    status = atcab_aes_gcm_decrypt_finish(&ctx, test_vector.tag, sizeof(test_vector.tag), &is_verified);
    if (status != ATCA_SUCCESS)
    {
        printf ("atcab_aes_gcm_decrypt_finish failed\n");
        return;
    }

    printf("result of plaintext:\n");
    for (int i = 0; i < 1; i++){
        for(int j = 0; j < 16; j++){
	        printf("0x%02x ", plaintext[i * 16 + j]);
        }
	    printf("\n");
    }

    if (memcmp(&test_vector.plaintext, plaintext, test_vector.text_size) == 0)
    {
        printf ("Results are match as expected.\n");
    }
    else
    {
        printf ("Results are not match.\n");
    }

    if (is_verified)
    {
        printf ("Verify success.\n");
    }
    else
    {
        printf ("Verify failed.\n");
    }

    return;
}



void get_atecc608cfg(ATCAIfaceCfg *cfg)
{
//	config for Raspberry pi
		cfg->iface_type                     = ATCA_I2C_IFACE;
                cfg->devtype                = ATECC608A;
                cfg->atcai2c.address        = 0X6A;
                cfg->atcai2c.bus            = 1;
                cfg->atcai2c.baud           = 100000;
                cfg->wake_delay             = 1500;
                cfg->rx_retries             = 20;

return;
}

int main(int argc, char *argv[])
{

    ATCAIfaceCfg cfg;

    get_atecc608cfg(&cfg);

    ATCA_STATUS status = atcab_init(&cfg);
	
    if (status != ATCA_SUCCESS) {
        fprintf(stderr, "atcab_init() failed with ret=0x%08d\n", status);
        exit(1);
    }

    uint8_t serial[ATCA_SERIAL_NUM_SIZE];
    status = atcab_read_serial_number(serial);

    if (status != ATCA_SUCCESS) {
	printf( "atcab_read_serial_number() failed with ret=0x%08d/r/n", status);
    }

    
    uint8_t revision[INFO_SIZE];
    status = atcab_info(revision);

    if (status != ATCA_SUCCESS) {
	printf( "atcab_read_serial_number() failed with ret=0x%08d/r/n", status);
    }


    uint8_t config_data[ATCA_ECC_CONFIG_SIZE];
    status = atcab_read_config_zone(config_data);

    if (status != ATCA_SUCCESS) {
	printf( "atcab_read_config_zone() failed with ret=0x%08d/r/n", status);
    }
	

	printf( "Serial Number:");

    char serialstr[18] = {};
    char character[3] = {};

	for ( int i = 0; i< 9; i++){
        sprintf(character , "%02x", serial[i]);
	    strcat(serialstr, character);
        }

    printf(serialstr);
    printf("\n");

	printf( "Revision Number:");

	for ( int i =0; i< 4; i++){
	    printf("%02x ", revision[i]);
        }

    printf("\n");
	printf( "Config Zone data:\n");

        for (int i = 0; i < 16; i++){
           for(int j = 0; j < 8; j++){
	     printf("%02x ", config_data[i * 8 + j]);
           }
	   printf("\n");
        }


    /* aes test */

    if (test_aes_enabled() != ATCA_SUCCESS)
    {
        printf ("AES test ignored. some error occured.");
    } 
    else
    {
        printf ("\n\n\nAES test start.\n\n\n");

        aestest();

    }

    // write to stdout
    fflush(stdout);

	atcab_release();

    return (0);
}



