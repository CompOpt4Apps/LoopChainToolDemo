%{ /* -*- C++ -*- */
#include "ParsePragmaMain_proto.H"

/* Work around an incompatibility in flex (at least versions
   2.5.31 through 2.5.33): it generates code that does
   not conform to C89.  See Debian bug 333231
   <http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=333231>.  */
#undef yywrap
#define yywrap() 1

/* The location of the current token. */
  static yy::location loc;

/* By default yylex returns int, we use token_type.
   Unfortunately yyterminate by default returns 0, which is
   not of token_type.  */
#define yyterminate() return token_maker::make_END(loc)
%}

%option prefix="main"
%option outfile="lex.yy.c"
%option noyywrap nounput batch debug

id    [a-zA-Z][a-zA-Z_0-9]*
blank [ \t]

%%

{blank}+ {
           driver.m_numChar += yyleng;
         }

%{
  typedef yy::ParsePragmaMain_parser token_maker;
  typedef yy::ParsePragmaMain_parser::token token;
%}

{id}     {
           driver.m_numChar += yyleng;
           return token_maker::make_IDENTIFIER(yytext, loc);
         }
%%

void
ParsePragmaMain_driver::scan_begin()
{
  yy_flex_debug = m_traceScanning;
  m_bufferStateHandle = (void*)yy_scan_string(m_pragmaStr.c_str());
}

void
ParsePragmaMain_driver::scan_end()
{
  yy_delete_buffer((YY_BUFFER_STATE)m_bufferStateHandle);
}
