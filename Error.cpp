/// @file Error.cpp
/// @brief File that implements a basic error class.
///
///

//
// 
// Copyright © 2003-2013 Kimball Thurston
// 
// This program is free software; you can redisribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
//

#include <cstdio>
#include <iostream>
#include <string>

#include "Error.h"


////////////////////////////////////////


Error::Error( void )
		: myCount( 0 )
{
}


////////////////////////////////////////


Error::~Error( void )
{
}


////////////////////////////////////////


void
Error::setSourceFile( const char *srcFile )
{
	mySourceFile = srcFile;
}


////////////////////////////////////////


void
Error::add( const char *fmt, ... )
{
	va_list fmtArgs;

	va_start( fmtArgs, fmt );
	
	std::cerr << mySourceFile << ": ";
	char errBuf[4096];
	std::vsnprintf( errBuf, 4096, fmt, fmtArgs );
	std::cerr << errBuf << std::endl;
	
	++myCount;

	va_end( fmtArgs );
}


////////////////////////////////////////


void
Error::add( int lineno, const char *fmt, ... )
{
	va_list fmtArgs;

	va_start( fmtArgs, fmt );
	
	std::cerr << mySourceFile << " (line " << lineno << "): ";
	char errBuf[4096];
	std::vsnprintf( errBuf, 4096, fmt, fmtArgs );
	std::cerr << errBuf << std::endl;
	
	++myCount;

	va_end( fmtArgs );
}


////////////////////////////////////////


int
Error::getCount( void )
{
	return myCount;
}


////////////////////////////////////////


static Error *theError = 0;

Error *
Error::get( void )
{
	if ( ! theError )
		theError = new Error;
	
	return theError;
}
