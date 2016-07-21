#include "read.h"
#include <fstream>

void readFromFile(char *memory, std::size_t size, void *data)
{
  std::ifstream *file = static_cast<std::ifstream*>(data);
  file->read(memory, size);
}