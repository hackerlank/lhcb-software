// $Id: GiGaSurfaceCnv.h,v 1.4 2001-11-19 18:27:01 ibelyaev Exp $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2001/08/12 17:24:54  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.2  2001/07/15 20:45:12  ibelyaev
// the package restructurisation
//
// ============================================================================
#ifndef         GIGA_GIGASurfaceCnv_H
#define         GIGA_GIGASurfaceCnv_H  1 
// ============================================================================
/// from STL 
#include <string>
#include <set>
/// base class from GiGa
#include "GiGaCnv/GiGaCnvBase.h" 
///
template <class TYPE> 
class CnvFactory;
///
class Surface; 
class G4LogicalSurface; 


/** @class GiGaSurfaceCnv GiGaSurfaceCnv.h GiGa/GiGaSurfaceCnv.h
 *  
 *  Converter of Surface class to Geant4 
 *  
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 */

class GiGaSurfaceCnv: public GiGaCnvBase
{
  ///
  friend class CnvFactory<GiGaSurfaceCnv>; 
  ///
public:
  ///
  typedef std::set<std::string> Surfaces;
  ///
protected: 
  /// Standard Constructor
  GiGaSurfaceCnv( ISvcLocator* );
  /// Standard (virtual) destructor 
  virtual ~GiGaSurfaceCnv();
  ///
public:

  /// Create representation 
  virtual StatusCode createRep
  ( DataObject*      Object  , 
    IOpaqueAddress*& Address ) ;

  /// Update representation 
  virtual StatusCode updateRep
  ( DataObject*      Object  , 
    IOpaqueAddress*  Address ) ;
 
  /// Class ID for created object == class ID for this specific converter
  static const CLID&          classID();
  /// storage Type 
  static const unsigned char storageType() ; 
  ///
protected: 
  /// miscellaneous functions which performs the conversion itself
  StatusCode createSkinSurface   ( const Surface* , G4LogicalSurface *& );
  ///
  StatusCode createBorderSurface ( const Surface* , G4LogicalSurface *& );
  ///
private: 
  ///
  GiGaSurfaceCnv           (); /// no default constructor 
  GiGaSurfaceCnv           ( const GiGaSurfaceCnv& ); /// no copy
  GiGaSurfaceCnv& operator=( const GiGaSurfaceCnv& ); /// no assignment 
  ///
private:
  ///
  Surfaces      m_surfaces;
  ///
};

// ============================================================================
// The End 
// ============================================================================
#endif   //     GIGA_GIGASurfaceCnv_H
// ============================================================================















