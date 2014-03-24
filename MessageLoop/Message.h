//
//  Message.h
//  MessageLoop
//
//  Created by Lymons on 14-3-20.
//  Copyright (c) 2014年 Lymons. All rights reserved.
//

#ifndef __MessageLoop__Message__
#define __MessageLoop__Message__

#include <iostream>
#include <string>

using namespace std;

class Handler;
class Looper;

class Message
{
public:
    Message();
    Message(string& mstr);
    
    void Empty();
    int SendMessage();
    int SendMessage(Handler* target);
    bool IsObtainExpired();
    
public:
    int type;
    string message;
    
private:
    friend class Handler;
    friend class Looper;
    
    void Handle();
    
    int mStatus;
    time_t mObtainTime;
    Handler* mTarget;
    
};

#endif /* defined(__MessageLoop__Message__) */
