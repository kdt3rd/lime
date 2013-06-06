/// @file RuleTable.cpp
/// @brief File that implements a rule table
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

#include <map>
#include <ostream>

#include "RuleTable.h"
#include "Rule.h"
#include "SymbolTable.h"
#include "Symbol.h"

////////////////////////////////////////


RuleTable::RuleTable( void )
{
}


////////////////////////////////////////


RuleTable::~RuleTable( void )
{
	myRules.clear();
	while ( ! myRuleList.empty() )
	{
		delete myRuleList.back();
		myRuleList.pop_back();
	}
}


////////////////////////////////////////


Rule *
RuleTable::createNewRule( const std::string &lhs )
{
	RuleMapIter i;
	Rule *retval = new Rule( lhs, myRuleList.size() );
	
	i = myRules.find( lhs );
	if ( i == myRules.end() )
		myRules[lhs].insert( myRules[lhs].begin(), retval );
	else
		(*i).second.insert( (*i).second.begin(), retval );
	
	myRuleList.push_back( retval );
	
	return retval;
}


////////////////////////////////////////


size_t
RuleTable::getNumRules( void )
{
	return myRuleList.size();
}


////////////////////////////////////////


Rule *
RuleTable::getNthRule( size_t i )
{
	return i < myRuleList.size() ? myRuleList[i] : 0;
}


////////////////////////////////////////


Rule *
RuleTable::findFirstRule( const std::string &lhs )
{
	RuleMapIter	 i;
	Rule		*retval = 0;
	
	i = myRules.find( lhs );
	if ( i != myRules.end() )
	{
		retval = (*i).second.front();
	}
	
	return retval;
}


////////////////////////////////////////


Rule *
RuleTable::getNextRule( const Rule *rule )
{
	RuleMapIter	 i;
	Rule		*retval = 0;
	
	if ( ! rule )
		return retval;
	
	i = myRules.find( rule->getLHS() );
	if ( i != myRules.end() )
	{
		RuleListIter ri, re;
		
		ri = (*i).second.begin();
		re = (*i).second.end();
		
		for ( ; ri != re; ++ri )
		{
			if ( (*ri) == rule )
			{
				++ri;
				break;
			}
		}
		
		if ( ri != re )
			retval = (*ri);
	}
	
	return retval;
}


////////////////////////////////////////


void
RuleTable::findPrecedences( void )
{
	RuleListIter	i, e;
	
	i = myRuleList.begin();
	e = myRuleList.end();
	
	for ( ; i != e; ++i )
	{
		if ( (*i)->getPrecedence().empty() )
		{
			Rule::RHSListConstIter ri, re;
			ri = (*i)->getRHS().begin();
			re = (*i)->getRHS().end();
			
			for ( ; ri != re; ++ri )
			{
				Symbol *tmpSym = SymbolTable::get()->findOrCreate( (*ri).first );
				if ( tmpSym->getPrecedence() >= 0 )
				{
					(*i)->setPrecedence( (*ri).first );
					break;
				}
			}
		}
	}
}


////////////////////////////////////////


void
RuleTable::computeLambdas( void )
{
	RuleListIter	i, e;
	bool			progress;
	
	do
	{
		progress = false;
		
		i = myRuleList.begin();
		e = myRuleList.end();
		for ( ; i != e; ++i )
		{
			Symbol *lhsSym = (*i)->getLHSSymbol();
			
			if ( lhsSym->isLambda() )
				continue;
			
			Rule::RHSListConstIter ri, re;
			ri = (*i)->getRHS().begin();
			re = (*i)->getRHS().end();
			
			for ( ; ri != re; ++ri )
			{
				Symbol *tmpSym = SymbolTable::get()->findOrCreate( (*ri).first );
				if ( ! tmpSym->isLambda() )
					break;
			}
			
			if ( ri == re )
			{
				lhsSym->setLambda( true );
				progress = true;
			}
		}
	} while ( progress );
}


////////////////////////////////////////


void
RuleTable::computeFirstSets( void )
{
	RuleListIter	i, e;
	bool			progress;
	
	do
	{
		progress = false;
		
		i = myRuleList.begin();
		e = myRuleList.end();
		for ( ; i != e; ++i )
		{
			Symbol *lhsSym = (*i)->getLHSSymbol();
			
			Rule::RHSListConstIter ri, re;
			ri = (*i)->getRHS().begin();
			re = (*i)->getRHS().end();
			
			for ( ; ri != re; ++ri )
			{
				Symbol *tmpSym = SymbolTable::get()->findOrCreate( (*ri).first );
				
				if ( tmpSym->getType() == Symbol::TERMINAL )
				{
					if ( lhsSym->setFirstSet( tmpSym->getName() ) )
						progress = true;
					break;
				}
				else if ( lhsSym == tmpSym )
				{
					if ( ! lhsSym->isLambda() )
						break;
				}
				else
				{
					if ( lhsSym->unionFirstSet( *tmpSym ) )
						progress = true;
					
					if ( ! tmpSym->isLambda() )
						break;
				}
			}
		}
	} while ( progress );
}


////////////////////////////////////////


bool
RuleTable::isOnRightSide( const std::string &lhs )
{
	RuleListIter	i, e;
	bool			retval = false;
	
	i = myRuleList.begin();
	e = myRuleList.end();
	for ( ; i != e; ++i )
	{
		Rule::RHSListConstIter ri, re;
		ri = (*i)->getRHS().begin();
		re = (*i)->getRHS().end();
			
		for ( ; ri != re; ++ri )
		{
			if ( (*ri).first == lhs )
			{
				retval = true;
				break;
			}
		}
	}
	
	return retval;
}


////////////////////////////////////////


void
RuleTable::print( std::ostream &out ) const
{
	RuleListConstIter i, e;
	
	e = myRuleList.end();
	for ( i = myRuleList.begin(); i != e; ++i )
	{
		out << "Rule " << (i - myRuleList.begin()) << ": ";
		(*i)->print( out );
		out << std::endl;
	}
}


////////////////////////////////////////


static RuleTable *theRuleTable = 0;

RuleTable *
RuleTable::get( void )
{
	if ( ! theRuleTable )
		theRuleTable = new RuleTable;
	return theRuleTable;
}

