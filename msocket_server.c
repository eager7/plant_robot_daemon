/****************************************************************************
 *
 * MODULE:             msocket_server.c
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
#include "msocket_server.h"
#include <sys/epoll.h> 
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define DBG_SOCK_SER 1

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
static void *pvSocketServerThread(void *psThreadInfoVoid);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
extern uint8 verbosity;
/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
static tsSocketServer sSocketServer;
static tsSocketClient sSocketClient[SOCKET_CLIENT_NUM];
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
temSocketStatus eSocketServerInit(int iPort, char *paNetAddress)
{
    DBG_vPrintf(DBG_SOCK_SER, "mSocketInit\n");
    signal(SIGPIPE, SIG_IGN);//ingnore signal interference

    memset(&sSocketServer, 0, sizeof(sSocketServer));
    memset(sSocketClient, 0, sizeof(tsSocketClient)*SOCKET_CLIENT_NUM);
    uint8 i = 0;
    for(i = 0; i < SOCKET_CLIENT_NUM; i++){
        sSocketClient[i].iSocketFd = -1;
    }
    
    sSocketServer.sAddr_Ipv4.sin_family = AF_INET;
    sSocketServer.sAddr_Ipv4.sin_port = htons(iPort);
    if(NULL == paNetAddress){
        sSocketServer.sAddr_Ipv4.sin_addr.s_addr = INADDR_ANY;
    } else {
        CheckError(inet_pton(AF_INET, paNetAddress, (void*)&sSocketServer.sAddr_Ipv4.sin_addr), 1, E_SOCKET_INIT);
    }
    
    if(-1 == (sSocketServer.iSocketFd = socket(AF_INET, SOCK_STREAM, 0)))
    {
        ERR_vPrintf(T_TRUE, "socket create error %s\n", strerror(errno));
        return E_SOCKET_INIT;
    }
    int on = 1;  /*SO_REUSEADDR port can used twice by program */
    CheckError(setsockopt(sSocketServer.iSocketFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)), 0, E_SOCKET_INIT);
    CheckError(bind(sSocketServer.iSocketFd, (struct sockaddr*)&sSocketServer.sAddr_Ipv4, sizeof(struct sockaddr_in)), 0, E_SOCKET_INIT);
    CheckError(listen(sSocketServer.iSocketFd, SOCKET_LISTEN_NUM), 0, E_SOCKET_INIT);

    DBG_vPrintf(DBG_SOCK_SER, "pthread_create\n");
    sSocketServer.sThread.pvThreadData = &sSocketServer;
    CheckError(eThreadStart(pvSocketServerThread, &sSocketServer.sThread, E_THREAD_JOINABLE), E_THREAD_OK, E_SOCKET_INIT);
    
    return E_SOCKET_OK;
}

temSocketStatus eSocketServerFinished(tsSocketServer *psmSocket)
{
    DBG_vPrintf(DBG_SOCK_SER, "mSocketFinished\n");
    CheckNull(psmSocket, E_SOCKET_CLOSE);
    
    CheckError(close(psmSocket->iSocketFd), 0, E_SOCKET_CLOSE);
    return E_SOCKET_OK;
}

temSocketStatus eSocketServerSend(int iSocketFd, char *paSendMsg, uint16 u16Length)
{
    DBG_vPrintf(DBG_SOCK_SER, "mSocketSend\n");
    CheckNull(paSendMsg, E_SOCKET_SEND);
    
    if( -1 == send(iSocketFd, paSendMsg, u16Length, 0)){
        ERR_vPrintf(T_TRUE, "socket send error %s\n", strerror(errno));
        return E_SOCKET_SEND;
    }
    return E_SOCKET_OK;
}

temSocketStatus eSocketServerRecv(tsSocketServer *psmSocket, char *paRecvMsg, uint16 u16Length)
{
    DBG_vPrintf(DBG_SOCK_SER, "mSocketSend\n");
    CheckNull(psmSocket, E_SOCKET_RECV);
    CheckNull(paRecvMsg, E_SOCKET_RECV);
    int ret = recv(psmSocket->iSocketFd, paRecvMsg, u16Length, 0);
    if( -1 == ret){
        ERR_vPrintf(T_TRUE, "socket send error %s\n", strerror(errno));
        return E_SOCKET_RECV;
    } else if(0 == ret){
        ERR_vPrintf(T_TRUE, "socket is disconnected..\n");
        return E_SOCKET_DISCONNECT;
    }
    return E_SOCKET_OK;
}
/****************************************************************************/
/***        Located  Functions                                            ***/
/****************************************************************************/
temSocketStatus eSocketServerHandle(int *pEpollFd, int iEpollResult, struct epoll_event *pEpollEventList)
{
    int n = 0; int i = 0;
    static uint8 u8NumConnClient = 0;
    struct epoll_event EpollEvevt;
    for(n = 0; n < iEpollResult; n++){
        if((pEpollEventList[n].events & EPOLLERR) || (pEpollEventList[n].events & EPOLLHUP))
        {
            ERR_vPrintf(T_TRUE,"The Fd Occured an Error, %s\n", strerror(errno));  
            continue;
        }
        else if(pEpollEventList[n].data.fd == sSocketServer.iSocketFd)    /*Server accept event*/
        {
            DBG_vPrintf(DBG_SOCK_SER, "sSocketServer.iSocketFd Changed\n");
            tsSocketClient *psSocketClientNew = NULL;
            for(i = 0; i < SOCKET_CLIENT_NUM; i ++){
                if(-1 == sSocketClient[i].iSocketFd){
                    psSocketClientNew = &sSocketClient[i];
                    break;
                }
            }
            int Len = sizeof(psSocketClientNew->addrclient);
            psSocketClientNew->iSocketFd = accept(sSocketServer.iSocketFd,
                    (struct sockaddr*)&psSocketClientNew->addrclient, (socklen_t *)&Len);
            if(-1 == psSocketClientNew->iSocketFd){
                ERR_vPrintf(T_TRUE, "socket accept error %s\n", strerror(errno));
            }else{
                INF_vPrintf(DBG_SOCK_SER, "A client[%d] Already Connected, The Number of Client is [%d]\n", psSocketClientNew->iSocketFd, i);
                EpollEvevt.data.fd = psSocketClientNew->iSocketFd;
                EpollEvevt.events = EPOLLIN | EPOLLET;  /*read ,Ede-Triggered, close*/
                if(-1 == epoll_ctl (*pEpollFd, EPOLL_CTL_ADD, psSocketClientNew->iSocketFd, &EpollEvevt)){
                    ERR_vPrintf(T_TRUE,"epoll_ctl failed, %s\n", strerror(errno));   
                    psSocketClientNew->iSocketFd = -1;
                    continue;
                }
                DBG_vPrintf(DBG_SOCK_SER, "Client Already Add Epoll_wait Fd\n");
                u8NumConnClient++;
                if(u8NumConnClient >= SOCKET_CLIENT_NUM){
                    EpollEvevt.data.fd = sSocketServer.iSocketFd;
                    EpollEvevt.events = EPOLLIN;  /*read*/
                    epoll_ctl(*pEpollFd, EPOLL_CTL_DEL, sSocketServer.iSocketFd, &EpollEvevt);
                }
            }
        }else{  /*Client recive event or disconnect event*/
            for(i = 0; i < SOCKET_CLIENT_NUM; i ++){
                if(pEpollEventList[n].data.fd == sSocketClient[i].iSocketFd){
                    /***********----------------RecvMessage-----------------************/                                
                    NOT_vPrintf(DBG_SOCK_SER, "Socket Client[%d] Begin Recv Data...\n", sSocketClient[i].iSocketFd);
                    sSocketClient[i].iSocketDataLen = recv(sSocketClient[i].iSocketFd, 
                        sSocketClient[i].csClientData, sizeof(sSocketClient[i].csClientData), 0);
                    if(-1 == sSocketClient[i].iSocketDataLen){
                        ERR_vPrintf(T_TRUE, "socket recv error %s\n", strerror(errno));
                    }else if(0 == sSocketClient[i].iSocketDataLen){   /*disconnect*/
                        ERR_vPrintf(T_TRUE, "The Client[%d] is disconnect, Closet It\n", sSocketClient[i].iSocketFd);
                        
                        EpollEvevt.data.fd = sSocketClient[i].iSocketFd;
                        EpollEvevt.events = EPOLLIN | EPOLLET | EPOLLRDHUP;  /*read ,Ede-Triggered, close*/
                        epoll_ctl(*pEpollFd, EPOLL_CTL_DEL, sSocketClient[i].iSocketFd, &EpollEvevt);
                        close(sSocketClient[i].iSocketFd);
                    
                        u8NumConnClient --;
                        if(u8NumConnClient < SOCKET_CLIENT_NUM){
                            EpollEvevt.data.fd = sSocketServer.iSocketFd;
                            EpollEvevt.events = EPOLLIN;  /*read*/
                            epoll_ctl(*pEpollFd, EPOLL_CTL_ADD, sSocketServer.iSocketFd, &EpollEvevt);
                        }
                    }else{
                        INF_vPrintf(DBG_SOCK_SER, "Recv Data is [%d]--- %s\n", sSocketClient[i].iSocketFd, sSocketClient[i].csClientData);
                        //TODO:
                        eSocketServerSend(sSocketClient[i].iSocketFd, "I Recv Msg\n", sizeof("I Recv Msg\n"));
                    }
                }
            }
            
        }
    }
    return E_SOCKET_OK;
}


static void *pvSocketServerThread(void *psThreadInfoVoid)
{
    DBG_vPrintf(verbosity, "pvSocketServerThread Starting\n");
    tsThread *psThreadInfo = (tsThread *)psThreadInfoVoid;
    tsSocketServer *psSocketServer = (tsSocketServer*)psThreadInfo->pvThreadData;
    psThreadInfo->eState = E_THREAD_RUNNING;

    int iEpollFd = epoll_create(65535);//epoll setting
    if(-1 == iEpollFd){
        ERR_vPrintf(T_TRUE,"epoll_create failed, %s\n", strerror(errno));  
        pthread_exit("epoll_create failed");
    }
    struct epoll_event EpollEvevt, EpollEventList[EPOLL_EVENT_NUM];
    EpollEvevt.data.fd = psSocketServer->iSocketFd;
    EpollEvevt.events = EPOLLIN;  /*read*/
    if(-1 == epoll_ctl (iEpollFd, EPOLL_CTL_ADD, psSocketServer->iSocketFd, &EpollEvevt)){
        ERR_vPrintf(T_TRUE,"epoll_create failed, %s\n", strerror(errno));  
        goto done;
    }

    sleep(5);//waiting other component run
    while (psThreadInfo->eState == E_THREAD_RUNNING)
    {
        int iEpollResult = epoll_wait(iEpollFd, EpollEventList, EPOLL_EVENT_NUM, -1);
        switch (iEpollResult)
        {
            case (E_EPOLL_ERROR):{
                ERR_vPrintf(T_TRUE,"epoll_wait failed, %s\n", strerror(errno));  
                goto done;
            }
            break;
            case (E_EPOLL_TIMEOUT):
                ERR_vPrintf(T_TRUE,"epoll_wait E_EPOLL_TIMEOUT\n");  
            break;
            default:{
                DBG_vPrintf(DBG_SOCK_SER, "Epoll_wait Find %d Changed\n", iEpollResult);
                //TODO:
                eSocketServerHandle(&iEpollFd, iEpollResult, EpollEventList);
            }
            break;
        }
        sleep(0);
    }
    
done:    
    close(iEpollFd);
    close(sSocketServer.iSocketFd);
    //SocketClientListFree();

    DBG_vPrintf(verbosity, "Exit\n");
    eThreadFinish(psThreadInfo);/* Return from thread clearing resources */
    return NULL;
}

