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
//#include <map>
//#include <time.h>

using namespace std;

class Message;

class Queue
{
public:
    Queue();
    
    Message* Next();
    void Push(Message* m);
    void Push(queue<Message*>& q);
    
private:
    Message* Pop();
    
private:
    queue<Message*> mMessageQueue;
    //multimap<time_t, Message*, less<time_t>> mMessageSet;
};

#endif /* defined(__MessageLoop__Queue__) */
