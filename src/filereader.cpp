#include "filereader.h"
#include <capnp/serialize.h>
#include "io.h"
#include <iostream>
#include <stdexcept>

using namespace Resonance::R2E;

R2EReader::R2EReader(const char *fileName):
    file(fileName),
    in(file)
{
    char head[4];
    in.read(head, 4);
    if( head[0]!='!' ||
        head[1] != 'R' ||
        head[2] != '2' ||
        head[3] != 'E')
    {
        throw std::logic_error("Wrong header");
    }
    capnp::InputStreamMessageReader reader(in);
    FileHeader::Reader header = reader.getRoot<FileHeader>();
    if(header.getVersion()> 2)
    {
        throw std::logic_error("File has newer version than supported");
    }
    packed = header.getPacked();
    
    std::cout << packed << "\n";
}

::capnp::MessageReader* R2EReader::nextItem()
{
    while(in.tryGetReadBuffer() != nullptr)
    {
        return new ::capnp::InputStreamMessageReader(in);
    }
    return 0;
}

