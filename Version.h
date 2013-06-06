/// @file Version.h
/// @brief File that declares the version info for the application.
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

#ifndef _Version_h_
#define _Version_h_


////////////////////////////////////////


namespace VersionInfo
{
	inline const char *appName( void )
	{
		return "Lime";
	}
	
	inline const char *appVersion( void )
	{
		return "0.9";
	}
}

#endif /* _Version_h_ */

