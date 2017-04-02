template<typename T> T byte_order_to_le(T x) { return x; }
template<typename T> T byte_order_from_le(T x) { return x; }

typedef uint64_t u64;
typedef uint32_t u32;
typedef int64_t  i64;
typedef int32_t  i32;
typedef float    f32;
typedef double   f64;
typedef uint16_t u16;
typedef int16_t  i16;
typedef uint8_t  u8;
typedef int8_t   i8;


struct First {};

template< typename field> struct calcStaticOffset{
    enum {
        V = field::staticSize + calcStaticOffset<typename field::previous>::V
    };
};

template<> struct calcStaticOffset<First> {
    enum {
        V = 0
    };
};

template< typename field> struct calcDynamicOffset{
    enum {
        V = field::headerSize + calcDynamicOffset<typename field::previous>::V
    };
};

template<> struct calcDynamicOffset<First> {
    enum {
        V = 0
    };
};

struct __Last{ typedef __Last F; typedef __Last N; };
struct __LastRecursive { typedef __LastRecursive F; typedef __LastRecursive N; };
struct __Finish{ typedef __Finish F; typedef __Finish N; };

class SerializedData;

class RecordConstructor
{
public:
    RecordConstructor(SerializedData *data, size_t staticSize, size_t headerSize);

    char* staticData(size_t size);
    char* dynamicData(size_t size);
    void finishDynamicSize();

    RecordConstructor* beginNested(u16 id, size_t staticSize, size_t headerSize, bool last);
    RecordConstructor* finishNested();

    SerializedData *root();

protected:
    SerializedData *data;

    size_t dynamicOffset;

    std::vector<size_t> staticOffset;
    std::vector<size_t> dynamicSizeOffset;
    std::vector<bool> finishDynamic;
};

template<int recordId, typename self> struct Record {

    enum {
        ID = recordId
    };

    template<typename type, typename parent> struct Field{
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

    template<typename type, typename parent> struct Vector {
        typedef type Tv;
        typedef type *V;
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

    template<typename notused, typename parent> struct Any {
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

    template<typename notused, typename parent> struct Void {
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

    template <typename notused, typename parent> struct AnyVector {
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
struct WrongType: SerializedException {};
struct WrongIndex: SerializedException {};

template<typename Field, typename Next, typename Dummy = void>
class ValueSetter {};
typedef ValueSetter<__LastRecursive, SerializedData*> LastRecursive;

template<typename T> class stringConvert
{
public:
    typedef T IteratorType;

    stringConvert(T &in);

    IteratorType begin();
    IteratorType end();
};

template<typename T> class vectorConvert
{
public:
    typedef T IteratorType;

    vectorConvert(T &in);

    IteratorType begin();
    IteratorType end();
};
/*
#ifdef QT_VERSION

template<>
class stringConvert<QString>
{
public:
    typedef QByteArray::const_iterator IteratorType;

    stringConvert(QString &in):
        ba(in.toUtf8())
    {

    }

    IteratorType begin()
    {
        return ba.constBegin();
    }

    IteratorType end()
    {
        return ba.constEnd();
    }

private:
    QByteArray ba;
};

#endif

template<>
class stringConvert<::std::string>
{
public:
    typedef std::string::const_iterator IteratorType;

    stringConvert(::std::string &in):
        str(in)
    {
    }

    IteratorType begin()
    {
        return str.begin();
    }

    IteratorType end()
    {
        return str.end();
    }
private:
    ::std::string &str;
};


template<>
class stringConvert<const char*>
{
public:
    typedef const char* IteratorType;

    stringConvert(const char* &in):
        str(in)
    {
    }

    IteratorType begin()
    {
        return str;
    }

    IteratorType end()
    {
        return str+strlen(str);
    }

private:
    const char *str;
};*/


template<typename T>
class vectorConvert< ::std::vector<T> >
{
public:
    typedef typename ::std::vector<T>::const_iterator IteratorType;

    vectorConvert(::std::vector<T> &in):
        vec(in)
    {
    }

    IteratorType begin()
    {
        return vec.begin();
    }

    IteratorType end()
    {
        return vec.end();
    }

private:
    ::std::vector<T> &vec;
};

template<typename T>
struct VectorContainer
{
    VectorContainer(T* data, ::std::size_t size):
        data(data),
        size(size)
    {
    }

    T* data;
    ::std::size_t size;
};

template<typename T>
class vectorConvert< VectorContainer<T> >
{
public:
    typedef T* IteratorType;

    vectorConvert( VectorContainer<T> in):
        vec(in)
    {
    }

    IteratorType begin()
    {
        return vec.data;
    }

    IteratorType end()
    {
        return vec.data+vec.size;
    }

private:
    VectorContainer<T> vec;
};

#ifdef QT_VERSION

template<>
class vectorConvert<QString>
{
public:
    typedef ::QByteArray::const_iterator IteratorType;

    vectorConvert(QString &in):
        ba(in.toUtf8())
    {

    }

    IteratorType begin()
    {
        return ba.constBegin();
    }

    IteratorType end()
    {
        return ba.constEnd();
    }

private:
    ::QByteArray ba;
};

template< typename T >
class vectorConvert< ::QVector<T> >
{
public:
    typedef typename ::QVector<T>::const_iterator IteratorType;

    vectorConvert( ::QVector<T> &in):
        vec(in)
    {

    }

    IteratorType begin()
    {
        return vec.constBegin();
    }

    IteratorType end()
    {
        return vec.constEnd();
    }

private:
    ::QVector<T> vec;
};

#endif

template<>
class vectorConvert< ::std::string >
{
public:
    typedef std::string::const_iterator IteratorType;

    vectorConvert(::std::string &in):
        str(in)
    {
    }

    IteratorType begin()
    {
        return str.begin();
    }

    IteratorType end()
    {
        return str.end();
    }
private:
    ::std::string &str;
};


template<>
class vectorConvert<const char*>
{
public:
    typedef const char* IteratorType;

    vectorConvert(const char* &in):
        str(in)
    {
    }

    IteratorType begin()
    {
        return str;
    }

    IteratorType end()
    {
        return str+strlen(str);
    }

private:
    const char *str;
};

template<>
class vectorConvert<char*>
{
public:
    typedef char* IteratorType;

    vectorConvert(char* &in):
        str(in)
    {
    }

    IteratorType begin()
    {
        return str;
    }

    IteratorType end()
    {
        return str+strlen(str);
    }

private:
    char *str;
};


