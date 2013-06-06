/// @file Parser.h
/// @brief File that declares the driver class for the lemon parser
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

#ifndef _Parser_h_
#define _Parser_h_

#include <string>
#include "Symbol.h"
#include "Rule.h"

////////////////////////////////////////


class Grammar;

class Parser
{
public:
	Parser( void );
	~Parser( void );
	
	void setSourceFile( const std::string &sourceFile );
	
	void parse( Grammar *grammar );
	
	int getErrCount( void ) const;
	
private:
	
	enum ParseState
	{
		INITIALIZE,
		WAITING_FOR_DECL_OR_RULE,
		WAITING_FOR_DECL_KEYWORD,
		WAITING_FOR_DECL_ARG,
		WAITING_FOR_PRECEDENCE_SYMBOL,
		WAITING_FOR_RULEPROD,
		IN_RHS,
		LHS_ALIAS_1,
		LHS_ALIAS_2,
		LHS_ALIAS_3,
		RHS_ALIAS_1,
		RHS_ALIAS_2,
		PRECEDENCE_MARK_1,
		PRECEDENCE_MARK_2,
		RESYNC_AFTER_RULE_ERROR,
		RESYNC_AFTER_DECL_ERROR,
		WAITING_FOR_DESTRUCTOR_SYMBOL,
		WAITING_FOR_DESTRUCTOR_DECL,
		WAITING_FOR_DATATYPE_SYMBOL,
		WAITING_FOR_DATATYPE_DECL
	};
	
	void handleNextToken( int tokenStartLine, const std::string &token );
	bool slurpFile( void );
	
	void chompString( std::string &str, bool innerWhite = false );
	
	std::string mySourceFile;
	std::string mySourceData;
	
	std::string myAppName;
	
	int			myErrCount;
	
	ParseState	myCurState;
	std::string myCurLHS;
	std::string myCurLHSAlias;
	
	Rule::RHSList myCurRHS;
	std::string myCurRuleCode;
	
	std::string myCurDeclKey;
	
	int				myCurPrecCounter;
	Symbol::Assoc	myCurDeclAssoc;
	
	Rule		*myPrevRule;
	
	Grammar		*myCurGrammar;
};

#endif /* _Parser_h_ */

