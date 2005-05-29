/***************************************************************************
                              tms9914/read.c
                             -------------------

    begin                : Dec 2001
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

#include "board.h"
#include <linux/spinlock.h>

static void check_for_eos( tms9914_private_t *priv, uint8_t byte )
{
	if( ( priv->eos_flags & REOS ) == 0 ) return;

	if( priv->eos_flags & BIN )
	{
		if( priv->eos == byte )
			set_bit( RECEIVED_END_BN, &priv->state );
	}else
	{
		if( ( priv->eos & 0x7f ) == ( byte & 0x7f ) )
			set_bit( RECEIVED_END_BN, &priv->state );
	}
}

static int wait_for_read_byte(gpib_board_t *board, tms9914_private_t *priv)
{
	if(wait_event_interruptible(board->wait,
		test_bit(READ_READY_BN, &priv->state) ||
		test_bit(DEV_CLEAR_BN, &priv->state) ||
		test_bit(TIMO_NUM, &board->status)))
	{
		printk("gpib: pio read wait interrupted\n");
		return -ERESTARTSYS;
	};
	if( test_bit( TIMO_NUM, &board->status ) )
	{
		return -ETIMEDOUT;
	}
	if( test_bit( DEV_CLEAR_BN, &priv->state ) )
	{
		return -EINTR;
	}
	return 0;
}

static ssize_t pio_read(gpib_board_t *board, tms9914_private_t *priv, uint8_t *buffer, size_t length, int *end, int *nbytes)
{
	ssize_t retval = 0;
	unsigned long flags;

	*nbytes = 0;
	*end = 0;
	while(*nbytes < length)
	{
		tms9914_release_holdoff(board, priv);
		retval = wait_for_read_byte(board, priv);
		if(retval < 0) return retval;;
		spin_lock_irqsave( &board->spinlock, flags );
		clear_bit( READ_READY_BN, &priv->state );
		buffer[ (*nbytes)++ ] = read_byte( priv, DIR );
		spin_unlock_irqrestore( &board->spinlock, flags );

		check_for_eos( priv, buffer[ *nbytes - 1 ] );

		if(test_and_clear_bit( RECEIVED_END_BN, &priv->state ) )
		{
			*end = 1;
			break;
		}
	}

	return retval;
}

ssize_t tms9914_read(gpib_board_t *board, tms9914_private_t *priv, uint8_t *buffer, size_t length, int *end, int *nbytes)
{
	ssize_t retval = 0;
	int bytes_read;
	
	*end = 0;
	*nbytes = 0;
	if(length == 0) return 0;

	clear_bit( DEV_CLEAR_BN, &priv->state );

	if(priv->holdoff_active == 0)
	{
		retval = wait_for_read_byte(board, priv);
		if(retval < 0) return retval;
	}
	if( priv->eos_flags & REOS )
		tms9914_set_holdoff_mode(board, priv, TMS9914_HOLDOFF_ALL);
	else
		tms9914_set_holdoff_mode(board, priv, TMS9914_HOLDOFF_EOI);
	// transfer data (except for last byte)
	if(length > 1)
	{
		// PIO transfer
		retval = pio_read(board, priv, buffer, length - 1, end, &bytes_read);
		*nbytes += bytes_read;
		if(retval < 0)
			return retval;
	}
	// read last byte if we havn't received an END yet
	if(*end == 0)
	{
		// make sure we holdoff after last byte read
		tms9914_set_holdoff_mode(board, priv, TMS9914_HOLDOFF_ALL);
		retval = pio_read(board, priv, &buffer[*nbytes], 1, end, &bytes_read);
		*nbytes += bytes_read;
		if(retval < 0)
			return retval;
	}
	return 0;
}

EXPORT_SYMBOL(tms9914_read);






