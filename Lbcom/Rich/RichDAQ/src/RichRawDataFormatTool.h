
//-----------------------------------------------------------------------------
/** @file RichRawDataFormatTool.h
 *
 *  Header file for tool : RichRawDataFormatTool
 *
 *  CVS Log :-
 *  $Id: RichRawDataFormatTool.h,v 1.10 2005-12-16 15:11:34 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RICHRAWDATAFORMATTOOL_H
#define RICHDAQ_RICHRAWDATAFORMATTOOL_H 1

// Boost
#include "boost/lexical_cast.hpp"

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// Base class
#include "RichKernel/RichToolBase.h"

// Kernel
#include "RichKernel/RichStatDivFunctor.h"

// Interfaces
#include "RichKernel/IRichRawDataFormatTool.h"
#include "RichKernel/IRichDetNumberingTool.h"

// local
#include "RichNonZeroSuppData.h"
#include "RichZeroSuppData.h"

// RichKernel
#include "RichKernel/RichMap.h"

// Event model
#include "Event/RawEvent.h"

// namespaces
using namespace LHCb; ///< LHCb general namespace

//-----------------------------------------------------------------------------
/** @class RichRawDataFormatTool RichRawDataFormatTool.h
 *
 *  Tool to encode and decode the Raw Event information for the RICH.
 *
 *  The following versions are included :-
 *
 *   Version 0 : DC04 compatibile. Can only decode this version, not encode
 *
 *   Version 1 : Same HPD bank format as version 0, but corrected header word bug
 *               plus incorporate Level 1 board groupings
 *
 *   Version 2 : Same as 1 but using a new zero suppressed HPD bank format
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 *
 *  @todo Remove DC04 hacks when no longer needed
 */
//-----------------------------------------------------------------------------

class RichRawDataFormatTool : public RichToolBase,
                              virtual public IRichRawDataFormatTool,
                              virtual public IIncidentListener {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichRawDataFormatTool( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// Destructor
  virtual ~RichRawDataFormatTool( );

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

  // Creates a bank data of a given version from the given RichSmartID vector
  const RichHPDDataBank * createDataBank( const RichSmartID::Vector & smartIDs,
                                          const RichDAQ::BankVersion version = RichDAQ::LHCb0 ) const;

  // Creates a bank data from the given raw block of data
  const RichHPDDataBank * createDataBank( const RichDAQ::LongType * dataStart,
                                          const unsigned int dataSize,
                                          const RichDAQ::BankVersion version = RichDAQ::LHCb0 ) const;

  // Creates a bank data from RichSmartIDs, sorted by Level 1 ID
  void createDataBank( const RichDAQ::L1Map & L1Data,
                       const RichDAQ::BankVersion version = RichDAQ::LHCb0 ) const;

  // Decode a RawBank into RichSmartID identifiers
  void decodeToSmartIDs( const RawBank & bank,
                         RichSmartID::Vector & smartIDs ) const;

  // Decode all RICH RawBanks into RichSmartID identifiers
  void decodeToSmartIDs( RichSmartID::Vector & smartIDs ) const;

private: // definitions

  // Summary data
  typedef std::pair< const RichDAQ::BankVersion, RichDAQ::Level1ID >            L1IDandV;
  typedef std::pair< unsigned long, std::pair< unsigned long, unsigned long > > L1CountAndSize;
  typedef RichMap< const L1IDandV, L1CountAndSize > L1TypeCount;

private: // methods

  /// Initialise for each event
  void InitEvent();

  /// Finalise for each event
  void FinishEvent();

  /// Retrieves the raw event.
  RawEvent * rawEvent() const;

  /** Final printout of Level 1 stats
   *
   *  @param count The Summary object to printout
   *  @param title The title to use
   */
  void printL1Stats( const L1TypeCount & count, const std::string & title ) const;

private: // data

  /// Pointer to Rich HPD ID tool
  const IRichDetNumberingTool * m_detNumTool;

  /// Pointer to Raw Event
  mutable RawEvent * m_rawEvent;

  /// Input location for RawEvent in TES
  std::string m_rawEventLoc;

  /// The number of hits marking the transistion between zero and non-zero suppressed data
  /// Used by version 0 of the data banks
  unsigned int m_zeroSuppresCut;

  /// Flag to turn on and off the summary information
  bool m_summary;

  mutable L1TypeCount m_l1decodeSummary; ///< Summary object for decoding
  mutable L1TypeCount m_l1encodeSummary; ///< Summary object for encoding

  /// Number of events processed
  mutable unsigned int m_evtCount;

  /// Flag to indicate if the tool has been used in a given event
  mutable bool m_hasBeenCalled;

};

inline void RichRawDataFormatTool::InitEvent()
{
  m_rawEvent = 0;
  m_hasBeenCalled = false;
}

inline void RichRawDataFormatTool::FinishEvent()
{
  if ( m_hasBeenCalled ) ++m_evtCount;
}

#endif // RICHDAQ_RICHRAWDATAFORMATTOOL_H
