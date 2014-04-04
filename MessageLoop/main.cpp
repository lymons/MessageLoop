//
//  main.cpp
//  MessageLoop
//
//  Created by Lymons on 14-3-20.
//  Copyright (c) 2014年 Lymons. All rights reserved.
//

#include <iostream>
#include <pthread.h>
#include <unistd.h>

#include "Looper.h"
#include "Handler.h"
#include "Message.h"

#define MSG_PRT(pid, t, what, msg) printf("pid: %llu, time: %s, type: %d, message: %s\n", pid, t, what, msg);

Handler   *h1, *h2;
pthread_t tid1, tid2;
__uint64_t pid1, pid2;

void getHumanTime(time_t t, char *str_time, int len)
{
    struct tm *local_time = NULL;
    local_time = localtime(&t);
    strftime(str_time, len, "%m/%d,%H:%M:%S", local_time);
}

class MyHandler : public Handler
{
protected:
    bool HandleMessage(Message& m) {
        if (!Handler::HandleMessage(m)) {
            return false;
        }
        
        char timebuf[64] = {0};
        getHumanTime(m.GetSendTime(), timebuf, sizeof(timebuf));
        MSG_PRT(pid1, timebuf, m.what, m.message.c_str());
        
        sleep(5);
        
        return true;
    }
};

void* testThread1(void* ptr)
{
    pthread_threadid_np(pthread_self(), &pid1);
    
    Handler* h = (Handler*)ptr;
    h->CallMe();
    for (int i = 0; i < 1000; ++ i) {
        Message* msg = h->ObtainEmptyMessage();
        msg->what = 0x101;
        msg->message = "hello ";
        char buf[10] = {0};
        sprintf(buf, "%i", i);
        msg->message.append(buf);
        
        msg->SendMessage(false);
        
        sleep(1);
    }
    h->HangUp();
    return NULL;
}

void* testThread2(void* ptr)
{
    pthread_threadid_np(pthread_self(), &pid2);
    
    Handler* h = (Handler*)ptr;
    h->CallMe();
    for (int i = 0; i < 1000; ++ i) {
        Message* msg = h->ObtainEmptyMessage();
        msg->what = 0x201;
        msg->message = "hello ";
        char buf[10] = {0};
        sprintf(buf, "%i", i);
        msg->message.append(buf);
        
        msg->SendMessage();
        
        sleep(1);
    }
    h->HangUp();
    return NULL;
}

void testMQ()
{
    __uint64_t* pid = &pid2;
    Looper::Prepare();
    
    h1 = new MyHandler();
    h2 = new Handler();
    h2->AddActionListener(new ActionListener(
     [pid](Message& m)
     {
         char timebuf[64] = {0};
         getHumanTime(m.GetSendTime(), timebuf, sizeof(timebuf));
         MSG_PRT(*pid, timebuf, m.what, m.message.c_str());
         
         sleep(2);
     }
     ));
    
    pthread_create(&tid1, NULL, testThread1, h1);
    pthread_create(&tid2, NULL, testThread2, h2);
    
    Looper::Run();
    
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    
}

int main(int argc, const char * argv[])
{
    
    // insert code here...
    std::cout << "Hello, World!\n";
    
    testMQ();
    
    return 0;
}


