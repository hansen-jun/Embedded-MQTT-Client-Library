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
**  Mbed MQTT Client Library with C
**************************************************************/
/** 
 * @example     wrapper.h
 * @brief       Wrapper Implement for Linux Platform Header
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01 
 *              - 2018/10/29 : zhaozhenge@outlook.com 
 *                  -# New
 * @version     00.00.02 
 *              - 2018/11/08 : zhaozhenge@outlook.com 
 *                  -# Independence the platform data
 * @version     00.00.03 
 *              - 2018/11/30 : zhaozhenge@outlook.com 
 *                  -# Change for MQTT Library
 */
 
/**************************************************************
**  Include
**************************************************************/
 
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

/**************************************************************
**  Symbol
**************************************************************/

#define D_MQC_PRINT     printf
#define malloc_wrapper  malloc
#define free_wrapper    free

/**************************************************************
**  Structure
**************************************************************/

/**
 * @brief      Custom Platform Data
 * @author     zhaozhenge@outlook.com
 * @date       2018/10/29
 */
typedef struct _S_PLATFORM_DATA
{
    int                     SemaphoreId;
    int                     SocketFd;
    char*                   DstAddress;
    uint16_t                DstPort;
}S_PLATFORM_DATA;

/**************************************************************
**  Interface
**************************************************************/

/** 
 * @brief               wrapper function initialize
 * @param[in,out]       Ctx                 User Context
 * @retval              0 for successful
 * @retval              -1 for fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/10/29
 */
extern int32_t wrapper_init(S_PLATFORM_DATA* Ctx);

/** 
 * @brief               wrapper function finalize
 * @param[in,out]       Ctx                 User Context
 * @return              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/10/29
 */
extern void wrapper_deinit(S_PLATFORM_DATA* Ctx);

/** 
 * @brief               Resource Lock function
 * @param[in]           Ctx         Lock Id
 * @retval              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/10/29
 */
extern void lock_wrapper(S_PLATFORM_DATA* Ctx);

/** 
 * @brief               Resource UnLock function
 * @param[in]           Ctx         Lock Id
 * @retval              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/10/29
 */
extern void unlock_wrapper(S_PLATFORM_DATA* Ctx);

/** 
 * @brief               Get the systime in milliseconds
 * @retval              Current time since system boot
 * @author              zhaozhenge@outlook.com
 * @date                2018/10/29
 */
extern uint32_t systick_wrapper(void);

/** 
 * @brief               wrapper Implement for TCP/IP network start
 * @param[in,out]       Ctx                 User Context
 * @retval              0 for successful
 * @retval              -1 for fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 */
extern int32_t network_open_wrapper(S_PLATFORM_DATA* Ctx);

/** 
 * @brief               wrapper Implement for TCP/IP network close
 * @param[in,out]       Ctx                 User Context
 * @return              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 */
extern void network_close_wrapper(S_PLATFORM_DATA* Ctx);

/** 
 * @brief               wrapper Implement for TCP/IP Data Send
 * @param[in,out]       Ctx                 User Context
 * @param[in]           Data                Data want to write via network
 * @param[in]           Size                Size of the Data
 * @return              The size of data actually write success ( < 0 means error)
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/08
 */
extern int32_t tcpwrite_wrapper(S_PLATFORM_DATA* Ctx, const uint8_t* Data, size_t Size);

/** 
 * @brief               wrapper Implement for TCP/IP data receive watching 
 * @param[in,out]       Ctx                 User Context
 * @param[in]           Timeout             Timeout for wait no data
 * @retval              0                   timeout
 * @retval              1                   success
 * @retval              -1                  fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/10/29
 */
extern int32_t tcpcheck_wrapper(S_PLATFORM_DATA* Ctx, uint32_t Timeout);

/** 
 * @brief               wrapper Implement for TCP/IP Data Receive
 * @param[in,out]       Ctx                 User Context
 * @param[in]           Data                Data that read via network
 * @param[in]           Size                Size of the Data that want to read
 * @return              Size that actually read via network
 * @note                -1 maybe returned if process fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/08
 */
extern int32_t tcpread_wrapper(S_PLATFORM_DATA* Ctx, uint8_t* Data, size_t Size);
