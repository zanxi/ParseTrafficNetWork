#ifndef DATAMODELINTERPOLATION_H
#define DATAMODELINTERPOLATION_H

#include <QVector>
#include <QPointF>
//#include "NumericalCalculation/polynomial.h"
//#include "NumericalCalculation/interpolation.h"

//  -----  Участок интерполяционной кривой

class DataModelInterpolation
{
public:    
    DataModelInterpolation();
    QVector<double> xs, ys;
    //std::vector<Polynomial> polynomes;
    std::vector<QPointF> points;
    //Interpolation spline;
    int number; // номер участка массива точек
};

#endif // DATAMODELINTERPOLATION_H
