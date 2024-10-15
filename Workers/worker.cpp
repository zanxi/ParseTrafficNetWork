#include "worker.h"
#include "dispatcher.h"

#include <chrono>
#include <iostream>

#include "debug/logger.h"

void Worker::getCondition(condition_variable* &cv)
{
    cv = &(this)->cv;
}

void Worker::run()
{
    while(running)
    {
        if(ready)
        {
            ready = false;
            request->process();
            //request->process();
            request->finish();
        }

        if(dispatcher::addWorker(this))
        {
            while(!ready && running)
            {
                if(cv.wait_for(ulock, chrono::seconds(1)) == cv_status::timeout)
                {
                    logger::WriteDebug_process("Wait - unlock .................... \n");

                }
            }
        }
    }
}
