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
#define PRIORITY_HIGHT  0x0001
#define PRIORITY_NORMAL 0x0010

#define SET_OWNNER(st, ow)  ((st)=((st)&0x0011)|(ow))
#define SET_PRIORI(st, pr)  ((st)=((st)&0x1100)|(pr))
#define OBTAIN_TIME_LIMIT   60*60                   // one hour

using namespace std;

class Message;
class Looper;

class ActionListener
{
public:
    typedef std::function<void(Message&)> ActionCallback;
    
public:
    ActionListener(ActionCallback cb) :_callback(cb) {
    }
    
    void fire(Message& e) {
        _callback(e);
    }
    
private:
    ActionCallback _callback;
};

class Handler
{
public:
    Handler();
    virtual ~Handler();
    void CallMe();
    void HangUp();
    
    void QueuePendingMessage();
    Message* ObtainEmptyMessage();
    
    void AddActionListener(ActionListener* al) {
        _listener = al;
    }
    
protected:
    virtual bool HandleMessage(Message& m);
    
private:
    friend class Message;
    bool SendMessage(Message& m, bool bPending=true);
    void RecycleMessage(Message* m);
    void PendingMessage(Message& m);
    
    ActionListener* _listener;
    
    Looper* _ownner;
    int _sourceIndentifier;
    queue<Message*> _pendingMessages;
    queue<Message*> _emptyMessages;
    list<Message*> _occupyMessages;
};

#endif /* defined(__MessageLoop__Handler__) */
