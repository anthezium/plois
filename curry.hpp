//! \file
//! \brief Defines a simple Curry framework.
//!
//! The data layout is summarized below:
//! \verbatim
//!
//!          holder
//!           --
//!          |  |
//!           --
//!            |
//!            |     heap_obj
//!            |      --------------------------
//!             ---->| info | data | heap_ptr[] |
//!                   --------------------------
//!                      |
//!                      |     info_table
//!                      |      --------
//!                       ---->| label  |
//!                            | arity  |
//!                            | step   |
//!                             --------
//!
//! \endverbatim

#pragma once
#include <cstddef>
#include <functional>

struct heap_obj;

//! \brief Contains information about each type of the Curry program.
//! Exactly one static table is created for each type.
struct info_table
{
  using stepfun_t = std::function<heap_obj const *(heap_obj const *)>;

  char const * label;   //! The node label, e.g., [], :, map, or zip
  size_t const arity;   //! The number of successors.
  stepfun_t const step; //! The step function
};

//! \brief Holds a pointer to a heap object.
//! Contains only a single pointer, and therefore guarantees atomic updates.
struct holder
{
  holder(heap_obj const * obj) : obj(obj) {}
  heap_obj const * obj;
};

//! \brief Models a pointer to a heap object.
struct heap_ptr
{
  heap_ptr(holder * h) : h(h) {}
  holder * h;
};

//! Represents a heap object (i.e., graph node).
struct heap_obj
{
  info_table const * info; //! Info for this type.
  void * data;             //! Instance data, e.g., a choice ID, or an int value.
  heap_ptr edges[];        //! Length is given by info->arity.
};


//! \brief Normalizes the held heap object.
//! Functions are head-normalized; constructors are fully normalized.
void normalize(heap_ptr p)
{
  //#define DEREF(p, x) (p)->(x)
  //#define ASSIGN_PTR(l, r) l = (r);
  //auto obj = DEREF(p.h, obj);
  auto obj = (p.h)->obj;
  auto rep1 = obj->info->step(obj);
  //ASSIGN_PTR(DEREF(p.h, obj), repl);
  (p.h)->obj = rep1;
}

template<typename T>
T data(heap_obj const * obj)
{
  static_assert(sizeof(T) <= sizeof(void *), "");
  union Data
  {
    Data(void * vp) : vp(vp) {}
    T value;
    void * vp;
  } data(obj->data);
  return data.value;
}

