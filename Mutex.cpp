//
//  Mutex.cpp
//  MessageLoop
//
//  Created by Lymons on 14-3-24.
//  Copyright (c) 2014年 Lymons. All rights reserved.
//

#include "Mutex.h"

#ifndef WIN32
static pthread_mutexattr_t  *sMutexAttr=NULL;
static void MutexAttrInit();
static pthread_once_t sMutexAttrInit = PTHREAD_ONCE_INIT;
#endif

MMutex::MMutex()
{
#ifdef WIN32
    ::InitializeCriticalSection(&fMutex);
#else
    (void)pthread_once(&sMutexAttrInit, MutexAttrInit);
    (void)pthread_mutex_init(&fMutex, sMutexAttr);
#endif
}

#ifndef WIN32
void MutexAttrInit()
{
    sMutexAttr = (pthread_mutexattr_t*)malloc(sizeof(pthread_mutexattr_t));
    ::memset(sMutexAttr, 0, sizeof(pthread_mutexattr_t));
    pthread_mutexattr_init(sMutexAttr);
}
#endif

MMutex::~MMutex()
{
#ifdef WIN32
    ::DeleteCriticalSection(&fMutex);
#else
    pthread_mutex_destroy(&fMutex);
#endif
}
