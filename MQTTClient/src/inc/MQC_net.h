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
 * @file        MQC_net.h
 * @brief       MQTT Client Library Net API Header
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01 
 *              - 2018/11/30 : zhaozhenge@outlook.com 
 *                  -# New
 */

#ifndef _MQC_NET_H_
#define _MQC_NET_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************
**  Include
**************************************************************/

#include "../../../CommonLib/CLIB_api.h"

/**************************************************************
**  Interface
**************************************************************/

/** 
 * @brief               Host order 16 bit data to Net order 16 bit data
 * @param[in]           Data        Host order 16 bit Data
 * @return              Net order 16 bit data
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/30
 * @callgraph
 * @callergraph
 */
extern uint16_t MQC_htons(uint16_t Data);

/** 
 * @brief               Host order 32 bit data to Net order 32 bit data
 * @param[in]           Data        Host order 32 bit Data
 * @return              Net order 32 bit data
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/30
 * @callgraph
 * @hidecallergraph
 */
extern uint32_t MQC_htonl(uint32_t Data);

/** 
 * @brief               Net order 16 bit data to Host order 16 bit data
 * @param[in]           Data        Net order 16 bit Data
 * @return              Host order 16 bit data
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/30
 * @callgraph
 * @callergraph
 */
extern uint16_t MQC_ntohs(uint16_t Data);

/** 
 * @brief               Net order 32 bit data to Host order 32 bit data
 * @param[in]           Data        Net order 32 bit Data
 * @return              Host order 32 bit data
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/30
 * @callgraph
 * @callergraph
 */
extern uint32_t MQC_ntohl(uint32_t Data);

#ifdef __cplusplus
}
#endif

#endif /* _MQC_NET_H_ */
