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
 * \file CHTparam.H
 *
 * \brief Parameters for the translator
 *
 *//*+*************************************************************************/

#ifndef _CHTparam_H_
#define _CHTparam_H_

//----- Standard Library -----//

#include <cassert>

/*------------------------------------------------------------------------------
 * Assertions
 *----------------------------------------------------------------------------*/

#define CHT_assert(cond) assert(cond)

/*------------------------------------------------------------------------------
 * Precision
 *----------------------------------------------------------------------------*/

#ifdef USE_SINGLE_PRECISION
  // Single
  typedef float Real;
#else
  // Double
  typedef double Real;
#endif

// Using CHTparam instead of CHT::param to save colons
namespace CHTparam
{
  const int g_verbosity = 1;
}

#endif /* ! defined _CHTparam_H_ */
