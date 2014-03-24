//
//  Handler.cpp
//  MessageLoop
//
//  Created by Lymons on 14-3-20.
//  Copyright (c) 2014年 Lymons. All rights reserved.
//

#include "Handler.h"
#include "Message.h"
#include "Looper.h"
#include "Queue.h"
#include "SocketNotifier.h"
#include "Mutex.h"

MMutex hMutex;

#include <assert.h>

Handler::Handler()
{
    // Handler必须在拥有Looper的线程中创建
    _ownner = Looper::GetLooper();
    assert(_ownner);
    
    // Hander必须在Looper.Run()之前创建完毕
    assert(!_ownner->mRuning);
//    if (!_ownner) {
//        throw runtime_error("在使用Handler之前请确保Looper.Prepare()先被调用");
//    }
    
    _sourceIndentifier = -1;
}

void Handler::CallMe()
{
    _sourceIndentifier = SocketNotifier::Instance().CreateClient(this);
}

void Handler::HangUp()
{
    SocketNotifier::Instance().DestoryClient(_sourceIndentifier);
}

bool Handler::HandleMessage(Message& m)
{  
    if ((m.mStatus & OWNNER_HANDLER) == 1) {
        MLock l(hMutex);
        _emptyMessages.push(&m);
        _occupyMessages.remove(&m);
    }
    
    if (m.IsObtainExpired()) {
        return false;
    }
    
    return true;
}

void Handler::QueuePendingMessage()
{
    MLock l(hMutex);
    
    printf("pendingQueue: %d\n", (int)_pendingMessages.size());
    _ownner->mQueue->Push(_pendingMessages);
}

Message* Handler::ObtainEmptyMessage()
{
    MLock l(hMutex);
    
    // 先把已经超时不用的Message转移到空闲队列中
    list<Message*>::iterator itor = _occupyMessages.begin();
    for (; itor != _occupyMessages.end(); ++itor) {
        Message* m = *itor;
        if (m->IsObtainExpired()) {
            _occupyMessages.remove(m);
            _emptyMessages.push(m);
        }
    }
    
    Message* message = NULL;
    if (!_emptyMessages.empty()) {
        message = _emptyMessages.front();
         _emptyMessages.pop();
    }
    
    if (!message) {
        message = new Message();
        message->mTarget = this;
        SET_OWNNER(message->mStatus, OWNNER_HANDLER);
    }
    message->mObtainTime = time(NULL);
    _occupyMessages.push_back(message);
    
    return message;
}

bool Handler::SendMessage(Message& m)
{
    // 必须在其他线程中调用该函数
    m.mTarget = this;

    {
        MLock l(hMutex);
        _pendingMessages.push(&m);
    }
    
    return SocketNotifier::Instance().SendMessage(_sourceIndentifier, NULL);
}

void Handler::SetIndentifier(int iden)
{
    _sourceIndentifier = iden;
}