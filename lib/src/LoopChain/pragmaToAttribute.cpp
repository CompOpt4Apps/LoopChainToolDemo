#ifdef CH_LANG_CC
/*
 *      _____     __             __
 *     / ___/__  / /  ___  _____/ /_
 *    / /__/ _ \/ _ \/ _ \/ _/_  __/
 *    \___/\___/_//_/\___/_/  /_/
 *    Please refer to Copyright.txt, in Chohort's root directory.
 */
#endif


/******************************************************************************/
/**
 *  \file pragmaToAttribute.cpp
 *
 *  \brief Visits nodes, and parses pragma statements to add attributes to AST
 *
 *  <ul>
 *   <li> Although unnecessary, we use a formal AST traversal so the user sees
 *        statements parsed in a recognizable order
 *  </ul>
 *
 *//*+*************************************************************************/

//----- ROSE Library -----//

#include "rose.h"

//----- Internal -----//

#include "CHTmsg.H"
#include "ParsePragmaMain_driver.H"
#include "ParsePragmaLoopChain_driver.H"

#include <list>

/*******************************************************************************
 */
///  Visitor that parses pragma nodes
/**
 *//*+*************************************************************************/

namespace LoopChainTransformation {

  class PragmaVisitor : public AstSimpleProcessing
  {
  public:
    /// Constructor
    PragmaVisitor();
    std::list<SgPragmaDeclaration*> getVisitedPragmas();
  protected:
    /// Task at each Sage node
    virtual void visit(SgNode* node);
    std::list<SgPragmaDeclaration*> visited_pragmas;
    ParsePragmaMain_driver m_pmdriver;  ///< The main driver for parsing
    ParsePragmaLoopChain_driver m_plcdriver;
                                        ///< The parsing context for LoopChain
                                        ///< pragmas
  };

  /*******************************************************************************
   *
   *  Class PragmaVisitor: member definitions
   *
   ******************************************************************************/

  /*--------------------------------------------------------------------*/
  //  Constructor
  /**
   *//*-----------------------------------------------------------------*/

  PragmaVisitor::PragmaVisitor()
  : visited_pragmas()
  {
    // given string is the name of the pragma (eg #pragma the_string_you_use ...)
    // cannot have: -
    m_pmdriver.registerParser("omplc", &m_plcdriver);
    // ENABLE DEBUGGING OF PARSER AND SCANNER HERE
    m_plcdriver.setTraceScanning(false);
    m_plcdriver.setTraceParsing(false);
  }

  /*--------------------------------------------------------------------*/
  //  Task at each Sage node
  /** \param[in]  a_SgNd The Sage node we are visiting
   *//*-----------------------------------------------------------------*/

  void PragmaVisitor::visit(SgNode *a_SgNd)
  {
    // For now, we are only interested in pragma statements.  Later, perhaps some
    // detection of patterns.
    if (a_SgNd->variantT() == V_SgPragmaDeclaration)
      {
        SgPragmaDeclaration* pragmaDeclSgNd = isSgPragmaDeclaration(a_SgNd);
        m_pmdriver.setPragmaFromSgNd(pragmaDeclSgNd);
        m_pmdriver.parse();
        this->visited_pragmas.push_back( pragmaDeclSgNd );
      }
  }

  std::list<SgPragmaDeclaration*> PragmaVisitor::getVisitedPragmas(){
    return this->visited_pragmas;
  }


  /*==============================================================================
   */
  //  Visits nodes, and parses pragma statements to add attributes to AST
  /**
   *  \param[in]  a_SgNd  Node to begin parsing at.  Normally, this should be
   *                      the SgFile node.
   *
   *//*=========================================================================*/

  //std::list<SgPragmaDeclaration*> pragmaToAttribute(SgNode* a_SgNd)
  std::list<SgPragmaDeclaration*> pragmaToAttribute(SgProject* project)
  {
    PragmaVisitor visitor;
    CHT::msg << CHT::fv2 << "Visiting Sage nodes to parse pragmas" << CHT::end;
    //visitor.traverse(a_SgNd, preorder);
    visitor.traverseInputFiles(project, preorder);
    return visitor.getVisitedPragmas();
  }

}
