
//-----------------------------------------------------------------------------
/** @file RichMCTruthTool.h
 *
 *  Header file for tool : RichMCTruthTool
 *
 *  CVS Log :-
 *  $Id: RichMCTruthTool.h,v 1.21 2006-01-19 08:42:22 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHMCTOOLS_RICHMCTRUTHTOOL_H
#define RICHMCTOOLS_RICHMCTRUTHTOOL_H 1

// from Gaudi
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichKernel/RichToolBase.h"

// Relations
#include "Relations/IAssociatorWeighted.h"
#include "Relations/IRelationWeighted.h"

// RichKernel
#include "RichKernel/RichMap.h"

// Event model
#include "Kernel/RichParticleIDType.h"
#include "Event/MCParticle.h"
#include "Event/MCTruth.h"
#include "Event/MCRichDigit.h"
#include "Event/MCRichDigitSummary.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichSegment.h"
#include "Event/MCRichTrack.h"
#include "Event/MCRichHit.h"

// Linkers
#include "Linker/LinkedTo.h"

// Interfaces
#include "RichKernel/IRichMCTruthTool.h"

// Constants
#include "Kernel/PhysicalConstants.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------
/** @class RichMCTruthTool RichMCTruthTool.h
 *
 *  Tool performing MC truth associations
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 *
 *  @todo Major tidy up once only one track type and track MC association type ...
 */
//-----------------------------------------------------------------------------

class RichMCTruthTool : public RichToolBase,
                        virtual public IRichMCTruthTool,
                        virtual public IIncidentListener {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichMCTruthTool( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );

  // Initialization of the tool after creation
  StatusCode initialize();

  // Finalization of the tool before deletion
  StatusCode finalize();

  /** Implement the handle method for the Incident service.
   *  This is used to inform the tool of software incidents.
   *
   *  @param incident The incident identifier
   */
  void handle( const Incident& incident );

public: // methods (and doxygen comments) inherited from interface

  // Get a vector of MCParticles associated to given RichSmartID
  bool mcParticles( const RichSmartID id,
                    std::vector<const MCParticle*> & mcParts ) const;

  // Determines the particle mass hypothesis for a given MCParticle
  Rich::ParticleIDType mcParticleType( const MCParticle * mcPart ) const;

  // Finds the MCRichDigit association for a RichSmartID channel identifier
  const MCRichDigit * mcRichDigit( const RichSmartID id ) const;

  // Finds the MCRichTrack associated to a given MCParticle
  const MCRichTrack * mcRichTrack( const MCParticle * mcPart ) const;

  // Finds the MCRichOpticalPhoton associated to a given MCRichHit
  const MCRichOpticalPhoton * mcOpticalPhoton( const MCRichHit * mcHit ) const;

  // Access the bit-pack history object for the given RichSmartID
  bool getMcHistories( const RichSmartID id,
                       std::vector<MCRichDigitSummary*> & histories ) const;

  // Checks if the given RichSmartID is the result of a background hit
  bool isBackground ( const RichSmartID id ) const;

  // Checks if the given MCRichDigit is the result of a background hit
  bool isBackground ( const MCRichDigit * digit ) const;

  // Checks if the given MCRichHit is the result of a background hit
  bool isBackground( const MCRichHit * hit ) const;

  // Checks if the given RichSmartID is the result of true Cherenkov
  bool isCherenkovRadiation( const RichSmartID id,
                             const Rich::RadiatorType rad ) const;

private: // definitions

  /// typedef of the Linker object for MCParticles to MCRichTracks
  typedef LinkedTo<MCRichTrack,MCParticle> MCPartToRichTracks;

  /// typedef of the Linker object for MCRichHits to MCRichOpticalPhotons
  typedef LinkedTo<MCRichOpticalPhoton,MCRichHit> MCRichHitToPhoton;

  /// Typedef for vector of pointers to MCRichDigitSummaries
  typedef std::vector<MCRichDigitSummary*> MCRichDigitSummaries;

  /// Typedef for map between RichSmartIDs and MCRichDigitSummary objects
  typedef RichMap< const RichSmartID, MCRichDigitSummaries > RichSummaryMap;

private: // private methods

  /// Returns the linker object for MCParticles to MCRichTracks
  MCPartToRichTracks * mcTrackLinks() const;

  /// Returns the linker object for MCRichHits to MCRichOpticalPhotons
  MCRichHitToPhoton * mcPhotonLinks() const;

  /// clean up current linker objects
  void cleanUpLinkers();

  /** Loads the MCRichDigits from the TES
   *
   * @return Pointer to the MCRichDigits
   * @retval NULL means no MC information is available
   */
  const MCRichDigits * mcRichDigits() const;

  /** Loads the MCRichDigitSummarys from TES
   *
   * @return Pointer to the MCRichDigitSummaryVector
   * @retval NULL means no MC information is available
   */
  const MCRichDigitSummarys * mcRichDigitSummaries() const;

  /** Loads the MCRichHits from the TES
   *
   * @return Pointer to the MCRichHits
   * @retval NULL means no MC information is available
   */
  const MCRichHits * mcRichHits() const;

  /// Initialise for a new event
  void InitNewEvent();

  /// Access the map between RichSmartIDs and MCRichDigitSummaries
  const RichSummaryMap & summaryMap() const;

private: // private data

  /// Flag to say MCRichDigits have been loaded for this event
  mutable bool m_mcRichDigitsDone;

  /// Flag to say MCRichDigitSummarys has been loaded for this event
  mutable bool m_mcRichDigitSumsDone;

  /// Flag to say mapping between RichSmartIDs and MCRichDigitSummary objects
  /// has been created for this event
  mutable bool m_summaryMapDone;

  /// Flag to say MCRichHits have been loaded for this event
  mutable bool m_mcRichHitsDone;

  /// Pointer to MCRichDigits
  mutable MCRichDigits * m_mcRichDigits;

  /// Pointer to MCRichDigitSummarys
  mutable MCRichDigitSummarys * m_mcRichDigitSums;

  /// Pointer to MCRichDigits
  mutable MCRichHits * m_mcRichHits;

  /// Linker for MCParticles to MCRichTracks
  mutable MCPartToRichTracks * m_mcTrackLinks;

  /// Linker for MCRichHits to MCRichOpticalPhotons
  mutable MCRichHitToPhoton * m_mcPhotonLinks;

  /// Location of MCRichDigits in EDS
  std::string m_mcRichDigitsLocation;

  /// Location of MCRichDigitSummarys in EDS
  std::string m_mcRichDigitSumsLocation;

  /// Location of MCRichHits in EDS
  std::string m_mcRichHitsLocation;

  /// PID information
  mutable RichMap<int,Rich::ParticleIDType> m_localID;

  /// Map between RichSmartIDs and MCRichDigitSummary objects
  mutable RichSummaryMap m_summaryMap;

  /// Flag to turn of the following of the chain RichSmartID -> MCRichDigit -> MCRichHit
  bool m_followMC;

};

inline void RichMCTruthTool::cleanUpLinkers()
{
  if ( m_mcTrackLinks         ) { delete m_mcTrackLinks;         m_mcTrackLinks         = 0; }
  if ( m_mcPhotonLinks        ) { delete m_mcPhotonLinks;        m_mcPhotonLinks        = 0; }
}

inline void RichMCTruthTool::InitNewEvent()
{
  m_mcRichDigitsDone       = false;
  m_mcRichDigitSumsDone    = false;
  m_mcRichHitsDone         = false;
  m_summaryMapDone         = false;
  cleanUpLinkers();
}

#endif // RICHMCTOOLS_RICHMCTRUTHTOOL_H
