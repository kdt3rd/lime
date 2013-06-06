/// @file Grammar.h
/// @brief File that declares the main grammar production class.
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

#ifndef _Grammar_h_
#define _Grammar_h_

#include <string>
#include <vector>
#include <map>

#include "ConfigList.h"
#include "LanguageDriver.h"

class Symbol;
class State;
class Action;


////////////////////////////////////////


class Grammar
{
public:
	Grammar( void );
	~Grammar( void );

	inline bool isBasisOnly( void ) const;
	inline void setBasisOnly( bool on_off );
	inline bool isCompressActions( void ) const;
	inline void setCompressActions( bool on_off );
	inline bool isNoActions( void ) const;
	inline void setNoActions( bool on_off );
	inline bool isDebugOutput( void ) const;
	inline void setDebugOutput( bool on_off );
	inline bool isQuiet( void ) const;
	inline void setQuiet( bool on_off );
	inline bool isStats( void ) const;
	inline void setStats( bool on_off );
	
	void setSourceFile( const char *sourceFile );
	inline const std::string &getSourceFile( void ) const;
	void setOutputDirectory( const char *outputDir );
	inline const std::string &getOutputDirectory( void ) const;
	
	void setLanguage( LanguageDriver::Language lang );
	inline LanguageDriver::Language getLanguage( void ) const;
	
	/// Returns true if ok, false if value already specified
	bool setValue( const std::string &name,
				   const std::string &value,
				   int				  line );
	
	void process( void );
	
	int getNumConflicts( void );
	
	void reprintGrammar( void ) const;
	void printStats( void ) const;
	
private:
	void findFirstSets( void );
	void findStates( void );
	void findLinks( void );
	void findFollowSets( void );
	void findActions( void );
	void compressTables( void );
	void reportOutput( void );
	void outputFiles( void );
	
	State *getNextState( void );
	void buildShifts( State *state );
	
	int resolveConflict( Action &act, Action &nextAct );
	
	Symbol *getStartSymbol( bool adderr = false );
	const std::string &getName( void ) const;
	
	bool myBasisOnly;
	bool myCompressActions;
	bool myNoActions;
	bool myDebugOutput;
	bool myQuiet;
	bool myStats;
	
	std::string mySourceFile;
	std::string myOutputDir;
	
	LanguageDriver::Language myLanguage;
	
	typedef std::pair< std::string, int >			ValueSetting;
	typedef std::map< std::string, ValueSetting >	ValueMap;
	typedef ValueMap::iterator						ValueMapIter;
	typedef ValueMap::const_iterator				ValueMapConstIter;
	
	ValueMap	mySettings;
	
	ConfigList	myCurConfigList;
	int			myNumConflicts;
	
	Symbol *myErrSym;
};


////////////////////////////////////////


inline bool Grammar::isBasisOnly( void ) const { return myBasisOnly; }
inline void Grammar::setBasisOnly( bool on_off ) { myBasisOnly = on_off; }

inline bool Grammar::isCompressActions( void ) const { return myCompressActions; }
inline void Grammar::setCompressActions( bool on_off ) { myCompressActions = on_off; }

inline bool Grammar::isNoActions( void ) const { return myNoActions; }
inline void Grammar::setNoActions( bool on_off ) { myNoActions = on_off; }

inline bool Grammar::isDebugOutput( void ) const { return myDebugOutput; }
inline void Grammar::setDebugOutput( bool on_off ) { myDebugOutput = on_off; }

inline bool Grammar::isQuiet( void ) const { return myQuiet; }
inline void Grammar::setQuiet( bool on_off ) { myQuiet = on_off; }

inline bool Grammar::isStats( void ) const { return myStats; }
inline void Grammar::setStats( bool on_off ) { myStats = on_off; }


////////////////////////////////////////


inline const std::string &
Grammar::getSourceFile( void ) const { return mySourceFile; }

inline const std::string &
Grammar::getOutputDirectory( void ) const { return myOutputDir; }

inline LanguageDriver::Language
Grammar::getLanguage( void ) const { return myLanguage; }

#endif /* _Grammar_h_ */

