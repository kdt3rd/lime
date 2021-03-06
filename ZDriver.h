/// @file ZDriver.h
/// @brief File that declares...
///
///

//
// 
// Copyright � 2003-2013 Kimball Thurston
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

#ifndef _ZDriver_h_
#define _ZDriver_h_

#include <iosfwd>
#include "LanguageDriver.h"

class Action;
class Rule;

class ZDriver;
struct ZDriverOutHelp
{
	ZDriver *myOut;
};


////////////////////////////////////////


class ZDriver : public Producer
{
public:
	ZDriver( const Producer::ValueMap &valMap );
	virtual ~ZDriver( void );

	virtual bool writeHeader( void );
	virtual bool writeSource( void );

private:
	void writeParserCtorDtor( std::ostream &out );
	void writeImplClassDecl( std::ostream &out );
	void writeImplClassCtorDtor( std::ostream &out );
	void writeStateTableDecl( std::ostream &out );
	void writeStateTable( std::ostream &out );
	void buildStateTable( std::ostream &out );
	void writeRuleTableDecl( std::ostream &out );
	void writeRuleTable( std::ostream &out );
	void buildRuleTable( std::ostream &out );
	void writeMainParserFunc( std::ostream &out );
	void writeShiftFuncDecl( std::ostream &out );
	void writeShiftFunc( std::ostream &out );
	void writeReduceFuncDecl( std::ostream &out );
	void writeReduceFunc( std::ostream &out );
	void writeAcceptFuncDecl( std::ostream &out );
	void writeAcceptFunc( std::ostream &out );
	void writeDestructorHandlerDecl( std::ostream &out );
	void writeDestructorHandler( std::ostream &out );
	void writeParserUtilDecl( std::ostream &out );
	void writeParserUtil( std::ostream &out );
	void writeErrorRoutinesDecl( std::ostream &out );
	void writeErrorRoutines( std::ostream &out );

	void emitErrorHandling( std::ostream &out );
	void emitValue( const ValueSetting	&val,
					std::ostream		&out );
	void emitDestructor( const std::string	&type,
						 const std::string	&dtor,
						 std::ostream		&out );
	void emitLineInfo( const std::string	&srcFile,
					   size_t				 line,
					   std::ostream			&out );
	void emitAction( const Action &act, std::ostream &out );
	void emitRule( const Rule *rp, std::ostream &out );
	void substCode( std::string &codeStr,
					const std::string &var,
					const std::string &replName,
					bool lhs = false,
					const std::string &lhsType = std::string() );
	void emitFuncBreak( std::ostream &out );

	ZDriverOutHelp endl( void );

	friend std::ostream &operator<<( std::ostream &os, ZDriverOutHelp out );

	inline void incOutLine( void );
	inline size_t getOutLine( void ) const;

	std::string myFileName;
	std::string myExtraArg;
	std::string myExtraArgCall;
	std::string myNameSpace;
	std::string myPimplName;

	size_t myCurLineNum;
};


////////////////////////////////////////


inline void ZDriver::incOutLine( void )
{
	++myCurLineNum;
}

inline size_t ZDriver::getOutLine( void ) const
{
	return myCurLineNum;
}

#endif /* _ZDriver_h_ */

