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

#include "ParsePragmaMain_driver.H"
#include "ParsePragmaMain_parser.H"
#include "CHTmsg.H"
#include "CHTparam.H"

//
void
ParsePragmaMain_driver::registerParser(
  const std::string&            a_pragmaIdent,
  ParsePragmaBase_driver *const a_pdriver)
{
  CHT_assert(a_pdriver != NULL);
  if (m_pragmaParsers.find(a_pragmaIdent) != m_pragmaParsers.end())
    {
      CHT::msg << "Pragma identifier " << a_pragmaIdent << " already "
        "registered with parser" << CHT::warn;
    }
  m_pragmaParsers[a_pragmaIdent] = a_pdriver;
}

//
void
ParsePragmaMain_driver::setPragmaFromSgNd(SgPragmaDeclaration* a_pragmaDeclSgNd)
{
  CHT_assert(a_pragmaDeclSgNd != NULL);
  SgPragma* pragmaSgNd = a_pragmaDeclSgNd->get_pragma();
  CHT_assert(pragmaSgNd != NULL);
  setPragma(a_pragmaDeclSgNd, pragmaSgNd->get_pragma());
}

//
int
ParsePragmaMain_driver::parse()
{
  if (m_pragmaStr.length() == 0) return 1;
  CHT::msg.setTerminateOnError(false);
  CHT::msg.resetNumErrors();
  CHT::msg.resetNumWarnings();
  CHT::msg << CHT::fv2 << "Parsing pragma \'" << m_pragmaStr << "\' "
           << CHT::nodeloc(m_pragmaDeclSgNd) << CHT::end;
  m_numChar = 0;
  // Parse the string
  scan_begin();
  yy::ParsePragmaMain_parser parser(*this);
  parser.set_debug_level(m_traceParsing);
  int res = parser.parse();
  scan_end();
  // Results
  const ParserMap::iterator subparserIt = m_pragmaParsers.find(m_pragmaIdent);
  if (subparserIt != m_pragmaParsers.end())
    {
      CHT::msg << CHT::fv4 << "Pragma token '" << m_pragmaIdent
               << "' recognized" << CHT::end;
      ParsePragmaBase_driver* subparser = subparserIt->second;
      subparser->setPragma(m_pragmaDeclSgNd, m_pragmaStr.c_str() + m_numChar);
      subparser->parse();
    }
  else
    {
      CHT::msg << CHT::fwarn << CHT::verb(1) << "Pragma token '"
               << m_pragmaIdent << "' not recognized "
               << CHT::nodeloc(m_pragmaDeclSgNd) << CHT::end;
    }
  // Set to terminate on error
  CHT::msg.setTerminateOnError(true);
  if (CHT::msg.numErrors() > 0)
    {
      CHT::msg << "There " << CHT::wasNumError << " parsing the pragma \'"
               << m_pragmaStr << "\' at " << CHT::nodeloc(m_pragmaDeclSgNd)
               << CHT::error;
    }
  if (CHT::msg.numWarnings() > 0)
    {
      CHT::msg << CHT::fwarn << CHT::verb(1) << "There " << CHT::wasNumWarning
               << " parsing the pragma \'" << m_pragmaStr << "\' at "
               << CHT::nodeloc(m_pragmaDeclSgNd) << CHT::end;
    }
  return res;
}
