/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Canceling Execution of a Thread
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2016 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/score/isr.h>
#include <rtems/score/threadimpl.h>

/*
 *  18.2.1 Canceling Execution of a Thread, P1003.1c/Draft 10, p. 181
 */

int pthread_cancel( pthread_t thread )
{
  Thread_Control   *the_thread;
  ISR_lock_Context  lock_context;
  Thread_Control   *executing;
  Per_CPU_Control  *cpu_self;

  the_thread = _Thread_Get( thread, &lock_context );

  if ( the_thread == NULL ) {
    return ESRCH;
  }

  cpu_self = _Per_CPU_Get();
  executing = _Per_CPU_Get_executing( cpu_self );

  if (
    the_thread == executing &&
    !_Per_CPU_Is_ISR_in_progress( cpu_self )
  ) {
    _ISR_lock_ISR_enable( &lock_context );
    _Thread_Exit( PTHREAD_CANCELED, THREAD_LIFE_TERMINATING );
  } else {
    _Thread_Dispatch_disable_with_CPU( cpu_self, &lock_context );
    _ISR_lock_ISR_enable( &lock_context );
    _Thread_Cancel( the_thread, executing, PTHREAD_CANCELED );
    _Thread_Dispatch_enable( cpu_self );
  }
  return 0;
}
