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
 * \file pragmaToAttribute.H
 *
 * \brief Visits nodes, and parses pragma statements to add attributes to AST
 *
 *//*+*************************************************************************/

#ifndef _pragmaToAttribute_H_
#define _pragmaToAttribute_H_

#include <list>

//--Forward declarations

class SgProject;

namespace LoopChainTransformation{
/// Visits nodes, and parses pragma statements to add attributes to AST
  //std::list<SgPragmaDeclaration*> pragmaToAttribute(SgNode* a_SgNd);
  std::list<SgPragmaDeclaration*> pragmaToAttribute(SgProject* project);
}
#endif  // ! defined _pragmaToAttribute_H_
