#ifndef READ_H
#define READ_H

#include <numeric>
#include <exception>

class Eof: public std::exception {};
class Error: public std::exception {};

void readFromFile(char *memory, std::size_t size, void *data);

#endif