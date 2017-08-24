#ifdef CH_LANG_CC
/*
 *      _____     __             __
 *     / ___/__  / /  ___  _____/ /_
 *    / /__/ _ \/ _ \/ _ \/ _/_  __/
 *    \___/\___/_//_/\___/_/  /_/
 *    Please refer to Copyright.txt, in Chohort's root directory.
 */
#endif

#ifndef __ParsePragmaMain_proto_H
#define __ParsePragmaMain_proto_H

//----- Internal -----//

#include "CHTconfig.H"
#include "ParsePragmaMain_parser.H"

// Tell Flex the lexer's prototype (note the specific name) ...
#if (CHTDEF_BISON_MAJOR>=3)
#define YY_DECL                                                         \
  yy::ParsePragmaMain_parser::symbol_type                               \
  mainlex(ParsePragmaMain_driver& driver)
#else
#define YY_DECL                                                         \
  yy::ParsePragmaMain_parser::token_type                                \
  mainlex(yy::ParsePragmaMain_parser::semantic_type* yylval,            \
          yy::ParsePragmaMain_parser::location_type* yylloc,            \
          ParsePragmaMain_driver& driver)
#endif
// ... and declare it for the parser's sake.
YY_DECL;

#include "ParsePragmaMain_driver.H"

#endif  // ! __ParsePragmaMain_proto_H
