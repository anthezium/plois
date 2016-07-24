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

enum NodeKind { OPER, CTOR };
//! \brief Contains information about each type of the Curry program.
//! Exactly one static table is created for each type.
struct info_table
{
  using stepfun_t = std::function<heap_obj const *(heap_obj const *)>;

  NodeKind kind;        //! The node kind.
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

//! \brief Performs one step at the given node.
void call_step(heap_ptr p);

//! \brief Head-normalizes the held heap object.
void head_normalize(heap_ptr p);

//! \brief Fully normalizes a heap object.
void normalize(heap_ptr p);
void normalize(heap_obj const * obj);

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

