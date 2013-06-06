/// @file FollowSet.h
/// @brief File that declares a follow set.
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

#ifndef _FollowSet_h_
#define _FollowSet_h_

#include <iosfwd>
#include <string>
#include <map>


////////////////////////////////////////


class FollowSet
{
public:
	typedef std::map< std::string, int >	SetMap;
	typedef SetMap::iterator				SetMapIter;
	typedef SetMap::const_iterator			SetMapConstIter;
	
	FollowSet( void );
	FollowSet( const FollowSet &other );
	
	~FollowSet( void );
	
	FollowSet &operator=( const FollowSet &other );
	
	// Returns true if actually changes
	bool add( const std::string &fs );
	// Returns true if actually changes
	bool combine( const FollowSet &other );
	
	bool isSet( const std::string &fs ) const;
	
	void print( std::ostream &out ) const;
	
private:
	SetMap mySet;
};

#endif /* _FollowSet_h_ */

