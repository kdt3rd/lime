/// @file Grammar.cpp
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

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <iosfwd>

#include "Grammar.h"
#include "Error.h"
#include "Rule.h"
#include "RuleTable.h"
#include "Symbol.h"
#include "SymbolTable.h"
#include "Config.h"
#include "State.h"
#include "StateTable.h"
#include "Util.h"
#include "Version.h"


////////////////////////////////////////


Grammar::Grammar( void )
		: myBasisOnly( false ), myCompressActions( true ), myNoActions( false ),
		  myDebugOutput( false ), myQuiet( true ), myStats( false ),
		  myLanguage( LanguageDriver::CPP ), myNumConflicts( 0 )
{
	SymbolTable::get()->findOrCreate("$");
	SymbolTable::get()->addDefault("{default}");
	myErrSym = SymbolTable::get()->findOrCreate("error");
	myErrSym->setDataType( "int" );
}


////////////////////////////////////////


Grammar::~Grammar( void )
{
}


////////////////////////////////////////


void
Grammar::setSourceFile( const char *sourceFile )  
{
	mySourceFile = sourceFile;
}


////////////////////////////////////////


void
Grammar::setOutputDirectory( const char *outputDir )
{
	myOutputDir = outputDir;
}


////////////////////////////////////////


void
Grammar::setLanguage( LanguageDriver::Language lang )
{
	myLanguage = lang;
}


////////////////////////////////////////


bool
Grammar::setValue( const std::string &name,
				   const std::string &value,
				   int				  line )
{
	bool retval = true;

	if ( mySettings.find( name ) == mySettings.end() )
		mySettings[ name ] = ValueSetting( value, line );
	else
		retval = false;

	return retval;
}


////////////////////////////////////////


void
Grammar::process( void )
{
	RuleTable::get()->findPrecedences();

	/// Compute the lambda-nonterminals and the first-sets for every
    /// nonterminal
	findFirstSets();

	// Compute all LR(0) states.  Also record follow-set propagation
	// links so that the follow-set can be computed later
	findStates();

	// Tie up loose ends on the propagation links
	findLinks();

	// Compute the follow set of every reducible configuration
	findFollowSets();

	// Compute the action tables
	findActions();

	// Compress the action tables
	if ( isCompressActions() )
		compressTables();

	// Generate a report of the parser generated.  (the "y.output" file) */
	if ( ! isQuiet() )
		reportOutput();

	// Generate the source code for the parser
	outputFiles();
}


////////////////////////////////////////


int
Grammar::getNumConflicts( void )
{
	return myNumConflicts;
}


////////////////////////////////////////


void
Grammar::reprintGrammar( void ) const
{
	SymbolTable *symTable = SymbolTable::get();
	size_t		 i, n;

	std::cout << "// Reprint of input file \"" << mySourceFile
			  << "\".\n// Symbols:\n";

	n = symTable->getNumSymbols();
	for ( i = 0; i < n; ++i )
	{
		std::cout << "// " << i << " " << symTable->getNthSymbol(i)->getName()
				  << std::endl;
	}

	std::cout << std::endl;

	n = RuleTable::get()->getNumRules();
	for ( i = 0; i < n; ++i )
	{
		Rule *rp = RuleTable::get()->getNthRule( i );

		std::cout << rp->getLHS() << " ::=";

		Rule::RHSListConstIter ri, re;

		ri = rp->getRHS().begin();
		re = rp->getRHS().end();

		for ( ; ri != re; ++ri )
		{
			std::cout << " " << (*ri).first;
		}
		std::cout << ".";
		if ( ! rp->getPrecedence().empty() )
			std::cout << " [" << rp->getPrecedence() << "]";
		std::cout << std::endl;
	}
}


////////////////////////////////////////


void
Grammar::printStats( void ) const
{
	size_t nt = SymbolTable::get()->getNumTerminals();
	size_t ns = SymbolTable::get()->getNumSymbols();

	std::cout << "Grammar statistics: " << nt << " terminals, "
			  << ns - nt << " nonterminals, "
			  << RuleTable::get()->getNumRules() << " rules" << std::endl;

	std::cout << "                    " << StateTable::get()->getNumStates()
			  << " states, " << 0 << " parser table entries, "
			  << myNumConflicts << " conflicts" << std::endl;
}


////////////////////////////////////////


/// Find all nonterminals which will generate the empty string.
/// Then go back and compute the first sets of every nonterminal.
/// The first set is the set of all terminal symbols which can begin
/// a string generated by that nonterminal.
void
Grammar::findFirstSets( void )
{
	// First compute all lambdas
	RuleTable::get()->computeLambdas();

	// Now compute the first sets
	RuleTable::get()->computeFirstSets();
}


////////////////////////////////////////


void
Grammar::findStates( void )
{
	Symbol *startSym = getStartSymbol( true );

	if ( ! startSym )
	{
		Error::get()->add( "No rules to choose as start rule." );
		return;
	}

	if ( RuleTable::get()->isOnRightSide( startSym->getName() ) )
	{
		Error::get()->add( "The start symbol \"%s\" occurs on the "
						   "right-hand side of a rule. This will result "
						   "in a parser which does not work properly.",
						   startSym->getName().c_str() );
	}

	myCurConfigList.reset();

	Rule	*startRule = RuleTable::get()->findFirstRule( startSym->getName() );
	while ( startRule )
	{
		Config *tmpCfg = myCurConfigList.addWithBasis( startRule, 0 );

		// All start rules have the start symbol as their left hand side
//		tmpCfg->addFollowSet( startSym->getName() );
		tmpCfg->addFollowSet( SymbolTable::get()->getNthSymbol( 0 )->getName() );

		startRule = RuleTable::get()->getNextRule( startRule );
	}

	getNextState();
}


////////////////////////////////////////


void
Grammar::findLinks( void )
{
	// Convert all backlinks into forward links.  Only the forward
	// links are used in the follow-set computation.
	size_t i, N;

	N = StateTable::get()->getNumStates();
	for ( i = 0; i < N; ++i )
	{
		State *stp = StateTable::get()->getNthState( i );
		for ( Config *cfp = stp->getConfig(); cfp; cfp = cfp->getNext() )
		{
			// little bit of housekeeping (for debugging)
			cfp->setState( stp );

			const Config::PropList &bp = cfp->getBackwardPropLinks();
			Config::PropListConstIter pi, pe;

			pe = bp.end();
			for ( pi = bp.begin(); pi != pe; ++pi )
				(*pi)->addForwardPropLink( cfp );
		}
	}
}


////////////////////////////////////////


void
Grammar::findFollowSets( void )
{
	size_t i, N;

	N = StateTable::get()->getNumStates();
	for ( i = 0; i < N; ++i )
	{
		State *stp = StateTable::get()->getNthState( i );
		for ( Config *cfp = stp->getConfig(); cfp; cfp = cfp->getNext() )
			cfp->setStatus( Config::INCOMPLETE );
	}

	bool progress = false;

	do
	{
		progress = false;

		for ( i = 0; i < N; ++i )
		{
			State *stp = StateTable::get()->getNthState( i );
			for ( Config *cfp = stp->getConfig(); cfp; cfp = cfp->getNext() )
			{
				if ( cfp->getStatus() == Config::COMPLETE )
					continue;

				const Config::PropList &plp = cfp->getForwardPropLinks();
				Config::PropListConstIter pi, pe;

				pe = plp.end();
				for ( pi = plp.begin(); pi != pe; ++pi )
				{
					if ( (*pi)->combineFollowSet( cfp->getFollowSet() ) )
					{
						(*pi)->setStatus( Config::INCOMPLETE );
						progress = true;
					}
				}

				cfp->setStatus( Config::COMPLETE );
			}
		}
	} while ( progress );
}


////////////////////////////////////////


void
Grammar::findActions( void )
{
	size_t i, j, nState, nSym, nRule;
	State	*stp;

	nState = StateTable::get()->getNumStates();
	nSym = SymbolTable::get()->getNumSymbols();

	// Find all reduce actions...
	for ( i = 0; i < nState; ++i )
	{
		stp = StateTable::get()->getNthState( i );
		for ( Config *cfp = stp->getConfig(); cfp; cfp = cfp->getNext() )
		{
			// Check if dot at extreme right
			if ( size_t( cfp->getDot() ) == cfp->getRule()->getRHS().size() )
			{
				for ( j = 0; j < nSym; ++j )
				{
					Symbol *sp = SymbolTable::get()->getNthSymbol( j );

					if ( ( Symbol::TERMINAL == sp->getType() ||
						   sp->getName() == "$" ) &&
						 cfp->getFollowSet().isSet( sp->getName() ) )
					{
						stp->addAction( Action::REDUCE, sp->getName(),
										0, cfp->getRule() );
					}
				}
			}
		}
	}

	Symbol *startSym = getStartSymbol();

	stp = StateTable::get()->getNthState( 0 );
	// Add the accepting token to the first state which is always the
	// starting state of the finite state machine as an action to accept
	// if the lookahead is the start nonterminal.
	if ( startSym && stp )
		stp->addAction( Action::ACCEPT, startSym->getName(), 0, 0 );

	// Resolve conflicts
	for ( i = 0; i < nState; ++i )
	{
		stp = StateTable::get()->getNthState( i );

		// Check for SHIFT-SHIFT conflicts first (nothing we can do to resolve)
		for ( Config *cfp = stp->getConfig(); cfp; cfp = cfp->getNext() )
		{
			if ( ! cfp->getForwardPropLinks().empty() )
				continue;

			Rule *curRule = cfp->getRule();

			if ( size_t( cfp->getDot() ) == curRule->getRHS().size() )
				continue;

			for ( Config *next = stp->getConfig(); next; next = next->getNext() )
			{
				if ( next == cfp )
					continue;

				Rule *nextRule = next->getRule();

				if ( size_t( next->getDot() ) == nextRule->getRHS().size() )
					continue;

				if ( curRule->getRHS()[cfp->getDot()].first ==
					 nextRule->getRHS()[next->getDot()].first )
				{
					std::cout << "Unresolved SHIFT-SHIFT conflict between:\n"
							  << std::endl;
					curRule->print( std::cout );
					std::cout << std::endl << "\nand\n" << std::endl;
					nextRule->print( std::cout );
					std::cout << std::endl << std::endl;
					++myNumConflicts;
				}
			}
		}

		stp->sortActions();
		ActionList &ap = stp->getActions();
		size_t nAct = ap.getNumActions();
		if ( nAct > 0 )
		{
			for ( j = 0; j < (nAct - 1); ++j )
			{
				Action &act = ap.getNthAction( j );
				for ( size_t k = j + 1; k < nAct; ++k )
				{
					Action &nact = ap.getNthAction( k );
					int numCs = resolveConflict( act, nact );
					if ( 0 != numCs )
					{
						std::cout << "  Conflict in state " << i << std::endl;
						myNumConflicts += numCs;
					}
				}
			}
		}
	}

	// Report errors for each rule that can never be reduced.
	nRule = RuleTable::get()->getNumRules();
	for ( i = 0; i < nRule; ++i )
		RuleTable::get()->getNthRule( i )->setCanReduce( false );

	for ( i = 0; i < nState; ++i )
	{
		stp = StateTable::get()->getNthState( i );

		ActionList &ap = stp->getActions();
		size_t nAct = ap.getNumActions();
		for ( j = 0; j < nAct; ++j )
		{
			Action &act = ap.getNthAction( j );
			if ( act.getType() == Action::REDUCE )
				act.getRule()->setCanReduce( true );
		}
	}

	for ( i = 0; i < nRule; ++i )
	{
		Rule *rp = RuleTable::get()->getNthRule( i );

		if ( rp->canReduce() )
			continue;

		std::ostringstream tmpOut;
		rp->print( tmpOut );

		Error::get()->add( rp->getRuleLine(),
						   "Rule for '%s':\n  %s\ncan not be reduced.\n",
						   rp->getLHS().c_str(), tmpOut.str().c_str() );
	}
}


////////////////////////////////////////


void
Grammar::compressTables( void )
{
	size_t i, nState;

	nState = StateTable::get()->getNumStates();

	// Find all reduce actions...
	for ( i = 0; i < nState; ++i )
		StateTable::get()->getNthState(i)->compress();
}


////////////////////////////////////////


void
Grammar::reportOutput( void )
{
	std::string outputName;
	Util::getFileName( outputName, myOutputDir, mySourceFile, ".out" );

	std::ofstream out;
	out.open( outputName.c_str() );

	StateTable::get()->print( out, myBasisOnly );

	out.close();
}


////////////////////////////////////////


void
Grammar::outputFiles( void )
{
	Producer *producer = LanguageDriver::getProducer( myLanguage,
													  mySettings,
													  myOutputDir,
													  getName(),
													  mySourceFile );

	if ( producer )
	{
		std::string arg, prefix;

		producer->setDebugOutput( isDebugOutput() );
		if ( producer->writeSource() )
		{
			if ( ! producer->writeHeader() )
				Error::get()->add( "Unable to write the output header file." );
		}
		else
			Error::get()->add( "Unable to write the output source file." );

		delete producer;
	}
}


////////////////////////////////////////


State *
Grammar::getNextState( void )
{
	Config *bp;
	State *state;

	myCurConfigList.sortBasis();
	bp = myCurConfigList.getBasis();

	state = StateTable::get()->find( bp );
	if ( state )
	{
		Config *sbp, *tbp;

		for ( sbp = state->getBasis(), tbp = bp; sbp && tbp;
			  sbp = sbp->getNextBasis(), tbp = tbp->getNextBasis() )
		{
			sbp->mergePropLinks( tbp );
		}
		myCurConfigList.deleteConfigs();
	}
	else
	{
		myCurConfigList.computeClosure();
		myCurConfigList.sort();

		state = new State( bp, myCurConfigList.getConfig() );

		myCurConfigList.resetPointers();

		StateTable::get()->add( state );

		// Cause things to recurse around (if necessary)
		buildShifts( state );
	}

	return state;
}


////////////////////////////////////////


void
Grammar::buildShifts( State *state )
{
	Config *cfp;

	// Each configuration becomes complete after it contibutes to a successor
	// state.  Initially, all configurations are incomplete
	for ( cfp = state->getConfig(); cfp; cfp = cfp->getNext() )
		cfp->setStatus( Config::INCOMPLETE );

	// Loop through all configs of state
	for ( cfp = state->getConfig(); cfp; cfp = cfp->getNext() )
	{
		if ( cfp->getStatus() == Config::COMPLETE )
			continue;

		const Rule::RHSList &rhs = cfp->getRule()->getRHS();

		if ( cfp->getDot() >= int( rhs.size() ) )
			continue;

		myCurConfigList.reset();

		// Symbol after the dot
		// for all configs in the state which have the symbol rhs[dot]
		// following it's dot, add the same config to the basis set under
		// construction but with the dot shifted one symbol to the right.
		for ( Config *bcfp = cfp; bcfp; bcfp = bcfp->getNext() )
		{
			if ( bcfp->getStatus() == Config::COMPLETE )
				continue;

			const Rule::RHSList &brhs = bcfp->getRule()->getRHS();
			if ( bcfp->getDot() >= int( brhs.size() ) )
				continue;

			if ( rhs[cfp->getDot()].first != brhs[bcfp->getDot()].first )
				continue;

			bcfp->setStatus( Config::COMPLETE );

			Config *cfg = myCurConfigList.addWithBasis( bcfp->getRule(),
														bcfp->getDot() + 1 );

			cfg->addBackwardPropLink( bcfp );
		}

		State *newstp = getNextState();

		// Add shift action to reach state newstp from state on symbol...
		state->addAction( Action::SHIFT, rhs[cfp->getDot()].first, newstp, 0 );
	}
}


////////////////////////////////////////


int
Grammar::resolveConflict( Action &act, Action &nextAct )
{
	Symbol *symA, *symB;
	int retval = 0;

	if ( act.getLookAhead() != nextAct.getLookAhead() )
		return 0;

	else if ( act.getType() == Action::SHIFT &&
			  nextAct.getType() == Action::REDUCE )
	{
		symA = act.getLookAheadSymbol();
		symB = nextAct.getRule()->getPrecedenceSymbol();

		if ( ! symB || symA->getPrecedence() < 0 || symB->getPrecedence() < 0 )
		{
			nextAct.setType( Action::CONFLICT );
			++retval;
		}
		else if ( symA->getPrecedence() > symB->getPrecedence() )
			nextAct.setType( Action::RD_RESOLVED );
		else if ( symA->getPrecedence() < symB->getPrecedence() )
			act.setType( Action::SH_RESOLVED );
		else if ( symA->getPrecedence() == symB->getPrecedence() &&
				  symA->getAssoc() == Symbol::RIGHT )
		{
			// Operator associativity breaks tie
			nextAct.setType( Action::RD_RESOLVED );
		}
		else if ( symA->getPrecedence() == symB->getPrecedence() &&
				  symA->getAssoc() == Symbol::LEFT )
		{
			// Operator associativity breaks tie
			act.setType( Action::SH_RESOLVED );
		}
		else
		{
			if ( !( symA->getPrecedence() == symB->getPrecedence() &&
					symA->getAssoc() == Symbol::NONE ) )
			{
				throw std::logic_error( "unexpected situation resolving conflicts" );
			}
			nextAct.setType( Action::CONFLICT );
			++retval;
		}

		if ( retval != 0 )
		{
			std::cout << "Unresolved SHIFT-REDUCE conflict between '"
					  << act.getLookAhead() << "' and '"
					  << nextAct.getLookAhead() << "'"
					  << std::endl;
		}
	}
	else if ( act.getType() == Action::REDUCE &&
			  nextAct.getType() == Action::REDUCE )
	{
		symA = act.getRule()->getPrecedenceSymbol();
		symB = nextAct.getRule()->getPrecedenceSymbol();

		if ( ! symA || ! symB || symA->getPrecedence() < 0 ||
			 symB->getPrecedence() < 0 ||
			 symA->getPrecedence() == symB->getPrecedence() )
		{
			nextAct.setType( Action::CONFLICT );
			++retval;
		}
		else if ( symA->getPrecedence() > symB->getPrecedence() )
			nextAct.setType( Action::RD_RESOLVED );
		else if ( symA->getPrecedence() < symB->getPrecedence() )
			act.setType( Action::SH_RESOLVED );

		if ( retval != 0 )
		{
			std::cout << "Unresolved REDUCE-REDUCE conflict between '"
					  << act.getLookAhead() << "' and '"
					  << nextAct.getLookAhead() << "'"
					  << std::endl;
		}
	}

	return retval;
}


////////////////////////////////////////


Symbol *
Grammar::getStartSymbol( bool adderr )
{
	Symbol *startSym = 0;
	ValueMapIter si = mySettings.find( std::string( "start_symbol" ) );

	if ( si != mySettings.end() )
	{
		startSym = SymbolTable::get()->find( (*si).second.first );
		if ( ! startSym )
		{
			if ( RuleTable::get()->getNumRules() > 0 )
				startSym = RuleTable::get()->getNthRule( 0 )->getLHSSymbol();

			if ( startSym && adderr )
				Error::get()->add( "The specified start symbol \"%s\" is not "
								   "in a nonterminal of the grammar.  \"%s\" "
								   "will be used as the start symbol instead.",
								   (*si).second.first.c_str(),
								   startSym->getName().c_str() );
		}
	}
	else
	{
		if ( RuleTable::get()->getNumRules() > 0 )
			startSym = RuleTable::get()->getNthRule( 0 )->getLHSSymbol();
	}

	return startSym;
}


////////////////////////////////////////


const std::string &
Grammar::getName( void ) const
{
	static const std::string defStr = VersionInfo::appName();
	ValueMapConstIter si = mySettings.find( std::string( "name" ) );

	return ( si != mySettings.end() ) ? (*si).second.first : defStr;
}
