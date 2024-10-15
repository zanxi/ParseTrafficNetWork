#pragma once

#include "thr_safe.h"
#include "tsmap.h"
#include "libh.h"

#include "datamodelinterpolation.h"

//  -----   ???????? ??????????????? ??????? ? ???? ??? ???????? ????? ????????????

// колличетво строк для хранения
const int numStrokiKvant = 128 * 8;

// ожидание блока в обработчике WorkThread
const std::chrono::milliseconds sleepTimer = 500ms;

// ожидание текущей операции в основной программе
const std::chrono::milliseconds sleepTimerMain = 750ms;

typedef tsmap<int, DataModelInterpolation> map_DataModelIntrpolation2;

//typedef contfree_safe_ptr< std::map<int, SearchTextData>> _SearchTextData;

// тип потокобезопасной очереди хранитель всех блоков строк
//typedef contfree_safe_ptr<std::list<SearchTextData>> _SearchTextData;
typedef ThreadSafeQ<DataModelInterpolation> list_DataModelInterpolation;

typedef std::map<int, DataModelInterpolation>  map_DataModelIntrpolation;

