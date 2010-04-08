// $Id: TrackKalmanFilter.h,v 1.32 2010-04-08 11:46:07 wouter Exp $
#ifndef TRACKFITTER_TRACKKALMANFILTER_H 
#define TRACKFITTER_TRACKKALMANFILTER_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackKalmanFilter.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/FitNode.h"

// From LoKi
#include "GaudiKernel/Range.h"

/** @class TrackKalmanFilter TrackKalmanFilter.h
 *  
 *
 *  @author Jose Angel Hernando Morata, Eduardo Rodrigues
 *  @date   2005-04-15
 *  reusing the previous code
 *  @author Rutger van der Eijk  07-04-1999
 *  @author Mattiew Needham 
 */

class TrackKalmanFilter : public GaudiTool,
                          virtual public ITrackKalmanFilter {
public: 
  /// Standard constructor
  TrackKalmanFilter( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent );

  /// Destructor
  virtual ~TrackKalmanFilter( );

  StatusCode initialize();

  //! fit a track
  StatusCode fit( LHCb::Track& track ) const ;
  
protected:
  typedef std::vector<LHCb::Node*> NodeContainer ;
  typedef Gaudi::Range_<NodeContainer> NodeRange;

  // ! Fit a range of nodes
  StatusCode fit( LHCb::Track& track, NodeRange& nodes, const Gaudi::TrackSymMatrix& seedCov ) const ;

  //! predict the state at this node
  StatusCode predict( LHCb::FitNode& node, LHCb::State& state ) const;  

  //! predict the state at this node for the reverse fit
  StatusCode predictReverseFit( const LHCb::FitNode& prevNode, 
                                const LHCb::FitNode& aNode,
                                LHCb::State& aState ) const;

  //! filter this node
  StatusCode filter( LHCb::FitNode& node, LHCb::State& state ) const;
  
  //! smooth 2 nodes
  StatusCode smooth( LHCb::FitNode& node0, const LHCb::FitNode& node1 ) const;

  //! smoother for bidirectional fit nodes
  StatusCode biSmooth( LHCb::FitNode& node0 ) const;

  //! update the residual such that it corresponds the the current state in the node
  void updateResidual( LHCb::FitNode& node ) const ;

  // ! check that the contents of the cov matrix are fine
  bool isPositiveMatrix( const Gaudi::TrackSymMatrix& mat ) const;

private:

  // job options
  bool m_biDirectionalFit;          ///< Flag for bidirectional fit
  bool m_smooth;                    ///< Flag for smoothing
  
  //! helper to print a failure comment
  StatusCode failure( const std::string& comment ) const;

  unsigned int m_DoF;

  bool m_debugLevel;
};
#endif // TRACKFITTER_TRACKKALMANFILTER_H


//=========================================================================
// Helper to print a failure comment
//=========================================================================
inline StatusCode TrackKalmanFilter::failure( const std::string& comment ) const {
  if ( m_debugLevel )
    debug() << "TrackKalmanFilter failure: " << comment << endreq;
  return StatusCode::FAILURE;
}



//=========================================================================
// 
//=========================================================================
inline bool TrackKalmanFilter::isPositiveMatrix( const Gaudi::TrackSymMatrix& mat ) const 
{
  unsigned int i = 0u;
  for ( ; i < Gaudi::TrackSymMatrix::kRows && mat(i,i) > 0.0 ; ++i ) {}
  return i == Gaudi::TrackSymMatrix::kRows ;
}
