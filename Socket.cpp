//
//  Socket.cpp
//  MessageLoop
//
//  Created by Lymons on 14-3-20.
//  Copyright (c) 2014年 Lymons. All rights reserved.
//

#include "Socket.h"
#include "SocketNotifier.h"

#include <stdio.h>

#define MYPORT 1234     // 侦听端口
#define BACKLOG 5       // 最大可连接客户端数量
#define BUF_SIZE 1024

Socket::Socket(SocketNotifier *n)
{
    _maxsock = 64;
    _socks.resize(_maxsock);
    _notifier = n;
    // 超时设置为1天
    _tv.tv_sec = 60*60*24;
    _tv.tv_usec = 0;
    
    FD_ZERO(&_sockets);
}

int Socket::StartServer()
{
    int sock_fd;
    int yes = 1;
    struct sockaddr_in server_addr;
    
    //创建侦听Socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Create listening socket error!");
        return -1;
    }
    
    // 配置侦听Socket，SO_REUSEADDR BOOL 允许套接口和一个已在使用中的地址捆绑。
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        perror("setsockopt error!");
        return -1;
    }
    
    bzero(&server_addr, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;         // host byte order
    server_addr.sin_port = htons(MYPORT);     // short, network byte order
    server_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    
    if (::bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind error!");
        return -1;
    }
    //开始侦听，最大连接数为BACKLOG
    if (listen(sock_fd, BACKLOG) == -1)
    {
        perror("listen error!");
        return -1;
    }
    
    _server_sock = sock_fd;
    _socks[0] = _server_sock;
    _conn_amount = 1;
    
    return sock_fd;
}

int Socket::StartClientConnect(int& server_sock)
{
    struct sockaddr_in server_addr;
    
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == sock_fd)
    {
        perror("socket fail !");
        return -1;
    }
    
    bzero(&server_addr, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(MYPORT);
    
    if(-1 == connect(sock_fd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)))
    {
        perror("connect fail !");
        return -1;
    }
    
    // 取出服务端接收到的socket fd
    char buf[10];
    //接收服务端传递过来的socket fd
    size_t len = recv(sock_fd, buf, sizeof(buf), 0);
    if (len <= 0) {
        printf("not received socket from server\n");
        
    } else {
        server_sock = atoi(buf);
    }

    return sock_fd;
}

/*
 * 必须在服务器端调用
 */
int Socket::WaitClient()
{
    // 初始化文件描述符集合 initialize file descriptor set
    FD_ZERO(&_sockets);
    // 把Sock_fd加入到文件描述符集合
    FD_SET(_server_sock, &_sockets);
    _maxsock = _socks[_conn_amount-1];

    // 把活动的socket的句柄加入到文件描述符集合中
    for (int i = 0; i < _conn_amount; i++) {
        FD_SET(_socks[i], &_sockets);
    }
    
    // 设置为永久阻塞，因此会阻塞掉调用线程。
    int ret = select(_maxsock + 1, &_sockets, NULL, NULL, NULL);
    if (ret <= 0) {
        perror("select error!");
        return -1;
    }

    // 轮询各个文件描述符（socket）
    for (int i = 0; i < _conn_amount; i++) {
        if (FD_ISSET(_socks[i], &_sockets)) {
            if (_socks[i] == _server_sock) {
                AcceptClient();
            } else {
                ReadClient(i);
            }
        }
    }
    
    return 0;
}

int Socket::NotifyOpposite(int sock)
{
    char idbuf[10] = { 0 };
    sprintf(idbuf, "%d", sock);
    return (int)send(sock, idbuf, strlen(idbuf), 0);
}

void Socket::CloseConnect(int sock)
{
    close(sock);
}

int Socket::ReadClient(int index)
{
    char buf[BUF_SIZE] = {0};
    
    //接收数据
    ssize_t len = recv(_socks[index], buf, sizeof(buf), 0);
    if (len <= 0) {
        printf("client[%d] close\n", index);
        close(_socks[index]);
        FD_CLR(_socks[index], &_sockets);
        _socks[index] = 0;
    }
    else {
        // 将接收数据的最后一位补0
        if (len < BUF_SIZE)
            memset(&buf[len], '\0', 1);
        printf("client[%d] send:%s\n", index, buf);
        
        // 通知客户端已收到消息
        send(_socks[index], "received", 9, 0);
        
        // 通知Handler处理未决的Message
        _notifier->Notify(_socks[index]);
    }
    
    return 0;
}

int Socket::AcceptClient()
{
    // 检查是否有新连接进来，如果有新连接进来，接收连接，生成新socket，
    // 并加入到监控文件描述符集合中。
    
    struct sockaddr_in client_addr;
    socklen_t sin_size;
    //接受连接
    int new_fd = accept(_server_sock, (struct sockaddr *)&client_addr, &sin_size);
    if (new_fd <= 0)
    {
        perror("accept socket error!");
        return 0;
    }
    
    // 将新的连接加入到监控文件描述符集合
    if (_conn_amount < _maxsock-1) {
        _socks[_conn_amount++] = new_fd;
        printf("new connection client[%d] %s:%d\n", _conn_amount, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        
        // 把这个fd发送给客户端
        NotifyOpposite(new_fd);
    }
    else {
        printf("max connections arrive, exit\n");
        send(new_fd, "limited", 8, 0);
    }
    
    return new_fd;
}

