//
//  Mutex.h
//  MessageLoop
//
//  Created by Lymons on 14-3-24.
//  Copyright (c) 2014年 Lymons. All rights reserved.
//

#ifndef __MessageLoop__Mutex__
#define __MessageLoop__Mutex__

#include <iostream>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <pthread.h>
#endif
#include <vector>

using namespace std;

class MMutex
{
public:
    
    MMutex();
    ~MMutex();
    
    inline void Lock()
    {
#ifdef WIN32
        ::EnterCriticalSection(&fMutex);
#else
        (void)pthread_mutex_lock(&fMutex);
#endif
    };
    
    inline void Unlock()
    {
#ifdef WIN32
        ::LeaveCriticalSection(&fMutex);
#else
        pthread_mutex_unlock(&fMutex);
#endif
    };
    
#ifdef WIN32
    inline CRITICAL_SECTION& getMutex(){return fMutex;}
#else
    inline pthread_mutex_t& getMutex(){return fMutex;}
#endif
    
private:
    
#ifdef WIN32
    CRITICAL_SECTION fMutex;
#else
    pthread_mutex_t fMutex;
#endif
};

class MLock
{
public:
    MLock(MMutex& cs) : _cs(cs)
    {
        _cs.Lock();
    }
    ~MLock()
    {
        _cs.Unlock();
    }
private:
    MMutex& _cs;
};

class MSemaphore
{
#ifdef WIN32
public:
    MSemaphore()
    {
        h = CreateSemaphore(NULL, 0, MAXLONG, NULL);
    }
    void Signal()
    {
        ReleaseSemaphore(h, 1, NULL);
    }
    
    bool Wait()
    {
        return WaitForSingleObject(h, INFINITE) == WAIT_OBJECT_0;
    }
    
    bool Wait(unsigned int msecond)
    {
        return WaitForSingleObject(h, msecond) == WAIT_OBJECT_0;
    }
    
    ~MSemaphore()
    {
        CloseHandle(h);
    }
    
private:
    HANDLE h;
#else
public:
    MSemaphore() : count(0)
    {
        pthread_cond_init(&cond, NULL);
    }
    void Signal()
    {
        MLock l(cs);
        count++;
        pthread_cond_signal(&cond);
    }
    
    bool Wait()
    {
        MLock l(cs);
        if(count == 0) {
            pthread_cond_wait(&cond, &cs.getMutex());
        }
        count--;
        return true;
    }
    
    bool Wait(unsigned int msecond)
    {
        MLock l(cs);
        if(count == 0) {
            timeval timev;
            timespec t;
            gettimeofday(&timev, NULL);
            t.tv_sec = timev.tv_sec + (msecond/1000);
            t.tv_nsec = (msecond%1000)*1000*1000;
            int ret = pthread_cond_timedwait(&cond, &cs.getMutex(), &t);
            if(ret != 0) {
                return false;
            }
        }
        count--;
        return true;
    }
    
private:
    pthread_cond_t cond;
    MMutex cs;
    int count;
#endif
    
public:
    MSemaphore(const MSemaphore&);
    MSemaphore& operator=(const MSemaphore&);
};

#endif /* defined(__MessageLoop__Mutex__) */
