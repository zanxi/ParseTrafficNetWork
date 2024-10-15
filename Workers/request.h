#pragma once
#include <string>
#ifndef REQUEST_H
#define REQUEST_H

#include "abstractrequest.h"
#include <string.h>
#include "thrsafe/shd.h"

using namespace  std;


typedef void (*logFunction)(string);
typedef void (*safeDataInterpolationfnc)(map_DataModelIntrpolation&, DataModelInput);
typedef void (*safeDataInterpfnc)(DataModelInput);

class Request : public AbstractRequest
{

    int value;
    DataModelInput dmi;
    logFunction outFnc;
    //safeDataInterpolationfnc sdiFnc;
    safeDataInterpfnc sdiFnc;

public:
    void setValue(int value) { this->value = value; }
    //void setValue(map_DataModelIntrpolation &Data, DataModelInterpolation d){ Data.insert(std::map<int, DataModelInterpolation>::value_type(d.number,d)); };
    void setValue(DataModelInput dmi_){ dmi=dmi_; };

    //safe_ResultSearch.insert(std::map<int, InfoSearchStroka>::value_type(iS.num, iS)

    void setOutput(logFunction fnc) {outFnc = fnc; }
    void setOutput(safeDataInterpfnc fnc) { sdiFnc = fnc; }
    void process();
    //void process(map_DataModelIntrpolation &safe_DataInterpolation, DataModelInput dmi);
    void process(DataModelInput dmi);
    void finish();

};

#endif // REQUEST_H
