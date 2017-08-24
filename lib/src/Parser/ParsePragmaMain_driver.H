#ifdef CH_LANG_CC
/*
 *      _____     __             __
 *     / ___/__  / /  ___  _____/ /_
 *    / /__/ _ \/ _ \/ _ \/ _/_  __/
 *    \___/\___/_//_/\___/_/  /_/
 *    Please refer to Copyright.txt, in Chohort's root directory.
 */
#endif

#ifndef _ParsePragmaMain_driver_H_
#define _ParsePragmaMain_driver_H_

//----- Standard Library -----//

#include <string>
#include <map>

//----- Internal -----//

#include "ParsePragmaBase_driver.H"

class ParsePragmaMain_driver : public ParsePragmaBase_driver
{
public:

  typedef std::map<std::string, ParsePragmaBase_driver*> ParserMap;

  ///
  ParsePragmaMain_driver()
    :
    ParsePragmaBase_driver(),
    m_pragmaIdent(""),
    m_numChar(0)
    { }

  ///
  virtual
  ~ParsePragmaMain_driver()
    { }

  ///
  virtual
  void scan_begin();

  ///
  virtual
  void scan_end();

  ///
  void registerParser(const std::string&            a_pragmaIdent,
                      ParsePragmaBase_driver *const a_pdriver);

  ///
  void setPragmaFromSgNd(SgPragmaDeclaration* a_pragmaDeclSgNd);

  ///
  virtual
  int parse();

  std::string m_pragmaIdent;          ///< Identification of pragma
  int m_numChar;                      ///< Number of characters to the end of
                                      ///< the pragma identifier token from the
                                      ///< beginning of the pragma string

protected:

  ParserMap m_pragmaParsers;
                                      ///< Parsing for identified pragma
                                      ///< statements
};

#endif  // ! _ParsePragmaMain_driver_H_
