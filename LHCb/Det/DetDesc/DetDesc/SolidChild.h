// $Id: SolidChild.h,v 1.10 2002-04-24 10:52:24 ibelyaev Exp $ 
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.9  2001/08/15 14:34:25  ibelyaev
// remove misplacded 'protected' attribute
//
// Revision 1.8  2001/08/13 09:51:36  ibelyaev
// bug fix in 'reset' method
//
// Revision 1.7  2001/08/09 16:47:58  ibelyaev
// update in interfaces and redesign of solids
// 
// ===========================================================================
#ifndef       DETDESC_SOLIDCHILD_H 
#define       DETDESC_SOLIDCHILD_H 1
/// STD and STL 
#include <functional>
// CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Vector/Rotation.h"
/// Detdesc 
#include "DetDesc/SolidException.h" 
#include "DetDesc/SolidBase.h" 
/// forward declarations 
class HepRotation;      ///< CLHEP 
class HepTransform3D;   ///< CLHEP
class MsgStream;        ///< GaudiKernel
class StreamBuffer;     ///< GaudiKernel
class SolidBoolean;     ///< DetDesc 
template <class TYPE>
class SolidFactory;     ///< DetDesc 

/** @class SolidChild SolidChild.h "DetDesc/SolidChils.h"
 *
 *  Simple helper class for implementation of "complex" boolean solids
 *  represent a "solid with position" (e.g. hole in the "mother" solid)
 *
 *  @bug cover and coverTop methods are wrong! 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date xx/xx/xxxx
 */

class SolidChild : public SolidBase
{
  ///
  friend class SolidBoolean;
  friend class SolidFactory<SolidChild>;
  ///
protected:
 
  /** constructor
   *  @param Name name of this solid 
   */
  SolidChild( const std::string& Name = "" );
  
  /** constructor    
   *  @param solid pointer to ISolid object
   *  @param mtrx  pointer to transformation 
   *  @param Name name of this solid 
   */ 
  SolidChild( ISolid*               solid      , 
              const HepTransform3D* mtrx  = 0  ,
              const std::string&    Name  = "" );
  
  /** constructor 
   *  @param solid pointer ot ISolid object
   *  @param pos   position
   *  @param rot   rotation
   *  @param Name name of this solid 
   */
  SolidChild( ISolid*               solid                 ,
              const HepPoint3D&     pos                   , 
              const HepRotation&    rot   = HepRotation() ,
              const std::string&    Name  = ""            ); 
  
public:
 
  /// destructor 
  virtual ~SolidChild();

public:
  
  /** retrieve the specific type of the solid
   *  @return specific type of the solid
   */
  inline virtual std::string typeName   () const { return "SolidChild" ; }
  
  /** retrieve the pointer to "simplified" solid - "cover"
   *  @return pointer to "simplified" solid - "cover"
   */
  inline virtual const ISolid* cover () const { return solid()->cover() ; };
  
  /** retrieve the pointer to "the most simplified cover"
   *  probably, something like "gabarite box"
   *  @return pointer to the most simplified cover
   */
  virtual inline const ISolid* coverTop () const 
  { return solid()->coverTop() ; };
  
  /** printout to STD/STL stream
   *  @param os STD/STL stream
   *  @return reference to the stream
   */
  virtual std::ostream& printOut ( std::ostream& os = std::cout ) const ;

  /** printout to Gaudi  stream
   *  @param os Gaudi stream
   *  @return reference to the stream
   */
  virtual MsgStream&    printOut ( MsgStream&    os             ) const ;

  /** - check for the given 3D-point. 
   *    Point coordinated are in the local reference 
   *    frame of the solid.   
   *  - implementation of ISolid absstract interface  
   *  @see ISolid 
   *  @param point point (in local reference system of the solid)
   *  @return true if the point is inside the solid
   */
  virtual bool isInside ( const HepPoint3D& point) const;
  
  /** reset to the initial ("after constructor") state
   */
  virtual ISolid*  reset();
  
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
  intersectionTicks ( const HepPoint3D&  Point  ,
                      const HepVector3D& Vector ,
                      ISolid::Ticks&     ticks  ) const;
  
  /** serialization for reading
   *  @param sb reference to stream buffer
   *  @return reference to stream buffer
   */
  virtual StreamBuffer& serialize( StreamBuffer& s )       ;   
  
  /** serialization for writing
   *  @param sb reference to stream buffer
   *  @return reference to stream buffer
   */
  virtual StreamBuffer& serialize( StreamBuffer& s ) const ;
  
public:

  /// "new" method - return solid itself 
  inline const ISolid* solid () const { return m_sc_solid; };
  // is solid transformed?  
  inline       bool    simple() const { return m_sc_simple; };  
  // transfromation
  inline const HepTransform3D& matrix () const  
  {
    if( 0 == m_sc_matrix ) { m_sc_matrix = new  HepTransform3D; }
    return *m_sc_matrix;
  };
  
private:
  
  /** copy constructor 
   *  @param solid reference to SolidChild object 
   */ 
  SolidChild           ( const SolidChild& solid );
  
  /** assignment  
   *  @param solid reference to SolidChild object 
   *  @return self reference 
   */ 
  SolidChild& operator=( const SolidChild& solid );
  
private:
  ///
  ISolid*                  m_sc_solid   ; ///< own solid  
  mutable HepTransform3D*  m_sc_matrix  ; ///< position/rotation  
  bool                     m_sc_simple  ; ///< flag if "simple"
  ///
};

/// ===========================================================================
#endif   ///<   DETDESC_SOLIDCHILD_H 
/// ===========================================================================
