// $Id: XmlDeSubCalorimeterCnv.cpp,v 1.3 2006-12-14 12:51:16 ranjard Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2003/06/16 13:36:27  sponce
// use new package DetDescCnv
//
// Revision 1.1.1.1  2002/03/28 13:32:34  ibelyaev
// Det/CaloDetXmlCnv: new package
//
// ============================================================================
// DetDesc 
#include  "DetDescCnv/XmlUserDetElemCnv.h"
// CaloDet 
#include  "CaloDet/DeSubCalorimeter.h"

// ============================================================================
/** @file XmlDeSubCalorimeterCnv.cpp
 * 
 * Instantiation of a static factory class used by clients to create
 * instances of Xml converter for class DeSubCalorimeter 
 * @see DeSubCalorimeter 
 *
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================

/// converter type 
typedef  XmlUserDetElemCnv<DeSubCalorimeter>  XmlDeSubCalorimeterCnv;
DECLARE_CONVERTER_FACTORY(XmlDeSubCalorimeterCnv)
// ============================================================================
// The End 
// ============================================================================












