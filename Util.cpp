/// @file Util.cpp
/// @brief File that implements some utility functions
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

#include <string>
#include "Util.h"


////////////////////////////////////////


void
Util::getFileName( std::string			&fileName,
				   const std::string	&outputDir,
				   const std::string	&srcFile,
				   const char			*ext )
{
	std::string baseName = srcFile;
	std::string::size_type cPos;
	
	if ( ! outputDir.empty() )
	{
		fileName = outputDir;
		if ( *(fileName.end()-1) != '/' )
			fileName.push_back( '/' );
	}
	
	cPos = baseName.rfind( '/' );
	if ( cPos != std::string::npos )
		baseName.erase( baseName.begin(), baseName.begin() + cPos + 1 );
	cPos = baseName.rfind( '.' );
	if ( cPos != std::string::npos )
		baseName.erase( baseName.begin() + cPos, baseName.end() );
	
	fileName.append( baseName );
	fileName.append( ext );
}
