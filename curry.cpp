#include <urcu-pointer.h>

void normalize(heap_ptr p)
{
  p.h->obj = p.h->obj->info->step(p.h->obj);
}
