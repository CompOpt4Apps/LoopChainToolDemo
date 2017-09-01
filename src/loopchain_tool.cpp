#ifdef CH_LANG_CC
/*
 *      _____     __             __
 *     / ___/__  / /  ___  _____/ /_
 *    / /__/ _ \/ _ \/ _ \/ _/_  __/
 *    \___/\___/_//_/\___/_/  /_/
 *    Please refer to Copyright.txt, in Chohort's root directory.
 */
#endif

// --- Standard Library -----//

#include <iostream>
#include <string>
#include <vector>

// --- ROSE Library -----//

#include "rose.h"

// --- Internal -----//

#include "CHTmsg.H"
#include "CHTparam.H"

// --- Local library for testing -----//
#include "LoopChainTransformation.hpp"

using namespace std;
using namespace LoopChainTransformation;

// Definition of class declared in CHTmsg.H
CHT::Msg& CHT::msg = CHT::Msg::getInstance();

int main(int argc, const char* argv[]){
  // Set stream for output
  CHT::msg.setOutputStream(cout);

  // Process command line arguments
  vector<string> argvVec(argv, argv + argc);

  // Look for verbosity
  int verbosity;
  if (CommandlineProcessing::isOptionWithParameter(argvVec, "-", "v", verbosity, true)){
    if( verbosity < 0 ){
      CHT::msg << "Input: 'verbosity' must be >= 0!" << CHT::error;
    }
  } else {
    verbosity = 0;
  }

  if( verbosity > 0 ){
      CHT::msg << CHT::fh2 << CHT::verb(-1) << "Verbosity set to " << verbosity << CHT::end;
  }

  CHT::msg.setVerbosity(verbosity);

  // Get the new argv and argc (after removing our own options)
  int newArgc;
  char** newArgv = NULL;
  CommandlineProcessing::generateArgcArgvFromList(argvVec, newArgc, newArgv);

  // Create the project
  SgProject* project = frontend(newArgc, newArgv);
  project->skipfinalCompileStep(true);

  CHT::msg.setLineSize( 10000 );

  // Call our transformation.
  TransformLoopChains( project );

  // Generate source code from AST
  int status = backend(project);
  if( status == 0 ){
    CHT::msg << CHT::fh2 << "Success" << CHT::end;
  } else {
    CHT::msg << CHT::fh1 << "Failure: Some unknown error occurred." << CHT::end;
  }

  return status;
}
