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

void check_for_eos( tms9914_private_t *priv, uint8_t byte )
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

	if( test_bit( RECEIVED_END_BN, &priv->state ) == 0 )
		write_byte( priv, AUX_RHDF, AUXCR );
}

static ssize_t pio_read(gpib_board_t *board, tms9914_private_t *priv, uint8_t *buffer, size_t length)
{
	size_t count = 0;
	ssize_t retval = 0;
	unsigned long flags;

	while(count < length)
	{
		if(wait_event_interruptible(board->wait,
			test_bit(READ_READY_BN, &priv->state) ||
			test_bit(TIMO_NUM, &board->status)))
		{
			printk("gpib: pio read wait interrupted\n");
			retval = -ERESTARTSYS;
			break;
		};
		if(test_bit(TIMO_NUM, &board->status))
		{
			retval = -ETIMEDOUT;
			break;
		}

		spin_lock_irqsave(&board->spinlock, flags);
		clear_bit(READ_READY_BN, &priv->state);
		buffer[ count++ ] = read_byte( priv, DIR );
		spin_unlock_irqrestore(&board->spinlock, flags);

		check_for_eos( priv, buffer[ count - 1 ] );

		if(test_bit(RECEIVED_END_BN, &priv->state))
			break;
	}

	return retval ? retval : count;
}

ssize_t tms9914_read(gpib_board_t *board, tms9914_private_t *priv, uint8_t *buffer, size_t length, int *end)
{
	size_t	count = 0;
	ssize_t retval = 0;

	*end = 0;

	if(length == 0) return 0;

	if( priv->eos_flags & REOS )
	{
		write_byte(priv, AUX_HLDA | AUX_CS, AUXCR);
		write_byte(priv, AUX_HLDE, AUXCR);
	}else
	{
		write_byte(priv, AUX_HLDA, AUXCR);
		write_byte(priv, AUX_HLDE | AUX_CS, AUXCR);
	}
	write_byte(priv, AUX_RHDF, AUXCR);

	// transfer data (except for last byte)
	length--;
	if(length)
	{
		// PIO transfer
		retval = pio_read(board, priv, buffer, length);
		if(retval < 0)
			return retval;
		else
			count += retval;
	}

	// read last byte if we havn't received an END yet
	if(test_bit(RECEIVED_END_BN, &priv->state) == 0)
	{
		// make sure we holdoff after last byte read
		write_byte(priv, AUX_HLDE, AUXCR);
		write_byte(priv, AUX_HLDA | AUX_CS, AUXCR);
		retval = pio_read(board, priv, &buffer[count], 1);
		if(retval < 0)
			return retval;
		else
			count++;
	}

	if(test_and_clear_bit(RECEIVED_END_BN, &priv->state))
		*end = 1;

	return count;
}

EXPORT_SYMBOL(tms9914_read);






