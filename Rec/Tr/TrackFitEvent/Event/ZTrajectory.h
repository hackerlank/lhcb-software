// $Id: ZTrajectory.h,v 1.2 2007-12-10 08:20:46 wouter Exp $
#ifndef EVENT_ZTRAJECTORY_H 
#define EVENT_ZTRAJECTORY_H 1

// Include files
#include "Kernel/Trajectory.h"
#include "Event/StateVector.h"
#include "Event/State.h"

/** @class ZTrajectory ZTrajectory.h Event/ZTrajectory.h
 *  Interface for trajectories parameterized along Z. Gives access to statevectors.
 *
 *  @author Wouter HULSBERGEN
 *  @date   2007-11-29
 */

namespace LHCb
{
  
  class ZTrajectory : public Trajectory 
  {
  public:
    /// Default constructor
    ZTrajectory() : Trajectory(0,0) {}
    /// Constructor taking the values of mu that defined the valid range of the trajectory
    ZTrajectory( double begin, double end ): Trajectory(begin,end) {}
    /// Constructor taking a range
    ZTrajectory( const Range& range ):  Trajectory(range) {}
    /// Desctructor
    virtual ~ZTrajectory( ) {} 
    /// return stateVector at position mu
    virtual StateVector stateVector( double mu ) const = 0 ;
    /// return a state at position mu
    virtual State state( double mu ) const { return State(stateVector(mu)) ; }
    /// return the set of reference statevectors for this parameterization (if any)
    virtual std::vector<StateVector> refStateVectors() const { return std::vector<StateVector>() ; }
  };
  
}

#endif // EVENT_ZTRAJECTORY_H
