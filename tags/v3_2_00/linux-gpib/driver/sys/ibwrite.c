/***************************************************************************
                              ibwrite.c
                             -------------------

    copyright            : (C) 2001, 2002 by Frank Mori Hess
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

#include "gpibP.h"

/*
 * IBWRT
 * Write cnt bytes of data from buf to the GPIB.  The write
 * operation terminates only on I/O complete.
 *
 * NOTE:
 *      1.  Prior to beginning the write, the interface is
 *          placed in the controller standby state.
 *      2.  Prior to calling ibwrt, the intended devices as
 *          well as the interface board itself must be
 *          addressed by calling ibcmd.
 */
ssize_t ibwrt(gpib_board_t *board, uint8_t *buf, size_t cnt, int send_eoi)
{
	size_t bytes_sent = 0;
	ssize_t ret = 0;
	int retval;

	if( cnt == 0 )
	{
		printk( "gpib: ibwrt() called with zero length?\n" );
		return 0;
	}

	if( board->master )
	{
		retval = ibgts( board );
		if( retval < 0 ) return retval;
	}
	osStartTimer( board, board->usec_timeout );
	ret = board->interface->write(board, buf, cnt, send_eoi);
	if(ret < 0)
	{
		printk("gpib write error\n");
	}else
	{
		buf += ret;
		bytes_sent += ret;
	}

	if( io_timed_out( board ) )
		ret = -ETIMEDOUT;

	osRemoveTimer(board);

	if(ret < 0) return ret;

	return bytes_sent;
}

