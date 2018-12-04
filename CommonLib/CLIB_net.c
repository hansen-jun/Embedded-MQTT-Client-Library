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
**  Net API Library with C
**************************************************************/
/** 
 * @file        CLIB_net.c
 * @brief       Net API with C
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01 
 *              - 2018/04/21 : zhaozhenge@outlook.com 
 *                  -# New
 * @version     00.00.02 
 *              - 2018/10/25 : zhaozhenge@outlook.com 
 *                  -# Remove the configuration for Endian Type
 */ 

/**************************************************************
**  Include
**************************************************************/

#include "CLIB_net.h"

#ifdef CLIB_NET_MODULE_ENABLED

/**************************************************************
**  Interface
**************************************************************/

/** 
 * @brief               Host order 16 bit data to Net order 16 bit data
 * @param[in]           Data        Host order 16 bit Data
 * @return              Net order 16 bit data
 * @author              zhaozhenge@outlook.com
 * @date                2018/10/25
 * @callgraph
 * @hidecallergraph
 */
extern uint16_t CLIB_htons(uint16_t Data)
{
    uint16_t    Ret         =   0;
    uint8_t*    ByteArray   =   NULL;
    
    ByteArray = (uint8_t*)&Ret;
    *(ByteArray++) = (uint8_t)(Data >> 8);
    *(ByteArray++) = (uint8_t)(Data);
    
    return Ret;
}

/** 
 * @brief               Host order 32 bit data to Net order 32 bit data
 * @param[in]           Data        Host order 32 bit Data
 * @return              Net order 32 bit data
 * @author              zhaozhenge@outlook.com
 * @date                2018/10/25
 * @callgraph
 * @hidecallergraph
 */
extern uint32_t CLIB_htonl(uint32_t Data)
{
    uint32_t    Ret         =   0;
    uint8_t*    ByteArray   =   NULL;
    
    ByteArray = (uint8_t*)&Ret;
    *(ByteArray++) = (uint8_t)(Data >> 24);
    *(ByteArray++) = (uint8_t)(Data >> 16);
    *(ByteArray++) = (uint8_t)(Data >> 8);
    *(ByteArray++) = (uint8_t)(Data);
    
    return Ret;
}

/** 
 * @brief               Net order 16 bit data to Host order 16 bit data
 * @param[in]           Data        Net order 16 bit Data
 * @return              Host order 16 bit data
 * @author              zhaozhenge@outlook.com
 * @date                2018/10/25
 * @callgraph
 * @hidecallergraph
 */
extern uint16_t CLIB_ntohs(uint16_t Data)
{
    uint16_t    Ret         =   0;
    uint8_t*    ByteArray   =   NULL;
    
    ByteArray = (uint8_t*)&Data;
    Ret = (uint16_t)( ( (uint32_t) ByteArray[0] << 8 ) | ( (uint32_t) ByteArray[1] ) );
    
    return Ret;
}

/** 
 * @brief               Net order 32 bit data to Host order 32 bit data
 * @param[in]           Data        Net order 32 bit Data
 * @return              Host order 32 bit data
 * @author              zhaozhenge@outlook.com
 * @date                2018/10/25
 * @callgraph
 * @hidecallergraph
 */
extern uint32_t CLIB_ntohl(uint32_t Data)
{
    uint16_t    Ret         =   0;
    uint8_t*    ByteArray   =   NULL;
    
    ByteArray = (uint8_t*)&Data;
    Ret = ( (uint32_t) ByteArray[0] << 24 ) | ( (uint32_t) ByteArray[1] << 16 ) | ( (uint32_t) ByteArray[2] << 8 ) | ( (uint32_t) ByteArray[3] );
    
    return Ret;
}

#endif /* CLIB_NET_MODULE_ENABLED */
