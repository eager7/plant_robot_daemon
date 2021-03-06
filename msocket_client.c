/****************************************************************************
 *
 * MODULE:             msocket_client.c
 *
 * COMPONENT:          socket interface
 *
 * REVISION:           $Revision:  0$
 *
 * DATED:              $Date: 2016-01-04 17:04:13 $
 *
 * AUTHOR:             PCT
 *
 ****************************************************************************
 *
 * Copyright panchangtao@gmail.com 2016. All rights reserved
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "msocket_client.h"
#include <sys/epoll.h> 
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define DBG_SOCK_CET 1

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
static void *pvSocketClientThread(void *psThreadInfoVoid);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
extern uint8 verbosity;
/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
temSocketStatus eSocketClientInit(int iPort, char *paNetAddress)
{

    return E_SOCKET_OK;
}



