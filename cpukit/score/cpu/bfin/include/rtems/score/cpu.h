/**
 * @file
 *
 * @brief Blackfin CPU Department Source
 *
 * This include file contains information pertaining to the Blackfin
 * processor.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *  adapted to Blackfin by Alain Schaefer <alain.schaefer@easc.ch>
 *                     and Antonio Giovanini <antonio@atos.com.br>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CPU_H
#define _RTEMS_SCORE_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/basedefs.h>
#include <rtems/score/bfin.h>

/* conditional compilation parameters */

/*
 *  Does the CPU follow the simple vectored interrupt model?
 *
 *  If TRUE, then RTEMS allocates the vector table it internally manages.
 *  If FALSE, then the BSP is assumed to allocate and manage the vector
 *  table
 *
 *  BFIN Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */
#define CPU_SIMPLE_VECTORED_INTERRUPTS TRUE

/**
 * Does the RTEMS invoke the user's ISR with the vector number and
 * a pointer to the saved interrupt frame (1) or just the vector
 * number (0)?
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define CPU_ISR_PASSES_FRAME_POINTER TRUE

#define CPU_HARDWARE_FP FALSE

#define CPU_SOFTWARE_FP FALSE

#define CPU_ALL_TASKS_ARE_FP FALSE

#define CPU_IDLE_TASK_IS_FP FALSE

#define CPU_USE_DEFERRED_FP_SWITCH FALSE

#define CPU_ENABLE_ROBUST_THREAD_DISPATCH FALSE

/**
 * Does the stack grow up (toward higher addresses) or down
 * (toward lower addresses)?
 *
 * If TRUE, then the grows upward.
 * If FALSE, then the grows toward smaller addresses.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define CPU_STACK_GROWS_UP               FALSE

/* FIXME: Is this the right value? */
#define CPU_CACHE_LINE_BYTES 32

#define CPU_STRUCTURE_ALIGNMENT

/**
 * @addtogroup RTEMSScoreCPUBfinCPUInterrupt
 * The following defines the number of bits actually used in the
 * interrupt field of the task mode.  How those bits map to the
 * CPU interrupt levels is defined by the routine @ref _CPU_ISR_Set_level.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define CPU_MODES_INTERRUPT_MASK   0x00000001

#define CPU_MAXIMUM_PROCESSORS 32

/*
 *  Processor defined structures required for cpukit/score.
 *
 *  Port Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */

/* may need to put some structures here.  */

#ifndef ASM

/**
 * @defgroup RTEMSScoreCPUBfinCPUContext Processor Dependent Context Management
 * 
 * @ingroup RTEMSScoreCPUBfin
 *
 * From the highest level viewpoint, there are 2 types of context to save.
 *
 *    -# Interrupt registers to save
 *    -# Task level registers to save
 *
 * Since RTEMS handles integer and floating point contexts separately, this
 * means we have the following 3 context items:
 *
 *    -# task level context stuff::  Context_Control
 *    -# floating point task stuff:: Context_Control_fp
 *    -# special interrupt level context :: CPU_Interrupt_frame
 *
 * On some processors, it is cost-effective to save only the callee
 * preserved registers during a task context switch.  This means
 * that the ISR code needs to save those registers which do not
 * persist across function calls.  It is not mandatory to make this
 * distinctions between the caller/callee saves registers for the
 * purpose of minimizing context saved during task switch and on interrupts.
 * If the cost of saving extra registers is minimal, simplicity is the
 * choice.  Save the same context on interrupt entry as for tasks in
 * this case.
 *
 * Additionally, if gdb is to be made aware of RTEMS tasks for this CPU, then
 * care should be used in designing the context area.
 *
 * On some CPUs with hardware floating point support, the Context_Control_fp
 * structure will not be used or it simply consist of an array of a
 * fixed number of bytes.   This is done when the floating point context
 * is dumped by a "FP save context" type instruction and the format
 * is not really defined by the CPU.  In this case, there is no need
 * to figure out the exact format -- only the size.  Of course, although
 * this is enough information for RTEMS, it is probably not enough for
 * a debugger such as gdb.  But that is another problem.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
/**@{**/

/**
 * This defines the minimal set of integer and processor state registers
 * that must be saved during a voluntary context switch from one thread
 * to another.
 */

/* make sure this stays in sync with the assembly function
   __CPU_Context_switch in cpu_asm.S  */
typedef struct {
    uint32_t   register_r4;
    uint32_t   register_r5;
    uint32_t   register_r6;
    uint32_t   register_r7;

    uint32_t   register_p3;
    uint32_t   register_p4;
    uint32_t   register_p5;
    uint32_t   register_fp;
    uint32_t   register_sp;

    uint32_t   register_rets;

    uint32_t   imask;
} Context_Control;

#define _CPU_Context_Get_SP( _context ) \
  (_context)->register_sp

/**
 * This defines the set of integer and processor state registers that must
 * be saved during an interrupt.  This set does not include any which are
 * in @ref Context_Control.
 */
typedef struct {
    /** This field is a hint that a port will have a number of integer
     * registers that need to be saved when an interrupt occurs or
     * when a context switch occurs at the end of an ISR.
     */
    /*uint32_t   special_interrupt_register;*/
} CPU_Interrupt_frame;

/** @} */

/**
 * @defgroup RTEMSScoreCPUBfinCPUInterrupt Processor Dependent Interrupt Management 
 * 
 * @ingroup RTEMSScoreCPUBfin
 */
/** @{ **/
/** @} **/

#endif /* ASM */

/**
 * @addtogroup RTEMSScoreCPUBfinCPUInterrupt
 * Amount of extra stack (above minimum stack size) required by
 * MPCI receive server thread.  Remember that in a multiprocessor
 * system this thread must exist and be able to process all directives.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 0

/**
 * @addtogroup RTEMSScoreCPUBfinCPUInterrupt
 * This defines the number of entries in the @ref _ISR_Vector_table managed
 * by RTEMS.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define CPU_INTERRUPT_NUMBER_OF_VECTORS      16

/**
 * @addtogroup RTEMSScoreCPUBfinCPUInterrupt
 * This defines the highest interrupt vector number for this port.
 */
#define CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER  (CPU_INTERRUPT_NUMBER_OF_VECTORS - 1)

/**
 * @addtogroup RTEMSScoreCPUBfinCPUInterrupt
 * This is defined if the port has a special way to report the ISR nesting
 * level.  Most ports maintain the variable @a _ISR_Nest_level.
 */
#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE

/**
 * @addtogroup RTEMSScoreCPUBfinCPUContext
 * Should be large enough to run all RTEMS tests.  This ensures
 * that a "reasonable" small application should not have any problems.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define CPU_STACK_MINIMUM_SIZE          (1024*8)

#define CPU_SIZEOF_POINTER 4

/**
 * CPU's worst alignment requirement for data types on a byte boundary.  This
 * alignment does not take into account the requirements for the stack.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define CPU_ALIGNMENT              8

/**
 * This number corresponds to the byte alignment requirement for the
 * heap handler.  This alignment requirement may be stricter than that
 * for the data types alignment specified by @ref CPU_ALIGNMENT.  It is
 * common for the heap to follow the same alignment requirement as
 * @ref CPU_ALIGNMENT.  If the @ref CPU_ALIGNMENT is strict enough for
 * the heap, then this should be set to @ref CPU_ALIGNMENT.
 *
 * @note  This does not have to be a power of 2 although it should be
 *        a multiple of 2 greater than or equal to 2.  The requirement
 *        to be a multiple of 2 is because the heap uses the least
 *        significant field of the front and back flags to indicate
 *        that a block is in use or free.  So you do not want any odd
 *        length blocks really putting length data in that bit.
 *
 *        On byte oriented architectures, @ref CPU_HEAP_ALIGNMENT normally will
 *        have to be greater or equal to than @ref CPU_ALIGNMENT to ensure that
 *        elements allocated from the heap meet all restrictions.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define CPU_HEAP_ALIGNMENT         CPU_ALIGNMENT

#define CPU_STACK_ALIGNMENT        8

#define CPU_INTERRUPT_STACK_ALIGNMENT CPU_CACHE_LINE_BYTES

#ifndef ASM

/*
 *  ISR handler macros
 */

/**
 * @addtogroup RTEMSScoreCPUBfinCPUInterrupt
 */
/**@{**/

/**
 * Disable all interrupts for an RTEMS critical section.  The previous
 * level is returned in @a _isr_cookie.
 *
 * @param[out] _isr_cookie will contain the previous level cookie
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define _CPU_ISR_Disable( _level ) \
  {                                     \
       __asm__ volatile ("cli %0; csync \n" : "=d" (_level) );     \
  }


/**
 * Enable interrupts to the previous level (returned by _CPU_ISR_Disable).
 * This indicates the end of an RTEMS critical section.  The parameter
 * @a _isr_cookie is not modified.
 *
 * @param[in] _isr_cookie contain the previous level cookie
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define _CPU_ISR_Enable( _level ) { \
    __asm__ __volatile__ ("sti %0; csync \n" : : "d" (_level) );   \
  }

/**
 * This temporarily restores the interrupt to @a _isr_cookie before immediately
 * disabling them again.  This is used to divide long RTEMS critical
 * sections into two or more parts.  The parameter @a _isr_cookie is not
 * modified.
 *
 * @param[in] _isr_cookie contain the previous level cookie
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define _CPU_ISR_Flash( _level ) { \
    __asm__ __volatile__ ("sti %0; csync; cli r0; csync" \
                          : : "d"(_level) : "R0" ); \
  }

RTEMS_INLINE_ROUTINE bool _CPU_ISR_Is_enabled( uint32_t level )
{
  return level != 0;
}

/**
 * This routine and @ref _CPU_ISR_Get_level
 * Map the interrupt level in task mode onto the hardware that the CPU
 * actually provides.  Currently, interrupt levels which do not
 * map onto the CPU in a generic fashion are undefined.  Someday,
 * it would be nice if these were "mapped" by the application
 * via a callout.  For example, m68k has 8 levels 0 - 7, levels
 * 8 - 255 would be available for bsp/application specific meaning.
 * This could be used to manage a programmable interrupt controller
 * via the rtems_task_mode directive.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define _CPU_ISR_Set_level( _new_level ) \
  { \
    __asm__ __volatile__ ( "sti %0; csync" : : "d"(_new_level ? 0 : 0xffff) ); \
  }

/**
 * Return the current interrupt disable level for this task in
 * the format used by the interrupt level portion of the task mode.
 *
 * @note This routine usually must be implemented as a subroutine.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
uint32_t   _CPU_ISR_Get_level( void );

/* end of ISR handler macros */

/** @} */

/* Context handler macros */

/**
 * @addtogroup RTEMSScoreCPUBfinCPUContext
 * Initialize the context to a state suitable for starting a
 * task after a context restore operation.  Generally, this
 * involves:
 *
 *    - setting a starting address
 *    - preparing the stack
 *    - preparing the stack and frame pointers
 *    - setting the proper interrupt level in the context
 *    - initializing the floating point context
 *
 * This routine generally does not set any unnecessary register
 * in the context.  The state of the "general data" registers is
 * undefined at task start time.
 *
 * @param[in] _the_context is the context structure to be initialized
 * @param[in] _stack_base is the lowest physical address of this task's stack
 * @param[in] _size is the size of this task's stack
 * @param[in] _isr is the interrupt disable level
 * @param[in] _entry_point is the thread's entry point.  This is
 *        always @a _Thread_Handler
 * @param[in] _is_fp is TRUE if the thread is to be a floating
 *       point thread.  This is typically only used on CPUs where the
 *       FPU may be easily disabled by software such as on the SPARC
 *       where the PSR contains an enable FPU bit.
 * @param[in] tls_area is the thread-local storage (TLS) area
 *
 * Port Specific Information:
 *
 * See implementation in cpu.c
 */
void _CPU_Context_Initialize(
  Context_Control  *the_context,
  uint32_t         *stack_base,
  uint32_t          size,
  uint32_t          new_level,
  void             *entry_point,
  bool              is_fp,
  void             *tls_area
);

/**
 * This routine is responsible for somehow restarting the currently
 * executing task.  If you are lucky, then all that is necessary
 * is restoring the context.  Otherwise, there will need to be
 * a special assembly routine which does something special in this
 * case.  For many ports, simply adding a label to the restore path
 * of @ref _CPU_Context_switch will work.  On other ports, it may be
 * possibly to load a few arguments and jump to the restore path. It will
 * not work if restarting self conflicts with the stack frame
 * assumptions of restoring a context.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define _CPU_Context_Restart_self( _the_context ) \
   _CPU_Context_restore( (_the_context) );

/* end of Context handler macros */

/* Fatal Error manager macros */

/**
 * This routine copies _error into a known place -- typically a stack
 * location or a register, optionally disables interrupts, and
 * halts/stops the CPU.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
#define _CPU_Fatal_halt( _source, _error ) \
  { \
    __asm__ volatile ( "cli R1; \
                    R1 = %0; \
                    _halt: \
                    idle; \
                    jump _halt;"\
                    : : "r" (_error) ); \
  }

/* end of Fatal Error manager macros */

#define CPU_USE_GENERIC_BITFIELD_CODE TRUE

#define CPU_USE_LIBC_INIT_FINI_ARRAY FALSE

/* functions */

/**
 * @brief CPU initialize.
 * This routine performs CPU dependent initialization.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
void _CPU_Initialize(void);

typedef void ( *CPU_ISR_raw_handler )( void );

void _CPU_ISR_install_raw_handler(
  uint32_t             vector,
  CPU_ISR_raw_handler  new_handler,
  CPU_ISR_raw_handler *old_handler
);

typedef void ( *CPU_ISR_handler )( uint32_t );

void _CPU_ISR_install_vector(
  uint32_t         vector,
  CPU_ISR_handler  new_handler,
  CPU_ISR_handler *old_handler
);

void *_CPU_Thread_Idle_body( uintptr_t ignored );

/**
 * @addtogroup RTEMSScoreCPUBfinCPUContext
 */
/**@{**/

/**
 * This routine switches from the run context to the heir context.
 *
 * @param[in] run points to the context of the currently executing task
 * @param[in] heir points to the context of the heir task
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
void _CPU_Context_switch(
  Context_Control  *run,
  Context_Control  *heir
);

/**
 * This routine is generally used only to restart self in an
 * efficient manner.  It may simply be a label in @ref _CPU_Context_switch.
 *
 * @param[in] new_context points to the context to be restored.
 *
 * @note May be unnecessary to reload some registers.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
RTEMS_NO_RETURN void _CPU_Context_restore( Context_Control *new_context );

/** @} */

/* FIXME */
typedef CPU_Interrupt_frame CPU_Exception_frame;

void _CPU_Exception_frame_print( const CPU_Exception_frame *frame );

/**
 * 
 * @defgroup RTEMSScoreCPUBfinCPUEndian CPUEndian
 * 
 * @ingroup RTEMSScoreCPUBfin
 * 
 * @brief CPUEndian
 */
/** @{ */
/*
 * 
 * The following routine swaps the endian format of an unsigned int.
 * It must be static because it is referenced indirectly.
 *
 * This version will work on any processor, but if there is a better
 * way for your CPU PLEASE use it.  The most common way to do this is to:
 *
 *    swap least significant two bytes with 16-bit rotate
 *    swap upper and lower 16-bits
 *    swap most significant two bytes with 16-bit rotate
 *
 * Some CPUs have special instructions which swap a 32-bit quantity in
 * a single instruction (e.g. i486).  It is probably best to avoid
 * an "endian swapping control bit" in the CPU.  One good reason is
 * that interrupts would probably have to be disabled to ensure that
 * an interrupt does not try to access the same "chunk" with the wrong
 * endian.  Another good reason is that on some CPUs, the endian bit
 * endianness for ALL fetches -- both code and data -- so the code
 * will be fetched incorrectly.
 *
 * @param[in] value is the value to be swapped
 * @return the value after being endian swapped
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */

static inline uint32_t CPU_swap_u32(
  uint32_t value
)
{
  uint32_t   byte1, byte2, byte3, byte4, swapped;

  byte4 = (value >> 24) & 0xff;
  byte3 = (value >> 16) & 0xff;
  byte2 = (value >> 8)  & 0xff;
  byte1 =  value        & 0xff;

  swapped = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
  return( swapped );
}

/**
 * This routine swaps a 16 bir quantity.
 *
 * @param[in] value is the value to be swapped
 * @return the value after being endian swapped
 */
#define CPU_swap_u16( value ) \
  (((value&0xff) << 8) | ((value >> 8)&0xff))
  
/** @} */

typedef uint32_t CPU_Counter_ticks;

uint32_t _CPU_Counter_frequency( void );

CPU_Counter_ticks _CPU_Counter_read( void );

static inline CPU_Counter_ticks _CPU_Counter_difference(
  CPU_Counter_ticks second,
  CPU_Counter_ticks first
)
{
  return second - first;
}

/** Type that can store a 32-bit integer or a pointer. */
typedef uintptr_t CPU_Uint32ptr;

#endif /* ASM */

#ifdef __cplusplus
}
#endif

#endif
