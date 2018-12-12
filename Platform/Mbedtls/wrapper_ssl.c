/*
 *  Copyright (C) 2018, ZhaoZhenge, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/**************************************************************
**  Mbed Websocket Client Library with C
**************************************************************/
/** 
 * @example     wrapper_ssl.c
 * @brief       Wrapper Implement for Mbedtls Platform
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01 
 *              - 2018/10/29 : zhaozhenge@outlook.com 
 *                  -# New
 * @version     00.00.02 
 *              - 2018/11/08 : zhaozhenge@outlook.com 
 *                  -# Independence the platform data
 * @version     00.00.03
 *              - 2018/12/10 : zhaozhenge@outlook.com 
 *                  -# Change for MQTT Library
 */

/**************************************************************
**  Include
**************************************************************/

#include <string.h>
#include "wrapper_ssl.h"

/**************************************************************
**  Global Param
**************************************************************/

static const char test_ca_crt_rsa[] =
"-----BEGIN CERTIFICATE-----\r\n"
"MIIC8DCCAlmgAwIBAgIJAOD63PlXjJi8MA0GCSqGSIb3DQEBBQUAMIGQMQswCQYD\r\n"
"VQQGEwJHQjEXMBUGA1UECAwOVW5pdGVkIEtpbmdkb20xDjAMBgNVBAcMBURlcmJ5\r\n"
"MRIwEAYDVQQKDAlNb3NxdWl0dG8xCzAJBgNVBAsMAkNBMRYwFAYDVQQDDA1tb3Nx\r\n"
"dWl0dG8ub3JnMR8wHQYJKoZIhvcNAQkBFhByb2dlckBhdGNob28ub3JnMB4XDTEy\r\n"
"MDYyOTIyMTE1OVoXDTIyMDYyNzIyMTE1OVowgZAxCzAJBgNVBAYTAkdCMRcwFQYD\r\n"
"VQQIDA5Vbml0ZWQgS2luZ2RvbTEOMAwGA1UEBwwFRGVyYnkxEjAQBgNVBAoMCU1v\r\n"
"c3F1aXR0bzELMAkGA1UECwwCQ0ExFjAUBgNVBAMMDW1vc3F1aXR0by5vcmcxHzAd\r\n"
"BgkqhkiG9w0BCQEWEHJvZ2VyQGF0Y2hvby5vcmcwgZ8wDQYJKoZIhvcNAQEBBQAD\r\n"
"gY0AMIGJAoGBAMYkLmX7SqOT/jJCZoQ1NWdCrr/pq47m3xxyXcI+FLEmwbE3R9vM\r\n"
"rE6sRbP2S89pfrCt7iuITXPKycpUcIU0mtcT1OqxGBV2lb6RaOT2gC5pxyGaFJ+h\r\n"
"A+GIbdYKO3JprPxSBoRponZJvDGEZuM3N7p3S/lRoi7G5wG5mvUmaE5RAgMBAAGj\r\n"
"UDBOMB0GA1UdDgQWBBTad2QneVztIPQzRRGj6ZHKqJTv5jAfBgNVHSMEGDAWgBTa\r\n"
"d2QneVztIPQzRRGj6ZHKqJTv5jAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBBQUA\r\n"
"A4GBAAqw1rK4NlRUCUBLhEFUQasjP7xfFqlVbE2cRy0Rs4o3KS0JwzQVBwG85xge\r\n"
"REyPOFdGdhBY2P1FNRy0MDr6xr+D2ZOwxs63dG1nnAnWZg7qwoLgpZ4fESPD3PkA\r\n"
"1ZgKJc2zbSQ9fCPxt2W3mdVav66c6fsb7els2W2Iz7gERJSX\r\n"
"-----END CERTIFICATE-----\r\n";
static const size_t test_ca_crt_rsa_len = sizeof( test_ca_crt_rsa );

static const char test_cli_crt_rsa[] =
"-----BEGIN CERTIFICATE-----\r\n"
"MIIDJjCCAo+gAwIBAgIBADANBgkqhkiG9w0BAQsFADCBkDELMAkGA1UEBhMCR0Ix\r\n"
"FzAVBgNVBAgMDlVuaXRlZCBLaW5nZG9tMQ4wDAYDVQQHDAVEZXJieTESMBAGA1UE\r\n"
"CgwJTW9zcXVpdHRvMQswCQYDVQQLDAJDQTEWMBQGA1UEAwwNbW9zcXVpdHRvLm9y\r\n"
"ZzEfMB0GCSqGSIb3DQEJARYQcm9nZXJAYXRjaG9vLm9yZzAeFw0xODEyMTAwNTQ4\r\n"
"MDhaFw0xOTAzMTAwNTQ4MDhaMIGAMQswCQYDVQQGEwJDTjERMA8GA1UECAwITGlh\r\n"
"b05pbmcxDzANBgNVBAcMBkRhTGlhbjERMA8GA1UECgwIUGVyc29uYWwxEzARBgNV\r\n"
"BAMMClpoYW9aaGVuZ2UxJTAjBgkqhkiG9w0BCQEWFnpoYW96aGVuZ2VAb3V0bG9v\r\n"
"ay5jb20wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDLpFEX+a2G/tsV\r\n"
"SXgII4QBqEg8VVRh9hswkGd0z3H4lhR53LPHyR7SQGd9kmbwY0TTcxFsSJCQVo5j\r\n"
"7vm9ihI2/V8k8sCzNWEzJJCr/2zmEqR2U4NWlw1T5UtcTa2XTNp49ujsdQSPWViT\r\n"
"oi8WBnlWubY/2bPaec4pdoFEYiZbjy3bjsJ991Aa0j48G5Lh+oPblnoTBCQDnW01\r\n"
"QqM1VeNpSiWiHRZzJpuUn7+7czMT0WPSyOO5KyTc1KrVWISE+WXeZDBgLZUAlnHw\r\n"
"Z+qu+w+L1/fs8QJE9hab/JitwRCC9WXMl2UFGhvbI4GUHSAWAHWnDUQZngTowIxg\r\n"
"jvkYIDONAgMBAAGjGjAYMAkGA1UdEwQCMAAwCwYDVR0PBAQDAgXgMA0GCSqGSIb3\r\n"
"DQEBCwUAA4GBADj3z/GuClV/5UramKXDnn8wD5hS+nc8VH/mwW5HBtKtsx9aVYNf\r\n"
"Ht7kvlTaczQZBWZI2ByrSlI/CfYm+hLdp6a45llsziFL8ViDpZN53LA0/0JNkc2k\r\n"
"I61CPHIqiUl35lXFzCkglrRCIdWEo3J1Y0wjtIvN0n6iW1lfPqffVOOT\r\n"
"-----END CERTIFICATE-----\r\n";
static const size_t test_cli_crt_rsa_len = sizeof( test_cli_crt_rsa );

static const char test_cli_key_rsa[] =
"-----BEGIN RSA PRIVATE KEY-----\r\n"
"MIIEogIBAAKCAQEAy6RRF/mthv7bFUl4CCOEAahIPFVUYfYbMJBndM9x+JYUedyz\r\n"
"x8ke0kBnfZJm8GNE03MRbEiQkFaOY+75vYoSNv1fJPLAszVhMySQq/9s5hKkdlOD\r\n"
"VpcNU+VLXE2tl0zaePbo7HUEj1lYk6IvFgZ5Vrm2P9mz2nnOKXaBRGImW48t247C\r\n"
"ffdQGtI+PBuS4fqD25Z6EwQkA51tNUKjNVXjaUoloh0WcyablJ+/u3MzE9Fj0sjj\r\n"
"uSsk3NSq1ViEhPll3mQwYC2VAJZx8GfqrvsPi9f37PECRPYWm/yYrcEQgvVlzJdl\r\n"
"BRob2yOBlB0gFgB1pw1EGZ4E6MCMYI75GCAzjQIDAQABAoIBABzDm/S//qPH0dhC\r\n"
"5WJ46cd2JqXnA6FGqvaEHyjfuGB9imzgIMDChsj58eRJzqJ70xfu8/IQmeLZZwl0\r\n"
"kvAT1ZjQ1ySX6WIm0xDvREQXMyGYVoSkLTQI6AgGpktmaXOqll1puuLTcI5y91D7\r\n"
"Ip2A0pjOaJDCRmZAuD8hDwdCudpKsX57fXbLsz1cRi9yT3HyLt04SlOChkaP/6Zj\r\n"
"d8XX5eF7Q6gRC2JGhFI066vNfJYpyux9+Tc+BmQEsy5mupoJ4+yP5YZpvarfDnpd\r\n"
"T9wruO8PRX9C11McZTsp3Q+CSPTiC5O/zOlIEjJrAnsY/Nv+aOjICMikIPxkZiZa\r\n"
"a99e8yECgYEA8NZoEQhKTEjDZEHgKCQoiJ4EvtKRSmIeo9sunTkTq8w7elcSTdAd\r\n"
"We2Y3j9umrhT7CAuvaPTvXm8VOe7ykwpT2vL9KQTJ2WbWHwtoxk65ZyPAiKorNVQ\r\n"
"+8ax1Y/zRJ8sXNHqztp1zBvo0N6zLCuLUED6QaKQ5YbHY/2Ugcf20CUCgYEA2HZs\r\n"
"6PuFMOx7U3HhZ1xKz2ybEEuuOxVxrBwvAowu3dXwaTd/nNKpDobGVobo2L9QMcA9\r\n"
"IFNA9VZ0T1DeGmlcaK4q/qMKafAjMGxoPFDt9QmGD8xuJjQAWySFMMHXoO/0DZps\r\n"
"2g6Ed7jj/tSRXKwr8lfAKwQQs0slv/gayckLpUkCgYAwAg1sl8J3wmlwqtEML2kF\r\n"
"ErrPbxoXQcfp0d9Q5/fL30Jb+M9lQ38WVg6K/woFPYk9II0wsF+c/Y9VPXxprxTt\r\n"
"CAf4FFzdFZwJ33cZdAYRj0vSmSYWjc8+4TTB5ZgQrcoiFpY/9bxVm5qlSD9fJq72\r\n"
"qH06aDWKC3vjqsDXvgytIQKBgDOUBF9/jz0ZHEnF2O/IDgte1ZQMdHefpHZeQNVa\r\n"
"kdBKKf7oOoAjsiTD+H6lptX+wB0asXYCYHgVIaHEoyen81z+I0CJmPGlRr1DF5NH\r\n"
"tWut2UPqclSoD22oRhRYHAk9I+oZ4dYTn3znkWwnOnvaip40SpX1YgVW4qfbdoae\r\n"
"YSgBAoGAC4YgpBzADYcdzVRcLZGPft2q8VgAqx215b6znTYrSCRWOu5KUoO4jymN\r\n"
"oHK+75mlo+vdr19fJ4uVGlyhM3RQGWMNDQEUo/FGytvZ6ThUGIhbcwtGadVvAuEC\r\n"
"75u4vIgL0q5pczrsxdPEkt568KjKrwUVg5z5570qRmzc4f/tNxc=\r\n"
"-----END RSA PRIVATE KEY-----\r\n";
static const size_t test_cli_key_rsa_len = sizeof( test_cli_key_rsa );

static const mbedtls_x509_crt_profile x509_crt_profile_default =
{
    /* Allow SHA-1 (weak, but still safe in controlled environments) */
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA1 ) |
    /* Only SHA-2 hashes */
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA224 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA256 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA384 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA512 ),
    0xFFFFFFF, /* Any PK alg    */
    0xFFFFFFF, /* Any curve     */
    256,
};

/**************************************************************
**  Function
**************************************************************/

/** 
 * @brief               Random Number get callback function
 * @param[in,out]       Ctx                 User Context for callback
 * @param[out]          Output              Random Number Sequence
 * @param[in]           Len                 The amount of random number
 * @retval              0                   success
 * @retval              -1                  fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/08
 */
static int Rng_ssl_callback(void* Ctx,  unsigned char* Output, size_t Len)
{
    random_wrapper(Output, Len);
    return 0;
}

/** 
 * @brief               SSL layer write callback function
 * @param[in,out]       Ctx                 User Context for callback
 * @param[in]           buf                 Data want to write via network
 * @param[in]           len                 Size of the Data
 * @return              Size that actually write via network
 * @note                -1 maybe returned if process fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/08
 */
static int Tcpwrite_ssl_callback(void* Ctx,  const unsigned char* buf, size_t len)
{
    int                 Err         =   0;
    S_PLATFORM_DATA*    CustomData  =   (S_PLATFORM_DATA*)Ctx;
    
    Err = tcpwrite_wrapper(CustomData, buf, len);
    
    return Err;
}

/** 
 * @brief               SSL layer read callback function
 * @param[in,out]       Ctx                 User Context for callback
 * @param[in]           buf                 Data that read via network
 * @param[in]           len                 Size of the Data that want to read
 * @return              Size that actually read via network
 * @note                -1 maybe returned if process fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/08
 */
static int Tcpread_ssl_callback(void* Ctx, unsigned char* buf, size_t len)
{
    int                 Err         =   0;
    S_PLATFORM_DATA*    CustomData  =   (S_PLATFORM_DATA*)Ctx;
    
    Err = tcpread_wrapper(CustomData, buf, len);
    
    return Err;
}

/** 
 * @brief               SSL layer read callback function ( with timeout )
 * @param[in,out]       Ctx                 User Context for callback
 * @param[in]           buf                 Data that read via network
 * @param[in]           len                 Size of the Data that want to read
 * @param[in]           Timeout             Timeout for wait no data
 * @return              Size that actually read via network
 * @note                -1 maybe returned if process fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/08
 */
static int Tcpreadtimeout_ssl_callback(void* Ctx, unsigned char* buf, size_t len, uint32_t Timeout)
{
    int                 Err         =   0;
    S_PLATFORM_DATA*    CustomData  =   (S_PLATFORM_DATA*)Ctx;
    
    Err = tcpcheck_wrapper(CustomData, Timeout);
    if(0 < Err)
    {
        Err = tcpread_wrapper(CustomData, buf, len);
    }
    
    return Err;
}

/**************************************************************
**  Interface
**************************************************************/

/** 
 * @brief               ssl wrapper function initialize
 * @param[in,out]       Ctx                 User Context
 * @param[in,out]       Handle              Platform Data
 * @retval              0 for successful
 * @retval              -1 for fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/10
 */
extern int32_t ssl_wrapper_init(S_SSL_DATA* Ctx, S_PLATFORM_DATA* Handle)
{
    int Err =   0;

    do
    {
        memset(Ctx, 0x00, sizeof(S_SSL_DATA));

        /* Init configuration */
        mbedtls_ssl_config_init(&(Ctx->SSLConf));
        mbedtls_x509_crt_init(&(Ctx->CaCert));
        mbedtls_x509_crt_init(&(Ctx->ClientCert));
        mbedtls_pk_init(&(Ctx->ClientKey));
        mbedtls_ssl_init(&(Ctx->SSLCtx));
        Err = mbedtls_ssl_config_defaults(&(Ctx->SSLConf), MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
        if( Err )
        {
            D_MQC_PRINT( " failed\n  ! mbedtls_ssl_config_defaults() returned %d\n\n", Err );
            break;
        }
        //mbedtls_ssl_conf_authmode(&(Ctx->SSLConf), MBEDTLS_SSL_VERIFY_NONE);
        //mbedtls_ssl_conf_ca_chain(&(Ctx->SSLConf), NULL, NULL);
        /* set CA certificate */
        mbedtls_ssl_conf_cert_profile(&(Ctx->SSLConf), &x509_crt_profile_default);
        Err = mbedtls_x509_crt_parse(&(Ctx->CaCert), (const unsigned char*)test_ca_crt_rsa, test_ca_crt_rsa_len);
        if(Err)
        {
            D_MQC_PRINT( " failed\n  ! mbedtls_x509_crt_parse() returned %d\n\n", Err );
            break;
        }
        mbedtls_ssl_conf_authmode(&(Ctx->SSLConf), MBEDTLS_SSL_VERIFY_REQUIRED);
        mbedtls_ssl_conf_ca_chain(&(Ctx->SSLConf), &(Ctx->CaCert), NULL);
        /* set client certificate */
        Err = mbedtls_x509_crt_parse(&(Ctx->ClientCert), (const unsigned char*)test_cli_crt_rsa, test_cli_crt_rsa_len);
        if(Err)
        {
            D_MQC_PRINT( " failed\n  ! mbedtls_x509_crt_parse() returned %d\n\n", Err );
            break;
        }
        /* set client private key */
        mbedtls_pk_init(&(Ctx->ClientKey));
        Err = mbedtls_pk_parse_key(&(Ctx->ClientKey), (const unsigned char*)test_cli_key_rsa, test_cli_key_rsa_len, NULL, 0);
        if(Err)
        {
            D_MQC_PRINT( " failed\n  ! mbedtls_pk_parse_key() returned %d\n\n", Err );
            break;
        }
        Err = mbedtls_ssl_conf_own_cert(&(Ctx->SSLConf), &(Ctx->ClientCert), &(Ctx->ClientKey));
        if(Err)
        {
            D_MQC_PRINT( " failed\n  ! mbedtls_ssl_conf_own_cert() returned %d\n\n", Err );
            break;
        }
        /* set configuration */
        mbedtls_ssl_conf_rng(&(Ctx->SSLConf), Rng_ssl_callback, NULL);
        /* Session setup */
        Err = mbedtls_ssl_setup(&(Ctx->SSLCtx), &(Ctx->SSLConf));
        if(Err)
        {
            D_MQC_PRINT( " failed\n  ! mbedtls_ssl_setup() returned %d\n\n", Err );
            break;
        }
        /* Set read/write wrap */
        mbedtls_ssl_set_bio(&(Ctx->SSLCtx), Handle, Tcpwrite_ssl_callback, Tcpread_ssl_callback, Tcpreadtimeout_ssl_callback);
        
        /* SSL reset */
        Err = mbedtls_ssl_session_reset(&(Ctx->SSLCtx));
        if(Err)
        {
            D_MQC_PRINT( " failed\n  ! mbedtls_ssl_session_reset() returned %d\n\n", Err );
            break;
        }
        
        /* Set timeout */
        mbedtls_ssl_conf_read_timeout(&(Ctx->SSLConf), 5000);
        
        /* SSL handshake */
        Err = MBEDTLS_ERR_SSL_WANT_WRITE;
        while( (MBEDTLS_ERR_SSL_WANT_WRITE == Err) || (MBEDTLS_ERR_SSL_WANT_READ == Err) )
        {
            Err = mbedtls_ssl_handshake(&(Ctx->SSLCtx));
        }
        if(Err)
        {
            D_MQC_PRINT( " failed\n  ! mbedtls_ssl_handshake() returned %d\n\n", Err );
            break;
        }
        Err = (0 <= Err)?0:-1;
        
	}while(0);
    
    if( Err )
    {
        /* free SSL */
        mbedtls_ssl_free( &(Ctx->SSLCtx) );
        mbedtls_ssl_config_free( &(Ctx->SSLConf) );
        mbedtls_x509_crt_free(&(Ctx->CaCert));
        mbedtls_x509_crt_free(&(Ctx->ClientCert));
        mbedtls_pk_free(&(Ctx->ClientKey));
    }
    
	return Err;
}

/** 
 * @brief               ssl wrapper function finalize
 * @param[in,out]       Ctx                 User Context
 * @return              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/08
 */
extern void ssl_wrapper_deinit(S_SSL_DATA* Ctx)
{
    mbedtls_ssl_close_notify( &(Ctx->SSLCtx) );
    
    /* free SSL */
    mbedtls_ssl_free( &(Ctx->SSLCtx) );
    mbedtls_ssl_config_free( &(Ctx->SSLConf) );
    mbedtls_x509_crt_free(&(Ctx->CaCert));
    mbedtls_x509_crt_free(&(Ctx->ClientCert));
    mbedtls_pk_free(&(Ctx->ClientKey));
    
    memset(Ctx, 0, sizeof(S_SSL_DATA));
    
    return;
}
 
/** 
 * @brief               wrapper Implement for SSL Session Data Send
 * @param[in,out]       Ctx                 User Context
 * @param[in]           Data                Data want to write via network
 * @param[in]           Size                Size of the Data
 * @return              The size of data actually write success ( < 0 means error)
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/08
 */
extern int32_t sslwrite_wrapper(S_SSL_DATA* Ctx, const uint8_t* Data, size_t Size)
{
    int32_t Err =   0;
    
    /* write over SSL */
    Err = MBEDTLS_ERR_SSL_WANT_WRITE;
    while( (MBEDTLS_ERR_SSL_WANT_WRITE == Err) || (MBEDTLS_ERR_SSL_WANT_READ == Err) )
    {
        Err = mbedtls_ssl_write(&(Ctx->SSLCtx), Data, Size);
    }
    
    return Err;
}

/** 
 * @brief               wrapper Implement for SSL Session Data Receive
 * @param[in,out]       Ctx                 User Context
 * @param[in]           Data                Data that read via network
 * @param[in]           Size                Size of the Data that want to read
 * @param[in]           Timeout             Timeout for wait no data
 * @return              Size that actually read via network(0 means timeout)
 * @note                -1 maybe returned if process fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/08
 */
extern int32_t sslread_wrapper(S_SSL_DATA* Ctx, uint8_t* Data, size_t Size, uint32_t Timeout)
{
    int32_t Err =   0;
    
    mbedtls_ssl_conf_read_timeout(&(Ctx->SSLConf), Timeout);
    
    /* read over SSL */
    Err = MBEDTLS_ERR_SSL_WANT_READ;
    while( (MBEDTLS_ERR_SSL_WANT_WRITE == Err) || (MBEDTLS_ERR_SSL_WANT_READ == Err) )
    {
        Err = mbedtls_ssl_read(&(Ctx->SSLCtx), Data, Size);
    }
    
    if(MBEDTLS_ERR_SSL_TIMEOUT == Err)
    {
        Err = 0;
    }
    
    return Err;
}
