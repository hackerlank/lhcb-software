
//-----------------------------------------------------------------------------
/** @file HitBase.h
 *
 *  Header file for track find hit base class Tf::HitBase
 *
 *  CVS Log :-
 *  $Id: HitBase.h,v 1.17 2010-01-19 13:18:35 smenzeme Exp $
 *
 *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
 *  @date   2007-05-30
 */
//-----------------------------------------------------------------------------

#ifndef TFKERNEL_HITBASE_H
#define TFKERNEL_HITBASE_H 1

#include <cmath>

// STL
#include <bitset>

// LHCbKernel
#include "Kernel/LHCbID.h"

// TfKernel
#include "TfKernel/RegionID.h"

namespace Tf
{

  // forward declarations
  class VeloRHit;
  class VeloPhiHit;
  class STHit;
  class OTHit;

  /** @class HitBase TfKernel/HitBase.h
   *  Base class for storage of hits in all tracking systems
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-05-30
   */
  class HitBase
  {

  public:

    /** @struct hit_base_tag
     *  @brief type tag for hit base */
    struct hit_base_tag                          {}; 
    /** @struct line_hit_tag
      * @brief type tag for line hit */
    struct line_hit_tag    : public hit_base_tag {}; 
    /** @struct ot_hit_tag 
      * @brief type tag for OT hit */
    struct ot_hit_tag      : public line_hit_tag {};
    /** @struct st_hit_tag
      * @brief type tag for ST hit */
    struct st_hit_tag      : public line_hit_tag {}; 
    /** @struct velo_hit_tag
      * @brief type tag for Velo hit */
    struct velo_hit_tag    : public hit_base_tag {}; 
    /** @struct velo_rhit_tag
      * @brief type tag for Velo R hit */
    struct velo_rhit_tag   : public velo_hit_tag {}; 
    /** @struct velo_phihit_tag
      * @brief type tag for Velo Phi hit */
    struct velo_phihit_tag : public velo_hit_tag {}; 

  public:

    /** @enum EStatus
     *
     *  Enumerated type for bits in status flag.
     *
     *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
     *  @date   2007-05-30
     */
    enum EStatus
      {
        Unusable=0,         ///< Hit is unusable (XXX???XXX On what basis, decided by whom?)
        UsedByUnknown,      ///< Hit has been used by an unkown algorithm
        UsedByTsaSeeding,   ///< Hit has been used by the Tsa Seeding algorithm
        UsedByPatForward,   ///< Hit has been used by the Pat Forward tracking algorithm
        UsedByPatSeeding,   ///< Hit has been used by the Pat Seeding algorithm
        UsedByVeloOpen,     ///< Hit has been used by the Velo Open tracking algorithm
        UsedByVeloGeneric,  ///< Hit has been used by the Velo Generic tracking algorithm
        UsedByVeloGeneral,  ///< Hit has been used by the Velo General tracking algorithm
        UsedByVeloSpace,    ///< Hit has been used by the Velo Space tracking algorithm 
        UsedByVeloRZ,       ///< Hit has been used by the Velo R-Z tracking algorithm
        UsedByPatMatch,     ///< Hit has been used by the Pat Matching algorithm (TT hit)
        UsedByPatDownstream,///< Hit has been used by the Pat Downstream algorithm (TT hit) 
        NUMSTATUSBITS       ///< Number of status types - For internal use.
      };

    /// Nested type that describes 'use' status of this hit
    typedef std::bitset<NUMSTATUSBITS> StatusFlag ;

    /// The hit type tag 
    typedef hit_base_tag hit_type_tag;


  public:

    //== Simple accessors to internal data members

    /** Access the coord value (XXX???XXX what is coord X ? Y ? Z ? )
     *  @return The value of coord for this hit */
    inline double        coord()         const noexcept { return m_coord ; }

    /** Access the variance value
     *  @return The value of the variance for this hit */
    inline double        variance()      const noexcept { return 1. / (m_weight * m_weight); }

    /** Access the weight (1/variance) value
     *  @return The value of the weight for this hit */
    inline double        weight  ()      const noexcept { return m_weight * m_weight; }

    /** Access the variance value
     *  @return The value of the variance for this hit */
    inline double        error()         const noexcept { return 1. / m_weight; }

    /** Access the (error) weight (1/error) value
     *  @return The value of the weight for this hit */
    inline double        errweight  ()   const noexcept { return m_weight; }

    /** Access the LHCbID channel identifier
     *  @return The channel identifier for this hit */
    inline LHCb::LHCbID lhcbID  ()      const noexcept { return m_lhcbID; }

    /** Access the RegionID for this hit
     *  @return The channel identifier for this hit */
    inline RegionID     regionID()      const noexcept { return m_regionID; }

    /** Access the LHCbID channel identifier for this
     *  @return The channel identifier for this hit */
    inline StatusFlag   status  ()      const noexcept { return m_status; }

    /** @brief Test if this hit satisfies the given status type.
     *  E.g.
     *  @code
     *   // test if the hit was previously used by the Tsa Seeding algorithm
     *   bool wasUsedByTsaSeeding = hit->testStatus(HitBase::UsedByTsaSeeding);
     *  @endcode
     *  @param stat The flag to test
     *  @return The status of the test
     *  @retval TRUE  The hit has the given status flag set
     *  @retval FALSE The hit does not have the given status flag set
     */
    inline bool         testStatus( const EStatus stat) const noexcept { return m_status.test(stat); }

    /** Set the given status flag for this hit
     *  @param[in] stat The status type
     *  @param[in] val  The flag value (true or false) for the given status type
     */
    inline void         setStatus ( const EStatus stat,
                                    const bool val = true ) const noexcept { m_status.set(stat,val) ; }

    /** Rest USED Flag, e.g. needed to start from scratch offline reco after HLT */
    
    inline void         resetUsedFlag () const noexcept { m_status = 0; }

    /** Test if the hit has been used by any algorithm
     *  @retval TRUE  The hit has been used by at least one algorithm
     *  @retval FALSE The hit has not been used by any algorithm yet
     */
    inline bool         isUsed    () const noexcept { return m_status.any() ; }

    /** Label this hit as having been used by an anonymous algorithm
     *  @param[in] flag Flag value to set (true=used, false=not used)
     */
    inline void         setUsed   ( const bool flag ) const noexcept { m_status.set(UsedByUnknown,flag) ; }

    /** Test if the hit has been used only by any algorithm other then the one specified 
     *  @param[in] stat The status type of the algorithm we don't care about
     *  @retval TRUE  The hit has been used by any other than the specified algorithm
     *  @retval FALSE The hit has not been used at all or only by the specified algorithm.
     */
    inline bool      isUsedByAnyOtherThan ( const EStatus stat ) const noexcept { 
      return 0 != (static_cast<unsigned long>(~(1 << stat)) & m_status.to_ulong());
    }
    /** Test if the hit has been used only by a specific algorithm
     *  @param[in] stat The status type
     *  @retval TRUE  The hit has been used and only by this algorithm
     *  @retval FALSE The hit has not been used at all or by any other algorithm.
     */
    inline bool      isUsedOnlyBy ( const EStatus stat ) const noexcept { 
      return m_status.test(stat) && ( 0 == (static_cast<unsigned long>(~(1 << stat)) & m_status.to_ulong()));
    }

    /** Ask by how many algorithms this hit was used.
     *  @retval Number of algorithms that used this hit.
     */
    inline unsigned int isUsedNTimes () const noexcept { return m_status.count(); }

    /** Access the ignore flag of the hit */
    inline bool ignore() const noexcept { return m_ignore; }

    /** Set the ignore flag of the hit */
    inline void setIgnore(const bool ignore) const noexcept { m_ignore = ignore; }

    /** Access the region number for the hit */
    inline unsigned int region  () const noexcept { return static_cast<unsigned int>(m_regionID.region()); }

    /** Access the layer number for the hit */
    inline unsigned int layer   () const noexcept { return static_cast<unsigned int>(m_regionID.layer()); }

    /** Access the station number for the hit */
    inline unsigned int station () const noexcept { return static_cast<unsigned int>(m_regionID.station()); }

    /** Access the region(detector) type for the hit */
    inline unsigned int type    () const noexcept { return static_cast<unsigned int>(m_regionID.type()); }

    /** Access the velo half for the hit.  
     * Internally stored as a layer, but client code is more readable using this accessor.
     */
    inline unsigned int veloHalf() const noexcept { return static_cast<unsigned int>(m_regionID.veloHalf()); }

    /** Access the zone number for the hit.  
     * A VELO r hit can be in zones 0-3 in the direction of global phi. 
     * A VELO  phi hit either in the inner or outer zone of a phi sensor. 
     */
    inline unsigned int zone    () const noexcept { return static_cast<unsigned int>(m_regionID.zone()); }

    // dynamic casts

    /** Dynamic cast this base hit to a VeloRHit hit
     *  @return Pointer to a VeloRHit hit
     *  @retval NULL     Hit is NOT of type VeloRHit
     *  @retval NON-NULL Hit is of type VeloRHit
     */
    const VeloRHit*   veloRHit()   const noexcept;

    /** Dynamic cast this base hit to a VeloPhiHit hit
     *  @return Pointer to a VeloPhiHit hit
     *  @retval NULL     Hit is NOT of type VeloPhiHit
     *  @retval NON-NULL Hit is of type VeloPhiHit
     */
    const VeloPhiHit* veloPhiHit() const noexcept;

    /** Dynamic cast this base hit to a OTHit hit
     *  @return Pointer to a OTHit hit
     *  @retval NULL     Hit is NOT of type OTHit
     *  @retval NON-NULL Hit is of type OTHit
     */
    const OTHit*      othit()      const noexcept;

    /** Dynamic cast this base hit to a STHit hit
     *  @return Pointer to a STHit hit
     *  @retval NULL     Hit is NOT of type STHit
     *  @retval NON-NULL Hit is of type STHit
     */
    const STHit*      sthit()      const noexcept;

  protected:

    // only inherited objects are supposed to be created

    /** Default constructor */
    HitBase() {}

    /** Constructor from hit information
     *  @param id       The LHCbID channel ID for this hit
     *  @param regionid The RegionID for this hit
     *  @param coord    The coord value for this hit (XXX???XXX What is this ?)
     *  @param error    The value of the error for this hit
     */
    HitBase( const LHCb::LHCbID id,
             const RegionID& regionid,
             const double coord    = 0,
             const double error = 0 )
      : m_coord(coord), m_weight(1. / error), m_lhcbID(id), m_regionID(regionid), m_ignore (false) {}

    /** Set the coord value
     *  @param x The coord value to use 
     */
    void setCoord(const double x) { m_coord = x ; }

    /** Set the variance value
     *  @param v The variance value to use 
     */
    void setVariance(const double v) { m_weight = 1. / std::sqrt(v) ; }

    /** Set the error value
     *  @param v The variance value to use 
     */
    void setError(const double e) { m_weight = 1. / e ; }

  protected:

    double m_coord;                 ///< The hit coord (XXX???XXX X value ?)
    double m_weight;                ///< The hit weight (1/error)
    LHCb::LHCbID m_lhcbID;          ///< The hit LHCbID channel identifier
    mutable StatusFlag m_status;    ///< The hit status word (bit packed)
    RegionID m_regionID;            ///< The hit RegionID
    mutable bool m_ignore;          ///< Flag for efficiency studies 


  };

}

#endif // TFKERNEL_HITBASE_H
