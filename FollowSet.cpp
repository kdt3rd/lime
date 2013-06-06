/// @file FollowSet.cpp
/// @brief File that implements a follow set list.
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

#include "FollowSet.h"


////////////////////////////////////////


FollowSet::FollowSet( void )
{
}


////////////////////////////////////////


FollowSet::FollowSet( const FollowSet &other )
		: mySet( other.mySet )
{
}


////////////////////////////////////////


FollowSet::~FollowSet( void )
{
}


////////////////////////////////////////


FollowSet &
FollowSet::operator=( const FollowSet &other )
{
	if ( this != &other )
		mySet = other.mySet;
	return *this;
}
	

////////////////////////////////////////


bool
FollowSet::add( const std::string &fs )
{
	bool retval = false;
	
	if ( mySet.find( fs ) == mySet.end() )
	{
		mySet[fs] = 1;
		retval = true;
	}
	
	return retval;
}


////////////////////////////////////////


bool
FollowSet::combine( const FollowSet &other )
{
	SetMapConstIter i, e;
	bool			retval = false;
	
	i = other.mySet.begin();
	e = other.mySet.end();
	for ( ; i != e; ++i )
	{
		if ( mySet.find( (*i).first ) == mySet.end() )
		{
			mySet[(*i).first] = 1;
			retval = true;
		}
	}
	
	return retval;
}


////////////////////////////////////////


bool
FollowSet::isSet( const std::string &fs ) const
{
	return mySet.find( fs ) != mySet.end();
}


////////////////////////////////////////


void
FollowSet::print( std::ostream &out ) const
{
	SetMapConstIter i, e;
	
	out << "[";
	
	e = mySet.end();
	for ( i = mySet.begin(); i != e; ++i )
		out << " " << (*i).first;
	
	out << " ]";
}

