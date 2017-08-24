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

#include "ParsePragmaBase_driver.H"
#include "CHTmsg.H"
#include "CHTparam.H"

/*--------------------------------------------------------------------*/
//  Attach attributes to the node.
/** If attributes with this name already exist, they are merged.
 *  The AST assumes responsibility for memory passed to it.
 *  \param[in]  a_name  Name of the attribute
 *  \param[in]  a_attr  Attribute to attach or merge
 *  \param[in]  a_primaryAttachLocation
 *                      Attempt to attach attributes here.  Default is
 *                      next statement after pragma
 *  \param[in]  a_secondaryAttachLocation
 *                      If attempt to attach attributes to primary
 *                      location failed, and attempt is made to the
 *                      secondary location.  Default is the enclosing
 *                      scope of the pragma.
 *//*-----------------------------------------------------------------*/

int
ParsePragmaBase_driver::attachAttributes(
  const char* const           a_name,
  ParsePragmaBaseAttribute*&  a_attr,
  const AttachLocation        a_primaryAttachLocation,
  const AttachLocation        a_secondaryAttachLocation)
{

//--Find a location to attach the attributes.  There are two attempts.

  SgStatement* attachStatementSgNd = NULL;
  for (int attachAttempt = 0; attachAttempt != 2; ++attachAttempt)
    {
      if (attachStatementSgNd != NULL) break;
      const AttachLocation attachLocation = (attachAttempt == 0) ?
        a_primaryAttachLocation : a_secondaryAttachLocation;
      switch (attachLocation)
        {
        case AttachToNextStatement:
          // Attach attribute to next IR statement
          attachStatementSgNd = m_pragmaDeclSgNd;
          while (attachStatementSgNd &&
                 attachStatementSgNd->variantT() == V_SgPragmaDeclaration)
            {
              attachStatementSgNd =
                SageInterface::getNextStatement(attachStatementSgNd);
            }
          break;
        case AttachToEnclScope:
          // Attach attribute to enclosing scope
          attachStatementSgNd =
            SageInterface::getEnclosingScope(m_pragmaDeclSgNd);
          break;
        case AttachToPragma:
          // Attach attribute to pragma statement
          attachStatementSgNd = m_pragmaDeclSgNd;
          break;
        }
    }

  if (attachStatementSgNd)
    {
      if (attachStatementSgNd->attributeExists(a_name))
        // Probably two pragma statements in a row
        {
          const AstAttribute* oldAttribute =
            attachStatementSgNd->getAttribute(a_name);
          a_attr->merge(dynamic_cast<const ParsePragmaBaseAttribute*>(
                          oldAttribute));
          attachStatementSgNd->updateAttribute(a_name, a_attr);
          a_attr = 0;
          CHT::msg << CHT::fv3 << "Updated " << a_name << " attribute in node "
                   << CHT::nodeloc(attachStatementSgNd) << CHT::end;
        }
      else
        {
          attachStatementSgNd->addNewAttribute(a_name, a_attr);
          a_attr = 0;
          CHT::msg << CHT::fv3 << "Added " << a_name << " attribute to node "
                   << CHT::nodeloc(attachStatementSgNd) << CHT::end;
        }
    }
  else
    {
      // Print warning
      CHT::msg << "Found no suitable place to attach attributes for pragma "
               << CHT::nodeloc(m_pragmaDeclSgNd) << CHT::warn;
      CHT::msg << "Pragma ignored." << CHT::body;
      return 1;
    }
  return 0;
}
