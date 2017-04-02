protected:
    char *block;
    size_t allocated;
    std::vector<char> vec;

    static const hel headerSizes[];
    static const hel staticSizes[];

    SerializedData(size_t allocate);
    void requireSize(size_t size);
    hel headerSize(rid id) const;
};


template <bool, class T = void> struct enable_if {};
template <class T> struct enable_if<true, T> { typedef T type; };

template<typename T> struct CheckIfField
{
    template <typename C> static char test( typename C::isField );
    template <typename C> static long test(...);

    enum { value = sizeof(test<T>(0))==sizeof(char) };
};

template<typename T> struct CheckIfAny
{
    template <typename C> static char test( typename C::isAny );
    template <typename C> static long test(...);

    enum { value = sizeof(test<T>(0))==sizeof(char) };
};

template<typename T> struct CheckIfVector
{
    template <typename C> static char test( typename C::isVector );
    template <typename C> static long test(...);

    enum { value = sizeof(test<T>(0))==sizeof(char) };
};

template<typename T> struct CheckIfAnyVector
{
    template <typename C> static char test( typename C::isAnyVector );
    template <typename C> static long test(...);

    enum { value = sizeof(test<T>(0))==sizeof(char) };
};

template<typename T> struct CheckIfVoid
{
    template <typename C> static char test( typename C::isVoid );
    template <typename C> static long test(...);

    enum { value = sizeof(test<T>(0))==sizeof(char) };
};

template<typename Field, typename Next>
class ValueSetter<
            Field,
            Next,
            typename enable_if<CheckIfField<Field>::value>::type
        >
{
template<typename _Field, typename _Next, typename _Dummy> friend class ValueSetter;
public:
    typedef Field F;
    typedef Next N;

    ValueSetter<typename Next::F, typename Next::N> set(const typename Field::T value)
    {
        *(reinterpret_cast<typename Field::T *>(constructor->staticData(Field::staticSize))) = byte_order_to_le(value);

        return ValueSetter<typename Next::F, typename Next::N>(constructor);
    }

    ValueSetter(RecordConstructor *constructor):
        constructor(constructor){}
protected:
    RecordConstructor *constructor;
};


template<typename Field, typename Next>
class ValueSetter<
            Field,
            Next,
            typename enable_if<CheckIfAny<Field>::value>::type
        >
{
template<typename _Field, typename _Next, typename _Dummy> friend class ValueSetter;
public:
    typedef Field F;
    typedef Next N;

    template<typename Record>
    typename Record::template constructor<ValueSetter<__Finish, Next> >::type begin()
    {
        return typename Record::template constructor<ValueSetter<__Finish, Next> >::type(
                    constructor->beginNested(
                        Record::ID,
                        Record::staticSize,
                        Record::headerSize,
                        true
                        )
                    );
    }

    template<typename Record>
    typename Record::recursive beginRecursive()
    {
        return typename Record::recursive(
                    constructor->beginNested(
                        Record::ID,
                        Record::staticSize,
                        Record::headerSize,
                        true
                        )
                    );
    }

    ValueSetter<typename Next::F, typename Next::N> set(const SerializedData *data)
    {
        size_t size = data->size();
        char *to = constructor->dynamicData(size);

        const char *begin = data->data();
        const char *end = begin+size;

        std::copy(begin, end, to);

        constructor->finishDynamicSize();
        return ValueSetter<typename Next::F, typename Next::N>(constructor);
    }

    ValueSetter<typename Next::F, typename Next::N> skip()
    {
        constructor->finishDynamicSize();
        return ValueSetter<typename Next::F, typename Next::N>(constructor);
    }

    ValueSetter(RecordConstructor *constructor):
        constructor(constructor){}
protected:
    RecordConstructor *constructor;
};

template<typename Field, typename Next>
class ValueSetter<
            Field,
            Next,
            typename enable_if<CheckIfVector<Field>::value>::type
        >
{
template<typename _Field, typename _Next, typename _Dummy> friend class ValueSetter;
public:
    typedef Field F;
    typedef Next N;

    ValueSetter<typename Next::F, typename Next::N> finish()
    {
        constructor->finishDynamicSize();
        return ValueSetter<typename Next::F, typename Next::N>(constructor);
    }

    ValueSetter<Field, Next> add(const typename Field::Tv value)
    {
        *reinterpret_cast<typename Field::Tv *>(constructor->dynamicData(sizeof(value))) = byte_order_to_le(value);

        return ValueSetter<Field, Next>(constructor);
    }

    template<typename VectorType>
    ValueSetter<Field, Next> addVector(VectorType value)
    {
        vectorConvert<VectorType> vector(value);

        size_t reqiredSize = (vector.end() - vector.begin())*sizeof(*vector.begin());

        typename Field::Tv *offset = reinterpret_cast<typename Field::Tv*>(constructor->dynamicData(reqiredSize));

        for(typename vectorConvert<VectorType>::IteratorType i = vector.begin();
                i < vector.end();
               ++i, ++offset) {
            *offset = byte_order_to_le(*i);
          }

        return ValueSetter<Field, Next>(constructor);
    }

    template<typename VectorType>
    ValueSetter<typename Next::F, typename Next::N> set(VectorType value)
    {
        return addVector(value).finish();
    }

    ValueSetter(RecordConstructor *constructor):
        constructor(constructor){}
protected:
    RecordConstructor *constructor;
};

template<typename Field, typename Next>
class ValueSetter<
            Field,
            Next,
            typename enable_if<CheckIfAnyVector<Field>::value>::type
        >
{
template<typename _Field, typename _Next, typename _Dummy> friend class ValueSetter;
public:
    typedef Field F;
    typedef Next N;

    template<typename Record>
    typename Record::template constructor<ValueSetter<__Finish,ValueSetter<Field,Next> > >::type begin()
    {
        return typename Record::template constructor<ValueSetter<__Finish,ValueSetter<Field,Next> > >::type(
                    constructor->beginNested(
                        Record::ID,
                        Record::staticSize,
                        Record::headerSize,
                        false
                        )
                    );
    }

    template<typename Record>
    typename Record::recursive beginRecursive()
    {
        return typename Record::recursive(
                    constructor->beginNested(
                        Record::ID,
                        Record::staticSize,
                        Record::headerSize,
                        false
                        )
                    );
    }

    ValueSetter<typename Next::F, typename Next::N> finish()
    {
        constructor->finishDynamicSize();
        return ValueSetter<typename Next::F, typename Next::N>(constructor);
    }

    ValueSetter(RecordConstructor *constructor):
        constructor(constructor)
    {}
protected:
    RecordConstructor *constructor;
};

template<typename Field, typename Next>
class ValueSetter<
            Field,
            Next,
            typename enable_if<CheckIfVoid<Field>::value>::type
        >
{
template<typename _Field, typename _Next, typename _Dummy> friend class ValueSetter;
public:
    typedef Field F;
    typedef Next N;

    ValueSetter<typename Next::F, typename Next::N> finish()
    {
        return ValueSetter<typename Next::F, typename Next::N>(constructor);
    }

    ValueSetter(RecordConstructor *constructor):
        constructor(constructor)
    {}
protected:
    RecordConstructor *constructor;
};



template<typename Next>
class ValueSetter<__Last, Next>
{
template<typename _Field, typename _Next, typename _Dummy> friend class ValueSetter;
public:
    typedef __Last F;
    typedef SerializedData* N;

    SerializedData* finish()
    {
        return result;
    }

    ValueSetter(RecordConstructor *constructor){
        result = constructor->root();
        delete constructor;
    }

protected:
    SerializedData *result;
};

template<typename Next>
class ValueSetter<__LastRecursive, Next>
{
    template<typename _Field, typename _Next, typename _Dummy> friend class ValueSetter;
public:
    typedef __LastRecursive F;
    typedef SerializedData* N;

    SerializedData* finish()
    {
        return result;
    }

    SerializedData* clear()
    {
        delete constructor;
        constructor = 0;
        return result;
    }

    ValueSetter(RecordConstructor *constructor):
        constructor(constructor){
        result = constructor->root();
    }
protected:
    RecordConstructor *constructor;
    SerializedData *result;
};

template<typename Next>
class ValueSetter<__Finish, Next>
{
    template<typename _Field, typename _Next, typename _Dummy> friend class ValueSetter;
public:
    typedef __Finish F;
    typedef Next N;

    ValueSetter<typename Next::F, typename Next::N> finish()
    {
        return ValueSetter<typename Next::F, typename Next::N>(constructor->finishNested());
    }

    ValueSetter(RecordConstructor *constructor):
        constructor(constructor){}
protected:
    RecordConstructor *constructor;
};
