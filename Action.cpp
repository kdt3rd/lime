/// @file Action.cpp
/// @brief File that implements an action structure.
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
#include <stdexcept>
#include <iomanip>

#include "Action.h"
#include "SymbolTable.h"

#include "State.h"
#include "Rule.h"

////////////////////////////////////////


Action::Action( Type t )
		: myType( t ), myState( 0 ), myRule( 0 )
{
}


////////////////////////////////////////


Action::Action( Type t, const std::string &la, State *stp, Rule *rlp )
		: myType( t ), myLookAhead( la ), myState( stp ), myRule( rlp )
{
}


////////////////////////////////////////


Action::Action( const Action &other )
		: myType( other.myType ), myLookAhead( other.myLookAhead ),
		  myState( other.myState ), myRule( other.myRule )
{
}


////////////////////////////////////////


Action::~Action( void )
{
	myState = 0;
	myRule = 0;
}


////////////////////////////////////////


Action &
Action::operator=( const Action &other )
{
	myType = other.myType;
	myLookAhead = other.myLookAhead;
	myState = other.myState;
	myRule = other.myRule;
	
	return *this;
}


////////////////////////////////////////


bool
Action::operator<( const Action &other ) const
{
	bool retval = false;
	
	if ( myLookAhead == other.myLookAhead )
	{
		if ( int( myType ) == int( other.myType ) )
		{
			if ( myRule )
				retval = myRule->getRuleIndex() < other.myRule->getRuleIndex();
			else if ( myState )
				retval = myState->getStateIndex() < other.myState->getStateIndex();
		}
		else if ( int( myType ) < int( other.myType ) )
			retval = true;
	}
	else if ( myLookAhead < other.myLookAhead )
		retval = true;
	
	return retval;
}


////////////////////////////////////////


void
Action::setType( Action::Type t )
{
	myType = t;
}

	
////////////////////////////////////////


Action::Type
Action::getType( void ) const
{
	return myType;
}


////////////////////////////////////////


bool
Action::isIgnoreType( void ) const
{
	bool retval = true;
	
	switch ( getType() )
	{
		case Action::SHIFT:
		case Action::REDUCE:
		case Action::ACCEPT:
		case Action::ERROR:
			retval = false;
			break;
		default:
			break;
	}
	
	return retval;
}


////////////////////////////////////////


void
Action::setLookAhead( const std::string &la )
{
	myLookAhead = la;
}


////////////////////////////////////////


const std::string &
Action::getLookAhead( void ) const
{
	return myLookAhead;
}


////////////////////////////////////////


Symbol *
Action::getLookAheadSymbol( void ) const
{
	return SymbolTable::get()->find( myLookAhead );
}


////////////////////////////////////////


void
Action::setState( State *stp )
{
	myState = stp;
}


////////////////////////////////////////


State *
Action::getState( void ) const
{
	return myType == SHIFT ? myState : 0;
}


////////////////////////////////////////


void
Action::setRule( Rule *rlp )
{
	myRule = rlp;
}


////////////////////////////////////////


Rule *
Action::getRule( void ) const
{
	return myType == REDUCE ? myRule : 0;
}


////////////////////////////////////////


bool
Action::print( std::ostream &out ) const
{
	bool retval = true;
	
	switch ( myType )
	{
		case SHIFT:
			out << std::setw(25) << myLookAhead << std::setw(0);
			out << " SHIFT  " << myState->getStateIndex();
			break;
			
		case REDUCE:
			out << std::setw(25) << myLookAhead << std::setw(0);
			out << " REDUCE " << myRule->getLHS() << " ("
				<< myRule->getRuleIndex() << ")";
			break;
			
		case ACCEPT:
			out << std::setw(25) << myLookAhead << std::setw(0);
			out << " ACCEPT";
			break;
			
		case ERROR:
			out << std::setw(25) << myLookAhead << std::setw(0);
			out << " ERROR";
			break;
			
		case CONFLICT:
			out << std::setw(25) << myLookAhead << std::setw(0);
			out << " REDUCE " << myRule->getLHS() << " ("
				<< myRule->getRuleIndex() << ") ** PARSING CONFLICT **";
			break;
			
		case SH_RESOLVED:
		case RD_RESOLVED:
		case NOT_USED:
		default:
			retval = false;
			break;
	}
	
	return retval;
}

