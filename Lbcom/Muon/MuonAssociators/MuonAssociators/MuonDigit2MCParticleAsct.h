// $Id: MuonDigit2MCParticleAsct.h,v 1.3 2003-01-17 14:14:54 sponce Exp $
#ifndef ASSOCIATORS_MuonDigit2MCParticleASCT_H 
#define ASSOCIATORS_MuonDigit2MCParticleASCT_H 1

// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
#include "Event/MuonDigit.h"
#include "Event/MCParticle.h"

// Associators
#include "Relations/Associator.h"
#include "Relations/Relation1D.h"

static const std::string& MuonDigit2MCParticleLocation = 
                   "Rec/Relations/MuonDigits2MCParticles";

/** @class MuonDigit2MCParticleAsct MuonDigit2MCParticleAsct.h 
 *         Associators/MuonDigit2MCParticleAsct.h
 *  
 *
 *  @author David Hutchcroft
 *  @date   12/06/2002
 */



class MuonDigit2MCParticleAsct: public Associator<MuonDigit,MCParticle>
{

  friend class ToolFactory<MuonDigit2MCParticleAsct>;
  
public:
  // Define data types
  typedef Relation1D<MuonDigit,MCParticle>          Table;
  typedef OwnType                                   Asct;
  typedef FromRange                                 Digits;
  typedef FromIterator                              DigitsIterator;
  typedef ToRange                                   MCParticles;
  typedef ToIterator                                MCParticlesIterator;
    
protected:

  /// Standard constructor
  MuonDigit2MCParticleAsct(const std::string& type,
                           const std::string& name,
                           const IInterface* parent ) 
  :Asct( type, name, parent) {
    setProperty( "Location", MuonDigit2MCParticleLocation );
    setProperty( "AlgorithmType", "MuonDigit2MCParticleAlg" );
    setProperty( "AlgorithmName", "MuonDigit2MCParticle" );
  }; 

  /// Destructor
  virtual ~MuonDigit2MCParticleAsct() {}; ///< Destructor

private:

};
#endif // ASSOCIATORS_MuonDigit2MCParticleASCT_H
