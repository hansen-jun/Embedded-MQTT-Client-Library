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
 * @file        MQC_config.h
 * @brief       MQTT Client Library Configuration Header
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01 
 *              - 2018/04/11 : zhaozhenge@outlook.com 
 *                  -# New
 */

#ifndef _MQC_CONFIG_H_
#define _MQC_CONFIG_H_

/**
 * @addtogroup  mbed_mqtt_library
 * @{ 
 */
 
/**
 * @name Library Configuration
 * @{
 */
 
/**********************************************************//**
**  @def MQC_EXTERN
**  
**  Define the extern function type of the library
**  
**  For example, in Linux or RTOS it may be "extern" , 
**  in Windows it may be "__declspec(dllexport)"
**************************************************************/
#define MQC_EXTERN extern

/**********************************************************//**
**  @def MQC_NET_API
**  
**  Enable use the htons(l) and ntohs(l) API in MQTT Client Library.
**  User may use their own net library when disable this feature.
**
**  Require the net API wrapper when comment this macro. \n
**  
**  Comment this macro to use Net API wrapper
**************************************************************/
#define MQC_NET_API

#if !defined (MQC_NET_API)
#define MQC_htons(a)                MQC_Wrap_htons(a)
#define MQC_htonl(a)                MQC_Wrap_htonl(a)
#define MQC_ntohs(a)                MQC_Wrap_ntohs(a)
#define MQC_ntohl(a)                MQC_Wrap_ntohl(a)
#endif  /* MQC_NET_API */

/**
 * @}
 */

/**
 * @}
 */

#endif /* _MQC_CONFIG_H_ */
