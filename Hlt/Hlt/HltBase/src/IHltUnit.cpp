// $Id: IHltUnit.cpp,v 1.1 2010-01-25 09:28:01 graven Exp $
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "HltBase/IHltUnit.h"
// ============================================================================
/** @file
 *  Implementation file for class LoKi::IHltUnit
 *  @date 2008-11-10
 *  @author Vanya BELYAEV Ivan.BELYAEV@nikhef.nl
 */
// ============================================================================
// the unique interface identifier
// ============================================================================
const InterfaceID& Hlt::IUnit::interfaceID()
{
    // the unique interafcx eidentifier
    static const InterfaceID s_IID{"Hlt::IUnit", 1, 0};
    //
    return s_IID;
}
// ============================================================================
// virtual & protected destructor
// ============================================================================
Hlt::IUnit::~IUnit()
{
}
// ============================================================================
// The END
// ============================================================================
