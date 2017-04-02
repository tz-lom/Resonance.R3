#include <stdint.h>
#include <numeric>
#include <vector>
#include <string>
#include <cstring>
#include <cassert>
#include <algorithm>

#if defined(QT_VERSION)||defined(COMLIB_EXPORTS)
#include <QString>
#include <QVector>
#include <QByteArray>
#endif

#ifdef PROTOCOL_PREBUILD
    #include "protocol.generated.cpp.h"
#else
    #include "protocol.generator.cpp.h"
#endif
