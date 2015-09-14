#include "filewriter.h"
#include <capnp/serialize.h>
#include "io.h"
#include <capnp/serialize-packed.h>

using namespace Resonance::R2E;

R2EWriter::R2EWriter(const char *fileName, uint64_t timestamp, bool packed):
    file(fileName),
    out(file),
    packed(packed)
{
    char head[] = "!R2E";
    out.write(head, 4);
    ::capnp::MallocMessageBuilder message;
    FileHeader::Builder header = message.initRoot<FileHeader>();
    header.setTimestamp(timestamp);
    header.setVersion(2);
    header.setPacked(packed);
    ::capnp::writeMessage(out, message);

}

R2EWriter::~R2EWriter()
{
    out.flush();
}

void R2EWriter::write(const FileItem::Reader &item)
{
    ::capnp::MallocMessageBuilder message;
    message.setRoot(item);
    if(packed)
    {
        ::capnp::writePackedMessage(out, message);
    }
    else
    {
        ::capnp::writeMessage(out, message);
    }
}
