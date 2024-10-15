#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "abstractrequest.h"
#include "worker.h"

#include <queue>
#include <mutex>
#include <thread>
#include <vector>

using namespace std;

class dispatcher
{
    static queue<Worker*> workers;
    static queue<AbstractRequest*> requests;
    static mutex requestsMutex;
    static mutex workersMutex;
    static vector<Worker*> allWorkers;
    static vector<thread*> threads;

public:
    static bool init(int workers);
    static bool stop();
    static void addRequest(AbstractRequest* request);
    static bool addWorker(Worker* work);

};

#endif // DISPATCHER_H
