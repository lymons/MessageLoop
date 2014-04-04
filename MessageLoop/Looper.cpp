//
//  Looper.cpp
//  MessageLoop
//
//  Created by Lymons on 14-3-20.
//  Copyright (c) 2014年 Lymons. All rights reserved.
//

#include "Looper.h"
#include "Queue.h"
#include "Message.h"
#include "Handler.h"
#include "SocketNotifier.h"

#include <assert.h>

#ifdef WIN32
#include <windows.h>
__declspec(thread) Looper* _looper = NULL;
#else
__thread Looper* _looper = NULL;
#endif

Looper::Looper()
{
    mQueue = new Queue();
    mRuning = false;
    mFinished = false;
}

void Looper::Prepare()
{
    // Looper只能在一个线程中创建一次
    assert(!_looper);
    
    _looper = new Looper();
    SocketNotifier::Instance().CreateServer();
}

void Looper::Run()
{
    Looper *looper = GetLooper();
    Queue *queue = looper->mQueue;
    looper->mRuning = true;
    
    while (!looper->mFinished) {
        Message *message = queue->Next();
        if (message) {
            message->Handle();
            queue->Recycle(message);
        }
    }
    
    delete looper;
    looper = NULL;
}

Looper* Looper::GetLooper()
{
    return _looper;
}

void Looper::Finished()
{
    if (_looper) {
        _looper->mFinished = true;
    }
}
