/// @file Symbol.h
/// @brief File that declares a symbol table entry.
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

#ifndef _Symbol_h_
#define _Symbol_h_

#include <string>
#include "FollowSet.h"


////////////////////////////////////////


class Symbol
{
public:
	enum Type
	{
		TERMINAL,
		NONTERMINAL
	};
	
	enum Assoc // Associativity
	{
		LEFT,
		RIGHT,
		NONE,
		UNKNOWN
	};
	
	Symbol( const std::string &name );
	
	Symbol( const Symbol &other );
	
	~Symbol( void );
	
	Symbol &operator=( const Symbol &other );
	bool operator<( const Symbol &other ) const;
	
	inline const std::string &getName( void ) const;
	
	inline Type getType( void ) const;
	
	inline void setIndex( size_t i );
	inline size_t getIndex( void ) const;
	
	// if defined (-1 otherwise)
	void setPrecedence( int prec );
	inline int getPrecedence( void ) const;
	
	// if precedence defined, then associativity is valid
	void setAssoc( Assoc newtype );
	inline Assoc getAssoc( void ) const;
	
	// Returns true if actually changes
	bool setFirstSet( const std::string &fs );
	// Returns true if actually changes
	bool unionFirstSet( const Symbol &other );
	inline const FollowSet &getFirstSet( void ) const;
	
	// true if NT and can generate an empty string
	void setLambda( bool on_off );
	inline bool isLambda( void ) const;
	
	// code that is run when this symbol is removed during error processing
	void setDestructor( const std::string &d );
	inline const std::string &getDestructor( void ) const;
	
	// line number where destructor code declared
	void setDestructorLine( int line );
	inline int getDestructorLine( void ) const;
	
	// if a non-term, data type of information held
	void setDataType( const std::string &dt );
	inline const std::string &getDataType( void ) const;
	
private:
	std::string  myName;
	size_t		 myIndex;
	
	Type		 myType;
	int			 myPrecedence;
	Assoc		 myAssoc;
	FollowSet	 myFirstSet;
	bool		 myLambda;
	std::string	 myDestructor;
	int			 myDestLine;
	std::string	 myDataType;
};


////////////////////////////////////////


inline const std::string &Symbol::getName( void ) const { return myName; }
inline Symbol::Type Symbol::getType( void ) const { return myType; }
inline void Symbol::setIndex( size_t i ) { myIndex = i; }
inline size_t Symbol::getIndex( void ) const { return myIndex; }
inline int Symbol::getPrecedence( void ) const { return myPrecedence; }
inline Symbol::Assoc Symbol::getAssoc( void ) const { return myAssoc; }
inline const FollowSet &Symbol::getFirstSet( void ) const { return myFirstSet; }
inline bool Symbol::isLambda( void ) const { return myLambda; }
inline const std::string &Symbol::getDestructor( void ) const { return myDestructor; }
inline int Symbol::getDestructorLine( void ) const { return myDestLine; }
inline const std::string &Symbol::getDataType( void ) const { return myDataType; }

#endif /* _Symbol_h_ */

