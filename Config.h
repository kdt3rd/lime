/// @file Config.h
/// @brief File that declares a class to store config for production rules.
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

#ifndef _Config_h_
#define _Config_h_

#include <iosfwd>
#include <vector>

#include "FollowSet.h"

class Rule;
class State;


////////////////////////////////////////


/// A configuration is a production rule of the grammar together with
/// a mark (dot) showing how much of that rule has been processed so far.
/// Configurations also contain a follow-set which is a list of terminal
/// symbols which are allowed to immediately follow the end of the rule.
/// Every configuration is recorded as an instance of the following.
class Config
{
public:
	typedef std::vector< Config * > PropList;
	typedef PropList::iterator		PropListIter;
	typedef PropList::const_iterator PropListConstIter;
	
	enum Status
	{
		COMPLETE,
		INCOMPLETE
	};
	
	Config( Rule *rp, int pp );
	~Config( void );
	
	inline void setStatus( Status st );
	inline Status getStatus( void ) const;
		
	inline Rule *getRule( void ) const;
	inline int getDot( void ) const;
	
	// For chaining configs together.
	inline void setNext( Config *next );
	inline Config *getNext( void ) const;
	
	inline void setNextBasis( Config *next );
	inline Config *getNextBasis( void ) const;
	
	// for debugging mostly
	inline void setState( State *st );
	
	// returns true if anythings changes
	bool addFollowSet( const std::string &fsItem );
	// returns true if anythings changes
	bool combineFollowSet( const FollowSet &other );
	inline const FollowSet &getFollowSet( void ) const;
	
	void addForwardPropLink( Config *fpl );
	inline const PropList &getForwardPropLinks( void ) const;
	void addBackwardPropLink( Config *fpl );
	inline const PropList &getBackwardPropLinks( void ) const;
	
	void printForwardPropLinks( int stateIdx );
	
	void mergePropLinks( Config *other );
	
	void print( std::ostream &out ) const;
	
private:
	Rule	*myRule; // Rule which this config is based on
	int		 myDot; // parse point in rule
	
	Config	*myNext;
	Config	*myNextBasis;
	
	FollowSet myFollowSet; // follow set for this config only
	
	PropList myForwardProps; // forward propagation links
	PropList myBackwardProps; // backwards propagation links
	
	Status	 myStatus; // status used during followset and shift computations
	
	State *myState;
};

inline void Config::setStatus( Config::Status st ) { myStatus = st; }
inline Config::Status Config::getStatus( void ) const { return myStatus; }
inline Rule *Config::getRule( void ) const { return myRule; }
inline int Config::getDot( void ) const { return myDot; }
inline void Config::setNext( Config *next ) { myNext = next; }
inline Config *Config::getNext( void ) const { return myNext; }
inline void Config::setNextBasis( Config *next ) { myNextBasis = next; }
inline Config *Config::getNextBasis( void ) const { return myNextBasis; }
inline void Config::setState( State *st ) { myState = st; }

inline const FollowSet &Config::getFollowSet( void ) const
{
	return myFollowSet;
}
inline const Config::PropList &Config::getForwardPropLinks( void ) const
{
	return myForwardProps;
}
inline const Config::PropList &Config::getBackwardPropLinks( void ) const
{
	return myBackwardProps;
}

#endif /* _Config_h_ */

