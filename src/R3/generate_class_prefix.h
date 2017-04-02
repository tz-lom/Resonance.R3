class SerializedData
{
    friend class RecordConstructor;
public:
    typedef u16 rid;
    typedef u32 hel;

    template< typename Field> typename Field::T extractField() const
    {
        if(id() != Field::record::ID) throw WrongType();

        const char *offset = data() + sizeof(rid) + sizeof(hel)*Field::record::headerSize;

        offset += Field::staticOffset;
        for(int i=Field::dynamicOffset-1; i>=0; --i)
        {
            offset += byte_order_from_le(reinterpret_cast<const hel*>(data()+sizeof(rid))[i]);
        }
        return byte_order_from_le(*(reinterpret_cast<const typename Field::T*>(offset)));
    }

    template< typename Field> void setField(const typename Field::T t)
    {
        if(id() != Field::record::ID) throw WrongType();

        char *offset = data() + sizeof(rid) + sizeof(hel)*Field::record::headerSize;

        offset += Field::staticOffset;
        for(int i=Field::dynamicOffset-1; i>=0; --i)
        {
            offset += byte_order_from_le(reinterpret_cast<hel*>(data()+sizeof(rid))[i]);
        }
        *(reinterpret_cast<typename Field::T*>(offset)) = byte_order_to_le(t);
    }

    template< typename Vector> std::vector<typename Vector::Tv> extractVector() const
    {
        if(id() != Vector::record::ID) throw WrongType();


        const char *offset = data() + sizeof(rid) + sizeof(hel)*Vector::record::headerSize;

        offset += Vector::staticOffset;
        for(int i=Vector::dynamicOffset-1; i>=0; --i)
        {
            offset += byte_order_from_le(reinterpret_cast<const hel*>(data()+sizeof(rid))[i]);
        }

        std::vector<typename Vector::Tv> result;

        hel size = reinterpret_cast<const hel*>(data() + sizeof(rid))[Vector::dynamicOffset];
        result.resize(size/sizeof(typename Vector::Tv));

        typename Vector::V first = reinterpret_cast<typename Vector::V>(const_cast<char*>(offset));
        typename Vector::V last = first + size/sizeof(typename Vector::Tv);

        typename std::vector<typename Vector::Tv>::iterator rp = result.begin();

        while (first!=last) {
            *rp = byte_order_from_le(*first);
            ++rp; ++first;
        }
        return result;
    }

    template< typename Any> SerializedData extractAny() const
    {
        if(id() != Any::record::ID) throw WrongType();
        const char *offset = data() + sizeof(rid) + sizeof(hel)*Any::record::headerSize;
        offset += Any::staticOffset;
        for(int i=Any::dynamicOffset-1; i>=0; --i)
        {
            offset += byte_order_from_le(reinterpret_cast<const hel*>(data()+sizeof(rid))[i]);
        }
        return SerializedData(offset, byte_order_from_le(reinterpret_cast<const hel*>(data()+sizeof(rid))[Any::dynamicOffset]) );
    }

    template< typename Vector > std::string extractString() const // @todo: add proper type check
    {
      if(id() != Vector::record::ID) throw WrongType();

      const char *offset = data() + sizeof(rid) + sizeof(hel)*Vector::record::headerSize;

      offset += Vector::staticOffset;
      for(int i=Vector::dynamicOffset-1; i>=0; --i)
      {
        offset += byte_order_from_le(reinterpret_cast<const hel*>(data()+sizeof(rid))[i]);
      }

      hel size = byte_order_from_le(reinterpret_cast<const hel*>(data() + sizeof(rid))[Vector::dynamicOffset]);

      return std::string(offset, size);
    }

#ifdef QT_VERSION
    template< typename Vector> QVector<typename Vector::Tv> extractQVector() const
    {
        if(id() != Vector::record::ID) throw WrongType();

        const char *offset = data() + sizeof(rid) + sizeof(hel)*Vector::record::headerSize;

        offset += Vector::staticOffset;
        for(int i=Vector::dynamicOffset-1; i>=0; --i)
        {
            offset += byte_order_from_le(reinterpret_cast<const hel*>(data()+sizeof(rid))[i]);
        }

        QVector<typename Vector::Tv> result;

        hel size = byte_order_from_le(reinterpret_cast<const hel*>(data() + sizeof(rid))[Vector::dynamicOffset]);
        result.resize(size/sizeof(typename Vector::Tv));

        typename Vector::V first = reinterpret_cast<typename Vector::V>(const_cast<char*>(offset));
        typename Vector::V last = first + size/sizeof(typename Vector::Tv);

        typename QVector<typename Vector::Tv>::iterator rp = result.begin();

        while (first!=last) {
            *rp = byte_order_from_le(*first);
            ++rp; ++first;
        }
        return result;
    }

    template< typename Vector > QString extractQString() const // @todo: add proper type check
    {
        if(id() != Vector::record::ID) throw WrongType();

        const char *offset = data() + sizeof(rid) + sizeof(hel)*Vector::record::headerSize;

        offset += Vector::staticOffset;
        for(int i=Vector::dynamicOffset-1; i>=0; --i)
        {
            offset += byte_order_from_le(reinterpret_cast<const hel*>(data()+sizeof(rid))[i]);
        }

        hel size = byte_order_from_le(reinterpret_cast<const hel*>(data() + sizeof(rid))[Vector::dynamicOffset]);

        return QString::fromUtf8(offset, size);
    }

    QByteArray dataAsByteArray();
#endif

    template <typename AnyVector> SerializedData extractAnyVectorElement(size_t index) const
    {
        if (id() != AnyVector::record::ID)
        throw WrongType();

        const char *offset =
          data() + sizeof(rid) + sizeof(hel) * AnyVector::record::headerSize;

        hel self_size =
              byte_order_from_le(
                  reinterpret_cast<const hel *>(data() + sizeof(rid))[AnyVector::dynamicOffset]
              );

        if(self_size==0) throw WrongIndex();

        offset += AnyVector::staticOffset;
        for (int i = AnyVector::dynamicOffset - 1; i >= 0; --i) {
        offset +=
            byte_order_from_le(reinterpret_cast<const hel *>(data() + sizeof(rid))[i]);
        }

        for(size_t i=index; i>0; --i){
          // skip Any object
          rid id = byte_order_from_le(*reinterpret_cast<const rid*>(offset));
          offset += sizeof(rid);

          size_t sz = 0;
          for(int i=headerSize(id)-1; i>=0; --i, offset+=sizeof(hel))
          {
              sz += reinterpret_cast<const hel*>(data()+sizeof(rid))[i];
          }
          offset += sz + staticSizes[id];

          if(offset > data()+self_size) throw WrongIndex();
        }

        return SerializedData(offset, 2); //@todo: this is hack, need to propper calculate size here
    }

    template <typename AnyVector> std::vector<SerializedData> extractAnyVector() const
    {
        if (id() != AnyVector::record::ID)
          throw WrongType();

        const char *offset =
            data() + sizeof(rid) + sizeof(hel) * AnyVector::record::headerSize;

        std::vector<SerializedData> result;

        offset += AnyVector::staticOffset;
        for (int i = AnyVector::dynamicOffset - 1; i >= 0; --i) {
          offset +=
              byte_order_from_le(reinterpret_cast<const hel*>(data() + sizeof(rid))[i]);
        }

        const char *end =
                offset + byte_order_from_le(
                    reinterpret_cast<const hel*>(data() + sizeof(rid))[AnyVector::dynamicOffset]
                );

        while(offset<end)
        {
            SerializedData extr(offset, 2); //@todo: this is hack, need to propper calculate size here
            result.push_back(extr);
            offset += extr.size();
        }

        return result;
    }

    typedef void(*reader)(char *memory, size_t size, void *opt);

    SerializedData(reader readdata, void *opt=0);
    SerializedData(const char *block, size_t size);

    inline const char* data() const
    {
        if(block==0)
        {
            return &vec[0];
        }
        else
        {
            return block;
        }
    }

    inline char* data()
    {
        if(block==0)
        {
            return &vec[0];
        }
        else
        {
            return block;
        }
    }

    inline rid id() const
    {
        if(allocated==0) return 0;
        return *(reinterpret_cast<const rid*>(data()));
    }

    std::size_t size() const;

    // + create_*
    // + headers

