%{ /* -*- C++ -*- */
#include "ParsePragmaLoopChain_proto.H"

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

extern "C" int yylex();
%}

%option prefix="loopchain"
%option outfile="lex.yy.c"
%option noyywrap nounput batch debug

/* Definitions
   id                   Any alphanumeric identifier.  Must start with letter.
   intnum               Any numeric identifier (no '.')
   npstr                Any string not containing parenthesis
   blanks               Blanks
 */
digit  [0-9]
id     [a-zA-Z_][a-zA-Z0-9_]*
npstr  [^\(\)]*
blank  [ \t]

/* Start conditions
   pstropen             Get first '(' while ignoring blanks
   pstr                 Gather string until closing ')'
*/
%x pstropen
%x pstr

%%

%{
  int pcount;                         /* Count of parenthesis */
  int errcount = 0;
  std::string strbuf;                 /* buffer */
%}

<INITIAL,pstropen>{blank}+ { }

%{
  typedef yy::ParsePragmaLoopChain_parser token_maker;
  typedef yy::ParsePragmaLoopChain_parser::token token;
  typedef yy::ParsePragmaLoopChain_parser::token_type token_type;
%}

"("                     { return token_maker::make_LPAREN(loc); }
")"                     { return token_maker::make_RPAREN(loc); }
"{"                     { return token_maker::make_LCURLY(loc); }
"}"                     { return token_maker::make_RCURLY(loc); }
":"                     { return token_maker::make_COLON(loc);  }
";"                     { return token_maker::make_COLON(loc);  }
","                     { return token_maker::make_COMMA(loc);  }
"for"                   { return token_maker::make_FOR(loc); }
"domain"                { return token_maker::make_DOMAINLC(loc); }
"parallel"              { return token_maker::make_PARALLEL(loc); }
"loopchain"             { return token_maker::make_LOOPCHAIN(loc); }
"omplc"                 { return token_maker::make_OMPLC(loc); }
"schedule"              { return token_maker::make_SCHEDULE(loc); }
"shift"                 { return token_maker::make_SHIFT(loc); }
"fuse"                  { return token_maker::make_FUSE(loc); }
"tile"                  { return token_maker::make_TILE(loc); }
"wavefront"             { return token_maker::make_WAVEFRONT(loc); }
"serial"                { return token_maker::make_SERIAL(loc); }
"read"                  { return token_maker::make_READ(loc); }
"write"                 { return token_maker::make_WRITE(loc); }
"with"                  { return token_maker::make_WITH(loc); }
{id}                    { return token_maker::make_ID(yytext,loc); }
"+"                     { return token_maker::make_PLUSOP(loc); }
"-"                     { return token_maker::make_MINUSOP(loc); }
"*"                     { return token_maker::make_TIMESOP(loc); }

  /* Grab an integer */
{digit}+              {
                          return token_maker::make_INT(std::atoi(yytext), loc);
                        }


%%

void
ParsePragmaLoopChain_driver::scan_begin()
{
  yy_flex_debug = m_traceScanning;
  m_bufferStateHandle = (void*)yy_scan_string(m_pragmaStr.c_str());
}

void
ParsePragmaLoopChain_driver::scan_end()
{
  yy_delete_buffer((YY_BUFFER_STATE)m_bufferStateHandle);
}
