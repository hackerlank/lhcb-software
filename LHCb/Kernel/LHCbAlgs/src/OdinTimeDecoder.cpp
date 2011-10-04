// Include files

// from Gaudi
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ToolFactory.h"

// data model
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/ODIN.h"

// detector description
#include "DetDesc/RunChangeIncident.h"

// local
#include "OdinTimeDecoder.h"

#ifdef _WIN32
#pragma warning(disable : 4355) // 'this' used in base member initializer list
#endif

//-----------------------------------------------------------------------------
// Implementation file for class : OdinTimeDecoder
//
// 2006-09-21 : Marco Clemencic
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( OdinTimeDecoder )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OdinTimeDecoder::OdinTimeDecoder( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_odinDecoder("ODINDecodeTool",this),
    m_currentRun(0)
{
  declareInterface<IEventTimeDecoder>(this);
}

//=============================================================================
// Destructor
//=============================================================================
OdinTimeDecoder::~OdinTimeDecoder() {}

//=============================================================================
// Initialize
//=============================================================================
StatusCode OdinTimeDecoder::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  // try to get the tool to decode the ODIN bank
  return m_odinDecoder.retrieve();
}

//=========================================================================
//  Get the ODIN object from T.E.S.
//=========================================================================
LHCb::ODIN *OdinTimeDecoder::getODIN() const
{
  // Decode the ODIN bank.
  m_odinDecoder->execute();
  // @FIXME: we must get the ODIN object from where the Tool created it
  if ( exist<LHCb::ODIN>(LHCb::ODINLocation::Default) ){
    return get<LHCb::ODIN>(LHCb::ODINLocation::Default);
  }
  return 0;
}

//=========================================================================
//  Return the time of current event
//=========================================================================
Gaudi::Time OdinTimeDecoder::getTime ( ) const {

  // Do not try to call the ODIN Decoder if we are not in RUNNING state.
  /// @todo use some better condition (e.g. the global state)
  if (UNLIKELY(FSMState() != Gaudi::StateMachine::RUNNING))
    return Gaudi::Time::epoch();

  static Gaudi::Time last_time(0);

  LHCb::ODIN *odin = getODIN();

  if (odin) {
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "GPS Time = " << odin->gpsTime() << endmsg;

    // Check the run number in ODIN
    if (m_currentRun != odin->runNumber()) {
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug() << "Firing " << IncidentType::RunChange << " incident. Old run="
                << m_currentRun;
      m_currentRun = odin->runNumber();
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug() << ", new run=" << m_currentRun << endmsg;
      incSvc()->fireIncident(RunChangeIncident(name(),m_currentRun));
    }

    last_time = odin->eventTime();
  }

  return last_time;
}
//=============================================================================
