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
**  Mbed Common C Library Interface
**************************************************************/
/** 
 * @file        CLIB_api.h
 * @brief       Mbed Common API with C Header
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01
 *              - 2018/10/29 : zhaozhenge@outlook.com
 *                  -# New
 */

#ifndef _CLIB_API_H_
#define _CLIB_API_H_

#ifdef __cplusplus
extern "C" {
#endif
 
/**************************************************************
**  Include
**************************************************************/

#include "CLIB_def.h"

#ifdef CLIB_BASE64_MODULE_ENABLED
#include "CLIB_base64.h"
#endif

#ifdef CLIB_BIT_MODULE_ENABLED
#include "CLIB_bit.h"
#endif

#ifdef CLIB_BUFFER_MODULE_ENABLED
#include "CLIB_buffer.h"
#endif

#ifdef CLIB_HEAP_MODULE_ENABLED
#include "CLIB_heap.h"
#endif

#ifdef CLIB_LIST_MODULE_ENABLED
#include "CLIB_list.h"
#endif

#ifdef CLIB_NET_MODULE_ENABLED
#include "CLIB_net.h"
#endif

#ifdef CLIB_RANDOM_MODULE_ENABLED
#include "CLIB_random.h"
#endif

#ifdef CLIB_RINGBUFFER_MODULE_ENABLED
#include "CLIB_ringbuffer.h"
#endif

#ifdef CLIB_SHA_MODULE_ENABLED
#include "CLIB_sha1.h"
#endif

#ifdef CLIB_STRING_MODULE_ENABLED
#include "CLIB_str.h"
#endif

#ifdef CLIB_TIME_MODULE_ENABLED
#include "CLIB_time.h"
#endif

#ifdef CLIB_TIMER_MODULE_ENABLED
#include "CLIB_timer.h"
#endif

#ifdef CLIB_PRINT_MODULE_ENABLED
#include "CLIB_xprintf.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* _CLIB_API_H_ */
