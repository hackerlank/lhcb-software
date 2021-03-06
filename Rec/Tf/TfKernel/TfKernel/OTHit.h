
//-----------------------------------------------------------------------------
/** @file OTHit.h
 *
 *  Header file for track finding class Tf::OTHit
 *
 *  CVS Log :-
 *  $Id: OTHit.h,v 1.11 2010-04-07 05:17:03 wouter Exp $
 *
 *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
 *  @date   2007-05-30
 */
//-----------------------------------------------------------------------------

#ifndef TFKERNEL_OTHIT_H 
#define TFKERNEL_OTHIT_H 1

// Include files
#include "TfKernel/LineHit.h"
#include "GaudiKernel/Range.h"
#include "Event/OTLiteTime.h"


namespace Tf
{

  /** @class OTHit
   *
   *  Representation of an OT hit
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-05-30
   */

  class OTHit : public LineHit
  {

  public:

    typedef DeOTModule DetectorElementType ; ///< Detector element type for OT
    typedef ot_hit_tag             hit_tag ; ///< The hit type tag

  public:

    /** Constructor from a DeOTModule and an OTLiteTime
     *  @param[in] aModule Reference to the associated DeOTModule
     *  @param[in] rawhit  The raw OT hit (OTLiteTime)
     */
    OTHit( const DeOTModule& aModule, const LHCb::OTLiteTime& rawhit );

    /** Constructor from a DeOTModule and an OTLiteTime
     *  @param[in] aModule Reference to the associated DeOTModule
     *  @param[in] rawhit  The raw OT hit (OTLiteTime)
     *  @param[in] rtrel   rt relation to be used (e.g. for no drift times)
     */
    OTHit( const DeOTModule& aModule, const LHCb::OTLiteTime& rawhit,
	const OTDet::RtRelation& rtrel );

  public: // Simple accessors to internal data members

    /** Returns the calibrated time is drift time + propagation time - default tof */
    inline double calibratedTime() const noexcept { return m_rawhit.calibratedTime(); }

    /** Access the associated DeOTModule for this hit
     *  @return Reference to the associated DeOTModule */
    inline const DeOTModule& module() const noexcept { return *m_module; }

    /** Access the raw hit information (OTLiteTime)
     *  @return The OTLiteTime for this hit */
    inline const LHCb::OTLiteTime& rawhit() const noexcept { return m_rawhit ; }

    /** Access the default drift distance. 
     *  Defined at the point halfway along the length the wire.
     *  @return The drift distrance */
    inline double driftDistance()  const noexcept { return m_driftDistance; }

    /** Access the y-coordinate of the readout side of the wire (defines t0) */
    inline double  yReadout()   const noexcept { return yEnd() ; }

    /** Access the propagation time relative to the default, which is halfway along the
        wire. The velocity has been corrected for sign and direction */
    inline double approxPropagationTime( const double globaly ) const noexcept
    {
      // we ASSUME here that the average of the walk correction is zero... (it should, by construction)
      return (yReadout() - globaly) / module().propagationVelocityY();
    }

    /** Access the propagation time relative to the default, which is halfway along the
        wire. The velocity has been corrected for sign and direction */
    inline double propagationTime( const double globaly ) const noexcept
    {
      double propTime = (yReadout() - globaly) / module().propagationVelocityY();
      double dist2strawend = module().wireLength(m_rawhit.channel()) - propTime * module().propagationVelocity();
      return propTime + module().walkRelation().walk_fast(dist2strawend);
    }

    /** The drift time after correction for propagation time */
    inline double approxDriftTime( const double globaly ) const noexcept
    { 
      return calibratedTime() - approxPropagationTime( globaly ) ;
    }

    /** The drift time after correction for propagation time */
    inline double driftTime( const double globaly ) const noexcept
    { 
      return calibratedTime() - propagationTime( globaly ) ;
    }

    /** The drift distance after correction for propagation time */
    inline double driftDistance( const double globaly ) const noexcept
    { 
      return m_rtrel->radius( driftTime( globaly) ) ; 
    }

    /** Test if this hit is out-of-time */
    bool outOfTime( const double globaly, const double numsigma ) const noexcept;

    /** obsolete. please, don't use. To select valid drifttimes, cut directly on the drifttime. */
    inline double approxUntruncatedDriftDistance( const double globaly ) const noexcept
    { 
      return m_rtrel->extrapolatedradius( approxDriftTime( globaly) ) ; 
    }

    /** obsolete. please, don't use. To select valid drifttimes, cut directly on the drifttime. */
    inline double untruncatedDriftDistance( const double globaly ) const noexcept
    { 
      return m_rtrel->extrapolatedradius( driftTime( globaly) ) ; 
    }

    /** obsolete. please, don't use. To select valid drifttimes, cut directly on the drifttime. */
    inline double untruncatedDriftDistance() const noexcept
    { 
      return m_rtrel->extrapolatedradius( driftTime(yMid()) ) ; 
    }

    /** Access the straw number for this hit */
    inline unsigned int straw() const noexcept { return m_rawhit.channel().straw() ; }

    /** Access the monolayer number for this hit */
    inline unsigned int monolayer() const noexcept { return module().monoLayerB(straw()) ; }

    /** The length of the wire */
    inline double wireLength() const noexcept { return module().wireLength(m_rawhit.channel()); }

  private: // data

    const DeOTModule* m_module ;         ///< Pointer to the associated DeOTModule
    const OTDet::RtRelation* m_rtrel;	 ///< The rt relation used to convert drift times to radii
    LHCb::OTLiteTime  m_rawhit ;         ///< The raw OTLiteTime for this hit
    double            m_driftDistance ;  ///< The default drift distance. Defined at the point halfway along the length the wire.

  private: // methods
    /** set drift distance and error to avoid duplicate code in constructors */
    void setDriftDistAndErr();
  };

  /// Type for container for OTHit
  typedef std::vector<const OTHit*> OTHits;

  /// Type for range of OTHits within a container
  typedef Gaudi::Range_<OTHits> OTHitRange;

  /// Type for a container for OTHitRange
  typedef std::vector<OTHitRange> OTHitRanges;

  ////////////////////////////////////////////////////////////////////////////////////
  // Inline function definitions
  ////////////////////////////////////////////////////////////////////////////////////

  inline void OTHit::setDriftDistAndErr()
  {
    // setting things from the OTHit. the cached drift distance is the
    // one in the middle of the wire, for now.
    const double time = approxDriftTime(yMid()) ;
    OTDet::RadiusWithError r = m_rtrel->radiusWithError( time ) ;
    m_driftDistance  = r.val ;
    setError(r.err) ;
  }

  inline OTHit::OTHit( const DeOTModule& aModule, const LHCb::OTLiteTime& rawhit,
		  const OTDet::RtRelation& rtrel ) :
    LineHit(aModule, rawhit),
    m_module(&aModule),
    m_rtrel(&rtrel),
    m_rawhit(rawhit)
  { setDriftDistAndErr(); }
  
  inline OTHit::OTHit( const DeOTModule& aModule, const LHCb::OTLiteTime& rawhit ) :
    LineHit(aModule,rawhit),
    m_module(&aModule),
    m_rtrel(&aModule.rtRelation()),
    m_rawhit(rawhit)
  { setDriftDistAndErr(); }

  inline bool OTHit::outOfTime( double globaly, double numsigma ) const noexcept
  {
    const double tres = m_rtrel->drifttimeError(m_driftDistance) ;
    const double tmax = m_rtrel->tmax() ;
    const double t    = driftTime( globaly) ;
    return t<-numsigma*tres || t>tmax + numsigma*tres ;
  }

  // our dynamic casts
  inline const OTHit* HitBase::othit() const noexcept
  { 
    return ( type() == RegionID::OT ? static_cast<const OTHit*>(this) : nullptr ); 
  }

}
#endif // TFKERNEL_OTHIT_H 
