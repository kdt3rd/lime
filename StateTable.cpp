/// @file StateTable.cpp
/// @brief File that implements a state table.
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

#include <map>
#include <vector>
#include <ostream>

#include "StateTable.h"

#include "State.h"
#include "Config.h"
#include "Rule.h"


////////////////////////////////////////


typedef std::vector< State * >		StateList;
typedef StateList::iterator			StateListIter;
typedef StateList::const_iterator	StateListConstIter;

static StateList theStateList;

static StateTable *theStateTable = 0;


////////////////////////////////////////


StateTable::StateTable( void )
{
}


////////////////////////////////////////


StateTable::~StateTable( void )
{
}


////////////////////////////////////////


State *
StateTable::find( const Config *bp ) const
{
	StateListConstIter i, e;
	State	*retval = 0;

	if ( bp == NULL )
		return NULL;

	e = theStateList.end();
	for ( i = theStateList.begin(); ! retval && i != e; ++i )
	{
		const Config *sbp = (*i)->getBasis();
		const Config *tbp = bp;
		
		bool same = true;
		
		while ( tbp && sbp )
		{
			if ( ( tbp->getRule()->getRuleIndex() !=
				   sbp->getRule()->getRuleIndex() ) ||
				 ( tbp->getDot() != sbp->getDot() ) )
			{
				same = false;
				break;
			}
			
			tbp = tbp->getNextBasis();
			sbp = sbp->getNextBasis();
		}

		if ( ( tbp && ! sbp ) || ( ! tbp && sbp ) )
			same = false;

		if ( same )
		{
			retval = (*i);
			break;
		}
	}
	
	return retval;
}


////////////////////////////////////////


bool
StateTable::add( State *newState )
{
	if ( ! newState )
		return false;

	if ( find( newState->getBasis() ) )
		return false;

	theStateList.push_back( newState );
//	privStateKey key = privStateKey( newState->getBasis() );
//	
//	theStateMap[key] = newState;
	return true;
}


////////////////////////////////////////


size_t
StateTable::getNumStates( void ) const
{
	return theStateList.size();
}


////////////////////////////////////////


State *
StateTable::getNthState( size_t i ) const
{
	return i < theStateList.size() ? theStateList[i] : 0;
}


////////////////////////////////////////


void
StateTable::print( std::ostream &out, bool basisOnly ) const
{
	StateListConstIter i, e;
	
	e = theStateList.end();
	for ( i = theStateList.begin(); i != e; ++i )
	{
		(*i)->print( out, basisOnly );
		out << std::endl;
	}
}


////////////////////////////////////////


StateTable *
StateTable::get( void )
{
	if ( ! theStateTable )
		theStateTable = new StateTable;
	
	return theStateTable;
}

	
