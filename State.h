/// @file State.h
/// @brief File that declares a class to record state transitions.
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

#ifndef _State_h_
#define _State_h_

#include <iosfwd>
#include "ActionList.h"

class Config;


////////////////////////////////////////


class State
{
public:
	State( Config *basis, Config *config );
	~State( void );
	
	Config *getBasis( void ) const;
	Config *getConfig( void ) const;
	
	// Globally sequential number for this state.
	int getStateIndex( void ) const;
	
	void addAction( Action::Type		 type,
					const std::string	&lookAhead,
					State				*state,
					Rule				*rule );
	void sortActions( void );
	ActionList &getActions( void );
	const ActionList &getActions( void ) const;
	
	void compress( void );
	
	void print( std::ostream &out, bool basisOnly ) const;
	
private:
	Config		*myBasis;		// Basis Configurations for this state
	Config		*myConfig;		// All Configurations for this set
	int			 myStateIndex;	// Sequential number for this state (auto)
	ActionList	 myActions;		// Array of actions for this state
	//int tabstart;            /* First index of the action table */
	//int tabdfltact;          /* Default action */
};

#endif /* _State_h_ */

