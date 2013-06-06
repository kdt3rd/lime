/// @file LanguageDriver.cpp
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

#include "LanguageDriver.h"
#include "CDriver.h"
#include "CPPDriver.h"
#include "ZDriver.h"
#include "Util.h"


////////////////////////////////////////


Producer::Producer( const Producer::ValueMap &valMap )
		: myDebugOutput( false ), myValues( valMap )
{
}


////////////////////////////////////////


Producer::~Producer( void )
{
}


////////////////////////////////////////


void
Producer::setOutputDir( const std::string &outputDir )
{
	myOutputDir = outputDir;
}


////////////////////////////////////////


void
Producer::setParserName( const std::string &name )
{
	myParserName = name;
}


////////////////////////////////////////


void
Producer::setSourceFile( const std::string &name )
{
	mySourceFile = name;
}


////////////////////////////////////////


const Producer::ValueSetting &
Producer::getValue( const std::string &name ) const
{
	static ValueSetting emptypair = ValueSetting( std::string(), -1 );
	ValueMapConstIter i = myValues.find( name );

	if ( i != myValues.end() )
		return (*i).second;

	return emptypair;
}


////////////////////////////////////////


bool
Producer::isValueSet( const std::string &name ) const
{
	ValueMapConstIter i = myValues.find( name );
	bool retval = false;

	if ( i != myValues.end() && ! (*i).first.empty() )
		retval = true;

	return retval;
}


////////////////////////////////////////


void
Producer::getFileName( std::string &fileName, const char *ext )
{
	Util::getFileName( fileName, getOutputDir(), getSourceFile(), ext );
}


////////////////////////////////////////


Producer *
LanguageDriver::getProducer( LanguageDriver::Language	 lang,
							 const Producer::ValueMap	&valMap,
							 const std::string			&outDir,
							 const std::string			&parserName,
							 const std::string			&srcFile )
{
	Producer *retval = 0;

	switch ( lang )
	{
		case C:
			retval = new CDriver( valMap );
			break;

		case CPP:
			retval = new CPPDriver( valMap );
			break;

		case Z:
			retval = new ZDriver( valMap );
			break;

		default:
			throw "Unknown Language";
			break;
	}

	retval->setOutputDir( outDir );
	retval->setParserName( parserName );
	retval->setSourceFile( srcFile );

	return retval;
}



