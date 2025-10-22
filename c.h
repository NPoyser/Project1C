#ifndef C_H
#define C_H
#include <stdio.h>
#include <stdint.h>


typedef uint64_t U64;

U64 SetBit(U64 value, int position);
U64 ClearBit(U64 value, int position);
int GetBit(U64 value, int position);


#endif
