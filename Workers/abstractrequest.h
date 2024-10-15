#ifndef ABSTRACTREQUEST_H
#define ABSTRACTREQUEST_H

#include <QPoint>
#include <vector>
//#include "datamodelinterpolation.h"
#include "datamodelinput.h"
#include "thrsafe/shd.h"


class AbstractRequest
{
public:


    virtual void setValue(int value) = 0;
    //virtual void setValue(map_DataModelIntrpolation &Data, DataModelInterpolation) = 0;
    virtual void setValue(DataModelInput) = 0;
    virtual void process() = 0;
    //virtual void process(map_DataModelIntrpolation &safe_DataInterpolation, DataModelInput dmi)=0;
    virtual void process(DataModelInput dmi)=0;
    virtual void finish() = 0;
};

#endif // ABSTRACTREQUEST_H
