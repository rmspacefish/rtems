/**
 * @file
 *
 * @ingroup RTEMSScoreScheduler
 *
 * @brief This source file contains the implementation of
 *   _Scheduler_default_Schedule().
 */

/*
 * Copyright (c) 2014 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/scheduler.h>

void _Scheduler_default_Schedule(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  (void) scheduler;
  (void) the_thread;
}
