#include "curry.hpp"
#include <cassert>
#include <string>
#include <cstdlib>
#include <initializer_list>
#include <algorithm>
#include <iostream>

template<size_t N>
heap_obj const * normalize_ctor(heap_obj const * obj)
{
  assert(obj);
  assert(N == obj->info->arity);
  for(size_t i=0; i<N; ++i)
    normalize(obj->edges[i]);
  return obj;
}

heap_obj const * make_heap_obj(
    info_table const & info
  , void * data
  , std::initializer_list<heap_ptr> const & args
  )
{
  size_t const N = args.size();
  auto ptr = (heap_obj *)(std::malloc(sizeof(heap_obj) + N * sizeof(heap_ptr)));
  ptr->info = &info;
  ptr->data = data;
  std::copy(args.begin(), args.end(), &ptr->edges[0]);
  return ptr;
}

heap_ptr box(heap_obj const * obj)
{
  return new holder(obj);
}

heap_ptr make_heap_ptr(
    info_table const & info
  , void * data
  , std::initializer_list<heap_ptr> const & args
  )
{
  return box(make_heap_obj(info, data, args));
}

// Type: List
info_table const info_nil{CTOR, "[]", 0, &normalize_ctor<0>};
info_table const info_cons{CTOR, ":", 2, &normalize_ctor<2>};

// Type: Int
info_table const info_int{CTOR, "Int", 0, &normalize_ctor<0>};
union IntData { int64_t i; void * p; };
heap_obj const * make_int(int i)
{
  static_assert(sizeof(int64_t) == sizeof(void *), "");
  IntData u;
  u.i = i;
  return make_heap_obj(info_int, u.p, {});
}

int64_t get_int(heap_obj const * obj)
{
  IntData u;
  u.p = obj->data;
  return u.i;
}

// Function: +
info_table const info_plus{OPER, "+", 2
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
  return make_heap_obj(info_plus, nullptr, {lhs, rhs});
}

// Function: length
// length [] = 0
// length (a:as) = 1 + length as
extern info_table const info_length;
info_table const info_length{OPER, "length", 1
  , [](heap_obj const * obj)
    {
      assert(obj);
      assert(1 == obj->info->arity);
      retry:
      heap_obj const * list = obj->edges[0].h->obj;
      if(list->info->kind == CTOR)
      {
        if(list->info == &info_cons)
        {
          return make_plus(
              box(make_int(1))
            , make_heap_ptr(info_length, nullptr, {list->edges[1]})
            );
        }
        else
        {
          assert(list->info == &info_nil);
          return make_int(0);
        }
      }
      else
      {
        assert(list->info->kind == OPER);
        normalize(obj->edges[0]);
        goto retry;
      }
    }
  };

int main()
{
  heap_ptr main = ({
    auto one = box(make_int(1));
    auto two = box(make_int(2));
    auto three = box(make_int(3));
    auto nil = make_heap_ptr(info_nil, nullptr, {});
    auto cons = [](heap_ptr head, heap_ptr tail)
      { return make_heap_ptr(info_cons, nullptr, {head, tail}); };
    auto list = cons(one, cons(two, cons(three, nil)));
    make_heap_ptr(info_length, nullptr, {list});
  });
  normalize(main);
  assert(main.h->obj->info == &info_int);
  std::cout << get_int(main.h->obj) << std::endl;
  return 0;
}
