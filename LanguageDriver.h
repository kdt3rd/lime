/// @file LanguageDriver.h
/// @brief File that declares an abstract class for output of a particular
///        language.
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

#ifndef _LanguageDriver_h_
#define _LanguageDriver_h_

#include <string>
#include <map>


////////////////////////////////////////


class Producer
{
public:
	typedef std::pair< std::string, int >			ValueSetting;
	typedef std::map< std::string, ValueSetting >	ValueMap;
	typedef ValueMap::iterator						ValueMapIter;
	typedef ValueMap::const_iterator				ValueMapConstIter;
	
	Producer( const ValueMap &valMap );
	virtual ~Producer( void );
	
	void setOutputDir( const std::string &outputDir );
	inline const std::string &getOutputDir( void ) const;
	
	void setParserName( const std::string &name );
	inline const std::string &getParserName( void ) const;
	
	void setSourceFile( const std::string &name );
	inline const std::string &getSourceFile( void ) const;
	
	inline void setDebugOutput( bool on_off );
	inline bool isDebugOutput( void ) const;
	
	const ValueSetting &getValue( const std::string &name ) const;
	bool isValueSet( const std::string &name ) const;
	
	virtual bool writeHeader( void ) = 0;
	virtual bool writeSource( void ) = 0;
	
protected:
	void getFileName( std::string &fileName, const char *ext );
	
private:
	std::string myOutputDir;
	std::string myParserName;
	std::string mySourceFile;
	
	bool myDebugOutput;
	
	ValueMap myValues;
};

class LanguageDriver
{
public:
	enum Language
	{
		C, ///< C based implementation
		CPP, ///< C++ based implementation
		Z, ///< C++ based implementation, but uses Zion Core
	};
	
	static Producer *getProducer( Language					 lang,
								  const Producer::ValueMap	&valMap,
								  const std::string			&outDir,
								  const std::string			&parserName,
								  const std::string			&srcFile );
};


////////////////////////////////////////


inline const std::string &
Producer::getOutputDir( void ) const { return myOutputDir; }
inline const std::string &
Producer::getParserName( void ) const { return myParserName; }
inline const std::string &
Producer::getSourceFile( void ) const { return mySourceFile; }
inline void
Producer::setDebugOutput( bool on_off ) { myDebugOutput = on_off; }
inline bool
Producer::isDebugOutput( void ) const { return myDebugOutput; }

#endif /* _LanguageDriver_h_ */

