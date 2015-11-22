class SerializedData
{
public:
    typedef u16 rid;
    typedef u32 hel;

    template< typename Field> typename Field::T extractField()
    {
        if(recordId() != Field::record::ID) throw WrongType();

        char *offset = block + sizeof(rid) + sizeof(hel)*Field::record::headerSize;

        offset += Field::staticOffset;
        for(int i=Field::dynamicOffset-1; i>=0; --i)
        {
            offset += byte_order_from_le(reinterpret_cast<hel*>(block+sizeof(rid))[i]);
        }
        return byte_order_from_le(*(reinterpret_cast<typename Field::T*>(offset)));
    }

    template< typename Field> void setField(const typename Field::T t)
    {
        if(recordId() != Field::record::ID) throw WrongType();

        char *offset = block + sizeof(rid) + sizeof(hel)*Field::record::headerSize;

        offset += Field::staticOffset;
        for(int i=Field::dynamicOffset-1; i>=0; --i)
        {
            offset += byte_order_from_le(reinterpret_cast<hel*>(block+sizeof(rid))[i]);
        }
        *(reinterpret_cast<typename Field::T*>(offset)) = byte_order_to_le(t);
    }

    template< typename Vector> std::vector<typename Vector::Tv> extractVector()
    {
        if(recordId() != Vector::record::ID) throw WrongType();

        char *offset = block + sizeof(rid) + sizeof(hel)*Vector::record::headerSize;

        offset += Vector::staticOffset;
        for(int i=Vector::dynamicOffset-1; i>=0; --i)
        {
            offset += byte_order_from_le(reinterpret_cast<hel*>(block+sizeof(rid))[i]);
        }

        std::vector<typename Vector::Tv> result;

        hel size = reinterpret_cast<hel*>(block + sizeof(rid))[Vector::dynamicOffset];
        result.resize(size/sizeof(typename Vector::Tv));

        typename Vector::V first = reinterpret_cast<typename Vector::V>(offset);
        typename Vector::V last = first + size/sizeof(typename Vector::Tv);

        typename std::vector<typename Vector::Tv>::iterator rp = result.begin();

        while (first!=last) {
            *rp = byte_order_from_le(*first);
            ++rp; ++first;
        }
        return result;
    }

    template< typename Any> SerializedData extractAny()
    {
        if(recordId() != Any::record::ID) throw WrongType();
        char *offset = block + sizeof(rid) + sizeof(hel)*Any::record::headerSize;
        offset += Any::staticOffset;
        for(int i=Any::dynamicOffset-1; i>=0; --i)
        {
            offset += byte_order_from_le(reinterpret_cast<hel*>(block+sizeof(rid))[i]);
        }
        return SerializedData(offset, false);
    }
    
    template< typename Vector > std::string extractString() // @todo: add proper type check
    {
      if(recordId() != Vector::record::ID) throw WrongType();
      
      char *offset = block + sizeof(rid) + sizeof(hel)*Vector::record::headerSize;
      
      offset += Vector::staticOffset;
      for(int i=Vector::dynamicOffset-1; i>=0; --i)
      {
        offset += byte_order_from_le(reinterpret_cast<hel*>(block+sizeof(rid))[i]);
      }
      
      hel size = reinterpret_cast<hel*>(block + sizeof(rid))[Vector::dynamicOffset];
      
      return std::string(offset, size);
    }
    

#ifdef QT_VERSION
    template< typename Vector> QVector<typename Vector::Tv> extractQVector()
    {
        if(recordId() != Vector::record::ID) throw WrongType();

        char *offset = block + sizeof(rid) + sizeof(hel)*Vector::record::headerSize;

        offset += Vector::staticOffset;
        for(int i=Vector::dynamicOffset-1; i>=0; --i)
        {
            offset += byte_order_from_le(reinterpret_cast<hel*>(block+sizeof(rid))[i]);
        }

        QVector<typename Vector::Tv> result;

        hel size = reinterpret_cast<hel*>(block + sizeof(rid))[Vector::dynamicOffset];
        result.resize(size/sizeof(typename Vector::Tv));

        typename Vector::V first = reinterpret_cast<typename Vector::V>(offset);
        typename Vector::V last = first + size/sizeof(typename Vector::Tv);

        typename std::vector<typename Vector::Tv>::iterator rp = result.begin();

        while (first!=last) {
            *rp = byte_order_from_le(*first);
            ++rp; ++first;
        }
        return result;
    }

    template< typename Vector > QString extractQString() // @todo: add proper type check
    {
        if(recordId() != Vector::record::ID) throw WrongType();

        char *offset = block + sizeof(rid) + sizeof(hel)*Vector::record::headerSize;

        offset += Vector::staticOffset;
        for(int i=Vector::dynamicOffset-1; i>=0; --i)
        {
            offset += byte_order_from_le(reinterpret_cast<hel*>(block+sizeof(rid))[i]);
        }

        hel size = reinterpret_cast<hel*>(block + sizeof(rid))[Vector::dynamicOffset];

        return QString::fromUtf8(offset, size);
    }

    QByteArray dataAsByteArray();
#endif

    typedef void(*reader)(char *memory, std::size_t size, void *opt);

    ~SerializedData();
    SerializedData(reader readBlock, void *opt=0);

    rid recordId();
    std::size_t size();
    char *data();

    SerializedData(char *block, bool owned=true);

    // + create_*
    // + headers

