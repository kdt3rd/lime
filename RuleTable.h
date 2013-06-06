/// @file RuleTable.h
/// @brief File that declares a class to store and manipulate a list of rules.
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

#ifndef _RuleTable_h_
#define _RuleTable_h_

#include <map>
#include <vector>
#include <string>
#include <iosfwd>


////////////////////////////////////////


class Rule;

class RuleTable
{
public:
	RuleTable( void );
	~RuleTable( void );
	
	/// Creates a new, empty rule. Will auto match it with other
	/// existing rules with the same lhs and add them into a chain.
	Rule *createNewRule( const std::string &lhs );
	
	/// Get a rule based on it's index
	size_t getNumRules( void );
	Rule *getNthRule( size_t i );
	
	Rule *findFirstRule( const std::string &lhs );
	/// Retrieves the next rule with the same lhs. (might be null)
	Rule *getNextRule( const Rule *rule );
	
	// Manipulators to intermesh the rule table
	
	/// Find the precedence for every production rule (that has one)
	void findPrecedences( void );
	void computeLambdas( void );
	void computeFirstSets( void );
	bool isOnRightSide( const std::string &lhs );
	
	void print( std::ostream &out ) const;
	
	static RuleTable *get( void );
	
private:
	typedef std::vector< Rule * >		RuleList;
	typedef RuleList::iterator			RuleListIter;
	typedef RuleList::const_iterator	RuleListConstIter;
	
	typedef std::map< const std::string, RuleList >	RuleMap;
	typedef RuleMap::iterator						RuleMapIter;
	typedef RuleMap::const_iterator					RuleMapConstIter;

	RuleList	myRuleList;
	RuleMap		myRules;
};

#endif /* _RuleTable_h_ */

