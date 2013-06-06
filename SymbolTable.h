/// @file SymbolTable.h
/// @brief File that declares a table of Symbols.
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

#ifndef _SymbolTable_h_
#define _SymbolTable_h_

#include <map>
#include <string>

class Symbol;


////////////////////////////////////////


class SymbolTable
{
public:
	SymbolTable( void );
	~SymbolTable( void );
	
	/// Looks up a symbol name, creating one if it doesn't exist yet.
	Symbol	*findOrCreate( const std::string &name );
	// Looks up symbol, returns NULL if it doesn't exist.
	Symbol	*find( const std::string &name ) const;
	
	void addDefault( const std::string &name );
	const std::string &getDefaultName( void ) const;
	Symbol *getDefault( void ) const;
	
	/// Access the symbols directly
	size_t getNumSymbols( void );
	Symbol *getNthSymbol( size_t i );
	
	/// Convenience routines
	size_t getNumTerminals( void );
	
	static SymbolTable *get( void );
	
private:
	void reindex( void );
	
	typedef std::map< std::string, Symbol *>	SymbolMap;
	typedef SymbolMap::iterator					SymbolMapIter;
	typedef SymbolMap::const_iterator			SymbolMapConstIter;
	
	SymbolMap	mySymbols;
	size_t		myNumTerminals;
	bool		myDefaultAdded;
	std::string myDefaultSymbol;
};

#endif /* _SymbolTable_h_ */

