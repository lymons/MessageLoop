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

#define PRIORITY_LEN    3

MMutex qMutex;

Queue::Queue()
{
    PriorityManager::iterator iter = mPriorityMgr.begin();
    for (; iter != mPriorityMgr.end(); ++ iter) {
        delete iter->second;
    }
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

bool Queue::Push(Message* m)
{
    MLock l(qMutex);
    
    if ((m->mStatus & PRIORITY_HIGHT) == 1) {
        int count = 0;
        bool allow = true;
        pair<bool, int>* prior = mPriorityMgr[m->mThreadID];
        if (prior) {
            count = prior->second;
            allow = prior->first;
        }
        if (allow) {
            mPriorityMessages.push(m);
            
            if (++ count >= PRIORITY_LEN) {
                // 达到最多容纳的数目限制后就不允许放入新的消息
                allow = false;
            }
            if (prior == NULL) {
                prior = new pair<bool, int>();
                mPriorityMgr[m->mThreadID] = prior;
            }
            prior->first = allow;
            prior->second = count;
        } else {
            return false;
        }

    } else {
        mMessageQueue.push(m);
    }
    
    return true;
}

void Queue::Push(queue<Message*>& q)
{
    size_t len = q.size();
    for (int i=0; i < len; ++i) {
        Message* m = q.front();
        if (Push(m)) {
            q.pop();
        }
    }
}

Message* Queue::Pop()
{
    Message* message = NULL;
    MLock l(qMutex);
    
    if (!mPriorityMessages.empty()) {
        message = mPriorityMessages.front();
        mPriorityMessages.pop();
        
        pair<bool, int>* prior = mPriorityMgr[message->mThreadID];
        if (prior) {
            // 只要是被处理，就必须等到所有线程的处理完成后才能放入新的消息
            prior->first = false;
        }
        return message;
    }
    
    if (!mMessageQueue.empty()) {
        message = mMessageQueue.top();
        mMessageQueue.pop();
    }
    
    return message;
}

void Queue::Recycle(Message* m)
{
    if ((m->mStatus & PRIORITY_HIGHT) == 1) {
        MLock l(qMutex);
        
        pair<bool, int>* prior = mPriorityMgr[m->mThreadID];
        if (prior && --prior->second <= 0) {
            delete prior;
            mPriorityMgr.erase(m->mThreadID);
        }
        
    }
    m->Recycle();
}


