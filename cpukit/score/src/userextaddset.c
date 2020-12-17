/**
 * @file
 *
 * @ingroup RTEMSScoreUserExt
 *
 * @brief This source file contains the implementation of
 *   _User_extensions_Add_set().
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

#include <rtems/score/userextimpl.h>
#include <rtems/score/smp.h>
#include <rtems/score/percpu.h>

static void _User_extensions_Set_ancestors( void )
{
#if defined(RTEMS_SMP)
  if ( _Chain_Is_empty( &_User_extensions_Switches_list ) ) {
    uint32_t cpu_max;
    uint32_t cpu_index;

    cpu_max = _SMP_Get_processor_maximum();

    for ( cpu_index = 0 ; cpu_index < cpu_max ; ++cpu_index ) {
       Per_CPU_Control *cpu;

       cpu = _Per_CPU_Get_by_index( cpu_index );
       cpu->ancestor = cpu->executing;
    }
  }
#endif
}

void _User_extensions_Add_set(
  User_extensions_Control *the_extension
)
{
  ISR_lock_Context lock_context;

  _User_extensions_Acquire( &lock_context );
  _Chain_Initialize_node( &the_extension->Node );
  _Chain_Append_unprotected(
    &_User_extensions_List.Active,
    &the_extension->Node
  );
  _User_extensions_Release( &lock_context );

  /*
   * If a switch handler is present, append it to the switch chain.
   */

  if ( the_extension->Callouts.thread_switch != NULL ) {
    the_extension->Switch.thread_switch =
      the_extension->Callouts.thread_switch;

    _Per_CPU_Acquire_all( &lock_context );
    _User_extensions_Set_ancestors();
    _Chain_Initialize_node( &the_extension->Switch.Node );
    _Chain_Append_unprotected(
      &_User_extensions_Switches_list,
      &the_extension->Switch.Node
    );
    _Per_CPU_Release_all( &lock_context );
  }
}
