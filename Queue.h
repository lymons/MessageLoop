//
//  Queue.h
//  MessageLoop
//
//  Created by Lymons on 14-3-20.
//  Copyright (c) 2014年 Lymons. All rights reserved.
//

#ifndef __MessageLoop__Queue__
#define __MessageLoop__Queue__

#include <iostream>
#include <queue>
#include <map>
#include <thread>
//#include <time.h>

using namespace std;

#include "Message.h"

struct MessageCmp
{
    bool operator() (Message* ma, Message* mb)
    {
        // 按照发送时间升序排列
        return ma->GetSendTime() > mb->GetSendTime();
    }
};

class Queue
{
public:
    Queue();
    
    void Recycle(Message* m);
    Message* Next();
    bool Push(Message* m);
    void Push(queue<Message*>& q);
    
private:
    Message* Pop();
    
private:
    priority_queue<Message*, vector<Message*>, MessageCmp> mMessageQueue;
    //queue<Message*> mMessageQueue;
    queue<Message*> mPriorityMessages;
    //multimap<time_t, Message*, less<time_t>> mMessageSet;
    typedef map<thread::id, pair<bool, int>*> PriorityManager;
    PriorityManager mPriorityMgr;
};

#endif /* defined(__MessageLoop__Queue__) */
