// $Id: PackedMCRichOpticalPhoton.h,v 1.1 2009-10-21 16:40:17 jonrob Exp $
#ifndef EVENT_PACKEDMCRICHOPTICALPHOTON_H
#define EVENT_PACKEDMCRICHOPTICALPHOTON_H 1

#include <string>

// Kernel
#include "Kernel/StandardPacker.h"

// Event
#include "Event/MCRichOpticalPhoton.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

class GaudiAlgorithm;

namespace LHCb
{
  // -----------------------------------------------------------------------

  /** @struct PackedMCRichOpticalPhoton Event/PackedMCRichOpticalPhoton.h
   *
   *  Packed MCRichOpticalPhoton
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  struct PackedMCRichOpticalPhoton
  {
    /// Default constructor
    PackedMCRichOpticalPhoton()
      : key(0),
        hpdx(0), hpdy(0), hpdz(0),
        pmirx(0), pmiry(0), pmirz(0),
        smirx(0), smiry(0), smirz(0),
        aerox(0), aeroy(0), aeroz(0),
        theta(0), phi(0),
        emisx(0), emisy(0), emisz(0),
        energy(0),
        pmomx(0), pmomy(0), pmomz(0),
        hpdqwx(0), hpdqwy(0), hpdqwz(0),
        mcrichhit(-1)
    {}

    int   key;
    int   hpdx, hpdy, hpdz;
    int   pmirx, pmiry, pmirz;
    int   smirx, smiry, smirz;
    int   aerox, aeroy, aeroz;
    int   theta, phi;
    int   emisx, emisy, emisz;
    int   energy;
    int   pmomx, pmomy, pmomz;
    int   hpdqwx, hpdqwy, hpdqwz;
    int   mcrichhit;

  };

  // -----------------------------------------------------------------------

  static const CLID CLID_PackedMCRichOpticalPhotons = 1522;

  /// Namespace for locations in TDS
  namespace PackedMCRichOpticalPhotonLocation
  {
    static const std::string& Default = "pSim/Rich/OpticalPhotons";
  }

  /** @class PackedMCRichOpticalPhotons Event/PackedMCRichOpticalPhoton.h
   *
   *  Packed MCRichOpticalPhotons
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class PackedMCRichOpticalPhotons : public DataObject
  {
  public:

    /// Vector of packed objects
    typedef std::vector<LHCb::PackedMCRichOpticalPhoton> Vector;

  public:

    /// Standard constructor
    PackedMCRichOpticalPhotons( ) { }

    /// Destructor
    virtual ~PackedMCRichOpticalPhotons( ) { }

    /// Class ID
    static const CLID& classID() { return CLID_PackedMCRichOpticalPhotons; }

    /// Class ID
    virtual const CLID& clID() const { return PackedMCRichOpticalPhotons::classID(); }

  public:

    /// Write access to the data vector
    Vector & data()             { return m_vect; }

    /// Read access to the data vector
    const Vector & data() const { return m_vect; }

  private:

    Vector m_vect; ///< The packed data objects

  };

  // -----------------------------------------------------------------------

  /** @class MCRichOpticalPhotonPacker Event/PackedMCRichOpticalPhoton.h
   *
   *  Utility class to handle the packing and unpacking of the MCRichOpticalPhotons
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class MCRichOpticalPhotonPacker
  {
  public:

    // These are required by the templated algorithms
    typedef LHCb::MCRichOpticalPhoton                    Data;
    typedef LHCb::PackedMCRichOpticalPhoton        PackedData;
    typedef LHCb::MCRichOpticalPhotons             DataVector;
    typedef LHCb::PackedMCRichOpticalPhotons PackedDataVector;
    static const std::string& packedLocation()   { return LHCb::PackedMCRichOpticalPhotonLocation::Default; }
    static const std::string& unpackedLocation() { return LHCb::MCRichOpticalPhotonLocation::Default; }

  public:

    /// Default Constructor
    MCRichOpticalPhotonPacker() {}

  public:

    /// Pack an MCRichOpticalPhoton
    void pack( DataVector       & phots,
               PackedDataVector & pphots,
               const unsigned int version );

    /// Unpack an MCRichOpticalPhoton
    void unpack( PackedDataVector & pphots,
                 DataVector       & phots );

    /// Compare two MCRichHits to check the packing -> unpacking performance
    StatusCode check( const DataVector & dataA,
                      const DataVector & dataB,
                      GaudiAlgorithm & parent );

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

  private:
    
    /// Scale factor for photon energies
    static const double PhotEnScale = 5.0e8;

  };

  // -----------------------------------------------------------------------

}

#endif // EVENT_PACKEDMCRICHOPTICALPHOTON_H
