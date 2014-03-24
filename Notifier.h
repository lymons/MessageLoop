//
//  Notifier.h
//  MessageLoop
//
//  Created by Lymons on 14-3-20.
//  Copyright (c) 2014年 Lymons. All rights reserved.
//

#ifndef __MessageLoop__Notifier__
#define __MessageLoop__Notifier__

#include <iostream>
#include <map>

#include "Handler.h"
#include "Mutex.h"

using namespace std;

template<class T>
class Notifier
{
public:
    static T& Instance() {
        // Unique point of access
        if (0 == _instance) {
            MLock l(_mutex);
            _instance = new T();
        }
        return *_instance;
    }
    
protected:
    Notifier(){}
    virtual ~Notifier(){}
    void SetHandler(Handler* h, int iden);
    void RemoveHandler(int iden);
    
private:
    static void Destroy() {
        // Destroy the only instance
        if ( _instance != 0 ) {
            MLock l(_mutex);
            delete _instance;
            _instance = 0;
        }
    }
    static T * volatile _instance; // The one and only instance
    
    map<int, Handler*> _handlers;
    
public:
    void Notify(int id);
    
public:
    virtual int  CreateServer() = 0;
    virtual int  CreateClient(Handler *h) = 0;
    virtual void DestoryServer(int id) = 0;
    virtual void DestoryClient(int id) = 0;
    virtual bool SendMessage(int id, void* m) = 0;
    virtual void* RecvMessage(int id) = 0;
    virtual void StartMonitor() = 0;
    
protected:
    static MMutex _mutex;
    
};

template<class T>
MMutex Notifier<T>::_mutex;

template<class T>
T * volatile Notifier<T>::_instance = 0;

template<class T>
void Notifier<T>::SetHandler(Handler* h, int iden)
{
    MLock l(_mutex);
    _handlers[iden] = h;
}

template<class T>
void Notifier<T>::RemoveHandler(int iden)
{
    MLock l(_mutex);
    _handlers.erase(iden);
}

template<class T>
void Notifier<T>::Notify(int id)
{
    Handler* handler = _handlers[id];
    if (handler) {
        handler->QueuePendingMessage();
    }
}


#endif /* defined(__MessageLoop__Notifier__) */
