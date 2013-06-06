/// @file CDriver.cpp
/// @brief File that implements a driver to produce C files.
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

#include <fstream>
#include "CDriver.h"
#include "Version.h"
#include "Symbol.h"
#include "SymbolTable.h"


////////////////////////////////////////


CDriver::CDriver( const Producer::ValueMap &valMap )
		: Producer( valMap )
{
}


////////////////////////////////////////


CDriver::~CDriver( void )
{
}


////////////////////////////////////////


bool
CDriver::writeHeader( void )
{
	const std::string &tokenType = getValue( "token_type" ).first;
	const std::string &extraArg = getValue( "extra_argument" ).first;
	const std::string &prefix = getValue( "token_prefix" ).first;
	std::string fileName;
	std::ofstream out;
	bool isOk = false;
	
	getFileName( fileName, ".h" );
	out.open( fileName.c_str() );
	
	if ( out.is_open() )
	{
		std::string poundDef = "_";
		poundDef.append( getParserName() );
		poundDef.append( "_h_" );
	
		out << "/*\n * This file auto-generated from " << getParserName()
			<< ".lem by " << VersionInfo::appName() << " version "
			<< VersionInfo::appVersion() << std::endl;
		out << " * Editing of this file strongly discouraged.\n */"
			  << std::endl;
		
		out << "\n#ifndef " << poundDef << std::endl;
		out << "#define " << poundDef << std::endl;
	
		out << "\n\nvoid " << getParserName()
			  << "( void *parser, int tok, ";
		if ( tokenType.empty() )
			out << "void *value";
		else
		{
			out << tokenType;
			if ( *(tokenType.end() - 1) != '*' )
				out << " value";
			else
				out << "value";
		}
		
		if ( ! extraArg.empty() )
			out << ", " << extraArg;
		out << " );\n";
	
		out << "void *" << getParserName() << "Alloc( void );\n";
		out << "void " << getParserName() << "Free( void *parser );\n\n";

		size_t i, nSym;
		int idx = 1;
			
		nSym = SymbolTable::get()->getNumSymbols();
		for ( i = 0; i < nSym; ++i )
		{
			Symbol *sp = SymbolTable::get()->getNthSymbol( i );
				
			if ( Symbol::TERMINAL == sp->getType() )
			{
				out << "#define " << prefix << sp->getName()
					<< " " << idx << std::endl;
				++idx;
			}
		}
		
		out << "\n#endif" << std::endl;
		isOk = true;
	}
	
	out.close();
	
	return isOk;
}


////////////////////////////////////////


bool
CDriver::writeSource( void )
{
	throw "Not Yet Implemented";
	
	return true;
}

