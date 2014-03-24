//
//  SocketNotifier.h
//  MessageLoop
//
//  Created by Lymons on 14-3-20.
//  Copyright (c) 2014年 Lymons. All rights reserved.
//

#ifndef __MessageLoop__SocketNotifier__
#define __MessageLoop__SocketNotifier__

#include <iostream>
#include <sys/socket.h>

#include "Notifier.h"

class Handler;
class Socket;

class SocketNotifier : public Notifier<SocketNotifier>
{
    friend class Notifier<SocketNotifier>;
protected:
    SocketNotifier();
    ~SocketNotifier();
    
public:
    int  CreateServer() ;
    int  CreateClient(Handler* h);
    void DestoryServer(int id);
    void DestoryClient(int id);
    bool SendMessage(int id, void* m);
    void* RecvMessage(int id);
    void StartMonitor();

private:
    
private:
    Socket* _socket;
    
};

#endif /* defined(__MessageLoop__SocketNotifier__) */
