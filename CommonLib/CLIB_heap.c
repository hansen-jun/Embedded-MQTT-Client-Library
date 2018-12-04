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
 * @file        CLIB_heap.c
 * @brief       Heap Memory Management API with C
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01 
 *              - 2018/05/04 : zhaozhenge@outlook.com 
 *                  -# New
 */

/**************************************************************
**  Include
**************************************************************/

#include "CLIB_heap.h"

#ifdef CLIB_HEAP_MODULE_ENABLED

/**************************************************************
**  Symbol
**************************************************************/

#define HEAP_BITS_PER_BYTE      ( ( size_t ) 8 )
/*!< Assumes 8bit bytes */

#define MINIMUM_BLOCK_SIZE      ( ( size_t ) ( Heap->m_prvHeapStructSize << 1 ) )
/*!< Block sizes must not get too small. */

/**************************************************************
**  Function
**************************************************************/

/** 
 * @brief               Called automatically to set up the required heap structures the first time
 * @param[in,out]       Heap                Heap information
 * @return              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/04/12
 * @callgraph
 * @callergraph
 */
static void prvHeapInit( S_HEAP_HANDLE* Heap )
{
    S_BLOCK_LINK*   FirstFreeBlock;
    uint8_t*        AlignedHeap;
    size_t          MemAddress;
    size_t          TotalHeapSize = Heap->m_prvTotalHeapSize;
    
    /* Ensure the heap starts on a correctly aligned boundary. */
    MemAddress = ( size_t ) Heap->m_prvHeap;

    if( ( MemAddress & (Heap->m_PortByteAlignmentMask) ) != 0 )
    {
        MemAddress += ( Heap->m_PortByteAlignment - 1 );
        MemAddress &= ~( ( size_t ) (Heap->m_PortByteAlignmentMask) );
        TotalHeapSize -= MemAddress - ( size_t ) Heap->m_prvHeap;
    }

    AlignedHeap = ( uint8_t * ) MemAddress;

    /* m_StartBlock is used to hold a pointer to the first item in the list of free
     * blocks.  The void cast is used to prevent compiler warnings.
     */
    Heap->m_StartBlock.NextFreeBlock = ( void * ) AlignedHeap;
    Heap->m_StartBlock.BlockSize = ( size_t ) 0;

    /* m_EndBlockPtr is used to mark the end of the list of free blocks and is inserted
     * at the end of the heap space.
     */
    MemAddress = ( ( size_t ) AlignedHeap ) + TotalHeapSize;
    MemAddress -= Heap->m_prvHeapStructSize;
    MemAddress &= ~( ( size_t ) (Heap->m_PortByteAlignmentMask) );
    Heap->m_EndBlockPtr = ( void * ) MemAddress;
    Heap->m_EndBlockPtr->BlockSize = 0;
    Heap->m_EndBlockPtr->NextFreeBlock = NULL;

    /* To start with there is a single free block that is sized to take up the
     * entire heap space, minus the space taken by m_EndBlockPtr. 
     */
    FirstFreeBlock = ( void * ) AlignedHeap;
    FirstFreeBlock->BlockSize = MemAddress - ( size_t ) FirstFreeBlock;
    FirstFreeBlock->NextFreeBlock = Heap->m_EndBlockPtr;

    /* Only one block exists - and it covers the entire usable heap space. */
    Heap->m_MinimumEverFreeBytesRemaining = FirstFreeBlock->BlockSize;
    Heap->m_FreeBytesRemaining = FirstFreeBlock->BlockSize;

    /* Work out the position of the top bit in a size_t variable. */
    Heap->m_BlockAllocatedBit = ( ( size_t ) 1 ) << ( ( sizeof( size_t ) * HEAP_BITS_PER_BYTE ) - 1 );
    return;
}

/** 
 * @brief               Insert the new block into the list of free blocks
 * @param[in,out]       Heap                Heap information
 * @param[in]           BlockToInsert       New memory block
 * @return              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/04/12
 * @callgraph
 * @callergraph
 */
static void prvInsertBlockIntoFreeList( S_HEAP_HANDLE* Heap, S_BLOCK_LINK *BlockToInsert )
{
    S_BLOCK_LINK*   Iterator;
    uint8_t*        MemAddress;

    /* Iterate through the list until a block is found that has a higher address
     * than the block being inserted.
     */
    for( Iterator = &(Heap->m_StartBlock); Iterator->NextFreeBlock < BlockToInsert; Iterator = Iterator->NextFreeBlock )
    {
        /* Nothing to do here, just iterate to the right position. */
    }

    /* Do the block being inserted, and the block it is being inserted after
     * make a contiguous block of memory? 
     */
    MemAddress = ( uint8_t * ) Iterator;
    if( ( MemAddress + Iterator->BlockSize ) == ( uint8_t * ) BlockToInsert )
    {
        Iterator->BlockSize += BlockToInsert->BlockSize;
        BlockToInsert = Iterator;
    }

    /* Do the block being inserted, and the block it is being inserted before
     * make a contiguous block of memory? 
     */
    MemAddress = ( uint8_t * ) BlockToInsert;
    if( ( MemAddress + BlockToInsert->BlockSize ) == ( uint8_t * ) Iterator->NextFreeBlock )
    {
        if( Iterator->NextFreeBlock != Heap->m_EndBlockPtr )
        {
            /* One big block from the two blocks. */
            BlockToInsert->BlockSize += Iterator->NextFreeBlock->BlockSize;
            BlockToInsert->NextFreeBlock = Iterator->NextFreeBlock->NextFreeBlock;
        }
        else
        {
            BlockToInsert->NextFreeBlock = Heap->m_EndBlockPtr;
        }
    }
    else
    {
        BlockToInsert->NextFreeBlock = Iterator->NextFreeBlock;
    }

    /* If the block being inserted plugged a gab, so was merged with the block
     * before and the block after, then it's NextFreeBlock pointer will have
     * already been set, and should not be set here as that would make it point
     * to itself. 
     */
    if( Iterator != BlockToInsert )
    {
        Iterator->NextFreeBlock = BlockToInsert;
    }
    return;
}

/**************************************************************
**  Interface
**************************************************************/

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
                                F_CLIB_LOCKFUNC Lock, F_CLIB_UNLOCKFUNC Unlock)
{
    /* check input Data */
    if( !Heap || !Memory || !HeapSize )
    {
        return (-1);
    }
    if( (32 != PortByteAlignment) && (16 != PortByteAlignment) && (8 != PortByteAlignment) )
    {
        return (-1);
    }
    if( ( !Lock && Unlock ) || (Lock && !Unlock) )
    {
        return (-1);
    }
    
    /* Init Heap */
    memset(Heap, 0, sizeof(S_HEAP_HANDLE));
    
    /* Allocate the memory for the heap */
    Heap->m_prvHeap = Memory;
    Heap->m_prvTotalHeapSize = HeapSize;
    Heap->m_EndBlockPtr = NULL;
    /* The size of the structure placed at the beginning of each allocated 
     * memory block must by correctly byte aligned. 
     */
    Heap->m_PortByteAlignment = PortByteAlignment;
    if(32 == Heap->m_PortByteAlignment)
    {
        Heap->m_PortByteAlignmentMask = 0x001f;
    }
    else if(16 == Heap->m_PortByteAlignment)
    {
        Heap->m_PortByteAlignmentMask = 0x000f;
    }
    else
    {
        Heap->m_PortByteAlignmentMask = 0x0007;
    }
    Heap->m_prvHeapStructSize = ( sizeof( S_BLOCK_LINK ) + ( ( size_t ) ( Heap->m_PortByteAlignment - 1 ) ) ) & ~( ( size_t ) Heap->m_PortByteAlignmentMask );
    Heap->m_FreeBytesRemaining = 0;
    Heap->m_MinimumEverFreeBytesRemaining = 0;
    Heap->m_BlockAllocatedBit = 0;
    
    Heap->LockFunc = Lock;
    Heap->UnlockFunc = Unlock;
    Heap->UsrData = UserData;
    
    /* Require initialisation to setup the list of free blocks */
    prvHeapInit(Heap);
    
    return (0);    
}

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
extern void *CLIB_heap_malloc( S_HEAP_HANDLE* Heap, size_t WantedSize )
{
    S_BLOCK_LINK *CurrentBlock, *PreviousBlock, *NewBlockLink;
    void *prvReturn = NULL;
    
    /* check input Data */
    if( !Heap )
    {
        return prvReturn;
    }
    
    if(Heap->LockFunc)
    {
        Heap->LockFunc(Heap->UsrData);
    }
    
    /* If this is the first call to malloc then the heap will require
     * initialisation to setup the list of free blocks. 
     */
    if( Heap->m_EndBlockPtr == NULL )
    {
        prvHeapInit(Heap);
    }
    /* Check the requested block size is not so large that the top bit is
     * set.  The top bit of the block size member of the S_BLOCK_LINK structure
     * is used to determine who owns the block - the application or the
     * kernel, so it must be free. 
     */
    if( ( WantedSize & Heap->m_BlockAllocatedBit ) == 0 )
    {
        /* The wanted size is increased so it can contain a S_BLOCK_LINK
         * structure in addition to the requested amount of bytes. 
         */
        if( WantedSize > 0 )
        {
            WantedSize += Heap->m_prvHeapStructSize;
            /* Ensure that blocks are always aligned to the required number of bytes. */
            if( ( WantedSize & Heap->m_PortByteAlignmentMask ) != 0x00 )
            {
                /* Byte alignment required. */
                WantedSize += ( Heap->m_PortByteAlignment - ( WantedSize & Heap->m_PortByteAlignmentMask ) );
            }
        }
        
        if( ( WantedSize > 0 ) && ( WantedSize <= Heap->m_FreeBytesRemaining ) )
        {
            /* Traverse the list from the start	(lowest address) block until
             * one of adequate size is found. 
             */
            PreviousBlock = &(Heap->m_StartBlock);
            CurrentBlock = Heap->m_StartBlock.NextFreeBlock;
            while( ( CurrentBlock->BlockSize < WantedSize ) && ( CurrentBlock->NextFreeBlock != NULL ) )
            {
                PreviousBlock = CurrentBlock;
                CurrentBlock = CurrentBlock->NextFreeBlock;
            }
            
            /* If the end marker was reached then a block of adequate size was not found. */
            if( CurrentBlock != Heap->m_EndBlockPtr )
            {
                /* Return the memory space pointed to - jumping over the
                 * S_BLOCK_LINK structure at its start.
                 */
                prvReturn = ( void * ) ( ( ( uint8_t * ) PreviousBlock->NextFreeBlock ) + Heap->m_prvHeapStructSize );
                
                /* This block is being returned for use so must be taken out of the list of free blocks. */
                PreviousBlock->NextFreeBlock = CurrentBlock->NextFreeBlock;
                
                /* If the block is larger than required it can be split into two. */
                if( ( CurrentBlock->BlockSize - WantedSize ) > MINIMUM_BLOCK_SIZE )
                {
                    /* This block is to be split into two.  Create a new
                     * block following the number of bytes requested. The void
                     * cast is used to prevent byte alignment warnings from the
                     * compiler. 
                     */
                    NewBlockLink = ( void * ) ( ( ( uint8_t * ) CurrentBlock ) + WantedSize );
                    
                    /* Calculate the sizes of two blocks split from the single block. */
                    NewBlockLink->BlockSize = CurrentBlock->BlockSize - WantedSize;
                    CurrentBlock->BlockSize = WantedSize;
                    
                    /* Insert the new block into the list of free blocks. */
                    prvInsertBlockIntoFreeList( Heap, NewBlockLink );
                }
                
                Heap->m_FreeBytesRemaining -= CurrentBlock->BlockSize;
                
                if( Heap->m_FreeBytesRemaining < Heap->m_MinimumEverFreeBytesRemaining )
                {
                    Heap->m_MinimumEverFreeBytesRemaining = Heap->m_FreeBytesRemaining;
                }
                
                /* The block is being returned - it is allocated and owned
                 * by the application and has no "next" block. 
                 */
                CurrentBlock->BlockSize |= Heap->m_BlockAllocatedBit;
                CurrentBlock->NextFreeBlock = NULL;
            }
        }
    }
    
    if(Heap->UnlockFunc)
    {
        Heap->UnlockFunc(Heap->UsrData);
    }
    
    return prvReturn;
}

/** 
 * @brief               Memory free function
 * @param[in,out]       Heap                Heap information
 * @param[in]           Pv                  Memory address that allocated from the heap handle
 * @return              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/04/12
 * @callgraph
 * @hidecallergraph
 */
extern void CLIB_heap_free(S_HEAP_HANDLE* Heap, void* Pv)
{
    uint8_t*        MemAddress = ( uint8_t * ) Pv;
    S_BLOCK_LINK*   Link;

    if( (Pv != NULL) && (Heap != NULL) )
    {
        /* The memory being freed will have an S_BLOCK_LINK structure immediately before it. */
        MemAddress -= Heap->m_prvHeapStructSize;

        /* This casting is to keep the compiler from issuing warnings. */
        Link = ( void * ) MemAddress;

        if( ( Link->BlockSize & Heap->m_BlockAllocatedBit ) != 0 )
        {
            if( Link->NextFreeBlock == NULL )
            {
                /* The block is being returned to the heap - it is no longer allocated. */
                Link->BlockSize &= ~(Heap->m_BlockAllocatedBit);
                
                if(Heap->LockFunc)
                {
                    Heap->LockFunc(Heap->UsrData);
                }

                /* Add this block to the list of free blocks. */
                Heap->m_FreeBytesRemaining += Link->BlockSize;
                prvInsertBlockIntoFreeList( Heap, ( ( S_BLOCK_LINK * ) Link ) );
                
                if(Heap->UnlockFunc)
                {
                    Heap->UnlockFunc(Heap->UsrData);
                }
    
            }
        }
    }
    return;
}

/** 
 * @brief               Get the remain memory of the heap
 * @param[in]           Heap                Heap information
 * @return              Remain memory of the heap
 * @author              zhaozhenge@outlook.com
 * @date                2018/04/12
 * @callgraph
 * @hidecallergraph
 */
extern size_t CLIB_heap_GetFreeHeapSize( S_HEAP_HANDLE* Heap )
{
    if( Heap )
    {
        return Heap->m_FreeBytesRemaining;
    }
    else
    {
        return 0;
    }
}

/** 
 * @brief               Get the minimum size ever free back to the heap
 * @param[in]           Heap                Heap information
 * @return              The minimum size ever free back to the heap
 * @author              zhaozhenge@outlook.com
 * @date                2018/04/12
 * @callgraph
 * @hidecallergraph
 */
extern size_t CLIB_heap_GetMinimumEverFreeHeapSize( S_HEAP_HANDLE* Heap )
{
    if( Heap )
    {
        return Heap->m_MinimumEverFreeBytesRemaining;
    }
    else
    {
        return 0;
    }
}

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
extern int32_t CLIB_heap_delete( S_HEAP_HANDLE* Heap )
{
    if( !Heap )
    {
        return (-1);
    }
    
    /* Initialize Heap */
    memset(Heap, 0, sizeof(S_HEAP_HANDLE));
    
    return (0);
}

#endif /* CLIB_HEAP_MODULE_ENABLED */
