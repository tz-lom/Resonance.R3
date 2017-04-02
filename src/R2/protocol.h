#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "generate.h"

#define PROTOCOL_DESCR \
    RECORD( Double, \
        FIELD(created, u64) \
        FIELD(received, u64) \
        FIELD(samples, u32) \
        VECTOR(data, f64) \
    ) \
 \
    RECORD( Int32, \
        FIELD(created, u64) \
        FIELD(received, u64) \
        FIELD(samples, u32) \
        VECTOR(data, i32) \
    ) \
 \
    RECORD( Int64, \
        FIELD(created, u64) \
        FIELD(received, u64) \
        FIELD(samples, u32) \
        VECTOR(data, i64) \
    )\
\
    RECORD( Message, \
        FIELD(created, u64) \
        FIELD(received, u64) \
        STRING(message) \
    )\
\
    RECORD( Event_Description, \
        STRING(description) \
    ) \
    \
    RECORD( Event_ApplyParameters, \
        STRING(parameters) \
    ) \
    \
    RECORD(Event_DescribeParameters, _VOID() )\
    \
    RECORD(Event_Parameters, \
        STRING(parameters) \
    ) \
    \
    RECORD(Event_Describe, _VOID()) \
\
    RECORD( Event, \
        FIELD(created, u64) \
        FIELD(received, u64) \
        STRING(from) \
        STRING(to) \
        ANY(event)\
    )\
    \
    RECORD( StreamDataBlock,\
        FIELD(stream, u8) \
        ANY(block)\
    )\
\
    \
    RECORD(ConnectionHeader_Int32, \
        FIELD(channels, u32) \
        FIELD(samplingRate, f64) \
    )\
    \
    RECORD(ConnectionHeader_Int64, \
        FIELD(channels, u32) \
        FIELD(samplingRate, f64) \
    )\
    \
    RECORD(ConnectionHeader_Double, \
        FIELD(channels, u32) \
        FIELD(samplingRate, f64) \
    )\
    RECORD(ConnectionHeader_Message, _VOID() )\
    \
    RECORD(ConnectionHeader_EventBus, _VOID() )\
    \
    RECORD(ConnectionHeader, \
        FIELD(type, u16) \
        ANY(description) \
    )\
    \
    RECORD(FileHeader, \
        FIELD(versionMajor, u16) \
        FIELD(versionMinor, u16) \
        FIELD(packed, bool) \
        FIELD(timestamp, u64) \
    )\
    \
    RECORD(File_Stream, \
        FIELD(id, u16) \
        STRING(name) \
        ANY(info) \
    )\
    \
    RECORD(File_DataBlock, \
        FIELD(stream, u16) \
        ANY(block) \
    )


namespace Resonance {
namespace R2 {

struct FileHeaderIdentificator {
    char header[2];
};

#include "generate_namespace_include.h"
GENERATE_PREFIX(PROTOCOL_DESCR)
#include "generate_class_prefix.h"
GENERATE_CLASS_H(PROTOCOL_DESCR)
#include "generate_class_postfix.h"
GENERATE_CLASS_TEMPLATES(PROTOCOL_DESCR)
#include "generate_undef.h"

}
}

#endif // PROTOCOL_H
