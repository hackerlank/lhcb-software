// $Id: IProductionTool.h,v 1.7 2007-03-08 13:40:09 robbep Exp $
#ifndef GENERATORS_IPRODUCTIONTOOL_H 
#define GENERATORS_IPRODUCTIONTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace HepMC { class GenEvent ; }
namespace LHCb { class GenCollision ; 
  class ParticleProperty ;
}



/** @class IProductionTool IProductionTool.h "Generators/IProductionTool.h"
 *  
 *  Abstract interface to production generators. Concrete tools are 
 *  interface to production generators (Pythia, HERWIG, ...) 
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-16
 */

static const InterfaceID IID_IProductionTool( "IProductionTool" , 4 , 0 ) ;

class IProductionTool : virtual public IAlgTool {
public:
  static const InterfaceID& interfaceID() { return IID_IProductionTool ; }
  
  /** Generate a primary interaction. 
   *  The generated event contains all what happens up to the decay of
   *  hadrons. Decay of hadrons will be performed in general by the decay
   *  tool. Then all hadrons must be declared stable in the production
   *  generator. This is done thanks to IProductionTool::setStable.
   *  @param[out] theEvent  Event generated by the production generator.
   *  @param[out] theInfo   Informations about the hard process of the
   *                        generated interaction.
   */
  virtual StatusCode generateEvent( HepMC::GenEvent * theEvent , 
                                    LHCb::GenCollision * theInfo ) = 0 ;

  /// Declare a particle stable to the production generator.
  virtual void setStable( const LHCb::ParticleProperty * thePP ) = 0 ;

  /** Update the properties of the particle in the production generator
   *  with values from the particle property service.
   */
  virtual void updateParticleProperties( const LHCb::ParticleProperty * thePP ) = 0 ;

  /// Turn off the fragmentation step in the generation.
  virtual void turnOffFragmentation( ) = 0 ;

  /// Turn on the fragmentation step in the generation.
  virtual void turnOnFragmentation( )  = 0 ;

  /** Hadronize the event.
   *  @param[in,out] theEvent  Unfragmented event to hadronize.
   *  @param[out]    theInfo   Informations about the hard process of the 
   *                           generated interaction.
   */
  virtual StatusCode hadronize( HepMC::GenEvent * theEvent , 
                                LHCb::GenCollision * theInfo ) = 0 ;

  /// Save the parton level event (when the fragmentation is turned off)
  virtual void savePartonEvent( HepMC::GenEvent * theEvent ) = 0 ;

  /// Retrieve the previously saved parton event to re-hadronize it.
  virtual void retrievePartonEvent( HepMC::GenEvent * theEvent ) = 0 ;

  /// Print configuration of production generator 
  virtual void printRunningConditions( ) = 0 ;

  /** Define special particles whose properties must not be updated from 
   *  the particle property service (like mass of top quark, ...)
   */
  virtual bool isSpecialParticle( const LHCb::ParticleProperty * thePP ) const = 0 ;

  /// Prepare the generator to force fragmentationto in the thePdgId flavour.
  virtual StatusCode setupForcedFragmentation( const int thePdgId ) = 0 ;

  /// Specific initialization of the Generator
  virtual StatusCode initializeGenerator( ) = 0 ;
};
#endif // GENERATORS_IPRODUCTIONTOOL_H
