/**
 * @file
 *
 * @ingroup RTEMSImplClassicIO
 *
 * @brief This source file contains the implementation of
 *   rtems_io_unregister_driver().
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

#include <rtems/ioimpl.h>
#include <rtems/rtems/intr.h>

#include <string.h>

rtems_status_code rtems_io_unregister_driver(
  rtems_device_major_number major
)
{
  if ( rtems_interrupt_is_in_progress() )
    return RTEMS_CALLED_FROM_ISR;

  if ( major < _IO_Number_of_drivers ) {
    ISR_lock_Context lock_context;

    _IO_Driver_registration_acquire( &lock_context );
    memset(
      &_IO_Driver_address_table[major],
      0,
      sizeof( rtems_driver_address_table )
    );
    _IO_Driver_registration_release( &lock_context );

    return RTEMS_SUCCESSFUL;
  }

  return RTEMS_UNSATISFIED;
}
