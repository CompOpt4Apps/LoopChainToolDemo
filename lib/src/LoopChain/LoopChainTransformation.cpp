#include "LoopChainTransformation.hpp"

// --- Standard Library -----//

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <set>

using namespace std;
using namespace LoopChainIR;
using namespace SageInterface;
using namespace SageBuilder;

namespace LoopChainTransformation {

  LCEncounter::LCEncounter( SgNode* node, std::vector<SchedulePlan*>* plan ):
  lc_node(node), chain( new LoopChainIR::LoopChain() ), plan(plan)
  {}

  void CollectLCAttributes::append_chain( SgNode* node ){

    LoopChainPragmaAttribute* attribute = static_cast<LoopChainPragmaAttribute*>(node->getAttribute("LoopChain"));

    // Assert that we are at a `loopchain for` pragma
    if( !( attribute != NULL && attribute->getDirective( LoopChainPragmaAttribute::LoopChainDirectiveParallel) ) ){
        CHT::msg << CHT::fv3 << "Incorrect node type" << CHT::end;
        abort();
    }

    LCEncounter* encounter = new LCEncounter( node, attribute->getPlan() );
    this->current_encounter = encounter;
    this->current_chain = encounter->chain;
    this->LCNodeList.push_back( encounter );
  }

  void CollectLCAttributes::append_nest( SgNode* node ){
    LoopChainPragmaAttribute* attribute = static_cast<LoopChainPragmaAttribute*>(node->getAttribute("LoopChain"));

    // Assert that we are at a `loopchain for` pragma
    if( !( attribute != NULL && attribute->getDirective( LoopChainPragmaAttribute::LoopChainDirectiveFor) ) ){
        CHT::msg << CHT::fv3 << "Incorrect node type" << CHT::end;
        abort();
    }

    // Assert that we have encountered a `loopchain parallel loopchain` pragma
    // and created an intermediate encounter
    if( this->current_encounter == NULL){
      CHT::msg << CHT::fv3 << "Attempting to append nest, but have no current encouter!" << CHT::end;
      abort();
    }

    SgForStatement* loop = isSgForStatement( node );
    assert( loop != NULL );

    LoopChainIR::LoopNest* nest = attribute->getLoopNest();
    // ordered list of the iterators we encounter.
    std::vector<SgName> iterators;
    // push back the 1st iterator
    iterators.push_back( SageInterface::getLoopIndexVariable(loop)->get_name() );

    // Retrieve inner most loop body
    for( LoopChainIR::RectangularDomain::size_type i = nest->getDomain().dimensions(); i > 1 ; i -= 1 ){
      // get inner body
      loop = isSgForStatement( isSgBasicBlock(loop->get_loop_body())->get_statements()[0] );
      if( loop == NULL ){
        CHT::msg << CHT::fv3 << "Not a perfectly nested loop" << CHT::end;
        abort();
      }
      // push back the i'th iterator
      iterators.push_back( SageInterface::getLoopIndexVariable(loop)->get_name() );
    }

    // Get the innermost statement (may or may not be a basic block)
    SgStatement* stmt = isSgStatement(loop->get_loop_body());
    assert( stmt != NULL );

    // Attempt to get body as a basic block
    SgBasicBlock* body = isSgBasicBlock( stmt );
    // if stmt isnt a basic block, wrap it in one and set as loop body
    if( body == NULL ){
      body = SageBuilder::buildBasicBlock( stmt );
      body->set_parent( loop );
      loop->set_loop_body( body );
    }
    // Build onto our current loop-chain encounter.
    this->current_encounter->nest_bodies.push_back( body );
    this->current_encounter->iterators.push_back( iterators );
    this->current_chain->append( *nest );
  }

  /*--------------------------------------------------------------------*/
  // tasks performed at each node
  /*--------------------------------------------------------------------*/
  void CollectLCAttributes::visit(SgNode *a_SgNd) {

    // check for a Quark attribute and translate it
    LoopChainPragmaAttribute* l_qAtt =
      static_cast<LoopChainPragmaAttribute*>((a_SgNd)->getAttribute("LoopChain"));

    if (l_qAtt != NULL){
      // Encounter #pragma loopchain parallel loopchain
      if(l_qAtt->getDirective(
                 LoopChainPragmaAttribute::LoopChainDirectiveParallel)){

        CHT::msg << CHT::fv3 << "Found LC attribute" << CHT::end;
        this->append_chain( a_SgNd );
      } else
      // Encounter #pragma loopchain for ...
      if(l_qAtt->getDirective(
                 LoopChainPragmaAttribute::LoopChainDirectiveFor)){

        CHT::msg << CHT::fv3 << "Found Nest" << CHT::end;
        this->append_nest( a_SgNd );
      }
    }
  }

  std::set<SgPragmaDeclaration*> parseLoopChainAnnotations( SgProject* project ){
    std::list<SgPragmaDeclaration*> visited_list = pragmaToAttribute(project);
    std::set<SgPragmaDeclaration*> pragmas( visited_list.begin(), visited_list.end() );
    return pragmas;
  }

  void TransformLoopChains( SgProject* project ){
    // Decorates AST having loopchain annotations with attributes
    std::set<SgPragmaDeclaration*> pragma_nodes = parseLoopChainAnnotations( project );

    CollectLCAttributes collect;
    collect.traverseInputFiles(project, preorder);
    int chain_count = 0;
    for(std::list<LCEncounter*>::iterator encounter_iter = collect.LCNodeList.begin() ; encounter_iter!=collect.LCNodeList.end(); ++encounter_iter){
      // Wrapped encounter of an entire loop chain.
      LCEncounter* encounter = *encounter_iter;
      // Statement block that _is_ the loop chain
      SgStatement* root_lc_node = isSgStatement( encounter->lc_node );
      // Ordered (by loop) list of ordered (by depth) list of iterators
      std::vector< std::vector<SgName> > iterators = encounter->iterators;
      // Orderd (by loop) list of inner-most loop-nest bodies
      std::vector< SgBasicBlock* > nest_bodies = encounter->nest_bodies;
      // LoopChainIR LoopChain object produced by the traversal
      LoopChainIR::LoopChain* chain = encounter->chain;
      // List of SchedulePlans for transformations
      std::vector<SchedulePlan*>* plan_list = encounter->plan;

      if( root_lc_node == NULL ){
        // Cannot think of a situation in which this would happen.
        CHT::msg << CHT::fv4 << "Not a Statement" << CHT::end;
        // Nor do I believe it should be allowed for any type that is not at lest a descenent of SgStatement
        abort();
      }

      CHT::msg << CHT::fv4 << "Found a pragma to outline" << CHT::end;

      CHT::msg << CHT::fv4 << "Scheduing code" << CHT::end;
      // Create LCIR schedule from the chain
      LoopChainIR::Schedule sched( *chain, "", "omplc_gen_iter_" );

      // Create transformations from plan
      Scheduler scheduler( plan_list, chain );

      // Now we apply the transformation
      sched.apply( scheduler.transformations );

      // For debugging purposes, print code from ISL's printer
      CHT::msg << CHT::fv4 << sched << CHT::end;
      CHT::msg << CHT::fv4 << "Code:\n" << sched.codegen() << CHT::end;

      // Get ISL ast for transformation
      ISLASTRoot* isl_ast = sched.codegenToIslAst();
      CHT::msg << CHT::fv4 << "Insert injection point" << CHT::end;
      SgBasicBlock* injection_point = SageBuilder::buildBasicBlock();
      SageInterface::insertStatement( root_lc_node, injection_point, false );

      CHT::msg << CHT::fv4 << "Transforming ISL to Sage" << CHT::end;
      // Walk the ISL ast and produce Sage ast and call information
      SageTransformationWalker walker( isl_ast->root, injection_point, CHT::msg.verbosity() >= 4 );

      std::vector< function_call_info* >& stmt_macros = *walker.getStatementMacroNodes();

      // foreach statement macro, copy+transform original associated loop-nest body's block, inject into transformed loop-chain
      for( auto stmt_iter = stmt_macros.begin(); stmt_iter != stmt_macros.end(); ++stmt_iter ){
        function_call_info& info = *(*stmt_iter);
        // Get block number
        // LCIR scheduler produces numbered statement macro names, use getRootStatementSymbol to dynamically produce regex
        std::regex finder( sched.getRootStatementSymbol() + "([[:digit:]]+)" );
        std::string name = info.name.getString();
        // Match statement macro name
        auto match = std::sregex_iterator( name.begin(), name.end(), finder );

        // Only expecting one match for the entire
        if( std::distance(match, std::sregex_iterator() ) != 1 ){
          std::cerr << "Not exactly 1 match" << std::endl;
          abort();
        }

        // Number is the 1st capture group (0th is the entire matched string)
        int block_number = std::stoi( (*match)[1] );

        CHT::msg << CHT::fv4 << "Func-call info: " << info.name.getString() << " @ " << static_cast<void*>(info.expr_node) << " is block " << block_number << CHT::end;

        // Copy block, and inject old to new iterator mappings as variable decls
        SgBasicBlock* cloned_block = SageInterface::deepCopy<SgBasicBlock>( nest_bodies[block_number] );
        // Original iterators and new expressions for this loop-nest block
        std::vector<SgName> original_iters = iterators[block_number];
        std::vector<SgExpression*> new_iter_expressions = info.parameter_expressions;

        // Should be the same size
        if( new_iter_expressions.size() != original_iters.size() ){
          std::cerr << "Originally " << original_iters.size() << " iterators, but " << new_iter_expressions.size() << " expressions" << std::endl;
          abort();
        }

        CHT::msg << CHT::fv4 << "Mapping new iterator expressions to old iterator symbols" << CHT::end;

        // foreach iterator, create var decl. with initialization from expression
        for( int iter_i = 0; iter_i < original_iters.size(); ++iter_i ){
          CHT::msg << CHT::fv4 << "Mapping " << original_iters[iter_i].getString() << " @ " << static_cast<void*>( &original_iters[iter_i] ) << CHT::end;

          // clone expression
          SgExpression* cloned_exp = SageInterface::deepCopy<SgExpression>( new_iter_expressions[iter_i] );

          // create new variable decl and initialization
          SgAssignInitializer* initalizer = SageBuilder::buildAssignInitializer( cloned_exp, SageBuilder::buildIntType() );
          SgVariableDeclaration* var_decl = SageBuilder::buildVariableDeclaration( original_iters[iter_i], SageBuilder::buildIntType(), initalizer, cloned_block );

          CHT::msg << CHT::fv4 << "Initializer @ " << static_cast<void*>( initalizer ) << CHT::end;
          CHT::msg << CHT::fv4 << "Var Decl @ " << static_cast<void*>( var_decl ) << CHT::end;

          // prepend var decl to cloned block
          SageInterface::prependStatement( var_decl, cloned_block );
        }

        // Insert modified cloned block after statement macro
        SageInterface::insertStatement( info.expr_node, cloned_block, false );

        // remove the statement macro
        SageInterface::removeStatement( info.expr_node );
      }

      // TODO remove loopchain pragma
      //root_lc_node->removeAttribute( "omplc" );

      // Remove original loop chain
      SageInterface::removeStatement( root_lc_node );
      chain += 1;
    }

    // Remove Pragma nodes
    for( SgPragmaDeclaration* pragma : pragma_nodes ){
      SageInterface::removeStatement( pragma );
    }

  }

} // End namespace _
