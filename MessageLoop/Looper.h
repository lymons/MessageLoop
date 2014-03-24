//
//  Looper.h
//  MessageLoop
//
//  Created by Lymons on 14-3-20.
//  Copyright (c) 2014年 Lymons. All rights reserved.
//

#ifndef __MessageLoop__Looper__
#define __MessageLoop__Looper__

#include <iostream>

class Queue;
class Message;

class Looper
{
   
public:
    static void Prepare();
    static Looper* GetLooper();
    static void Run();
    static void Finished();
    
public:
    int running;
    
private:
    friend class Handler;
    Queue *mQueue;
    bool mRuning;
    bool mFinished;
    
private:
    Looper();
};



#endif /* defined(__MessageLoop__Looper__) */
