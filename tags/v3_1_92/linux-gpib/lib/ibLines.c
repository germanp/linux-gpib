/***************************************************************************
                          lib/ibLines.c
                             -------------------

    copyright            : (C) 2001,2002,2003 by Frank Mori Hess
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

#include "ib_internal.h"

int internal_iblines( ibConf_t *conf, short *line_status )
{
	int retval;
	ibBoard_t *board;

	if( conf->is_interface == 0 )
	{
		setIberr( EDVR );
		return -1;
	}

	board = interfaceBoard( conf );

	retval = ioctl( board->fileno, IBLINES, line_status );
	if( retval < 0 )
	{
		switch( errno )
		{
			default:
				setIbcnt( errno );
				setIberr( EDVR );
				break;
		}
		return -1;
	}
	return 0;
}

int iblines( int ud, short *line_status )
{
	ibConf_t *conf;
	int retval;

	conf = general_enter_library( ud, 1, 1 );
	if( conf == NULL )
		return exit_library( ud, 1 );

	retval = internal_iblines( conf, line_status );
	if( retval < 0 )
	{
		return exit_library( ud, 1 );
	}

	return exit_library( ud, 0 );
}

void TestSRQ( int boardID, short *result )
{
	short line_status;

	ibConf_t *conf;
	int retval;

	conf = enter_library( boardID );
	if( conf == NULL )
	{
		exit_library( boardID, 1 );
		return;
	}

	retval = internal_iblines( conf, &line_status );
	if( retval < 0 )
	{
		exit_library( boardID, 1 );
		return;
	}

	if( ( line_status & ValidSRQ ) == 0 )
	{
		setIberr( ECAP );
		exit_library( boardID, 1 );
		return;
	}

	if( line_status & BusSRQ )
	{
		*result = 1;
	}else
		*result = 0;


	exit_library( boardID, 0 );
}
