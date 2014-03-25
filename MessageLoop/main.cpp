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

class MyHandler : public Handler
{
protected:
    bool HandleMessage(Message& m) {
        if (!Handler::HandleMessage(m)) {
            return false;
        }
        
        printf("type: %d, message: %s\n", m.what, m.message.c_str());
        
        sleep(5);
        
        return true;
    }
};

MyHandler *h1;

void* testThread1(void* ptr)
{
    Handler* h = (Handler*)ptr;
    h->CallMe();
    for (int i = 0; i < 1000; ++ i) {
        Message* msg = h->ObtainEmptyMessage();
        msg->what = 0x101;
        msg->message = "hello ";
        char buf[10] = {0};
        sprintf(buf, "%i", i);
        msg->message.append(buf);
        
        msg->SendMessage();
        
        sleep(2);
    }
    h->HangUp();
    return NULL;
}

void* testThread2(void* ptr)
{
    return NULL;
}

void testMQ()
{
    pthread_t tid1, tid2;
    Looper::Prepare();
    
    h1 = new MyHandler();
    
    pthread_create(&tid1, NULL, testThread1, h1);
    pthread_create(&tid2, NULL, testThread2, NULL);
    
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


