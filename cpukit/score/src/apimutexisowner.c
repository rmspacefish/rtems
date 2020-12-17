/**
 * @file
 *
 * @ingroup RTEMSScoreAPIMutex
 *
 * @brief This source file contains the implementation of
 *   _API_Mutex_Is_owner().
 */

/*
 *  COPYRIGHT (c) 1989-2015.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/apimutex.h>
#include <rtems/score/percpu.h>

bool _API_Mutex_Is_owner( const API_Mutex_Control *the_mutex )
{
  return the_mutex->Mutex._Mutex._Queue._owner == _Thread_Get_executing();
}
