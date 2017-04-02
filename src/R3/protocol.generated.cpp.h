

namespace Resonance {
namespace R3 {

template <typename T>
T byte_order_to_le(T x) {
  return x;
}
template <typename T>
T byte_order_from_le(T x) {
  return x;
}

typedef uint64_t u64;
typedef uint32_t u32;
typedef int64_t i64;
typedef int32_t i32;
typedef float f32;
typedef double f64;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint8_t u8;
typedef int8_t i8;

struct First {};

template <typename field>
struct calcStaticOffset {
  enum {
    V = field::staticSize + calcStaticOffset<typename field::previous>::V
  };
};

template <>
struct calcStaticOffset<First> {
  enum { V = 0 };
};

template <typename field>
struct calcDynamicOffset {
  enum {
    V = field::headerSize + calcDynamicOffset<typename field::previous>::V
  };
};

template <>
struct calcDynamicOffset<First> {
  enum { V = 0 };
};

struct __Last {
  typedef __Last F;
  typedef __Last N;
};
struct __LastRecursive {
  typedef __LastRecursive F;
  typedef __LastRecursive N;
};
struct __Finish {
  typedef __Finish F;
  typedef __Finish N;
};

class SerializedData;

class RecordConstructor {
 public:
  RecordConstructor(SerializedData* data, size_t staticSize, size_t headerSize);

  char* staticData(size_t size);
  char* dynamicData(size_t size);
  void finishDynamicSize();

  RecordConstructor* beginNested(u16 id,
                                 size_t staticSize,
                                 size_t headerSize,
                                 bool last);
  RecordConstructor* finishNested();

  SerializedData* root();

 protected:
  SerializedData* data;

  size_t dynamicOffset;

  std::vector<size_t> staticOffset;
  std::vector<size_t> dynamicSizeOffset;
  std::vector<bool> finishDynamic;
};

template <int recordId, typename self>
struct Record {
  enum { ID = recordId };

  template <typename type, typename parent>
  struct Field {
    typedef type T;
    typedef bool isField;

    enum {
      staticOffset = calcStaticOffset<parent>::V,
      dynamicOffset = calcDynamicOffset<parent>::V,
      headerSize = 0,
      staticSize = sizeof(T)
    };

    typedef parent previous;
    typedef self record;
  };

  template <typename type, typename parent>
  struct Vector {
    typedef type Tv;
    typedef type* V;
    typedef bool isVector;

    enum {
      staticOffset = calcStaticOffset<parent>::V,
      dynamicOffset = calcDynamicOffset<parent>::V,
      headerSize = 1,
      staticSize = 0
    };

    typedef parent previous;
    typedef self record;
  };

  template <typename notused, typename parent>
  struct Any {
    typedef bool isAny;

    enum {
      staticOffset = calcStaticOffset<parent>::V,
      dynamicOffset = calcDynamicOffset<parent>::V,
      headerSize = 1,
      staticSize = 0
    };

    typedef parent previous;
    typedef self record;
  };

  template <typename notused, typename parent>
  struct Void {
    typedef bool isVoid;

    enum {
      staticOffset = calcStaticOffset<parent>::V,
      dynamicOffset = calcDynamicOffset<parent>::V,
      headerSize = 0,
      staticSize = 0
    };

    typedef parent previous;
    typedef self record;
  };

  template <typename notused, typename parent>
  struct AnyVector {
    typedef bool isAnyVector;

    enum {
      staticOffset = calcStaticOffset<parent>::V,
      dynamicOffset = calcDynamicOffset<parent>::V,
      headerSize = 1,
      staticSize = 0
    };

    typedef parent previous;
    typedef self record;
  };
};

struct SerializedException {};
struct WrongType : SerializedException {};
struct WrongIndex : SerializedException {};

template <typename Field, typename Next, typename Dummy = void>
class ValueSetter {};
typedef ValueSetter<__LastRecursive, SerializedData*> LastRecursive;

template <typename T>
class stringConvert {
 public:
  typedef T IteratorType;

  stringConvert(T& in);

  IteratorType begin();
  IteratorType end();
};

template <typename T>
class vectorConvert {
 public:
  typedef T IteratorType;

  vectorConvert(T& in);

  IteratorType begin();
  IteratorType end();
};
template <typename T>
class vectorConvert< ::std::vector<T> > {
 public:
  typedef typename ::std::vector<T>::const_iterator IteratorType;

  vectorConvert(::std::vector<T>& in) : vec(in) {}

  IteratorType begin() { return vec.begin(); }

  IteratorType end() { return vec.end(); }

 private:
  ::std::vector<T>& vec;
};

template <typename T>
struct VectorContainer {
  VectorContainer(T* data, ::std::size_t size) : data(data), size(size) {}

  T* data;
  ::std::size_t size;
};

template <typename T>
class vectorConvert<VectorContainer<T> > {
 public:
  typedef T* IteratorType;

  vectorConvert(VectorContainer<T> in) : vec(in) {}

  IteratorType begin() { return vec.data; }

  IteratorType end() { return vec.data + vec.size; }

 private:
  VectorContainer<T> vec;
};

template <>
class vectorConvert<QString> {
 public:
  typedef ::QByteArray::const_iterator IteratorType;

  vectorConvert(QString& in) : ba(in.toUtf8()) {}

  IteratorType begin() { return ba.constBegin(); }

  IteratorType end() { return ba.constEnd(); }

 private:
  ::QByteArray ba;
};

template <typename T>
class vectorConvert< ::QVector<T> > {
 public:
  typedef typename ::QVector<T>::const_iterator IteratorType;

  vectorConvert(::QVector<T>& in) : vec(in) {}

  IteratorType begin() { return vec.constBegin(); }

  IteratorType end() { return vec.constEnd(); }

 private:
  ::QVector<T> vec;
};

template <>
class vectorConvert< ::std::string> {
 public:
  typedef std::string::const_iterator IteratorType;

  vectorConvert(::std::string& in) : str(in) {}

  IteratorType begin() { return str.begin(); }

  IteratorType end() { return str.end(); }

 private:
  ::std::string& str;
};

template <>
class vectorConvert<const char*> {
 public:
  typedef const char* IteratorType;

  vectorConvert(const char*& in) : str(in) {}

  IteratorType begin() { return str; }

  IteratorType end() { return str + strlen(str); }

 private:
  const char* str;
};

template <>
class vectorConvert<char*> {
 public:
  typedef char* IteratorType;

  vectorConvert(char*& in) : str(in) {}

  IteratorType begin() { return str; }

  IteratorType end() { return str + strlen(str); }

 private:
  char* str;
};
struct JSON_Object : Record<0 + 1, JSON_Object> {
  struct keys : AnyVector<First, First> {};
  struct values : AnyVector<First, keys> {};
  enum {
    headerSize = 0 + keys::headerSize + values::headerSize,
    staticSize = 0
  };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<JSON_Object::keys,
                        ValueSetter<JSON_Object::values, Continue> >
        type;
  };
  typedef ValueSetter<
      JSON_Object::keys,
      ValueSetter<JSON_Object::values,
                  ValueSetter<__LastRecursive, __LastRecursive> > >
      recursive;
  static JSON_Object::constructor<ValueSetter<__Last, __Last> >::type create();
  static recursive createRecursive();
};
struct JSON_Array : Record<1 + 1, JSON_Array> {
  struct values : AnyVector<First, First> {};
  enum { headerSize = 0 + values::headerSize, staticSize = 0 };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<JSON_Array::values, Continue> type;
  };
  typedef ValueSetter<JSON_Array::values,
                      ValueSetter<__LastRecursive, __LastRecursive> >
      recursive;
  static JSON_Array::constructor<ValueSetter<__Last, __Last> >::type create();
  static recursive createRecursive();
};
struct JSON_Null : Record<2 + 1, JSON_Null> {
  struct __void : Void<First, First> {};
  enum { headerSize = 0 + __void::headerSize, staticSize = 0 };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<JSON_Null::__void, Continue> type;
  };
  typedef ValueSetter<JSON_Null::__void,
                      ValueSetter<__LastRecursive, __LastRecursive> >
      recursive;
  static JSON_Null::constructor<ValueSetter<__Last, __Last> >::type create();
  static recursive createRecursive();
};
struct JSON_Number : Record<3 + 1, JSON_Number> {
  struct value : Field<double, First> {};
  enum {
    headerSize = 0 + value::headerSize,
    staticSize = 0 + sizeof(value::T)
  };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<JSON_Number::value, Continue> type;
  };
  typedef ValueSetter<JSON_Number::value,
                      ValueSetter<__LastRecursive, __LastRecursive> >
      recursive;
  static JSON_Number::constructor<ValueSetter<__Last, __Last> >::type create();
  static recursive createRecursive();
};
struct JSON_Bool : Record<4 + 1, JSON_Bool> {
  struct value : Field<bool, First> {};
  enum {
    headerSize = 0 + value::headerSize,
    staticSize = 0 + sizeof(value::T)
  };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<JSON_Bool::value, Continue> type;
  };
  typedef ValueSetter<JSON_Bool::value,
                      ValueSetter<__LastRecursive, __LastRecursive> >
      recursive;
  static JSON_Bool::constructor<ValueSetter<__Last, __Last> >::type create();
  static recursive createRecursive();
};
struct JSON_String : Record<5 + 1, JSON_String> {
  struct value : Vector<char, First> {};
  enum { headerSize = 0 + value::headerSize, staticSize = 0 };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<JSON_String::value, Continue> type;
  };
  typedef ValueSetter<JSON_String::value,
                      ValueSetter<__LastRecursive, __LastRecursive> >
      recursive;
  static JSON_String::constructor<ValueSetter<__Last, __Last> >::type create();
  static recursive createRecursive();
};
struct ConnectionHeader_EventBus : Record<6 + 1, ConnectionHeader_EventBus> {
  struct __void : Void<First, First> {};
  enum { headerSize = 0 + __void::headerSize, staticSize = 0 };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<ConnectionHeader_EventBus::__void, Continue> type;
  };
  typedef ValueSetter<ConnectionHeader_EventBus::__void,
                      ValueSetter<__LastRecursive, __LastRecursive> >
      recursive;
  static ConnectionHeader_EventBus::constructor<
      ValueSetter<__Last, __Last> >::type
  create();
  static recursive createRecursive();
};
struct EventBus_Description : Record<7 + 1, EventBus_Description> {
  struct created : Field<u64, First> {};
  struct received : Field<u64, created> {};
  struct uid : Vector<char, received> {};
  struct streams : AnyVector<First, uid> {};
  struct parameters : Any<First, streams> {};
  enum {
    headerSize = 0 + created::headerSize + received::headerSize +
                 uid::headerSize +
                 streams::headerSize +
                 parameters::headerSize,
    staticSize = 0 + sizeof(created::T) + sizeof(received::T)
  };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<
        EventBus_Description::created,
        ValueSetter<
            EventBus_Description::received,
            ValueSetter<
                EventBus_Description::uid,
                ValueSetter<EventBus_Description::streams,
                            ValueSetter<EventBus_Description::parameters,
                                        Continue> > > > >
        type;
  };
  typedef ValueSetter<
      EventBus_Description::created,
      ValueSetter<
          EventBus_Description::received,
          ValueSetter<
              EventBus_Description::uid,
              ValueSetter<EventBus_Description::streams,
                          ValueSetter<EventBus_Description::parameters,
                                      ValueSetter<__LastRecursive,
                                                  __LastRecursive> > > > > >
      recursive;
  static EventBus_Description::constructor<ValueSetter<__Last, __Last> >::type
  create();
  static recursive createRecursive();
};
struct EventBus_Description_Stream
    : Record<8 + 1, EventBus_Description_Stream> {
  struct name : Vector<char, First> {};
  struct type : Any<First, name> {};
  struct urls : AnyVector<First, type> {};
  enum {
    headerSize = 0 + name::headerSize + type::headerSize + urls::headerSize,
    staticSize = 0
  };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<
        EventBus_Description_Stream::name,
        ValueSetter<EventBus_Description_Stream::type,
                    ValueSetter<EventBus_Description_Stream::urls, Continue> > >
        type;
  };
  typedef ValueSetter<
      EventBus_Description_Stream::name,
      ValueSetter<
          EventBus_Description_Stream::type,
          ValueSetter<EventBus_Description_Stream::urls,
                      ValueSetter<__LastRecursive, __LastRecursive> > > >
      recursive;
  static EventBus_Description_Stream::constructor<
      ValueSetter<__Last, __Last> >::type
  create();
  static recursive createRecursive();
};
struct EventBus_Description_Url : Record<9 + 1, EventBus_Description_Url> {
  struct url : Vector<char, First> {};
  enum { headerSize = 0 + url::headerSize, staticSize = 0 };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<EventBus_Description_Url::url, Continue> type;
  };
  typedef ValueSetter<EventBus_Description_Url::url,
                      ValueSetter<__LastRecursive, __LastRecursive> >
      recursive;
  static EventBus_Description_Url::constructor<
      ValueSetter<__Last, __Last> >::type
  create();
  static recursive createRecursive();
};
struct EventBus_DescribeRequest : Record<10 + 1, EventBus_DescribeRequest> {
  struct created : Field<u64, First> {};
  struct received : Field<u64, created> {};
  enum {
    headerSize = 0 + created::headerSize + received::headerSize,
    staticSize = 0 + sizeof(created::T) + sizeof(received::T)
  };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<
        EventBus_DescribeRequest::created,
        ValueSetter<EventBus_DescribeRequest::received, Continue> >
        type;
  };
  typedef ValueSetter<
      EventBus_DescribeRequest::created,
      ValueSetter<EventBus_DescribeRequest::received,
                  ValueSetter<__LastRecursive, __LastRecursive> > >
      recursive;
  static EventBus_DescribeRequest::constructor<
      ValueSetter<__Last, __Last> >::type
  create();
  static recursive createRecursive();
};
struct EventBus_ApplyParameters : Record<11 + 1, EventBus_ApplyParameters> {
  struct created : Field<u64, First> {};
  struct received : Field<u64, created> {};
  struct parameters : Any<First, received> {};
  enum {
    headerSize =
        0 + created::headerSize + received::headerSize + parameters::headerSize,
    staticSize = 0 + sizeof(created::T) + sizeof(received::T)
  };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<
        EventBus_ApplyParameters::created,
        ValueSetter<
            EventBus_ApplyParameters::received,
            ValueSetter<EventBus_ApplyParameters::parameters, Continue> > >
        type;
  };
  typedef ValueSetter<
      EventBus_ApplyParameters::created,
      ValueSetter<
          EventBus_ApplyParameters::received,
          ValueSetter<EventBus_ApplyParameters::parameters,
                      ValueSetter<__LastRecursive, __LastRecursive> > > >
      recursive;
  static EventBus_ApplyParameters::constructor<
      ValueSetter<__Last, __Last> >::type
  create();
  static recursive createRecursive();
};
struct EventBus_Transition : Record<12 + 1, EventBus_Transition> {
  struct created : Field<u64, First> {};
  struct received : Field<u64, created> {};
  struct name : Vector<char, received> {};
  enum {
    headerSize =
        0 + created::headerSize + received::headerSize + name::headerSize,
    staticSize = 0 + sizeof(created::T) + sizeof(received::T)
  };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<
        EventBus_Transition::created,
        ValueSetter<EventBus_Transition::received,
                    ValueSetter<EventBus_Transition::name, Continue> > >
        type;
  };
  typedef ValueSetter<
      EventBus_Transition::created,
      ValueSetter<
          EventBus_Transition::received,
          ValueSetter<EventBus_Transition::name,
                      ValueSetter<__LastRecursive, __LastRecursive> > > >
      recursive;
  static EventBus_Transition::constructor<ValueSetter<__Last, __Last> >::type
  create();
  static recursive createRecursive();
};
struct ConnectionHeader_Message : Record<13 + 1, ConnectionHeader_Message> {
  struct __void : Void<First, First> {};
  enum { headerSize = 0 + __void::headerSize, staticSize = 0 };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<ConnectionHeader_Message::__void, Continue> type;
  };
  typedef ValueSetter<ConnectionHeader_Message::__void,
                      ValueSetter<__LastRecursive, __LastRecursive> >
      recursive;
  static ConnectionHeader_Message::constructor<
      ValueSetter<__Last, __Last> >::type
  create();
  static recursive createRecursive();
};
struct Message : Record<14 + 1, Message> {
  struct created : Field<u64, First> {};
  struct received : Field<u64, created> {};
  struct message : Vector<char, received> {};
  enum {
    headerSize =
        0 + created::headerSize + received::headerSize + message::headerSize,
    staticSize = 0 + sizeof(created::T) + sizeof(received::T)
  };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<Message::created,
                        ValueSetter<Message::received,
                                    ValueSetter<Message::message, Continue> > >
        type;
  };
  typedef ValueSetter<
      Message::created,
      ValueSetter<
          Message::received,
          ValueSetter<Message::message,
                      ValueSetter<__LastRecursive, __LastRecursive> > > >
      recursive;
  static Message::constructor<ValueSetter<__Last, __Last> >::type create();
  static recursive createRecursive();
};
struct File_Stream : Record<15 + 1, File_Stream> {
  struct id : Field<u16, First> {};
  struct name : Vector<char, id> {};
  struct info : Any<First, name> {};
  enum {
    headerSize = 0 + id::headerSize + name::headerSize + info::headerSize,
    staticSize = 0 + sizeof(id::T)
  };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<File_Stream::id,
                        ValueSetter<File_Stream::name,
                                    ValueSetter<File_Stream::info, Continue> > >
        type;
  };
  typedef ValueSetter<
      File_Stream::id,
      ValueSetter<
          File_Stream::name,
          ValueSetter<File_Stream::info,
                      ValueSetter<__LastRecursive, __LastRecursive> > > >
      recursive;
  static File_Stream::constructor<ValueSetter<__Last, __Last> >::type create();
  static recursive createRecursive();
};
struct File_DataBlock : Record<16 + 1, File_DataBlock> {
  struct stream : Field<u16, First> {};
  struct block : Any<First, stream> {};
  enum {
    headerSize = 0 + stream::headerSize + block::headerSize,
    staticSize = 0 + sizeof(stream::T)
  };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<File_DataBlock::stream,
                        ValueSetter<File_DataBlock::block, Continue> >
        type;
  };
  typedef ValueSetter<
      File_DataBlock::stream,
      ValueSetter<File_DataBlock::block,
                  ValueSetter<__LastRecursive, __LastRecursive> > >
      recursive;
  static File_DataBlock::constructor<ValueSetter<__Last, __Last> >::type
  create();
  static recursive createRecursive();
};
struct FileHeader : Record<17 + 1, FileHeader> {
  struct versionMajor : Field<u16, First> {};
  struct versionMinor : Field<u16, versionMajor> {};
  struct packed : Field<bool, versionMinor> {};
  struct timestamp : Field<u64, packed> {};
  enum {
    headerSize = 0 + versionMajor::headerSize + versionMinor::headerSize +
                 packed::headerSize +
                 timestamp::headerSize,
    staticSize = 0 + sizeof(versionMajor::T) + sizeof(versionMinor::T) +
                 sizeof(packed::T) +
                 sizeof(timestamp::T)
  };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<
        FileHeader::versionMajor,
        ValueSetter<
            FileHeader::versionMinor,
            ValueSetter<FileHeader::packed,
                        ValueSetter<FileHeader::timestamp, Continue> > > >
        type;
  };
  typedef ValueSetter<
      FileHeader::versionMajor,
      ValueSetter<FileHeader::versionMinor,
                  ValueSetter<FileHeader::packed,
                              ValueSetter<FileHeader::timestamp,
                                          ValueSetter<__LastRecursive,
                                                      __LastRecursive> > > > >
      recursive;
  static FileHeader::constructor<ValueSetter<__Last, __Last> >::type create();
  static recursive createRecursive();
};
struct Float64 : Record<18 + 1, Float64> {
  struct created : Field<u64, First> {};
  struct received : Field<u64, created> {};
  struct samples : Field<u32, received> {};
  struct data : Vector<f64, samples> {};
  enum {
    headerSize = 0 + created::headerSize + received::headerSize +
                 samples::headerSize +
                 data::headerSize,
    staticSize =
        0 + sizeof(created::T) + sizeof(received::T) + sizeof(samples::T)
  };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<
        Float64::created,
        ValueSetter<Float64::received,
                    ValueSetter<Float64::samples,
                                ValueSetter<Float64::data, Continue> > > >
        type;
  };
  typedef ValueSetter<
      Float64::created,
      ValueSetter<Float64::received,
                  ValueSetter<Float64::samples,
                              ValueSetter<Float64::data,
                                          ValueSetter<__LastRecursive,
                                                      __LastRecursive> > > > >
      recursive;
  static Float64::constructor<ValueSetter<__Last, __Last> >::type create();
  static recursive createRecursive();
};
struct ConnectionHeader_Int32 : Record<19 + 1, ConnectionHeader_Int32> {
  struct channels : Field<u32, First> {};
  struct samplingRate : Field<f64, channels> {};
  struct multiplier : Field<f64, samplingRate> {};
  enum {
    headerSize = 0 + channels::headerSize + samplingRate::headerSize +
                 multiplier::headerSize,
    staticSize = 0 + sizeof(channels::T) + sizeof(samplingRate::T) +
                 sizeof(multiplier::T)
  };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<
        ConnectionHeader_Int32::channels,
        ValueSetter<
            ConnectionHeader_Int32::samplingRate,
            ValueSetter<ConnectionHeader_Int32::multiplier, Continue> > >
        type;
  };
  typedef ValueSetter<
      ConnectionHeader_Int32::channels,
      ValueSetter<
          ConnectionHeader_Int32::samplingRate,
          ValueSetter<ConnectionHeader_Int32::multiplier,
                      ValueSetter<__LastRecursive, __LastRecursive> > > >
      recursive;
  static ConnectionHeader_Int32::constructor<ValueSetter<__Last, __Last> >::type
  create();
  static recursive createRecursive();
};
struct Int32 : Record<20 + 1, Int32> {
  struct created : Field<u64, First> {};
  struct received : Field<u64, created> {};
  struct samples : Field<u32, received> {};
  struct data : Vector<i32, samples> {};
  enum {
    headerSize = 0 + created::headerSize + received::headerSize +
                 samples::headerSize +
                 data::headerSize,
    staticSize =
        0 + sizeof(created::T) + sizeof(received::T) + sizeof(samples::T)
  };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<
        Int32::created,
        ValueSetter<
            Int32::received,
            ValueSetter<Int32::samples, ValueSetter<Int32::data, Continue> > > >
        type;
  };
  typedef ValueSetter<
      Int32::created,
      ValueSetter<Int32::received,
                  ValueSetter<Int32::samples,
                              ValueSetter<Int32::data,
                                          ValueSetter<__LastRecursive,
                                                      __LastRecursive> > > > >
      recursive;
  static Int32::constructor<ValueSetter<__Last, __Last> >::type create();
  static recursive createRecursive();
};
struct ConnectionHeader_Int64 : Record<21 + 1, ConnectionHeader_Int64> {
  struct channels : Field<u32, First> {};
  struct samplingRate : Field<f64, channels> {};
  struct multiplier : Field<f64, samplingRate> {};
  enum {
    headerSize = 0 + channels::headerSize + samplingRate::headerSize +
                 multiplier::headerSize,
    staticSize = 0 + sizeof(channels::T) + sizeof(samplingRate::T) +
                 sizeof(multiplier::T)
  };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<
        ConnectionHeader_Int64::channels,
        ValueSetter<
            ConnectionHeader_Int64::samplingRate,
            ValueSetter<ConnectionHeader_Int64::multiplier, Continue> > >
        type;
  };
  typedef ValueSetter<
      ConnectionHeader_Int64::channels,
      ValueSetter<
          ConnectionHeader_Int64::samplingRate,
          ValueSetter<ConnectionHeader_Int64::multiplier,
                      ValueSetter<__LastRecursive, __LastRecursive> > > >
      recursive;
  static ConnectionHeader_Int64::constructor<ValueSetter<__Last, __Last> >::type
  create();
  static recursive createRecursive();
};
struct ConnectionHeader_Float64 : Record<22 + 1, ConnectionHeader_Float64> {
  struct channels : Field<u32, First> {};
  struct samplingRate : Field<f64, channels> {};
  enum {
    headerSize = 0 + channels::headerSize + samplingRate::headerSize,
    staticSize = 0 + sizeof(channels::T) + sizeof(samplingRate::T)
  };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<
        ConnectionHeader_Float64::channels,
        ValueSetter<ConnectionHeader_Float64::samplingRate, Continue> >
        type;
  };
  typedef ValueSetter<
      ConnectionHeader_Float64::channels,
      ValueSetter<ConnectionHeader_Float64::samplingRate,
                  ValueSetter<__LastRecursive, __LastRecursive> > >
      recursive;
  static ConnectionHeader_Float64::constructor<
      ValueSetter<__Last, __Last> >::type
  create();
  static recursive createRecursive();
};
struct Int64 : Record<23 + 1, Int64> {
  struct created : Field<u64, First> {};
  struct received : Field<u64, created> {};
  struct samples : Field<u32, received> {};
  struct data : Vector<i64, samples> {};
  enum {
    headerSize = 0 + created::headerSize + received::headerSize +
                 samples::headerSize +
                 data::headerSize,
    staticSize =
        0 + sizeof(created::T) + sizeof(received::T) + sizeof(samples::T)
  };
  template <typename Continue>
  struct constructor {
    typedef ValueSetter<
        Int64::created,
        ValueSetter<
            Int64::received,
            ValueSetter<Int64::samples, ValueSetter<Int64::data, Continue> > > >
        type;
  };
  typedef ValueSetter<
      Int64::created,
      ValueSetter<Int64::received,
                  ValueSetter<Int64::samples,
                              ValueSetter<Int64::data,
                                          ValueSetter<__LastRecursive,
                                                      __LastRecursive> > > > >
      recursive;
  static Int64::constructor<ValueSetter<__Last, __Last> >::type create();
  static recursive createRecursive();
};
class SerializedData {
  friend class RecordConstructor;

 public:
  typedef u16 rid;
  typedef u32 hel;

  template <typename Field>
  typename Field::T extractField() const {
    if (id() != Field::record::ID)
      throw WrongType();

    const char* offset =
        data() + sizeof(rid) + sizeof(hel) * Field::record::headerSize;

    offset += Field::staticOffset;
    for (int i = Field::dynamicOffset - 1; i >= 0; --i) {
      offset += byte_order_from_le(
          reinterpret_cast<const hel*>(data() + sizeof(rid))[i]);
    }
    return byte_order_from_le(
        *(reinterpret_cast<const typename Field::T*>(offset)));
  }

  template <typename Field>
  void setField(const typename Field::T t) {
    if (id() != Field::record::ID)
      throw WrongType();

    char* offset =
        data() + sizeof(rid) + sizeof(hel) * Field::record::headerSize;

    offset += Field::staticOffset;
    for (int i = Field::dynamicOffset - 1; i >= 0; --i) {
      offset +=
          byte_order_from_le(reinterpret_cast<hel*>(data() + sizeof(rid))[i]);
    }
    *(reinterpret_cast<typename Field::T*>(offset)) = byte_order_to_le(t);
  }

  template <typename Vector>
  std::vector<typename Vector::Tv> extractVector() const {
    if (id() != Vector::record::ID)
      throw WrongType();

    const char* offset =
        data() + sizeof(rid) + sizeof(hel) * Vector::record::headerSize;

    offset += Vector::staticOffset;
    for (int i = Vector::dynamicOffset - 1; i >= 0; --i) {
      offset += byte_order_from_le(
          reinterpret_cast<const hel*>(data() + sizeof(rid))[i]);
    }

    std::vector<typename Vector::Tv> result;

    hel size = reinterpret_cast<const hel*>(data() +
                                            sizeof(rid))[Vector::dynamicOffset];
    result.resize(size / sizeof(typename Vector::Tv));

    typename Vector::V first = reinterpret_cast<typename Vector::V>(offset);
    typename Vector::V last = first + size / sizeof(typename Vector::Tv);

    typename std::vector<typename Vector::Tv>::iterator rp = result.begin();

    while (first != last) {
      *rp = byte_order_from_le(*first);
      ++rp;
      ++first;
    }
    return result;
  }

  template <typename Any>
  SerializedData extractAny() const {
    if (id() != Any::record::ID)
      throw WrongType();
    const char* offset =
        data() + sizeof(rid) + sizeof(hel) * Any::record::headerSize;
    offset += Any::staticOffset;
    for (int i = Any::dynamicOffset - 1; i >= 0; --i) {
      offset += byte_order_from_le(
          reinterpret_cast<const hel*>(data() + sizeof(rid))[i]);
    }
    return SerializedData(offset,
                          byte_order_from_le(reinterpret_cast<const hel*>(
                              data() + sizeof(rid))[Any::dynamicOffset]));
  }

  template <typename Vector>
  std::string extractString() const {
    if (id() != Vector::record::ID)
      throw WrongType();

    const char* offset =
        data() + sizeof(rid) + sizeof(hel) * Vector::record::headerSize;

    offset += Vector::staticOffset;
    for (int i = Vector::dynamicOffset - 1; i >= 0; --i) {
      offset += byte_order_from_le(
          reinterpret_cast<const hel*>(data() + sizeof(rid))[i]);
    }

    hel size = byte_order_from_le(reinterpret_cast<const hel*>(
        data() + sizeof(rid))[Vector::dynamicOffset]);

    return std::string(offset, size);
  }

  template <typename Vector>
  QVector<typename Vector::Tv> extractQVector() const {
    if (id() != Vector::record::ID)
      throw WrongType();

    const char* offset =
        data() + sizeof(rid) + sizeof(hel) * Vector::record::headerSize;

    offset += Vector::staticOffset;
    for (int i = Vector::dynamicOffset - 1; i >= 0; --i) {
      offset += byte_order_from_le(
          reinterpret_cast<const hel*>(data() + sizeof(rid))[i]);
    }

    QVector<typename Vector::Tv> result;

    hel size = byte_order_from_le(reinterpret_cast<const hel*>(
        data() + sizeof(rid))[Vector::dynamicOffset]);
    result.resize(size / sizeof(typename Vector::Tv));

    typename Vector::V first = reinterpret_cast<typename Vector::V>(offset);
    typename Vector::V last = first + size / sizeof(typename Vector::Tv);

    typename QVector<typename Vector::Tv>::iterator rp = result.begin();

    while (first != last) {
      *rp = byte_order_from_le(*first);
      ++rp;
      ++first;
    }
    return result;
  }

  template <typename Vector>
  QString extractQString() const {
    if (id() != Vector::record::ID)
      throw WrongType();

    const char* offset =
        data() + sizeof(rid) + sizeof(hel) * Vector::record::headerSize;

    offset += Vector::staticOffset;
    for (int i = Vector::dynamicOffset - 1; i >= 0; --i) {
      offset += byte_order_from_le(
          reinterpret_cast<const hel*>(data() + sizeof(rid))[i]);
    }

    hel size = byte_order_from_le(reinterpret_cast<const hel*>(
        data() + sizeof(rid))[Vector::dynamicOffset]);

    return QString::fromUtf8(offset, size);
  }

  QByteArray dataAsByteArray();

  template <typename AnyVector>
  SerializedData extractAnyVectorElement(size_t index) const {
    if (id() != AnyVector::record::ID)
      throw WrongType();

    const char* offset =
        data() + sizeof(rid) + sizeof(hel) * AnyVector::record::headerSize;

    hel self_size = byte_order_from_le(reinterpret_cast<const hel*>(
        data() + sizeof(rid))[AnyVector::dynamicOffset]);

    if (self_size == 0)
      throw WrongIndex();

    offset += AnyVector::staticOffset;
    for (int i = AnyVector::dynamicOffset - 1; i >= 0; --i) {
      offset += byte_order_from_le(
          reinterpret_cast<const hel*>(data() + sizeof(rid))[i]);
    }

    for (size_t i = index; i > 0; --i) {
      rid id = byte_order_from_le(*reinterpret_cast<const rid*>(offset));
      offset += sizeof(rid);

      size_t sz = 0;
      for (int i = headerSize(id) - 1; i >= 0; --i, offset += sizeof(hel)) {
        sz += reinterpret_cast<const hel*>(data() + sizeof(rid))[i];
      }
      offset += sz + staticSizes[id];

      if (offset > data() + self_size)
        throw WrongIndex();
    }

    return SerializedData(offset, 2);
  }

  template <typename AnyVector>
  std::vector<SerializedData> extractAnyVector() const {
    if (id() != AnyVector::record::ID)
      throw WrongType();

    const char* offset =
        data() + sizeof(rid) + sizeof(hel) * AnyVector::record::headerSize;

    std::vector<SerializedData> result;

    offset += AnyVector::staticOffset;
    for (int i = AnyVector::dynamicOffset - 1; i >= 0; --i) {
      offset += byte_order_from_le(
          reinterpret_cast<const hel*>(data() + sizeof(rid))[i]);
    }

    const char* end =
        offset + byte_order_from_le(reinterpret_cast<const hel*>(
                     data() + sizeof(rid))[AnyVector::dynamicOffset]);

    while (offset < end) {
      SerializedData extr(offset, 2);
      result.push_back(extr);
      offset += extr.size();
    }

    return result;
  }

  typedef void (*reader)(char* memory, size_t size, void* opt);

  SerializedData(reader readdata, void* opt = 0);
  SerializedData(const char* block, size_t size);

  inline const char* data() const {
    if (block == 0) {
      return &vec[0];
    } else {
      return block;
    }
  }

  inline char* data() {
    if (block == 0) {
      return &vec[0];
    } else {
      return block;
    }
  }

  inline rid id() const {
    if (allocated == 0)
      return 0;
    return *(reinterpret_cast<const rid*>(data()));
  }

  std::size_t size() const;
  enum { __LastType = Int64::ID };
  friend struct JSON_Object;
  friend struct JSON_Array;
  friend struct JSON_Null;
  friend struct JSON_Number;
  friend struct JSON_Bool;
  friend struct JSON_String;
  friend struct ConnectionHeader_EventBus;
  friend struct EventBus_Description;
  friend struct EventBus_Description_Stream;
  friend struct EventBus_Description_Url;
  friend struct EventBus_DescribeRequest;
  friend struct EventBus_ApplyParameters;
  friend struct EventBus_Transition;
  friend struct ConnectionHeader_Message;
  friend struct Message;
  friend struct File_Stream;
  friend struct File_DataBlock;
  friend struct FileHeader;
  friend struct Float64;
  friend struct ConnectionHeader_Int32;
  friend struct Int32;
  friend struct ConnectionHeader_Int64;
  friend struct ConnectionHeader_Float64;
  friend struct Int64;

 protected:
  char* block;
  size_t allocated;
  std::vector<char> vec;

  static const hel headerSizes[];
  static const hel staticSizes[];

  SerializedData(size_t allocate);
  void requireSize(size_t size);
  hel headerSize(rid id) const;
};

template <bool, class T = void>
struct enable_if {};
template <class T>
struct enable_if<true, T> {
  typedef T type;
};

template <typename T>
struct CheckIfField {
  template <typename C>
  static char test(typename C::isField);
  template <typename C>
  static long test(...);

  enum { value = sizeof(test<T>(0)) == sizeof(char) };
};

template <typename T>
struct CheckIfAny {
  template <typename C>
  static char test(typename C::isAny);
  template <typename C>
  static long test(...);

  enum { value = sizeof(test<T>(0)) == sizeof(char) };
};

template <typename T>
struct CheckIfVector {
  template <typename C>
  static char test(typename C::isVector);
  template <typename C>
  static long test(...);

  enum { value = sizeof(test<T>(0)) == sizeof(char) };
};

template <typename T>
struct CheckIfAnyVector {
  template <typename C>
  static char test(typename C::isAnyVector);
  template <typename C>
  static long test(...);

  enum { value = sizeof(test<T>(0)) == sizeof(char) };
};

template <typename T>
struct CheckIfVoid {
  template <typename C>
  static char test(typename C::isVoid);
  template <typename C>
  static long test(...);

  enum { value = sizeof(test<T>(0)) == sizeof(char) };
};

template <typename Field, typename Next>
class ValueSetter<Field,
                  Next,
                  typename enable_if<CheckIfField<Field>::value>::type> {
  template <typename _Field, typename _Next, typename _Dummy>
  friend class ValueSetter;

 public:
  typedef Field F;
  typedef Next N;

  ValueSetter<typename Next::F, typename Next::N> set(
      const typename Field::T value) {
    *(reinterpret_cast<typename Field::T*>(
        constructor->staticData(Field::staticSize))) = byte_order_to_le(value);

    return ValueSetter<typename Next::F, typename Next::N>(constructor);
  }

  ValueSetter(RecordConstructor* constructor) : constructor(constructor) {}

 protected:
  RecordConstructor* constructor;
};

template <typename Field, typename Next>
class ValueSetter<Field,
                  Next,
                  typename enable_if<CheckIfAny<Field>::value>::type> {
  template <typename _Field, typename _Next, typename _Dummy>
  friend class ValueSetter;

 public:
  typedef Field F;
  typedef Next N;

  template <typename Record>
  typename Record::template constructor<ValueSetter<__Finish, Next> >::type
  begin() {
    return typename Record::template constructor<ValueSetter<__Finish, Next> >::
        type(constructor->beginNested(Record::ID, Record::staticSize,
                                      Record::headerSize, true));
  }

  template <typename Record>
  typename Record::recursive beginRecursive() {
    return typename Record::recursive(constructor->beginNested(
        Record::ID, Record::staticSize, Record::headerSize, true));
  }

  ValueSetter<typename Next::F, typename Next::N> set(
      const SerializedData* data) {
    size_t size = data->size();
    char* to = constructor->dynamicData(size);

    const char* begin = data->data();
    const char* end = begin + size;

    std::copy(begin, end, to);

    constructor->finishDynamicSize();
    return ValueSetter<typename Next::F, typename Next::N>(constructor);
  }

  ValueSetter<typename Next::F, typename Next::N> skip() {
    constructor->finishDynamicSize();
    return ValueSetter<typename Next::F, typename Next::N>(constructor);
  }

  ValueSetter(RecordConstructor* constructor) : constructor(constructor) {}

 protected:
  RecordConstructor* constructor;
};

template <typename Field, typename Next>
class ValueSetter<Field,
                  Next,
                  typename enable_if<CheckIfVector<Field>::value>::type> {
  template <typename _Field, typename _Next, typename _Dummy>
  friend class ValueSetter;

 public:
  typedef Field F;
  typedef Next N;

  ValueSetter<typename Next::F, typename Next::N> finish() {
    constructor->finishDynamicSize();
    return ValueSetter<typename Next::F, typename Next::N>(constructor);
  }

  ValueSetter<Field, Next> add(const typename Field::Tv value) {
    *reinterpret_cast<typename Field::Tv*>(
        constructor->dynamicData(sizeof(value))) = byte_order_to_le(value);

    return ValueSetter<Field, Next>(constructor);
  }

  template <typename VectorType>
  ValueSetter<Field, Next> addVector(VectorType value) {
    vectorConvert<VectorType> vector(value);

    size_t reqiredSize =
        (vector.end() - vector.begin()) * sizeof(*vector.begin());

    typename Field::Tv* offset = reinterpret_cast<typename Field::Tv*>(
        constructor->dynamicData(reqiredSize));

    for (typename vectorConvert<VectorType>::IteratorType i = vector.begin();
         i < vector.end(); ++i, ++offset) {
      *offset = byte_order_to_le(*i);
    }

    return ValueSetter<Field, Next>(constructor);
  }

  template <typename VectorType>
  ValueSetter<typename Next::F, typename Next::N> set(VectorType value) {
    return addVector(value).finish();
  }

  ValueSetter(RecordConstructor* constructor) : constructor(constructor) {}

 protected:
  RecordConstructor* constructor;
};

template <typename Field, typename Next>
class ValueSetter<Field,
                  Next,
                  typename enable_if<CheckIfAnyVector<Field>::value>::type> {
  template <typename _Field, typename _Next, typename _Dummy>
  friend class ValueSetter;

 public:
  typedef Field F;
  typedef Next N;

  template <typename Record>
  typename Record::template constructor<
      ValueSetter<__Finish, ValueSetter<Field, Next> > >::type
  begin() {
    return typename Record::template constructor<
        ValueSetter<__Finish, ValueSetter<Field, Next> > >::
        type(constructor->beginNested(Record::ID, Record::staticSize,
                                      Record::headerSize, false));
  }

  template <typename Record>
  typename Record::recursive beginRecursive() {
    return typename Record::recursive(constructor->beginNested(
        Record::ID, Record::staticSize, Record::headerSize, false));
  }

  ValueSetter<typename Next::F, typename Next::N> finish() {
    constructor->finishDynamicSize();
    return ValueSetter<typename Next::F, typename Next::N>(constructor);
  }

  ValueSetter(RecordConstructor* constructor) : constructor(constructor) {}

 protected:
  RecordConstructor* constructor;
};

template <typename Field, typename Next>
class ValueSetter<Field,
                  Next,
                  typename enable_if<CheckIfVoid<Field>::value>::type> {
  template <typename _Field, typename _Next, typename _Dummy>
  friend class ValueSetter;

 public:
  typedef Field F;
  typedef Next N;

  ValueSetter<typename Next::F, typename Next::N> finish() {
    return ValueSetter<typename Next::F, typename Next::N>(constructor);
  }

  ValueSetter(RecordConstructor* constructor) : constructor(constructor) {}

 protected:
  RecordConstructor* constructor;
};

template <typename Next>
class ValueSetter<__Last, Next> {
  template <typename _Field, typename _Next, typename _Dummy>
  friend class ValueSetter;

 public:
  typedef __Last F;
  typedef SerializedData* N;

  SerializedData* finish() { return result; }

  ValueSetter(RecordConstructor* constructor) {
    result = constructor->root();
    delete constructor;
  }

 protected:
  SerializedData* result;
};

template <typename Next>
class ValueSetter<__LastRecursive, Next> {
  template <typename _Field, typename _Next, typename _Dummy>
  friend class ValueSetter;

 public:
  typedef __LastRecursive F;
  typedef SerializedData* N;

  SerializedData* finish() { return result; }

  SerializedData* clear() {
    delete constructor;
    constructor = 0;
    return result;
  }

  ValueSetter(RecordConstructor* constructor) : constructor(constructor) {
    result = constructor->root();
  }

 protected:
  RecordConstructor* constructor;
  SerializedData* result;
};

template <typename Next>
class ValueSetter<__Finish, Next> {
  template <typename _Field, typename _Next, typename _Dummy>
  friend class ValueSetter;

 public:
  typedef __Finish F;
  typedef Next N;

  ValueSetter<typename Next::F, typename Next::N> finish() {
    return ValueSetter<typename Next::F, typename Next::N>(
        constructor->finishNested());
  }

  ValueSetter(RecordConstructor* constructor) : constructor(constructor) {}

 protected:
  RecordConstructor* constructor;
};
}
}

namespace Resonance {
namespace R3 {

SerializedData::SerializedData(size_t allocate)
    : block(0), allocated(allocate), vec(allocate) {}

SerializedData::SerializedData(const char* block, size_t allocate)
    : block(const_cast<char*>(block)), allocated(allocate), vec() {}

void SerializedData::requireSize(size_t size) {
  if (block)
    throw SerializedException();
  vec.resize(size);
  allocated = size;
}

inline SerializedData::hel SerializedData::headerSize(rid id) const {
  if (id < 1 || id > __LastType)
    throw WrongType();
  return headerSizes[id];
}

QByteArray SerializedData::dataAsByteArray() {
  return QByteArray(data(), size());
}

std::size_t SerializedData::size() const {
  if (id() == 0)
    return 0;
  std::size_t sz =
      sizeof(rid) + headerSize(id()) * sizeof(hel) + staticSizes[id()];
  for (int i = headerSize(id()) - 1; i >= 0; --i) {
    sz += reinterpret_cast<const hel*>(data() + sizeof(rid))[i];
  }
  return sz;
}

SerializedData::SerializedData(reader readdata, void* opt)
    : block(0), allocated(sizeof(rid)), vec(allocated) {
  readdata(data(), sizeof(rid), opt);

  rid id = byte_order_from_le(*reinterpret_cast<rid*>(data()));

  size_t hs = headerSize(id);

  allocated += hs * sizeof(hel);
  requireSize(allocated);

  hel* header = reinterpret_cast<hel*>(data() + sizeof(rid));

  readdata((char*)header, sizeof(hel) * hs, opt);
  std::size_t bulkSize = staticSizes[id];
  for (unsigned int i = 0; i < hs; ++i) {
    bulkSize += byte_order_from_le(header[i]);
  }

  allocated += bulkSize;
  requireSize(allocated);

  readdata(data() + sizeof(rid) + sizeof(hel) * hs, bulkSize, opt);
}
SerializedData* result;

RecordConstructor::RecordConstructor(SerializedData* data,
                                     size_t staticSize,
                                     size_t headerSize)
    : data(data),
      dynamicOffset(sizeof(SerializedData::rid) +
                    sizeof(SerializedData::hel) * headerSize +
                    staticSize) {
  staticOffset.push_back(sizeof(SerializedData::rid) +
                         sizeof(SerializedData::hel) * headerSize);
  dynamicSizeOffset.push_back(sizeof(SerializedData::rid));
  finishDynamic.push_back(headerSize > 0);
}

char* RecordConstructor::staticData(size_t size) {
  char* ptr = data->data() + staticOffset.back();
  staticOffset.back() += size;
  return ptr;
}

char* RecordConstructor::dynamicData(size_t size) {
  data->requireSize(dynamicOffset + size);
  char* ptr = data->data() + dynamicOffset;
  dynamicOffset += size;

  for (std::vector<size_t>::iterator i = dynamicSizeOffset.begin();
       i != dynamicSizeOffset.end(); ++i) {
    *reinterpret_cast<SerializedData::hel*>(data->data() + *i) += size;
  }

  return ptr;
}

void RecordConstructor::finishDynamicSize() {
  *reinterpret_cast<SerializedData::hel*>(data->data() +
                                          dynamicSizeOffset.back()) =
      byte_order_to_le(*reinterpret_cast<SerializedData::hel*>(
          data->data() + dynamicSizeOffset.back()));
  dynamicSizeOffset.back() += sizeof(SerializedData::hel);
}

SerializedData* RecordConstructor::root() {
  finishNested();
  return data;
}

RecordConstructor* RecordConstructor::beginNested(u16 id,
                                                  size_t staticSize,
                                                  size_t headerSize,
                                                  bool last) {
  size_t nestedSize =
      sizeof(id) + staticSize + headerSize * sizeof(SerializedData::hel);
  char* obj = dynamicData(nestedSize);
  *reinterpret_cast<u16*>(obj) = byte_order_to_le(id);

  SerializedData::hel* dyn =
      reinterpret_cast<SerializedData::hel*>(obj + sizeof(SerializedData::rid));
  for (size_t j = headerSize; j > 0; --j, ++dyn)
    *dyn = 0;

  size_t hOffset = sizeof(id) + (obj - data->data());
  size_t sOffset = hOffset + headerSize * sizeof(SerializedData::hel);

  staticOffset.push_back(sOffset);
  dynamicSizeOffset.push_back(hOffset);
  finishDynamic.push_back(last && headerSize > 0);

  return this;
}

RecordConstructor* RecordConstructor::finishNested() {
  if (finishDynamic.size() == 0)
    return this;
  if (finishDynamic.back())
    finishDynamicSize();

  staticOffset.pop_back();
  dynamicSizeOffset.pop_back();
  finishDynamic.pop_back();

  return this;
}

JSON_Object::constructor<ValueSetter<__Last, __Last> >::type
JSON_Object::create() {
  SerializedData* sd =
      new SerializedData(sizeof(SerializedData::rid) +
                         JSON_Object::headerSize * sizeof(SerializedData::hel) +
                         JSON_Object::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)JSON_Object::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = JSON_Object::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return JSON_Object::constructor<ValueSetter<__Last, __Last> >::type(
      new RecordConstructor(sd, JSON_Object::staticSize,
                            JSON_Object::headerSize));
}
JSON_Object::recursive JSON_Object::createRecursive() {
  SerializedData* sd =
      new SerializedData(sizeof(SerializedData::rid) +
                         JSON_Object::headerSize * sizeof(SerializedData::hel) +
                         JSON_Object::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)JSON_Object::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = JSON_Object::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return JSON_Object::recursive(new RecordConstructor(
      sd, JSON_Object::staticSize, JSON_Object::headerSize));
}
JSON_Array::constructor<ValueSetter<__Last, __Last> >::type
JSON_Array::create() {
  SerializedData* sd =
      new SerializedData(sizeof(SerializedData::rid) +
                         JSON_Array::headerSize * sizeof(SerializedData::hel) +
                         JSON_Array::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)JSON_Array::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = JSON_Array::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return JSON_Array::constructor<ValueSetter<__Last, __Last> >::type(
      new RecordConstructor(sd, JSON_Array::staticSize,
                            JSON_Array::headerSize));
}
JSON_Array::recursive JSON_Array::createRecursive() {
  SerializedData* sd =
      new SerializedData(sizeof(SerializedData::rid) +
                         JSON_Array::headerSize * sizeof(SerializedData::hel) +
                         JSON_Array::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)JSON_Array::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = JSON_Array::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return JSON_Array::recursive(new RecordConstructor(sd, JSON_Array::staticSize,
                                                     JSON_Array::headerSize));
}
JSON_Null::constructor<ValueSetter<__Last, __Last> >::type JSON_Null::create() {
  SerializedData* sd =
      new SerializedData(sizeof(SerializedData::rid) +
                         JSON_Null::headerSize * sizeof(SerializedData::hel) +
                         JSON_Null::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)JSON_Null::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = JSON_Null::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return JSON_Null::constructor<ValueSetter<__Last, __Last> >::type(
      new RecordConstructor(sd, JSON_Null::staticSize, JSON_Null::headerSize));
}
JSON_Null::recursive JSON_Null::createRecursive() {
  SerializedData* sd =
      new SerializedData(sizeof(SerializedData::rid) +
                         JSON_Null::headerSize * sizeof(SerializedData::hel) +
                         JSON_Null::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)JSON_Null::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = JSON_Null::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return JSON_Null::recursive(
      new RecordConstructor(sd, JSON_Null::staticSize, JSON_Null::headerSize));
}
JSON_Number::constructor<ValueSetter<__Last, __Last> >::type
JSON_Number::create() {
  SerializedData* sd =
      new SerializedData(sizeof(SerializedData::rid) +
                         JSON_Number::headerSize * sizeof(SerializedData::hel) +
                         JSON_Number::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)JSON_Number::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = JSON_Number::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return JSON_Number::constructor<ValueSetter<__Last, __Last> >::type(
      new RecordConstructor(sd, JSON_Number::staticSize,
                            JSON_Number::headerSize));
}
JSON_Number::recursive JSON_Number::createRecursive() {
  SerializedData* sd =
      new SerializedData(sizeof(SerializedData::rid) +
                         JSON_Number::headerSize * sizeof(SerializedData::hel) +
                         JSON_Number::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)JSON_Number::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = JSON_Number::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return JSON_Number::recursive(new RecordConstructor(
      sd, JSON_Number::staticSize, JSON_Number::headerSize));
}
JSON_Bool::constructor<ValueSetter<__Last, __Last> >::type JSON_Bool::create() {
  SerializedData* sd =
      new SerializedData(sizeof(SerializedData::rid) +
                         JSON_Bool::headerSize * sizeof(SerializedData::hel) +
                         JSON_Bool::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)JSON_Bool::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = JSON_Bool::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return JSON_Bool::constructor<ValueSetter<__Last, __Last> >::type(
      new RecordConstructor(sd, JSON_Bool::staticSize, JSON_Bool::headerSize));
}
JSON_Bool::recursive JSON_Bool::createRecursive() {
  SerializedData* sd =
      new SerializedData(sizeof(SerializedData::rid) +
                         JSON_Bool::headerSize * sizeof(SerializedData::hel) +
                         JSON_Bool::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)JSON_Bool::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = JSON_Bool::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return JSON_Bool::recursive(
      new RecordConstructor(sd, JSON_Bool::staticSize, JSON_Bool::headerSize));
}
JSON_String::constructor<ValueSetter<__Last, __Last> >::type
JSON_String::create() {
  SerializedData* sd =
      new SerializedData(sizeof(SerializedData::rid) +
                         JSON_String::headerSize * sizeof(SerializedData::hel) +
                         JSON_String::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)JSON_String::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = JSON_String::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return JSON_String::constructor<ValueSetter<__Last, __Last> >::type(
      new RecordConstructor(sd, JSON_String::staticSize,
                            JSON_String::headerSize));
}
JSON_String::recursive JSON_String::createRecursive() {
  SerializedData* sd =
      new SerializedData(sizeof(SerializedData::rid) +
                         JSON_String::headerSize * sizeof(SerializedData::hel) +
                         JSON_String::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)JSON_String::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = JSON_String::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return JSON_String::recursive(new RecordConstructor(
      sd, JSON_String::staticSize, JSON_String::headerSize));
}
ConnectionHeader_EventBus::constructor<ValueSetter<__Last, __Last> >::type
ConnectionHeader_EventBus::create() {
  SerializedData* sd = new SerializedData(
      sizeof(SerializedData::rid) +
      ConnectionHeader_EventBus::headerSize * sizeof(SerializedData::hel) +
      ConnectionHeader_EventBus::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)ConnectionHeader_EventBus::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = ConnectionHeader_EventBus::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return ConnectionHeader_EventBus::constructor<ValueSetter<__Last, __Last> >::
      type(new RecordConstructor(sd, ConnectionHeader_EventBus::staticSize,
                                 ConnectionHeader_EventBus::headerSize));
}
ConnectionHeader_EventBus::recursive
ConnectionHeader_EventBus::createRecursive() {
  SerializedData* sd = new SerializedData(
      sizeof(SerializedData::rid) +
      ConnectionHeader_EventBus::headerSize * sizeof(SerializedData::hel) +
      ConnectionHeader_EventBus::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)ConnectionHeader_EventBus::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = ConnectionHeader_EventBus::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return ConnectionHeader_EventBus::recursive(
      new RecordConstructor(sd, ConnectionHeader_EventBus::staticSize,
                            ConnectionHeader_EventBus::headerSize));
}
EventBus_Description::constructor<ValueSetter<__Last, __Last> >::type
EventBus_Description::create() {
  SerializedData* sd = new SerializedData(sizeof(SerializedData::rid) +
                                          EventBus_Description::headerSize *
                                              sizeof(SerializedData::hel) +
                                          EventBus_Description::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)EventBus_Description::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = EventBus_Description::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return EventBus_Description::constructor<ValueSetter<__Last, __Last> >::type(
      new RecordConstructor(sd, EventBus_Description::staticSize,
                            EventBus_Description::headerSize));
}
EventBus_Description::recursive EventBus_Description::createRecursive() {
  SerializedData* sd = new SerializedData(sizeof(SerializedData::rid) +
                                          EventBus_Description::headerSize *
                                              sizeof(SerializedData::hel) +
                                          EventBus_Description::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)EventBus_Description::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = EventBus_Description::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return EventBus_Description::recursive(new RecordConstructor(
      sd, EventBus_Description::staticSize, EventBus_Description::headerSize));
}
EventBus_Description_Stream::constructor<ValueSetter<__Last, __Last> >::type
EventBus_Description_Stream::create() {
  SerializedData* sd = new SerializedData(
      sizeof(SerializedData::rid) +
      EventBus_Description_Stream::headerSize * sizeof(SerializedData::hel) +
      EventBus_Description_Stream::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)EventBus_Description_Stream::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = EventBus_Description_Stream::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return EventBus_Description_Stream::
      constructor<ValueSetter<__Last, __Last> >::type(
          new RecordConstructor(sd, EventBus_Description_Stream::staticSize,
                                EventBus_Description_Stream::headerSize));
}
EventBus_Description_Stream::recursive
EventBus_Description_Stream::createRecursive() {
  SerializedData* sd = new SerializedData(
      sizeof(SerializedData::rid) +
      EventBus_Description_Stream::headerSize * sizeof(SerializedData::hel) +
      EventBus_Description_Stream::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)EventBus_Description_Stream::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = EventBus_Description_Stream::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return EventBus_Description_Stream::recursive(
      new RecordConstructor(sd, EventBus_Description_Stream::staticSize,
                            EventBus_Description_Stream::headerSize));
}
EventBus_Description_Url::constructor<ValueSetter<__Last, __Last> >::type
EventBus_Description_Url::create() {
  SerializedData* sd = new SerializedData(sizeof(SerializedData::rid) +
                                          EventBus_Description_Url::headerSize *
                                              sizeof(SerializedData::hel) +
                                          EventBus_Description_Url::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)EventBus_Description_Url::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = EventBus_Description_Url::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return EventBus_Description_Url::constructor<ValueSetter<__Last, __Last> >::
      type(new RecordConstructor(sd, EventBus_Description_Url::staticSize,
                                 EventBus_Description_Url::headerSize));
}
EventBus_Description_Url::recursive
EventBus_Description_Url::createRecursive() {
  SerializedData* sd = new SerializedData(sizeof(SerializedData::rid) +
                                          EventBus_Description_Url::headerSize *
                                              sizeof(SerializedData::hel) +
                                          EventBus_Description_Url::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)EventBus_Description_Url::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = EventBus_Description_Url::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return EventBus_Description_Url::recursive(
      new RecordConstructor(sd, EventBus_Description_Url::staticSize,
                            EventBus_Description_Url::headerSize));
}
EventBus_DescribeRequest::constructor<ValueSetter<__Last, __Last> >::type
EventBus_DescribeRequest::create() {
  SerializedData* sd = new SerializedData(sizeof(SerializedData::rid) +
                                          EventBus_DescribeRequest::headerSize *
                                              sizeof(SerializedData::hel) +
                                          EventBus_DescribeRequest::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)EventBus_DescribeRequest::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = EventBus_DescribeRequest::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return EventBus_DescribeRequest::constructor<ValueSetter<__Last, __Last> >::
      type(new RecordConstructor(sd, EventBus_DescribeRequest::staticSize,
                                 EventBus_DescribeRequest::headerSize));
}
EventBus_DescribeRequest::recursive
EventBus_DescribeRequest::createRecursive() {
  SerializedData* sd = new SerializedData(sizeof(SerializedData::rid) +
                                          EventBus_DescribeRequest::headerSize *
                                              sizeof(SerializedData::hel) +
                                          EventBus_DescribeRequest::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)EventBus_DescribeRequest::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = EventBus_DescribeRequest::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return EventBus_DescribeRequest::recursive(
      new RecordConstructor(sd, EventBus_DescribeRequest::staticSize,
                            EventBus_DescribeRequest::headerSize));
}
EventBus_ApplyParameters::constructor<ValueSetter<__Last, __Last> >::type
EventBus_ApplyParameters::create() {
  SerializedData* sd = new SerializedData(sizeof(SerializedData::rid) +
                                          EventBus_ApplyParameters::headerSize *
                                              sizeof(SerializedData::hel) +
                                          EventBus_ApplyParameters::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)EventBus_ApplyParameters::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = EventBus_ApplyParameters::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return EventBus_ApplyParameters::constructor<ValueSetter<__Last, __Last> >::
      type(new RecordConstructor(sd, EventBus_ApplyParameters::staticSize,
                                 EventBus_ApplyParameters::headerSize));
}
EventBus_ApplyParameters::recursive
EventBus_ApplyParameters::createRecursive() {
  SerializedData* sd = new SerializedData(sizeof(SerializedData::rid) +
                                          EventBus_ApplyParameters::headerSize *
                                              sizeof(SerializedData::hel) +
                                          EventBus_ApplyParameters::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)EventBus_ApplyParameters::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = EventBus_ApplyParameters::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return EventBus_ApplyParameters::recursive(
      new RecordConstructor(sd, EventBus_ApplyParameters::staticSize,
                            EventBus_ApplyParameters::headerSize));
}
EventBus_Transition::constructor<ValueSetter<__Last, __Last> >::type
EventBus_Transition::create() {
  SerializedData* sd = new SerializedData(sizeof(SerializedData::rid) +
                                          EventBus_Transition::headerSize *
                                              sizeof(SerializedData::hel) +
                                          EventBus_Transition::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)EventBus_Transition::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = EventBus_Transition::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return EventBus_Transition::constructor<ValueSetter<__Last, __Last> >::type(
      new RecordConstructor(sd, EventBus_Transition::staticSize,
                            EventBus_Transition::headerSize));
}
EventBus_Transition::recursive EventBus_Transition::createRecursive() {
  SerializedData* sd = new SerializedData(sizeof(SerializedData::rid) +
                                          EventBus_Transition::headerSize *
                                              sizeof(SerializedData::hel) +
                                          EventBus_Transition::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)EventBus_Transition::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = EventBus_Transition::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return EventBus_Transition::recursive(new RecordConstructor(
      sd, EventBus_Transition::staticSize, EventBus_Transition::headerSize));
}
ConnectionHeader_Message::constructor<ValueSetter<__Last, __Last> >::type
ConnectionHeader_Message::create() {
  SerializedData* sd = new SerializedData(sizeof(SerializedData::rid) +
                                          ConnectionHeader_Message::headerSize *
                                              sizeof(SerializedData::hel) +
                                          ConnectionHeader_Message::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)ConnectionHeader_Message::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = ConnectionHeader_Message::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return ConnectionHeader_Message::constructor<ValueSetter<__Last, __Last> >::
      type(new RecordConstructor(sd, ConnectionHeader_Message::staticSize,
                                 ConnectionHeader_Message::headerSize));
}
ConnectionHeader_Message::recursive
ConnectionHeader_Message::createRecursive() {
  SerializedData* sd = new SerializedData(sizeof(SerializedData::rid) +
                                          ConnectionHeader_Message::headerSize *
                                              sizeof(SerializedData::hel) +
                                          ConnectionHeader_Message::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)ConnectionHeader_Message::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = ConnectionHeader_Message::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return ConnectionHeader_Message::recursive(
      new RecordConstructor(sd, ConnectionHeader_Message::staticSize,
                            ConnectionHeader_Message::headerSize));
}
Message::constructor<ValueSetter<__Last, __Last> >::type Message::create() {
  SerializedData* sd = new SerializedData(
      sizeof(SerializedData::rid) +
      Message::headerSize * sizeof(SerializedData::hel) + Message::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)Message::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = Message::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return Message::constructor<ValueSetter<__Last, __Last> >::type(
      new RecordConstructor(sd, Message::staticSize, Message::headerSize));
}
Message::recursive Message::createRecursive() {
  SerializedData* sd = new SerializedData(
      sizeof(SerializedData::rid) +
      Message::headerSize * sizeof(SerializedData::hel) + Message::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)Message::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = Message::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return Message::recursive(
      new RecordConstructor(sd, Message::staticSize, Message::headerSize));
}
File_Stream::constructor<ValueSetter<__Last, __Last> >::type
File_Stream::create() {
  SerializedData* sd =
      new SerializedData(sizeof(SerializedData::rid) +
                         File_Stream::headerSize * sizeof(SerializedData::hel) +
                         File_Stream::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)File_Stream::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = File_Stream::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return File_Stream::constructor<ValueSetter<__Last, __Last> >::type(
      new RecordConstructor(sd, File_Stream::staticSize,
                            File_Stream::headerSize));
}
File_Stream::recursive File_Stream::createRecursive() {
  SerializedData* sd =
      new SerializedData(sizeof(SerializedData::rid) +
                         File_Stream::headerSize * sizeof(SerializedData::hel) +
                         File_Stream::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)File_Stream::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = File_Stream::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return File_Stream::recursive(new RecordConstructor(
      sd, File_Stream::staticSize, File_Stream::headerSize));
}
File_DataBlock::constructor<ValueSetter<__Last, __Last> >::type
File_DataBlock::create() {
  SerializedData* sd = new SerializedData(sizeof(SerializedData::rid) +
                                          File_DataBlock::headerSize *
                                              sizeof(SerializedData::hel) +
                                          File_DataBlock::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)File_DataBlock::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = File_DataBlock::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return File_DataBlock::constructor<ValueSetter<__Last, __Last> >::type(
      new RecordConstructor(sd, File_DataBlock::staticSize,
                            File_DataBlock::headerSize));
}
File_DataBlock::recursive File_DataBlock::createRecursive() {
  SerializedData* sd = new SerializedData(sizeof(SerializedData::rid) +
                                          File_DataBlock::headerSize *
                                              sizeof(SerializedData::hel) +
                                          File_DataBlock::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)File_DataBlock::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = File_DataBlock::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return File_DataBlock::recursive(new RecordConstructor(
      sd, File_DataBlock::staticSize, File_DataBlock::headerSize));
}
FileHeader::constructor<ValueSetter<__Last, __Last> >::type
FileHeader::create() {
  SerializedData* sd =
      new SerializedData(sizeof(SerializedData::rid) +
                         FileHeader::headerSize * sizeof(SerializedData::hel) +
                         FileHeader::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)FileHeader::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = FileHeader::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return FileHeader::constructor<ValueSetter<__Last, __Last> >::type(
      new RecordConstructor(sd, FileHeader::staticSize,
                            FileHeader::headerSize));
}
FileHeader::recursive FileHeader::createRecursive() {
  SerializedData* sd =
      new SerializedData(sizeof(SerializedData::rid) +
                         FileHeader::headerSize * sizeof(SerializedData::hel) +
                         FileHeader::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)FileHeader::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = FileHeader::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return FileHeader::recursive(new RecordConstructor(sd, FileHeader::staticSize,
                                                     FileHeader::headerSize));
}
Float64::constructor<ValueSetter<__Last, __Last> >::type Float64::create() {
  SerializedData* sd = new SerializedData(
      sizeof(SerializedData::rid) +
      Float64::headerSize * sizeof(SerializedData::hel) + Float64::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)Float64::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = Float64::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return Float64::constructor<ValueSetter<__Last, __Last> >::type(
      new RecordConstructor(sd, Float64::staticSize, Float64::headerSize));
}
Float64::recursive Float64::createRecursive() {
  SerializedData* sd = new SerializedData(
      sizeof(SerializedData::rid) +
      Float64::headerSize * sizeof(SerializedData::hel) + Float64::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)Float64::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = Float64::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return Float64::recursive(
      new RecordConstructor(sd, Float64::staticSize, Float64::headerSize));
}
ConnectionHeader_Int32::constructor<ValueSetter<__Last, __Last> >::type
ConnectionHeader_Int32::create() {
  SerializedData* sd = new SerializedData(sizeof(SerializedData::rid) +
                                          ConnectionHeader_Int32::headerSize *
                                              sizeof(SerializedData::hel) +
                                          ConnectionHeader_Int32::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)ConnectionHeader_Int32::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = ConnectionHeader_Int32::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return ConnectionHeader_Int32::constructor<ValueSetter<__Last, __Last> >::
      type(new RecordConstructor(sd, ConnectionHeader_Int32::staticSize,
                                 ConnectionHeader_Int32::headerSize));
}
ConnectionHeader_Int32::recursive ConnectionHeader_Int32::createRecursive() {
  SerializedData* sd = new SerializedData(sizeof(SerializedData::rid) +
                                          ConnectionHeader_Int32::headerSize *
                                              sizeof(SerializedData::hel) +
                                          ConnectionHeader_Int32::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)ConnectionHeader_Int32::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = ConnectionHeader_Int32::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return ConnectionHeader_Int32::recursive(
      new RecordConstructor(sd, ConnectionHeader_Int32::staticSize,
                            ConnectionHeader_Int32::headerSize));
}
Int32::constructor<ValueSetter<__Last, __Last> >::type Int32::create() {
  SerializedData* sd = new SerializedData(
      sizeof(SerializedData::rid) +
      Int32::headerSize * sizeof(SerializedData::hel) + Int32::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)Int32::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = Int32::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return Int32::constructor<ValueSetter<__Last, __Last> >::type(
      new RecordConstructor(sd, Int32::staticSize, Int32::headerSize));
}
Int32::recursive Int32::createRecursive() {
  SerializedData* sd = new SerializedData(
      sizeof(SerializedData::rid) +
      Int32::headerSize * sizeof(SerializedData::hel) + Int32::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)Int32::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = Int32::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return Int32::recursive(
      new RecordConstructor(sd, Int32::staticSize, Int32::headerSize));
}
ConnectionHeader_Int64::constructor<ValueSetter<__Last, __Last> >::type
ConnectionHeader_Int64::create() {
  SerializedData* sd = new SerializedData(sizeof(SerializedData::rid) +
                                          ConnectionHeader_Int64::headerSize *
                                              sizeof(SerializedData::hel) +
                                          ConnectionHeader_Int64::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)ConnectionHeader_Int64::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = ConnectionHeader_Int64::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return ConnectionHeader_Int64::constructor<ValueSetter<__Last, __Last> >::
      type(new RecordConstructor(sd, ConnectionHeader_Int64::staticSize,
                                 ConnectionHeader_Int64::headerSize));
}
ConnectionHeader_Int64::recursive ConnectionHeader_Int64::createRecursive() {
  SerializedData* sd = new SerializedData(sizeof(SerializedData::rid) +
                                          ConnectionHeader_Int64::headerSize *
                                              sizeof(SerializedData::hel) +
                                          ConnectionHeader_Int64::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)ConnectionHeader_Int64::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = ConnectionHeader_Int64::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return ConnectionHeader_Int64::recursive(
      new RecordConstructor(sd, ConnectionHeader_Int64::staticSize,
                            ConnectionHeader_Int64::headerSize));
}
ConnectionHeader_Float64::constructor<ValueSetter<__Last, __Last> >::type
ConnectionHeader_Float64::create() {
  SerializedData* sd = new SerializedData(sizeof(SerializedData::rid) +
                                          ConnectionHeader_Float64::headerSize *
                                              sizeof(SerializedData::hel) +
                                          ConnectionHeader_Float64::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)ConnectionHeader_Float64::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = ConnectionHeader_Float64::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return ConnectionHeader_Float64::constructor<ValueSetter<__Last, __Last> >::
      type(new RecordConstructor(sd, ConnectionHeader_Float64::staticSize,
                                 ConnectionHeader_Float64::headerSize));
}
ConnectionHeader_Float64::recursive
ConnectionHeader_Float64::createRecursive() {
  SerializedData* sd = new SerializedData(sizeof(SerializedData::rid) +
                                          ConnectionHeader_Float64::headerSize *
                                              sizeof(SerializedData::hel) +
                                          ConnectionHeader_Float64::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)ConnectionHeader_Float64::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = ConnectionHeader_Float64::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return ConnectionHeader_Float64::recursive(
      new RecordConstructor(sd, ConnectionHeader_Float64::staticSize,
                            ConnectionHeader_Float64::headerSize));
}
Int64::constructor<ValueSetter<__Last, __Last> >::type Int64::create() {
  SerializedData* sd = new SerializedData(
      sizeof(SerializedData::rid) +
      Int64::headerSize * sizeof(SerializedData::hel) + Int64::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)Int64::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = Int64::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return Int64::constructor<ValueSetter<__Last, __Last> >::type(
      new RecordConstructor(sd, Int64::staticSize, Int64::headerSize));
}
Int64::recursive Int64::createRecursive() {
  SerializedData* sd = new SerializedData(
      sizeof(SerializedData::rid) +
      Int64::headerSize * sizeof(SerializedData::hel) + Int64::staticSize);
  *reinterpret_cast<SerializedData::rid*>(sd->data()) =
      byte_order_to_le((SerializedData::rid)Int64::ID);
  SerializedData::hel* dyn = reinterpret_cast<SerializedData::hel*>(
      sd->data() + sizeof(SerializedData::rid));
  for (size_t j = Int64::headerSize; j > 0; --j, ++dyn)
    *dyn = 0;
  return Int64::recursive(
      new RecordConstructor(sd, Int64::staticSize, Int64::headerSize));
}
const SerializedData::hel SerializedData::headerSizes[] = {
    0,
    JSON_Object::headerSize,
    JSON_Array::headerSize,
    JSON_Null::headerSize,
    JSON_Number::headerSize,
    JSON_Bool::headerSize,
    JSON_String::headerSize,
    ConnectionHeader_EventBus::headerSize,
    EventBus_Description::headerSize,
    EventBus_Description_Stream::headerSize,
    EventBus_Description_Url::headerSize,
    EventBus_DescribeRequest::headerSize,
    EventBus_ApplyParameters::headerSize,
    EventBus_Transition::headerSize,
    ConnectionHeader_Message::headerSize,
    Message::headerSize,
    File_Stream::headerSize,
    File_DataBlock::headerSize,
    FileHeader::headerSize,
    Float64::headerSize,
    ConnectionHeader_Int32::headerSize,
    Int32::headerSize,
    ConnectionHeader_Int64::headerSize,
    ConnectionHeader_Float64::headerSize,
    Int64::headerSize};
const SerializedData::hel SerializedData::staticSizes[] = {
    0,
    JSON_Object::staticSize,
    JSON_Array::staticSize,
    JSON_Null::staticSize,
    JSON_Number::staticSize,
    JSON_Bool::staticSize,
    JSON_String::staticSize,
    ConnectionHeader_EventBus::staticSize,
    EventBus_Description::staticSize,
    EventBus_Description_Stream::staticSize,
    EventBus_Description_Url::staticSize,
    EventBus_DescribeRequest::staticSize,
    EventBus_ApplyParameters::staticSize,
    EventBus_Transition::staticSize,
    ConnectionHeader_Message::staticSize,
    Message::staticSize,
    File_Stream::staticSize,
    File_DataBlock::staticSize,
    FileHeader::staticSize,
    Float64::staticSize,
    ConnectionHeader_Int32::staticSize,
    Int32::staticSize,
    ConnectionHeader_Int64::staticSize,
    ConnectionHeader_Float64::staticSize,
    Int64::staticSize};
}
}
