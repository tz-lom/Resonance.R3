#include "read.h"
#include <fstream>

void readFromFile(char *memory, std::size_t size, void *data)
{
  std::ifstream *file = static_cast<std::ifstream*>(data);
  if(!file->read(memory, size))
  {
    if(file->eof()) throw Eof();
    throw Error();
  }
}