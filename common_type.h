#ifndef COMMON_TYPE_H
#define COMMON_TYPE_H
#include "devMacro.h"
#include <complex>

#if _DEV_QT
#   include <QColor>
#   include <QString>
#else

#endif

#if _DEV_QT
#   define NULL_ADD nullptr
    typedef QString SVString;
    typedef QColor  SVColor;
#else
#   define NULL_ADD NULL
#endif
typedef std::complex<double>        Complex;
typedef unsigned char               IndexOfPt;
#endif // COMMON_TYPE_H
