#ifdef CH_LANG_CC
/*
 *      _____     __             __
 *     / ___/__  / /  ___  _____/ /_
 *    / /__/ _ \/ _ \/ _ \/ _/_  __/
 *    \___/\___/_//_/\___/_/  /_/
 *    Please refer to Copyright.txt, in Chohort's root directory.
 */
#endif

#ifndef _ParsePragmaLoopChain_driver_H_
#define _ParsePragmaLoopChain_driver_H_

//----- Standard Library -----//

#include <string>

//----- Internal -----//

#include "ParsePragmaBase_driver.H"

//----- External -----//

#include <LoopChainIR/LoopChain.hpp>
#include <LoopChainIR/LoopNest.hpp>
#include "SchedulePlan.hpp"

class ParsePragmaLoopChain_driver;

/// Class storing attributes for LoopChain pragma
class LoopChainPragmaAttribute : public ParsePragmaBaseAttribute
{
public:
  enum LoopChainDirective
  {
    LoopChainDirectiveBegin  = (1<<0),
    LoopChainDirectiveEnd    = (1<<1),
    LoopChainDirectiveFile   = (1<<2),
    LoopChainDirectiveInline = (1<<3),
    LoopChainDirectiveLoop   = (1<<4),
    LoopChainDirectiveStrip  = (1<<5),
    LoopChainDirectiveParallel  = (1<<6),
    LoopChainDirectiveFor  = (1<<7)
  };

  LoopChainPragmaAttribute()
    :
    m_directives(0),
    m_linkIndex(-1)
    { }

  /// Copy constructor
  LoopChainPragmaAttribute(const LoopChainPragmaAttribute& a_attr)
    :
    m_directives(a_attr.m_directives),
    m_linkIndex(a_attr.m_linkIndex),
    m_file(a_attr.m_file),
    lc(a_attr.lc)
    { }

  virtual
  ~LoopChainPragmaAttribute()
    { }

  // Override base to allow deep copy of attribute when deep copy of SgNode is
  // made (otherwise silently fails)
  virtual
  AstAttribute* copy()
    {
      return new LoopChainPragmaAttribute(*this);
    }

  virtual
  void merge(const ParsePragmaBaseAttribute* a_attr)
    {
      const LoopChainPragmaAttribute *lcAttr =
        dynamic_cast<const LoopChainPragmaAttribute*>(a_attr);
      m_directives |= lcAttr->m_directives;
    }

  void addDirective(LoopChainDirective a_directive)
    {
      m_directives |= a_directive;
    }

  bool getDirective(const LoopChainDirective& a_directive) const
    {
      return m_directives & a_directive;
    }

  bool haveDirectives() const
    {
      return m_directives != 0;
    }

  /// Save name of new file(s) required for chaining
  void setFile(const std::string& a_file);

  const std::string& getFile() const
    {
      return m_file;
    }

  /// Save link index for chain
  void setLinkIndex(const int a_linkIndex)
    {
      m_linkIndex = a_linkIndex;
    }

  /// Get link index for chain
  int getLinkIndex() const
    {
      return m_linkIndex;
    }

  LoopChainIR::LoopChain* getLoopChain(){
    return this->lc;
  }

  void setLoopChain(LoopChainIR::LoopChain *other){
    this->lc = other;
  }

  LoopChainIR::LoopNest* getLoopNest(){
    return (this->ln);
  }

  void setLoopNest(LoopChainIR::LoopNest *other){
    this->ln = other;
  }

  std::vector<SchedulePlan*>* getPlan(){
    return (this->plan);
  }

  void setPlan( std::vector<SchedulePlan*>* plan ){
    this->plan = plan;
  }


protected:

  int m_directives;                   ///< All directives
  int m_linkIndex;                    ///< Link in chain (starting at 0) for
                                      ///< loop directives
  LoopChainIR::LoopChain *lc;
  LoopChainIR::LoopNest *ln;
  std::vector<SchedulePlan*>* plan;
  std::string m_file;                 ///< File string of additional file
};

/// LoopChain pragma driver
class ParsePragmaLoopChain_driver : public ParsePragmaBase_driver
{
public:
  ///
  ParsePragmaLoopChain_driver()
    :
    ParsePragmaBase_driver(),
    m_attr(0),
    m_attachLocation(AttachToNextStatement)
    { }

  ///

  virtual
  ~ParsePragmaLoopChain_driver()
    { }

  ///
  virtual
  void scan_begin();

  ///
  virtual
  void scan_end();

  ///
  virtual
  int parse();

  /// Set attach location for attributes (default is AttachToNextStatement)
  void setAttachLocation(const AttachLocation a_attachLocation)
    {
      m_attachLocation = a_attachLocation;
    }

  /// Clear all files
  static void clearNewFiles();

  /// Get files
  static const std::vector<std::string>& getNewFiles();

  friend class LoopChainPragmaAttribute;

  LoopChainPragmaAttribute* m_attr;   ///< Attributes determined from pragma to
                                      ///< attach to SgNd.  Allocated for each
                                      ///< call to parse()

protected:

  AttachLocation m_attachLocation;    ///< Primary attach location
  static std::vector<std::string> s_newFiles;
                                      ///< List of new files that are required
                                      ///< in the project for chaining
};

#endif  // ! _ParsePragmaLoopChain_driver_H_
