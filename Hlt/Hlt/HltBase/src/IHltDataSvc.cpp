// $Id: IHltDataSvc.cpp,v 1.3 2009-10-25 21:07:10 graven Exp $ 
// ============================================================================
// Incldue files 
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/IHltDataSvc.h"
// ============================================================================
// Return the interface ID
// ============================================================================
const InterfaceID& IHltDataSvc::interfaceID() 
{ 
  static const InterfaceID id( "IHltDataSvc", 2, 1 );
  return id; 
}
// =============================================================================
// virtual & protected destructor 
// ============================================================================
IHltDataSvc::~IHltDataSvc() {}
// ============================================================================
//  The END 
// ============================================================================
