// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/Lib/GeometryInfo.h,v 1.6 2001-03-15 12:43:40 ibelyaev Exp $
#ifndef     DETDESC_GEOMETRYINFO_H
#define     DETDESC_GEOMETRYINFO_H 1 
// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Transform3D.h"
// GaudiKernel
#include "GaudiKernel/IDataDirectory.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/IGeometryInfo.h" 
#include "DetDesc/IDetectorElement.h" 
// GaudiKernel
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
// STD and STL 
#include <iostream>
#include <string>
#include <functional>
#include <algorithm> 
//
class MsgStream;
class GaudiException;
class DetectorElement;
//


/** @class GeometryInfo GeometryInfo.h DetDesc/GeometryInfo.h

    The most trivial implementation of IGeomtryInfo interface 
    
    @author Vanya Belyaev 
*/


class GeometryInfo:   public IGeometryInfo
{
  //
  friend class DetectorElement;
  //
public:
  //
  typedef  std::vector<std::string>     ChildName;
  //  
 protected:
  // constructor, creates "ghost"
  GeometryInfo( IDetectorElement*            de         ); 
  // constructor, creates "orphan"   
  GeometryInfo( IDetectorElement*            de          ,
                const std::string&           LogVol      ); 
  // constructor, create regular element 
  GeometryInfo( IDetectorElement*            de          ,
                const std::string&           LogVol      , 
                const std::string&           Support     ,
                const ILVolume::ReplicaPath& ReplicaPath ); 
  // constructor, create regular element 
  GeometryInfo( IDetectorElement*            de              ,
                const std::string&           LogVol          , 
                const std::string&           Support         ,
                const std::string&           ReplicaNamePath );
  // destructor 
  virtual ~GeometryInfo();
 private:
  // copy constructor is private! 
  GeometryInfo           ( const GeometryInfo& );
  // assignment is private! 
  GeometryInfo& operator=( const GeometryInfo& );
  //
 public:
  /// All functions form IGeometryInfo interface. 
  /// Is this "geometry object" assosiated with Logical Volume?
  inline       bool             hasLVolume   () const ;   //                             
  /// Is this "geometry object" supported?
  inline       bool             hasSupport   () const ;   //                             
  /// transformation matrix from global reference system to the local one 
  inline const HepTransform3D&  matrix       () const ; 
  /// transformation matrix from local  reference system to the global one 
  inline const HepTransform3D&  matrixInv    () const ; 
  /// tranform the point from the global reference systemn to the local  reference system  
  inline       HepPoint3D       toLocal      ( const HepPoint3D& globalPoint ) const ;       
  /// tranform the point from the local  reference systemn to the global reference system  
  inline       HepPoint3D       toGlobal     ( const HepPoint3D& localPoint  ) const ;       
  /// is the given point in the global reference system inside this detector element?
  inline       bool             isInside     ( const HepPoint3D& globalPoint ) const ;      
  /// return the name of the daughter element to which the given point belongs to 
  inline       std::string      belongsToPath( const HepPoint3D& globalPoint );      
  /// return the C++ pointer to the daughter element to which the given point belongs to    
  inline       IGeometryInfo*   belongsTo    ( const HepPoint3D& globalPoint );     
  /// return the name of the daughter element to which the given point belongs to (taking into account the level)
  inline       std::string      belongsToPath( const HepPoint3D& globalPoint ,       
                                               const int         level       );       
  /// return the C++ pointer to the daughter element to which the given point belongs to  (taking into account the level)   
  inline       IGeometryInfo*   belongsTo    ( const HepPoint3D& globalPoint ,        
                                               const int         level       );      
  /// return the full geometry info for a given point in teh global reference system 
  inline StatusCode fullGeoInfoForPoint( const HepPoint3D&        point      , 
                                         const int                level      , 
                                         IGeometryInfo*&          start      , 
                                         ILVolume::PVolumePath&   volumePath );       
  /// return the full geometry info for a given point in teh global reference system 
  inline StatusCode fullGeoInfoForPoint( const HepPoint3D&        point      , 
                                         const int                level      , 
                                         IGeometryInfo*&          start      , 
                                         ILVolume::ReplicaPath&   volumePath );      
  /// return the full geometry info for a given point in teh global reference system 
  inline StatusCode fullGeoInfoForPoint( const HepPoint3D&        point      , 
                                         const int                level      , 
                                         std::string&             start      , 
                                         ILVolume::PVolumePath&   volumePath );       
  /// return the full geometry info for a given point in teh global reference system 
  inline StatusCode fullGeoInfoForPoint( const HepPoint3D&        point      , 
                                         const int                level      , 
                                         std::string&             start      , 
                                         ILVolume::ReplicaPath&   volumePath );     
  /// return teh name of the assosiated logical volume 
  inline const std::string&      lvolumeName() const ;                                        
  /// return the C++ pointer4 to the assosiated logical volume 
  inline       ILVolume*         lvolume    () const ;                                             
  /// return the location (address) of this geometry object  
  inline StatusCode       location   ( IGeometryInfo*&              start , 
                                       ILVolume::ReplicaPath&       replicaPath ) const ;   
  /// return the location (address) of this geometry object  
  inline StatusCode       location   ( std::string&                 start , 
                                       ILVolume::ReplicaPath&       replicaPath ) const ;   
  /// return the name of the logical volume with a given address 
  inline std::string      lvolumePath( const std::string&           start       , 
                                       const ILVolume::ReplicaPath& replicaPath );   
  /// return the C++ pointer to  the logical volume with a given address 
  inline ILVolume*        lvolume    ( const std::string&           start       , 
                                       const ILVolume::ReplicaPath& replicaPath );    
  /// return the name of the logical volume with a given address 
  inline std::string      lvolumePath( IGeometryInfo*               start       , 
                                       const ILVolume::ReplicaPath& replicaPath );   
  /// return the C++ pointer to  the logical volume with a given address 
  inline ILVolume*        lvolume    ( IGeometryInfo*               start       , 
                                       const ILVolume::ReplicaPath& replicaPath );  
  /// overloades printout to  std::ostream 
  virtual std::ostream& printOut   ( std::ostream& ) const;
  /// overloiaded printout to the MsgStream 
  virtual MsgStream&    printOut   ( MsgStream&    ) const;
  /// reset to the initial state 
  inline  const IGeometryInfo*  reset() const; 
  
  /** functions for simplifications of navigation functions 
      (according to feedback from users to release v3)
  */
  
  /// pointer to the parent IGeometryInfo                  ( could throw GeometryInfoException )
  inline virtual       IGeometryInfo*               parentIGeometryInfo()       ;
  
  /// pointer to the parent IGeometryInfo (const version)  ( could throw GeometryInfoException )
  inline virtual const IGeometryInfo*               parentIGeometryInfo() const ;

  /// (reference to) container of children IGeometryInfo ( could throw GeometryInfoException )
  inline virtual       IGeometryInfo::IGIChildrens& childIGeometryInfos()       ; 

  /// (reference to) container of children IGeometryInfo ( could throw GeometryInfoException )
  inline virtual const IGeometryInfo::IGIChildrens& childIGeometryInfos() const ; 
  /// iterators for manipulation of daughter elements 
  /// begin iterator 
  inline virtual IGeometryInfo::IGIChildrens::iterator        childBegin()       ; 
  /// begin iterator (const version)
  inline virtual IGeometryInfo::IGIChildrens::const_iterator  childBegin() const ; 
  /// end  iterator 
  inline virtual IGeometryInfo::IGIChildrens::iterator        childEnd  ()       ; 
  /// end  iterator (const version)
  inline virtual IGeometryInfo::IGIChildrens::const_iterator  childEnd  () const ; 
  /// IInspectable interface
  virtual bool acceptInspector( IInspector* )       ; 
  ///
  virtual bool acceptInspector( IInspector* ) const ; 
  ///
  /// serialization  for reading 
  virtual StreamBuffer& serialize( StreamBuffer& )       ; 
  /// serialization  for writing 
  virtual StreamBuffer& serialize( StreamBuffer& ) const ; 

 private:
  ///
  inline bool              isInsideDaughter ( const HepPoint3D& globalPoint ) const ;   
  inline bool              childLoaded      () const { return m_gi_childLoaded ; }                               
  inline StatusCode        reset            ( const int Level );                           
  inline IDetectorElement* detElem          () const { return m_gi_iDetectorElement ; }                                    
  inline IMessageSvc*      msgSvc           () const { return m_gi_messSvc          ; }                                    
  inline IDataProviderSvc* dataSvc          () const { return m_gi_dataSvc          ; }                                           
  inline IGeometryInfo*    geoByName        ( const std::string& name ) const ;               
  inline ILVolume*         findLogical      () const ;                                      
  inline HepTransform3D*   calculateMatrix  () const ;                                   
  /// could throw GeometryInfoException
  inline StatusCode                    loadChildren     () const ;                                    
  /// retrive the replica path (mistrerious "rpath")
  inline        ILVolume::ReplicaPath& supportPath     () const ; 
  /// Assertion 
  inline void                Assert       ( bool assertion          , 
                                            const std::string& name ) const; 
  /// Assertion 
  inline void                Assert       ( bool assertion           , 
                                            const std::string& name  , 
                                            const GaudiException& ge ) const; 
  ///
 private:
  // Logical Volume
  /// flag for logicac volume association 
  bool                                 m_gi_has_logical     ;    
  /// name of logical volume ( full path (address) in the Transient Store )  
  std::string                          m_gi_lvolumeName     ;  
  /// C++ pointer to the logical volume (loaded on demand only!)
  mutable ILVolume*                    m_gi_lvolume         ;      // the logical volume itself 
  //matrices  
  /// transformation from the  global reference system to the local  reference system
  mutable HepTransform3D*              m_gi_matrix          ;
  /// transformation FROM local reference system  to the global  reference system
  mutable HepTransform3D*              m_gi_matrixInv       ;      // inverse transformation 
  // address:
  /// flag for support association 
  bool                                 m_gi_has_support     ; 
  /// name of DetectorElement (full path(address) in the Transient Store) , which supports the addres)
  std::string                          m_gi_supportName     ;   
  /// C++ pointer to element, which supports the address (loaded on demand) 
  mutable IGeometryInfo*               m_gi_support         ;  
  /// the address itself (numeric replic apath) 
  mutable ILVolume::ReplicaPath        m_gi_supportPath     ;   
  /// another form of address  (name replica path)
  std::string                          m_gi_supportNamePath ;   
  //
 private:
  // technicalities
  // The corresponding IDtectorElement object (NB! important! the dynamic_cast to DataObject* is mandatory)
  mutable IDetectorElement*            m_gi_iDetectorElement ;        
  //
 private: 
  //
  //internal technicalities  
  /// flag for  parent object 
  mutable bool                         m_gi_parentLoaded   ;
  /// C++ pointer to parent object (resolved on demand only) 
  mutable IGeometryInfo*               m_gi_parent         ; 
  /// flag for children objects 
  mutable bool                         m_gi_childLoaded    ;
  /// container of pointers to children objects (resolved on demand only)
  mutable IGeometryInfo::IGIChildrens  m_gi_childrens      ;
  /// container of names of children objects (resolved on demand only)
  mutable GeometryInfo::ChildName      m_gi_childrensNames ; 
  /// pointer to data provider service (obtained either via concructor or via DataObject facilities from DetectorElement
  mutable IDataProviderSvc*            m_gi_dataSvc        ;
  /// pointer to message service used for printout 
  IMessageSvc*                         m_gi_messSvc        ; 
  //  
};
///
#include "GeometryInfo.icpp" 
///
#endif   // __DETDESC_GEOMETRYINFO_GEOMETRYINFO_H__




