%skeleton "lalr1.cc" /* -*- C++ -*- */
// Above must be first line: indicate C++ and the skeleton to use
// Indicate to output a header file and it should be named exactly this
%defines "ParsePragmaLoopChain_parser.H"
// The name of the parser class
%define parser_class_name {ParsePragmaLoopChain_parser}
%define api.token.constructor
%define api.value.type variant
%code requires {
#define yylex loopchainlex
#include <string>
#include <tuple>
#include <vector>
#include <list>
#include <algorithm>
#include <set>
#include <map>
#include <LoopChainIR/RectangularDomain.hpp>
#include <LoopChainIR/LoopNest.hpp>
#include <LoopChainIR/Accesses.hpp>
#include "SchedulePlan.hpp"

class ParsePragmaLoopChain_driver;

enum class AccessDirectionEnum { READ, WRITE };

class PartialDataspace {
public:
  std::string name;
  AccessDirectionEnum direction;
  std::set<LoopChainIR::Tuple>* accesses;

  PartialDataspace( std::string name, AccessDirectionEnum direction, std::set<LoopChainIR::Tuple>* accesses)
  : name( name ), direction( direction ), accesses( accesses )
  { }
};

}
// The parsing context.
%param { ParsePragmaLoopChain_driver& driver }
%locations
%debug
%error-verbose
%code {
#include <sstream>
#include <set>
#include "ParsePragmaLoopChain_proto.H"


// Keeps track of symbols during exp note expansion.
// TODO: Better way of doing this.
bool finding_for_domain = false;
std::set<std::string> domain_symbols;

}

%token END      0 "end of file"
%token LPAREN   "("
%token RPAREN   ")"
%token LCURLY   "{"
%token RCURLY   "}"
%token COLON    ":"
%token COMMA    ","
%token SEMI     ";"
%token WITH
%token READ
%token WRITE
%token SCHEDULE
%token SERIAL
%token PARALLEL
%token SHIFT
%token FUSE
%token WAVEFRONT
%token TILE
%token DOMAINLC
%token LOOPCHAIN
%token OMPLC
%token FOR
%token <std::string> OP
%token PLUSOP
%token MINUSOP
%token TIMESOP
%token <std::string> ID
%token <int> INT "integer"

%type <std::list<std::string>*> int_list const_extents_defn /*exp_list extents_defn*/
%type <std::list<std::list<std::string>*>*> const_extents_defn_list
%type <std::vector<SchedulePlan*>*> schedule_directive
%type <std::list<SchedulePlan*>*> schedule_atom schedule_list
%type <SchedulePlan*> valid_inner_tile_schedule
%type <TilePlan*> tile_schedule_atom
%type <std::string> str_expr e_token
%type <int> evaled_expr
%type <LoopChainIR::RectangularDomain*> oneD_domain_list oneD_domain
%type <LoopChainIR::LoopNest*> for_directive
%type <std::list<int>*> index_expression_list
%type <std::set<LoopChainIR::Tuple>*> index_access_list
%type <AccessDirectionEnum> direction
%type <PartialDataspace*> access_atom
%type <std::list<PartialDataspace*>*> access_list
%type <std::list<LoopChainIR::Dataspace>*> access_pattern_grammar;

%start loopchain_directive

%left PLUSOP MINUSOP
%left TIMESOP
%right UNARY_MINUS

%%

loopchain_directive:
    for_directive { CHT::msg << CHT::fv4 << "LOOP NEST"  << CHT::end; }
  | chain_directive { CHT::msg << CHT::fv4 << "LOOP CHAIN"  << CHT::end; }
  ;

chain_directive:
  LOOPCHAIN schedule_directive
    {
      driver.m_attr->addDirective( LoopChainPragmaAttribute::LoopChainDirectiveParallel );
      driver.m_attr->setPlan( $2 );
      CHT::msg << CHT::fv1 << "keyword loopchain captured" << CHT::end;
    }

for_directive:
  FOR DOMAINLC
    {
      finding_for_domain = true;
    }
  LPAREN oneD_domain_list RPAREN
    {
      finding_for_domain = false;
    }
  access_pattern_grammar
    {
      driver.m_attr->addDirective( LoopChainPragmaAttribute::LoopChainDirectiveFor );
      CHT::msg << CHT::fv1 << "Creating a Loop Nest" << CHT::end;
      LoopChainIR::LoopNest *nest = new LoopChainIR::LoopNest(*$5, *$8);
      CHT::msg << CHT::fv1 << "Setting the Loop Nest" << CHT::end;
      driver.m_attr->setLoopNest(nest);
      CHT::msg << CHT::fv1 << "LoopNest is complete" << CHT::end;
    }

schedule_directive :
    SCHEDULE LPAREN schedule_list RPAREN
    {

      $$ = new std::vector<SchedulePlan*>( $3->begin(), $3->end() );
      delete $3;
    }
  | SCHEDULE LPAREN RPAREN
    {
      $$ = new std::vector<SchedulePlan*>();
    }

schedule_list :
    schedule_atom
    {
      $$ = $1;
    }
  | schedule_atom COMMA schedule_list
    {
      for( auto a : *$3 ){
        $1->push_back( a );
      }
      delete $3;
      $$ = $1;
    }

schedule_atom :
    FUSE LPAREN const_extents_defn_list RPAREN
    {
      std::list<SchedulePlan*>* plans = new std::list<SchedulePlan*>();

      LoopChainIR::LoopChain::size_type id = 0;
      for( std::list<std::string>* extents : *$3 ){
        plans->push_back( new ShiftPlan( id, new std::vector<std::string>( extents->begin(), extents->end() ) ) );
        delete extents;
        id += 1;
      }
      delete $3;

      plans->push_back( new FusePlan( false ) );

      $$ = plans;
    }
  | FUSE LPAREN RPAREN
  {
    std::list<SchedulePlan*>* plans = new std::list<SchedulePlan*>();

    plans->push_back( new FusePlan( true ) );

    $$ = plans;
  }
  | PARALLEL
    {
      std::list<SchedulePlan*>* plans = new std::list<SchedulePlan*>();

      plans->push_back(  new ParallelPlan() );

      $$ = plans;
    }
  | tile_schedule_atom
    {
      std::list<SchedulePlan*>* plans = new std::list<SchedulePlan*>();
      plans->push_back( $1 );
      $$ = plans;
    }
  | WAVEFRONT
    {
      std::list<SchedulePlan*>* plans = new std::list<SchedulePlan*>();

      plans->push_back(  new WavefrontPlan() );

      $$ = plans;
    }

tile_schedule_atom :
  TILE LPAREN const_extents_defn COMMA valid_inner_tile_schedule COMMA valid_inner_tile_schedule RPAREN
  {
    $$ = new TilePlan( new std::vector<std::string>( $3->begin(), $3->end() ), $5, $7 );
    delete $3;
  }

/*
extents_defn:
  LPAREN exp_list RPAREN
  {
    $$ = $2;
  }

exp_list :
  expr
  {
    std::list<std::string>* list = new std::list<std::string>();
    std::string s = static_cast<std::ostringstream*>( &(std::ostringstream() << $1) )->str();
    list->push_back( s );
    $$ = list;
  }
  | expr COMMA exp_list
  {
    std::string s = static_cast<std::ostringstream*>( &(std::ostringstream() << $1) )->str();
    $3->push_front( s );
    $$ = $3;
  }
*/

const_extents_defn_list :
    const_extents_defn
  {
    std::list<std::list<std::string>*>* list_of_consts = new std::list<std::list<std::string>*>();
    list_of_consts->push_back( $1 );
    $$ = list_of_consts;
  }
  | const_extents_defn COMMA const_extents_defn_list
  {
    $3->push_front( $1 );
    $$ = $3;
  }

const_extents_defn :
  LPAREN int_list RPAREN
  {
    $$ = $2;
  }

int_list :
    evaled_expr
  {
    std::list<std::string>* int_list = new std::list<std::string>();
    std::string s = static_cast<std::ostringstream*>( &(std::ostringstream() << $1) )->str();
    int_list->push_back( s );
    $$ = int_list;
  }
  | evaled_expr COMMA int_list
  {
    std::string s = static_cast<std::ostringstream*>( &(std::ostringstream() << $1) )->str();
    $3->push_front( s );
    $$ = $3;
  }

valid_inner_tile_schedule :
  SERIAL
  {
    $$ = new SerialPlan();
  }
| PARALLEL
  {
    $$ = new ParallelPlan();
  }
| WAVEFRONT
  {
    $$ = new WavefrontPlan();
  }
| tile_schedule_atom
  {
    $$ = $1;
  }

oneD_domain_list:
    oneD_domain
    {
      $$ = $1;
    }
  | oneD_domain COMMA oneD_domain_list
    {
      CHT::msg << CHT::fv1 << "Appending a domain " << $1->symbolics() << $3->symbolics() << CHT::end;
      $1->append(*$3);
      CHT::msg << CHT::fv1 << "Appended a domain " << $1->symbolics() << CHT::end;
      $$ = $1;
    }

oneD_domain:
  str_expr COLON str_expr
    {
      CHT::msg << CHT::fv1 << "Creating a domain: " << $1 << ", " << $3 << CHT::end;
      LoopChainIR::RectangularDomain* domain = new LoopChainIR::RectangularDomain($1,$3, domain_symbols);
      CHT::msg << CHT::fv1 << "Created a domain" << CHT::end;
      domain_symbols.clear();
      $$ = domain;
    }

access_pattern_grammar :
    WITH LPAREN id_list RPAREN access_list
    {
      std::map< std::string, LoopChainIR::Dataspace > dataspaces;
      for( PartialDataspace* partial : *$5 ){
        // if a dataspace does not already exist
        LoopChainIR::TupleCollection partialCollection( *(partial->accesses) );
        LoopChainIR::TupleCollection empty( partialCollection.dimensions() );
        CHT::msg << CHT::fv4 << "partial collection " << partialCollection
                             << "empty " << empty << CHT::end;
        if( dataspaces.count( partial->name ) == 0 ){
          CHT::msg << CHT::fv4 << "Partially creating Dataspace " << partial->name << CHT::end;
          dataspaces.emplace( partial->name,
                              LoopChainIR::Dataspace( partial->name,
                                                      ((partial->direction == AccessDirectionEnum::READ)?
                                                          partialCollection
                                                        : empty),
                                                      ((partial->direction == AccessDirectionEnum::WRITE)?
                                                          partialCollection
                                                        : empty)
                                                    )
                            );
        }
        // if a dataspace does already exist;
        else {
          CHT::msg << CHT::fv4 << "Updating existing Dataspace " << partial->name << CHT::end;
          std::map< std::string, LoopChainIR::Dataspace >::iterator pos = dataspaces.find( partial->name );
          LoopChainIR::Dataspace existing = pos->second;
          dataspaces.erase( pos );
          LoopChainIR::TupleCollection reads(  existing.reads(),
                                              ((partial->direction == AccessDirectionEnum::READ)?
                                                    partialCollection
                                                  : empty)
                                            );
          LoopChainIR::TupleCollection writes(  existing.writes(),
                                                ((partial->direction == AccessDirectionEnum::WRITE)?
                                                    partialCollection
                                                  : empty)
                                              );
          LoopChainIR::Dataspace updated( partial->name, reads, writes );
          dataspaces.emplace( partial->name, updated );
        }

        delete partial;
      }

      std::list<LoopChainIR::Dataspace>* unmapped_dataspaces = new std::list<LoopChainIR::Dataspace>();
      for( std::pair<std::string, LoopChainIR::Dataspace> name_dataspace : dataspaces ){
        unmapped_dataspaces->push_back( name_dataspace.second );
      }

      CHT::msg << CHT::fv4 << "Created Dataspaces for LoopNest:" << CHT::end;
      for( LoopChainIR::Dataspace ds : *unmapped_dataspaces ){
        CHT::msg << CHT::fv4 << ds << CHT::end;
      }

      $$ = unmapped_dataspaces;
    }

id_list :
    ID
    { }
  | ID COMMA id_list
    { }

access_list :
    access_atom
    {
      $$ = new std::list<PartialDataspace*>(1, $1);
    }
  | access_atom COMMA access_list
    {
      $3->push_front( $1 );
      $$ = $3;
    }

access_atom :
    direction ID LCURLY index_access_list RCURLY
    {
      $$ = new PartialDataspace( $2, $1, $4 );
    }

index_access_list :
    LPAREN index_expression_list RPAREN
    {
      $$ = new std::set<LoopChainIR::Tuple>();
      $$->insert( LoopChainIR::Tuple( std::vector<int>( $2->begin(), $2->end() ) ) );
    }
  | LPAREN index_expression_list RPAREN COMMA index_access_list
    {
      $5->insert( LoopChainIR::Tuple( std::vector<int>( $2->begin(), $2->end() ) ) );
      $$ = $5;
    }

index_expression_list :
    evaled_expr
    {
      $$ = new std::list<int>( 1, $1 );
    }
  | evaled_expr COMMA index_expression_list
    {
      $3->push_front( $1 );
      $$ = $3;
    }

evaled_expr:
    evaled_expr PLUSOP evaled_expr
    {
      $$ = $1 + $3;
    }
  | evaled_expr MINUSOP evaled_expr
    {
      $$ = $1 - $3;
    }
  | evaled_expr TIMESOP evaled_expr
    {
      $$ = $1 * $3;
    }
  | LPAREN evaled_expr RPAREN
    {
      $$ = $2;
    }
  | MINUSOP evaled_expr %prec UNARY_MINUS
    {
      $$ = - $2;
    }
  | INT
    {
      $$ = $1;
    }
  | ID
    {
      // Zero support for any symbolics.
      $$ = 0;
    }


str_expr:
    str_expr PLUSOP str_expr
    {
      std::string s = $1 + " + " + $3;
      $$ = s;
    }
  | str_expr MINUSOP str_expr
    {
      std::string s = $1 + " - " + $3;
      $$ = s;
    }
  | str_expr TIMESOP str_expr
  {
    std::string s = $1 + " * " + $3;
    $$ = s;
  }
  | LPAREN str_expr RPAREN
    {
      std::string s = "(" + $2 + ")";
      $$ = s;
    }
  | MINUSOP str_expr %prec UNARY_MINUS
    {
      std::string s = "(- " + $2 + ")";
      $$ = s;
    }
  | e_token
    {
      $$ = $1;
    }

direction :
    READ
    {
      $$ = AccessDirectionEnum::READ;
    }
  | WRITE
    {
      $$ = AccessDirectionEnum::WRITE;
    }

e_token:
    INT
    {
      std::string s = static_cast<std::ostringstream*>( &(std::ostringstream() << $1) )->str();
      $$ = s;
    }
  | ID
    {
      if( finding_for_domain ){
        domain_symbols.insert($1);
      }
      CHT::msg << CHT::fv3 << "Found symbol: " << $1 << CHT::end;
      $$ = $1;
    }

%%

// Send errors to the driver.  Error handling is defined in class
// ParsePragmaBase_driver
void yy::ParsePragmaLoopChain_parser::error(
  const yy::ParsePragmaLoopChain_parser::location_type& l,
  const std::string&                                    m)
{
  driver.error( m);
}
