//
//  Message.cpp
//  MessageLoop
//
//  Created by Lymons on 14-3-20.
//  Copyright (c) 2014年 Lymons. All rights reserved.
//

#include "Message.h"
#include "Handler.h"

Message::Message()
{
    SET_OWNNER(mStatus, OWNNER_USER);
}

Message::Message(string& mstr)
{
    message = mstr;
    SET_OWNNER(mStatus, OWNNER_USER);
}

Message::Message(Message* m)
{
    what = m->what;
    message = m->message;
    parcel = m->parcel;
    mStatus = m->mStatus;
}

Message::~Message()
{
    if (parcel) {
        delete parcel;
    }
}

void Message::Handle()
{
    mTarget->HandleMessage(*this);
}

void Message::Recycle()
{
    mTarget->RecycleMessage(this);
}

void Message::Empty()
{
    
}

int Message::SendMessage()
{
    return mTarget->SendMessage(*this);
}

int Message::SendMessage(Handler* target)
{
    return target->SendMessage(*this);
}

bool Message::IsObtainExpired()
{
    if ((mStatus & OWNNER_HANDLER) != 1){
        // The message was instanced by user is never expired.
        return false;
    }
        
    time_t current = time(NULL);
    if ((current-mObtainTime) > OBTAIN_TIME_LIMIT) {
        return true;
    }
    
    return false;
}