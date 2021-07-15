/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup aarch64_start
 *
 * @brief AArch64 MMU configuration.
 */

/*
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LIBBSP_AARCH64_SHARED_AARCH64_MMU_H
#define LIBBSP_AARCH64_SHARED_AARCH64_MMU_H

#include <bsp/start.h>
#include <bsp/linker-symbols.h>
#include <rtems/score/aarch64-system-registers.h>
#include <bspopts.h>
#include <bsp/utility.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* VMSAv8 Long-descriptor fields */
#define MMU_DESC_AF                        BSP_BIT64( 10 )
#define MMU_DESC_SH_INNER                  ( BSP_BIT64( 9 ) | BSP_BIT64( 8 ) )
#define MMU_DESC_WRITE_DISABLE             BSP_BIT64( 7 )
/* PAGE and TABLE flags are the same bit, but only apply on certain levels */
#define MMU_DESC_TYPE_TABLE                BSP_BIT64( 1 )
#define MMU_DESC_TYPE_PAGE                 BSP_BIT64( 1 )
#define MMU_DESC_VALID                     BSP_BIT64( 0 )
#define MMU_DESC_MAIR_ATTR( val )          BSP_FLD64( val, 2, 3 )
#define MMU_DESC_MAIR_ATTR_GET( reg )      BSP_FLD64GET( reg, 2, 3 )
#define MMU_DESC_MAIR_ATTR_SET( reg, val ) BSP_FLD64SET( reg, val, 2, 3 )
#define MMU_DESC_PAGE_TABLE_MASK           0xFFFFFFFFF000LL

/* Page table configuration */
#define MMU_PAGE_BITS           12
#define MMU_PAGE_SIZE           ( 1 << MMU_PAGE_BITS )
#define MMU_BITS_PER_LEVEL      9
#define MMU_TOP_LEVEL_PAGE_BITS ( 2 * MMU_BITS_PER_LEVEL + MMU_PAGE_BITS )

#define AARCH64_MMU_FLAGS_BASE \
  ( MMU_DESC_VALID | MMU_DESC_SH_INNER | MMU_DESC_AF )

#define AARCH64_MMU_DATA_RO_CACHED \
  ( AARCH64_MMU_FLAGS_BASE | MMU_DESC_MAIR_ATTR( 3 ) | MMU_DESC_WRITE_DISABLE )
#define AARCH64_MMU_CODE_CACHED AARCH64_MMU_DATA_RO_CACHED
#define AARCH64_MMU_CODE_RW_CACHED AARCH64_MMU_DATA_RW_CACHED

#define AARCH64_MMU_DATA_RO \
  ( AARCH64_MMU_FLAGS_BASE | MMU_DESC_MAIR_ATTR( 1 ) | MMU_DESC_WRITE_DISABLE )
#define AARCH64_MMU_CODE AARCH64_MMU_DATA_RO
#define AARCH64_MMU_CODE_RW AARCH64_MMU_DATA_RW

/* RW implied by not ORing in RO */
#define AARCH64_MMU_DATA_RW_CACHED \
  ( AARCH64_MMU_FLAGS_BASE | MMU_DESC_MAIR_ATTR( 3 ) )
#define AARCH64_MMU_DATA_RW \
  ( AARCH64_MMU_FLAGS_BASE | MMU_DESC_MAIR_ATTR( 1 ) )
#define AARCH64_MMU_DEVICE ( AARCH64_MMU_FLAGS_BASE | MMU_DESC_MAIR_ATTR( 0 ) )

typedef struct {
  uintptr_t begin;
  uintptr_t end;
  uint64_t flags;
} aarch64_mmu_config_entry;

#define AARCH64_MMU_DEFAULT_SECTIONS \
  { \
    .begin = (uintptr_t) bsp_section_fast_text_begin, \
    .end = (uintptr_t) bsp_section_fast_text_end, \
    .flags = AARCH64_MMU_CODE_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_section_fast_data_begin, \
    .end = (uintptr_t) bsp_section_fast_data_end, \
    .flags = AARCH64_MMU_DATA_RW_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_section_start_begin, \
    .end = (uintptr_t) bsp_section_start_end, \
    .flags = AARCH64_MMU_CODE_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_section_vector_begin, \
    .end = (uintptr_t) bsp_section_vector_end, \
    .flags = AARCH64_MMU_DATA_RW_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_section_text_begin, \
    .end = (uintptr_t) bsp_section_text_end, \
    .flags = AARCH64_MMU_CODE_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_section_rodata_begin, \
    .end = (uintptr_t) bsp_section_rodata_end, \
    .flags = AARCH64_MMU_DATA_RO_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_section_data_begin, \
    .end = (uintptr_t) bsp_section_data_end, \
    .flags = AARCH64_MMU_DATA_RW_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_section_bss_begin, \
    .end = (uintptr_t) bsp_section_bss_end, \
    .flags = AARCH64_MMU_DATA_RW_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_section_rtemsstack_begin, \
    .end = (uintptr_t) bsp_section_rtemsstack_end, \
    .flags = AARCH64_MMU_DATA_RW_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_section_work_begin, \
    .end = (uintptr_t) bsp_section_work_end, \
    .flags = AARCH64_MMU_DATA_RW_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_section_stack_begin, \
    .end = (uintptr_t) bsp_section_stack_end, \
    .flags = AARCH64_MMU_DATA_RW_CACHED \
  }, { \
    .begin = (uintptr_t) bsp_section_nocache_begin, \
    .end = (uintptr_t) bsp_section_nocache_end, \
    .flags = AARCH64_MMU_DEVICE \
  }, { \
    .begin = (uintptr_t) bsp_section_nocachenoload_begin, \
    .end = (uintptr_t) bsp_section_nocachenoload_end, \
    .flags = AARCH64_MMU_DEVICE \
  }, { \
    .begin = (uintptr_t) bsp_translation_table_base, \
    .end = (uintptr_t) bsp_translation_table_end, \
    .flags = AARCH64_MMU_DATA_RW_CACHED \
  }, { \
/*
 * The vector table must be in writable and executable memory as it stores both
 * exception code and the mutable pointer to which it jumps
 */ \
    .begin = (uintptr_t) bsp_start_vector_table_begin, \
    .end = (uintptr_t) bsp_start_vector_table_end, \
    .flags = AARCH64_MMU_CODE_RW_CACHED \
  }

/* setup straight mapped block entries */
BSP_START_TEXT_SECTION static inline void aarch64_mmu_page_table_set_blocks(
  uint64_t *page_table,
  uint64_t base,
  uint32_t bits_offset,
  uint64_t default_attr
)
{
  uint64_t page_flag = 0;

  if ( bits_offset == MMU_PAGE_BITS ) {
    page_flag = MMU_DESC_TYPE_PAGE;
  }

  for ( uint64_t i = 0; i < ( 1 << MMU_BITS_PER_LEVEL ); i++ ) {
    page_table[i] = base | ( i << bits_offset );
    page_table[i] |= default_attr | page_flag;
  }
}

BSP_START_TEXT_SECTION static inline rtems_status_code
aarch64_mmu_page_table_alloc( uint64_t **page_table )
{
  /* First page table is already in use as TTB0 */
  static uintptr_t *current_page_table =
    (uintptr_t *) bsp_translation_table_base;

  current_page_table += MMU_PAGE_SIZE;
  *page_table = (uint64_t *) current_page_table;

  /* Out of linker-allocated page tables? */
  uintptr_t consumed_pages = (uintptr_t) current_page_table;
  consumed_pages -= (uintptr_t) bsp_translation_table_base;
  consumed_pages /= MMU_PAGE_SIZE;

  if ( consumed_pages > AARCH64_MMU_TRANSLATION_TABLE_PAGES ) {
    *page_table = NULL;
    return RTEMS_NO_MEMORY;
  }

  return RTEMS_SUCCESSFUL;
}

BSP_START_TEXT_SECTION static inline uintptr_t aarch64_mmu_get_index(
  uintptr_t root_address,
  uintptr_t vaddr,
  uint32_t shift
)
{
  uintptr_t mask = ( 1 << ( MMU_BITS_PER_LEVEL + 1 ) ) - 1;

  return ( ( vaddr - root_address ) >> shift ) & mask;
}

BSP_START_TEXT_SECTION static inline rtems_status_code
aarch64_mmu_get_sub_table(
  uint64_t *page_table_entry,
  uint64_t **sub_table,
  uintptr_t physical_root_address,
  uint32_t shift
)
{
  /* check if the index already has a page table */
  if ( ( *page_table_entry & MMU_DESC_TYPE_TABLE ) == MMU_DESC_TYPE_TABLE ) {
    /* extract page table address */
    uint64_t table_pointer = *page_table_entry & MMU_DESC_PAGE_TABLE_MASK;
    /* This cast should be safe since the address was inserted in this mode */
    *sub_table = (uint64_t *) (uintptr_t) table_pointer;
  } else {
    /* allocate new page table and set block */
    rtems_status_code sc = aarch64_mmu_page_table_alloc( sub_table );

    if ( sc != RTEMS_SUCCESSFUL ) {
      return sc;
    }

    aarch64_mmu_page_table_set_blocks(
      *sub_table,
      physical_root_address,
      shift - MMU_BITS_PER_LEVEL,
      *page_table_entry & ~MMU_DESC_PAGE_TABLE_MASK
    );
    *page_table_entry = (uintptr_t) *sub_table;
    *page_table_entry |= MMU_DESC_TYPE_TABLE | MMU_DESC_VALID;
  }

  return RTEMS_SUCCESSFUL;
}

BSP_START_TEXT_SECTION static inline rtems_status_code aarch64_mmu_map_block(
  uint64_t *page_table,
  uintptr_t root_address,
  uintptr_t addr,
  uint64_t size,
  uint32_t level,
  uint64_t flags
)
{
  uint32_t shift = ( 2 - level ) * MMU_BITS_PER_LEVEL + MMU_PAGE_BITS;
  uintptr_t granularity = 1 << shift;
  uint64_t page_flag = 0;

  if ( level == 2 ) {
    page_flag = MMU_DESC_TYPE_PAGE;
  }

  while ( size > 0 ) {
    uintptr_t index = aarch64_mmu_get_index( root_address, addr, shift );
    uintptr_t block_bottom = RTEMS_ALIGN_DOWN( addr, granularity );
    uint64_t chunk_size = granularity;

    /* check for perfect block match */
    if ( block_bottom == addr ) {
      if ( size >= chunk_size ) {
        /* when page_flag is set the last level must be a page descriptor */
        if ( page_flag || ( page_table[index] & MMU_DESC_TYPE_TABLE ) != MMU_DESC_TYPE_TABLE ) {
          /* no sub-table, apply block properties */
          page_table[index] = addr | flags | page_flag;
          size -= chunk_size;
          addr += chunk_size;
          continue;
        }
      } else {
        /* block starts on a boundary, but is short */
        chunk_size = size;
      }
    } else {
      uintptr_t block_top = RTEMS_ALIGN_UP( addr, granularity );
      chunk_size = block_top - addr;

      if ( chunk_size > size ) {
        chunk_size = size;
      }
    }

    /* Deal with any subtable modification  */
    uintptr_t new_root_address = root_address + index * granularity;
    uint64_t *sub_table = NULL;
    rtems_status_code sc;

    sc = aarch64_mmu_get_sub_table(
      &page_table[index],
      &sub_table,
      new_root_address,
      shift
    );

    if ( sc != RTEMS_SUCCESSFUL ) {
      return sc;
    }

    sc = aarch64_mmu_map_block(
      sub_table,
      new_root_address,
      addr,
      chunk_size,
      level + 1,
      flags
    );

    if ( sc != RTEMS_SUCCESSFUL ) {
      return sc;
    }

    size -= chunk_size;
    addr += chunk_size;
  }

  return RTEMS_SUCCESSFUL;
}

BSP_START_DATA_SECTION extern const aarch64_mmu_config_entry
  aarch64_mmu_config_table[];

BSP_START_DATA_SECTION extern const size_t
  aarch64_mmu_config_table_size;

BSP_START_TEXT_SECTION static inline void
aarch64_mmu_set_translation_table_entries(
  uint64_t *ttb,
  const aarch64_mmu_config_entry *config
)
{
  /* Force alignemnt to 4k page size */
  uintptr_t begin = RTEMS_ALIGN_DOWN( config->begin, MMU_PAGE_SIZE );
  uintptr_t end = RTEMS_ALIGN_UP( config->end, MMU_PAGE_SIZE );
  rtems_status_code sc;

  sc = aarch64_mmu_map_block(
    ttb,
    0x0,
    begin,
    end - begin,
    0,
    config->flags
  );

  if ( sc != RTEMS_SUCCESSFUL ) {
    rtems_fatal_error_occurred( sc );
  }
}

BSP_START_TEXT_SECTION static inline void aarch64_mmu_setup_translation_table(
  const aarch64_mmu_config_entry *config_table,
  size_t config_count
)
{
  size_t i;
  uint64_t *ttb = (uint64_t *) bsp_translation_table_base;

  aarch64_mmu_page_table_set_blocks(
    ttb,
    (uintptr_t) NULL,
    MMU_TOP_LEVEL_PAGE_BITS,
    0
  );

  _AArch64_Write_ttbr0_el1( (uintptr_t) ttb );

  /* Configure entries required for each memory section */
  for ( i = 0; i < config_count; ++i ) {
    aarch64_mmu_set_translation_table_entries( ttb, &config_table[i] );
  }
}

BSP_START_TEXT_SECTION static inline void
aarch64_mmu_setup_translation_table_and_enable(
  const aarch64_mmu_config_entry *config_table,
  size_t config_count
)
{
  uint64_t sctlr;

  aarch64_mmu_setup_translation_table(
    config_table,
    config_count
  );

  /* Enable MMU and cache */
  sctlr = _AArch64_Read_sctlr_el1();
  sctlr |= AARCH64_SCTLR_EL1_I | AARCH64_SCTLR_EL1_C | AARCH64_SCTLR_EL1_M;
  _AArch64_Write_sctlr_el1( sctlr );
}

BSP_START_TEXT_SECTION static inline void aarch64_mmu_setup( void )
{
  /* Set TCR */
  /* 128GB/36 bits mappable (64-0x1c) */
  _AArch64_Write_tcr_el1(
    AARCH64_TCR_EL1_T0SZ( 0x1c ) | AARCH64_TCR_EL1_IRGN0( 0x1 ) |
    AARCH64_TCR_EL1_ORGN0( 0x1 ) | AARCH64_TCR_EL1_SH0( 0x3 ) | AARCH64_TCR_EL1_TG0( 0x0 )
  );

  /* Set MAIR */
  _AArch64_Write_mair_el1(
    AARCH64_MAIR_EL1_ATTR0( 0x0 ) | AARCH64_MAIR_EL1_ATTR1( 0x4 ) |
    AARCH64_MAIR_EL1_ATTR2( 0x44 ) | AARCH64_MAIR_EL1_ATTR3( 0xFF )
  );
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_AARCH64_SHARED_AARCH64_MMU_H */
