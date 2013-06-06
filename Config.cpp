/// @file Config.cpp
/// @brief File that implements a config point
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

#include "Config.h"
#include "Rule.h"
#include "State.h"


////////////////////////////////////////


Config::Config( Rule *rp, int dot )
		: myRule( rp ), myDot( dot ), myNext( 0 ),
		  myNextBasis( 0 ), myStatus( INCOMPLETE ), myState( 0 )
{
}


////////////////////////////////////////


Config::~Config( void )
{
	delete myNext;
	myNextBasis = 0;
}


////////////////////////////////////////


bool
Config::addFollowSet( const std::string &fsItem )
{
	return myFollowSet.add( fsItem );
}


////////////////////////////////////////


bool
Config::combineFollowSet( const FollowSet &other )
{
	return myFollowSet.combine( other );
}


////////////////////////////////////////


void
Config::addForwardPropLink( Config *fpl )
{
//	myForwardProps.push_back( fpl );
	myForwardProps.insert( myForwardProps.begin(), fpl );
}


////////////////////////////////////////


void
Config::addBackwardPropLink( Config *bpl )
{
//	myBackwardProps.push_back( bpl );
	myBackwardProps.insert( myBackwardProps.begin(), bpl );
}


////////////////////////////////////////


void
Config::mergePropLinks( Config *other )
{
	PropList &otherPL = other->myBackwardProps;
	PropListIter i, e;
	
	for ( i = otherPL.begin(), e = otherPL.end(); i != e; ++i )
		myBackwardProps.insert( myBackwardProps.begin(), (*i) );
	otherPL.clear();
//	while ( ! otherPL.empty() )
//	{
//		myBackwardProps.push_back( otherPL.back() );
//		otherPL.pop_back();
//	}
}


////////////////////////////////////////


void
Config::print( std::ostream &out ) const
{
	if ( myDot == int( myRule->getRHS().size() ) )
	{
		out << "  " << myRule->getLHS() << "("
			<< myRule->getRuleIndex() << ") ::=";
	}
	else
		out << "  " << myRule->getLHS() << " ::=";
	
	const Rule::RHSList &rhs = myRule->getRHS();
	size_t i, N;
	
	N = rhs.size();
	for ( i = 0; i <= N; ++i )
	{
		if ( i == size_t( myDot ) )
			out << " *";
		if ( i < N )
			out << " " << rhs[i].first;
	}
	
	out << std::endl;
	
	out << "     FollowSet: ";
	myFollowSet.print( out );
	
	bool doneEndl = false;
	
	PropListConstIter pi, pe;
	
	pe = myForwardProps.end();
	for ( pi = myForwardProps.begin(); pi != pe; ++pi )
	{
		if ( ! doneEndl )
		{
			doneEndl = true;
			out << std::endl;
		}
		
		out << "     To:   " << (*pi)->getRule()->getLHS() << " (" <<
			(*pi)->myState->getStateIndex() << ")";
//			(*pi)->getDot() << ")";
		
		if ( (pi + 1) == pe )
			doneEndl = false;
		else
			out << std::endl;
	}
	
	pe = myBackwardProps.end();
	for ( pi = myBackwardProps.begin(); pi != pe; ++pi )
	{
		if ( ! doneEndl )
		{
			doneEndl = true;
			out << std::endl;
		}
		
		out << "     From: " << (*pi)->getRule()->getLHS() << " (" <<
			(*pi)->myState->getStateIndex() << ")";
//			(*pi)->getDot() << ")";
		
		if ( (pi + 1) == pe )
			doneEndl = false;
		else
			out << std::endl;
	}
}


