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
#include <thread>

using namespace std;

class Handler;
class Looper;

class Parcel {
protected:
    Parcel() {
        
    }
    
public:
    virtual ~Parcel() {
        
    }
    virtual Parcel& operator=(const Parcel& p) = 0;

};

class Message
{
public:
    Message();
    Message(string& mstr);
    Message(Message* m);
    ~Message();
    
    void Recycle();
    void Empty();
    int  SendMessage(bool bPending = true);
    int  SendMessage(Handler* target, bool bPending = true);
    time_t GetSendTime() {
        return mSendTime;
    }
    bool IsObtainExpired();
    
public:
    int what;
    string message;
    Parcel* parcel;
    
private:
    friend class Handler;
    friend class Looper;
    friend class Queue;
    
    void Handle();
    
    int mStatus;
    thread::id mThreadID;
    time_t mObtainTime;
    time_t mSendTime;
    Handler* mTarget;
    
};

#endif /* defined(__MessageLoop__Message__) */
