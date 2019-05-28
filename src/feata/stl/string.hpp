#pragma once

#include <QString>
#include <QStringList>
#include "defs/common.hpp"


using Char = QChar;
using String = QString;
using StringList = QStringList;

inline String operator""_qs(const char* str, size_t)
{
    return String(str);
}

template<class T>
String TO_STRING(const T& x)
{
    return QString("%1").arg(x);
}
