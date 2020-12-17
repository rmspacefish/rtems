/**
 * @file
 *
 * @ingroup RTEMSScoreProtHeap
 *
 * @brief This source file contains the implementation of
 *   _Protected_heap_Free().
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/protectedheap.h>

bool _Protected_heap_Free(
  Heap_Control *the_heap,
  void         *start_address
)
{
  bool    status;

  _RTEMS_Lock_allocator();
    status = _Heap_Free( the_heap, start_address );
  _RTEMS_Unlock_allocator();
  return status;
}
