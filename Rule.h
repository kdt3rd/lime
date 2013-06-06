/// @file Rule.h
/// @brief File that declares a production rule.
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

#ifndef _Rule_h_
#define _Rule_h_

#include <iosfwd>
#include <vector>
#include <string>


////////////////////////////////////////


class Symbol;

class Rule
{
public:
	typedef std::pair< std::string, std::string > RHSEntry;
	typedef std::vector< RHSEntry > RHSList;
	typedef RHSList::iterator RHSListIter;
	typedef RHSList::const_iterator RHSListConstIter;

	Rule( const std::string &lhs, int ruleIndex );
	~Rule( void );
	
	/// The left hand side for the rule
	inline const std::string &getLHS( void ) const { return myLHS; }
	Symbol *getLHSSymbol( void ) const;
	
	/// Rule index for use during rule production analysis.
	inline int getRuleIndex( void ) const { return myRuleIndex; }
	
	/// Alias for left hand side.
	void setLHSAlias( const std::string &lhsalias );
	inline const std::string &getLHSAlias( void ) const { return myLHSAlias; }
	
	/// Line number for the rule.
	void setRuleLine( int line );
	inline int getRuleLine( void ) const { return myRuleLine; }
	
	/// The RHS symbols.
	void setRHS( const RHSList &rhs );
	inline const RHSList &getRHS( void ) const { return myRHSList; }
	
	/// The code to run when the rule is reduced.
	void setCode( int codeline, const std::string &code );
	inline int getCodeLine( void ) const { return myCodeLine; }
	inline const std::string &getCode( void ) const { return myCode; }
	
	// The precedence symbol for this rule
	void setPrecedence( const std::string &precSym );
	inline const std::string &getPrecedence( void ) const { return myPrecedence;}
	Symbol *getPrecedenceSymbol( void ) const;
	
	
	void setCanReduce( bool on_off );
	inline bool canReduce( void ) const { return myCanReduce; }
	
	void print( std::ostream &out ) const;
	
private:
	Rule( const Rule &other );
	Rule &operator=( const Rule &other );
	
	int			 myRuleIndex;
	std::string	 myLHS;
	std::string	 myLHSAlias;
	int			 myRuleLine;
	RHSList		 myRHSList;
	int			 myCodeLine;
	std::string	 myCode;
	
	std::string	 myPrecedence;
	
	bool		 myCanReduce;
};

#endif /* _Rule_h_ */

