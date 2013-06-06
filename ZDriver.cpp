/// @file ZDriver.cpp
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

#include <algorithm>
#include <functional>
#include <fstream>
#include <sstream>
#include <iostream>
#include <ctype.h>
#include "ZDriver.h"
#include "Version.h"
#include "Symbol.h"
#include "SymbolTable.h"
#include "State.h"
#include "StateTable.h"
#include "Rule.h"
#include "RuleTable.h"
#include "Util.h"

std::ostream &
operator<<( std::ostream &os, ZDriverOutHelp out )
{
	out.myOut->incOutLine();
	return std::flush( os.put( os.widen( '\n' ) ) );
}


////////////////////////////////////////


ZDriver::ZDriver( const Producer::ValueMap &valMap )
		: Producer( valMap )
{
	std::string extraArg = getValue( "extra_argument" ).first;

	if ( ! extraArg.empty() )
	{
		std::string::size_type varPos, comPos;

		myExtraArg = ", ";
		myExtraArg.append( extraArg );

		comPos = extraArg.find( ',' );
		while ( comPos != std::string::npos )
		{
			varPos = extraArg.find_last_of( " \t*&", comPos );
			if ( varPos != std::string::npos )
			{
				++varPos;
				if ( ! myExtraArgCall.empty() )
					myExtraArgCall.append( ", " );
				myExtraArgCall.append( extraArg.begin() + varPos,
									   extraArg.begin() + comPos );
			}
			comPos = extraArg.find( ',', comPos + 1 );
		}
		varPos = extraArg.find_last_of( " \t*&" );
		if ( varPos != std::string::npos )
		{
			++varPos;
			if ( ! myExtraArgCall.empty() )
				myExtraArgCall.append( ", " );
			myExtraArgCall.append( extraArg.begin() + varPos,
								   extraArg.end() );
		}
	}

	myNameSpace = getValue( "namespace" ).first;
}


////////////////////////////////////////


ZDriver::~ZDriver( void )
{
}


////////////////////////////////////////


bool
ZDriver::writeHeader( void )
{
	const std::string &tokenType = getValue( "token_type" ).first;
	const std::string &prefix = getValue( "token_prefix" ).first;
	std::string fileName = getOutputDir();
	std::ofstream out;
	bool isOk = false;

	getFileName( fileName, ".h" );

	out.open( fileName.c_str() );
	if ( out.is_open() )
	{
		size_t i, nSym;

		myCurLineNum = 0;
		myPimplName = "priv";
		myPimplName.append( getParserName() );
		myPimplName.append( "Impl" );


		std::string nsStart, nsEnd;
		std::string poundDef = "_";

		if ( ! myNameSpace.empty() )
		{
			std::string::size_type nsPos, oldPos;

			nsStart = myNameSpace;

			// Cheat so we get the proper number of :: replacements...
			if ( *(nsStart.end() - 1) != ':' )
				nsStart.append( "::" );

			oldPos = 0;
			nsPos = nsStart.find( "::" );
			while ( nsPos != std::string::npos )
			{
				poundDef.append( nsStart.begin() + oldPos,
								 nsStart.begin() + nsPos );
				poundDef.push_back( '_' );
				nsStart.replace( nsPos, 2, "\n{\n" );
				nsStart.insert( oldPos, "namespace " );
				nsEnd.append( "\n} // namespace\n" );
				oldPos = nsPos + 13; // len("namespace ") + len("\n{\n")
				nsPos = nsStart.find( "::", nsPos );
			}
		}

		poundDef.append( getParserName() );
		poundDef.append( "_h_" );

		out << "// This file auto-generated from " << getParserName()
			<< ".lem by " << VersionInfo::appName() << " version "
			<< VersionInfo::appVersion() << endl();
		out << "// Editing of this file strongly discouraged." << endl();

		out << endl() << "#ifndef " << poundDef << endl();
		out << "#define " << poundDef << endl();

		emitValue( getValue( "header_include" ), out );
		out << endl() << "class " << myPimplName << ";" << endl();

		out << endl() << endl() << nsStart << endl();
		myCurLineNum += std::count_if( nsStart.begin(), nsStart.end(),
									   std::bind2nd( std::equal_to<char>(), '\n' ) );

		out << "class " << getParserName() << endl() << "{" << endl()
			<< "public:" << endl();

		// Generate the terminal tokens that the lexer will feed us
		int idx = 1;

		nSym = SymbolTable::get()->getNumSymbols();
		out << endl() << "    enum Terminal" << endl() << "    {" << endl();
		out << "        " << prefix << "EOF = 0";
		for ( i = 0; i < nSym; ++i )
		{
			Symbol *sp = SymbolTable::get()->getNthSymbol( i );

			if ( Symbol::TERMINAL == sp->getType() )
			{
				out << "," << endl();
				out << "        " << prefix << sp->getName() << " = " << idx;
				++idx;
			}
		}
		out << endl() << "    };";

		// Finish out the file
		out << endl() << endl() << "    " << getParserName() << "( void );"
			<< endl();
		out << "    ~" << getParserName() << "( void );" << endl();
		out << endl() << endl();
		out << "    void parse( " << getParserName() << "::Terminal tok, ";

		if ( tokenType.empty() )
			out << "void *value";
		else
		{
			out << tokenType;
			if ( *(tokenType.end() - 1) != '*' && *(tokenType.end() - 1) != '&' )
				out << " value";
			else
				out << "value";
		}
		out << myExtraArg;
		out << " );" << endl();

		out << endl() << "private:" << endl();
		out << "	// No copying of this class" << endl();
		out << "	" << getParserName() << "( const " << getParserName()
			<< " & );" << endl();
		out << "	" << getParserName() << " &operator=( const "
			<< getParserName() << " & );" << endl();
		out << "    ValuePtr<" << myPimplName << "> myImplementation;" << endl();
		out << endl() << endl() << "};" << endl();
		out << endl() << nsEnd << endl() << endl();
		myCurLineNum += std::count_if( nsEnd.begin(), nsEnd.end(),
									   std::bind2nd( std::equal_to<char>(), '\n' ) );
		out << "#endif /* " << poundDef << " */ " << endl();
		out.close();
		isOk = true;
	}

	return isOk;
}


////////////////////////////////////////


bool
ZDriver::writeSource( void )
{
	std::ofstream out;
	bool isOk = false;

	getFileName( myFileName, ".cpp" );
	out.open( myFileName.c_str() );

	if ( out.is_open() )
	{
		myCurLineNum = 1;
		myPimplName = "priv";
		myPimplName.append( getParserName() );
		myPimplName.append( "Impl" );

		out << "// This file auto-generated from " << getParserName()
			<< ".lem by " << VersionInfo::appName() << " version "
			<< VersionInfo::appVersion() << endl();
		out << "// Editing of this file strongly discouraged." << endl();

		emitValue( getValue( "include" ), out );
		
		out << endl() << "#include <Core.h>" << endl();
		
		std::string incName;
		Util::getFileName( incName, std::string(), getSourceFile(), ".h" );
		out << endl() << "#include \"" << incName << "\"" << endl();

		out << endl() << "#include <utility>" << endl();
		out << "#include <stack>" << endl();
		out << "#include <map>" << endl();
		out << "#include <vector>" << endl();
		out << "#include <iostream>" << endl();
		out << "#include <Util/Any.h>" << endl();

		if ( ! myNameSpace.empty() )
		{
			emitFuncBreak( out );
			out << "using " << myNameSpace;
			if ( *(myNameSpace.end()-1) != ':' )
				out << "::";
			out << getParserName() << ";" << endl();
		}
		writeImplClassDecl( out );
		writeParserCtorDtor( out );
		writeMainParserFunc( out );
		writeImplClassCtorDtor( out );
		writeShiftFunc( out );
		writeReduceFunc( out );
		writeAcceptFunc( out );
		writeDestructorHandler( out );
		writeParserUtil( out );
		writeErrorRoutines( out );
		emitValue( getValue( "code" ), out );

		isOk = true;
	}

	return isOk;
}


////////////////////////////////////////


void
ZDriver::writeParserCtorDtor( std::ostream &out )
{
	emitFuncBreak( out );
	out << getParserName() << "::" << getParserName()
		<< "( void )" << endl();
	out << "    : myImplementation( new " << myPimplName << " )" << endl();
	out << "{" << endl();
	out << "}" << endl();

	emitFuncBreak( out );
	out << getParserName() << "::~" << getParserName()
		<< "( void )" << endl();
	out << "{" << endl();
	out << "}" << endl();

	const std::string &tokenType = getValue( "token_type" ).first;

	emitFuncBreak( out );
	out << "void " << getParserName() << "::parse( " << getParserName()
		<< "::Terminal tok, ";
	if ( tokenType.empty() )
		out << "void *";
	else
	{
		out << tokenType;
		if ( *(tokenType.end() - 1) != '*' && *(tokenType.end() - 1) != '&' )
			out << " ";
	}
	out << "value" << myExtraArg << " )" << endl();
	out << "{" << endl();
	out << "    myImplementation->parse( tok, value";
	if ( ! myExtraArgCall.empty() )
		out << ", " << myExtraArgCall;
	out << " );" << endl();
	out << "}" << endl();
}


////////////////////////////////////////


void
ZDriver::writeImplClassDecl( std::ostream &out )
{
	const std::string &tokenType = getValue( "token_type" ).first;

	out << endl();
	out << "enum ParserAct" << endl();
	out << "{" << endl();
	out << "    PA_SHIFT," << endl();
	out << "    PA_REDUCE," << endl();
	out << "    PA_ERROR," << endl();
	out << "    PA_ACCEPT," << endl();
	out << "    PA_NOP" << endl();
	out << "};" << endl() << endl();

	out << "class " << myPimplName << endl() << "{" << endl();
	out << "public:" << endl() << endl();
	out << "    " << myPimplName << "( void );" << endl();
	out << "    ~" << myPimplName << "( void );" << endl();
	out << endl() << endl();
	out << "    void parse( " << getParserName() << "::Terminal tok, ";

	if ( tokenType.empty() )
		out << "void *value";
	else
	{
		out << tokenType;
		if ( *(tokenType.end() - 1) != '*' && *(tokenType.end() - 1) != '&' )
			out << " value";
		else
			out << "value";
	}
	out << myExtraArg;
	out << " );" << endl();

	out << endl() << "private:" << endl();
	out << "    // first is stack num, second is the symbol index" << endl();
	out << "    typedef std::pair<int,int> StackID;" << endl();
	out << "    typedef std::pair<StackID,Util::Any> StackEntry;" << endl();
	out << "    typedef std::stack<StackEntry> ParseStack;" << endl();

	out << endl();

	writeShiftFuncDecl( out );
	writeReduceFuncDecl( out );
	writeAcceptFuncDecl( out );
	writeDestructorHandlerDecl( out );
	writeParserUtilDecl( out );
	writeErrorRoutinesDecl( out );

	out << endl()
		<< "    ParseStack myStack;" << endl();
	out << "    int myErrCount;" << endl();

	writeStateTableDecl( out );
	writeRuleTableDecl( out );

	out << "};" << endl();
}


////////////////////////////////////////


void
ZDriver::writeImplClassCtorDtor( std::ostream &out )
{
	emitFuncBreak( out );
	out << myPimplName << "::" << myPimplName
		<< "( void )" << endl();
	out << "    : myErrCount( -1 )" << endl();
	out << "{" << endl();
	out << "    initTables();" << endl();
	out << "}" << endl();

	emitFuncBreak( out );
	out << myPimplName << "::~" << myPimplName
		<< "( void )" << endl();
	out << "{" << endl();
	out << "    while ( ! myStack.empty() )" << endl();
	out << "        popStack();" << endl();
	out << "}" << endl();
}


////////////////////////////////////////


void
ZDriver::writeStateTableDecl( std::ostream &out )
{
	out << endl() << endl();
	out << "    typedef std::pair<ParserAct,int> ActionEntry;" << endl();
	out << "    typedef std::map<int,ActionEntry> ActionMap;" << endl();
	out << "    typedef std::vector<ActionMap> StateList;" << endl();
	out << "    typedef std::vector<ActionEntry> StateDefaultList;" << endl();
	out << endl();
	out << "    StateList myStates;" << endl();
	out << "    StateDefaultList myStateDefaultActions;" << endl();
}


////////////////////////////////////////


void
ZDriver::buildStateTable( std::ostream &out )
{
	size_t i, j, nState, nAct, nTotal;

	nState = StateTable::get()->getNumStates();

	// Count all actions
	nTotal = 0;
	for ( i = 0; i < nState; ++i )
	{
		State		*stp = StateTable::get()->getNthState(i);
		ActionList	&ap = stp->getActions();

		nAct = ap.getNumActions();
		for ( j = 0; j < nAct; ++j )
		{
			Action &act = ap.getNthAction( j );
			if ( ! act.isIgnoreType() )
				nTotal++;
			if ( act.getLookAhead() == SymbolTable::get()->getDefaultName() )
				nTotal++;
		}
	}


	out << "    myStates.assign( " << nState << ", ActionMap() );" << endl();
	out << "    myStateDefaultActions.assign( " << nState
		<< ", ActionEntry(PA_ERROR,-2) );" << endl();

	out << "    for ( int i = 0; i < " << nTotal << "; ++i )" << endl();
	out << "    {" << endl();
	out << "        if ( theStateTable[i][1] == -1 )" << endl();
	out << "            myStateDefaultActions[theStateTable[i][0]] = ActionEntry((ParserAct)theStateTable[i][2], theStateTable[i][3]);" << endl();
	out << "        else" << endl();
	out << "            myStates[theStateTable[i][0]][theStateTable[i][1]] = ActionEntry((ParserAct)theStateTable[i][2], theStateTable[i][3]);" << endl();
	out << "    }" << endl();
}


////////////////////////////////////////


void
ZDriver::writeStateTable( std::ostream &out )
{
	size_t i, j, nState, nAct, nTotal;

	nState = StateTable::get()->getNumStates();

	// Count all actions
	nTotal = 0;
	for ( i = 0; i < nState; ++i )
	{
		State		*stp = StateTable::get()->getNthState(i);
		ActionList	&ap = stp->getActions();

		nAct = ap.getNumActions();
		for ( j = 0; j < nAct; ++j )
		{
			Action &act = ap.getNthAction( j );
			if ( ! act.isIgnoreType() )
				nTotal++;
			if ( act.getLookAhead() == SymbolTable::get()->getDefaultName() )
				nTotal++;
		}
	}

	out << "static int theStateTable[" << nTotal << "][4] =" << endl();
	out << "{" << endl();

	for ( i = 0; i < nState; ++i )
	{
		State		*stp = StateTable::get()->getNthState(i);
		ActionList	&ap = stp->getActions();

		nAct = ap.getNumActions();
		for ( j = 0; j < nAct; ++j )
		{
			Action &act = ap.getNthAction( j );

			if ( ! act.isIgnoreType() )
			{
				out << "    // State " << i << endl();
				out << "    { " << i << ", " << act.getLookAheadSymbol()->getIndex() << ", ";
				emitAction( act, out );
				out << " }," << endl();
			}

			if ( act.getLookAhead() == SymbolTable::get()->getDefaultName() )
			{
				out << "    // State " << i << " default action" << endl();
				out << "    { " << i << ", " << -1 << ", ";
				emitAction( act, out );
				out << " }," << endl();
			}
		}
	}
	out << "};" << endl();
}


////////////////////////////////////////


void
ZDriver::writeRuleTableDecl( std::ostream &out )
{
	out << endl() << endl();
	out << "    // first is LHS, second is number of RHS" << endl();
	out << "    typedef std::pair<int,int> RuleInfo;" << endl();
	out << "    typedef std::vector<RuleInfo> RuleList;" << endl();
	out << endl();
	out << "    RuleList myRules;" << endl();
}


////////////////////////////////////////


void
ZDriver::writeRuleTable( std::ostream &out )
{
	size_t i, nRule;

	nRule = RuleTable::get()->getNumRules();

	out << endl() << endl();
	out << "static int theRuleTable[" << nRule << "][2] =" << endl();
	out << "{" << endl();

	for ( i = 0; i < nRule; ++i )
	{
		Rule *rp = RuleTable::get()->getNthRule( i );
		out << "    // ";
		rp->print( out );
		out << endl();
		out << "    { " << rp->getLHSSymbol()->getIndex()
			<< ", " << rp->getRHS().size() << " }, " << endl();
	}

	out << "};" << endl();
}


////////////////////////////////////////


void
ZDriver::buildRuleTable( std::ostream &out )
{
	size_t nRule;

	nRule = RuleTable::get()->getNumRules();

	out << endl() << endl();
	out << "    // Rule Table" << endl() << endl();
	out << "    myRules.reserve( " << nRule << " );" << endl();

	out << "    for ( int i = 0; i < " << nRule << "; ++i )" << endl();
	out << "        myRules.push_back( RuleInfo( theRuleTable[i][0], theRuleTable[i][1] ) );" << endl();
}


////////////////////////////////////////


void
ZDriver::writeMainParserFunc( std::ostream &out )
{
	const std::string &tokenType = getValue( "token_type" ).first;
	const std::string &prefix = getValue( "token_prefix" ).first;

	emitFuncBreak( out );
	out << "void " << myPimplName << "::parse( " << getParserName()
		<< "::Terminal tok, ";
	if ( tokenType.empty() )
		out << "void *";
	else
	{
		out << tokenType;
		if ( *(tokenType.end() - 1) != '*' && *(tokenType.end() - 1) != '&' )
			out << " ";
	}
	out << "value" << myExtraArg << " )" << endl();
	out << "{" << endl();

	out << "    int actVal;" << endl();
	out << "    ParserAct action;" << endl();
	out << "    bool errHit = false;" << endl();
	out << "    bool eoInput = (tok == " << getParserName() << "::"
		<< prefix << "EOF );" << endl();
	out << "    bool done = false;" << endl();
	out << endl();
	out << "    if ( myStack.empty() )" << endl();
	out << "    {" << endl();
	out << "        if ( eoInput )" << endl();
	out << "            return;" << endl();
	out << endl();
	out << "        myErrCount = -1;" << endl();
	out << "    }" << endl();
	out << endl();
	out << "    Util::Any data( value );" << endl();
	out << endl();
	out << "    do" << endl() << "    {" << endl();
	out << "        action = findParserAction( actVal, tok );" << endl();
	out << "        if ( PA_SHIFT == action )" << endl();
	out << "        {" << endl();
	out << "            shift( actVal, tok, data );" << endl();
	out << "            --myErrCount;" << endl();
	out << "            if ( eoInput && ! myStack.empty() )" << endl();
	out << "                tok = " << getParserName() << "::"
		<< prefix << "EOF;" << endl();
	out << "            else" << endl();
	out << "                done = true;" << endl();
	out << "        }" << endl();
	out << "        else if ( PA_REDUCE == action )" << endl();
	out << "        {" << endl();
	if ( ! myExtraArgCall.empty() )
		out << "            reduce( actVal, " << myExtraArgCall << " );" << endl();
	else
		out << "            reduce( actVal );" << endl();
	out << "        }" << endl();
	out << "        else if ( PA_ERROR == action )" << endl();
	out << "        {" << endl();
	emitErrorHandling( out );
	out << "        }" << endl();
	out << "        else // ACCEPT == action || NOP == action" << endl();
	out << "        {" << endl();
	if ( isValueSet( "parse_accept" ) )
	{
		if ( !myExtraArgCall.empty() )
			out << "            accept( " << myExtraArgCall << " );" << endl();
		else
			out << "            accept();" << endl();
	}
	out << "            done = true;" << endl();
	out << "        }" << endl();
	out << "    } while ( ! done && ! myStack.empty() );" << endl();
	out << "}" << endl();
}


////////////////////////////////////////


void
ZDriver::writeShiftFuncDecl( std::ostream &out )
{
	out << "    void shift( int newState, int symIdx, const Util::Any &data );"
		<< endl();
}


////////////////////////////////////////


void
ZDriver::writeShiftFunc( std::ostream &out )
{
	emitFuncBreak( out );
	out << "void " << myPimplName
		<< "::shift( int newState, int symIdx, const Util::Any &data )"
		<< endl();
	out << "{" << endl();

	if ( isDebugOutput() )
		out << "    std::cout << \"SHIFT to state \" << newState << std::endl;"
			<< endl();

	out << "    StackEntry newItem = StackEntry( StackID( newState, symIdx ),"
		<< "									 Util::Any( data ) );"
		<< endl();
	out << "    myStack.push( newItem );" << endl();
	out << "}" << endl();
}


////////////////////////////////////////


void
ZDriver::writeReduceFuncDecl( std::ostream &out )
{
	out << "    void reduce( int ruleNum" << myExtraArg << " );" << endl();
}


////////////////////////////////////////


void
ZDriver::writeReduceFunc( std::ostream &out )
{
	size_t i, nRule;

	nRule = RuleTable::get()->getNumRules();

	emitFuncBreak( out );
	out << "void " << myPimplName << "::reduce( int ruleNum"
		<< myExtraArg << " )" << endl();
	out << "{" << endl();
	if ( isDebugOutput() )
		out << "    std::cout << \"REDUCE rule \" << ruleNum << std::endl;"
			<< endl();
	out << "    int newVal;" << endl();
	out << "    ParserAct next;" << endl();
	out << "    Util::Any data;" << endl();
	out << "    std::vector< Util::Any > rhsData;" << endl() << endl();
	out << "    rhsData.reserve( myRules[ruleNum].second );" << endl();
	out << "    for ( int i = 0, N = myRules[ruleNum].second; i != N; ++i )"
		<< endl();
	out << "    {" << endl();
	out << "        if ( myStack.empty() )" << endl()
		<< "            rhsData.insert( rhsData.begin(), data );" << endl()
		<< "        else" << endl()
		<< "        {" << endl();
	out << "            rhsData.insert( rhsData.begin(), myStack.top().second );"
		<< endl();
	out << "            myStack.pop();" << endl()
		<< "        }" << endl();
	out << "    }" << endl();

	out << endl();
    out << "    next = findParserAction( newVal, myRules[ruleNum].first );"
		<< endl();

	out << endl();
	out << "    switch ( ruleNum )" << endl();
	out << "    {" << endl();
	for ( i = 0; i < nRule; ++i )
	{
		Rule *rp = RuleTable::get()->getNthRule( i );

		out << "        case " << i << ":" << endl();
		out << "        {" << endl();
		out << "            // ";
		rp->print( out );
		out << endl();

		emitRule( rp, out );

		out << "            break;" << endl();
		out << "        }" << endl() << endl();
	}

	out << "        default:" << endl();
	out << "            throw \"Unknown Rule Number\";" << endl();
	out << "            break;" << endl();
	out << "    }" << endl();

	out << endl();
	out << "    if ( PA_SHIFT == next )" << endl();
	out << "        shift( newVal, myRules[ruleNum].first, data );" << endl();
	if ( isValueSet( "parse_accept" ) )
	{
		out << "    else" << endl();
		if ( !myExtraArgCall.empty() )
			out << "        accept( " << myExtraArgCall << " );" << endl();
		else
			out << "        accept();" << endl();
	}

	out << "}" << endl();
}


////////////////////////////////////////


void
ZDriver::writeAcceptFuncDecl( std::ostream &out )
{
	const std::string &extraArg = getValue( "extra_argument" ).first;

	if ( isValueSet( "parse_accept" ) )
	{
		if ( extraArg.empty() )
			out << "    void accept( void );" << endl();
		else
			out << "    void accept( " << extraArg << " );" << endl();
	}
}


////////////////////////////////////////


void
ZDriver::writeAcceptFunc( std::ostream &out )
{
	const std::string &extraArg = getValue( "extra_argument" ).first;

	if ( isValueSet( "parse_accept" ) )
	{
		emitFuncBreak( out );
		if ( extraArg.empty() )
			out << "void " << myPimplName << "::accept( void )" << endl();
		else
			out << "void " << myPimplName << "::accept( "
				<< extraArg << " )" << endl();
		out << "{" << endl();

		emitValue( getValue( "parse_accept" ), out );

		out << "}" << endl();
	}
}


////////////////////////////////////////


void
ZDriver::writeDestructorHandlerDecl( std::ostream &out )
{
	out << "    void callDtor( int symIdx, Util::Any &data );" << endl();
}


////////////////////////////////////////


void
ZDriver::writeDestructorHandler( std::ostream &out )
{
	size_t i, nSym;

	nSym = SymbolTable::get()->getNumSymbols();

	emitFuncBreak( out );
	out << "void " << myPimplName
		<< "::callDtor( int symIdx, Util::Any &data )"
		<< endl();
	out << "{" << endl();
	out << "    switch ( symIdx )" << endl();
	out << "    {" << endl();
	out << "        case 0:" << endl();
	for ( i = 0; i < nSym; ++i )
	{
		Symbol *sp = SymbolTable::get()->getNthSymbol( i );
		if ( Symbol::TERMINAL == sp->getType() )
		{
			out << "        case " << i << ":" << endl();
		}
	}
	const std::string &tokenDtor = getValue( "token_destructor" ).first;
	const std::string &tokenType = getValue( "token_type" ).first;

	if ( ! tokenDtor.empty() )
	{
		out << "            ";
		emitDestructor( tokenType, tokenDtor, out );
		out << endl();
	}
	out << "            break;" << endl();

	for ( i = 0; i < nSym; ++i )
	{
		Symbol *sp = SymbolTable::get()->getNthSymbol( i );
		const std::string &dtor = sp->getDestructor();

		if ( Symbol::NONTERMINAL == sp->getType() && ! dtor.empty() )
		{
			out << "        case " << i << ":" << endl();
			out << "            ";
			emitDestructor( sp->getDataType(), tokenDtor, out );
			out << endl();
			out << "            break;" << endl();
		}
	}

	out << "        default:" << endl();
	out << "            break;" << endl();
	out << "    }" << endl();
	out << "}" << endl();
}


////////////////////////////////////////


void
ZDriver::writeParserUtilDecl( std::ostream &out )
{
	out << "    void popStack( void );" << endl();
	out << "    ParserAct findParserAction( int &newVal, int tok );"
		<< endl();
	out << "    void initTables( void );" << endl();
}


////////////////////////////////////////


void
ZDriver::writeParserUtil( std::ostream &out )
{
	emitFuncBreak( out );
	out << "void " << myPimplName << "::popStack( void )" << endl();
	out << "{" << endl();
	out << "    callDtor( myStack.top().first.second, myStack.top().second );"
		<< endl();
	out << "    myStack.pop();" << endl();
	out << "}" << endl();

	emitFuncBreak( out );
	out << "ParserAct " << myPimplName
		<< "::findParserAction( int &newVal, int tok )" << endl();
	out << "{" << endl();
	out << "    int stateNum;" << endl();
	out << "    ParserAct retval = PA_NOP;" << endl();
	out << "    bool found = false;" << endl();
	out << endl();
	out << "    stateNum = myStack.empty() ? 0 : myStack.top().first.first;"
		<< endl();
	out << "    ActionMap::iterator i = myStates[stateNum].find( tok );"
		<< endl();
	out << "    if ( tok >= 0 && tok <= "
		<< SymbolTable::get()->getNumSymbols() << " )" << endl();
	out << "    {" << endl();
	out << "        if ( i != myStates[stateNum].end() )" << endl();
	out << "        {" << endl();
	out << "            retval = (*i).second.first;" << endl();
	out << "            newVal = (*i).second.second;" << endl();
	out << "            found = true;" << endl();
	out << "        }" << endl();
	out << "    }" << endl();
	out << "    else if ( ! myStates[stateNum].empty() )" << endl();
	out << "    {" << endl();
	out << "        retval = PA_NOP;" << endl();
	out << "        newVal = 0;" << endl();
	out << "        found = true;" << endl();
	out << "    }" << endl();
	out << endl();
	out << "    if ( ! found )" << endl();
	out << "    {" << endl();
	out << "        retval = myStateDefaultActions[stateNum].first;" << endl();
	out << "        newVal = myStateDefaultActions[stateNum].second;" << endl();
	out << "    }" << endl();

	out << "    return retval;" << endl();
	out << "}" << endl();

	emitFuncBreak( out );

	writeStateTable( out );
	writeRuleTable( out );

	emitFuncBreak( out );

	out << "void " << myPimplName << "::initTables( void )" << endl();
	out << "{" << endl();
	buildStateTable( out );
	buildRuleTable( out );
	out << "}" << endl();
}


////////////////////////////////////////


void
ZDriver::writeErrorRoutinesDecl( std::ostream &out )
{
	const std::string &extraArg = getValue( "extra_argument" ).first;

	out << "    void syntaxError( " << getParserName()
		<< "::Terminal tok, const Util::Any &data" << myExtraArg
		<< " );" << endl();

	if ( extraArg.empty() )
		out << "    void parseFailed( void );" << endl();
	else
		out << "    void parseFailed( " << extraArg << " );" << endl();
}


////////////////////////////////////////


void
ZDriver::writeErrorRoutines( std::ostream &out )
{
	const std::string &tokenType = getValue( "token_type" ).first;
	const std::string &extraArg = getValue( "extra_argument" ).first;

	emitFuncBreak( out );
	out << "void " << myPimplName << "::syntaxError( "
		<< getParserName()
		<< "::Terminal tok, const Util::Any &data"
		<< myExtraArg << " )" << endl();
	out << "{" << endl();
	const std::string &seStr = getValue( "syntax_error" ).first;

	if ( isDebugOutput() )
	{
		out << "    std::cout << \"SYNTAX ERROR with token \" << tok << std::endl;"
			<< endl();
	}

	if ( ! seStr.empty() )
	{
		if ( seStr.find( "TOKEN" ) != std::string::npos )
		{
			out << "    const ";
			if ( tokenType.empty() )
				out << "void *";
			else
			{
				out << tokenType;
				if ( *(tokenType.end() - 1) != '*' &&
					 *(tokenType.end() - 1) != '&' )
					out << " ";
			}
			out << "TOKEN = Util::any_cast< const ";
			if ( tokenType.empty() )
				out << "void *";
			else
				out << tokenType;
			out << " >( data );" << endl();
		}
		emitValue( getValue( "syntax_error" ), out );
	}
	else
		std::cout << "WARNING: missing syntax_error directive" << std::endl;

	out << "}" << endl();

	emitFuncBreak( out );
	out << "void " << myPimplName << "::parseFailed( ";
	if ( extraArg.empty() )
		out << "void";
	else
		out << extraArg;
	out << " )" << endl();
	out << "{" << endl();

	if ( isDebugOutput() )
	{
		out << "    std::cout << \"PARSE FAILURE\" << std::endl;"
			<< endl();
	}

	if (getValue( "parse_failure" ).first.empty())
		std::cout << "WARNING: missing parse_failure directive" << std::endl;

	emitValue( getValue( "parse_failure" ), out );
	out << "}" << endl();
}


////////////////////////////////////////


void
ZDriver::emitErrorHandling( std::ostream &out )
{
	const std::string &prefix = getValue( "token_prefix" ).first;
	std::string indent = "            ";

	out << indent << "// Syntax error handling:" << endl();
	out << indent << "// 1. Call the %syntax_error function." << endl();
	out << indent << "// 2. Pop stack until a state where we can shift the err symbol." << endl();
	out << indent << "// 3. Shift the error symbol." << endl();
	out << indent << "// 4. Set error count to three." << endl();
	out << indent << "// 5. Begin accepting and shifting new tokens." << endl();
	out << indent << "// 6. No new error processing will begin until 3 tokens are successful." << endl();
	out << indent << "if ( myErrCount < 0 )" << endl();
	out << indent << "{" << endl();
	out << indent << "    syntaxError( tok, data";
	if ( ! myExtraArgCall.empty() )
		out << ", " << myExtraArgCall;
	out << " );" << endl();
	out << indent << "}" << endl();

	Symbol *errsp = SymbolTable::get()->find( "error" );
	out << indent << "if ( myStack.top().first.first == " << errsp->getIndex()
		<< " || errHit )" << endl();
	out << indent << "{" << endl();
	out << indent << "    callDtor( static_cast<int>(tok), data );" << endl();
	out << indent << "    done = true;" << endl();
	out << indent << "}" << endl();
	out << indent << "else" << endl();
	out << indent << "{" << endl();
	out << indent << "    while ( ! myStack.empty() &&" << endl();
	out << indent << "            myStack.top().first.first != "
		<< errsp->getIndex() << " )" << endl();
	out << indent << "    {" << endl();
	out << indent << "        action = findParserAction( actVal, "
		<< errsp->getIndex() << " );" << endl();
	out << indent << "        if ( action == PA_SHIFT )" << endl();
	out << indent << "            break;" << endl();
	out << indent << "        popStack();" << endl();
	out << indent << "    }" << endl();
	out << indent << "    if ( myStack.empty() || tok == "
		<< getParserName() << "::" << prefix << "EOF )" << endl();
	out << indent << "    {" << endl();
	out << indent << "        callDtor( static_cast<int>(tok), data );"
		<< endl();
	out << indent << "        parseFailed(";
	if ( ! myExtraArgCall.empty() )
		out << " " << myExtraArgCall << " ";
	out << ");" << endl();
	out << indent << "        done = true;" << endl();
	out << indent << "    }" << endl();
	out << indent << "    else if ( myStack.top().first.first != "
		<< errsp->getIndex() << " )" << endl();
	out << indent << "    {" << endl();
	out << indent << "        data = static_cast<void *>(0);"
		<< endl();
	out << indent << "        shift( actVal, " << errsp->getIndex()
		<< ", data );" << endl();
	out << indent << "    }" << endl();
	out << indent << "}" << endl();

	out << indent << "myErrCount = 3;" << endl();
	out << indent << "errHit = true;" << endl();
}


////////////////////////////////////////


void
ZDriver::emitValue( const Producer::ValueSetting	&val,
					std::ostream					&out )
{
	if ( ! val.first.empty() )
	{
		emitLineInfo( getSourceFile(), val.second, out );
		out << val.first << endl();
		myCurLineNum += std::count_if( val.first.begin(), val.first.end(),
									   std::bind2nd( std::equal_to<char>(), '\n' ) );
		emitLineInfo( myFileName, getOutLine(), out );
	}
}


////////////////////////////////////////


void
ZDriver::emitDestructor( const std::string	&type,
						 const std::string	&dtor,
						 std::ostream		&out )
{
	std::string tmpdtor = dtor;
	std::string delName = "Util::any_cast< ";
	size_t ddPos;
	
	delName.append( type );
	delName.append( " >( data )" );

	ddPos = tmpdtor.find( "$$" );
	while ( ddPos != std::string::npos )
	{
		tmpdtor.replace( ddPos, 2, delName );
		ddPos = tmpdtor.find( "$$" );
	}
	out << tmpdtor;
}


////////////////////////////////////////


void
ZDriver::emitLineInfo( const std::string	&srcFile,
					   int				 line,
					   std::ostream		&out )
{
	out << "#line " << line << " \"" << srcFile << "\"" << endl();
}


////////////////////////////////////////


void
ZDriver::emitAction( const Action &act, std::ostream &out )
{
	switch ( act.getType() )
	{
		case Action::SHIFT:
			out << "PA_SHIFT, "
				<< act.getState()->getStateIndex();
			break;

		case Action::REDUCE:
			out << "PA_REDUCE, "
				<< act.getRule()->getRuleIndex();
			break;

		case Action::ACCEPT:
			out << "PA_ACCEPT, 0";
			break;

		case Action::ERROR:
			out << "PA_ERROR, -2";
			break;

		default:
			out << "PA_NOP, -1";
			break;
	}
}


////////////////////////////////////////


void
ZDriver::emitRule( const Rule *rp, std::ostream &out )
{
	std::string rpCode = rp->getCode();
	std::string::size_type curPos;
	int codeLine = rp->getCodeLine();

	while ( rpCode[0] == ' ' || rpCode[0] == '\t' || rpCode[0] == '\n' )
	{
		if ( rpCode[0] == '\n' )
			++codeLine;
		rpCode.erase( rpCode.begin() );
	}

	emitLineInfo( getSourceFile(), codeLine, out );

	const Rule::RHSList &rhs = rp->getRHS();
	Rule::RHSListConstIter ri, re;

	if ( ! rpCode.empty() )
	{
		curPos = 0;
		rpCode.insert( curPos, "            " );
		while ( curPos != std::string::npos )
		{
			curPos = rpCode.find( '\n', curPos );
			if ( curPos != std::string::npos )
			{
				incOutLine();
				++curPos;
				rpCode.insert( curPos, "        " );
			}
		}

		while ( rpCode[rpCode.size() - 1] == ' ' ||
				rpCode[rpCode.size() - 1] == '\t' )
			rpCode.erase( rpCode.end() - 1 );

		std::string replStr = "Util::any_cast< ";
		Symbol *lhsSym = SymbolTable::get()->find( rp->getLHS() );
		replStr.append( lhsSym->getDataType() );
		replStr.append( " >( data )" );
		substCode( rpCode, rp->getLHSAlias(), replStr, true,
				   lhsSym->getDataType() );

		const std::string &tokenType = getValue( "token_type" ).first;
		
		for ( ri = rhs.begin(), re = rhs.end(); ri != re; ++ri )
		{
			Symbol *sp = SymbolTable::get()->find( (*ri).first );
			const std::string &dataType = sp->getDataType();
			std::ostringstream tmpOut;
			
			replStr = "Util::any_cast< ";
			if ( Symbol::NONTERMINAL == sp->getType() && ! dataType.empty() )
				replStr.append( dataType );
			else
				replStr.append( tokenType );
			replStr.append( " >( rhsData[" );
			tmpOut << ( ri - rhs.begin() );
			replStr.append( tmpOut.str() );
			replStr.append( "] )" );

			substCode( rpCode, (*ri).second, replStr );
		}

		out << rpCode;
	}

	emitLineInfo( myFileName, getOutLine(), out );

	for ( ri = rhs.begin(), re = rhs.end(); ri != re; ++ri )
	{
		Symbol *sp = SymbolTable::get()->find( (*ri).first );
		if ( (*ri).second.empty() )
		{
			out << "            callDtor( " << sp->getIndex()
				<< ", rhsData[" << ( ri - rhs.begin() )
				<< "] );" << endl();
		}
	}
}


////////////////////////////////////////


void
ZDriver::substCode( std::string			&codeStr,
					const std::string	&var,
					const std::string	&replName,
					bool				 lhs,
					const std::string	&lhsType )
{
	if ( var.empty() )
		return;

	std::string::size_type curPos = codeStr.find( var );
	while ( curPos != std::string::npos )
	{
		if ( curPos == 0 ||
			 ( ! ( isalnum( codeStr[curPos-1] ) ||
				   '_' == codeStr[curPos-1] ) ) )
		{
			std::string::size_type ep;

			ep = curPos + var.size();
			if ( ! ( isalnum( codeStr[ep] ) || '_' == codeStr[ep] ) )
			{
				if ( lhs )
				{
					std::string::size_type cp = ep;
					while ( isspace( codeStr[cp] ) )
						++cp;
					if ( codeStr[cp] == '=' && codeStr[cp + 1] != '=' )
					{
						std::string::size_type replSize =
							var.size() + ( cp - ep ) + 1;
						std::string realRepl = "data = (";
						realRepl.append( lhsType );
						realRepl.push_back( ')' );
						codeStr.replace( curPos, replSize, realRepl );
					}
					else
						codeStr.replace( curPos, var.size(), replName );
				}
				else
					codeStr.replace( curPos, var.size(), replName );
			}

			curPos = codeStr.find( var, ep );
		}
		else
			curPos = codeStr.find( var, curPos + 1 );
	}
}


////////////////////////////////////////


void
ZDriver::emitFuncBreak( std::ostream &out )
{
	out << endl();
	out << endl();
	out << "////////////////////////////////////////" << endl();
	out << endl();
	out << endl();
}


////////////////////////////////////////


ZDriverOutHelp
ZDriver::endl( void )
{
	ZDriverOutHelp retval;
	retval.myOut = this;
	return retval;
}

