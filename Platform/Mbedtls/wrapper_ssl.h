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
 * @example     wrapper_ssl.h
 * @brief       SSL/TLS wrapper Implement for mbedtls library Header
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01 
 *              - 2018/11/08 : zhaozhenge@outlook.com 
 *                  -# New
 */
 
/**************************************************************
**  Include
**************************************************************/

#if defined(PLATFORM_LINUX)
#include "../Linux/wrapper.h"
#elif defined(PLATFORM_WINDOWS)
#include "../Windows/wrapper.h"
#else
#include "../Lwip/wrapper.h"
#endif

#include "mbedtls/ssl.h"

/**************************************************************
**  Structure
**************************************************************/

/**
 * @brief      Custom SSL Data
 * @author     zhaozhenge@outlook.com
 * @date       2018/11/08
 */
typedef struct _S_SSL_DATA
{
    mbedtls_x509_crt                CaCert;
    mbedtls_x509_crt                ClientCert;
    mbedtls_pk_context              ClientKey;
    mbedtls_ssl_config              SSLConf;
    mbedtls_ssl_context             SSLCtx;
}S_SSL_DATA;

/**************************************************************
**  Interface
**************************************************************/

/** 
 * @brief               wrapper function initialize
 * @param[in,out]       Ctx                 User Context
 * @param[in,out]       Handle              Platform Data
 * @retval              0 for successful
 * @retval              -1 for fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/08
 */
extern int32_t ssl_wrapper_init(S_SSL_DATA* Ctx, S_PLATFORM_DATA* Handle);

/** 
 * @brief               ssl wrapper function finalize
 * @param[in,out]       Ctx                 User Context
 * @return              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/08
 */
extern void ssl_wrapper_deinit(S_SSL_DATA* Ctx);

/** 
 * @brief               wrapper Implement for SSL Session Data Send
 * @param[in,out]       Ctx                 User Context
 * @param[in]           Data                Data want to write via network
 * @param[in]           Size                Size of the Data
 * @return              The size of data actually write success ( < 0 means error)
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/08
 */
extern int32_t sslwrite_wrapper(S_SSL_DATA* Ctx, const uint8_t* Data, size_t Size);

/** 
 * @brief               wrapper Implement for SSL Session Data Receive
 * @param[in,out]       Ctx                 User Context
 * @param[in]           Data                Data that read via network
 * @param[in]           Size                Size of the Data that want to read
 * @param[in]           Timeout             Timeout for wait no data
 * @return              Size that actually read via network
 * @note                -1 maybe returned if process fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/08
 */
extern int32_t sslread_wrapper(S_SSL_DATA* Ctx, uint8_t* Data, size_t Size, uint32_t Timeout);
