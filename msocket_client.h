/****************************************************************************
 *
 * MODULE:             socket lib interface
 *
 * COMPONENT:          msocket_client.h
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

#ifndef __MSOCKET_CLIENT_H__
#define __MSOCKET_CLIENT_H__

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
#include "msocket_server.h"
#include "list.h"

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/


/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
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

temSocketStatus eSocketClientInit(int iPort, char *paNetAddress);
temSocketStatus eSocketClientFinished(tsSocketServer *psmSocket);
temSocketStatus eSocketClientRecv(tsSocketServer *psmSocket, char *paRecvMsg, uint16 u16Length);
temSocketStatus eSocketClientSend(int iSocketFd, char *paSendMsg, uint16 u16Length);

#if defined __cplusplus
}
#endif

#endif /* __MTHREADS_H__ */


