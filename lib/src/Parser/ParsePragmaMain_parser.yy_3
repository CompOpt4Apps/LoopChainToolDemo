%skeleton "lalr1.cc" /* -*- C++ -*- */
// Above must be first line: indicate C++ and the skeleton to use
// Indicate to output a header file and it should be named exactly this
%defines "ParsePragmaMain_parser.H"
// The name of the parser class
%define parser_class_name {ParsePragmaMain_parser}
%define api.token.constructor
%define api.value.type variant
%code requires {
#define yylex mainlex
#include <string>
class ParsePragmaMain_driver;
}
// The parsing context.
%param { ParsePragmaMain_driver& driver }
%locations
%debug
%error-verbose
%code {
#include "ParsePragmaMain_proto.H"
}
%token END      0 "end of file"
%token <std::string> IDENTIFIER "identifier"

%%

ident:
  /* empty */
| IDENTIFIER { driver.m_pragmaIdent = $1;
               YYACCEPT; }
;

%%

// Send errors to the driver.  Error handling is defined in class
// ParsePragmaBase_driver
void
yy::ParsePragmaMain_parser::error(
  const yy::ParsePragmaMain_parser::location_type& l,
  const std::string&                               m)
{
  driver.error(m);
}
