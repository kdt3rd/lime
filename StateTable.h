/// @file StateTable.h
/// @brief File that declares a table to store and find states
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

#ifndef _StateTable_h_
#define _StateTable_h_

#include <iosfwd>

class Config;
class State;


////////////////////////////////////////


class StateTable
{
public:
	StateTable( void );
	~StateTable( void );
	
	State *find( const Config *b ) const;
	bool add( State *newState );
	
	size_t getNumStates( void ) const;
	State *getNthState( size_t i ) const;
	
	void print( std::ostream &out, bool basisOnly ) const;
	
	static StateTable *get( void );
};

#endif /* _StateTable_h_ */

