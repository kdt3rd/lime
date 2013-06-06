/// @file main.cpp
/// @brief File that implements the command line option parsing and driving.
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

#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <getopt.h>

#include "Error.h"
#include "Parser.h"
#include "Grammar.h"
#include "RuleTable.h"
#include "Version.h"


////////////////////////////////////////


void
versionAndExit( void )
{
	std::cout <<
		VersionInfo::appName() << " version " << VersionInfo::appVersion() << "\n"
		"Copyright (c) 2003 Kimball Thurston\n"
		"Derived from Lemon, copyright 1991-1997 D. Richard Hipp\n"
		"Freely Distributable under the General Public License (GPL)\n"
			  << std::endl;
	
	std::exit( 0 );
}


////////////////////////////////////////


void
usageAndExit( const char *appName, int exitVal )
{
	std::cout << "Usage:\n" << appName <<
		" [-b|--basis] [-n|--no-compress] [-g|--grammar-no-actions]\n"
		"  [-l|--lang (c|c++|z)] [-d|--debug] [-v|--verbose] [-s|--stats]\n"
		"  [-V|--version] [-h|--help] <grammarfile> <outputdir>\n\n"
		" --basis                   Print only the basis in the output report.\n"
		" --no-compress             Do not compress the action table.\n"
		" --grammar-no-actions      Print grammar without actions.\n"
		" --lang=<val>              Specify output implementation language.\n"
		"                           Current languages:\n"
		"                           c   - standard C language <UNFINISHED>\n"
		"                           c++ - C++, not exception friendly\n"
		"                           z   - C++, uses the Zion Core library\n"
		"                                 and should be exception safe.\n"
		" --debug                   Adds some basic debugging output to the\n"
		"                           parser which will print as it parses.\n"
		" --verbose                 Produce an extra report file (file.out).\n"
		" --stats                   Print parser statistics to standard out.\n"
		" --version                 Print the version number and exit.\n"
		" --help                    Print this message and exit.\n\n"
		" file.lem                  Grammar file to parse\n"
		" outputdir                 Output Directory to place output files.\n"
			  << std::endl;
	
	std::cout << std::endl
			  << "The input grammar file can have a number of arguments.\n"
		"All arguments are optional as far as the parser is concerned, but\n"
		"missing things like destructors and such can cause memory leaks.\n"
		"Arguments are specified by the beginning of the line starting with a\n"
		"% sign, then the argument name, then the value of the argument enclosed\n"
		"in curly braces. For example:\n%name { MyParserName }\n\n"
		"The known arguments:\n"
		"name             -- The 'name' of the parser - used to generate appropriate\n"
		"                    function or class names depending on the output type.\n"
		"namespace        -- For output languages like C++, puts parser into a\n"
		"                    namespace.\n"
		"header_include   -- Will be added before the definition of things in the\n"
		"                    generated header file.\n"
		"include          -- Should be a chunk of code to add to the include section\n"
		"                    of the generated source file.\n"
		"code             -- Extra code to be placed at the end of the generated\n"
		"                    source file.\n"
		"token_destructor -- How to cleanup the token as given to the main parse\n"
		"                    function.\n"
		"token_prefix     -- Generated terminal token names will have this prefix.\n"
		"syntax_error     -- If specified, the provided code chunk will be executed\n"
		"                    when a syntax error is found during parsing.\n"
		"parse_accept     -- Extra code to run when the parser accepts.\n"
		"parse_failure    -- Extra code to run when the parser fails.\n"
		"extra_argument   -- An extra argument that will appear in the generated\n"
		"                    parse function which will then be available during\n"
		"                    rule productions so you can modify state in some other\n"
		"                    object.\n"
		"token_type       -- Allows you to specify the type of the token being passed\n"
		"                    to the parse function.\n"
		"start_symbol     -- Allows you to specify the main start / root symbol.\n"
		"                    If this is not specified, the left hand side of the first\n"
		"                    rule will be used.\n"
		"stack_overflow   -- Under languages that have a hard-coded stack size\n"
		"                    (see stack_size), this is extra code to run when an\n"
		"                    overflow occurs. DEPRECATED\n"
		"stack_size       -- Certain output languages may have an implementation\n"
		"                    with a limited stack size. This should be an integer\n"
		"                    value to adjust the default size. DEPRECATED\n";
	std::cout << std::endl
			  << "Rules are specified by a form similar to:\n"
		"lhs(alias) ::= TERMINAL_TOKEN otherrule(alias) TERMINAL_TOKEN\n"
		"{\n\n    ... Code to Process Rule ...\n}\n"
		"\nThe aliases for the various elements can be used in the code to process\n"
		"the rule. You are responsible for cleaning up memory for any of the variable\n"
		"aliases specified on the right hand side (if appropriate).\n"
		"Rules can have a couple of arguments that specify how to handle the\n"
		"left hand side productions. Arguments begin with the name of the argument\n"
		"followed by the rule name followed by the value:\n"
		"type -- Specify the variable type for the left hand side production\n"
		"destructor -- Specify the destructor used to clean up the left hand side\n"
		"              production after accepting the parse or an error occurs\n"
			  << std::endl;
	
	std::cout << "\nTODO: Write doco on specifying rule precedences\n" << std::endl;
	
	std::exit( exitVal );
}


////////////////////////////////////////


bool
parseOptions( Grammar &g, int argc, char *argv[] )
{
	static struct option long_options[] =
		{
			{ "basis", 0, 0, 'b' },
			{ "no-compress", 0, 0, 'n' },
			{ "grammar-no-actions", 0, 0, 'g' },
			{ "lang", 1, 0, 'l' },
			{ "debug", 0, 0, 'd' },
			{ "verbose", 0, 0, 'v' },
			{ "stats", 0, 0, 's' },
			{ "version", 0, 0, 'V' },
			{ "help", 0, 0, 'h' },
			{ 0, 0, 0, 0 }
		};
	
	bool retval = true;
	
	while ( 1 )
	{
		int c;
		
		c = getopt_long( argc, argv, "bngl:dvsVh", long_options, 0 );
		
		// Next arg isn't an option.
		// TERMINATE LOOP
		if ( c == -1 )
			break;
		
		switch ( c )
		{
			case 'b':
				g.setBasisOnly( true );
				break;
				
			case 'n':
				g.setCompressActions( false );
				break;
				
			case 'g':
				g.setNoActions( true );
				break;
				
			case 'l':
			{
				if ( optarg && optarg[0] != '\0' )
				{
					std::string lang = optarg;

					if ( lang == "c" )
						g.setLanguage( LanguageDriver::C );
					else if ( lang == "c++" )
						g.setLanguage( LanguageDriver::CPP );
					else if ( lang == "z" )
						g.setLanguage( LanguageDriver::Z );
					else
					{
						std::cerr << "Unknown output language '"
								  << lang << "'\n" << std::endl;
						usageAndExit( argv[0], 1 );
					}
				}
				else
				{
					std::cerr << "Language type required\n" << std::endl;
					usageAndExit( argv[0], 1 );
				}
				break;
			}
			
			case 'd':
				g.setDebugOutput( true );
				break;
				
			case 'v':
				g.setQuiet( false );
				break;
				
			case 's':
				g.setStats( true );
				break;
				
			case 'V':
				versionAndExit();
				break;
				
			case '?':
			case 'h':
			case 0:
				usageAndExit( argv[0], 0 );
				break;
				
			default:
				std::cerr << "Unknown option '" << argv[optind] << "'\n"
						  << std::endl;
				usageAndExit( argv[0], 1 );
				break;
		}
	}
	
	if ( optind == ( argc - 2 ) )
	{
		Error::get()->setSourceFile( argv[optind] );
		g.setSourceFile( argv[optind] );
		g.setOutputDirectory( argv[optind+1] );
	}
	else
		retval = false;
	
	return retval;
}


////////////////////////////////////////


int
main( int argc, char *argv[] )
{
	int		retval = 0;
	
	try
	{
		Grammar	theGrammar;
		if ( parseOptions( theGrammar, argc, argv ) )
		{
			Parser fileParse;
		
			fileParse.setSourceFile( theGrammar.getSourceFile() );
		
			fileParse.parse( &theGrammar );
		}
		else
		{
			std::cerr << "Error processing command line arguments\n"
					  << std::endl;
			usageAndExit( argv[0], 2 );
		}
	
		if ( 0 == Error::get()->getCount() )
		{
			if ( RuleTable::get()->getNumRules() > 0 )
			{
				if ( theGrammar.isNoActions() )
					theGrammar.reprintGrammar();
				else
					theGrammar.process();
			
				retval = theGrammar.getNumConflicts();
			}
			else
			{
				Error::get()->add( "Empty grammar." );
				retval = 1;
			}
	
			if ( theGrammar.isStats() )
				theGrammar.printStats();
	
			if ( theGrammar.getNumConflicts() > 0 )
			{
				Error::get()->add( "%d parsing conflicts.",
								   theGrammar.getNumConflicts() );
			}
		}
	}
	catch ( std::exception &e )
	{
		std::cerr << "Exception Occurred: " << e.what() << std::endl;
		std::cerr << "Terminating" << std::endl;
		retval = 1;
	}
	catch ( const char *e )
	{
		std::cerr << "Exception Occurred: " << e << std::endl;
		std::cerr << "Terminating" << std::endl;
		retval = 1;
	}
	catch ( ... )
	{
		std::cerr << "Unknown Exception Occurred, Terminating..." << std::endl;
		retval = 1;
	}
	
	return retval;
}
