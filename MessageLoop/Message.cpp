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

void Message::Handle()
{
    mTarget->HandleMessage(*this);
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
    time_t current = time(NULL);
    if ((current-mObtainTime) > OBTAIN_TIME_LIMIT) {
        return true;
    }
    
    return false;
}