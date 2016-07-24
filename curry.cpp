#include "curry.hpp"
#include <urcu-pointer.h>

void call_step(heap_ptr p)
{
  p.h->obj = p.h->obj->info->step(p.h->obj);
}

void head_normalize(heap_ptr p)
{
  while(p.h->obj->info->kind == OPER)
    call_step(p);
}

void normalize(heap_ptr p)
{
  head_normalize(p);
  size_t const N = p.h->obj->info->arity;
  for(size_t i=0; i<N; ++i)
    normalize(p.h->obj->edges[i]);
}
