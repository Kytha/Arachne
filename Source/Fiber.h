#pragma once
#include "Context.h"
#include <functional>

#define BIT(x) (1 << x)

struct result{
    uint64_t type;
    union{
        float    as_float;
        uint64_t as_int;
        bool     as_bool;
        void    *as_ptr;
    }val;
};

using taskFunction = void (*)(void*);

enum FiberFlags 
{
  NONE = 0,
  LARGE_FIBER = BIT(0),
};

struct Fiber
{
  int priority;
  void* stackMemory;
  void* arg;
  uint64_t returnValue;
  uint32_t id;
  uint32_t flags;
  struct Context context;
  Fiber   *prev, *next;
};