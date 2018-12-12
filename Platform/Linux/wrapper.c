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
 * @example     wrapper.c
 * @brief       Wrapper Implement for Linux Platform
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
 * @version     00.00.04 
 *              - 2018/12/12 : zhaozhenge@outlook.com 
 *                  -# Modify some comment
 */

/**************************************************************
**  Include
**************************************************************/

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h> 
#include <sys/socket.h>
#include <sys/select.h>
#include "wrapper.h"
#include "MQC_wrap.h"

/**************************************************************
**  Interface
**************************************************************/

extern uint16_t MQC_Wrap_htons(uint16_t Data)
{
    return htons(Data);
}

extern uint32_t MQC_Wrap_htonl(uint32_t Data)
{
    return htonl(Data);
}

extern uint16_t MQC_Wrap_ntohs(uint16_t Data)
{
    return ntohs(Data);
}

extern uint32_t MQC_Wrap_ntohl(uint32_t Data)
{
    return ntohl(Data);
}

/** 
 * @brief               wrapper function initialize
 * @param[in,out]       Ctx                 User Context
 * @retval              0 for successful
 * @retval              -1 for fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/30
 */
extern int32_t wrapper_init(S_PLATFORM_DATA* Ctx)
{
    int                 Err         =   0;
    int                 val         =   1;
    char*               TmpPtr      =   NULL;
    uint16_t            TmpPort     =   0; 
    do
    {
        TmpPtr = Ctx->DstAddress;
        TmpPort = Ctx->DstPort;
        
        memset(Ctx, 0xFF, sizeof(S_PLATFORM_DATA));
        
        Ctx->DstPort = TmpPort;
        Ctx->DstAddress = TmpPtr;
        
        /* Create Semaphore */
        Err = semget((key_t)1234, 1, IPC_CREAT | 0666);
        if(0 > Err)
        {
            D_MQC_PRINT( " failed\n  ! semget() returned %d\n\n", Err );
            break;
        }
        Ctx->SemaphoreId = Err;
        Err = semctl(Ctx->SemaphoreId, 0, SETVAL, val);
		if(Err)
        {
            D_MQC_PRINT( " failed\n  ! semctl() returned %d\n\n", Err );
            break;
        }
        
        /* Create Random Sequence */
        srand(systick_wrapper());

	}while(0);
    
	return Err;
}

/** 
 * @brief               wrapper Implement for TCP/IP network start
 * @param[in,out]       Ctx                 User Context
 * @retval              0 for successful
 * @retval              -1 for fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 */
extern int32_t network_open_wrapper(S_PLATFORM_DATA* Ctx)
{
    int                 Err         =   0;
    struct hostent*     hptr        =   NULL;
    struct sockaddr_in  ServerAddr;
    
    do
    {
        hptr = gethostbyname(Ctx->DstAddress);
        if( !hptr )
        {
            Err = -1;
            D_MQC_PRINT( " failed\n  ! gethostbyname() returned NULL\n\n" );
            break;
        }
        
        /* Create Socket */
        Err = socket(AF_INET, SOCK_STREAM, 0);
        if(0 >= Err)
        {
            D_MQC_PRINT( " failed\n  ! socket() returned %d\n\n", Err );
            break;
        }
        Ctx->SocketFd = Err;
        ServerAddr.sin_family = AF_INET;
        ServerAddr.sin_port = htons(Ctx->DstPort);
        memcpy( &(ServerAddr.sin_addr.s_addr), hptr->h_addr_list[0], sizeof(ServerAddr.sin_addr.s_addr));
        memset(&(ServerAddr.sin_zero), 0, sizeof(ServerAddr.sin_zero)); 
        Err = connect(Ctx->SocketFd, (struct sockaddr *)&ServerAddr, sizeof(struct sockaddr_in));
        if(Err)
        {
            D_MQC_PRINT( " failed\n  ! connect() returned %d\n\n", Err );
            break;
        }
    }while(0);
    
    return Err;
}

/** 
 * @brief               wrapper Implement for TCP/IP network close
 * @param[in,out]       Ctx                 User Context
 * @return              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 */
extern void network_close_wrapper(S_PLATFORM_DATA* Ctx)
{
    if(0 < Ctx->SocketFd)
    {
        close(Ctx->SocketFd);
    }
    
    return;
}

/** 
 * @brief               wrapper function finalize
 * @param[in,out]       Ctx                 User Context
 * @return              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/30
 */
extern void wrapper_deinit(S_PLATFORM_DATA* Ctx)
{
    int                 Err         =   0;
    
    if(0 <= Ctx->SemaphoreId)
    {
        Err = semctl(Ctx->SemaphoreId, 0, IPC_RMID, 0);
        if(Err)
        {
            D_MQC_PRINT( " failed\n  ! semctl() returned %d\n\n", Err );
        }
    }
    
    network_close_wrapper(Ctx);
    
    memset(Ctx, 0, sizeof(S_PLATFORM_DATA));
    
    return;
}

/** 
 * @brief               This function get some random data
 * @param[out]          Output          Used to store the random data
 * @param[in]           Len             The size in bytes of the random data
 * @return              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/10
 */
extern void random_wrapper(uint8_t* Output, size_t Len)
{
    size_t  i   =   0;
    for(i = 0; i < Len; i++)
    {
        Output[i] = rand() / 256;
    }
    return;
}

/** 
 * @brief               Resource Lock function
 * @param[in]           Ctx         Lock Id
 * @retval              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/30
 */
extern void lock_wrapper(S_PLATFORM_DATA* Ctx)
{
    int             Err         =   0;
    struct sembuf   sem_buf;
    
    memset(&sem_buf, 0, sizeof(sem_buf));

    sem_buf.sem_num =   0;
    sem_buf.sem_op  =   -1;

    Err = semop(Ctx->SemaphoreId, &sem_buf, 1);
    if(Err)
    {
        D_MQC_PRINT( " failed\n  ! semop() returned %d\n\n", Err );
    }
    return;
}

/** 
 * @brief               Resource UnLock function
 * @param[in]           Ctx         Lock Id
 * @retval              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/30
 */
extern void unlock_wrapper(S_PLATFORM_DATA* Ctx)
{
    int             Err         =   0;
    struct sembuf   sem_buf;

    memset(&sem_buf, 0, sizeof(sem_buf));

    sem_buf.sem_num =   0;
    sem_buf.sem_op  =   1;

    Err = semop(Ctx->SemaphoreId, &sem_buf, 1);
    if(Err)
    {
        D_MQC_PRINT( " failed\n  ! semop() returned %d\n\n", Err );
    }
    return;
}

/** 
 * @brief               Get the systime in milliseconds
 * @retval              Current time since system boot
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/30
 */
extern uint32_t systick_wrapper(void)
{
    int             Err =   0;
    uint32_t        Ret =   0;
    struct timeval  t_time;
    
    Err = gettimeofday(&t_time, NULL);
    if(Err)
    {
        D_MQC_PRINT( " failed\n  ! gettimeofday() returned %d\n\n", Err );
        Ret = 0;
    }
    else
    {
        Ret = ((long)t_time.tv_sec)*1000+(long)t_time.tv_usec/1000;
    }
    return Ret;
}
 
/** 
 * @brief               wrapper Implement for TCP/IP Data Send
 * @param[in,out]       Ctx                 User Context
 * @param[in]           Data                Data want to write via network
 * @param[in]           Size                Size of the Data
 * @return              The size of data actually write success ( < 0 means error)
 * @author              zhaozhenge@outlook.com
 * @date                2018/10/29
 */
extern int32_t tcpwrite_wrapper(S_PLATFORM_DATA* Ctx, const uint8_t* Data, size_t Size)
{
    int32_t Err =   0;
    
    Err = send(Ctx->SocketFd, Data, Size, 0);
    
    return Err;
}

/** 
 * @brief               wrapper Implement for TCP/IP data receive watching 
 * @param[in,out]       Ctx                 User Context
 * @param[in]           Timeout             Timeout for wait no data
 * @retval              0                   timeout
 * @retval              1                   success
 * @retval              -1                  fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/30
 */
extern int32_t tcpcheck_wrapper(S_PLATFORM_DATA* Ctx, uint32_t Timeout)
{
    int32_t         Err         =   0;
    fd_set          Fds;
    struct timeval  Tv;

    FD_ZERO(&Fds);
    FD_SET(Ctx->SocketFd, &Fds);
    
    if(0 == Timeout)
    {
        Err = select(Ctx->SocketFd + 1, &Fds, NULL, NULL, NULL);
    }
    else
    {
        Tv.tv_sec = Timeout / 1000 ;
        Tv.tv_usec = (Timeout % 1000) * 1000;
        Err = select(Ctx->SocketFd + 1, &Fds, NULL, NULL, &Tv);
    }
    
    if(0 > Err)
    {
        D_MQC_PRINT( " failed\n  ! select() returned %d\n\n", Err );
        Err = -1;
    }
    else if(0 == Err)
    {
        Err = 0;
    }
    else
    {
        if(FD_ISSET(Ctx->SocketFd, &Fds))
        {
            Err = 1;
        }
        else
        {
            Err = 0;
        }
    }
    
    return Err;
}
 
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
extern int32_t tcpread_wrapper(S_PLATFORM_DATA* Ctx, uint8_t* Data, size_t Size)
{
    int32_t Err =   0;
    
    Err = recv(Ctx->SocketFd, Data, Size, 0);
    
    return Err;
}
