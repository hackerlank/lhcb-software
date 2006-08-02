// $Id: LAssembly.h,v 1.11 2006-08-02 09:15:47 jpalac Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
#ifndef DETDESC_LASSEMBLY_H 
#define DETDESC_LASSEMBLY_H 1
// Include files
// from DetDesc 
#include "DetDesc/LogVolBase.h"
#include "DetDesc/CLIDLAssembly.h"
#include "DetDesc/DetDesc.h"

template <class TYPE>
class DataObjectFactory;

static const std::string EmptyString("");

/** @class LAssembly LAssembly.h DetDesc/LAssembly.h
 *  
 *  The implementation of "Logical Assembly" notion.
 *  It is a logicla voluem without shape and material.
 *  It is just a placeholder for its child volumes.
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   18/11/2001
 */

class LAssembly : 
  public LogVolBase
{
  /// friend factory for instantiation 
  friend class DataObjectFactory<LAssembly>;
  
public:
  
  /** constructor
   *  @exception LVolumeException wrong paramaters value
   *  @param name name of logical volume 
   *  @param sensitivity  name of sensitive detector object (for simulation)
   *  @param magnetic  name of magnetic field object (for simulation)
   */
  LAssembly
  ( const std::string& name             ,
    ISolid*            Solid            ,
    const std::string& sensitivity = "" ,
    const std::string& magnetic    = "" );
  
  /// destructor 
  virtual ~LAssembly();
  
public:  
  
  /** is this volume "Assembly" of other volumes?
   *  @see ILVolume 
   *  notion of Assembly Volume is imported from Geant4.
   *  "Assembly Volume" has no associated material and shape, 
   *  thus material name shoudl me empty, pointer to solid
   *  and pointer to material should be both nulls
   *  @return true if volume is Assembly
   */ 
  inline virtual bool            
  isAssembly () const { return true    ; } 
  
  /** the solid, associated with the Logical Volume  
   *  @see ILVolume 
   *  @return the solid, associated with the Logical Volume  
   */
  inline virtual const ISolid*   
  solid      () const { return 0  ; }
  
 /** the material, associated with the Logical Volume  
   *  For Assembly Volumes material pointes to NULL!
   *  @see ILVolume 
   *  @exception LVolumeException no material
   *  @return the material, associated with the Logical Volume  
   */
  inline virtual const Material* 
  material   () const { return 0       ; };
  
  /** the material(by name), associated with the Logical Volume  
   *  @see ILVolume 
   *  @return the material(by name), associated with the Logical Volume  
   */
  inline virtual const std::string&  
  materialName () const { return EmptyString; }

  /** check for the given 3D-point. Point coordinates are in the 
   *  local reference frame of the logical volume
   *  If cover volume is defined, check whether point is inside it
   *  before checking inside daughters.
   *  @see ILVolume 
   *  @param LocalPoint point (in local reference system of the solid)
   *  @return true if the point is inside the solid
   */
  inline virtual bool isInside  ( const Gaudi::XYZPoint& LocalPoint ) const
  { 
    if (! isInsideCoverBox(LocalPoint) ) return false;
    return isInsideDaughter( LocalPoint ); 
    //    return isInsideCoverBox(LocalPoint) && isInsideDaughter( LocalPoint );
    //return isInsideDaughter( LocalPoint );
    /*
    return (false) 
      ?
      (m_solid->isInside(LocalPoint) && isInsideDaughter( LocalPoint ))
      :
      isInsideDaughter( LocalPoint ); 
    */
    //    return isInsideDaughter( LocalPoint );
  }
  
  /** calculate the daughter path containing the Point in Local frame , 
   *  can be VERY slow for complex geometry, 
   *  therefore use the appropriate Level for usage 
   *  @see ILVolume 
   *  @param  LocalPoint point in local reference system of logical volume 
   *  @param  Level depth level
   *  @param  pVolumePath  vector of physical volumes
   *  @return status code 
   */
  virtual StatusCode belongsTo
  ( const Gaudi::XYZPoint&        LocalPoint  ,
    const int                Level       , 
    ILVolume::PVolumePath&   pVolumePath ) const ;
  
  /** calculate the daughter path containing the Point in Local frame , 
   *  can be VERY slow for complex geometry, 
   *  therefore use the appropriate Level for usage 
   *  @see ILVolume 
   *  @param  LocalPoint point in local reference system of logical volume 
   *  @param  Level depth level
   *  @param  replicaPath  vector of physical volumes
   *  @return status code 
   */
  virtual StatusCode belongsTo
  ( const Gaudi::XYZPoint&        LocalPoint  ,
    const int                Level       , 
    ILVolume::ReplicaPath&   replicaPath ) const  ;
  
  /** intersection of the logical volume with with the line \n
   *  The line is parametrized in the local reference system 
   *  of the logical volume by initial Point and direction Vector \n 
   *  @f$ \vec{x}(t) = \vec{p} + t\times \vec{v} @f$  \n 
   * 
   *   Method returns the number of intersection points("ticks") and 
   *   the container of pairs - ticks and pointer to the corresponding 
   *   material. The simplification is determined by value of threshold
   *   (in units of radiation length) 
   *   Method throws LVolumeException in the case, then 
   *   solid is not defined or material is not accessible.
   *
   *  @see ILVolume 
   *  @exception LogVolumeException solid or/and matherial problems 
   *  @param Point initial point at the line
   *  @param Vector direction vector of the line
   *  @param intersections output container 
   *  @param threshold threshold value 
   *  @return number of intersections  
   */
  virtual unsigned int intersectLine
  ( const Gaudi::XYZPoint        & Point         , 
    const Gaudi::XYZVector       & Vector        , 
    ILVolume::Intersections & intersections , 
    const double              threshold     ) const ;
  
  /** intersection of the logical volume with with the line \n
   *  Theine is parametrized in the local reference system 
   *  of the logical volume by initial Point and direction Vector \n 
   *  @f$ \vec{x}(t) = \vec{p} + t\times \vec{v} @f$  \n 
   * 
   *   Method returns the number of intersection points("ticks") and 
   *   the container of pairs - ticks and pointer to the corresponding 
   *   material. The simplification is determined by value of threshold
   *   (in units of radiation length) 
   *   Method throws LVolumeException in the case, then 
   *   solid is not defined or material is not accessible.
   *
   *  @see ILVolume 
   *  @exception LogVolumeException solid or/and matherial problems 
   *  @param Point initial point at the line
   *  @param Vector direction vector of the line
   *  @param intersections output container
   *  @param tickMin minimum value of possible Tick
   *  @param tickMax maximum value of possible Tick
   *  @param Threshold threshold value 
   *  @return number of intersections  
   */
  virtual unsigned int intersectLine
  ( const Gaudi::XYZPoint         & Point         , 
    const Gaudi::XYZVector        & Vector        , 
    ILVolume::Intersections  & intersections , 
    const ISolid::Tick         tickMin       , 
    const ISolid::Tick         tickMax       , 
    const double               Threshold     ) const ;

  
  /** class/object identifier (static method)
   *  @return unique class identifier 
   */
  static  const CLID& classID ()           ;
  
  /** class/object identification (virtual method)
   *  @return unique class identifier 
   */      
  virtual const CLID& clID    ()     const ;
  
  /** printout to STD/STL stream
   *  @see ILVolume 
   *  @param os STD/STL stream
   *  @return reference to the stream
   */
  virtual std::ostream& printOut 
  ( std::ostream & os = std::cout ) const ;
  
  /** printout to Gaudi MsgStream stream
   *  @see ILVolume 
   *  @param os Gaudi MsgStream  stream
   *  @return reference to the stream
   */
  virtual MsgStream&    printOut
  ( MsgStream    & os             ) const;


protected:
  
  /** default constructor
   */
  LAssembly();

private:
  inline bool isInsideCoverBox(const Gaudi::XYZPoint& localPoint ) const
  {
    if (!m_coverBox) makeCoverBox();
    return ( localPoint.x() < xMin() || localPoint.x() > xMax() ||
             localPoint.y() < yMin() ||  localPoint.y() > yMax() ||
             localPoint.z() < zMin() || localPoint.z() > zMax() );
  }

  inline const bool intersectCoverBox(const Gaudi::XYZPoint& start,
                                      const Gaudi::XYZPoint& end    ) const
  {
    if (!m_coverBox) makeCoverBox();
    if ( (zMin() > start.z()) && (zMin() > end.z()) ) return false ;
    if ( (zMax() < start.z()) && (zMax() < end.z()) ) return false ;
    if ( (xMin() > start.x()) && (xMin() > end.x()) ) return false ;
    if ( (xMax() < start.x()) && (xMax() < end.x()) ) return false ;
    if ( (yMin() > start.y()) && (yMin() > end.y()) ) return false ;
    if ( (yMax() < start.y()) && (yMax() < end.y()) ) return false ;
    return true;
    
  }
  
  
  void   computeCoverBoxParams () const;
  inline bool coverBoxComputed() const { return m_coverComputed; }
  void makeCoverBox() const;
  void makeCoverBoxSolid() const;

  inline double xMin() const   { return m_xMin;  }
  inline double xMax() const   { return m_xMax;  }
  inline double yMin() const   { return m_yMin;  }
  inline double yMax() const   { return m_yMax;  }
  inline double zMin() const   { return m_zMin;  }
  inline double zMax() const   { return m_zMax;  }
  
private:
  mutable ISolid* m_solid;
  mutable double m_xMin;
  mutable double m_xMax;
  mutable double m_yMin;
  mutable double m_yMax;
  mutable double m_zMin;
  mutable double m_zMax;
  mutable bool   m_coverComputed;
  mutable bool   m_coverBox;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // DETDESC_LASSEMBLY_H
// ============================================================================
