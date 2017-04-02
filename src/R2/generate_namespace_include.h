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

template<int recordId, typename self> struct Record {

    enum {
        ID = recordId
    };

    template<typename type, typename parent> struct Field{
        typedef type T;

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
        enum {
            staticOffset = calcStaticOffset<parent>::V,
            dynamicOffset = calcDynamicOffset<parent>::V,
            headerSize = 0,
            staticSize = 0
        };

        typedef parent previous;
        typedef self record;
    };
};

struct SerializedException {};
struct WrongType: SerializedException {};


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


