SerializedData::SerializedData(char *block, bool owned):block(block),owned(owned){}

SerializedData::~SerializedData()
{
    if(owned) delete [] block;
}

char* SerializedData::data()
{
    return block;
}

inline SerializedData::hel SerializedData::headerSize(rid id)
{
    if(id<1 || id>__LastType) throw WrongType();
    return headerSizes[id];
}

#ifdef QT_VERSION
QByteArray SerializedData::dataAsByteArray()
{
    return QByteArray(block, size());
}
#endif

std::size_t SerializedData::size()
{
    std::size_t sz =  sizeof(rid) + headerSize(recordId())*sizeof(hel) + staticSizes[recordId()];
    for(int i=headerSize(recordId())-1; i>=0; --i)
    {
        sz += reinterpret_cast<hel*>(block+sizeof(rid))[i];
    }
    return sz;
}

SerializedData::rid SerializedData::recordId()
{
    return *(reinterpret_cast<rid*>(block));
}

SerializedData::SerializedData(reader readBlock, void *opt):
    owned(true)
{
    rid id;
    readBlock((char*)&id, sizeof(rid), opt);
    std::size_t hs = headerSize(id);
    hel header[hs];
    readBlock((char*)&header, sizeof(hel)*hs, opt);
    std::size_t bulkSize = staticSizes[id];
    for(unsigned int i=0; i<hs; ++i)
    {
        bulkSize += byte_order_from_le(header[i]);
    }

    block = new char[sizeof(rid)+hs*sizeof(hel)+bulkSize];

    *(reinterpret_cast<rid*>(block)) = id;
    for(unsigned int i=0; i<hs; ++i)
    {
        reinterpret_cast<hel*>(block+sizeof(rid))[i] = (header[i]);
    }
    std::copy(header, header+hs, reinterpret_cast<hel*>(block+sizeof(rid)));
    readBlock(block+sizeof(rid)+sizeof(hel)*hs , bulkSize, opt);
}

