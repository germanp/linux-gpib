/***************************************************************************
                          gpibP.h
                             -------------------

    copyright            : (C) 2002,2003 by Frank Mori Hess
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

#ifndef _GPIB_P_H
#define _GPIB_P_H

#include <linux/types.h>

#include "gpib_user.h"
#include "gpib_types.h"
#include "gpib_proto.h"
#include "gpib_ioctl.h"
#include "config.h"

#include <linux/fs.h>

void gpib_register_driver(gpib_interface_t *interface);
void gpib_unregister_driver(gpib_interface_t *interface);
struct pci_dev* gpib_pci_find_device( const gpib_board_t *board, unsigned int vendor_id,
	unsigned int device_id, const struct pci_dev *from);
unsigned int num_gpib_events( const gpib_event_queue_t *queue );
int push_gpib_event( gpib_board_t *board, short event_type );
int pop_gpib_event( gpib_event_queue_t *queue, short *event_type );

extern gpib_board_t board_array[GPIB_MAX_NUM_BOARDS];

extern struct list_head registered_drivers;

#if defined( GPIB_CONFIG_KERNEL_DEBUG )
#define GPIB_DPRINTK( format, args... ) printk( "gpib debug: " format, ## args )
#else
#define GPIB_DPRINTK( arg... )
#endif

#include <asm/io.h>

void writeb_wrapper( unsigned int value, unsigned long address );
unsigned int readb_wrapper( unsigned long address );
void outb_wrapper( unsigned int value, unsigned long address );
unsigned int inb_wrapper( unsigned long address );
void writew_wrapper( unsigned int value, unsigned long address );
unsigned int readw_wrapper( unsigned long address );
void outw_wrapper( unsigned int value, unsigned long address );
unsigned int inw_wrapper( unsigned long address );

#endif	// _GPIB_P_H

