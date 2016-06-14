#include "curry.hpp"
#include <cassert>
#include <string>
#include <cstdlib>

template<size_t N>
heap_obj const * normalize_ctor(heap_obj const * obj)
{
  assert(obj);
  assert(N == obj->info->arity);
  for(size_t i=0; i<N; ++i)
    normalize(obj->edges[i]);
  return obj;
}

// Type: List
info_table const info_nil{"[]", 0, &normalize_ctor<0>};
info_table const info_cons{":", 2, &normalize_ctor<2>};

// Type: Int
info_table const info_int{"Int", 0, &normalize_ctor<0>};
heap_obj const * make_int(int i)
{
  // return new heap_obj{&info_int, i};
  auto ptr = (heap_obj *)(std::malloc(sizeof(heap_obj)));
  ptr->info = &info_int;
  static_assert(sizeof(int64_t) == sizeof(void *), "");
  union { int64_t i; void * p; } u;
  u.i = i;
  ptr->info = &info_int;
  ptr->data = u.p;
  return ptr;
}

// Function: +
info_table const info_plus{"+", 2
  , [](heap_obj const * obj)
    {
      assert(obj);
      assert(2 == obj->info->arity);
      normalize(obj->edges[0]);
      normalize(obj->edges[1]);
      return make_int(
          data<int>(obj->edges[0].h->obj) + data<int>(obj->edges[1].h->obj)
        );
    }
  };
heap_obj const * make_plus(heap_ptr lhs, heap_ptr rhs)
{
  //return new heap_obj{&info_plus, nullptr, {lhs, rhs}};
  auto ptr = (heap_obj *)(std::malloc(sizeof(heap_obj) + 2 * sizeof(heap_ptr)));
  ptr->info = &info_plus;
  ptr->data = nullptr;
  ptr->edges[0] = lhs;
  ptr->edges[1] = rhs;
  return ptr;
}

// Function: length
// length [] = 0
// length (a:as) = 1 + length as
info_table const info_length{"length", 1
  , [](heap_obj const * obj)
    {
      assert(obj);
      assert(1 == obj->info->arity);
      if(obj->info == &info_cons)
      {
        // TODO
      }
      else
      {
        assert(obj->info == &info_nil);
        return make_int(0);
      }

    }
  };

int main()
{
  return 0;
}
