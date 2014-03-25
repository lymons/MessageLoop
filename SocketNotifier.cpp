//
//  SocketNotifier.cpp
//  MessageLoop
//
//  Created by Lymons on 14-3-20.
//  Copyright (c) 2014年 Lymons. All rights reserved.
//

#include "SocketNotifier.h"
#include "Socket.h"

__thread int _clientID = -1;
__thread int _clientIDFromServer = -1;

SocketNotifier::SocketNotifier()
{
    _socket = new Socket(this);
}

SocketNotifier::~SocketNotifier()
{
    delete _socket;
}

int SocketNotifier::CreateServer()
{
    return _socket->StartServer();
}

/*
 * 必须在子线程中使用
 */
int SocketNotifier::CreateClient(Handler* h)
{
    if (_clientID < 0) {
        _clientID =  _socket->StartClientConnect(_clientIDFromServer);
        SetHandler(h, _clientIDFromServer);
    }
    
    return _clientID;
}

void SocketNotifier::DestoryServer(int id)
{
    _socket->CloseConnect(id);
}

void SocketNotifier::DestoryClient(int id)
{
    RemoveHandler(_clientIDFromServer);
    _socket->CloseConnect(_clientID);
}

bool SocketNotifier::SendMessage(int id, void *m)
{
    return _socket->NotifyOpposite(id);
}

void* SocketNotifier::RecvMessage(int id)
{
    return NULL;
}

void SocketNotifier::StartMonitor()
{
    _socket->WaitClient();
}


