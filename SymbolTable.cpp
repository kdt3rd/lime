/// @file SymbolTable.cpp
/// @brief File that declares...
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

#include <stdexcept>

#include "SymbolTable.h"
#include "Symbol.h"


////////////////////////////////////////


SymbolTable::SymbolTable( void )
		: myNumTerminals( 1 ), myDefaultAdded( false )
{
}


////////////////////////////////////////


SymbolTable::~SymbolTable( void )
{
}


////////////////////////////////////////


Symbol *
SymbolTable::findOrCreate( const std::string &name )
{
	SymbolMapIter i;
	Symbol	*retval = 0;
	
	i = mySymbols.find( name );
	if ( i == mySymbols.end() )
	{
		retval = new Symbol( name );
		mySymbols[ name ] = retval;
		
		reindex();
		
		if ( Symbol::TERMINAL == retval->getType() )
			++myNumTerminals;
	}
	else
		retval = (*i).second;
	
	return retval;
}


////////////////////////////////////////


Symbol *
SymbolTable::find( const std::string &name ) const
{
	SymbolMapConstIter i;
	Symbol	*retval = 0;
	
	i = mySymbols.find( name );
	if ( i != mySymbols.end() )
		retval = (*i).second;
	
	return retval;
}


////////////////////////////////////////


void
SymbolTable::addDefault( const std::string &name )
{
	if ( myDefaultAdded )
		throw std::logic_error( "Default Symbol already specified" );
	
	SymbolMapIter i;
	
	i = mySymbols.find( name );
	if ( i == mySymbols.end() )
	{
		Symbol *tmp = new Symbol( name );
		myDefaultSymbol = name;
		mySymbols[ name ] = tmp;
		
		reindex();
	}
	else
		throw std::logic_error( "Default Symbol collides with existing symbol" );
	
	myDefaultAdded = true;
}


////////////////////////////////////////


const std::string &
SymbolTable::getDefaultName( void ) const
{
	return myDefaultSymbol;
}


////////////////////////////////////////


Symbol *
SymbolTable::getDefault( void ) const
{
	return find( myDefaultSymbol );
}


////////////////////////////////////////


size_t
SymbolTable::getNumSymbols( void )
{
	size_t N = mySymbols.size();
	return myDefaultAdded ? N - 1 : N;
}


////////////////////////////////////////


Symbol *
SymbolTable::getNthSymbol( size_t i )
{
	Symbol *retval = 0;
	
	if ( i < mySymbols.size() )
	{
		SymbolMapIter mi = mySymbols.begin();
		while ( i > 0 )
		{
			++mi;
			--i;
		}
		
		retval = (*mi).second;
	}
	
	return retval;
}
	 

////////////////////////////////////////


size_t
SymbolTable::getNumTerminals( void )
{
	return myNumTerminals;
}


////////////////////////////////////////


static SymbolTable *theSymTable = 0;

SymbolTable *
SymbolTable::get( void )
{
	if ( ! theSymTable )
		theSymTable = new SymbolTable;
	
	return theSymTable;
}


////////////////////////////////////////


void
SymbolTable::reindex( void )
{
	SymbolMapIter mi = mySymbols.begin();
	SymbolMapIter me = mySymbols.end();
	size_t curidx = 0;
	
	for ( ; mi != me; ++mi )
		(*mi).second->setIndex( curidx++ );
}


