#include "dispatcher.h"
#include "debug/logger.h"

#include <iostream>
#include <QDebug>


using namespace std;

queue<AbstractRequest*> dispatcher::requests;
queue<Worker*> dispatcher::workers;
mutex dispatcher::requestsMutex;
mutex dispatcher::workersMutex;
vector<Worker*> dispatcher::allWorkers;
vector<thread*> dispatcher::threads;

bool dispatcher::init(int workers)
{
    thread *t = 0;
    Worker *w = 0;
    for(int i=0; i<workers; ++i)
    {
        w = new Worker;
        allWorkers.push_back(w);
        t = new thread(&Worker::run,w);
        threads.push_back(t);
    }
    return true;
}

bool dispatcher::stop()
{
    for(int i=0;i<allWorkers.size();++i)
    {
        allWorkers[i]->stop();
    }

    logger::WriteDebug_DispatcherStop(QString("Stopped workers.").toStdString());

    for(int j=0; j<threads.size();++j)
    {
        threads[j]->join();
        logger::WriteDebug_DispatcherStop(QString(" Joinable threads .").toStdString());
    }

    return true;
}

void dispatcher::addRequest(AbstractRequest* request)
{
    workersMutex.lock();
    if(!workers.empty())
    {
        Worker* work = workers.front();
        work->setRequest(request);
        condition_variable *cv;
        work->getCondition(cv);
        cv->notify_one();
        workers.pop();
        workersMutex.unlock();
    }
    else
    {
        workersMutex.unlock();

        requestsMutex.lock();
        requests.push(request);
        requestsMutex.unlock();

    }
}

bool dispatcher::addWorker(Worker* work)
{
    bool wait = true;
    requestsMutex.lock();
    if(!requests.empty())
    {
        AbstractRequest* request = requests.front();
        work->setRequest(request);
        requests.pop();
        wait=false;
        requestsMutex.unlock();
    }
    else
    {
        requestsMutex.unlock();

        workersMutex.lock();
        workers.push(work);
        workersMutex.unlock();
    }

    return wait;

}

