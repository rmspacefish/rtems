/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerCBS
 *
 * @brief This source file contains the implementation of
 *   _Scheduler_CBS_Destroy_server().
 */

/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulercbs.h>

int _Scheduler_CBS_Destroy_server (
  Scheduler_CBS_Server_id server_id
)
{
  int ret = SCHEDULER_CBS_OK;
  rtems_id tid;

  if ( server_id >= _Scheduler_CBS_Maximum_servers )
    return SCHEDULER_CBS_ERROR_INVALID_PARAMETER;

  if ( !_Scheduler_CBS_Server_list[server_id].initialized )
    return SCHEDULER_CBS_ERROR_NOSERVER;

  if ( (tid = _Scheduler_CBS_Server_list[server_id].task_id) != -1 )
    ret = _Scheduler_CBS_Detach_thread ( server_id, tid );

  _Scheduler_CBS_Server_list[server_id].initialized = false;
  return ret;
}
