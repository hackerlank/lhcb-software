/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.8  2001/08/09 16:47:58  ibelyaev
/// update in interfaces and redesign of solids
/// 
/// ===========================================================================
#ifndef     DETDESC_SOLIDBOX_H
#define     DETDESC_SOLIDBOX_H
/// STD and STL 
#include <cmath> 
#include <iostream> 
/// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
/// GAUDI Kernel 
#include "GaudiKernel/StreamBuffer.h" 
/// DetDesc 
#include "DetDesc/SolidBase.h" 
/// forward declarations
template <class TYPE>
class SolidFactory;          ///< DetDesc 

/** @class SolidBox SolidBox.h "DetDesc/SolidBox.h"
 *
 *  A simple implementation of BOX 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date xx/xx/xxx
 */

class SolidBox: public virtual SolidBase
{
  /// friend factory 
  friend class SolidFactory<SolidBox>;

public:
  
  /** standard(public)  constructor 
   *  @param      Name        name of this box  
   *  @param      xHalf half-size of the box in x-direction
   *  @param      yHalf half-size of the box in y-direction
   *  @param      zHalf half-size of the box in z-direction
   *  @exception  SolidException wrong parameters range 
   */
  SolidBox( const std::string& Name  ,
            const double       xHalf , 
            const double       yHalf , 
            const double       zHalf); 
  
  /** destructor  
   */
  virtual ~SolidBox();  
  
  /** - retrieve the box type 
   *  - implementation of ISolid abstract interface 
   *  @see ISolid 
   *  return box type
   */
  inline       std::string  typeName ()  const { return "SolidBox" ; };
  
  /** - check for the given 3D-point. 
   *    Point coordinated are in the local reference 
   *    frame of the solid.   
   *  - implementation of ISolid absstract interface  
   *  @see ISolid 
   *  @param point point (in local reference system of the solid)
   *  @return true if the point is inside the solid
   */
  virtual inline bool isInside  ( const HepPoint3D& point ) const;
  
  /** - retrieve the pointer to "simplified" solid - "cover"
   *  - implementation of ISolid abstract interface 
   *  The Box is the most simple shape
   *  @see ISolid 
   *  @return pointer to "simplified" solid - "cover"
   */
  virtual const ISolid* cover () const ;
  
  /** - retrieve the pointer to "the most simplified cover" 
   *  - implementation  of ISolid abstract interface 
   *  - reimplementation of SolidBase::coverTop() 
   *  The Box is the most simple shape
   *  @see ISolid 
   *  @see SolidBase 
   *  @return pointer to the most simplified cover 
   */
  virtual const ISolid* coverTop () const ;
  
  /** - printout to STD/STL stream    
   *  - implementation  of ISolid abstract interface 
   *  - reimplementation of SolidBase::printOut( std::ostream& )
   *  @see SolidBase 
   *  @see ISolid 
   *  @param os STD/STL stream
   *  @return reference to the stream 
   */
  virtual std::ostream& printOut ( std::ostream& os ) const;
  
  /** - printout to Gaudi MsgStream stream    
   *  - implementation  of ISolid abstract interface 
   *  - reimplementation of SolidBase::printOut( MsgStream& )
   *  @see SolidBase 
   *  @see ISolid 
   *  @param os Gaudi MsgStream  stream
   *  @return reference to the stream 
   */
  virtual MsgStream&    printOut ( MsgStream&    os ) const;
  
  /** - calculate the intersection points("ticks") of the solid objects 
   *    with given line. 
   *  -# Line is parametrized with parameter \a t :
   *     \f$ \vec{x}(t) = \vec{p} + t \times \vec{v} \f$ 
   *      - \f$ \vec{p} \f$ is a point on the line 
   *      - \f$ \vec{v} \f$ is a vector along the line  
   *  -# \a tick is just a value of parameter \a t, at which the
   *    intersection of the solid and the line occurs
   *  -# both  \a Point  (\f$\vec{p}\f$) and \a Vector  
   *    (\f$\vec{v}\f$) are defined in local reference system 
   *   of the solid 
   *  - implementation of ISolid abstract interface  
   *  @see ISolid 
   *  @param Point initial point for the line
   *  @param Vector vector along the line
   *  @param ticks output container of "Ticks"
   *  @return the number of intersection points
   */
  virtual inline  unsigned int 
  intersectionTicks 
  ( const HepPoint3D&  Point  ,
    const HepVector3D& Vector ,
    ISolid::Ticks   &  ticks  ) const ; 

  /** calculate the intersection points("ticks") of the solid objects 
   *  with given line. 
   *  - Line is parametrized with parameter \a t : 
   *     \f$ \vec{x}(t) = \vec{p} + t \times \vec{v} \f$ 
   *      - \f$ \vec{p} \f$ is a point on the line 
   *      - \f$ \vec{v} \f$ is a vector along the line  
   *  - \a tick is just a value of parameter \a t, at which the
   *    intersection of the solid and the line occurs
   *  - both  \a Point  (\f$\vec{p}\f$) and \a Vector  
   *    (\f$\vec{v}\f$) are defined in local reference system 
   *   of the solid 
   *  Only intersection ticks within the range 
   *   \a tickMin and \a tickMax are taken into account.
   *  @see ISolid::intersectionTicks()
   *  @param Point initial point for the line
   *  @param Vector vector along the line
   *  @param tickMin minimum value of Tick 
   *  @param tickMax maximu value of Tick 
   *  @param ticks output container of "Ticks"
   *  @return the number of intersection points
   */
  virtual unsigned int
  intersectionTicks 
  ( const HepPoint3D & Point   ,
    const HepVector3D& Vector  ,
    const Tick       & tickMin ,
    const Tick       & tickMax ,
    Ticks            & ticks   ) const ;

  /** - serialization for reading
   *  - implementation of ISerialize abstract interface 
   *  - reimplementation of SolidBase::serialize 
   *  @see ISerialize 
   *  @see ISolid  
   *  @see SolidBase   
   *  @param      s               reference to stream buffer
   *  @exception  SolidException  wrong parameters range 
   *  @return reference to stream buffer
   */
  virtual StreamBuffer& serialize( StreamBuffer& s )       ;
  
  /** - serialization for writing
   *  - implementation of ISerialize abstract interface 
   *  - reimplementation of SolidBase::serialize 
   *  @see ISerialize 
   *  @see ISolid  
   *  @see SolidBase   
   *  @param s reference to stream buffer
   *  @return reference to stream buffer
   */
  virtual StreamBuffer& serialize( StreamBuffer& s ) const ; 
  
  /**  return the full x-size of the box 
   *  @return the full x-size of the box 
   */
  inline         double  xsize      ()  const 
  { return m_box_xHalfLength*2 ; };
  
  /**  return the full y-size of the box 
   *  @return the full y-size of the box 
   */
  inline         double  ysize      ()  const 
  { return m_box_yHalfLength*2 ; };
  
  /**  return the full z-size of the box 
   *  @return the full z-size of the box 
   */
  inline         double  zsize      ()  const 
  { return m_box_zHalfLength*2 ; };
  
  /**  return the half x-size of the box 
   *  @return the half x-size of the box 
   */
  inline         double  xHalfLength()  const 
  { return m_box_xHalfLength   ; };
  
  /**  return the half y-size of the box 
   *  @return the half y-size of the box 
   */
  inline         double  yHalfLength()  const 
  { return m_box_yHalfLength   ; };
  
  /**  return the half z-size of the box 
   *  @return the half z-size of the box 
   */
  inline         double  zHalfLength()  const 
  { return m_box_zHalfLength   ; };
  
protected:
  
  /** default (protected) constructor 
   */
  SolidBox(); 
  
  /** set parameters for bounding solids (box, sphere and cylinder)
   *  @return status code 
   */
  StatusCode setBP() ;
  
private:
  
  SolidBox           ( const SolidBox & );  ///< no copy-constructor 
  SolidBox& operator=( const SolidBox & );  ///< no assignment 
  
private:
  
  double               m_box_xHalfLength ;   ///< x/2 
  double               m_box_yHalfLength ;   ///< y/2 
  double               m_box_zHalfLength ;   ///< z/2

};

/// ===========================================================================
/** - check for the given 3D-point. 
 *    Point coordinated are in the local reference 
 *    frame of the solid.   
 *  - implementation of ISolid absstract interface  
 *  @see ISolid 
 *  @param point point (in local reference system of the solid)
 *  @return true if the point is inside the solid
 */
/// ===========================================================================
inline bool SolidBox::isInside( const HepPoint3D& point ) const
{ 
  if ( abs( point.z() ) > zHalfLength() || 
       abs( point.x() ) > xHalfLength() ||
       abs( point.y() ) > yHalfLength() ) { return false; }
  return true; 
};

/// ===========================================================================
#endif ///<  DETDESC_SOLIDBOX_H
/// ===========================================================================






