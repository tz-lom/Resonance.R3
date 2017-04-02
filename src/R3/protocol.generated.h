

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

    typename Vector::V first = reinterpret_cast<typename Vector::V>(const_cast<char*>(offset));
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
