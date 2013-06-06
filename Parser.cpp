/// @file Parser.cpp
/// @brief File that implements the main parser driver.
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

#include <cstdio>
#include <ctype.h>

#include "Parser.h"
#include "Error.h"
#include "Grammar.h"
#include "RuleTable.h"
#include "SymbolTable.h"


////////////////////////////////////////


Parser::Parser( void )
		: myErrCount( 0 ), myPrevRule( 0 ), myCurGrammar( 0 )
{
}


////////////////////////////////////////


Parser::~Parser( void )
{
}


////////////////////////////////////////


void
Parser::setSourceFile( const std::string &sourceFile )  
{
	mySourceFile = sourceFile;
}


////////////////////////////////////////


// Skip C++ comments to end of line
inline void skipCPPComment( std::string::iterator &curP,
							std::string::iterator  endP )
{
	curP += 2;
	while ( curP != endP && *curP != '\n' )
		++curP;
}


////////////////////////////////////////


inline void skipCComment( std::string::iterator &curP,
						  int					&curLine,
						  std::string::iterator	 endP )
{
	// Skip C comment. Little tricker than C++... :/
	int curNest = 1;
	
	curP += 2;
	
	while ( curP != endP )
	{
		if ( *curP == '\n' )
		{
			++curLine;
			++curP;
		}
		else if ( curP < ( endP - 1 ) )
		{
			if ( *curP == '*' && *(curP + 1) == '/' )
			{
				--curNest;
				curP += 2;
				if ( curNest == 0 )
					break;
			}
			else if ( *curP == '/' && *(curP + 1) == '*' )
			{
				++curNest;
				curP += 2;
			}
		}
		else
		{
			break;
		}
	}
	
	if ( curNest != 0 )
	{
		curP = endP;
	}
}


////////////////////////////////////////


inline void skipStringLiteral( std::string::iterator	&curP,
							   int						&curLine,
							   std::string::iterator	 endP,
							   char						 startChar = '\"' )
{
	++curP;
	while ( curP != endP )
	{
		if ( *curP == '\n' )
			curLine++;
					
		if ( *curP == '\\' )
		{
			++curP;
			if ( curP < ( endP - 1 ) )
				++curP;
		}
		else if ( *curP == startChar )
		{
			++curP;
			break;
		}
		else
			++curP;
	}
}


////////////////////////////////////////


void
Parser::parse( Grammar *grammar )
{
	myCurGrammar = grammar;
	
	if ( slurpFile() )
	{
		int						 curLine = 1;
		std::string::iterator	 curP = mySourceData.begin();
		std::string::iterator	 endP = mySourceData.end();
		std::string				 curToken;
		int						 tsLine = 0;
		
		myCurState = INITIALIZE;
		
		while ( curP != endP )
		{
			if ( *curP == '\n' )
			{
				++curLine;
				++curP;
				continue;
			}
			else if ( isspace( *(curP) ) )
			{
				++curP;
				continue;
			}
			else if ( curP < ( endP - 1 ) )
			{
				if ( *curP == '/' && *(curP + 1) == '/' )
				{
					skipCPPComment( curP, endP );
					continue;
				}
				else if ( *curP == '/' && *(curP + 1) == '*' )
				{
					skipCComment( curP, curLine, endP );
					if ( curP == endP )
						Error::get()->add( tsLine,
										   "Comment is not closed before end of file" );
					continue;
				}
			}
			
			curToken.clear();
			tsLine = curLine;
			
			// String literal
			if ( *curP == '\"' )
			{
				std::string::iterator	 strStart = curP;
				
				skipStringLiteral( curP, curLine, endP );
				
				if ( curP == endP )
				{
					Error::get()->add( tsLine,
									   "String literal not terminated." );
				}
				else
					curToken = mySourceData.substr(
						strStart - mySourceData.begin(),
						curP - strStart );
			}
			else if ( *curP == '{' ) // chunk of code
			{
				int						 nestLevel = 1;
				std::string::iterator	 codeStart = curP;
				
				++curP;
				while ( curP != endP )
				{
					if ( *curP == '\n' )
					{
						++curLine;
						++curP;
					}
					else if ( *curP == '{' )
					{
						++nestLevel;
						++curP;
					}
					else if ( *curP == '}' )
					{
						--nestLevel;
						++curP;
						
						if ( nestLevel == 0 )
							break;
					}
					else if ( curP < ( endP - 1 ) )
					{
						if ( *curP == '/' && *(curP + 1) == '/' )
						{
							skipCPPComment( curP, endP );
						}
						else if ( *curP == '/' && *(curP + 1) == '*' )
						{
							skipCComment( curP, curLine, endP );
							if ( curP == endP )
								Error::get()->add( tsLine,
												   "Comment is not closed before end of file" );
						}
						else if ( *curP == '\'' || *curP == '\"' )
						{
							skipStringLiteral( curP, curLine, endP, *curP );
						}
						else
							++curP;
					}
				}
				
				if ( curP == endP )
				{
					Error::get()->add( tsLine,
									   "Code block starting here is not terminated" );
				}
				else
					curToken = mySourceData.substr(
						codeStart - mySourceData.begin(),
						curP - codeStart );
			}
			else if ( isalnum( *curP ) )
			{
				curToken.push_back( *curP );
				++curP;
				while ( curP != endP &&
						( isalnum( *curP ) || *curP == '_' || *curP == ':' ) )
				{
					curToken.push_back( *curP );
					++curP;
				}
			}
			else if ( curP < ( endP - 2 ) &&
					  *curP == ':' &&
					  *(curP + 1) == ':' &&
					  *(curP + 2) == '=' ) // operator ::=
			{
				curToken = "::=";
				curP += 3;
			}
			else if ( curP < ( endP - 2 ) &&
					  *curP == ':' &&
					  *(curP + 1) == ':' &&
					  isalpha( *(curP + 2) ) ) // global namespace tag
			{
				curToken.push_back( *curP );
				++curP;
				while ( curP != endP &&
						( isalnum( *curP ) || *curP == '_' || *curP == ':' ) )
				{
					curToken.push_back( *curP );
					++curP;
				}
			}
			else // remaining single char tokens
			{
				curToken.push_back( *curP );
				++curP;
			}
			
			if ( ! curToken.empty() )
				handleNextToken( tsLine, curToken );
			else
				curP = endP;
		}
	}
}


////////////////////////////////////////


void
Parser::handleNextToken( int tsLine, const std::string &token )
{
//	std::printf( "Token line %d: '%s' curState: %d\n",
//				 tsLine, token.c_str(), int( myCurState ) );
	
	switch ( myCurState )
	{
		case INITIALIZE:
			myPrevRule = 0;
			myCurPrecCounter = 0;
			myCurDeclAssoc = Symbol::UNKNOWN;
			// FALL THROUGH

		case WAITING_FOR_DECL_OR_RULE:
			if ( token[0] == '%' )
			{
				myCurState = WAITING_FOR_DECL_KEYWORD;
			}
			else if ( islower( token[0] ) )
			{
				SymbolTable::get()->findOrCreate( token );
				myCurLHS = token;
				myCurLHSAlias.clear();
				myCurRHS.clear();
				
				myCurState = WAITING_FOR_RULEPROD;
			}
			else if ( token[0] == '{' )
			{
				if ( myPrevRule )
				{
					if ( myPrevRule->getCode().empty() )
					{
						std::string tmpVal = token;
						chompString( tmpVal );
						myPrevRule->setCode( tsLine, tmpVal );
					}
					else
						Error::get()->add( tsLine,
										   "Multiple code fragments for rule found at this line." );
				}
				else
					Error::get()->add( tsLine,
									   "No rule to attach the code fragment starting at this line." );
			}
			else if( token[0] == '[' )
			{
				myCurState = PRECEDENCE_MARK_1;
			}
			else
			{
				Error::get()->add( tsLine,
								   "Token '%s' should be either \"%%\" or a nonterminal name.",
								   token.c_str() );
			}
			break;
			
		case PRECEDENCE_MARK_1:
			if ( ! isupper( token[0] ) )
			{
				Error::get()->add( tsLine,
								   "The precedence symbol must be a terminal." );
			}
			else if ( myPrevRule )
			{
				if ( myPrevRule->getPrecedence().empty() )
				{
					SymbolTable::get()->findOrCreate( token );
					myPrevRule->setPrecedence( token );
				}
				else
					Error::get()->add( tsLine,
									   "Precedence mark on this line is not the first "
									   "to follow the previous rule." );
			}
			else
				Error::get()->add( tsLine,
								   "There is no prior rule to assign precedence '[%s]'.",
								   token.c_str() );
			
			myCurState = PRECEDENCE_MARK_2;
			break;
			
		case PRECEDENCE_MARK_2:
			if ( token[0] != ']' )
				Error::get()->add( tsLine, "Missing ']' on precedence mark." );

			myCurState = WAITING_FOR_DECL_OR_RULE;
			break;
			
		case WAITING_FOR_RULEPROD:
			if ( token == "::=" )
				myCurState = IN_RHS;
			else if ( token[0] == '(' )
				myCurState = LHS_ALIAS_1;
			else
			{
				Error::get()->add( tsLine,
								   "Expected to see a \":\" following the LHS symbol \"%s\".",
								   myCurLHS.c_str() );
				myCurState = RESYNC_AFTER_RULE_ERROR;
			}
			break;
			
		case LHS_ALIAS_1:
			if ( isalpha( token[0] ) )
			{
				myCurLHSAlias = token;
				myCurState = LHS_ALIAS_2;
			}
			else
			{
				Error::get()->add( tsLine,
								   "'%s' is not a valid alias for the LHS '%s'.",
								   token.c_str(), myCurLHS.c_str() );
				myCurState = RESYNC_AFTER_RULE_ERROR;
			}
			break;
			
		case LHS_ALIAS_2:
			if( token[0] == ')' )
				myCurState = LHS_ALIAS_3;
			else
			{
				Error::get()->add( tsLine,
								   "Missing ')' following LHS alias name '%s'.",
								   myCurLHS.c_str() );
				myCurState = RESYNC_AFTER_RULE_ERROR;
			}
			break;
			
		case LHS_ALIAS_3:
			if ( token == "::=" )
				myCurState = IN_RHS;
			else
			{
				if ( ! myCurLHSAlias.empty() )
				{
					Error::get()->add( tsLine,
									   "Missing '::=' following: '%s(%s)'.",
									   myCurLHS.c_str(),
									   myCurLHSAlias.c_str() );
				}
				else
				{
					Error::get()->add( tsLine,
									   "Missing '::=' following: '%s'.",
									   myCurLHS.c_str() );
				}
				
				myCurState = RESYNC_AFTER_RULE_ERROR;
			}
			break;
			
		case IN_RHS:
			if ( token[0] == '.' )
			{
				myPrevRule = RuleTable::get()->createNewRule( myCurLHS );
				
				myPrevRule->setLHSAlias( myCurLHSAlias );
				myPrevRule->setRuleLine( tsLine );
				myPrevRule->setRHS( myCurRHS );
				
				myCurState = WAITING_FOR_DECL_OR_RULE;
			}
			else if ( isalpha( token[0] ) )
			{
				SymbolTable::get()->findOrCreate( token );
				myCurRHS.push_back( Rule::RHSEntry( token, std::string() ) );
			}
			else if ( token[0] == '(' && ! myCurRHS.empty() )
				myCurState = RHS_ALIAS_1;
			else
			{
				Error::get()->add( tsLine,
								   "Illegal identifier in RHS of rule: '%s'.",
								   token.c_str() );
				myCurState = RESYNC_AFTER_RULE_ERROR;
			}
			break;
			
		case RHS_ALIAS_1:
			if ( isalpha( token[0] ) )
			{
				myCurRHS.back().second = token;
				myCurState = RHS_ALIAS_2;
			}
			else
			{
				Error::get()->add( tsLine,
								   "'%s' is not a valid alias for RHS symbol '%s'.",
								   token.c_str(),
								   myCurRHS.back().first.c_str() );
				myCurState = RESYNC_AFTER_RULE_ERROR;
			}
			break;
			
		case RHS_ALIAS_2:
			if ( token[0] == ')' )
				myCurState = IN_RHS;
			else
			{
				Error::get()->add( tsLine,
								   "Missing ')' following RHS name '%s'.",
								   myCurRHS.back().first.c_str() );
				myCurState = RESYNC_AFTER_RULE_ERROR;
			}
			break;
			
		case WAITING_FOR_DECL_KEYWORD:
			if ( isalpha( token[0] ) )
			{
				if ( token == "name" ||
					 token == "namespace" ||
					 token == "header_include" ||
					 token == "include" ||
					 token == "code" ||
					 token == "token_destructor" ||
					 token == "token_prefix" ||
					 token == "syntax_error" ||
					 token == "parse_accept" ||
					 token == "parse_failure" ||
					 token == "stack_overflow" ||
					 token == "extra_argument" ||
					 token == "token_type" ||
					 token == "stack_size" ||
					 token == "start_symbol" )
				{
					myCurDeclKey = token;
					myCurState = WAITING_FOR_DECL_ARG;
				}
				else if ( token == "left" )
				{
					++myCurPrecCounter;
					myCurDeclAssoc = Symbol::LEFT;
					myCurState = WAITING_FOR_PRECEDENCE_SYMBOL;
				}
				else if ( token == "right" )
				{
					++myCurPrecCounter;
					myCurDeclAssoc = Symbol::RIGHT;
					myCurState = WAITING_FOR_PRECEDENCE_SYMBOL;
				}
				else if ( token == "nonassoc" )
				{
					++myCurPrecCounter;
					myCurDeclAssoc = Symbol::NONE;
					myCurState = WAITING_FOR_PRECEDENCE_SYMBOL;
				}
				else if ( token == "destructor" )
				{
					myCurState = WAITING_FOR_DESTRUCTOR_SYMBOL;
				}
				else if ( token == "type" )
				{
					myCurState = WAITING_FOR_DATATYPE_SYMBOL;
				}
				else
				{
					Error::get()->add( tsLine,
									   "Unknown declaration name '%s'.",
									   token.c_str() );
					myCurState = RESYNC_AFTER_DECL_ERROR;
				}
			}
			else
			{
				Error::get()->add( tsLine, "Invalid declaration symbol '%s'.",
								   token.c_str() );
				myCurState = RESYNC_AFTER_DECL_ERROR;
			}
			break;
			
		case WAITING_FOR_DESTRUCTOR_SYMBOL:
			if ( isalpha( token[0] ) )
			{
				SymbolTable::get()->findOrCreate( token );
				myCurDeclKey = token;
				myCurState = WAITING_FOR_DESTRUCTOR_DECL;
			}
			else
			{
				Error::get()->add( tsLine,
								   "Symbol name missing after %%destructor keyword." );
				myCurState = RESYNC_AFTER_DECL_ERROR;
			}
			break;
			
		case WAITING_FOR_DESTRUCTOR_DECL:
			if ( token[0] == '{' || token[0] == '\"' || isalnum( token[0] ) )
			{
				Symbol *destSym = SymbolTable::get()->findOrCreate( myCurDeclKey );
				
				if ( destSym->getDestructor().empty() )
				{
					std::string tmpVal = token;
					chompString( tmpVal, true );
					
					destSym->setDestructor( tmpVal );
					destSym->setDestructorLine( tsLine );
					myCurState = WAITING_FOR_DECL_OR_RULE;
				}
				else
				{
					Error::get()->add( tsLine,
									   "Duplicate definition of destructor for '%s'.",
									   myCurDeclKey.c_str() );
					myCurState = RESYNC_AFTER_DECL_ERROR;
				}
			}
			else
			{
				Error::get()->add( tsLine,
								   "Illegal argument to destructor declaration for '%s': '%s'.",
								   myCurDeclKey.c_str(), token.c_str() );
				myCurState = RESYNC_AFTER_DECL_ERROR;
			}
			break;
			
		case WAITING_FOR_DATATYPE_SYMBOL:
			if ( isalpha( token[0] ) )
			{
				SymbolTable::get()->findOrCreate( token );
				myCurDeclKey = token;
				myCurState = WAITING_FOR_DATATYPE_DECL;
			}
			else
			{
				Error::get()->add( tsLine,
								   "Symbol name missing after %%data keyword." );
				myCurState = RESYNC_AFTER_DECL_ERROR;
			}
			break;
			
		case WAITING_FOR_DATATYPE_DECL:
			if ( token[0] == '{' || token[0] == '\"' || isalnum( token[0] ) )
			{
				Symbol *destSym = SymbolTable::get()->findOrCreate( myCurDeclKey );
				
				if ( destSym->getDataType().empty() )
				{
					std::string tmpVal = token;
					chompString( tmpVal, true );
					
					destSym->setDataType( tmpVal );

					myCurState = WAITING_FOR_DECL_OR_RULE;
				}
				else
				{
					Error::get()->add( tsLine,
									   "Duplicate definition of datatype for '%s'.",
									   myCurDeclKey.c_str() );
					myCurState = RESYNC_AFTER_DECL_ERROR;
				}
			}
			else
			{
				Error::get()->add( tsLine,
								   "Illegal argument to data type declaration for '%s': '%s'.",
								   myCurDeclKey.c_str(), token.c_str() );
				myCurState = RESYNC_AFTER_DECL_ERROR;
			}
			break;
			
		case WAITING_FOR_PRECEDENCE_SYMBOL:
			if ( token[0] == '.' )
			{
				myCurState = WAITING_FOR_DECL_OR_RULE;
			}
			else if ( isupper( token[0] ) )
			{
				Symbol *precSym = SymbolTable::get()->findOrCreate( token );
				
				if ( precSym->getPrecedence() == -1 )
				{
					precSym->setPrecedence( myCurPrecCounter );
					precSym->setAssoc( myCurDeclAssoc );
				}
				else
				{
					Error::get()->add( tsLine,
									   "Symbol '%s' has already been given a precedence.",
									   precSym->getName().c_str() );
				}
			}
			else
				Error::get()->add( tsLine, "Unable to assign a precedence to '%s'.",
								   token.c_str() );
			break;
			
		case WAITING_FOR_DECL_ARG:
			if ( token[0] == '{' || token[0] == '\"' || isalnum( token[0] ) )
			{
				std::string tmpVal = token;
				chompString( tmpVal, true );
				if ( myCurGrammar->setValue( myCurDeclKey, tmpVal,
											 tsLine ) )
				{
					myCurState = WAITING_FOR_DECL_OR_RULE;
				}
				else
				{
					Error::get()->add( tsLine, "Duplicate definition of value for '%s'.",
									   myCurDeclKey.c_str() );
					myCurState = RESYNC_AFTER_DECL_ERROR;
				}
			}
			else
			{
				Error::get()->add( tsLine,
								   "Illegal argument to value setting for '%s': '%s'.",
								   myCurDeclKey.c_str(), token.c_str() );
				myCurState = RESYNC_AFTER_DECL_ERROR;
			}
			break;
			
		case RESYNC_AFTER_RULE_ERROR:
		case RESYNC_AFTER_DECL_ERROR:
			if ( token[0] == '.' )
				myCurState = WAITING_FOR_DECL_OR_RULE;
			if ( token[0] == '%' )
				myCurState = WAITING_FOR_DECL_KEYWORD;
			break;
			
		default:
			Error::get()->add( tsLine, "State machine into invalid state %d.",
							   myCurState );
			break;
	}
}


////////////////////////////////////////


bool
Parser::slurpFile( void )
{
	bool retval = false;
	
	FILE *input = std::fopen( mySourceFile.c_str(), "rb" );
	
	if ( input )
	{
		size_t fileSize = 0;
		
		if ( 0 == std::fseek( input, 0, SEEK_END ) )
		{
			fileSize = std::ftell( input );
			rewind( input );
		}
		
		if ( fileSize > 0 )
		{
			char *fileData = new char[ fileSize + 1 ];
			
			if ( fileSize == std::fread( fileData, 1,
										 fileSize, input ) )
			{
				fileData[fileSize] = '\0';
				mySourceData = fileData;
				retval = true;
			}
			else
				Error::get()->add( "Unable to read %ld bytes from file",
								   fileSize );
			
			delete [] fileData;
		}
		else
			Error::get()->add( "File is empty" );
	}
	else
		Error::get()->add( "Unable to open file for read" );
	
	return retval;
}


////////////////////////////////////////


void
Parser::chompString( std::string &str, bool innerWhite )
{
	while ( str[0] == ' ' || str[0] == '\t' )
		str.erase( str.begin() );
	if ( str[0] == '{' )
	{
		str.erase( str.begin() );
		
		while ( str[str.size() - 1] == ' ' ||
				str[str.size() - 1] == '\t' )
			str.erase( str.end() - 1 );
		
		if ( str[str.size() - 1] == '}' )
			str.erase( str.end() - 1 );
	}
	else if ( str[0] == '"' )
	{
		str.erase( str.begin() );
		
		while ( str[str.size() - 1] == ' ' ||
				str[str.size() - 1] == '\t' )
			str.erase( str.end() - 1 );
		
		if ( str[str.size() - 1] == '"' )
			str.erase( str.end() - 1 );
	}
	
	if ( innerWhite )
	{
		while ( str[0] == ' ' || str[0] == '\t' )
			str.erase( str.begin() );
		while ( str[str.size() - 1] == ' ' ||
				str[str.size() - 1] == '\t' )
			str.erase( str.end() - 1 );
	}
}

