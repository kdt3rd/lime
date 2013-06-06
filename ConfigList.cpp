/// @file ConfigList.cpp
/// @brief File that implements a list of configs
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

#include <sstream>
#include <iostream>
#include <iomanip>

#include "ConfigList.h"

#include <algorithm>

#include "Error.h"
#include "Rule.h"
#include "RuleTable.h"
#include "Config.h"
#include "Symbol.h"
#include "SymbolTable.h"


////////////////////////////////////////


ConfigList::ConfigList( void )
		: myFront( 0 ), myLast( 0 ), myFrontBasis( 0 ), myLastBasis( 0 )
{
}


////////////////////////////////////////


ConfigList::~ConfigList( void )
{
	// We don't own these - the state constructed will...
	myFront = myLast = myFrontBasis = myLastBasis = 0;
}



////////////////////////////////////////


Config *
ConfigList::add( Rule *rp, int dot )
{
	Config *retval = find( rp->getRuleIndex(), dot );
	
	if ( ! retval )
	{
		retval = new Config( rp, dot );
		
		if ( ! myFront )
		{
			myFront = retval;
			myLast = retval;
		}
		else
		{
			myLast->setNext( retval );
			myLast = retval;
		}
		
		myConfigs.push_back( retval );
	}
	
	return retval;
}


////////////////////////////////////////


Config *
ConfigList::addWithBasis( Rule *rp, int dot )
{
	Config *retval = find( rp->getRuleIndex(), dot );
	
	if ( ! retval )
	{
		retval = new Config( rp, dot );
		
		if ( ! myFront )
		{
			myFront = retval;
			myLast = retval;
		}
		else
		{
			myLast->setNext( retval );
			myLast = retval;
		}
		
		if ( ! myFrontBasis )
		{
			myFrontBasis = retval;
			myLastBasis = retval;
		}
		else
		{
			myLastBasis->setNextBasis( retval );
			myLastBasis = retval;
		}
		
		myConfigs.push_back( retval );
	}
	
	return retval;
}


////////////////////////////////////////


Config *
ConfigList::getConfig( void )
{
	return myFront;
}


////////////////////////////////////////


Config *
ConfigList::getBasis( void )
{
	return myFrontBasis;
}


////////////////////////////////////////


class cfgListComp
{
public:
	bool operator()( const Config *a, const Config *b )
	{
		Rule *ar = a->getRule();
		Rule *br = b->getRule();
		
		return ( ar->getRuleIndex() == br->getRuleIndex() ) ?
			a->getDot() < b->getDot() :
			ar->getRuleIndex() < br->getRuleIndex();
	}
};


////////////////////////////////////////


void
ConfigList::sort( void )
{
	if ( myFront )
	{
		std::vector<Config *> tmpList;
		std::vector<Config *>::iterator i, e;
		Config *tmp = myFront;
		
		while ( tmp )
		{
			tmpList.push_back( tmp );
			tmp = tmp->getNext();
		}

		std::sort( tmpList.begin(), tmpList.end(), cfgListComp() );
		
		myFront = tmpList.front();
		myLast = tmpList.back();
		
		e = tmpList.end();
		for ( i = tmpList.begin(); i != e; ++i )
		{
			if ( (i + 1) != e )
				(*i)->setNext( *(i+1) );
			else
				(*i)->setNext( 0 );
		}
	}
}


////////////////////////////////////////


void
ConfigList::sortBasis( void )
{
	if ( myFrontBasis )
	{
		std::vector<Config *> tmpList;
		std::vector<Config *>::iterator i, e;
		Config *tmp = myFrontBasis;
		
		while ( tmp )
		{
			tmpList.push_back( tmp );
			tmp = tmp->getNextBasis();
		}

		std::sort( tmpList.begin(), tmpList.end(), cfgListComp() );
		
		myFrontBasis = tmpList.front();
		myLastBasis = tmpList.back();
		
		e = tmpList.end();
		for ( i = tmpList.begin(); i != e; ++i )
		{
			if ( (i + 1) != e )
				(*i)->setNextBasis( *(i+1) );
			else
				(*i)->setNextBasis( 0 );
		}
	}
}


////////////////////////////////////////


void
ConfigList::computeClosure( void )
{
	for ( Config *cfp = myFront; cfp; cfp = cfp->getNext() )
	{
		Rule *rp = cfp->getRule();
		int dot = cfp->getDot();
		
		const Rule::RHSList &rhs = rp->getRHS();
		
		if ( dot >= int( rhs.size() ) )
			continue;
		
		Symbol *sp = SymbolTable::get()->find( rhs[dot].first );
		
		if ( Symbol::NONTERMINAL == sp->getType() )
		{
			Rule *tmpRp = RuleTable::get()->findFirstRule( rhs[dot].first );
			
			if ( ! tmpRp && rhs[dot].first != "error" )
			{
				Error::get()->add( rp->getRuleLine(),
								   "Nonterminal \"%s\" has no rules.",
								   rhs[dot].first.c_str() );
			}
			
			for ( ; tmpRp; tmpRp = RuleTable::get()->getNextRule( tmpRp ) )
			{
				Config *newcfp = add( tmpRp, 0 );
				
//				std::ostringstream out1;
//				newcfp->getFollowSet().print( out1 );
//				std::cout << tmpRp->getLHS() << " initial( " << out1.str() << " )\n";
				
				int i;
				int N = int( rhs.size() );
				for ( i = dot + 1; i < N; ++i )
				{
					Symbol *xsp = SymbolTable::get()->find( rhs[i].first );
					
					if ( Symbol::TERMINAL == xsp->getType() )
					{
//						std::cout << " add( " << rhs[i].first << " )\n";
						newcfp->addFollowSet( xsp->getName() );
						break;
					}
					else
					{
//						std::ostringstream out2;
//						xsp->getFirstSet().print( out2 );
//						std::cout << " combine( " << out2.str() << " )\n";
						
						newcfp->combineFollowSet( xsp->getFirstSet() );
						if ( ! xsp->isLambda() )
							break;
					}
				}
				
				if( i == N )
					cfp->addForwardPropLink( newcfp );
			}
		}
	}
	
//	std::ostringstream tmpOut;
//	for ( Config *cfp = myFront; cfp; cfp = cfp->getNext() )
//	{
//		cfp->getRule()->print( tmpOut );
//		tmpOut << std::endl;
//	}
//	std::cout << "ConfigList elements:\n" << tmpOut.str() << std::endl;
}


////////////////////////////////////////


void
ConfigList::deleteConfigs( void )
{
	delete myFront;
	reset();
}


////////////////////////////////////////


void
ConfigList::resetPointers( void )
{
	myFront = myLast = NULL;
}


////////////////////////////////////////


void
ConfigList::reset( void )
{
	resetPointers();
	myFrontBasis = myLastBasis = NULL;
	myConfigs.clear();
}


////////////////////////////////////////


Config *
ConfigList::find( int ruleidx, int dot )
{
	ListIter i, e;
	Config *retval = 0;
	
	e = myConfigs.end();
	for ( i = myConfigs.begin(); i != e; ++i )
	{
		if ( (*i)->getRule()->getRuleIndex() == ruleidx &&
			 (*i)->getDot() == dot )
		{
			retval = (*i);
			break;
		}
	}
	
	return retval;
}

		
