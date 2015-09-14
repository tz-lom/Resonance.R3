#ifndef FILEWRITER_SAVELIB_H
#define FILEWRITER_SAVELIB_H

#include "Messages.capnp.h"
#include "io.h"


namespace Resonance {
namespace R2E {


class R2EWriter
{
public:
    R2EWriter(const char *fileName, ::uint64_t timestamp, bool packed=true);
    ~R2EWriter();

    void write(const FileItem::Reader &item);

private:
    FileOutputStream file;
    ::kj::BufferedOutputStreamWrapper out;
    bool packed;
};

}
}

#endif
