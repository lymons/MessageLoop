//
//  Queue.cpp
//  MessageLoop
//
//  Created by Lymons on 14-3-20.
//  Copyright (c) 2014年 Lymons. All rights reserved.
//

#include "Queue.h"
#include "Message.h"
#include "SocketNotifier.h"
#include "Mutex.h"

MMutex qMutex;

Queue::Queue()
{
    
}

Message* Queue::Next()
{
    SocketNotifier* notifier = &SocketNotifier::Instance();
    Message* message = Pop();
    if (!message) {
        notifier->StartMonitor();
        message = Pop();
    }
    
    return message;
}

void Queue::Push(Message* m)
{
    //int id = m->GetTargetIndentifier();
    MLock l(qMutex);
    mMessageQueue.push(m);
}

void Queue::Push(queue<Message*>& q)
{
    size_t len = q.size();
    for (int i=0; i < len; ++i) {
        Message* m = q.front();
        Push(m);
        q.pop();
    }
}

Message* Queue::Pop()
{
    Message* message = NULL;
    MLock l(qMutex);
    
    if (!mMessageQueue.empty()) {
        message = mMessageQueue.front();
        mMessageQueue.pop();
    }
    
    return message;
}


