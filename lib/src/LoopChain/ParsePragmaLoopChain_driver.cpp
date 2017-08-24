#ifdef CH_LANG_CC
/*
 *      _____     __             __
 *     / ___/__  / /  ___  _____/ /_
 *    / /__/ _ \/ _ \/ _ \/ _/_  __/
 *    \___/\___/_//_/\___/_/  /_/
 *    Please refer to Copyright.txt, in Chohort's root directory.
 */
#endif

//----- Internal -----//

#include "ParsePragmaLoopChain_driver.H"
#include "ParsePragmaLoopChain_parser.H"
#include "CHTmsg.H"
#include "CHTparam.H"


/*==============================================================================
 * Definition of static variables
 *============================================================================*/

std::vector<std::string> ParsePragmaLoopChain_driver::s_newFiles;


/*******************************************************************************
 *
 * Class LoopChainPragmaAttribute: member definitions
 *
 ******************************************************************************/


/*==============================================================================
 * Public member functions
 *============================================================================*/

/*--------------------------------------------------------------------*/
//  Save name of new file(s) required for chaining
/**
 *  \param[in]  a_file  File name surrounded by parenthesis.  E.g.,
 *                      "(filename)"
 *//*-----------------------------------------------------------------*/

void
LoopChainPragmaAttribute::setFile(const std::string& a_file)
{
  //**FIXME a_file may be more than one file.  Should parse in parser
  // Has () which need to be removed
  size_t firstp = a_file.find_first_of('(');
  size_t lastp = a_file.find_last_of(')');
  if (firstp == std::string::npos || lastp == std::string::npos)
    {
      CHT::msg << "Improperly formatted file name \'" << a_file << '\''
               << CHT::error;
    }
  m_file = a_file.substr(firstp+1, lastp - firstp - 1);
  ParsePragmaLoopChain_driver::s_newFiles.push_back(m_file);
}


/*******************************************************************************
 *
 * Class ParsePragmaLoopChain_driver: member definitions
 *
 ******************************************************************************/


/*==============================================================================
 * Public member functions
 *============================================================================*/

/*--------------------------------------------------------------------*/
//  Start parsing of the contents of a LoopChain pragma
/**
 *//*-----------------------------------------------------------------*/

int
ParsePragmaLoopChain_driver::parse()
{
  if (m_pragmaStr.length() == 0) return 1;
  CHT::msg << CHT::fv3 << "Parsing loop-chain pragma \'" << m_pragmaStr << '\''
           << CHT::end;
  m_attr = new LoopChainPragmaAttribute;
  // By default, attributes are attached to the next statement after the pragma.
  // However, begin and end directives are attached directly to the pragma
  // statement.
  m_attachLocation = AttachToNextStatement;
  // Begin parsing
  scan_begin();
  yy::ParsePragmaLoopChain_parser parser(*this);
  parser.set_debug_level(m_traceParsing);
  int res = parser.parse();
  scan_end();
  // End parsing
  if (res) return res;
  if (m_attr->haveDirectives())
    {
      ParsePragmaBaseAttribute* baseAttr = m_attr;
      res = attachAttributes("LoopChain",
                             baseAttr,
                             m_attachLocation,
                             AttachToEnclScope);
      // On success, ROSE takes over memory and baseAttr == 0
      m_attr = static_cast<LoopChainPragmaAttribute*>(baseAttr);
    }
  if (m_attr)
    {
      delete m_attr;
      m_attr = 0;
    }
  CHT_assert(m_attr == 0);
  return res;
}

/*--------------------------------------------------------------------*/
//  Clear all files
/**
 *//*-----------------------------------------------------------------*/

void
ParsePragmaLoopChain_driver::clearNewFiles()
{
  s_newFiles.clear();
}

/*--------------------------------------------------------------------*/
//  Get files
/** \return             List of new files to add to the project
 *//*-----------------------------------------------------------------*/

const std::vector<std::string>&
ParsePragmaLoopChain_driver::getNewFiles()
{
  return s_newFiles;
}
