/**
 * @file
 *
 * @ingroup RTEMSImplClassicTask
 *
 * @brief This source file contains the implementation of
 *   rtems_task_wake_when().
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasks.h>
#include <rtems/rtems/clockimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/todimpl.h>
#include <rtems/score/watchdogimpl.h>

rtems_status_code rtems_task_wake_when(
  const rtems_time_of_day *time_buffer
)
{
  uint32_t          seconds;
  Thread_Control   *executing;
  Per_CPU_Control  *cpu_self;
  rtems_status_code status;

  if ( !_TOD_Is_set() )
    return RTEMS_NOT_DEFINED;

  status = _TOD_Validate( time_buffer, TOD_DISABLE_TICKS_VALIDATION );

  if ( status != RTEMS_SUCCESSFUL ) {
    return status;
  }

  seconds = _TOD_To_seconds( time_buffer );

  if ( seconds <= _TOD_Seconds_since_epoch() )
    return RTEMS_INVALID_CLOCK;

  cpu_self = _Thread_Dispatch_disable();
    executing = _Per_CPU_Get_executing( cpu_self );
    _Thread_Set_state( executing, STATES_WAITING_FOR_TIME );
    _Thread_Wait_flags_set( executing, THREAD_WAIT_STATE_BLOCKED );
    _Thread_Timer_insert_realtime(
      executing,
      cpu_self,
      _Thread_Timeout,
      _Watchdog_Ticks_from_seconds( seconds )
    );
  _Thread_Dispatch_direct( cpu_self );
  return RTEMS_SUCCESSFUL;
}
