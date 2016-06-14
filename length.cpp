#include "curry.hpp"
#include <cassert>
#include <string>

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
  return new heap_obj{&info_int, (void*)i};
}

// Function: +
info_table const info_plus{"+", 2
  , [](heap_obj const * obj)
    {
      assert(obj);
      assert(2 == obj->info->arity);
      normalize(obj->edges[0]);
      normalize(obj->edges[1]);
      return make_int(data<int>(obj->edges[0].h->obj) + data<int>(obj->edges[1].h->obj));
    }
  };
heap_obj const * make_plus(heap_ptr lhs, heap_ptr rhs)
{
  return new heap_obj{&info_plus, nullptr, {lhs, rhs}};
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
