/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Pthread Set Affinity
 */

/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#define  _GNU_SOURCE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_DECL_PTHREAD_SETAFFINITY_NP

#include <pthread.h>
#include <errno.h>

#include <rtems/score/threadimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/posix/posixapi.h>

int pthread_setaffinity_np(
  pthread_t        thread,
  size_t           cpusetsize,
  const cpu_set_t *cpuset
)
{
  Thread_Control   *the_thread;
  ISR_lock_Context  lock_context;
  Per_CPU_Control  *cpu_self;
  Status_Control    status;

  if ( cpuset == NULL ) {
    return EFAULT;
  }

  the_thread = _Thread_Get( thread, &lock_context );

  if ( the_thread == NULL ) {
    return ESRCH;
  }

  cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
  _Thread_State_acquire_critical( the_thread, &lock_context );

  status = _Scheduler_Set_affinity(
    the_thread,
    cpusetsize,
    cpuset
  );

  _Thread_State_release( the_thread, &lock_context );
  _Thread_Dispatch_enable( cpu_self );
  return _POSIX_Get_error( status );
}
#endif
