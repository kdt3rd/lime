/// @file ConfigList.h
/// @brief File that declares a table to store configs
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

#ifndef _ConfigList_h_
#define _ConfigList_h_

#include <vector>

class Rule;
class Config;


////////////////////////////////////////


class ConfigList
{
public:
	ConfigList( void );
	~ConfigList( void );
	
	Config *add( Rule *rp, int dot );
	Config *addWithBasis( Rule *rp, int dot );
	
	Config *getConfig( void );
	Config *getBasis( void );
	
	void	sort( void );
	void	sortBasis( void );
	
	void	computeClosure( void );

	void	deleteConfigs( void );
	
	void	resetPointers( void );
	void	reset( void );
private:
	Config *find( size_t ruleidx, int dot );
	
	typedef std::vector< Config * >	List;
	typedef List::iterator			ListIter;

	Config *myFront;
	Config *myLast;
	
	Config *myFrontBasis;
	Config *myLastBasis;
	
	List	myConfigs;
};

#endif /* _ConfigList_h_ */

