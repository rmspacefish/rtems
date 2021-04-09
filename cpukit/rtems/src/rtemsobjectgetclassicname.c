/**
 * @file
 *
 * @ingroup RTEMSImplClassicObject
 *
 * @brief This source file contains the implementation of
 *   rtems_object_get_classic_name().
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/object.h>
#include <rtems/rtems/statusimpl.h>
#include <rtems/score/objectimpl.h>

rtems_status_code rtems_object_get_classic_name(
  rtems_id      id,
  rtems_name   *name
)
{
  Status_Control status;
  Objects_Name   name_u;

  if ( !name )
    return RTEMS_INVALID_ADDRESS;

  status = _Objects_Id_to_name( id, &name_u );

  *name = name_u.name_u32;
  return _Status_Get( status );
}
