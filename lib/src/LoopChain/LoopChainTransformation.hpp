#ifndef LOOPCHAINTRANSFORMATION_HPP
#define LOOPCHAINTRANSFORMATION_HPP

/* ---- Rose ---- */
#include "rose.h"

/* ---- LoopChainIR ---- */
#include <LoopChainIR/LoopChain.hpp>
#include <LoopChainIR/Schedule.hpp>
#include <LoopChainIR/SageTransformationWalker.hpp>

/* ---- Cohort Internals ---- */
#include "CHTmsg.H"
#include "pragmaToAttribute.H"
#include "ParsePragmaLoopChain_driver.H"

#include "SchedulePlan.hpp"

namespace LoopChainTransformation {

  /*!
  \class LCEncounter
  \brief Encapsulates a LoopChain (maintaining state) through the CollectLCAttributes traversal
  */

  class LCEncounter {
  public:
    //! \brief Sage node that we encountered the loop-chain pragma on
    SgNode* lc_node;

    //! Names of the iterators, in order of depth
    std::vector< std::vector<SgName> > iterators;

    //! \brief Bodies of the annotated loop nests
    std::vector<SgBasicBlock*> nest_bodies;

    //! Resulting (or intermediate, if in construction) loop-chain object
    LoopChainIR::LoopChain* chain;

    //! Plan of the loopChain;
    std::vector<SchedulePlan*>* plan;

    /*!
    \brief Construct an loop-chain encounter that starts at the current node.
    \param chain the node (typically a scope node) where the chain is/begins.
    */
    LCEncounter(SgNode* chain, std::vector<SchedulePlan*>* plan );
  };


  /*!
  \class CollectLCAttributes
  \brief Pass that traverses LoopChain annotations, collecting loop-nests into a LoopChainIR::LoopChain object
  */
  class CollectLCAttributes : public AstSimpleProcessing {
  public:
    /*!
    \brief List of LCEncounter objects.
    */
    std::list<LCEncounter*> LCNodeList; // a list of SgNodes parallel attributes

    /*!
    \brief The current (i.e. most recent) LoopChainIR::LoopChain
    */
    LoopChainIR::LoopChain* current_chain;
    LCEncounter* current_encounter;
    void append_chain( SgNode* );
    void append_nest( SgNode* );

  protected:
    /// Task at each Sage node
    virtual void visit( SgNode* node );
  };


  /*!
  \brief For all files in the project, calls pragmaToAttribute to parse the LoopChain pragma nodes.
  */
  std::set<SgPragmaDeclaration*> parseLoopChainAnnotations( SgProject* project );

  /*!
  \brief Front-to-Back LoopChain transformation. Call on the project to be transformed.
  */
  void TransformLoopChains( SgProject* project );

}

#endif
