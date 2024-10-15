#pragma once
#ifndef WORKER_H
#define WORKER_H

#include "abstractrequest.h"

#include <condition_variable>
#include <mutex>

using namespace std;


class Worker
{
    condition_variable cv;
    mutex mtx;
    unique_lock<mutex> ulock;
    AbstractRequest* request;
    //map_DataModelIntrpolation safe_DataIntrepolation;
    bool running;
    bool ready;


public:
    Worker(){ running = true; ready = false; ulock = unique_lock<mutex>(mtx); };
    void run();
    void stop(){ running = false; }
    void setRequest(AbstractRequest* request) { this->request = request; ready = true; }
    void getCondition(condition_variable* &cv);
};

#endif // WORKER_H
