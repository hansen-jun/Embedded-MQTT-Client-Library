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
**  Mbed Common C Library Define
**************************************************************/
/** 
 * @file        CLIB_def.h
 * @brief       Mbed Common Data Define with C Header
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01
 *              - 2018/07/20 : zhaozhenge@outlook.com
 *                  -# New
 * @version     00.00.02
 *              - 2018/10/29 : zhaozhenge@outlook.com
 *                  -# Add configuration of sub module
 */

#ifndef _CLIB_DEF_H_
#define _CLIB_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif
 
/**************************************************************
**  Include
**************************************************************/

#include <stdint.h>
#include <stddef.h>

/**************************************************************
**  Symbol
**************************************************************/

//#define CLIB_BASE64_MODULE_ENABLED
#define CLIB_BIT_MODULE_ENABLED
//#define CLIB_BUFFER_MODULE_ENABLED
#define CLIB_HEAP_MODULE_ENABLED
#define CLIB_LIST_MODULE_ENABLED
#define CLIB_NET_MODULE_ENABLED
//#define CLIB_RANDOM_MODULE_ENABLED
//#define CLIB_RINGBUFFER_MODULE_ENABLED
//#define CLIB_SHA_MODULE_ENABLED
//#define CLIB_STRING_MODULE_ENABLED
//#define CLIB_TIME_MODULE_ENABLED
//#define CLIB_TIMER_MODULE_ENABLED
//#define CLIB_PRINT_MODULE_ENABLED

#define CLIB_ALIGNBYTES             (sizeof(void *))
#define CLIB_ALIGN(p,alignbytes)    ((((size_t)p+alignbytes-1)&~(alignbytes-1)))

/**************************************************************
**  Callback function
**************************************************************/

/**
 * @addtogroup  mbed_c_library
 * @brief       Mbed Common Library with C
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01
 *              - 2018/07/20 : zhaozhenge@outlook.com
 *                  -# New
 * @{ 
 */

/**
 *  @brief          memory allocate callback function
 *  @param[in]      Size            Memory size that want to allocated
 *  @return         Memory address that allocated successfully
 */
typedef void* (*F_CLIB_MALLOCFUNC) (size_t Size);

/**
 *  @brief          memory free callback function
 *  @param[in]      Ptr             memory pointer that want to free
 *  @return         None
 */
typedef void  (*F_CLIB_FREEFUNC)   (void* Ptr);

/**
 *  @brief          Lock resource callback function
 *  @param[in]      CustomData      Custom Data defined by user
 *  @return         None
 */
typedef void (*F_CLIB_LOCKFUNC)    (void* CustomData);

/**
 *  @brief          UnLock resource callback function
 *  @param[in]      CustomData      Custom Data defined by user
 *  @return         None
 */
typedef void (*F_CLIB_UNLOCKFUNC)  (void* CustomData);

/**
 *  @brief          memory allocator
 *  @param[in]      Size            Memory size that want to allocated
 *  @return         Memory address that allocated successfully
 */
typedef void*(CLIB_allocator)(size_t Size);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* _CLIB_DEF_H_ */
