//
//  Handler.h
//  MessageLoop
//
//  Created by Lymons on 14-3-20.
//  Copyright (c) 2014年 Lymons. All rights reserved.
//

#ifndef __MessageLoop__Handler__
#define __MessageLoop__Handler__

#include <iostream>
#include <queue>
#include <list>

#define OWNNER_HANDLER  0x0100
#define OWNNER_USER     0x1000
#define STATUS_EMPTY    0x0001
#define STATUS_OCCUPY   0x0010

#define SET_OWNNER(st, ow)  (((st)&0x0011)|(ow))
#define OBTAIN_TIME_LIMIT   60*60                   // one hour

using namespace std;

class Message;
class Looper;

class Handler
{
public:
    Handler();
    void CallMe();
    void HangUp();
    
    void QueuePendingMessage();
    Message* ObtainEmptyMessage();
    void SetIndentifier(int iden);
    
protected:
    virtual bool HandleMessage(Message& m);
    
private:
    friend class Message;
    bool SendMessage(Message& m);
    
    Looper* _ownner;
    int _sourceIndentifier;
    queue<Message*> _pendingMessages;
    queue<Message*> _emptyMessages;
    list<Message*> _occupyMessages;
};

#endif /* defined(__MessageLoop__Handler__) */
