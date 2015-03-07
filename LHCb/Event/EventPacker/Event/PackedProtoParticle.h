// $Id: PackedProtoParticle.h,v 1.5 2009-11-10 10:24:09 jonrob Exp $
#ifndef EVENT_PACKEDPROTOPARTICLE_H 
#define EVENT_PACKEDPROTOPARTICLE_H 1

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StatusCode.h"

#include <string>
#include <vector>

// Kernel
#include "Event/StandardPacker.h"

// Event
#include "Event/ProtoParticle.h"

class GaudiAlgorithm;

namespace LHCb
{

  /** @class PackedProtoParticle PackedProtoParticle.h Event/PackedProtoParticle.h
   *  This is the packed version of the ProtoParticle
   *
   *  @author Olivier Callot
   *  @date   2008-11-13
   */
  struct PackedProtoParticle 
  {

    /// Standard constructor
    PackedProtoParticle( ) :
      key(0), 
      track(0), richPID(0), muonPID(0),
      firstHypo(0), lastHypo(0),
      firstExtra(0), lastExtra(0)
    {}

    /// Copy Constructor
    PackedProtoParticle( const PackedProtoParticle& c ) :
      key( c.key ), 
      track( c.track ), richPID( c.richPID ), muonPID( c.muonPID ),
      firstHypo( c.firstHypo ),  lastHypo( c.lastHypo  ),
      firstExtra( c.firstExtra ), lastExtra( c.lastExtra )
    {}

    long long key;
    int track;
    int richPID;
    int muonPID;
    unsigned short int firstHypo;
    unsigned short int lastHypo;
    unsigned short int firstExtra;
    unsigned short int lastExtra;

  };
  
  static const CLID CLID_PackedProtoParticles = 1552;

  // Namespace for locations in TDS
  namespace PackedProtoParticleLocation
  {
    static const std::string& Charged  =  "pRec/ProtoP/Charged";
    static const std::string& Neutral  =  "pRec/Protop/Neutral";
    static const std::string& InStream = "/pRec/ProtoP/Custom";
  }

  class PackedProtoParticles : public DataObject 
  {

  public:
    
    /// Default Packing Version
    static char defaultPackingVersion() { return 0; }
    
  public: 
  
    /// Standard constructor
    PackedProtoParticles( ) : m_packingVersion(defaultPackingVersion()) { }

    virtual ~PackedProtoParticles( ) {}; ///< Destructor

    virtual const CLID& clID()  const { return PackedProtoParticles::classID(); }
    static  const CLID& classID()     { return CLID_PackedProtoParticles;       }

  public:

    std::vector<PackedProtoParticle>& protos()             { return m_vect; }
    const std::vector<PackedProtoParticle>& protos() const { return m_vect; }

    std::vector<int>& refs()                               { return m_refs; }
    const std::vector<int>& refs() const                   { return m_refs; }
 
    std::vector<std::pair<int,int> >& extras()             { return m_extra; }
    const std::vector<std::pair<int,int> >& extras() const { return m_extra; }

  public:

    /// Set the packing version
    void setPackingVersion( const char ver ) { m_packingVersion = ver; }

    /// Access the packing version
    char packingVersion() const { return m_packingVersion; }

  private:

    std::vector<PackedProtoParticle> m_vect;
    std::vector<int>                 m_refs;
    std::vector<std::pair<int,int> > m_extra;

    /// Data packing version
    char m_packingVersion;

  };

  /** @class ProtoParticlePacker Event/PackedProtoParticle.h
   *
   *  Utility class to handle the packing and unpacking of ProtoParticles
   *
   *  @author Christopher Rob Jones
   *  @date   05/04/2012
   */
  class ProtoParticlePacker
  {

  public:

    typedef LHCb::ProtoParticle                    Data;
    typedef LHCb::PackedProtoParticle        PackedData;
    typedef LHCb::ProtoParticles             DataVector;
    typedef LHCb::PackedProtoParticles PackedDataVector;

  private:

    /// Default Constructor hidden
    ProtoParticlePacker() : m_parent(NULL) {}

  public:

    /// Default Constructor
    ProtoParticlePacker( GaudiAlgorithm & parent ) : m_parent(&parent) {}

  public:

    /// Pack a ProtoParticle
    void pack( const Data & proto,
               PackedData & pproto,
               PackedDataVector & pprotos ) const;

    /// Pack ProtoParticles
    void pack( const DataVector & protos,
               PackedDataVector & pprotos ) const;

    /// Unpack a single ProtoParticle
    void unpack( const PackedData       & pproto,
                 Data                   & proto,
                 const PackedDataVector & pprotos,
                 DataVector             & protos ) const;

    /// Unpack ProtoParticles
    void unpack( const PackedDataVector & pprotos,
                 DataVector             & protos ) const;

    /// Compare two ProtoParticle containers to check the packing -> unpacking performance
    StatusCode check( const DataVector & dataA,
                      const DataVector & dataB ) const;

    /// Compare two ProtoParticles to check the packing -> unpacking performance
    StatusCode check( const Data & dataA,
                      const Data & dataB ) const;

  private:

    /// Access the parent algorithm
    GaudiAlgorithm& parent() const { return * m_parent; }

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

    /// Pointer to parent algorithm
    GaudiAlgorithm * m_parent;

  };

}

#endif // EVENT_PACKEDPROTOPARTICLE_H
