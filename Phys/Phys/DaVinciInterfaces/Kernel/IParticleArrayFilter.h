// $Id: IParticleArrayFilter.h,v 1.2 2009-08-04 09:45:31 jpalac Exp $
#ifndef KERNEL_IPARTICLEARRAYFILTER_H
#define KERNEL_IPARTICLEARRAYFILTER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from Event
#include "Event/Particle.h"

/** @class IParticleArrayFilter IParticleArrayFilter.h Kernel/IParticleArrayFilter.h
 *
 *  Apply a filter to an array of Particles and either remove those that
 *  do not pass from the array or fill a new one with those that pass.
 *  @author Juan Palacios
 *  @date   2007-07-24
 */
class GAUDI_API IParticleArrayFilter : virtual public IAlgTool
{

 public:

  DeclareInterfaceID(IParticleArrayFilter, 2, 0);

 public:

  /// Filter and put results into new array
  virtual StatusCode filter( const LHCb::Particle::ConstVector&,
                             LHCb::Particle::ConstVector& ) const = 0;
  /// Filter and remove elements that do not pass filter from array
  virtual StatusCode filter( LHCb::Particle::ConstVector& ) const = 0;

};

#endif // KERNEL_IPARTICLEARRAYFILTER_H
