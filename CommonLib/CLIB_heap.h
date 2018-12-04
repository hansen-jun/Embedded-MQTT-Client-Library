/*
 * Copyright (C) 2018, ZhaoZhenge,  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *  This file is copyed from FreeRTOS (http://www.FreeRTOS.org) (http://aws.amazon.com/freertos) 
 *  and only changed some functions and variable names 
 */

/**************************************************************
**  Heap Manage Library with C
**************************************************************/
/** 
 * @file        CLIB_heap.h
 * @brief       Heap Memory Management API with C Header
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01 
 *              - 2018/05/04 : zhaozhenge@outlook.com 
 *                  -# New
 */

#ifndef _CLIB_HEAP_H_
#define _CLIB_HEAP_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************
**  Include
**************************************************************/

#include <string.h>
#include "CLIB_def.h"

/**************************************************************
**  Structure
**************************************************************/

/** 
 *  @brief      The linked list structure 
 *  @details    This is used to link free blocks in order of their memory address
 *  @author     zhaozhenge@outlook.com
 *  @date       2018/04/12
 */
typedef struct _S_BLOCK_LINK
{
    struct _S_BLOCK_LINK*   NextFreeBlock;              /*!< The next free block in the list. */
    size_t                  BlockSize;                  /*!< The size of the free block. */
} S_BLOCK_LINK;

/** 
 *  @brief      The Heap Handle structure
 *  @author     zhaozhenge@outlook.com
 *  @date       2018/04/12
 */
typedef struct _S_HEAP_HANDLE
{
    uint8_t*                m_prvHeap;                  /*!< Buffer used to create the Heap. */
    size_t                  m_prvTotalHeapSize;         /*!< The size of the Heap buffer. */
    
    S_BLOCK_LINK            m_StartBlock;               /*!< one in a couple of list links to mark the start of the list */
    S_BLOCK_LINK*           m_EndBlockPtr;              /*!< one in a couple of list links to mark the end of the list */
    
    size_t                  m_prvHeapStructSize;        /*!< The size of the structure placed at the beginning of each allocated memory block */
    size_t                  m_PortByteAlignment;        /*!< Byte Alignment */
    size_t                  m_PortByteAlignmentMask;    /*!< Byte Alignment Mask */
    
    size_t                  m_FreeBytesRemaining;       /*!< Keeps track of the number of free bytes remaining, but says nothing about fragmentation. */
    size_t                  m_MinimumEverFreeBytesRemaining;
    
    size_t                  m_BlockAllocatedBit;        /*!< Gets set to the top bit of an size_t type.  When this bit in the BlockSize
                                                         *   member of an S_BLOCK_LINK structure is set then the block belongs to the
                                                         *   application.  When the bit is free the block is still part of the free heap
                                                         *   space. 
                                                         */
    
    F_CLIB_LOCKFUNC         LockFunc;                   /*!< Lock callback function */
    F_CLIB_UNLOCKFUNC       UnlockFunc;                 /*!< Unlock callback function */
    void*                   UsrData;                    /*!< UserData used for callback function */
} S_HEAP_HANDLE;

/**************************************************************
**  Interface
**************************************************************/

/**
 * @addtogroup  heap_memory_module
 * @ingroup     mbed_c_library
 * @brief       Heap Memory Management API
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01
 *              - 2018/05/04 : zhaozhenge@outlook.com
 *                  -# New
 * @{ 
 */

/** 
 * @brief               create a memory heap
 * @param[in,out]       Heap                Heap information
 * @param[in]           Memory              Buffer used to create the Heap
 * @param[in]           HeapSize            The size of the Heap buffer
 * @param[in]           PortByteAlignment   Byte Alignment, only can be 32 or 16 or 8
 * @param[in]           UserData            User Data used for callback function 
 * @param[in]           Lock                Resource Lock callback function
 * @param[in]           Unlock              Resource Unlock callback function
 * @retval              0                   success
 * @retval              -1                  fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/04/12
 * @callgraph
 * @hidecallergraph
 */
extern int32_t CLIB_heap_create(S_HEAP_HANDLE* Heap, uint8_t* Memory, size_t HeapSize, 
                                size_t PortByteAlignment, void* UserData, 
                                F_CLIB_LOCKFUNC Lock, F_CLIB_UNLOCKFUNC Unlock);

/** 
 * @brief               Memory allocate function
 * @param[in,out]       Heap                Heap information
 * @param[in]           WantedSize          Memory size that want to allocate
 * @return              Memory address allocated successfully
 * @note                NULL maybe returned when no memory enough 
 * @author              zhaozhenge@outlook.com
 * @date                2018/04/12
 * @callgraph
 * @hidecallergraph
 */
extern void* CLIB_heap_malloc(S_HEAP_HANDLE* Heap, size_t WantedSize);

/** 
 * @brief               Memory free function
 * @param[in,out]       Heap                Heap information
 * @param[in]           Pv                  Memory address that allocated from the heap handle
 * @return              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/04/12
 * @hidecallgraph
 * @hidecallergraph
 */
extern void CLIB_heap_free(S_HEAP_HANDLE* Heap, void* Pv);

/** 
 * @brief               Get the remain memory of the heap
 * @param[in]           Heap                Heap information
 * @return              Remain memory of the heap
 * @author              zhaozhenge@outlook.com
 * @date                2018/04/12
 * @callgraph
 * @hidecallergraph
 */
extern size_t CLIB_heap_GetFreeHeapSize(S_HEAP_HANDLE* Heap);

/** 
 * @brief               Get the minimum size ever free back to the heap
 * @param[in]           Heap                Heap information
 * @return              The minimum size ever free back to the heap
 * @author              zhaozhenge@outlook.com
 * @date                2018/04/12
 * @callgraph
 * @hidecallergraph
 */
extern size_t CLIB_heap_GetMinimumEverFreeHeapSize( S_HEAP_HANDLE* Heap );

/** 
 * @brief               delete a memory heap
 * @param[in,out]       Heap                Heap information
 * @retval              0                   success
 * @retval              -1                  fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/04/12
 * @callgraph
 * @hidecallergraph
 */
extern int32_t CLIB_heap_delete( S_HEAP_HANDLE* Heap );

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* _CLIB_HEAP_H_ */
