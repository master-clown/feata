#pragma once

#include <QMap>


template<class FirstT, class SecT>
using Pair = QPair<FirstT, SecT>;

template<class KeyT, class ValueT>
using Map = QMap<KeyT, ValueT>;
