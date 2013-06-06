/// @file State.cpp
/// @brief File that declares...
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

#include <ostream>
#include <iostream>

#include "State.h"

#include "Config.h"


////////////////////////////////////////


State::State( Config *basis, Config *config )
		: myBasis( basis ), myConfig( config )
{
	static int nextStateIDX = 0;
	
	myStateIndex = nextStateIDX++;
}


////////////////////////////////////////


State::~State( void )
{
}


////////////////////////////////////////


Config *
State::getBasis( void ) const
{
	return myBasis;
}


////////////////////////////////////////


Config *
State::getConfig( void ) const
{
	return myConfig;
}


////////////////////////////////////////


int
State::getStateIndex( void ) const
{
	return myStateIndex;
}


////////////////////////////////////////


void
State::addAction( Action::Type		 t,
				  const std::string	&lookAhead,
				  State				*stp,
				  Rule				*rule )
{
	myActions.addAction( t, lookAhead, stp, rule );
}


////////////////////////////////////////


void
State::sortActions( void )
{
	myActions.sort();
}


////////////////////////////////////////


ActionList &
State::getActions( void )
{
	return myActions;
}


////////////////////////////////////////


const ActionList &
State::getActions( void ) const
{
	return myActions;
}


////////////////////////////////////////


void
State::compress( void )
{
	myActions.compress();
}


////////////////////////////////////////


void
State::print( std::ostream &out, bool basisOnly ) const
{
	Config *cfp;
	
	out << "State " << getStateIndex() << ":\n";
	
	if ( basisOnly )
		cfp = getBasis();
	else
		cfp = getConfig();
			
	while ( cfp )
	{
		cfp->print( out );
		
		out << std::endl;
				
		if ( basisOnly )
			cfp = cfp->getNextBasis();
		else
			cfp = cfp->getNext();
	}
	
	out << std::endl;
	
	out << "  Actions:" << std::endl;
	for ( size_t i = 0; i < myActions.getNumActions(); ++i )
	{
		const Action &act = myActions.getNthAction( i );
		if ( act.print( out ) )
			 out << std::endl;
	}
}

