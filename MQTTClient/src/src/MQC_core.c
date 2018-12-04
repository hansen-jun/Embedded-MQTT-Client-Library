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
 * @file        MQC_core.c
 * @brief       MQTT Client Library Core Function
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01 
 *              - 2018/06/18 : zhaozhenge@outlook.com 
 *                  -# New
 */

/**************************************************************
**  Include
**************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "../inc/MQC_core.h"
#include "../inc/MQC_queue.h"
#include "../../../CommonLib/CLIB_api.h"
#include "MQC_wrap.h"

/**************************************************************
**  Symbol
**************************************************************/

#define D_MQC_PROTOCOL_LEVEL                        (4)     /*!< Ver 3.1.1 */

#define D_MQC_MAX_MESSAGE_HEADER_SIZE               (5)     /*!< The maximum message header size of MQTT */

#define D_MQC_CONNECT_MSG_VARIABLE_HEADER_SIZE      (10)    /*!< ProtocolName(6) + ProtocolLevel(1) + ConnectFlags(1) + KeepAlive(2) */
#define D_MQC_SUBSCRIBE_MSG_VARIABLE_HEADER_SIZE    (2)     /*!< PacketIdentifier */
#define D_MQC_UNSUBSCRIBE_MSG_VARIABLE_HEADER_SIZE  (2)     /*!< PacketIdentifier */
#define D_MQC_PUBACK_MSG_VARIABLE_HEADER_SIZE       (2)     /*!< PacketIdentifier */
#define D_MQC_PUBREC_MSG_VARIABLE_HEADER_SIZE       (2)     /*!< PacketIdentifier */
#define D_MQC_PUBREL_MSG_VARIABLE_HEADER_SIZE       (2)     /*!< PacketIdentifier */
#define D_MQC_PUBCOMP_MSG_VARIABLE_HEADER_SIZE      (2)     /*!< PacketIdentifier */
#define D_MQC_PINGREQ_MSG_VARIABLE_HEADER_SIZE      (0)     /*!< No Data */
#define D_MQC_DISCONNECT_MSG_VARIABLE_HEADER_SIZE   (0)     /*!< No Data */

#define D_MQC_CALLBACK_SAFECALL(Ret, function, ...)  {\
                                                        if(MQCHandler->UnlockFunc)\
                                                        {\
                                                            MQCHandler->UnlockFunc(MQCHandler->UsrCtx);\
                                                        }\
                                                        if(function)\
                                                        {\
                                                            Ret = function(__VA_ARGS__);\
                                                        }\
                                                        else\
                                                        {\
                                                            Ret = 0;\
                                                        }\
                                                        if(MQCHandler->LockFunc)\
                                                        {\
                                                            MQCHandler->LockFunc(MQCHandler->UsrCtx);\
                                                        }\
                                                        (void)Ret;\
                                                     }

/**************************************************************
**  Structure
**************************************************************/

/**
 * @brief       MQTT Protocol Process Context
 * @author      zhaozhenge@outlook.com
 * @date        2018/11/30
 */
typedef struct _S_MQC_PROTOCOL_DATA
{
    E_MQC_MSG_TYPE          Type;
    /*!< Message Type */
    
    int32_t                 (*processFunc)(S_MQC_SESSION_HANDLE* MQCHandler, uint8_t FixedHeader, uint8_t* Data, uint32_t DataSize);
    /*!< Message Process Function */
}S_MQC_PROTOCOL_DATA;

/**************************************************************
**  Global Param
**************************************************************/

static const uint8_t*       D_MQC_STR_PROTOCOL  =   (uint8_t*)"MQTT";

static int32_t  prvMQC_processConnack   (S_MQC_SESSION_HANDLE* MQCHandler, uint8_t FixedHeader, uint8_t* Data, uint32_t DataSize);
static int32_t  prvMQC_processPublish   (S_MQC_SESSION_HANDLE* MQCHandler, uint8_t FixedHeader, uint8_t* Data, uint32_t DataSize);
static int32_t  prvMQC_processPuback    (S_MQC_SESSION_HANDLE* MQCHandler, uint8_t FixedHeader, uint8_t* Data, uint32_t DataSize);
static int32_t  prvMQC_processPubrec    (S_MQC_SESSION_HANDLE* MQCHandler, uint8_t FixedHeader, uint8_t* Data, uint32_t DataSize);
static int32_t  prvMQC_processPubrel    (S_MQC_SESSION_HANDLE* MQCHandler, uint8_t FixedHeader, uint8_t* Data, uint32_t DataSize);
static int32_t  prvMQC_processPubcomp   (S_MQC_SESSION_HANDLE* MQCHandler, uint8_t FixedHeader, uint8_t* Data, uint32_t DataSize);
static int32_t  prvMQC_processSuback    (S_MQC_SESSION_HANDLE* MQCHandler, uint8_t FixedHeader, uint8_t* Data, uint32_t DataSize);
static int32_t  prvMQC_processUnsuback  (S_MQC_SESSION_HANDLE* MQCHandler, uint8_t FixedHeader, uint8_t* Data, uint32_t DataSize);
static int32_t  prvMQC_processPingresp  (S_MQC_SESSION_HANDLE* MQCHandler, uint8_t FixedHeader, uint8_t* Data, uint32_t DataSize);
static const    S_MQC_PROTOCOL_DATA     ProtocolData[]      =   {
    {   E_MQC_MSG_CONNACK   ,   prvMQC_processConnack   },
    {   E_MQC_MSG_PUBLISH   ,   prvMQC_processPublish   },
    {   E_MQC_MSG_PUBACK    ,   prvMQC_processPuback    },
    {   E_MQC_MSG_PUBREC    ,   prvMQC_processPubrec    },
    {   E_MQC_MSG_PUBREL    ,   prvMQC_processPubrel    },
    {   E_MQC_MSG_PUBCOMP   ,   prvMQC_processPubcomp   },
    {   E_MQC_MSG_SUBACK    ,   prvMQC_processSuback    },
    {   E_MQC_MSG_UNSUBACK  ,   prvMQC_processUnsuback  },
    {   E_MQC_MSG_PINGRESP  ,   prvMQC_processPingresp  }
};

/**************************************************************
**  Function
**************************************************************/

/** 
 * @brief               calculate the time passed
 * @param[in]           SystimeCountOrig        Base SystimeCount with millisecond
 * @param[in]           SystimeCountNow         Now SystimeCount with millisecond
 * @return              Passed time with millisecond
 * @author              zhaozhenge@outlook.com
 * @date                2018/06/18
 * @callgraph
 * @callergraph
 */
static uint32_t prvMQC_CheckPassTime(uint32_t SystimeCountOrig, uint32_t SystimeCountNow)
{
    uint32_t Ret = 0;
    if(SystimeCountOrig <= SystimeCountNow)
    {
        Ret = SystimeCountNow - SystimeCountOrig;
    }
    else
    {
        Ret = 0xFFFFFFFF - SystimeCountOrig + SystimeCountNow;
    }
    return Ret;
}
 
/** 
 * @brief               Encode an Integer Remaining Length into MQTT format
 * @param[in]           Dst                     Destination buffer (can be NULL for checking size)
 * @param[in,out]       Dstlen                  \b in   :   Size of the destination buffer \n
 *                                              \b out  :   Number of bytes written                                            
 * @param[in]           RemainingLength         Amount of the Remaining Data encoded
 * @retval              0                       success
 * @retval              -1                      Destination buffer too small
 * @note                Input \a *Dstlen = 0 to obtain the required buffer size in \a *Dstlen
 * @author              zhaozhenge@outlook.com
 * @date                2018/06/18
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_RemainingLengthEncode( uint8_t* Dst, uint32_t* Dstlen, uint32_t RemainingLength )
{
    uint32_t    MsgLength       =   0;
    uint8_t*    EndPtr          =   NULL;
    uint32_t    CalData_32bit   =   0;
    
    /* check the RemainingLength */
    if( 128 > RemainingLength )
    {
        MsgLength = MsgLength + 1;
    }
    else if( 16383 > RemainingLength )
    {
        MsgLength = MsgLength + 2;
    }
    else if( 2097151 > RemainingLength )
    {
        MsgLength = MsgLength + 3;
    }
    else if( 268435455 > RemainingLength )
    {
        MsgLength = MsgLength + 4;
    }
    else
    {
        return (-1);
    }
    if( (NULL == Dst) || (0 == *Dstlen) )
    {
        *Dstlen = MsgLength;
        return (0);
    }
    if( *Dstlen < MsgLength )
    {
        /* Buffer not enough */
        return (-1);
    }
    /* encode the Message */
    EndPtr = Dst;
    CalData_32bit = RemainingLength;
    do
    {
        *EndPtr = CalData_32bit % 128;
        CalData_32bit = CalData_32bit / 128;
        if(CalData_32bit)
        {
            *EndPtr = (*EndPtr) | 128;
        }
        EndPtr++;
    }while(CalData_32bit);
    *Dstlen = MsgLength;
    return (0);
}

/** 
 * @brief               Decode an Integer Remaining Length from MQTT format
 * @param[in]           Src                     Source buffer
 * @param[in]           Srclen                  Size of the Source buffer                  
 * @param[out]          RemainingLength         Decode result of the Remaining Length
 * @retval              0                       success
 * @retval              1                       data not enough
 * @retval              -1                      Decode error
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/28
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_RemainingLengthDecode( uint8_t* Src, uint32_t Srclen, uint32_t* RemainingLength )
{
    uint32_t    Multiplier  =   0;
    uint32_t    Result      =   0;
    uint32_t    i           =   0;
    uint8_t     Data        =   0;
    
    if(!Srclen)
    {
        return (1);
    }
    
    Data = *(Src + Srclen - 1);
    if( (Data & 128) )
    {
        if(Srclen >= 4)
        {
            return (-1);
        }
        else
        {
            return (1);
        }
    }

    for(i = 0, Multiplier=1; i < Srclen; i++)
    {
        Data = *Src;
        Result += (Data & 127) * Multiplier;
        Multiplier *= 128;
        Src++;
    }
    
    *RemainingLength = Result;
    
    return (0);
}

/** 
 * @brief               Encode a buffer into MQTT CONNECT Message format
 * @param[in]           Dst                     Destination buffer (can be NULL for checking size)
 * @param[in,out]       Dstlen                  \b in   :   Size of the destination buffer \n
 *                                              \b out  :   Number of bytes written
 * @param[in]           CleanSessionSetting     The Setting information of CleanSession Flag
 * @param[in]           WillMessageSetting      The Setting information of Will Message
 * @param[in]           AuthoritionSetting      The Setting information of Authorition
 * @param[in]           KeepAliveInterval       Keep Alive Message sent interval (in Second)
 * @param[in]           ClientId                Unique Client Id
 * @retval              0                       success
 * @retval              -1                      Destination buffer too small
 * @note                Input \a *Dstlen = 0 to obtain the required buffer size in \a *Dstlen
 * @author              zhaozhenge@outlook.com
 * @date                2018/06/18
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_ConnectMessageEncode( uint8_t* Dst, size_t* Dstlen, bool CleanSessionSetting, S_MQC_WILL_INFO* WillMessageSetting, 
                                            S_MQC_AUTH_INFO* AuthoritionSetting, uint16_t KeepAliveInterval, S_MQC_UTF8_DATA* ClientId )
{
    uint32_t    RemainingLength        =   0;
    uint32_t    EncodeRemainingLength  =   0;
    size_t      WriteDataSize          =   0;
    int32_t     Ret                    =   0;
    uint8_t*    EndPtr                 =   NULL;
    uint16_t    OrigDataLength         =   0;

    /* calculate the RemainingLength of CONNECT Message */
    RemainingLength = RemainingLength + D_MQC_CONNECT_MSG_VARIABLE_HEADER_SIZE;
    /* Client Id */
    RemainingLength = RemainingLength + sizeof(ClientId->Length) + ClientId->Length;
    /* Will Message */
    if(WillMessageSetting->Enable)
    {
        RemainingLength = RemainingLength + sizeof(uint16_t) + WillMessageSetting->Message.Topic.Length;
        RemainingLength = RemainingLength + sizeof(uint16_t) + WillMessageSetting->Message.Length;
    }
    /* authorization */
    if(AuthoritionSetting->UsernameEnable)
    {
        RemainingLength = RemainingLength + sizeof(AuthoritionSetting->Username.Length) + AuthoritionSetting->Username.Length;
    }
    if(AuthoritionSetting->PasswordEnable)
    {
        RemainingLength = RemainingLength + sizeof(AuthoritionSetting->Password.Length) + AuthoritionSetting->Password.Length;
    }
    Ret = prvMQC_RemainingLengthEncode( NULL, &EncodeRemainingLength, RemainingLength );
    if(Ret)
    {
        return (-1);
    }
    /* calculate the Total Message Size of CONNECT Message */
    WriteDataSize = EncodeRemainingLength + RemainingLength + 1;
    if( (NULL == Dst) || (0 == *Dstlen) )
    {
        *Dstlen = WriteDataSize;
        return (0);
    }
    if(*Dstlen < WriteDataSize)
    {
        /* No enough Buffer */
        return (-1);
    }

    /* Make the Message Data */
    EndPtr = Dst;
    
    /* Fixed Header */
    *EndPtr = (E_MQC_MSG_CONNECT << 4);
    EndPtr++;
    /* Remaining Length */
    Ret = prvMQC_RemainingLengthEncode( EndPtr, &EncodeRemainingLength, RemainingLength );
    if(Ret)
    {
        return (-1);
    }
    EndPtr = EndPtr + EncodeRemainingLength;
    /* Protocol Name */
    OrigDataLength = strlen((char*)D_MQC_STR_PROTOCOL);
    *((uint16_t*)EndPtr) = MQC_htons(OrigDataLength);
    EndPtr = EndPtr + sizeof(uint16_t);
    memcpy(EndPtr, D_MQC_STR_PROTOCOL, OrigDataLength);
    EndPtr = EndPtr + OrigDataLength;
    /* Protocol Level */
    *EndPtr = D_MQC_PROTOCOL_LEVEL;
    EndPtr++;
    /* Connect Flags */
    *EndPtr = ( CleanSessionSetting?(1<<1):(0) ) + 
              ( WillMessageSetting->Enable?(1<<2):(0) ) + 
              ( WillMessageSetting->QoS<<3 ) + 
              ( WillMessageSetting->Retain?(1<<5):(0) ) + 
              ( AuthoritionSetting->PasswordEnable?(1<<6):(0) ) +
              ( AuthoritionSetting->UsernameEnable?(1<<7):(0) );
    EndPtr++;
    /* Keep Alive */
    *((uint16_t*)EndPtr) = MQC_htons(KeepAliveInterval);
    EndPtr = EndPtr + sizeof(uint16_t);
    /* Client Identifier */
    *((uint16_t*)EndPtr) = MQC_htons(ClientId->Length);
    EndPtr = EndPtr + sizeof(uint16_t);
    if(ClientId->Data && ClientId->Length)
    {
        memcpy(EndPtr, ClientId->Data, ClientId->Length);
        EndPtr = EndPtr + ClientId->Length;
    }
    /* Will Topic */
    if(WillMessageSetting->Enable)
    {
        *((uint16_t*)EndPtr) = MQC_htons(WillMessageSetting->Message.Topic.Length);
        EndPtr = EndPtr + sizeof(uint16_t);
        memcpy(EndPtr, WillMessageSetting->Message.Topic.Data, WillMessageSetting->Message.Topic.Length);
        EndPtr = EndPtr + WillMessageSetting->Message.Topic.Length;
    }
    /* Will Message */
    if( WillMessageSetting->Enable && WillMessageSetting->Message.Length && WillMessageSetting->Message.Content )
    {
        *((uint16_t*)EndPtr) = MQC_htons(WillMessageSetting->Message.Length);
        EndPtr = EndPtr + sizeof(uint16_t);
        memcpy(EndPtr, WillMessageSetting->Message.Content, WillMessageSetting->Message.Length);
        EndPtr = EndPtr + WillMessageSetting->Message.Length;
    }
    /* User Name */
    if(AuthoritionSetting->UsernameEnable)
    {
        *((uint16_t*)EndPtr) = MQC_htons(AuthoritionSetting->Username.Length);
        EndPtr = EndPtr + sizeof(uint16_t);
        memcpy(EndPtr, AuthoritionSetting->Username.Data, AuthoritionSetting->Username.Length);
        EndPtr = EndPtr + AuthoritionSetting->Username.Length;
    }
    /* Password */
    if(AuthoritionSetting->PasswordEnable)
    {
        *((uint16_t*)EndPtr) = MQC_htons(AuthoritionSetting->Password.Length);
        EndPtr = EndPtr + sizeof(uint16_t);
        memcpy(EndPtr, AuthoritionSetting->Password.Data, AuthoritionSetting->Password.Length);
        EndPtr = EndPtr + AuthoritionSetting->Password.Length;
    }
    *Dstlen = WriteDataSize;
    return (0);
}

/** 
 * @brief               Encode a buffer into MQTT DISCONNECT Message format
 * @param[in]           Dst                     Destination buffer (can be NULL for checking size)
 * @param[in,out]       Dstlen                  \b in   :   Size of the destination buffer \n
 *                                              \b out  :   Number of bytes written
 * @retval              0                       success
 * @retval              -1                      Destination buffer too small
 * @note                Input \a *Dstlen = 0 to obtain the required buffer size in \a *Dstlen
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/21
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_DisconnectMessageEncode(uint8_t* Dst, size_t* Dstlen)
{
    uint32_t    RemainingLength         =   0;
    uint32_t    EncodeRemainingLength   =   0;
    size_t      WriteDataSize           =   0;
    int32_t     Ret                     =   0;
    uint8_t*    EndPtr                  =   NULL;
    
    /* calculate the RemainingLength of DISCONNECT Message */
    RemainingLength = RemainingLength + D_MQC_DISCONNECT_MSG_VARIABLE_HEADER_SIZE;
    
    Ret = prvMQC_RemainingLengthEncode( NULL, &EncodeRemainingLength, RemainingLength );
    if(Ret)
    {
        return (-1);
    }
    /* calculate the Total Message Size of PINGREQ Message */
    WriteDataSize = EncodeRemainingLength + RemainingLength + 1;
    if( (NULL == Dst) || (0 == *Dstlen) )
    {
        *Dstlen = WriteDataSize;
        return (0);
    }
    if(*Dstlen < WriteDataSize)
    {
        /* No enough Buffer */
        return (-1);
    }
    
    /* Make the Message Data */
    EndPtr = Dst;
    
    /* Fixed Header */
    *EndPtr = ( E_MQC_MSG_DISCONNECT << 4 );
    EndPtr++; 
    /* Remaining Length */
    Ret = prvMQC_RemainingLengthEncode( EndPtr, &EncodeRemainingLength, RemainingLength );
    if(Ret)
    {
        return (-1);
    }
    *Dstlen = WriteDataSize;
    return (0);
}
 
/** 
 * @brief               Encode a buffer into MQTT SUBSCRIBE Message format
 * @param[in]           Dst                     Destination buffer (can be NULL for checking size)
 * @param[in,out]       Dstlen                  \b in   :   Size of the destination buffer \n
 *                                              \b out  :   Number of bytes written
 * @param[in]           PacketIdentifier        Packet Identifier
 * @param[in]           TopicFilterList         Topic Filter List
 * @param[in]           QoSList                 Topic Filter QoS List
 * @param[in]           ListNum                 The Count of the topic filter list
 * @param[out]          ExtData                 Extra Data used to store some customized information
 * @retval              0                       success
 * @retval              -1                      Destination buffer too small
 * @note                Input \a *Dstlen = 0 to obtain the required buffer size in \a *Dstlen
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/19
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_SubscribeMessageEncode(uint8_t* Dst, size_t* Dstlen, uint16_t PacketIdentifier, S_MQC_UTF8_DATA* TopicFilterList, E_MQC_QOS_LEVEL* QoSList, uint32_t ListNum, S_MQC_MSG_SUB_DATA* ExtData)
{
    uint32_t    RemainingLength         =   0;
    uint32_t    i                       =   0;
    size_t      WriteDataSize           =   0;
    uint32_t    EncodeRemainingLength   =   0;
    int32_t     Ret                     =   0;
    uint8_t*    EndPtr                  =   NULL;
    
    /* calculate the RemainingLength of SUBSCRIBE Message */
    RemainingLength = RemainingLength + D_MQC_SUBSCRIBE_MSG_VARIABLE_HEADER_SIZE;
    for(i = 0; i < ListNum; i++)
    {
        /* Topic Filter */
        RemainingLength = RemainingLength + sizeof(uint16_t) + TopicFilterList[i].Length;
        /* Requested QoS */
        RemainingLength = RemainingLength + 1;
    }
    Ret = prvMQC_RemainingLengthEncode( NULL, &EncodeRemainingLength, RemainingLength );
    if(Ret)
    {
        return (-1);
    }
    /* calculate the Total Message Size of CONNECT Message */
    WriteDataSize = EncodeRemainingLength + RemainingLength + 1;
    if( (NULL == Dst) || (0 == *Dstlen) )
    {
        *Dstlen = WriteDataSize;
        return (0);
    }
    if(*Dstlen < WriteDataSize)
    {
        /* No enough Buffer */
        return (-1);
    }
    
    /* Make the Message Data */
    EndPtr = Dst;
    
    /* Fixed Header */
    *EndPtr = (E_MQC_MSG_SUBSCRIBE << 4) + (1<<1);
    EndPtr++;
    /* Remaining Length */
    Ret = prvMQC_RemainingLengthEncode( EndPtr, &EncodeRemainingLength, RemainingLength );
    if(Ret)
    {
        return (-1);
    }
    EndPtr = EndPtr + EncodeRemainingLength;
    /* PacketIdentifier */
    *((uint16_t*)EndPtr) = MQC_htons(PacketIdentifier);
    EndPtr = EndPtr + sizeof(uint16_t);
    /* Topic Filter */
    for(i = 0; i < ListNum; i++)
    {
        *((uint16_t*)EndPtr) = MQC_htons(TopicFilterList[i].Length);
        EndPtr = EndPtr + sizeof(uint16_t);
        if(TopicFilterList[i].Length)
        {
            memcpy(EndPtr, TopicFilterList[i].Data, TopicFilterList[i].Length);
        }
        if(ExtData)
        {
            ExtData->TopicFilterList[i].Data = (TopicFilterList[i].Length)?EndPtr:NULL;
            ExtData->TopicFilterList[i].Length = TopicFilterList[i].Length;
            ExtData->ListNum = ListNum;
        }
        EndPtr = EndPtr + TopicFilterList[i].Length;
        *EndPtr = QoSList[i];
        EndPtr++;
    }
    *Dstlen = WriteDataSize;
    return (0);
}

/** 
 * @brief               Encode a buffer into MQTT UNSUBSCRIBE Message format
 * @param[in]           Dst                     Destination buffer (can be NULL for checking size)
 * @param[in,out]       Dstlen                  \b in   :   Size of the destination buffer \n
 *                                              \b out  :   Number of bytes written
 * @param[in]           PacketIdentifier        Packet Identifier
 * @param[in]           TopicFilterList         Topic Filter List
 * @param[in]           ListNum                 The Count of the topic filter list
 * @param[out]          ExtData                 Extra Data used to store some customized information
 * @retval              0                       success
 * @retval              -1                      Destination buffer too small
 * @note                Input \a *Dstlen = 0 to obtain the required buffer size in \a *Dstlen
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/21
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_UnsubscribeMessageEncode(uint8_t* Dst, size_t* Dstlen, uint16_t PacketIdentifier, S_MQC_UTF8_DATA* TopicFilterList, uint32_t ListNum, S_MQC_MSG_UNSUB_DATA* ExtData)
{
    uint32_t    RemainingLength         =   0;
    uint32_t    i                       =   0;
    size_t      WriteDataSize           =   0;
    uint32_t    EncodeRemainingLength   =   0;
    int32_t     Ret                     =   0;
    uint8_t*    EndPtr                  =   NULL;
    
    /* calculate the RemainingLength of UNSUBSCRIBE Message */
    RemainingLength = RemainingLength + D_MQC_UNSUBSCRIBE_MSG_VARIABLE_HEADER_SIZE;
    for(i = 0; i < ListNum; i++)
    {
        /* Topic Filter */
        RemainingLength = RemainingLength + sizeof(uint16_t) + TopicFilterList[i].Length;
    }
    Ret = prvMQC_RemainingLengthEncode( NULL, &EncodeRemainingLength, RemainingLength );
    if(Ret)
    {
        return (-1);
    }
    /* calculate the Total Message Size of CONNECT Message */
    WriteDataSize = EncodeRemainingLength + RemainingLength + 1;
    if( (NULL == Dst) || (0 == *Dstlen) )
    {
        *Dstlen = WriteDataSize;
        return (0);
    }
    if(*Dstlen < WriteDataSize)
    {
        /* No enough Buffer */
        return (-1);
    }
    
    /* Make the Message Data */
    EndPtr = Dst;
    
    /* Fixed Header */
    *EndPtr = (E_MQC_MSG_UNSUBSCRIBE << 4) + (1<<1);
    EndPtr++;
    /* Remaining Length */
    Ret = prvMQC_RemainingLengthEncode( EndPtr, &EncodeRemainingLength, RemainingLength );
    if(Ret)
    {
        return (-1);
    }
    EndPtr = EndPtr + EncodeRemainingLength;
    /* PacketIdentifier */
    *((uint16_t*)EndPtr) = MQC_htons(PacketIdentifier);
    EndPtr = EndPtr + sizeof(uint16_t);
    /* Topic Filter */
    for(i = 0; i < ListNum; i++)
    {
        *((uint16_t*)EndPtr) = MQC_htons(TopicFilterList[i].Length);
        EndPtr = EndPtr + sizeof(uint16_t);
        memcpy(EndPtr, TopicFilterList[i].Data, TopicFilterList[i].Length);
        if(ExtData)
        {
            ExtData->TopicFilterList[i].Data = EndPtr;
            ExtData->TopicFilterList[i].Length = TopicFilterList[i].Length;
            ExtData->ListNum = ListNum;
        }
        EndPtr = EndPtr + TopicFilterList[i].Length;
    }
    *Dstlen = WriteDataSize;
    return (0);
}

/** 
 * @brief               Encode a buffer into MQTT PUBLISH Message format
 * @param[in]           Dst                     Destination buffer (can be NULL for checking size)
 * @param[in,out]       Dstlen                  \b in   :   Size of the destination buffer \n
 *                                              \b out  :   Number of bytes written
 * @param[in]           PacketIdentifier        Packet Identifier
 * @param[in]           Message                 Publish Message
 * @param[in]           Retry                   Retry (DUP) flag of the Publish Message (false: first send / true: retry)
 * @param[in]           QoS                     QoS level of the Publish Message
 * @param[in]           Retain                  If a Retain Message
 * @param[out]          ExtData                 Extra Data used to store some customized information
 * @retval              0                       success
 * @retval              -1                      Destination buffer too small
 * @note                Input \a *Dstlen = 0 to obtain the required buffer size in \a *Dstlen
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/21
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_PublishMessageEncode(uint8_t* Dst, size_t* Dstlen, uint16_t PacketIdentifier, S_MQC_MESSAGE_INFO* Message, bool Retry, E_MQC_QOS_LEVEL QoS, bool Retain, S_MQC_MSG_PUB_DATA* ExtData)
{
    uint32_t    RemainingLength         =   0;
    uint32_t    EncodeRemainingLength   =   0;
    size_t      WriteDataSize           =   0;
    int32_t     Ret                     =   0;
    uint8_t*    EndPtr                  =   NULL;
    
    /* calculate the RemainingLength of PUBLISH Message */
    RemainingLength = RemainingLength + sizeof(uint16_t) + Message->Topic.Length;
    if( E_MQC_QOS_0 != QoS )
    {
        RemainingLength = RemainingLength + sizeof(uint16_t);
    }
    
    RemainingLength = RemainingLength + Message->Length;
    
    Ret = prvMQC_RemainingLengthEncode( NULL, &EncodeRemainingLength, RemainingLength );
    if(Ret)
    {
        return (-1);
    }
    /* calculate the Total Message Size of PUBLISH Message */
    WriteDataSize = EncodeRemainingLength + RemainingLength + 1;
    if( (NULL == Dst) || (0 == *Dstlen) )
    {
        *Dstlen = WriteDataSize;
        return (0);
    }
    if(*Dstlen < WriteDataSize)
    {
        /* No enough Buffer */
        return (-1);
    }
    
    /* Make the Message Data */
    EndPtr = Dst;
    
    /* Fixed Header */
    Retry = Retry ? (1):(0);
    *EndPtr = ( E_MQC_MSG_PUBLISH << 4 ) + 
              ( (( E_MQC_QOS_0 != QoS )?Retry:0) << 3 ) +
              ( QoS << 1 ) +
              ( Retain?(1):(0) );
    EndPtr++; 
    /* Remaining Length */
    Ret = prvMQC_RemainingLengthEncode( EndPtr, &EncodeRemainingLength, RemainingLength );
    if(Ret)
    {
        return (-1);
    }
    EndPtr = EndPtr + EncodeRemainingLength;
    /* Topic Name */
    *((uint16_t*)EndPtr) = MQC_htons(Message->Topic.Length);
    EndPtr = EndPtr + sizeof(uint16_t);
    memcpy(EndPtr, Message->Topic.Data, Message->Topic.Length);
    if(ExtData)
    {
        ExtData->Message.Topic.Data = EndPtr;
        ExtData->Message.Topic.Length = Message->Topic.Length;
    }
    EndPtr = EndPtr + Message->Topic.Length;
    /* PacketIdentifier */
    if( E_MQC_QOS_0 != QoS )
    {
        *((uint16_t*)EndPtr) = MQC_htons(PacketIdentifier);
        EndPtr = EndPtr + sizeof(uint16_t);
    }
    /* Message Content */
    memcpy(EndPtr, Message->Content, Message->Length);
    if(ExtData)
    {
        ExtData->Message.Content = EndPtr;
        ExtData->Message.Length = Message->Length;
    }
    *Dstlen = WriteDataSize;
    return (0);  
}

/** 
 * @brief               Encode a buffer into MQTT PUBACK Message format
 * @param[in]           Dst                     Destination buffer (can be NULL for checking size)
 * @param[in,out]       Dstlen                  \b in   :   Size of the destination buffer \n
 *                                              \b out  :   Number of bytes written
 * @param[in]           PacketIdentifier        Packet Identifier
 * @retval              0                       success
 * @retval              -1                      Destination buffer too small
 * @note                Input \a *Dstlen = 0 to obtain the required buffer size in \a *Dstlen
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/21
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_PubackMessageEncode(uint8_t* Dst, size_t* Dstlen, uint16_t PacketIdentifier)
{
    uint32_t    RemainingLength         =   0;
    uint32_t    EncodeRemainingLength   =   0;
    size_t      WriteDataSize           =   0;
    int32_t     Ret                     =   0;
    uint8_t*    EndPtr                  =   NULL;
    
    /* calculate the RemainingLength of PUBACK Message */
    RemainingLength = RemainingLength + D_MQC_PUBACK_MSG_VARIABLE_HEADER_SIZE;
    
    Ret = prvMQC_RemainingLengthEncode( NULL, &EncodeRemainingLength, RemainingLength );
    if(Ret)
    {
        return (-1);
    }
    /* calculate the Total Message Size of PUBACK Message */
    WriteDataSize = EncodeRemainingLength + RemainingLength + 1;
    if( (NULL == Dst) || (0 == *Dstlen) )
    {
        *Dstlen = WriteDataSize;
        return (0);
    }
    if(*Dstlen < WriteDataSize)
    {
        /* No enough Buffer */
        return (-1);
    }
    
    /* Make the Message Data */
    EndPtr = Dst;
    
    /* Fixed Header */
    *EndPtr = ( E_MQC_MSG_PUBACK << 4 );
    EndPtr++; 
    /* Remaining Length */
    Ret = prvMQC_RemainingLengthEncode( EndPtr, &EncodeRemainingLength, RemainingLength );
    if(Ret)
    {
        return (-1);
    }
    EndPtr = EndPtr + EncodeRemainingLength;
    /* PacketIdentifier */
    *((uint16_t*)EndPtr) = MQC_htons(PacketIdentifier);
    EndPtr = EndPtr + sizeof(uint16_t);
    *Dstlen = WriteDataSize;
    return (0);
}

/** 
 * @brief               Encode a buffer into MQTT PUBREC Message format
 * @param[in]           Dst                     Destination buffer (can be NULL for checking size)
 * @param[in,out]       Dstlen                  \b in   :   Size of the destination buffer \n
 *                                              \b out  :   Number of bytes written
 * @param[in]           PacketIdentifier        Packet Identifier
 * @retval              0                       success
 * @retval              -1                      Destination buffer too small
 * @note                Input \a *Dstlen = 0 to obtain the required buffer size in \a *Dstlen
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/21
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_PubrecMessageEncode(uint8_t* Dst, size_t* Dstlen, uint16_t PacketIdentifier)
{
    uint32_t    RemainingLength         =   0;
    uint32_t    EncodeRemainingLength   =   0;
    size_t      WriteDataSize           =   0;
    int32_t     Ret                     =   0;
    uint8_t*    EndPtr                  =   NULL;
    
    /* calculate the RemainingLength of PUBREC Message */
    RemainingLength = RemainingLength + D_MQC_PUBREC_MSG_VARIABLE_HEADER_SIZE;
    
    Ret = prvMQC_RemainingLengthEncode( NULL, &EncodeRemainingLength, RemainingLength );
    if(Ret)
    {
        return (-1);
    }
    /* calculate the Total Message Size of PUBREC Message */
    WriteDataSize = EncodeRemainingLength + RemainingLength + 1;
    if( (NULL == Dst) || (0 == *Dstlen) )
    {
        *Dstlen = WriteDataSize;
        return (0);
    }
    if(*Dstlen < WriteDataSize)
    {
        /* No enough Buffer */
        return (-1);
    }
    
    /* Make the Message Data */
    EndPtr = Dst;
    
    /* Fixed Header */
    *EndPtr = ( E_MQC_MSG_PUBREC << 4 );
    EndPtr++; 
    /* Remaining Length */
    Ret = prvMQC_RemainingLengthEncode( EndPtr, &EncodeRemainingLength, RemainingLength );
    if(Ret)
    {
        return (-1);
    }
    EndPtr = EndPtr + EncodeRemainingLength;
    /* PacketIdentifier */
    *((uint16_t*)EndPtr) = MQC_htons(PacketIdentifier);
    EndPtr = EndPtr + sizeof(uint16_t);
    *Dstlen = WriteDataSize;
    return (0);
}

/** 
 * @brief               Encode a buffer into MQTT PUBREL Message format
 * @param[in]           Dst                     Destination buffer (can be NULL for checking size)
 * @param[in,out]       Dstlen                  \b in   :   Size of the destination buffer \n
 *                                              \b out  :   Number of bytes written
 * @param[in]           PacketIdentifier        Packet Identifier
 * @retval              0                       success
 * @retval              -1                      Destination buffer too small
 * @note                Input \a *Dstlen = 0 to obtain the required buffer size in \a *Dstlen
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/21
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_PubrelMessageEncode(uint8_t* Dst, size_t* Dstlen, uint16_t PacketIdentifier)
{
    uint32_t    RemainingLength         =   0;
    uint32_t    EncodeRemainingLength   =   0;
    size_t      WriteDataSize           =   0;
    int32_t     Ret                     =   0;
    uint8_t*    EndPtr                  =   NULL;
    
    /* calculate the RemainingLength of PUBREL Message */
    RemainingLength = RemainingLength + D_MQC_PUBREL_MSG_VARIABLE_HEADER_SIZE;
    
    Ret = prvMQC_RemainingLengthEncode( NULL, &EncodeRemainingLength, RemainingLength );
    if(Ret)
    {
        return (-1);
    }
    /* calculate the Total Message Size of PUBREL Message */
    WriteDataSize = EncodeRemainingLength + RemainingLength + 1;
    if( (NULL == Dst) || (0 == *Dstlen) )
    {
        *Dstlen = WriteDataSize;
        return (0);
    }
    if(*Dstlen < WriteDataSize)
    {
        /* No enough Buffer */
        return (-1);
    }
    
    /* Make the Message Data */
    EndPtr = Dst;
    
    /* Fixed Header */
    *EndPtr = ( E_MQC_MSG_PUBREL << 4 ) + ( 1<<1 );
    EndPtr++; 
    /* Remaining Length */
    Ret = prvMQC_RemainingLengthEncode( EndPtr, &EncodeRemainingLength, RemainingLength );
    if(Ret)
    {
        return (-1);
    }
    EndPtr = EndPtr + EncodeRemainingLength;
    /* PacketIdentifier */
    *((uint16_t*)EndPtr) = MQC_htons(PacketIdentifier);
    EndPtr = EndPtr + sizeof(uint16_t);
    *Dstlen = WriteDataSize;
    return (0);
}

/** 
 * @brief               Encode a buffer into MQTT PUBCOMP Message format
 * @param[in]           Dst                     Destination buffer (can be NULL for checking size)
 * @param[in,out]       Dstlen                  \b in   :   Size of the destination buffer \n
 *                                              \b out  :   Number of bytes written
 * @param[in]           PacketIdentifier        Packet Identifier
 * @retval              0                       success
 * @retval              -1                      Destination buffer too small
 * @note                Input \a *Dstlen = 0 to obtain the required buffer size in \a *Dstlen
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/21
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_PubcompMessageEncode(uint8_t* Dst, size_t* Dstlen, uint16_t PacketIdentifier)
{
    uint32_t    RemainingLength         =   0;
    uint32_t    EncodeRemainingLength   =   0;
    size_t      WriteDataSize           =   0;
    int32_t     Ret                     =   0;
    uint8_t*    EndPtr                  =   NULL;
    
    /* calculate the RemainingLength of PUBCOMP Message */
    RemainingLength = RemainingLength + D_MQC_PUBCOMP_MSG_VARIABLE_HEADER_SIZE;
    
    Ret = prvMQC_RemainingLengthEncode( NULL, &EncodeRemainingLength, RemainingLength );
    if(Ret)
    {
        return (-1);
    }
    /* calculate the Total Message Size of PUBCOMP Message */
    WriteDataSize = EncodeRemainingLength + RemainingLength + 1;
    if( (NULL == Dst) || (0 == *Dstlen) )
    {
        *Dstlen = WriteDataSize;
        return (0);
    }
    if(*Dstlen < WriteDataSize)
    {
        /* No enough Buffer */
        return (-1);
    }
    
    /* Make the Message Data */
    EndPtr = Dst;
    
    /* Fixed Header */
    *EndPtr = ( E_MQC_MSG_PUBCOMP << 4 );
    EndPtr++; 
    /* Remaining Length */
    Ret = prvMQC_RemainingLengthEncode( EndPtr, &EncodeRemainingLength, RemainingLength );
    if(Ret)
    {
        return (-1);
    }
    EndPtr = EndPtr + EncodeRemainingLength;
    /* PacketIdentifier */
    *((uint16_t*)EndPtr) = MQC_htons(PacketIdentifier);
    EndPtr = EndPtr + sizeof(uint16_t);
    *Dstlen = WriteDataSize;
    return (0);
}

/** 
 * @brief               Encode a buffer into MQTT PINGREQ Message format
 * @param[in]           Dst                     Destination buffer (can be NULL for checking size)
 * @param[in,out]       Dstlen                  \b in   :   Size of the destination buffer \n
 *                                              \b out  :   Number of bytes written
 * @retval              0                       success
 * @retval              -1                      Destination buffer too small
 * @note                Input \a *Dstlen = 0 to obtain the required buffer size in \a *Dstlen
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/21
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_PingreqMessageEncode(uint8_t* Dst, size_t* Dstlen)
{
    uint32_t    RemainingLength         =   0;
    uint32_t    EncodeRemainingLength   =   0;
    size_t      WriteDataSize           =   0;
    int32_t     Ret                     =   0;
    uint8_t*    EndPtr                  =   NULL;
    
    /* calculate the RemainingLength of PINGREQ Message */
    RemainingLength = RemainingLength + D_MQC_PINGREQ_MSG_VARIABLE_HEADER_SIZE;
    
    Ret = prvMQC_RemainingLengthEncode( NULL, &EncodeRemainingLength, RemainingLength );
    if(Ret)
    {
        return (-1);
    }
    /* calculate the Total Message Size of PINGREQ Message */
    WriteDataSize = EncodeRemainingLength + RemainingLength + 1;
    if( (NULL == Dst) || (0 == *Dstlen) )
    {
        *Dstlen = WriteDataSize;
        return (0);
    }
    if(*Dstlen < WriteDataSize)
    {
        /* No enough Buffer */
        return (-1);
    }
    
    /* Make the Message Data */
    EndPtr = Dst;
    
    /* Fixed Header */
    *EndPtr = ( E_MQC_MSG_PINGREQ << 4 );
    EndPtr++; 
    /* Remaining Length */
    Ret = prvMQC_RemainingLengthEncode( EndPtr, &EncodeRemainingLength, RemainingLength );
    if(Ret)
    {
        return (-1);
    }
    *Dstlen = WriteDataSize;
    return (0);
}

/** 
 * @brief               Release the received data
 * @param[in,out]       MQCHandler              MQTT client handler
 * @return              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/04/12
 * @callgraph
 * @callergraph
 */
static void prvMQC_PackageFree( S_MQC_SESSION_HANDLE* MQCHandler )
{
    if(MQCHandler->SessionCtx.RecvData)
    {
        MQCHandler->FreeFunc(MQCHandler->SessionCtx.RecvData);
    }
    MQCHandler->SessionCtx.RecvData = NULL;
    MQCHandler->SessionCtx.RecvDataSize = 0;
    MQCHandler->SessionCtx.TotalRecvDataSize = 0;
    MQCHandler->SessionCtx.HeaderDataSize = 0;
    return;
}

/** 
 * @brief               Notify User there is Message that is discarded
 * @param[in,out]       MQCHandler      MQTT client handler
 * @param[in]           Message         Discarded Message
 * @param[in]           Result          Discard Reasion
 * @retval              0               success
 * @retval              -1              fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMessageDiscardNotify(S_MQC_SESSION_HANDLE* MQCHandler, S_MQC_MSG_CTX*  Message, E_MQC_BEHAVIOR_RESULT Result)
{
    int32_t Ret = 0;
    switch( Message->MsgData[0] >> 4 )
    {
        case E_MQC_MSG_SUBSCRIBE:
            D_MQC_CALLBACK_SAFECALL(Ret, Message->ExtData.Subscribe.ResultFuncCB, Result, Message->ExtData.Subscribe.TopicFilterList, NULL, Message->ExtData.Subscribe.ListNum);
            break;
        case E_MQC_MSG_UNSUBSCRIBE:
            D_MQC_CALLBACK_SAFECALL(Ret, Message->ExtData.UnSubscribe.ResultFuncCB, Result, Message->ExtData.UnSubscribe.TopicFilterList, Message->ExtData.UnSubscribe.ListNum);
            break;
        case E_MQC_MSG_PUBLISH:
            D_MQC_CALLBACK_SAFECALL(Ret, Message->ExtData.Publish.ResultFuncCB, Result, &(Message->ExtData.Publish.Message));
            break;
        default:
            /* Do nothing */
            break;
    }
    return Ret;
}

/** 
 * @brief               Send CONNECT Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           CleanSession            Session Clean Flag
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_CoreConnect(S_MQC_SESSION_HANDLE* MQCHandler, bool CleanSession)
{
    size_t      WriteDataSize   =   0;  
    uint8_t*    WriteData       =   NULL;
    int32_t     Ret             =   D_MQC_RET_OK;
    
    do
    {
        /* Get the data size */
        Ret = prvMQC_ConnectMessageEncode( WriteData, &WriteDataSize, CleanSession, &(MQCHandler->WillMessage), 
                                         &(MQCHandler->Authorition), MQCHandler->KeepAliveInterval, &(MQCHandler->ClientId) );
        if(Ret)
        {
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
        }
        
        /* alloc memory to buffer the send data */
        WriteData = MQCHandler->MallocFunc(WriteDataSize);
        if(!WriteData)
        {
            Ret = D_MQC_RET_NO_MEMORY;
            break;
        }
        
        /* Encode CONNECT Message data */
        Ret = prvMQC_ConnectMessageEncode( WriteData, &WriteDataSize, CleanSession, &(MQCHandler->WillMessage), 
                                         &(MQCHandler->Authorition), MQCHandler->KeepAliveInterval, &(MQCHandler->ClientId) );
        if(Ret)
        {
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
        }
        
        /* Use callback function to send data */
        Ret = MQCHandler->WriteFuncCB(MQCHandler->UsrCtx, WriteData, WriteDataSize);
        
        if(Ret)
        {
            Ret = D_MQC_RET_CALLBACK_ERROR;
        }
        else
        {
            Ret = D_MQC_RET_OK;
        }
    }while(0);
    
    /* Free the malloc memory */
    if(WriteData)
    {
        MQCHandler->FreeFunc(WriteData);
        WriteData = NULL;
    }
    
    return Ret;
}

/** 
 * @brief               Clean the local Session Data 
 * @param[in,out]       MQCHandler              MQTT client handler
 * @return              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static void prvMQC_CoreCleanSession(S_MQC_SESSION_HANDLE* MQCHandler)
{
    S_MQC_MSG_CTX*  Message =   NULL;
    
    do
    {
        Message = MQC_MsgQueue_pop(&(MQCHandler->SessionCtx.MessageQueue));
        if(Message)
        {
            /* Notify the application this message discarded via callback function */
            (void)prvMessageDiscardNotify(MQCHandler, Message, E_MQC_BEHAVIOR_CANCEL);
            MQCHandler->FreeFunc(Message->MsgData);
            MQCHandler->FreeFunc(Message);
        }
    }while(Message);
    
    return;
}

/** 
 * @brief               Send DISCONNECT Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_CoreDisconnect(S_MQC_SESSION_HANDLE* MQCHandler)
{
    size_t          WriteDataSize   =   0;  
    uint8_t*        WriteData       =   NULL;
    int32_t         Ret             =   D_MQC_RET_OK;
    
    do
    {
        /* Get the data size */
        Ret = prvMQC_DisconnectMessageEncode( WriteData, &WriteDataSize );
        
        if(Ret)
        {
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
        }
        
        /* alloc memory to buffer the send data */
        WriteData = MQCHandler->MallocFunc(WriteDataSize);
        if(!WriteData)
        {
            Ret = D_MQC_RET_NO_MEMORY;
            break;
        }
        
        /* Encode PUBLISH Message data */
        Ret = prvMQC_DisconnectMessageEncode( WriteData, &WriteDataSize );
        if(Ret)
        {
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
        }
        
        /* Use callback function to send data */
        Ret = MQCHandler->WriteFuncCB(MQCHandler->UsrCtx, WriteData, WriteDataSize);
        if(Ret)
        {
            Ret = D_MQC_RET_CALLBACK_ERROR;
            break;
        }
        
        Ret = D_MQC_RET_OK;
        
    }while(0);
    
    /* Free the malloc memory */
    if(WriteData)
    {
        MQCHandler->FreeFunc(WriteData);
        WriteData = NULL;
    }
    
    return Ret;
}

/** 
 * @brief               Send SUBSCRIBE Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           TopicFilterList         Topic Filter List
 * @param[in]           QoSList                 Topic Filter QoS List
 * @param[in]           ListNum                 The Count of the topic filter list
 * @param[in]           ResultFuncCB            Callback function will be called when received response
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_CoreSubscribe(S_MQC_SESSION_HANDLE* MQCHandler, S_MQC_UTF8_DATA* TopicFilterList, E_MQC_QOS_LEVEL* QoSList, uint32_t ListNum, F_SUBSCRIBE_RES_CBFUNC ResultFuncCB)
{
    size_t          WriteDataSize   =   0;  
    uint8_t*        WriteData       =   NULL;
    S_MQC_MSG_CTX*  PacketCtx       =   NULL;
    int32_t         Ret             =   D_MQC_RET_OK;
    
    do
    {
        MQCHandler->SessionCtx.MessageQueue.PacketIdentifier = (65535 == MQCHandler->SessionCtx.MessageQueue.PacketIdentifier)?1:(MQCHandler->SessionCtx.MessageQueue.PacketIdentifier+1);
        
        /* Get the data size */
        Ret = prvMQC_SubscribeMessageEncode( WriteData, &WriteDataSize, MQCHandler->SessionCtx.MessageQueue.PacketIdentifier, TopicFilterList, QoSList, ListNum, NULL );
        if(Ret)
        {
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
        }
        
        /* alloc memory to buffer the send data */
        WriteData = MQCHandler->MallocFunc(WriteDataSize);
        if(!WriteData)
        {
            Ret = D_MQC_RET_NO_MEMORY;
            break;
        }
        
        /* alloc memory to buffer the message in queue */
        PacketCtx = MQCHandler->MallocFunc(sizeof(S_MQC_MSG_CTX) + ListNum * sizeof(S_MQC_UTF8_DATA));
        if(!PacketCtx)
        {
            Ret = D_MQC_RET_NO_MEMORY;
            break;
        }
        
        /* Encode SUBSCRIBE Message data */
        Ret = prvMQC_SubscribeMessageEncode( WriteData, &WriteDataSize, MQCHandler->SessionCtx.MessageQueue.PacketIdentifier, TopicFilterList, QoSList, ListNum, &(PacketCtx->ExtData.Subscribe) );
        if(Ret)
        {
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
        }
        
        PacketCtx->SendCount                        =   MQCHandler->MessageRetryCount;
        PacketCtx->ExpireTime                       =   MQCHandler->MessageRetryInterval*1000;
        PacketCtx->Timeout                          =   MQCHandler->MessageRetryInterval*1000;
        PacketCtx->MsgLength                        =   WriteDataSize;
        PacketCtx->MsgData                          =   WriteData;
        PacketCtx->PacketIdentifier                 =   MQCHandler->SessionCtx.MessageQueue.PacketIdentifier;
        PacketCtx->ExtData.Subscribe.ResultFuncCB   =   ResultFuncCB;
        
        /* Push the Message data in queue */
        PacketCtx = MQC_MsgQueue_push( &(MQCHandler->SessionCtx.MessageQueue), PacketCtx );
        
        /* Use callback function to send data */
        (void)MQCHandler->WriteFuncCB(MQCHandler->UsrCtx, WriteData, WriteDataSize);
        
        if(PacketCtx)
        {
            /* Notify the application this message discarded via callback function */
            Ret = prvMessageDiscardNotify(MQCHandler, PacketCtx, E_MQC_BEHAVIOR_CANCEL);
            WriteData = PacketCtx->MsgData;
        }
        else
        {
            WriteData = NULL;
        }
        
        Ret = D_MQC_RET_OK;
        
    }while(0);
    
    /* Free the malloc memory */
    if(WriteData)
    {
        MQCHandler->FreeFunc(WriteData);
        WriteData = NULL;
    }
    
    if(PacketCtx)
    {
        MQCHandler->FreeFunc(PacketCtx);
        PacketCtx = NULL;
    }
    
    return Ret;
}

/** 
 * @brief               Send UNSUBSCRIBE Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           TopicFilterList         Topic Filter List
 * @param[in]           ListNum                 The Count of the topic filter list
 * @param[in]           ResultFuncCB            Callback function will be called when received response
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_CoreUnSubscribe(S_MQC_SESSION_HANDLE* MQCHandler, S_MQC_UTF8_DATA* TopicFilterList, uint32_t ListNum, F_UNSUBSCRIBE_RES_CBFUNC ResultFuncCB)
{
    size_t          WriteDataSize   =   0;  
    uint8_t*        WriteData       =   NULL;
    S_MQC_MSG_CTX*  PacketCtx       =   NULL;
    int32_t         Ret             =   D_MQC_RET_OK;
    
    do
    {
        MQCHandler->SessionCtx.MessageQueue.PacketIdentifier = (65535 == MQCHandler->SessionCtx.MessageQueue.PacketIdentifier)?1:(MQCHandler->SessionCtx.MessageQueue.PacketIdentifier+1);
        
        /* Get the data size */
        Ret = prvMQC_UnsubscribeMessageEncode( WriteData, &WriteDataSize, MQCHandler->SessionCtx.MessageQueue.PacketIdentifier, TopicFilterList, ListNum, NULL );
        if(Ret)
        {
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
        }
        
        /* alloc memory to buffer the send data */
        WriteData = MQCHandler->MallocFunc(WriteDataSize);
        if(!WriteData)
        {
            Ret = D_MQC_RET_NO_MEMORY;
            break;
        }
        
        /* alloc memory to buffer the message in queue */
        PacketCtx = MQCHandler->MallocFunc(sizeof(S_MQC_MSG_CTX) + ListNum * sizeof(S_MQC_UTF8_DATA));
        if(!PacketCtx)
        {
            Ret = D_MQC_RET_NO_MEMORY;
            break;
        }
        
        /* Encode UNSUBSCRIBE Message data */
        Ret = prvMQC_UnsubscribeMessageEncode( WriteData, &WriteDataSize, MQCHandler->SessionCtx.MessageQueue.PacketIdentifier, TopicFilterList, ListNum, &(PacketCtx->ExtData.UnSubscribe) );
        if(Ret)
        {
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
        }
        
        PacketCtx->SendCount                        =   MQCHandler->MessageRetryCount;
        PacketCtx->ExpireTime                       =   MQCHandler->MessageRetryInterval*1000;
        PacketCtx->Timeout                          =   MQCHandler->MessageRetryInterval*1000;
        PacketCtx->MsgLength                        =   WriteDataSize;
        PacketCtx->MsgData                          =   WriteData;
        PacketCtx->PacketIdentifier                 =   MQCHandler->SessionCtx.MessageQueue.PacketIdentifier;
        PacketCtx->ExtData.UnSubscribe.ResultFuncCB =   ResultFuncCB;
        
        /* Push the Message data in queue */
        PacketCtx = MQC_MsgQueue_push( &(MQCHandler->SessionCtx.MessageQueue), PacketCtx);
        
        /* Use callback function to send data */
        (void)MQCHandler->WriteFuncCB(MQCHandler->UsrCtx, WriteData, WriteDataSize);
        
        if(PacketCtx)
        {
            /* Notify the application this message discarded via callback function */
            Ret = prvMessageDiscardNotify(MQCHandler, PacketCtx, E_MQC_BEHAVIOR_CANCEL);
            WriteData = PacketCtx->MsgData;
        }
        else
        {
            WriteData = NULL;
        }
        
        Ret = D_MQC_RET_OK;
        
    }while(0);
    
    /* Free the malloc memory */
    if(WriteData)
    {
        MQCHandler->FreeFunc(WriteData);
        WriteData = NULL;
    }
    
    if(PacketCtx)
    {
        MQCHandler->FreeFunc(PacketCtx);
        PacketCtx = NULL;
    }
    
    return Ret;
}

/** 
 * @brief               Send PUBLISH Message (QoS Level = 0)
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           Message                 Message Content
 * @param[in]           Retain                  Retain Message Flag
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @note                QoS0 level Message has no response
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_CorePublish_withoutQoS(S_MQC_SESSION_HANDLE* MQCHandler, S_MQC_MESSAGE_INFO* Message, bool Retain)
{
    size_t          WriteDataSize   =   0;  
    uint8_t*        WriteData       =   NULL;
    int32_t         Ret             =   D_MQC_RET_OK;
    
    do
    {
        /* Get the data size */
        Ret = prvMQC_PublishMessageEncode( WriteData, &WriteDataSize, 0, Message, false, E_MQC_QOS_0, Retain, NULL );
        if(Ret)
        {
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
        }
        
        /* alloc memory to buffer the send data */
        WriteData = MQCHandler->MallocFunc(WriteDataSize);
        if(!WriteData)
        {
            Ret = D_MQC_RET_NO_MEMORY;
            break;
        }
        
        /* Encode UNSUBSCRIBE Message data */
        Ret = prvMQC_PublishMessageEncode( WriteData, &WriteDataSize, 0, Message, false, E_MQC_QOS_0, Retain, NULL );
        if(Ret)
        {
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
        }
        
        /* Use callback function to send data */
        Ret = MQCHandler->WriteFuncCB(MQCHandler->UsrCtx, WriteData, WriteDataSize);
        if(Ret)
        {
            Ret = D_MQC_RET_CALLBACK_ERROR;
            break;
        }
        
        Ret = D_MQC_RET_OK;
        
    }while(0);
    
    /* Free the malloc memory */
    if(WriteData)
    {
        MQCHandler->FreeFunc(WriteData);
        WriteData = NULL;
    }
    
    return Ret;
}

/** 
 * @brief               Send PUBLISH Message (QoS Level = 1 or 2)
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           Message                 Message Content
 * @param[in]           QoS                     QoS Level
 * @param[in]           Retain                  Retain Message Flag
 * @param[in]           ResultFuncCB            Callback function will be called when received response
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @note                Response callback called means server has received the message 
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_CorePublish_withQoS(S_MQC_SESSION_HANDLE* MQCHandler, S_MQC_MESSAGE_INFO* Message, E_MQC_QOS_LEVEL QoS, bool Retain, F_PUBLISH_RES_CBFUNC ResultFuncCB)
{
    size_t          WriteDataSize   =   0;  
    uint8_t*        WriteData       =   NULL;
    S_MQC_MSG_CTX*  PacketCtx       =   NULL;
    int32_t         Ret             =   D_MQC_RET_OK;
    
    do
    {
        MQCHandler->SessionCtx.MessageQueue.PacketIdentifier = (65535 == MQCHandler->SessionCtx.MessageQueue.PacketIdentifier)?1:(MQCHandler->SessionCtx.MessageQueue.PacketIdentifier+1);
        
        /* Get the data size */
        Ret = prvMQC_PublishMessageEncode( WriteData, &WriteDataSize, MQCHandler->SessionCtx.MessageQueue.PacketIdentifier, Message, false, QoS, Retain, NULL );
        if(Ret)
        {
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
        }
        
        /* alloc memory to buffer the send data */
        WriteData = MQCHandler->MallocFunc(WriteDataSize);
        if(!WriteData)
        {
            Ret = D_MQC_RET_NO_MEMORY;
            break;
        }
        
        /* alloc memory to buffer the message in queue */
        PacketCtx = MQCHandler->MallocFunc(sizeof(S_MQC_MSG_CTX));
        if(!PacketCtx)
        {
            Ret = D_MQC_RET_NO_MEMORY;
            break;
        }
        
        /* Encode UNSUBSCRIBE Message data */
        Ret = prvMQC_PublishMessageEncode( WriteData, &WriteDataSize, MQCHandler->SessionCtx.MessageQueue.PacketIdentifier, Message, false, QoS, Retain, &(PacketCtx->ExtData.Publish) );
        if(Ret)
        {
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
        }
        
        PacketCtx->SendCount                        =   MQCHandler->MessageRetryCount;
        PacketCtx->ExpireTime                       =   MQCHandler->MessageRetryInterval*1000;
        PacketCtx->Timeout                          =   MQCHandler->MessageRetryInterval*1000;
        PacketCtx->MsgLength                        =   WriteDataSize;
        PacketCtx->MsgData                          =   WriteData;
        PacketCtx->PacketIdentifier                 =   MQCHandler->SessionCtx.MessageQueue.PacketIdentifier;
        PacketCtx->ExtData.Publish.ResultFuncCB     =   ResultFuncCB;
        
        /* Push the Message data in queue */
        PacketCtx = MQC_MsgQueue_push( &(MQCHandler->SessionCtx.MessageQueue), PacketCtx);
        
        /* Use callback function to send data */
        (void)MQCHandler->WriteFuncCB(MQCHandler->UsrCtx, WriteData, WriteDataSize);
        /* Set DUP (retry) flag to true */
        CLIB_BIT_SET(WriteData[0], 3);
        
        if(PacketCtx)
        {
            /* Notify the application this message discarded via callback function */
            Ret = prvMessageDiscardNotify(MQCHandler, PacketCtx, E_MQC_BEHAVIOR_CANCEL);
            WriteData = PacketCtx->MsgData;
        }
        else
        {
            WriteData = NULL;
        }
        
        Ret = D_MQC_RET_OK;
        
    }while(0);
    
    /* Free the malloc memory */
    if(WriteData)
    {
        MQCHandler->FreeFunc(WriteData);
        WriteData = NULL;
    }
    
    if(PacketCtx)
    {
        MQCHandler->FreeFunc(PacketCtx);
        PacketCtx = NULL;
    }
    
    return Ret;
}

/** 
 * @brief               Send PUBLISH Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           Message                 Message Content
 * @param[in]           QoS                     QoS Level
 * @param[in]           Retain                  Retain Message Flag
 * @param[in]           ResultFuncCB            Callback function will be called when received response
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @note                If QoS Level = 0, the ResultFuncCB param will be discarded. (Result will be returned immediately by the function)
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_CorePublish(S_MQC_SESSION_HANDLE* MQCHandler, S_MQC_MESSAGE_INFO* Message, E_MQC_QOS_LEVEL QoS, bool Retain, F_PUBLISH_RES_CBFUNC ResultFuncCB)
{
    int32_t         Ret             =   D_MQC_RET_OK;
    
    if(E_MQC_QOS_0 == QoS)
    {
        Ret = prvMQC_CorePublish_withoutQoS(MQCHandler, Message, Retain);
    }
    else
    {
        Ret = prvMQC_CorePublish_withQoS(MQCHandler, Message, QoS, Retain, ResultFuncCB);
    }
    
    return Ret;
}

/** 
 * @brief               Send PUBACK Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           PacketIdentifier        Packet Identifier
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_CorePuback(S_MQC_SESSION_HANDLE* MQCHandler, uint16_t PacketIdentifier)
{
    size_t          WriteDataSize   =   0;  
    uint8_t*        WriteData       =   NULL;
    int32_t         Ret             =   D_MQC_RET_OK;
    
    do
    {
        /* Get the data size */
        Ret = prvMQC_PubackMessageEncode( WriteData, &WriteDataSize, PacketIdentifier );
        if(Ret)
        {
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
        }
        
        /* alloc memory to buffer the send data */
        WriteData = MQCHandler->MallocFunc(WriteDataSize);
        if(!WriteData)
        {
            Ret = D_MQC_RET_NO_MEMORY;
            break;
        }
        
        /* Encode PUBACK Message data */
        Ret = prvMQC_PubackMessageEncode( WriteData, &WriteDataSize, PacketIdentifier );
        if(Ret)
        {
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
        }
        
        /* Use callback function to send data */
        Ret = MQCHandler->WriteFuncCB(MQCHandler->UsrCtx, WriteData, WriteDataSize);
        if(Ret)
        {
            Ret = D_MQC_RET_CALLBACK_ERROR;
            break;
        }
        
        Ret = D_MQC_RET_OK;
        
    }while(0);
    
    /* Free the malloc memory */
    if(WriteData)
    {
        MQCHandler->FreeFunc(WriteData);
        WriteData = NULL;
    }
    
    return Ret;
}

/** 
 * @brief               Send PUBREC Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           PacketIdentifier        Packet Identifier
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_CorePubrec(S_MQC_SESSION_HANDLE* MQCHandler, uint16_t PacketIdentifier)
{
    size_t          WriteDataSize   =   0;  
    uint8_t*        WriteData       =   NULL;
    S_MQC_MSG_CTX*  PacketCtx       =   NULL;
    int32_t         Ret             =   D_MQC_RET_OK;
    
    do
    {
        /* Get the data size */
        Ret = prvMQC_PubrecMessageEncode( WriteData, &WriteDataSize, PacketIdentifier );
        if(Ret)
        {
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
        }
        
        /* alloc memory to buffer the send data */
        WriteData = MQCHandler->MallocFunc(WriteDataSize);
        if(!WriteData)
        {
            Ret = D_MQC_RET_NO_MEMORY;
            break;
        }
        
        /* alloc memory to buffer the message in queue */
        PacketCtx = MQCHandler->MallocFunc(sizeof(S_MQC_MSG_CTX));
        if(!PacketCtx)
        {
            Ret = D_MQC_RET_NO_MEMORY;
            break;
        }
        
        /* Encode UNSUBSCRIBE Message data */
        Ret = prvMQC_PubrecMessageEncode( WriteData, &WriteDataSize, PacketIdentifier );
        if(Ret)
        {
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
        }
        
        PacketCtx->SendCount                        =   MQCHandler->MessageRetryCount;
        PacketCtx->ExpireTime                       =   MQCHandler->MessageRetryInterval*1000;
        PacketCtx->Timeout                          =   MQCHandler->MessageRetryInterval*1000;
        PacketCtx->MsgLength                        =   WriteDataSize;
        PacketCtx->MsgData                          =   WriteData;
        PacketCtx->PacketIdentifier                 =   PacketIdentifier;
        
        /* Push the Message data in queue */
        PacketCtx = MQC_MsgQueue_push( &(MQCHandler->SessionCtx.MessageQueue), PacketCtx);
        
        /* Use callback function to send data */
        (void)MQCHandler->WriteFuncCB(MQCHandler->UsrCtx, WriteData, WriteDataSize);
        
        if(PacketCtx)
        {
            /* Notify the application this message discarded via callback function */
            Ret = prvMessageDiscardNotify(MQCHandler, PacketCtx, E_MQC_BEHAVIOR_CANCEL);
            WriteData = PacketCtx->MsgData;
        }
        else
        {
            WriteData = NULL;
        }
        
        Ret = D_MQC_RET_OK;
        
    }while(0);
    
    /* Free the malloc memory */
    if(WriteData)
    {
        MQCHandler->FreeFunc(WriteData);
        WriteData = NULL;
    }
    
    if(PacketCtx)
    {
        MQCHandler->FreeFunc(PacketCtx);
        PacketCtx = NULL;
    }
    
    return Ret;
}

/** 
 * @brief               Send PUBREL Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           PacketIdentifier        Packet Identifier
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_CorePubrel(S_MQC_SESSION_HANDLE* MQCHandler, uint16_t PacketIdentifier)
{
    size_t          WriteDataSize   =   0;  
    uint8_t*        WriteData       =   NULL;
    S_MQC_MSG_CTX*  PacketCtx       =   NULL;
    int32_t         Ret             =   D_MQC_RET_OK;
    
    do
    {
        /* Get the data size */
        Ret = prvMQC_PubrelMessageEncode( WriteData, &WriteDataSize, PacketIdentifier );
        if(Ret)
        {
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
        }
        
        /* alloc memory to buffer the send data */
        WriteData = MQCHandler->MallocFunc(WriteDataSize);
        if(!WriteData)
        {
            Ret = D_MQC_RET_NO_MEMORY;
            break;
        }
        
        /* alloc memory to buffer the message in queue */
        PacketCtx = MQCHandler->MallocFunc(sizeof(S_MQC_MSG_CTX));
        if(!PacketCtx)
        {
            Ret = D_MQC_RET_NO_MEMORY;
            break;
        }
        
        /* Encode UNSUBSCRIBE Message data */
        Ret = prvMQC_PubrelMessageEncode( WriteData, &WriteDataSize, PacketIdentifier );
        if(Ret)
        {
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
        }
        
        PacketCtx->SendCount                        =   MQCHandler->MessageRetryCount;
        PacketCtx->ExpireTime                       =   MQCHandler->MessageRetryInterval*1000;
        PacketCtx->Timeout                          =   MQCHandler->MessageRetryInterval*1000;
        PacketCtx->MsgLength                        =   WriteDataSize;
        PacketCtx->MsgData                          =   WriteData;
        PacketCtx->PacketIdentifier                 =   PacketIdentifier;
        
        /* Push the Message data in queue */
        PacketCtx = MQC_MsgQueue_push( &(MQCHandler->SessionCtx.MessageQueue), PacketCtx);
        
        /* Use callback function to send data */
        (void)MQCHandler->WriteFuncCB(MQCHandler->UsrCtx, WriteData, WriteDataSize);
        
        if(PacketCtx)
        {
            /* Notify the application this message discarded via callback function */
            Ret = prvMessageDiscardNotify(MQCHandler, PacketCtx, E_MQC_BEHAVIOR_CANCEL);
            WriteData = PacketCtx->MsgData;
        }
        else
        {
            WriteData = NULL;
        }
        
        Ret = D_MQC_RET_OK;
        
    }while(0);
    
    /* Free the malloc memory */
    if(WriteData)
    {
        MQCHandler->FreeFunc(WriteData);
        WriteData = NULL;
    }
    
    if(PacketCtx)
    {
        MQCHandler->FreeFunc(PacketCtx);
        PacketCtx = NULL;
    }
    
    return Ret;
}

/** 
 * @brief               Send PUBCOMP Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           PacketIdentifier        Packet Identifier
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_CorePubcomp(S_MQC_SESSION_HANDLE* MQCHandler, uint16_t PacketIdentifier)
{
    size_t          WriteDataSize   =   0;  
    uint8_t*        WriteData       =   NULL;
    int32_t         Ret             =   D_MQC_RET_OK;
    
    do
    {
        /* Get the data size */
        Ret = prvMQC_PubcompMessageEncode( WriteData, &WriteDataSize, PacketIdentifier );
        if(Ret)
        {
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
        }
        
        /* alloc memory to buffer the send data */
        WriteData = MQCHandler->MallocFunc(WriteDataSize);
        if(!WriteData)
        {
            Ret = D_MQC_RET_NO_MEMORY;
            break;
        }
        
        /* Encode PUBACK Message data */
        Ret = prvMQC_PubcompMessageEncode( WriteData, &WriteDataSize, PacketIdentifier );
        if(Ret)
        {
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
        }
        
        /* Use callback function to send data */
        Ret = MQCHandler->WriteFuncCB(MQCHandler->UsrCtx, WriteData, WriteDataSize);
        if(Ret)
        {
            Ret = D_MQC_RET_CALLBACK_ERROR;
            break;
        }
        
        Ret = D_MQC_RET_OK;
        
    }while(0);
    
    /* Free the malloc memory */
    if(WriteData)
    {
        MQCHandler->FreeFunc(WriteData);
        WriteData = NULL;
    }
    
    return Ret;
}

/** 
 * @brief               Send PUBCOMP Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_CorePing(S_MQC_SESSION_HANDLE* MQCHandler)
{
    size_t          WriteDataSize   =   0;  
    uint8_t*        WriteData       =   NULL;
    int32_t         Ret             =   D_MQC_RET_OK;
    
    do
    {
        /* Get the data size */
        Ret = prvMQC_PingreqMessageEncode( WriteData, &WriteDataSize);
        if(Ret)
        {
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
        }
        
        /* alloc memory to buffer the send data */
        WriteData = MQCHandler->MallocFunc(WriteDataSize);
        if(!WriteData)
        {
            Ret = D_MQC_RET_NO_MEMORY;
            break;
        }
        
        /* Encode UNSUBSCRIBE Message data */
        Ret = prvMQC_PingreqMessageEncode( WriteData, &WriteDataSize );
        if(Ret)
        {
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
        }
        
        /* Use callback function to send data */
        Ret = MQCHandler->WriteFuncCB(MQCHandler->UsrCtx, WriteData, WriteDataSize);
        if(Ret)
        {
            Ret = D_MQC_RET_CALLBACK_ERROR;
            break;
        }
        
        Ret = D_MQC_RET_OK;
        
    }while(0);
    
    /* Free the malloc memory */
    if(WriteData)
    {
        MQCHandler->FreeFunc(WriteData);
        WriteData = NULL;
    }
    
    return Ret;
}

/** 
 * @brief               Read the Remaining Length of Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_BAD_FORMAT
 * @retval              D_MQC_RET_NO_MEMORY
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_ReadMessageRemainingLength( S_MQC_SESSION_HANDLE* MQCHandler )
{
    int32_t     Ret         =   D_MQC_RET_OK;
    uint8_t*    StartPtr    =   NULL;
    
    do
    {
        Ret = prvMQC_RemainingLengthDecode( MQCHandler->SessionCtx.RecvData + 1, MQCHandler->SessionCtx.RecvDataSize - 1, &(MQCHandler->SessionCtx.TotalRecvDataSize) );
        if(-1 == Ret)
        {
            /* Bad Format */
            Ret = D_MQC_RET_BAD_FORMAT;
            break;
        }
        else if(1 == Ret)
        {
            /* continue to receive Remaining Length data */
            Ret = D_MQC_RET_OK;
        }
        else
        {
            /* Get the total message length  */
            MQCHandler->SessionCtx.HeaderDataSize = MQCHandler->SessionCtx.RecvDataSize;
            MQCHandler->SessionCtx.TotalRecvDataSize += MQCHandler->SessionCtx.HeaderDataSize;
            if(D_MQC_MAX_MESSAGE_HEADER_SIZE < MQCHandler->SessionCtx.TotalRecvDataSize)
            {
                StartPtr = MQCHandler->MallocFunc(MQCHandler->SessionCtx.TotalRecvDataSize);
                if(!StartPtr)
                {
                    Ret = D_MQC_RET_NO_MEMORY;
                    break;
                }
                memcpy(StartPtr, MQCHandler->SessionCtx.RecvData, MQCHandler->SessionCtx.RecvDataSize);
                MQCHandler->FreeFunc(MQCHandler->SessionCtx.RecvData);
                MQCHandler->SessionCtx.RecvData = StartPtr;
            }
            Ret = D_MQC_RET_OK;
        }
        
        return Ret;
        
    }while(0);
    
    /* package free */
    prvMQC_PackageFree(MQCHandler);
        
    return Ret;
}

/** 
 * @brief               Check and Process CONNACK Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           FixedHeader             Fixed Header
 * @param[in]           Data                    Message variable header and payload
 * @param[in]           DataSize                Message remaining length (variable header size + payload size)
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_BAD_SEQUEUE
 * @retval              D_MQC_RET_BAD_FORMAT
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_processConnack(S_MQC_SESSION_HANDLE* MQCHandler, uint8_t FixedHeader, uint8_t* Data, uint32_t DataSize )
{
    int32_t                 Ret             =   D_MQC_RET_OK;
    E_MQC_BEHAVIOR_RESULT   Result          =   E_MQC_BEHAVIOR_COMPLETE;
    bool                    SessionPresent  =   false;
    
    do
    {
        /* Status check */
        if(E_MQC_STATUS_WORK == MQCHandler->SessionCtx.Status)
        {
            /* Bad Sequence */
            Ret = D_MQC_RET_BAD_SEQUEUE;
            break;
        }
        
        if( 0 != (FixedHeader & 0x0f) )
        {
            /* Bad Format */
            Ret = D_MQC_RET_BAD_FORMAT;
            break;
        }
        
        if(2 != DataSize)
        {
            /* Bad Format */
            Ret = D_MQC_RET_BAD_FORMAT;
            break;
        }
        
        if( 0 != (Data[0] & 0xfe) )
        {
            /* Bad Format */
            Ret = D_MQC_RET_BAD_FORMAT;
            break;
        }
        
        SessionPresent = CLIB_BIT_CHECK(Data[0], 0);
        
        if( SessionPresent != (!MQC_MsgQueue_empty( &(MQCHandler->SessionCtx.MessageQueue) )) )
        {
            /* Need reset  */
            Result = E_MQC_BEHAVIOR_NEEDRESET;
        }
        
        /* Change status */
        if( E_MQC_STATUS_RESET == MQCHandler->SessionCtx.Status )
        {
            MQCHandler->SessionCtx.Status = E_MQC_STATUS_OPEN;
            /* Timer Cancel */
            MQCHandler->SessionCtx.TimeoutCount = 0;
        }
        else
        {
            if( (D_MQC_OPEN_SUCCESS_CODE == Data[1]) && (E_MQC_BEHAVIOR_COMPLETE == Result) )
            {
                /* Connect success */
                MQCHandler->SessionCtx.Status = E_MQC_STATUS_WORK;
                /* Keep Alive Timer Start */
                MQCHandler->SessionCtx.TimeoutCount = MQCHandler->KeepAliveInterval * 1000;
            }
            else
            {
                /* Connect fail */
                MQCHandler->SessionCtx.Status = E_MQC_STATUS_OPEN;
                /* Timer Cancel */
                MQCHandler->SessionCtx.TimeoutCount = 0;
            }
        }
        
        /* Notify user the connect result */
        D_MQC_CALLBACK_SAFECALL(Ret, MQCHandler->OpenResetFuncCB, MQCHandler->UsrCtx, Result, Data[1]);
        
        Ret = D_MQC_RET_OK;

    }while(0);
    
    return Ret;
}

/** 
 * @brief               Check and Process PUBLISH Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           FixedHeader             Fixed Header
 * @param[in]           Data                    Message variable header and payload
 * @param[in]           DataSize                Message remaining length (variable header size + payload size)
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_BAD_FORMAT
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_processPublish(S_MQC_SESSION_HANDLE* MQCHandler, uint8_t FixedHeader, uint8_t* Data, uint32_t DataSize)
{
    int32_t             Ret                 =   D_MQC_RET_OK;
    uint8_t             QoS                 =   0;
    uint16_t            PacketIdentifier    =   0;
    S_MQC_MESSAGE_INFO  Message;
    
    QoS = (FixedHeader & 0x06) >> 1;
    switch( QoS )
    {
        case E_MQC_QOS_0:
        case E_MQC_QOS_1:
        case E_MQC_QOS_2:
            /* Get Topic */
            if( DataSize < sizeof(uint16_t) )
            {
                /* Bad Format */
                Ret = D_MQC_RET_BAD_FORMAT;
                break;
            }
            Message.Topic.Length = MQC_ntohs(*((uint16_t*)Data));
            if( !Message.Topic.Length )
            {
                /* Bad Format */
                Ret = D_MQC_RET_BAD_FORMAT;
                break;
            }
            DataSize = DataSize - sizeof(uint16_t);
            if( Message.Topic.Length > DataSize )
            {
                /* Bad Format */
                Ret = D_MQC_RET_BAD_FORMAT;
                break;
            }
            Data = Data + sizeof(uint16_t);
            Message.Topic.Data = Data;
            DataSize = DataSize - Message.Topic.Length;
            Data = Data + Message.Topic.Length;
            /* Get Packet Identifier */
            if(E_MQC_QOS_0 != QoS)
            {
                if(sizeof(uint16_t) > DataSize)
                {
                    /* Bad Format */
                    Ret = D_MQC_RET_BAD_FORMAT;
                    break;
                }
                PacketIdentifier = MQC_ntohs(*((uint16_t*)Data));
                DataSize = DataSize - sizeof(uint16_t);
                Data = Data + sizeof(uint16_t);
                /* Search message in the queue */
                if(MQC_MsgQueue_search(&(MQCHandler->SessionCtx.MessageQueue), PacketIdentifier, E_MQC_MSG_PUBREC))
                {
                    /* Discard */
                    Ret = D_MQC_RET_OK;
                    break;
                }
            }
            /* Get Message */
            Message.Length = DataSize;
            Message.Content = (DataSize) ? Data : NULL;
            /* Send response to server */
            if(E_MQC_QOS_1 == QoS)
            {
                Ret = prvMQC_CorePuback(MQCHandler, PacketIdentifier);
            }
            else if(E_MQC_QOS_2 == QoS)
            {
                Ret = prvMQC_CorePubrec(MQCHandler, PacketIdentifier);
            }
            else
            {
                /* Do nothing */
                Ret = D_MQC_RET_OK;
            }
            if(D_MQC_RET_OK != Ret)
            {
                break;
            }
            /* Notify User message received */
            D_MQC_CALLBACK_SAFECALL(Ret, MQCHandler->ReadFuncCB, MQCHandler->UsrCtx, E_MQC_MSG_PUBLISH, &Message);
            Ret = D_MQC_RET_OK;
            break;
        default:
            Ret = D_MQC_RET_BAD_FORMAT;
            break;
    }
    
    return Ret;
}

/** 
 * @brief               Check and Process PUBACK Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           FixedHeader             Fixed Header
 * @param[in]           Data                    Message variable header and payload
 * @param[in]           DataSize                Message remaining length (variable header size + payload size)
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_BAD_FORMAT
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_processPuback(S_MQC_SESSION_HANDLE* MQCHandler, uint8_t FixedHeader, uint8_t* Data, uint32_t DataSize)
{
    int32_t         Ret                 =   D_MQC_RET_OK;
    uint16_t        PacketIdentifier    =   0;
    S_MQC_MSG_CTX*  Message             =   NULL;
    
    do
    {
        if( 0x00 != (FixedHeader & 0x0f) )
        {
            /* Bad Format */
            Ret = D_MQC_RET_BAD_FORMAT;
            break;
        }
        
        if(sizeof(uint16_t) != DataSize)
        {
            /* Bad Format */
            Ret = D_MQC_RET_BAD_FORMAT;
            break;
        }
        
        /* Get Packet Identifier */
        PacketIdentifier = MQC_ntohs(*((uint16_t*)Data));
        
        /* Search message in the queue */
        Message = MQC_MsgQueue_search(&(MQCHandler->SessionCtx.MessageQueue), PacketIdentifier, E_MQC_MSG_PUBLISH);
        if(Message)
        {
            /* Judge if the message pairs by fixed header */
            if( E_MQC_QOS_1 != ( (Message->MsgData[0] & 0x06) >> 1) )
            {
                /* Discard */
                Ret = D_MQC_RET_OK;
                break;
            }
            
            /* delete this message from the queue */
            MQC_MsgQueue_slice(&(MQCHandler->SessionCtx.MessageQueue), Message);
            
            /* Notify user the publish complete */
            D_MQC_CALLBACK_SAFECALL(Ret, Message->ExtData.Publish.ResultFuncCB, E_MQC_BEHAVIOR_COMPLETE, &(Message->ExtData.Publish.Message));
            
            /* Free the memory */
            MQCHandler->FreeFunc(Message->MsgData);
            MQCHandler->FreeFunc(Message);
        }
        
        Ret = D_MQC_RET_OK;
        
    }while(0);
    
    return Ret;
}

/** 
 * @brief               Check and Process PUBREC Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           FixedHeader             Fixed Header
 * @param[in]           Data                    Message variable header and payload
 * @param[in]           DataSize                Message remaining length (variable header size + payload size)
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_BAD_FORMAT
 * @retval              D_MQC_RET_NO_MEMORY
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_processPubrec(S_MQC_SESSION_HANDLE* MQCHandler, uint8_t FixedHeader, uint8_t* Data, uint32_t DataSize)
{
    int32_t         Ret                 =   D_MQC_RET_OK;
    uint16_t        PacketIdentifier    =   0;
    S_MQC_MSG_CTX*  Message             =   NULL;
    
    do
    {
        if( 0x00 != (FixedHeader & 0x0f) )
        {
            /* Bad Format */
            Ret = D_MQC_RET_BAD_FORMAT;
            break;
        }
        
        if(sizeof(uint16_t) != DataSize)
        {
            /* Bad Format */
            Ret = D_MQC_RET_BAD_FORMAT;
            break;
        }
        
        /* Get Packet Identifier */
        PacketIdentifier = MQC_ntohs(*((uint16_t*)Data));
        
        /* Search message in the queue */
        Message = MQC_MsgQueue_search(&(MQCHandler->SessionCtx.MessageQueue), PacketIdentifier, E_MQC_MSG_PUBLISH);
        if(Message)
        {
            /* Judge if the message pairs by fixed header */
            if( E_MQC_QOS_2 != ( (Message->MsgData[0] & 0x06) >> 1) )
            {
                /* Discard */
                Ret = D_MQC_RET_OK;
                break;
            }
            
            /* delete this message from the queue */
            MQC_MsgQueue_slice(&(MQCHandler->SessionCtx.MessageQueue), Message);
            
            /* Notify user the publish complete */
            D_MQC_CALLBACK_SAFECALL(Ret, Message->ExtData.Publish.ResultFuncCB, E_MQC_BEHAVIOR_COMPLETE, &(Message->ExtData.Publish.Message));
            
            /* Free the memory */
            MQCHandler->FreeFunc(Message->MsgData);
            MQCHandler->FreeFunc(Message);
            
            /* Send PUBREL Message */
            Ret = prvMQC_CorePubrel(MQCHandler, PacketIdentifier);
        }
        
    }while(0);
    
    return Ret;
}

/** 
 * @brief               Check and Process PUBREL Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           FixedHeader             Fixed Header
 * @param[in]           Data                    Message variable header and payload
 * @param[in]           DataSize                Message remaining length (variable header size + payload size)
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_BAD_FORMAT
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_processPubrel(S_MQC_SESSION_HANDLE* MQCHandler, uint8_t FixedHeader, uint8_t* Data, uint32_t DataSize)
{
    int32_t         Ret                 =   D_MQC_RET_OK;
    uint16_t        PacketIdentifier    =   0;
    S_MQC_MSG_CTX*  Message             =   NULL;
    
    do
    {
        if( 0x02 != (FixedHeader & 0x0f) )
        {
            /* Bad Format */
            Ret = D_MQC_RET_BAD_FORMAT;
            break;
        }
        
        if(sizeof(uint16_t) != DataSize)
        {
            /* Bad Format */
            Ret = D_MQC_RET_BAD_FORMAT;
            break;
        }
        
        /* Get Packet Identifier */
        PacketIdentifier = MQC_ntohs(*((uint16_t*)Data));
        
        /* Search message in the queue */
        Message = MQC_MsgQueue_search(&(MQCHandler->SessionCtx.MessageQueue), PacketIdentifier, E_MQC_MSG_PUBREC);
        if(Message)
        {
            /* delete this message from the queue */
            MQC_MsgQueue_slice(&(MQCHandler->SessionCtx.MessageQueue), Message);
            
            /* Free the memory */
            MQCHandler->FreeFunc(Message->MsgData);
            MQCHandler->FreeFunc(Message);
        }
        
        /* Send PUBCOMP Message */
        Ret = prvMQC_CorePubcomp(MQCHandler, PacketIdentifier);
        
    }while(0);
    
    return Ret;
}

/** 
 * @brief               Check and Process PUBCOMP Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           FixedHeader             Fixed Header
 * @param[in]           Data                    Message variable header and payload
 * @param[in]           DataSize                Message remaining length (variable header size + payload size)
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_BAD_FORMAT
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_processPubcomp(S_MQC_SESSION_HANDLE* MQCHandler, uint8_t FixedHeader, uint8_t* Data, uint32_t DataSize)
{
    int32_t         Ret                 =   D_MQC_RET_OK;
    uint16_t        PacketIdentifier    =   0;
    S_MQC_MSG_CTX*  Message             =   NULL;
    
    do
    {
        if( 0x00 != (FixedHeader & 0x0f) )
        {
            /* Bad Format */
            Ret = D_MQC_RET_BAD_FORMAT;
            break;
        }
        
        if(sizeof(uint16_t) != DataSize)
        {
            /* Bad Format */
            Ret = D_MQC_RET_BAD_FORMAT;
            break;
        }
        
        /* Get Packet Identifier */
        PacketIdentifier = MQC_ntohs(*((uint16_t*)Data));
        
        /* Search message in the queue */
        Message = MQC_MsgQueue_search(&(MQCHandler->SessionCtx.MessageQueue), PacketIdentifier, E_MQC_MSG_PUBREL);
        if(Message)
        {          
            /* delete this message from the queue */
            MQC_MsgQueue_slice(&(MQCHandler->SessionCtx.MessageQueue), Message);
            
            /* Free the memory */
            MQCHandler->FreeFunc(Message->MsgData);
            MQCHandler->FreeFunc(Message);
        }
        
        Ret = D_MQC_RET_OK;
        
    }while(0);
    
    return Ret;
}

/** 
 * @brief               Check and Process SUBACK Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           FixedHeader             Fixed Header
 * @param[in]           Data                    Message variable header and payload
 * @param[in]           DataSize                Message remaining length (variable header size + payload size)
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_BAD_FORMAT
 * @retval              D_MQC_RET_NO_MEMORY
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_processSuback(S_MQC_SESSION_HANDLE* MQCHandler, uint8_t FixedHeader, uint8_t* Data, uint32_t DataSize)
{
    int32_t             Ret                 =   D_MQC_RET_OK;
    uint16_t            PacketIdentifier    =   0;
    uint32_t            i                   =   0;
    S_MQC_MSG_CTX*      Message             =   NULL;
    E_MQC_RETURN_CODE*  CodeList            =   NULL;
    
    do
    {
        if( 0x00 != (FixedHeader & 0x0f) )
        {
            /* Bad Format */
            Ret = D_MQC_RET_BAD_FORMAT;
            break;
        }
        
        if( sizeof(uint16_t) > DataSize )
        {
            /* Bad Format */
            Ret = D_MQC_RET_BAD_FORMAT;
            break;
        }
        
        /* Get Packet Identifier */
        PacketIdentifier = MQC_ntohs(*((uint16_t*)Data));
        Data = Data + sizeof(uint16_t);
        DataSize = DataSize - sizeof(uint16_t);
        
        if(!DataSize)
        {
            /* Bad Format */
            Ret = D_MQC_RET_BAD_FORMAT;
            break;
        }
        
        CodeList = MQCHandler->MallocFunc( sizeof(E_MQC_RETURN_CODE) * DataSize);
        if(!CodeList)
        {
            Ret = D_MQC_RET_NO_MEMORY;
            break;
        }
        
        for(i = 0; i < DataSize; i++)
        {
            switch(*Data)
            {
                case E_MQC_CODE_QOS0:
                case E_MQC_CODE_QOS1:
                case E_MQC_CODE_QOS2:
                case E_MQC_CODE_FAIL:
                    CodeList[i] = (E_MQC_RETURN_CODE)(*Data);
                    break;
                default:
                    /* Bad Format */
                    Ret = D_MQC_RET_BAD_FORMAT;
                    break;
            }
            Data++;
        }
        
        if(D_MQC_RET_OK != Ret)
        {
            break;
        }
        
        /* Search message in the queue */
        Message = MQC_MsgQueue_search(&(MQCHandler->SessionCtx.MessageQueue), PacketIdentifier, E_MQC_MSG_SUBSCRIBE);
        if(Message)
        {
            if(DataSize != Message->ExtData.Subscribe.ListNum)
            {
                /* Discard */
                Ret = D_MQC_RET_OK;
                break;
            }
            
            /* Notify user the subscribe complete */
            D_MQC_CALLBACK_SAFECALL(Ret, Message->ExtData.Subscribe.ResultFuncCB, E_MQC_BEHAVIOR_COMPLETE, Message->ExtData.Subscribe.TopicFilterList, CodeList, DataSize);
            
            /* delete this message from the queue */
            MQC_MsgQueue_slice(&(MQCHandler->SessionCtx.MessageQueue), Message);
            
            /* Free the memory */
            MQCHandler->FreeFunc(Message->MsgData);
            MQCHandler->FreeFunc(Message);
        }
        
        Ret = D_MQC_RET_OK;
        
    }while(0);
    
    if(CodeList)
    {
        MQCHandler->FreeFunc(CodeList);
    }
    
    return Ret;
}

/** 
 * @brief               Check and Process UNSUBACK Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           FixedHeader             Fixed Header
 * @param[in]           Data                    Message variable header and payload
 * @param[in]           DataSize                Message remaining length (variable header size + payload size)
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_BAD_FORMAT
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_processUnsuback(S_MQC_SESSION_HANDLE* MQCHandler, uint8_t FixedHeader, uint8_t* Data, uint32_t DataSize)
{
    int32_t         Ret                 =   D_MQC_RET_OK;
    uint16_t        PacketIdentifier    =   0;
    S_MQC_MSG_CTX*  Message             =   NULL;
    
    do
    {
        if( 0x00 != (FixedHeader & 0x0f) )
        {
            /* Bad Format */
            Ret = D_MQC_RET_BAD_FORMAT;
            break;
        }
        
        if( sizeof(uint16_t) != DataSize )
        {
            /* Bad Format */
            Ret = D_MQC_RET_BAD_FORMAT;
            break;
        }
        
        /* Get Packet Identifier */
        PacketIdentifier = MQC_ntohs(*((uint16_t*)Data));
        
        /* Search message in the queue */
        Message = MQC_MsgQueue_search(&(MQCHandler->SessionCtx.MessageQueue), PacketIdentifier, E_MQC_MSG_UNSUBSCRIBE);
        if(Message)
        {
            /* Notify user the subscribe complete */
            D_MQC_CALLBACK_SAFECALL(Ret, Message->ExtData.UnSubscribe.ResultFuncCB, E_MQC_BEHAVIOR_COMPLETE, Message->ExtData.UnSubscribe.TopicFilterList, Message->ExtData.UnSubscribe.ListNum);
            
            /* delete this message from the queue */
            MQC_MsgQueue_slice(&(MQCHandler->SessionCtx.MessageQueue), Message);
            
            /* Free the memory */
            MQCHandler->FreeFunc(Message->MsgData);
            MQCHandler->FreeFunc(Message);
        }
        
        Ret = D_MQC_RET_OK;
        
    }while(0);
    
    return Ret;
}

/** 
 * @brief               Check and Process UNSUBACK Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           FixedHeader             Fixed Header
 * @param[in]           Data                    Message variable header and payload
 * @param[in]           DataSize                Message remaining length (variable header size + payload size)
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_BAD_FORMAT
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_processPingresp(S_MQC_SESSION_HANDLE* MQCHandler, uint8_t FixedHeader, uint8_t* Data, uint32_t DataSize)
{
    int32_t         Ret                 =   D_MQC_RET_OK;
    
    do
    {
        if( 0x00 != (FixedHeader & 0x0f) )
        {
            /* Bad Format */
            Ret = D_MQC_RET_BAD_FORMAT;
            break;
        }
        
        if( 0 != DataSize )
        {
            /* Bad Format */
            Ret = D_MQC_RET_BAD_FORMAT;
            break;
        }
        
        /* Notify User message received */
        D_MQC_CALLBACK_SAFECALL(Ret, MQCHandler->ReadFuncCB, MQCHandler->UsrCtx, E_MQC_MSG_PINGRESP, NULL);
        
        Ret = D_MQC_RET_OK;
        
    }while(0);
    
    return Ret;
}

/** 
 * @brief               Check and Process Message Data
 * @param[in,out]       MQCHandler              MQTT client handler
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_BAD_FORMAT
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_BAD_SEQUEUE
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_ReadMessageData( S_MQC_SESSION_HANDLE* MQCHandler )
{
    int32_t     Ret     =   D_MQC_RET_OK;
    uint32_t    Length  =   0;
    uint8_t     Type    =   0;
    uint32_t    i       =   0;
    
    Length  =   sizeof(ProtocolData) / sizeof(S_MQC_PROTOCOL_DATA);
    Type    =   MQCHandler->SessionCtx.RecvData[0] >> 4;
    Ret     =   D_MQC_RET_BAD_FORMAT;
    
    for(i = 0; i < Length; i++)
    {
        if(ProtocolData[i].Type == Type)
        {
            Ret = ProtocolData[i].processFunc(MQCHandler, MQCHandler->SessionCtx.RecvData[0], MQCHandler->SessionCtx.RecvData + MQCHandler->SessionCtx.HeaderDataSize, MQCHandler->SessionCtx.TotalRecvDataSize - MQCHandler->SessionCtx.HeaderDataSize);
            break;
        }
    }
    
    /* package free */
    prvMQC_PackageFree(MQCHandler);
    
    return Ret;
}

/** 
 * @brief               Read a MQTT Message data
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           Data                    Data read from the network
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_BAD_FORMAT
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_BAD_SEQUEUE
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static int32_t prvMQC_CoreReadMessage(S_MQC_SESSION_HANDLE* MQCHandler, uint8_t Data)
{
    int32_t Ret =   D_MQC_RET_OK;
    
    do
    {
        /* Read MQTT Message */
        if(!MQCHandler->SessionCtx.RecvData)
        {
            /* If have not received any data, create the buffer to store the received data */
            MQCHandler->SessionCtx.RecvData = MQCHandler->MallocFunc(D_MQC_MAX_MESSAGE_HEADER_SIZE);
            if(!MQCHandler->SessionCtx.RecvData)
            {
                /* Have no enouh memory */
                Ret = D_MQC_RET_NO_MEMORY;
                break;
            }
        }
        /* Append the new Data */
        MQCHandler->SessionCtx.RecvData[MQCHandler->SessionCtx.RecvDataSize] = Data;
        MQCHandler->SessionCtx.RecvDataSize++;
        /* Read data */
        if(0 == MQCHandler->SessionCtx.TotalRecvDataSize)
        {
            /* Read the Remaining Length */
            Ret = prvMQC_ReadMessageRemainingLength(MQCHandler);
            if(D_MQC_RET_OK != Ret)
            {
                break;
            }
        }
        if(MQCHandler->SessionCtx.TotalRecvDataSize == MQCHandler->SessionCtx.RecvDataSize)
        {
            /* Read the total message data */
            Ret = prvMQC_ReadMessageData(MQCHandler);
            break;
        }
    }while(0);
    
    return Ret;
}

/** 
 * @brief               Callback function for Send a MQTT Message when process the Message Queue
 * @param[in,out]       Message                 Message Information
 * @param[in]           UserCtx                 MQTT client handler
 * @return              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static void prvMQC_WriteCBForProcess(S_MQC_MSG_CTX*  Message, void* UserCtx)
{
    S_MQC_SESSION_HANDLE*   MQCHandler  =   (S_MQC_SESSION_HANDLE*)UserCtx;
    
    /* Use callback function to send data */
    (void)MQCHandler->WriteFuncCB(MQCHandler->UsrCtx, Message->MsgData, Message->MsgLength);
    return;
}

/** 
 * @brief               Callback function for Discard a MQTT Message when process the Message Queue
 * @param[in,out]       Message                 Message Information
 * @param[in]           UserCtx                 MQTT client handler
 * @return              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static void prvMQC_TimeoutCBForProcess(S_MQC_MSG_CTX*  Message, void* UserCtx)
{
    S_MQC_SESSION_HANDLE*   MQCHandler  =   (S_MQC_SESSION_HANDLE*)UserCtx;
    
    /* Notify the application this message timeout via callback function */
    (void)prvMessageDiscardNotify(MQCHandler, Message, E_MQC_BEHAVIOR_TIMEOUT);
    MQCHandler->FreeFunc(Message->MsgData);
    MQCHandler->FreeFunc(Message);
    
    return;
}

/** 
 * @brief               Callback function for resume a MQTT Message when session reconnect
 * @param[in,out]       Message                 Message Information
 * @param[in]           UserCtx                 MQTT client handler
 * @return              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
static void prvMQC_ForeachCBForReset(S_MQC_MSG_CTX*  Message, void* UserCtx)
{
    S_MQC_SESSION_HANDLE*   MQCHandler  =   (S_MQC_SESSION_HANDLE*)UserCtx;
    
    Message->SendCount  =   MQCHandler->MessageRetryCount + 1;
    Message->ExpireTime =   0;
    
    return;
}

/**************************************************************
**  Interface
**************************************************************/

/** 
 * @brief               Start a MQTT Session
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           SystimeCount            System timer count with milisecond
 * @retval              D_MQC_RET_OK
 * @author              zhaozhenge@outlook.com
 * @date                2018/06/15
 * @callgraph
 * @callergraph
 */
extern int32_t MQC_CoreStart(S_MQC_SESSION_HANDLE* MQCHandler, uint32_t SystimeCount)
{
    int32_t Ret = D_MQC_RET_OK;
    
    if(MQCHandler->LockFunc)
    {
        MQCHandler->LockFunc(MQCHandler->UsrCtx);
    }
  
    do
    {    
        memset(&(MQCHandler->SessionCtx), 0, sizeof(S_MQC_SESSION_CTX));
        
        /* Init Message Queue */
        Ret = MQC_MsgQueue_create(&(MQCHandler->SessionCtx.MessageQueue), SystimeCount);
        if(D_MQC_RET_OK != Ret)
        {
            break;
        }
        /* Set Recv Data to None */
        prvMQC_PackageFree(MQCHandler);
        /* Cancel the timer */
        MQCHandler->SessionCtx.TimeoutCount = 0;
        MQCHandler->SessionCtx.SystimeCount = SystimeCount;
        
        /* Session Created */
        MQCHandler->SessionCtx.Status = E_MQC_STATUS_OPEN;
        
    }while(0);    
    
    if(MQCHandler->UnlockFunc)
    {
        MQCHandler->UnlockFunc(MQCHandler->UsrCtx);
    }
    
    return D_MQC_RET_OK;
}

/** 
 * @brief               Stop the MQTT Session
 * @param[in,out]       MQCHandler              MQTT client handler
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/06/15
 * @callgraph
 * @callergraph
 */
extern int32_t MQC_CoreStop(S_MQC_SESSION_HANDLE* MQCHandler)
{
    int32_t Ret = D_MQC_RET_OK;
    
    if(MQCHandler->LockFunc)
    {
        MQCHandler->LockFunc(MQCHandler->UsrCtx);
    }
    
    /* Status check */
    switch (MQCHandler->SessionCtx.Status)
    {
        case E_MQC_STATUS_OPEN:
            prvMQC_CoreCleanSession(MQCHandler);
            break;
        case E_MQC_STATUS_CONNECT:
        case E_MQC_STATUS_WORK:
        case E_MQC_STATUS_RESET:
            prvMQC_CoreCleanSession(MQCHandler);
            Ret = prvMQC_CoreDisconnect(MQCHandler);
            break;
        default:
            Ret =   D_MQC_RET_UNEXPECTED_ERROR;
            return Ret;
    }
    
    /* Set Recv Data to None */
    prvMQC_PackageFree(MQCHandler);
    /* Cancel the timer */
    MQCHandler->SessionCtx.TimeoutCount = 0;
    MQCHandler->SessionCtx.SystimeCount = 0;
    /* Finalize Message Queue */
    MQC_MsgQueue_delete(&(MQCHandler->SessionCtx.MessageQueue));
    MQCHandler->SessionCtx.Status = E_MQC_STATUS_INVALID;
    
    memset(&(MQCHandler->SessionCtx), 0, sizeof(S_MQC_SESSION_CTX));

    if(MQCHandler->UnlockFunc)
    {
        MQCHandler->UnlockFunc(MQCHandler->UsrCtx);
    }
    
    return Ret;
}

/** 
 * @brief               Open a MQTT connection with CONNECT Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           Timeout                 Timeout with millisecond for cannot receive server response
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_NO_NOTIFY
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_BAD_SEQUEUE
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/06/16
 * @callgraph
 * @callergraph
 */
extern int32_t MQC_CoreOpen(S_MQC_SESSION_HANDLE* MQCHandler, uint32_t Timeout)
{
    int32_t Ret = D_MQC_RET_OK;

    if(MQCHandler->LockFunc)
    {
        MQCHandler->LockFunc(MQCHandler->UsrCtx);
    }
    
    /* Status check */
    switch (MQCHandler->SessionCtx.Status)
    {
        /* Status check */
        case E_MQC_STATUS_OPEN:
            /* Connect with server */
            Ret = prvMQC_CoreConnect(MQCHandler, MQCHandler->CleanSession);
            if(D_MQC_RET_OK == Ret)
            {
                MQCHandler->SessionCtx.Status = E_MQC_STATUS_CONNECT;
                MQCHandler->SessionCtx.TimeoutCount = Timeout;
            }
            break;
        case E_MQC_STATUS_CONNECT:
            /* Has been connecting, wait the result callback */
            Ret = D_MQC_RET_OK;
            break;
        case E_MQC_STATUS_WORK:
            /* Has been connected */
            Ret = D_MQC_RET_NO_NOTIFY;
            break;
        case E_MQC_STATUS_RESET:
            Ret = D_MQC_RET_BAD_SEQUEUE;
            break;
        default:
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
    }

    if(MQCHandler->UnlockFunc)
    {
        MQCHandler->UnlockFunc(MQCHandler->UsrCtx);
    }
    
    return Ret;
}

/** 
 * @brief               Close a MQTT connection with DISCONNECT Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/04
 * @callgraph
 * @callergraph
 */
extern int32_t MQC_CoreClose(S_MQC_SESSION_HANDLE* MQCHandler)
{
    int32_t Ret = D_MQC_RET_OK;

    if(MQCHandler->LockFunc)
    {
        MQCHandler->LockFunc(MQCHandler->UsrCtx);
    }
    
    /* Status check */
    switch (MQCHandler->SessionCtx.Status)
    {
        /* Status check */
        case E_MQC_STATUS_OPEN:
            Ret = D_MQC_RET_OK;
            break;
        case E_MQC_STATUS_CONNECT:
        case E_MQC_STATUS_WORK:
        case E_MQC_STATUS_RESET:
            if(MQCHandler->CleanSession)
            {
                /* Clean Session */
                prvMQC_CoreCleanSession(MQCHandler);
            }
            else
            {
                /* Suspend Session */
                MQC_MsgQueue_foreach(&(MQCHandler->SessionCtx.MessageQueue), prvMQC_ForeachCBForReset, MQCHandler);
            }
            Ret = prvMQC_CoreDisconnect(MQCHandler);
            /* Set Recv Data to None */
            prvMQC_PackageFree(MQCHandler);
            /* Cancel the timer */
            MQCHandler->SessionCtx.TimeoutCount = 0;
            MQCHandler->SessionCtx.Status = E_MQC_STATUS_OPEN;
            break;
        default:
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
    }

    if(MQCHandler->UnlockFunc)
    {
        MQCHandler->UnlockFunc(MQCHandler->UsrCtx);
    }
    
    return Ret;
}

/** 
 * @brief               Reset a MQTT connection with CONNECT Message (CleanSession Flag On)
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           Timeout                 Timeout with millisecond for cannot receive server response
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_BAD_SEQUEUE
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/04
 * @callgraph
 * @callergraph
 */
extern int32_t MQC_CoreReset(S_MQC_SESSION_HANDLE* MQCHandler, uint32_t Timeout)
{
    int32_t Ret = D_MQC_RET_OK;

    if(MQCHandler->LockFunc)
    {
        MQCHandler->LockFunc(MQCHandler->UsrCtx);
    }
    
    /* Status check */
    switch (MQCHandler->SessionCtx.Status)
    {
        /* Status check */
        case E_MQC_STATUS_OPEN:
            prvMQC_CoreCleanSession(MQCHandler);
            Ret = prvMQC_CoreConnect(MQCHandler, true);
            if(D_MQC_RET_OK == Ret)
            {
                MQCHandler->SessionCtx.Status = E_MQC_STATUS_RESET;
                MQCHandler->SessionCtx.TimeoutCount = Timeout;
            }
            break;
        case E_MQC_STATUS_CONNECT:
            Ret = D_MQC_RET_BAD_SEQUEUE;
            break;
        case E_MQC_STATUS_WORK:
            Ret = D_MQC_RET_BAD_SEQUEUE;
            break;
        case E_MQC_STATUS_RESET:
            Ret = D_MQC_RET_OK;
            break;
        default:
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
    }

    if(MQCHandler->UnlockFunc)
    {
        MQCHandler->UnlockFunc(MQCHandler->UsrCtx);
    }
    
    return Ret;
}

/** 
 * @brief               Subscribe topic via MQTT Session.
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           TopicFilterList         Topic Filter List
 * @param[in]           QoSList                 Topic Filter's QoS List
 * @param[in]           ListNum                 Count of the topic filter list
 * @param[in]           ResultFuncCB            Callback function will be called to notify if received Server Response
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_BAD_SEQUEUE
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_NO_MEMORY
 * @author              zhaozhenge@outlook.com
 * @date                2018/06/19
 * @callgraph
 * @callergraph
 */
extern int32_t MQC_CoreSubscribe(S_MQC_SESSION_HANDLE* MQCHandler, S_MQC_UTF8_DATA* TopicFilterList, E_MQC_QOS_LEVEL* QoSList, uint32_t ListNum, F_SUBSCRIBE_RES_CBFUNC ResultFuncCB)
{
    int32_t Ret = D_MQC_RET_OK;

    if(MQCHandler->LockFunc)
    {
        MQCHandler->LockFunc(MQCHandler->UsrCtx);
    }
    
    /* Status check */
    switch (MQCHandler->SessionCtx.Status)
    {
        /* Status check */
        case E_MQC_STATUS_OPEN:
            Ret = D_MQC_RET_BAD_SEQUEUE;
            break;
        case E_MQC_STATUS_CONNECT:
        case E_MQC_STATUS_WORK:  
        case E_MQC_STATUS_RESET:
            /* MQTT V3.1.1 allow client to send message before receive CONNACK */
            Ret = prvMQC_CoreSubscribe(MQCHandler, TopicFilterList, QoSList, ListNum, ResultFuncCB);
            break;
        default:
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
    }

    if(MQCHandler->UnlockFunc)
    {
        MQCHandler->UnlockFunc(MQCHandler->UsrCtx);
    }
    
    return Ret;
}

/** 
 * @brief               Cancel a subscribed topic via MQTT Session.
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           TopicFilterList         Topic Filter List
 * @param[in]           ListNum                 Count of the topic filter list
 * @param[in]           ResultFuncCB             Callback function will be called to notify if received Server Response
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_BAD_SEQUEUE
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_NO_MEMORY
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/04
 * @callgraph
 * @callergraph
 */
extern int32_t MQC_CoreUnsubscribe(S_MQC_SESSION_HANDLE* MQCHandler, S_MQC_UTF8_DATA* TopicFilterList, uint32_t ListNum, F_UNSUBSCRIBE_RES_CBFUNC ResultFuncCB)
{
    int32_t Ret = D_MQC_RET_OK;

    if(MQCHandler->LockFunc)
    {
        MQCHandler->LockFunc(MQCHandler->UsrCtx);
    }
    
    /* Status check */
    switch (MQCHandler->SessionCtx.Status)
    {
        /* Status check */
        case E_MQC_STATUS_OPEN:
            Ret = D_MQC_RET_BAD_SEQUEUE;
            break;
        case E_MQC_STATUS_CONNECT:
        case E_MQC_STATUS_WORK:  
        case E_MQC_STATUS_RESET:
            /* MQTT V3.1.1 allow client to send message before receive CONNACK */
            Ret = prvMQC_CoreUnSubscribe(MQCHandler, TopicFilterList, ListNum, ResultFuncCB);
            break;
        default:
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
    }

    if(MQCHandler->UnlockFunc)
    {
        MQCHandler->UnlockFunc(MQCHandler->UsrCtx);
    }
    
    return Ret;
}

/** 
 * @brief               Publish message via MQTT Session.
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           Message                 MQTT Message
 * @param[in]           QoS                     Message's QoS
 * @param[in]           Retain                  Message's Retain Flag
 * @param[in]           ResultFuncCB            Callback function will be called to notify if received Server Response
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/06/19
 * @callgraph
 * @callergraph
 */
extern int32_t MQC_CorePublish(S_MQC_SESSION_HANDLE* MQCHandler, S_MQC_MESSAGE_INFO* Message, E_MQC_QOS_LEVEL QoS, bool Retain, F_PUBLISH_RES_CBFUNC ResultFuncCB)
{
    int32_t Ret = D_MQC_RET_OK;

    if(MQCHandler->LockFunc)
    {
        MQCHandler->LockFunc(MQCHandler->UsrCtx);
    }
    
    /* Status check */
    switch (MQCHandler->SessionCtx.Status)
    {
        /* Status check */
        case E_MQC_STATUS_OPEN:
            Ret = D_MQC_RET_BAD_SEQUEUE;
            break;
        case E_MQC_STATUS_CONNECT:
        case E_MQC_STATUS_WORK:  
        case E_MQC_STATUS_RESET:
            /* MQTT V3.1.1 allow client to send message before receive CONNACK */
            Ret = prvMQC_CorePublish(MQCHandler, Message, QoS, Retain, ResultFuncCB);
            break;
        default:
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
    }

    if(MQCHandler->UnlockFunc)
    {
        MQCHandler->UnlockFunc(MQCHandler->UsrCtx);
    }
    
    return Ret;
}

/** 
 * @brief               Send a PINFREQ Message via MQTT Session.
 * @param[in,out]       MQCHandler              MQTT client handler
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_BAD_SEQUEUE
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/04
 * @callgraph
 * @callergraph
 */
extern int32_t MQC_CorePing(S_MQC_SESSION_HANDLE* MQCHandler)
{
    int32_t Ret = D_MQC_RET_OK;

    if(MQCHandler->LockFunc)
    {
        MQCHandler->LockFunc(MQCHandler->UsrCtx);
    }
    
    /* Status check */
    switch (MQCHandler->SessionCtx.Status)
    {
        /* Status check */
        case E_MQC_STATUS_OPEN:
            Ret = D_MQC_RET_BAD_SEQUEUE;
            break;
        case E_MQC_STATUS_CONNECT:
        case E_MQC_STATUS_WORK:  
        case E_MQC_STATUS_RESET:
            /* MQTT V3.1.1 allow client to send message before receive CONNACK */
            Ret = prvMQC_CorePing(MQCHandler);
            break;
        default:
            Ret = D_MQC_RET_UNEXPECTED_ERROR;
            break;
    }

    if(MQCHandler->UnlockFunc)
    {
        MQCHandler->UnlockFunc(MQCHandler->UsrCtx);
    }
    
    return Ret;
}

/** 
 * @brief               Set the data read from network to the MQTT Sesstion.
 *                      The data will be parsed as MQTT protocol.
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           Data                    Data read from the network
 * @param[in]           Size                    Size of the Data
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_BAD_SEQUEUE
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_BAD_FORMAT
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/04
 * @callgraph
 * @callergraph
 */
extern int32_t MQC_CoreRead(S_MQC_SESSION_HANDLE* MQCHandler, uint8_t* Data, size_t Size)
{
    uint8_t     CurData     =   0;
    int32_t     Ret         =   D_MQC_RET_OK;
    
    if(MQCHandler->LockFunc)
    {
        MQCHandler->LockFunc(MQCHandler->UsrCtx);
    }
    
    while(Size-- > 0)
    {
        CurData = *Data++;
        /* Status check */
        switch (MQCHandler->SessionCtx.Status)
        {
            /* Status check */
            case E_MQC_STATUS_OPEN:
                Ret = D_MQC_RET_BAD_SEQUEUE;
                break;
            case E_MQC_STATUS_CONNECT:
            case E_MQC_STATUS_WORK:
            case E_MQC_STATUS_RESET:
                /* Read MQTT Message */
                Ret = prvMQC_CoreReadMessage(MQCHandler, CurData);
                break;
            default:
                Ret = D_MQC_RET_UNEXPECTED_ERROR;
                break;
        }
        if(D_MQC_RET_OK != Ret)
        {
            break;
        }
    }
    
    if(MQCHandler->UnlockFunc)
    {
        MQCHandler->UnlockFunc(MQCHandler->UsrCtx);
    }
    
    return Ret;
}

/** 
 * @brief               This function will be called periodically to keep the MQTT session alive
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           SystimeCount            System timer count with millisecond
 * @return              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/04
 * @callgraph
 * @callergraph
 */
extern void MQC_CoreContinue(S_MQC_SESSION_HANDLE* MQCHandler, uint32_t SystimeCount)
{
    uint32_t    PassedTime  = 0;
    int32_t     Ret         = 0;
    
    if(MQCHandler->LockFunc)
    {
        MQCHandler->LockFunc(MQCHandler->UsrCtx);
    }
    
    /* Status check */
    switch (MQCHandler->SessionCtx.Status)
    {
        case E_MQC_STATUS_OPEN:
            MQCHandler->SessionCtx.SystimeCount = SystimeCount;
            break;
        case E_MQC_STATUS_CONNECT:
        case E_MQC_STATUS_RESET:
            PassedTime = prvMQC_CheckPassTime(MQCHandler->SessionCtx.SystimeCount, SystimeCount);
            if( PassedTime >= MQCHandler->SessionCtx.TimeoutCount )
            {
                /* Timeout */
                MQCHandler->SessionCtx.Status = E_MQC_STATUS_OPEN;
                /* Set Recv Data to None */
                prvMQC_PackageFree(MQCHandler);
                /* Cancel the timer */
                MQCHandler->SessionCtx.TimeoutCount = 0;
                if(MQCHandler->CleanSession)
                {
                    /* Clean Session */
                    prvMQC_CoreCleanSession(MQCHandler);
                }
                /* Call the callback function */
                D_MQC_CALLBACK_SAFECALL(Ret, MQCHandler->OpenResetFuncCB, MQCHandler->UsrCtx, E_MQC_BEHAVIOR_TIMEOUT, 0);
            }
            else
            {
                MQCHandler->SessionCtx.TimeoutCount = MQCHandler->SessionCtx.TimeoutCount - PassedTime;
            }
            MQCHandler->SessionCtx.SystimeCount = SystimeCount;
            MQC_MsgQueue_process(&(MQCHandler->SessionCtx.MessageQueue), SystimeCount, prvMQC_WriteCBForProcess, prvMQC_TimeoutCBForProcess, MQCHandler);
            break;
        case E_MQC_STATUS_WORK:
            if( MQCHandler->KeepAliveInterval )
            {
                PassedTime = prvMQC_CheckPassTime(MQCHandler->SessionCtx.SystimeCount, SystimeCount);
                if( PassedTime >= MQCHandler->SessionCtx.TimeoutCount )
                {
                    /* Timeout */
                    Ret = prvMQC_CorePing(MQCHandler);
                    MQCHandler->SessionCtx.TimeoutCount = MQCHandler->KeepAliveInterval * 1000;
                }
                else
                {
                    MQCHandler->SessionCtx.TimeoutCount = MQCHandler->SessionCtx.TimeoutCount - PassedTime;
                }
            }
            MQCHandler->SessionCtx.SystimeCount = SystimeCount;
            MQC_MsgQueue_process(&(MQCHandler->SessionCtx.MessageQueue), SystimeCount, prvMQC_WriteCBForProcess, prvMQC_TimeoutCBForProcess, MQCHandler);
            break;
        default:
            break;
    }
 
    if(MQCHandler->UnlockFunc)
    {
        MQCHandler->UnlockFunc(MQCHandler->UsrCtx);
    }
    
    return;
}
