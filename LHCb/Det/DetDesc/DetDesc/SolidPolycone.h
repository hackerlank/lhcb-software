// $Id: SolidPolycone.h,v 1.2 2002-05-11 18:25:46 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2001/10/25 14:04:04  ibelyaev
// New shape: SolidPolycone is added
// 
// ============================================================================
#ifndef DETDESC_SOLIDPOLYCONE_H 
#define DETDESC_SOLIDPOLYCONE_H 1
// Include files
/// STD and STL 
#include <cmath> 
#include <iostream>
#include <functional>
#include <algorithm>
#include <vector>
/// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Units/PhysicalConstants.h"
/// DetDesc 
#include "DetDesc/SolidBase.h" 
/// forward declarations 
template <class TYPE>
class SolidFactory;

/** @class SolidPolycone SolidPolycone.h DetDesc/SolidPolycone.h
 *  
 *  A simple implementation of "Polycone" shape
 *  "PCON" in Geant3 (G4Polycone in Geant4) 
 *  
 *
 *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
 *  @date   23/10/2001
 */

class SolidPolycone: public virtual SolidBase
{
  /// friend factory for instantiation 
  friend class SolidFactory<SolidPolycone>;

public:
  
  typedef std::pair<double,double>  Pair     ;
  typedef std::pair<double,Pair>    Triplet  ;
  typedef std::vector<Triplet>      Triplets ;
  typedef Triplets::const_iterator  Iterator ;
  
  /** Standard constructor
   *  
   *  @param Name          the name of the polycone
   *  @param Params        vector of "triplets" (z,(rMax,rMin))
   *  @param StartPhiAngle the azimuthal angle phi at which polycone "begins"
   *  @param DeltaPhiAngle the opening angle 
   */
  SolidPolycone( const std::string&  Name                       ,
                 const Triplets   &  Params                     ,
                 const double        StartPhiAngle = 0          ,
                 const double        DeltaPhiAngle = 360*degree );

  /// destructor 
  virtual ~SolidPolycone();
  
  /** - retrieve the polycone  type 
   *  - implementation of ISolid abstract interface 
   *  @see ISolid 
   *  return box type
   */
  inline std::string typeName () const { return "SolidPolycone"; };
  
  /** - check for the given 3D-point. 
   *    Point coordinates are in the local reference 
   *    frame of the solid.   
   *  - implementation of ISolid absstract interface  
   *  @see ISolid 
   *  @param point point (in local reference system of the solid)
   *  @return true if the point is inside the solid
   */
  bool isInside (  const HepPoint3D& point ) const ;
  
  /** -# retrieve the pointer to "simplified" solid - "cover"
   *  -# implementation of ISolid abstract interface 
   *  @see ISolid 
   *  @return pointer to "simplified" solid - "cover"
   */
  const ISolid* cover () const ;

  /** - printout to STD/STL stream    
   *  - implementation  of ISolid abstract interface 
   *  - reimplementation of SolidBase::printOut( std::ostream& )
   *  @see SolidBase 
   *  @see ISolid 
   *  @param stream STD/STL stream
   *  @return reference to the stream 
   */
  virtual std::ostream&  printOut ( std::ostream& os = std::cout )    const;
  
  /** - printout to Gaudi MsgStream stream    
   *  - implementation  of ISolid abstract interface 
   *  - reimplementation of SolidBase::printOut( MsgStream& )
   *  @see SolidBase 
   *  @see ISolid 
   *  @param stream STD/STL stream
   *  @return reference to the stream 
   */
  virtual MsgStream&     printOut ( MsgStream&                   )    const; 
  
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
  virtual unsigned int 
  intersectionTicks ( const HepPoint3D & Point  ,  
                      const HepVector3D& Vector ,  
                      ISolid::Ticks    & ticks  ) const ;
  
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
  StreamBuffer& serialize( StreamBuffer& sb )       ; 
  
  /** - serialization for writing
   *  - implementation of ISerialize abstract interface 
   *  - reimplementation of SolidBase::serialize 
   *  @see ISerialize 
   *  @see ISolid  
   *  @see SolidBase   
   *  @param s reference to stream buffer
   *  @return reference to stream buffer
   */
  StreamBuffer& serialize( StreamBuffer& sb ) const ;
  
  /** specific for polycone
   */
  
  /** accessor to number of triplets 
   *  @return number of z-planes 
   */
  inline const unsigned int       
  number   () const { return triplets().size() ; }
  
  /** accessor to all triplets 
   *  @return reference to all triplets 
   */ 
  inline const Triplets&
  triplets () const { return m_triplets ; }
  
  /** accessor to triplets begin iterator 
   *  @return begin iterator to triplets comntainer 
   */
  inline Triplets::const_iterator 
  begin    () const { return triplets().begin(); }
  
  /** accessor to triplets end iterator 
   *  @return end iterator to triplets container 
   */
  inline Triplets::const_iterator 
  end      () const { return triplets().end  (); }

  /** accessor to z-position of plane indexed with some index
   *  @param indx   plane index 
   *  @return z-position
   */
  inline const double  z( Triplets::size_type index ) const 
  { return ( begin() + index )->first ; }
  
  /** accessor to rMax of plane indexed with some index
   *  @param indx   plane index 
   *  @return rMax
   */
  inline const double  RMax( Triplets::size_type index ) const 
  { return ( begin() + index )->second.first ; }
  
  /** accessor to rMin of plane indexed with some index
   *  @param indx   plane index 
   *  @return rMin
   */
  inline const double  RMin( Triplets::size_type index ) const 
  { return ( begin() + index )->second.second ; }
  
  /** accessor to start phi angle 
   *  @return start phi angle 
   */
  inline const double startPhiAngle() const { return m_startPhiAngle;}

  /** accessor to delta phi angle 
   *  @return delta phi angle 
   */
  inline const double deltaPhiAngle() const { return m_deltaPhiAngle;}
  
  
protected:
  
  /** @class CmpZ
   *  simple utility class (functor) for comparizon of
   *  z-posiiton of the triplet
   */ 
  class CmpZ: 
    public std::binary_function<Triplet,double,bool>
  { 
  public:
    inline bool operator() ( const Triplet& triplet , const double&  Z ) const
    { return triplet.first >= Z ; }
  };
  
protected:
  
  /** default protected  constructor 
   *  @param name name of conical tube segment  
   */
  SolidPolycone( const std::string& Name = "Anonymous Polycone") ;

  /** set bounding parameters 
   *  @return satus code 
   */
  StatusCode setBP();
  
private:
  
  SolidPolycone           ( const SolidPolycone & );  ///< no copy-constructor 
  SolidPolycone& operator=( const SolidPolycone & );  ///< no assignment 
  
private:
  
  Triplets       m_triplets      ; ///< vector of parameters 
  double         m_startPhiAngle ; ///< start phi angle 
  double         m_deltaPhiAngle ; ///< delta phi angle 
  
};

// ============================================================================
// The END 
// ============================================================================
#endif ///< DETDESC_SOLIDPOLYCONE_H
// ============================================================================

