/****************************************************************************
 *
 * MODULE:             socket lib interface
 *
 * COMPONENT:          msocket_server.h
 *
 * REVISION:           $Revision: 52723 $
 *
 * DATED:              $Date: 2016-01-04 17:04:13 $
 *
 * AUTHOR:             panchangtao
 *
 ****************************************************************************
 *
 * Copyright panchangtao@gmail.com 2016. All rights reserved
 *
 ***************************************************************************/

#ifndef __MSOCKET_SERVER_H__
#define __MSOCKET_SERVER_H__

#if defined __cplusplus
extern "C"{
#endif

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "utils.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include "mthread.h"
#include "list.h"

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef enum
{
    E_SOCKET_OK,
    E_SOCKET_NULL,
    E_SOCKET_INIT,
    E_SOCKET_SEND,
    E_SOCKET_RECV,
    E_SOCKET_CLOSE,
    E_SOCKET_ERROR,
    E_SOCKET_TIMEOUT,
    E_SOCKET_DISCONNECT,
    E_SOCKET_NO_MEM,
} temSocketStatus;

typedef enum
{
    E_EPOLL_OK = 1,
    E_EPOLL_TIMEOUT = 0,
    E_EPOLL_ERROR = -1,
}teSelectResult;

typedef enum
{
    E_WAIT_OK = 0,
    E_WAIT_TIMEOUT = ETIMEDOUT,
}teCondWaitResult;

typedef struct
{
    int iSocketFd;
    pthread_t pthSocketServer;
    tsThread sThread;
    struct sockaddr_in sAddr_Ipv4;
    pthread_mutex_t mutex;
} tsSocketServer;

typedef struct _tSocektClient
{
    int                             iSocketFd;
    struct sockaddr_in              addrclient;
    int                             iSocketDataLen;
    char                            csClientData[MXBF];
}tsSocketClient;

//typedef void *(*tprThreadFunction)(void *psThreadInfoVoid);

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define SOCKET_LISTEN_NUM 5
#define EPOLL_EVENT_NUM 10
#define SOCKET_CLIENT_NUM 10
/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Locate   Functions                                            ***/
/****************************************************************************/

temSocketStatus eSocketServerInit(int iPort, char *paNetAddress);
temSocketStatus eSocketServerFinished(tsSocketServer *psmSocket);
temSocketStatus eSocketServerRecv(tsSocketServer *psmSocket, char *paRecvMsg, uint16 u16Length);
temSocketStatus eSocketServerSend(int iSocketFd, char *paSendMsg, uint16 u16Length);

#if defined __cplusplus
}
#endif

#endif /* __MTHREADS_H__ */


