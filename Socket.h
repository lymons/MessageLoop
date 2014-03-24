//
//  Socket.h
//  MessageLoop
//
//  Created by Lymons on 14-3-20.
//  Copyright (c) 2014年 Lymons. All rights reserved.
//

#ifndef __MessageLoop__Socket__
#define __MessageLoop__Socket__

#include <iostream>
#include <vector>

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

class SocketNotifier;

class Socket
{
public:
    Socket(SocketNotifier *n);
    
    int StartServer();
    int StartClientConnect(int& server_sock);
    int WaitClient();
    int NotifyServer(int sock);
    void CloseConnect(int sock);
    
private:
    SocketNotifier *_notifier;
    fd_set _sockets;
    int _server_sock;
    int _maxsock;
    int _conn_amount;
    struct timeval _tv; //Select超时返回的时间。
    
    vector<int> _socks;
    
private:
    int AcceptClient();
    int ReadClient(int index);
};

#endif /* defined(__MessageLoop__Socket__) */
