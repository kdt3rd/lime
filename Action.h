/// @file Action.h
/// @brief File that declares an action structure.
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

#ifndef _Action_h_
#define _Action_h_

#include <iosfwd>
#include <string>

class Symbol;
class Rule;
class State;


////////////////////////////////////////


class Action
{
public:
	enum Type
	{
		SHIFT,
		ACCEPT,
		REDUCE,
		ERROR,
		CONFLICT,
		SH_RESOLVED,
		RD_RESOLVED,
		NOT_USED
	};
	
	Action( Type t );
	Action( Type t, const std::string &la, State *stp, Rule *rlp );
	Action( const Action &other );
	~Action( void );
	
	Action &operator=( const Action &other );
	bool operator<( const Action &other ) const;
	
	void setType( Type t );
	Type getType( void ) const;
	/// Is it a type to ignore during code gen...
	bool isIgnoreType( void ) const;
	
	/// Set the look-ahead symbol
	void setLookAhead( const std::string &la );
	const std::string &getLookAhead( void ) const;
	Symbol *getLookAheadSymbol( void ) const;
	
	/// New state, if a shift
	void setState( State *stp );
	State *getState( void ) const;
	
	/// new rule if a reduce
	void setRule( Rule *rlp );
	Rule *getRule( void ) const;
	
	// returns true if prints something
	bool print( std::ostream &out ) const;
	
private:
	Type myType;
	
	std::string myLookAhead;
	State *myState; // only if a shift
	Rule *myRule; // only if a reduce
};

#endif /* _Action_h_ */

