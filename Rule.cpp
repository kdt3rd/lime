/// @file Rule.cpp
/// @brief File that implements the basic rule class.
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

#include "Rule.h"

#include "Symbol.h"
#include "SymbolTable.h"


////////////////////////////////////////


Rule::Rule( const std::string &lhs, int ruleIndex )
		: myRuleIndex( ruleIndex ), myLHS( lhs ), myRuleLine( 0 ),
		  myCodeLine( 0 ), myCanReduce( false )
{
}


////////////////////////////////////////


Rule::~Rule( void )
{
}


////////////////////////////////////////


Symbol *
Rule::getLHSSymbol( void ) const
{
	return SymbolTable::get()->find( myLHS );
}


////////////////////////////////////////


void
Rule::setLHSAlias( const std::string &lhsalias )
{
	myLHSAlias = lhsalias;
}


////////////////////////////////////////


void
Rule::setRuleLine( int line )
{
	myRuleLine = line;
}


////////////////////////////////////////


void
Rule::setRHS( const RHSList &rhs )
{
	myRHSList = rhs;
}


////////////////////////////////////////


void
Rule::setCode( int codeline, const std::string &code )
{
	myCodeLine = codeline;
	myCode = code;
}


////////////////////////////////////////


void
Rule::setPrecedence( const std::string &precSym )
{
	myPrecedence = precSym;
}


////////////////////////////////////////


Symbol *
Rule::getPrecedenceSymbol( void ) const
{
	return myPrecedence.empty() ? 0 : SymbolTable::get()->find( myPrecedence );
}


////////////////////////////////////////


void
Rule::setCanReduce( bool on_off )
{
	myCanReduce = on_off;
}


////////////////////////////////////////


void
Rule::print( std::ostream &out ) const
{
	out << getLHS() << " ::=";
		
	Rule::RHSListConstIter ri, re;
	re = myRHSList.end();
	for ( ri = myRHSList.begin(); ri != re; ++ri )
		out << " " << (*ri).first;
}
