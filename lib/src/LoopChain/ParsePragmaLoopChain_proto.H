#ifdef CH_LANG_CC
/*
 *      _____     __             __
 *     / ___/__  / /  ___  _____/ /_
 *    / /__/ _ \/ _ \/ _ \/ _/_  __/
 *    \___/\___/_//_/\___/_/  /_/
 *    Please refer to Copyright.txt, in Chohort's root directory.
 */
#endif

#ifndef _ParsePragmaLoopChain_proto_H_
#define _ParsePragmaLoopChain_proto_H_

//----- Internal -----//

#include "CHTconfig.H"
#include "ParsePragmaLoopChain_parser.H"

// Tell Flex the lexer's prototype (note the specific name) ...
#if (CHTDEF_BISON_MAJOR>=3)
#define YY_DECL                                                         \
  yy::ParsePragmaLoopChain_parser::symbol_type                          \
  loopchainlex(ParsePragmaLoopChain_driver& driver)
#else
#define YY_DECL                                                         \
  yy::ParsePragmaLoopChain_parser::token_type                           \
  loopchainlex(yy::ParsePragmaLoopChain_parser::semantic_type* yylval,  \
          yy::ParsePragmaLoopChain_parser::location_type* yylloc,       \
          ParsePragmaLoopChain_driver& driver)
#endif
// ... and declare it for the parser's sake.
YY_DECL;

#include "ParsePragmaLoopChain_driver.H"

#endif  // ! __ParsePragmaLoopChain_proto_H
