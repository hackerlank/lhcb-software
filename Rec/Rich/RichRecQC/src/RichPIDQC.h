
//-----------------------------------------------------------------------------
/** @file RichPIDQC.h
 *
 *  Header file for RICH reconstruction monitoring algorithm : RichPIDQC
 *
 *  CVS Log :-
 *  $Id: RichPIDQC.h,v 1.20 2005-04-15 16:33:32 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2002-06-13
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECQC_RICHPIDQC_H
#define RICHRECQC_RICHPIDQC_H 1

// base class
#include "RichKernel/RichAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// Relations
#include "Relations/IAssociatorWeighted.h"
#include "Relations/IRelationWeighted.h"

// RichRecBase
#include "RichRecBase/RichTrackSelector.h"

// Event model
#include "Event/RichPID.h"
#include "Event/TrStoredTrack.h"
#include "Event/MCParticle.h"

// RichKernel
#include "RichKernel/RichMap.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// interfaces
#include "RichKernel/IRichMCTruthTool.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// Boost
#include "boost/lexical_cast.hpp"

//-----------------------------------------------------------------------------
/** @class RichPIDQC RichPIDQC.h
 *
 *  Quality control monitor for RichPIDs
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-06-13
 *
 *  @todo Review and perhaps rewrite this monitor... Its getting a little messy.
 */
//-----------------------------------------------------------------------------

class RichPIDQC : public RichAlgBase {

public:

  /// Standard constructor
  RichPIDQC( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichPIDQC( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // definitions

  /// tracking MC truth definition
  typedef IAssociatorWeighted<TrStoredTrack,MCParticle,double> TrackFitAsct;

private: // methods

  /// Loads the PID data from configured location
  StatusCode loadPIDData();

  /// Book histograms
  StatusCode bookHistograms();

  /// Book MCTruth histograms
  StatusCode bookMCHistograms();

  /** Perform C++ gymnastics to get momentum state for a given track
   *
   *  @return Pointer to TrStateP at the requested z position
   */
  const TrStateP * getTrStateP ( const TrStoredTrack * track, ///< Pointer to a TrStoredTrack
                                 const double zPos = -999999  ///< z position at which to get the state
                                 ) const;

  /// Count all TrStoredTracks in given location passing the selection criteria
  void countTrStoredTracks( const std::string & location );

  /// Count all TrgTracks in given location passing the selection criteria
  void countTrgTracks( const std::string & location );

  /// Does this RichPID object pass the selection criteria
  bool pidIsSelected( const RichPID * pid ) const;

  /// Get the momentum for the track associated to the PID object
  double momentum( const RichPID * pid ) const;

  /// Get the track type associated to the PID object
  Rich::Track::Type trackType( const RichPID * pid ) const;

  /// Get the MC type for the track associated to the PID object
  Rich::ParticleIDType trueMCType( const RichPID * pid ) const;

  /// Get the associated track key
  int tkNumber( const RichPID * pid ) const;

private: // data

  // Vector of pointers to RichPIDs
  std::vector<ContainedObject*> m_richPIDs;

  // job options
  std::string m_mcHstPth;        ///< Output MC truth histogram path
  std::string m_hstPth;          ///< Output histogram path
  std::string m_pidTDS;          ///< Location of target RichPIDs in TDS
  double m_pMinCut;              ///< minimum momentum cut
  double m_pMaxCut;              ///< maximum momentum cut
  int m_minMultCut;              ///< Minimum track multiplicity
  int m_maxMultCut;              ///< Maximum track multiplicity
  bool m_truth;                  ///< MCTruth available
  bool m_doHistos;               ///< Flag controlling the creation of histograms
  int m_bins;                    ///< Number of bins
  bool m_finalPrintOut;          ///< Perform final prinout of PID tables
  bool m_extraHistos;            ///< Fill full set of histograms
  bool m_ignoreThres; ///< Flag to turn on/off the setting of PIDs as "below threshold"

  RichTrackSelector m_trSelector;  ///< Track selector

  IRichMCTruthTool * m_mcTruth;    ///< MC Truth tool

  // Summary information
  double m_sumTab[6][6];
  int m_nEvents[2];
  int m_nTracks[2];

  int m_multiplicity;
  int m_totalSelTracks;

  typedef RichMap<Rich::Track::Type,std::pair<unsigned int,unsigned int> > TkCount;
  /// Count the number of PID objects by track type
  TkCount m_trackCount;

  typedef RichMap<std::string,std::pair<unsigned int,unsigned int> > PIDsByType;
  /// Count the number of PID objects by PID type
  PIDsByType m_pidPerTypeCount;

  float m_dllKaonCut;
  float m_dllPionCut;

  // Histograms

  IHistogram1D* m_ids;           ///< Particle identifications per mass hypothesis
  IHistogram1D* m_Nids;          ///< Number of particle identifications per event

  IHistogram1D* m_pRaw[Rich::NParticleTypes];     ///< Particle ID raw probability
  IHistogram1D* m_pNorm[Rich::NParticleTypes];    ///< Particle ID normalised probability
  IHistogram1D* m_deltaLL[Rich::NParticleTypes];  ///< Delta Log-Likelihood
  IHistogram1D* m_deltaLLTrue[Rich::NParticleTypes];  ///< Delta Log-Likelihood true particle hypothesis
  IHistogram1D* m_deltaLLFake[Rich::NParticleTypes];  ///< Delta Log-Likelihood fakeparticle hypothesis

  /// dll between types : True type
  IHistogram1D* m_dLLTrue[Rich::NParticleTypes][Rich::NParticleTypes];
  /// dll between types : Fake type
  IHistogram1D* m_dLLFalse[Rich::NParticleTypes][Rich::NParticleTypes];

  /// nSigma v ptot between types : True type
  IHistogram2D* m_nsigvpTrue[Rich::NParticleTypes][Rich::NParticleTypes];
  /// nsigma v ptot between types : Fake type
  IHistogram2D* m_nsigvpFalse[Rich::NParticleTypes][Rich::NParticleTypes];

  /// Momentum spectrum for IDed type versus true type
  IHistogram1D* m_ptotSpec[Rich::NParticleTypes+1][Rich::NParticleTypes+1];

  IHistogram2D* m_perfTable;        ///< Overall PID performance table

  IHistogram1D* m_pidRate;          ///< Fraction of selected tracks with PID results
  IHistogram1D* m_eventRate;        ///< Events with/without PID results

};

inline const TrStateP * RichPIDQC::getTrStateP( const TrStoredTrack * track,
                                                const double zPos ) const
{
  return ( track ? dynamic_cast<const TrStateP*>((const TrState*)track->closestState(zPos)) : 0 );
}

#endif // RICHRECQC_RICHPIDQC_H
