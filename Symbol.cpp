/// @file Symbol.cpp
/// @brief File that implements the symbol class.
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

#include <ctype.h>
#include <cstdio>
#include <map>
#include "Symbol.h"


////////////////////////////////////////


Symbol::Symbol( const std::string &name )
		: myName( name ), myIndex( 0 ), myPrecedence( -1 ),
		  myAssoc( UNKNOWN ), myLambda( false ), myDestLine( 0 )
{
	myType = isupper( name[0] ) ? TERMINAL : NONTERMINAL;
}


////////////////////////////////////////


Symbol::Symbol( const Symbol &other )
		: myName( other.myName ), myIndex( other.myIndex ),
		  myType( other.myType ),
		  myPrecedence( other.myPrecedence ), myAssoc( other.myAssoc ),
		  myFirstSet( other.myFirstSet ), myLambda( other.myLambda ),
		  myDestructor( other.myDestructor ),
		  myDestLine( other.myDestLine ),
		  myDataType( other.myDataType )
{
}


////////////////////////////////////////


Symbol::~Symbol( void )
{
}


////////////////////////////////////////


Symbol &Symbol::operator=( const Symbol &other )
{
	myName = other.myName;
	myIndex = other.myIndex;
	myType = other.myType;
	myPrecedence = other.myPrecedence;
	myAssoc = other.myAssoc;
	myFirstSet = other.myFirstSet;
	myLambda = other.myLambda;
	myDestructor = other.myDestructor;
	myDestLine = other.myDestLine;
	myDataType = other.myDataType;
	
	return *this;
}


////////////////////////////////////////


bool
Symbol::operator<( const Symbol &other ) const
{
	return myName < other.myName;
}


////////////////////////////////////////


void
Symbol::setPrecedence( int prec )
{
	myPrecedence = prec;
}


////////////////////////////////////////


void
Symbol::setAssoc( Symbol::Assoc newtype )
{
	myAssoc = newtype;
}


////////////////////////////////////////


bool
Symbol::setFirstSet( const std::string &fs )
{
	return myFirstSet.add( fs );
}


////////////////////////////////////////


bool
Symbol::unionFirstSet( const Symbol &other )
{
	return myFirstSet.combine( other.myFirstSet );
}


////////////////////////////////////////


void
Symbol::setLambda( bool on_off )
{
	myLambda = on_off;
}


////////////////////////////////////////


void
Symbol::setDestructor( const std::string &d )
{
	myDestructor = d;
}


////////////////////////////////////////


void
Symbol::setDestructorLine( int line )
{
	myDestLine = line;
}


////////////////////////////////////////


void
Symbol::setDataType( const std::string &dt )
{
	myDataType = dt;
}


