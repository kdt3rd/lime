/// @file ActionList.h
/// @brief File that declares a list of actions (and some operations)
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

#ifndef _ActionList_h_
#define _ActionList_h_

#include <vector>

#include "Action.h"


////////////////////////////////////////


class ActionList
{
public:
	ActionList( void );
	ActionList( const ActionList &other );
	~ActionList( void );
	
	void addAction( Action::Type		 type,
					const std::string	&lookAhead,
					State				*state,
					Rule				*rule );
	
	size_t getNumActions( void ) const;
	Action &getNthAction( size_t i );
	const Action &getNthAction( size_t i ) const;
	
	void sort( void );
	void compress( void );
	
private:
	typedef std::vector<Action>		List;
	typedef List::iterator			ListIter;
	typedef List::const_iterator	ListConstIter;
	
	List myActions;
};

#endif /* _ActionList_h_ */

