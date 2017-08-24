#ifdef CH_LANG_CC
/*
 *      _____     __             __
 *     / ___/__  / /  ___  _____/ /_
 *    / /__/ _ \/ _ \/ _ \/ _/_  __/
 *    \___/\___/_//_/\___/_/  /_/
 *    Please refer to Copyright.txt, in Chohort's root directory.
 */
#endif

#ifndef _ParsePragmaBase_driver_H_
#define _ParsePragmaBase_driver_H_

//----- Standard Library -----//

#include <string>

//----- ROSE Library -----//

#include "rose.h"

//----- Internal -----//

#include "CHTmsg.H"

/// Class storing attributes for LoopChain pragma
class ParsePragmaBaseAttribute : public AstAttribute
{
public:

  virtual
  ~ParsePragmaBaseAttribute()
    { }

  virtual void
  merge(const ParsePragmaBaseAttribute* a_attr) = 0;
};

class ParsePragmaBase_driver
{
public:

  /// Actions for attributes when there is no statement after the pragma
  enum AttachLocation
  {
    AttachToNextStatement,            ///< Attach to next statement
    AttachToEnclScope,                ///< Attach to enclosing scope
    AttachToPragma                    ///< Attach to pragma declaration itself
  };

  ///
  ParsePragmaBase_driver()
    :
    m_pragmaStr(""),
    m_pragmaDeclSgNd(NULL),
    m_bufferStateHandle(NULL),
    m_traceScanning(false),
    m_traceParsing(false)
    { }

  ///
  ParsePragmaBase_driver(const char *const a_pragmaStr)
    :
    m_pragmaStr(a_pragmaStr),
    m_pragmaDeclSgNd(NULL),
    m_bufferStateHandle(NULL),
    m_traceScanning(false),
    m_traceParsing(false)
    { }

  ///
  virtual
  ~ParsePragmaBase_driver()
    { }

  ///
  virtual
  void scan_begin() = 0;

  ///
  virtual
  void scan_end() = 0;

  ///
  virtual
  int parse() = 0;

  /// Error handling.
  virtual
  void error (const std::string& m)
    {
      CHT::msg << m << CHT::error;
    }

  ///
  void setTraceScanning(const bool a_trace)
    {
      m_traceScanning = a_trace;
    }

  ///
  void setTraceParsing(const bool a_trace)
    {
      m_traceParsing = a_trace;
    }

  ///
  void setPragma(SgPragmaDeclaration* a_pragmaDeclSgNd,
                 const std::string&   a_pragmaStr)
    {
      m_pragmaDeclSgNd = a_pragmaDeclSgNd;
      m_pragmaStr      = a_pragmaStr;
    }

  ///
  int attachAttributes(const char *const           a_name,
                       ParsePragmaBaseAttribute*&  a_attr,
                       const AttachLocation        a_primaryAttachLocation =
                         AttachToNextStatement,
                       const AttachLocation        a_secondaryAttachLocation =
                         AttachToEnclScope);

  std::string m_pragmaStr;            ///< Pragma string

protected:

  SgPragmaDeclaration* m_pragmaDeclSgNd;
                                      ///< Sage node marking the pragma
                                      ///< declaration

  void* m_bufferStateHandle;          ///< Handle to buffers allocated when
                                      ///< lexing the a string (assumed to be
                                      ///< a pointer)
  bool m_traceScanning;               ///< T - trace scanning
  bool m_traceParsing;                ///< T - trace parsing
};

#endif  // ! _ParsePragmaBase_driver_H_
