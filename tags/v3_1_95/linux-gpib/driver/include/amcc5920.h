/***************************************************************************
                          amcc5920.h  -  description
                             -------------------
  Header for amcc5920 pci chip

    copyright            : (C) 2002 by Frank Mori Hess
    email                : fmhess@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _AMCC5920_GPIB_H
#define _AMCC5920_GPIB_H

// plx pci chip registers and bits
enum amcc_registers
{
	AMCC_INTCS_REG = 0x38,
	AMCC_PASS_THRU_REG	= 0x60,
};

enum amcc_incsr_bits
{
	AMCC_ADDON_INTR_ENABLE_BIT = 0x2000,
	AMCC_ADDON_INTR_ACTIVE_BIT = 0x400000,
	AMCC_INTR_ACTIVE_BIT = 0x800000,
};

static const int bits_per_region = 8;

extern inline uint32_t amcc_wait_state_bits(unsigned int region, unsigned int num_wait_states)
{
	return (num_wait_states & 0x7) << ( --region * bits_per_region );
};

enum amcc_prefetch_bits
{
	PREFETCH_DISABLED = 0x0,
	PREFETCH_SMALL = 0x8,
	PREFETCH_MEDIUM = 0x10,
	PREFETCH_LARGE = 0x18,
};

extern inline uint32_t amcc_prefetch_bits(unsigned int region, enum amcc_prefetch_bits prefetch)
{
	return prefetch << ( --region * bits_per_region );
};

extern inline uint32_t amcc_PTADR_mode_bit(unsigned int region)
{
	return 0x80 << ( --region * bits_per_region);
};

extern inline uint32_t amcc_disable_write_fifo_bit(unsigned int region)
{
	return 0x20 << ( --region * bits_per_region);
};

#endif	// _AMCC5920_GPIB_H
