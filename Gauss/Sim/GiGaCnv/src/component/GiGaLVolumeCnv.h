// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGaCnv/src/component/GiGaLVolumeCnv.h,v 1.1.1.1 2001-04-23 08:34:15 ibelyaev Exp $ 
#ifndef     GIGA_GIGALVOLUMECNV_H
#define     GIGA_GIGALVOLUMECNV_H 1 
/// GiGaCnv
#include "GiGaCnv/GiGaCnvBase.h"
///
template <class T> 
class CnvFactory;

/** @class GiGaLVolumeCnv     GiGaLVolumeCnv.h GiGa/GiGaLVolumeCnv.h

    converter of LVolumes into Geant4 G4LogicalVolume 

    @author Vanya Belyaev
*/


class GiGaLVolumeCnv: public GiGaCnvBase
{
  ///
  friend class CnvFactory<GiGaLVolumeCnv>;
  ///
protected:
  ///
  GiGaLVolumeCnv( ISvcLocator* Locator );
  /// Standard (virtual) destructor 
  virtual ~GiGaLVolumeCnv();
  ///
public:
  /// create representation
  virtual StatusCode createRep( DataObject*     Object  , IOpaqueAddress*& Address ) ;
  /// Update representation 
  virtual StatusCode updateRep( DataObject*     Object  , IOpaqueAddress*  Address ) ; 
  /// class ID for converted objects
  static const CLID&         classID();
  /// storage Type 
  static const unsigned char storageType() ; 
  ///
private:
  ///
  GiGaLVolumeCnv           ()                       ; /// no default constructor 
  GiGaLVolumeCnv           ( const GiGaLVolumeCnv& ); /// no copy
  GiGaLVolumeCnv& operator=( const GiGaLVolumeCnv& ); /// no assignment  
  ///
};




#endif      GIGA_GIGALVOLUMECNV_H
