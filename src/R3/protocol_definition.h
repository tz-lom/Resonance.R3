#define PROTOCOL_DESCR \
    RECORD(JSON_Object, \
        VECTOR_ANY(keys) \
        VECTOR_ANY(values) \
    )\
    RECORD(JSON_Array, \
        VECTOR_ANY(values) \
    )\
    RECORD(JSON_Null, _VOID() ) \
    RECORD(JSON_Number, FIELD(value, double) ) \
    RECORD(JSON_Bool, FIELD(value, bool) ) \
    RECORD(JSON_String, STRING(value) ) \
    \
    RECORD(ConnectionHeader_EventBus, _VOID() )\
    \
    RECORD(EventBus_Description,\
        FIELD(created, u64) \
        FIELD(received, u64) \
        STRING(uid) \
        VECTOR_ANY(streams) \
        ANY(parameters)\
    )\
    \
    RECORD(EventBus_Description_Stream,\
        STRING(name) \
        ANY(type) \
        VECTOR_ANY(urls) \
    )\
    \
    RECORD(EventBus_Description_Url,\
           STRING(url) \
    )\
    \
    \
    RECORD(EventBus_DescribeRequest,\
        FIELD(created, u64) \
        FIELD(received, u64) \
    )\
\
    RECORD(EventBus_ApplyParameters, \
        FIELD(created, u64) \
        FIELD(received, u64) \
        ANY(parameters)\
    )\
    \
    RECORD(EventBus_Transition, \
        FIELD(created, u64) \
        FIELD(received, u64) \
        STRING(name) \
    )\
    \
    RECORD(ConnectionHeader_Message, _VOID() )\
    \
    RECORD( Message, \
        FIELD(created, u64) \
        FIELD(received, u64) \
        STRING(message) \
    )\
    RECORD(File_Stream, \
        FIELD(id, u16) \
        STRING(name) \
        ANY(info) \
    )\
    \
    RECORD(File_DataBlock, \
        FIELD(stream, u16) \
        ANY(block) \
    )\
    \
    RECORD(FileHeader, \
        FIELD(versionMajor, u16) \
        FIELD(versionMinor, u16) \
        FIELD(packed, bool) \
        FIELD(timestamp, u64) \
    )\
    \
    RECORD( Float64, \
        FIELD(created, u64) \
        FIELD(received, u64) \
        FIELD(samples, u32) \
        VECTOR(data, f64) \
    ) \
    \
    RECORD(ConnectionHeader_Int32, \
        FIELD(channels, u32) \
        FIELD(samplingRate, f64) \
        FIELD(multiplier, f64) \
    )\
    RECORD( Int32, \
        FIELD(created, u64) \
        FIELD(received, u64) \
        FIELD(samples, u32) \
        VECTOR(data, i32) \
    ) \
 \
    RECORD(ConnectionHeader_Int64, \
        FIELD(channels, u32) \
        FIELD(samplingRate, f64) \
        FIELD(multiplier, f64) \
    )\
    \
    RECORD(ConnectionHeader_Float64, \
        FIELD(channels, u32) \
        FIELD(samplingRate, f64) \
    )\
    RECORD( Int64, \
        FIELD(created, u64) \
        FIELD(received, u64) \
        FIELD(samples, u32) \
        VECTOR(data, i64) \
    )

