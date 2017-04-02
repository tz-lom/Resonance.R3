SerializedData::SerializedData(size_t allocate):
    block(0),
    allocated(allocate),
    vec(allocate)
{
}

SerializedData::SerializedData(const char *block, size_t allocate):
    block(const_cast<char*>(block)),
    allocated(allocate),
    vec()
{
}


void SerializedData::requireSize(size_t size)
{
    if(block) throw SerializedException();
    vec.resize(size);
    allocated = size;
}

inline SerializedData::hel SerializedData::headerSize(rid id) const
{
    if(id<1 || id>__LastType) throw WrongType();
    return headerSizes[id];
}

#ifdef QT_VERSION
QByteArray SerializedData::dataAsByteArray()
{
    return QByteArray(data(), size());
}
#endif

std::size_t SerializedData::size() const
{
    if(id()==0) return 0;
    std::size_t sz =  sizeof(rid) + headerSize(id())*sizeof(hel) + staticSizes[id()];
    for(int i=headerSize(id())-1; i>=0; --i)
    {
        sz += reinterpret_cast<const hel*>(data()+sizeof(rid))[i];
    }
    return sz;
}

SerializedData::SerializedData(reader readdata, void *opt):
    block(0),
    allocated(sizeof(rid)),
    vec(allocated)
{
    readdata(data(), sizeof(rid), opt);

    rid id = byte_order_from_le(*reinterpret_cast<rid*>(data()));

    size_t hs = headerSize(id);

    allocated += hs*sizeof(hel);
    requireSize(allocated);

    hel *header = reinterpret_cast<hel*>(data() + sizeof(rid));

    readdata((char*)header, sizeof(hel)*hs, opt);
    std::size_t bulkSize = staticSizes[id];
    for(unsigned int i=0; i<hs; ++i)
    {
        bulkSize += byte_order_from_le(header[i]);
    }

    allocated += bulkSize;
    requireSize(allocated);

    readdata(data()+sizeof(rid)+sizeof(hel)*hs , bulkSize, opt);
}
SerializedData *result;


RecordConstructor::RecordConstructor(SerializedData *data, size_t staticSize, size_t headerSize):
    data(data),
    dynamicOffset(sizeof(SerializedData::rid) + sizeof(SerializedData::hel)*headerSize + staticSize)
{
    staticOffset.push_back(sizeof(SerializedData::rid) + sizeof(SerializedData::hel)*headerSize);
    dynamicSizeOffset.push_back(sizeof(SerializedData::rid));
    finishDynamic.push_back(headerSize>0);
}

char *RecordConstructor::staticData(size_t size)
{
    char *ptr = data->data()+staticOffset.back();
    staticOffset.back() += size;
    return ptr;
}

char* RecordConstructor::dynamicData(size_t size)
{
    data->requireSize(dynamicOffset + size);
    char *ptr = data->data() + dynamicOffset;
    dynamicOffset += size;

    for(std::vector<size_t>::iterator i=dynamicSizeOffset.begin(); i!=dynamicSizeOffset.end(); ++i)
    {
        *reinterpret_cast<SerializedData::hel*>(data->data()+*i) += size;
    }

    return ptr;
}

void RecordConstructor::finishDynamicSize()
{
    *reinterpret_cast<SerializedData::hel*>(data->data()+dynamicSizeOffset.back()) = byte_order_to_le(*reinterpret_cast<SerializedData::hel*>(data->data()+dynamicSizeOffset.back()));
    dynamicSizeOffset.back() += sizeof(SerializedData::hel);
}


SerializedData *RecordConstructor::root()
{
    finishNested();
    return data;
}

RecordConstructor* RecordConstructor::beginNested(u16 id, size_t staticSize, size_t headerSize, bool last)
{
    size_t nestedSize = sizeof(id)+staticSize+headerSize*sizeof(SerializedData::hel);
    char *obj = dynamicData(nestedSize);
    *reinterpret_cast<u16*>(obj) = byte_order_to_le(id);

    SerializedData::hel *dyn = reinterpret_cast<SerializedData::hel*>(obj+sizeof(SerializedData::rid));
    for(size_t j=headerSize; j>0; --j, ++dyn) *dyn = 0;

    size_t hOffset = sizeof(id) + (obj-data->data()) ;
    size_t sOffset = hOffset + headerSize*sizeof(SerializedData::hel);

    staticOffset.push_back(sOffset);
    dynamicSizeOffset.push_back(hOffset);
    finishDynamic.push_back(last && headerSize>0);

    return this;
}

RecordConstructor* RecordConstructor::finishNested()
{
    if(finishDynamic.size()==0) return this;
    if(finishDynamic.back()) finishDynamicSize();

    staticOffset.pop_back();
    dynamicSizeOffset.pop_back();
    finishDynamic.pop_back();

    return this;
}
