/**
 * @file
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This source file contains the definition of
 *   ::_Status_Object_name_errors_to_status.
 */

/*  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/rtems/statusimpl.h>

const rtems_status_code _Status_Object_name_errors_to_status[] = {
  /** This maps OBJECTS_NAME_OR_ID_LOOKUP_SUCCESSFUL to RTEMS_SUCCESSFUL. */
  RTEMS_SUCCESSFUL,
  /** This maps OBJECTS_INVALID_NAME to RTEMS_INVALID_NAME. */
  RTEMS_INVALID_NAME,
  /** This maps OBJECTS_INVALID_ADDRESS to RTEMS_INVALID_ADDRESS. */
  RTEMS_INVALID_ADDRESS,
  /** This maps OBJECTS_INVALID_ID to RTEMS_INVALID_ID. */
  RTEMS_INVALID_ID,
  /** This maps OBJECTS_INVALID_NODE to RTEMS_INVALID_NODE. */
  RTEMS_INVALID_NODE
};
