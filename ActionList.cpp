/// @file ActionList.cpp
/// @brief File that implements a list of actions and some operations.
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

#include <algorithm>

#include "ActionList.h"
#include "Action.h"
#include "SymbolTable.h"
#include "Symbol.h"


////////////////////////////////////////


ActionList::ActionList( void )
{
}


////////////////////////////////////////


ActionList::~ActionList( void )
{
}


////////////////////////////////////////


void
ActionList::addAction( Action::Type			 type,
					   const std::string	&lookAhead,
					   State				*state,
					   Rule					*rule )
{
	myActions.push_back( Action( type, lookAhead, state, rule ) );
}


////////////////////////////////////////


size_t
ActionList::getNumActions( void ) const
{
	return myActions.size();
}


////////////////////////////////////////


const Action &
ActionList::getNthAction( size_t i ) const
{
	return myActions[i];
}


////////////////////////////////////////


Action &
ActionList::getNthAction( size_t i )
{
	return myActions[i];
}


////////////////////////////////////////


void ActionList::sort( void )
{
	std::sort( myActions.begin(), myActions.end() );
}


////////////////////////////////////////


void ActionList::compress( void )
{
	ListIter i, e;
	
	i = myActions.begin();
	e = myActions.end();
	
	// Find the first REDUCE action
	for ( ; i != e; ++i )
	{
		if ( (*i).getType() == Action::REDUCE )
			break;
	}
	
	if ( i != e )
	{
		ListIter firstReduce = i;
		// Stash off the rule
		Rule *rule = (*i).getRule();

		// See if all other REDUCE actions use the same rule
		int cnt = 1;
		
		for ( ++i; i != e; ++i )
		{
			if ( (*i).getType() == Action::REDUCE )
			{
				if ( (*i).getRule() != rule )
					break;
				++cnt;
			}
		}
		
		if ( i == e && cnt > 1 )
		{
			// Combine all REDUCE actions into a single default
			Symbol *sym = SymbolTable::get()->findOrCreate(
				std::string( "{default}" ) );
			
			(*firstReduce).setLookAhead( sym->getName() );
			
			for ( i = firstReduce + 1; i != e; ++i )
			{
				if ( (*i).getType() == Action::REDUCE )
					(*i).setType( Action::NOT_USED );
			}
			
			sort();
		}
	}
}
