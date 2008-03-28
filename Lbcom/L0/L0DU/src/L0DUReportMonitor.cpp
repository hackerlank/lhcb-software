// $Id: L0DUReportMonitor.cpp,v 1.7 2008-03-28 16:53:11 odescham Exp $
// Include files 
#include <cmath>
// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram1D.h"
// local
#include "L0DUReportMonitor.h"
// local
//-----------------------------------------------------------------------------
// Implementation file for class : L0DUReportMonitor
//
// 2006-02-16 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0DUReportMonitor );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DUReportMonitor::L0DUReportMonitor( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator ),
    m_chanCntMap(), 
    m_condCntMap(),
    m_chanRateMap(),
    m_condRateMap(),
    m_evtCntMap(),
    m_decCntMap()  
{
  declareProperty( "ReportLocation"    , m_reportLocation =  LHCb::L0DUReportLocation::Default );
  declareProperty( "DataMonitor"       , m_data = false );
  declareProperty( "ConditionsMonitor" , m_cond = false );
  declareProperty( "ChannelsMonitor"   , m_chan = false );
  declareProperty( "DecisionMonitor"   , m_dec  = true );
  declareProperty( "DataReBinFactor"   , m_bin = 1);
  declareProperty( "SplitConfig"       , m_split = true);

  setHistoDir( name );
}
//=============================================================================
// Destructor
//=============================================================================
L0DUReportMonitor::~L0DUReportMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0DUReportMonitor::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  debug() << "==> Initialize" << endmsg;

  m_prevTCK   = -1;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0DUReportMonitor::execute() {

  debug() << "==> Execute" << endmsg;
  

  if( !exist<LHCb::L0DUReport>( m_reportLocation )){
    Error("L0DUReport not found at location " + m_reportLocation ).ignore();
    return StatusCode::SUCCESS;
  }

  LHCb::L0DUReport* report   = get<LHCb::L0DUReport>( m_reportLocation );
  LHCb::L0DUConfig* config   = report->configuration();
  unsigned int tck           = report->tck();
  std::stringstream ttck("");
  ttck << format("0x%04X", tck) ;


  if(config == NULL){
    Error("L0DUConfig for tck = " + ttck.str() + " has not been registered -> cannot monitor the report").ignore();
    return StatusCode::SUCCESS;
  }
  

  // Initialisation
  bool init = false;
  if((int) tck != m_prevTCK ){
    m_prevTCK = (int) tck;
    // is this tck known ?
    std::map<unsigned int,double>::iterator it = m_evtCntMap.find(tck);
    if( it == m_evtCntMap.end() ){
      m_configs[tck]=config;
      m_evtCnt = 0.;
      m_decCnt = 0.;
      init = true;
    }
    else{  
      m_chanCnt     = m_chanCntMap[tck];
      m_condCnt     = m_condCntMap[tck];
      m_chanRate    = m_chanRateMap[tck];
      m_chanRelRate = m_chanRelRateMap[tck];
      m_condRate    = m_condRateMap[tck];
      m_evtCnt      = m_evtCntMap[tck];
      m_decCnt      = m_decCntMap[tck];
    }
    if(m_prevTCK != -1){
      if( !m_split){
        Warning("New configuration tck found : reset all histos").ignore();
        debug() << "New configuration : " << tck << " (previous was : " << m_prevTCK << ")" <<endreq;
        debug() << "Reset all histograms (number of histos : " << totalNumberOfHistos() << ")" << endreq;
        // reset all 1D histos
        const Histo1DMapTitle& h1d = histo1DMapTitle();
        for(Histo1DMapTitle::const_iterator id = h1d.begin() ; id != h1d.end() ; id++){
          debug() << "Reset 1D histo [title = '" << (*id).first << "'," << (*id).second<<"]" << endreq;
          if(NULL != (*id).second)(*id).second->reset();
        }
        // reset all 2D histos
        const Histo2DMapTitle& h2d = histo2DMapTitle();
        for(Histo2DMapTitle::const_iterator id = h2d.begin() ; id != h2d.end() ; id++){
          debug() << "Reset 2D histo [title = '" << (*id).first << "'," << (*id).second<<"]" << endreq;
          if(NULL != (*id).second)(*id).second->reset(); 
        }
      } 
    } 
  }
  LHCb::L0DUChannel::Map channels = config->channels();
  LHCb::L0DUElementaryCondition::Map conditions = config->conditions();
  

  
  m_evtCnt += 1.;  // event counter
  if(report->decision())m_decCnt += 1.; // decision counter
  std::stringstream chanName("[");
  std::stringstream condName("[");
  m_chanSeq[ "Global" ] = 0;
  for(LHCb::L0DUChannel::Map::iterator it = channels.begin();it!=channels.end();it++){
    if( init )m_chanCnt[ (*it).first ]=0;//init    
    if(report->channelDecision( ((*it).second)->id() )) m_chanCnt[ (*it).first ] += 1.;
    m_chanRate[(*it).first ] = 0.;
    m_chanRelRate[(*it).first ] = 0.;
    chanName << ":" <<  (*it).first;
    m_chanSeq[ (*it).first ] = 0;
  }
  for(LHCb::L0DUElementaryCondition::Map::iterator it = conditions.begin();it!=conditions.end();it++){
    if( init ) m_condCnt[ (*it).first ]=0;//init    
    if( report->conditionValue( ((*it).second)->id() ) ) m_condCnt[ (*it).first ] += 1.;
    m_condRate[(*it).first ] = 0.;
    condName << ":" <<  (*it).first;
    m_condSeq[ (*it).first ] = 0;
  }
  chanName << "]";
  condName << "]";

  // Ratios

  for(CounterMap::iterator it = m_chanCnt.begin(); m_chanCnt.end()!=it;it++){
    double rate = 0;
    double relRate = 0;
    if(m_evtCnt != 0) rate = 100. * (*it).second / m_evtCnt;
    if(m_decCnt != 0) relRate= 100. * (*it).second / m_decCnt;    
    m_chanRate[(*it).first] = rate ;
    m_chanRelRate[(*it).first] = relRate ;
  }
  for(CounterMap::iterator it = m_condCnt.begin(); m_condCnt.end()!=it;it++){
    double rate = 0;
    if(m_evtCnt !=0 ) rate = 100.* (*it).second / m_evtCnt;
    m_condRate[ (*it).first ]= rate ;
  }


  m_chanCntMap[tck]    = m_chanCnt;
  m_condCntMap[tck]     = m_condCnt;
  m_chanRateMap[tck]   = m_chanRate;
  m_chanRelRateMap[tck]= m_chanRelRate;
  m_condRateMap[tck]    = m_condRate;
  m_evtCntMap[tck]     = m_evtCnt;
  m_decCntMap[tck]     = m_decCnt;


  // -----------------------------------------------------------------------------------------
  // --------------------------------- Histogramming -----------------------------------------
  // -----------------------------------------------------------------------------------------
  std::stringstream base("");

  if(m_split){
    base << "Config_" << ttck.str();
  }
  else{
    base << "CurrentConfig";
  }
  

  // ------------ Data  ------------
  if(m_data){
    std::vector<std::string> moniData;
    for(LHCb::L0DUElementaryCondition::Map::iterator it = conditions.begin();it!=conditions.end();it++){
      LHCb::L0DUElementaryCondition* condition = (*it).second;
      LHCb::L0DUElementaryData* data = (LHCb::L0DUElementaryData*) condition->data();
      bool ok=true;
      for(std::vector<std::string>::iterator idata = moniData.begin() ; idata != moniData.end() ; idata++){
        if( data->name() == *idata){
          ok=false;
          break;
        }      
      }
      if( !ok )continue;
      if( data->type() == LHCb::L0DUElementaryData::Constant)continue;
      moniData.push_back( data->name() );
      std::stringstream dUnit("");
      dUnit << base.str() + "/L0Data/" << moniData.size();
      std::stringstream dName("");
      if(data->type() == LHCb::L0DUElementaryData::Compound)
        dName << " L0 compound  data ";
      else
        dName << " L0 predefined  data ";
      
      dName << " -" << data->name() << "- distribution (TCK = " << ttck.str() << ")";
      long dBin = data->saturation()+1;
      double value = (double) data->digit();
      plot1D( value , dUnit.str() , dName.str(), 0 , (double) dBin , dBin/m_bin);
    }
  }
  // ------------ L0DU Conditions ------------
  if(m_cond){
    std::stringstream ecName("");
    ecName << "L0DU Elementary Conditions counters (TCK = " << ttck.str() << ")";
    
    int ecBin = conditions.size();
    plot1D( -1. , base.str() + "/L0Conditions/Counters/1" , ecName.str()+ " - BX=T0", -1. ,(double) ecBin  , ecBin+1);
    plot1D( -1. , base.str() + "/L0Conditions/Counters/Prev2/1" , ecName.str()+ " - BX=Prev2", -1. ,(double) ecBin  , ecBin+1);
    plot1D( -1. , base.str() + "/L0Conditions/Counters/Prev1/1" , ecName.str()+ " - BX=Prev1", -1. ,(double) ecBin  , ecBin+1);
    plot1D( -1. , base.str() + "/L0Conditions/Counters/Next1/1" , ecName.str()+ " - BX=Next1", -1. ,(double) ecBin  , ecBin+1);
    plot1D( -1. , base.str() + "/L0Conditions/Counters/Next2/1" , ecName.str()+ " - BX=Next2", -1. ,(double) ecBin  , ecBin+1);
    if(ecBin>1)plot2D( -1. , -1. , base.str() + "/L0Conditions/Counters2D/1" 
                       , "L0DU Elementary Conditions 2D INclusive counters (TCK = " + ttck.str() + ")"              
                       , -1. ,(double) ecBin, -1. ,(double) ecBin , ecBin+1 , ecBin+1);
    if(ecBin>1)plot2D( -1. , -1. , base.str() + "/L0Conditions/Counters2D/2" 
                       , "L0DU Elementary Conditions 2D EXclusive counters (TCK = " + ttck.str() + ")"              
                       , -1. ,(double) ecBin, -1. ,(double) ecBin , ecBin+1 , ecBin+1);
    
    for(LHCb::L0DUElementaryCondition::Map::iterator it = conditions.begin();it!=conditions.end();it++){
      int id = ((*it).second)->id() ;
      std::string name = (*it).first;
      if( report->conditionValue( id ) ){
        plot1D( (double) id , base.str() + "/L0Conditions/Counters/1" ,ecName.str() + " - BX=T0", -1. ,(double) ecBin  , ecBin+1);
        m_condSeq[name] |=  (1 << 2);
      }
      if( report->conditionValue( id,-2 ) ){  
        plot1D( (double) id , base.str() + "/L0Conditions/Counters/Prev2/1"
                ,ecName.str() + " - BX=Prev2", -1. ,(double) ecBin, ecBin+1);
        m_condSeq[name] |=  (1 << 0);
      }
      
      if( report->conditionValue( id,-1 ) ){  
        plot1D( (double) id , base.str() + "/L0Conditions/Counters/Prev1/1" 
                ,ecName.str() + " - BX=Prev1", -1. ,(double) ecBin, ecBin+1);
        m_condSeq[name] |=  (1 << 1);
      }
      if( report->conditionValue( id,+1 ) ){      
        plot1D( (double) id , base.str() + "/L0Conditions/Counters/Next1/1" 
                ,ecName.str() + " - BX=Next1", -1. ,(double) ecBin, ecBin+1);
        m_condSeq[name] |=  (1 << 3);
      }    
      if( report->conditionValue( id,+2 ) ){
        plot1D( (double) id , base.str() + "/L0Conditions/Counters/Next2/1" 
                ,ecName.str() + " - BX=Next2", -1. ,(double) ecBin, ecBin+1);
        m_condSeq[name] |=  (1 << 4);
      }
      std::stringstream xid("");
      xid << id+1;
      plot1D( m_condSeq[name] , base.str() + "/L0Conditions/Sequence/" + xid.str() 
              , "Decision sequence over 5 BX for -" + name + "- Elementary Condition (TCK " + ttck.str() +")  - LSB = Prev2"    
              , 0. , 32. , 32); 
      
      if( ecBin <= 1)continue;
      // correlations
      for(LHCb::L0DUElementaryCondition::Map::iterator jt = it ;jt!=conditions.end();jt++){
        int jd = ((*jt).second)->id() ;
        
        if( report->conditionValue( id ) && report->conditionValue( jd ) ){
          // Inclusive 2D counters
          plot2D( (double) id , (double) jd , base.str() + "/L0Conditions/Counters2D/1" 
                  , "L0DU Elementary Conditions 2D INclusive counters (TCK = " + ttck.str() + ")"
                  , -1. ,(double) ecBin, -1. ,(double) ecBin , ecBin+1 , ecBin+1);
        // Exclusive 2D counters
          for(LHCb::L0DUElementaryCondition::Map::iterator kt =  conditions.begin() ;kt!=conditions.end();kt++){
            int kd = ((*kt).second)->id() ;
            if( report->conditionValue( kd ) || kd == id || kd == jd)continue;
            plot2D( (double) id , (double) jd , base.str() + "/L0Conditions/Counters2D/2" 
                    , "L0DU Elementary Conditions 2D EXclusive counters (TCK = " + ttck.str() + ")"
                    , -1. ,(double) ecBin, -1. ,(double) ecBin , ecBin+1 , ecBin+1);
          }
        }      
      } 
    }
  }  
  // ------------ L0DU Channels --------------
  if(m_chan){
    std::stringstream tcName("");
    tcName << "L0DU  Channels Decision counters (TCK = " << ttck.str() << ")";
    std::stringstream fcName("");
    fcName << "L0DU  triggered Channels counters (TCK = " << ttck.str() << ")";
    int cBin = channels.size();
    plot1D( -1. , base.str() + "/L0Channels/Counters/1" , tcName.str()+ " - BX=T0", -1. ,(double) cBin  , cBin+1);
    plot1D( -1. , base.str() + "/L0Channels/Counters/2" , fcName.str()+ " - BX=T0", -1. ,(double) cBin  , cBin+1);
    plot1D( -1. , base.str() + "/L0Channels/Counters/Prev2/1 " , tcName.str()+ " - BX=Prev2", -1. ,(double) cBin  , cBin+1);
    plot1D( -1. , base.str() + "/L0Channels/Counters/Prev1/1" , tcName.str()+ " - BX=Prev1", -1. ,(double) cBin  , cBin+1);
    plot1D( -1. , base.str() + "/L0Channels/Counters/Next1/1" , tcName.str()+ " - BX=Next1", -1. ,(double) cBin  , cBin+1);
    plot1D( -1. , base.str() + "/L0Channels/Counters/Next2/1" , tcName.str()+ " - BX=Next2", -1. ,(double) cBin  , cBin+1);
    if(cBin > 1)plot2D( -1. , -1. , base.str() + "/L0Channels/Counters2D/1" 
                        , "L0DU Channels Decision 2D INclusive counters (TCK = " + ttck.str() + ")"
                        , -1. ,(double) cBin, -1. ,(double) cBin , cBin+1 , cBin+1);
    if(cBin > 1)plot2D( -1. , -1. , base.str() + "/L0Channels/Counters2D/2" 
                        , "L0DU Channels Decision 2D EXclusive counters (TCK = " + ttck.str() + ")"
                        , -1. ,(double) cBin, -1. ,(double) cBin , cBin+1 , cBin+1);
    
    
    for(LHCb::L0DUChannel::Map::iterator it = channels.begin();it!=channels.end();it++){
      int id = ((*it).second)->id() ;
      std::string name = (*it).first;
      if( report->channelDecision( id ) ){
        plot1D( (double) id , base.str() + "/L0Channels/Counters/1" , tcName.str()+ " - BX=T0"    , -1. ,(double) cBin  , cBin+1);
        m_chanSeq["Global"] |=  (1 << 2);
        m_chanSeq[name] |=  (1 << 2);
      }    
      if( report->channelDecision( id , -2) ){
        plot1D( (double) id , base.str() + "/L0Channels/Counters/Prev2/1" 
                , tcName.str()+ " - BX=Prev2" , -1. ,(double) cBin  , cBin+1);
        m_chanSeq["Global"] |=  (1 << 0);
        m_chanSeq[name] |=  (1 << 0);
      }
      
      if( report->channelDecision( id , -1) ){
        plot1D( (double) id , base.str() + "/L0Channels/Counters/Prev1/1" 
                , tcName.str()+ " - BX=Prev1" , -1. ,(double) cBin  , cBin+1);
        m_chanSeq["Global"] |=  (1 << 1);
        m_chanSeq[name] |=  (1 << 1);
      }
      if( report->channelDecision( id , +1) ){
        plot1D( (double) id , base.str() + "/L0Channels/Counters/Next1/1" 
                , tcName.str()+ " - BX=Next1" , -1. ,(double) cBin  , cBin+1);
        m_chanSeq["Global"] |=  (1 << 3);
        m_chanSeq[name] |=  (1 << 3);
      }
    
      if( report->channelDecision( id , +2) ){
        plot1D( (double) id , base.str() + "/L0Channels/Counters/Next2/1"
                , tcName.str()+ " - BX=Next2" , -1. ,(double) cBin  , cBin+1);
        m_chanSeq["Global"] |=  (1 << 4);
        m_chanSeq[name] |=  (1 << 4);
      }    
      if( report->triggeredChannel( id ) )
        plot1D( (double) id , base.str() + "/L0Channels/Counters/2" , fcName.str()+ " - BX=T0"    , -1. ,(double) cBin  , cBin+1);
      
      std::stringstream xid("");
      xid << id+1;
      plot1D( m_chanSeq[name] , base.str() + "/L0Channels/Sequence/" + xid.str() 
              , "Decision sequence over 5 BX for -" + name + "- channel (TCK " + ttck.str() +") - LSB = Prev2"    , 0. , 32. , 32); 
      
      if(cBin <= 1)continue;
      // correlations
      for(LHCb::L0DUChannel::Map::iterator jt = it ;jt!=channels.end();jt++){
        int jd = ((*jt).second)->id() ;
        if( report->channelDecision( id ) && report->channelDecision( jd ) ){
          // inclusive 2D counters
          plot2D( (double) id , (double) jd , base.str() + "/L0Channels/Counters2D/1" 
                  , "L0DU Channels Decision 2D INclusive counters (TCK = " + ttck.str() + ")"
                  , -1. ,(double) cBin, -1. ,(double) cBin , cBin+1 , cBin+1);
          //exclusive 2D counters
          for(LHCb::L0DUChannel::Map::iterator kt = channels.begin();kt!=channels.end();kt++){
            int kd = ((*kt).second)->id() ;
            if( report->channelDecision( kd ) || kd == id || kd == jd)continue;
            plot2D( (double) id , (double) jd , base.str() + "/L0Conditions/Counters2D/2" 
                    , "L0DU Channels Decision 2D EXclusive counters (TCK = " + ttck.str() + ")"
                    , -1. ,(double) cBin, -1. ,(double) cBin , cBin+1 , cBin+1);
          }
        }
      }    
    }  
  }
  // ------------ Decision  ------------
  if(m_dec){
    std::stringstream decName("");
    decName << "L0 Decision counters (TCK = " << ttck.str() << ") - Counter/Decision/TTB/FB";
    plot1D( -1. , base.str() + "/L0Decision/Counters/1" , decName.str(), -1. ,3  , 4);
    if(report->decision())        plot1D( 0. , base.str() + "/L0Decision/Counters/1" , decName.str(),  -1. ,3  , 4);
    if(report->timingTriggerBit())plot1D( 1. , base.str() + "/L0Decision/Counters/1" , decName.str(),  -1. ,3  , 4);
    if(report->forceBit())        plot1D( 2. , base.str() + "/L0Decision/Counters/1" , decName.str(),  -1. ,3  , 4);
    std::stringstream seqName("");
    seqName << "L0 Decision sequence over 5 BX (TCK = " << ttck.str() << ") - LSB = Prev2";
    plot1D( (double) m_chanSeq["Global"] , base.str() + "/L0Decision/Sequence/1" , seqName.str(), 0 ,32  , 32);
  }  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0DUReportMonitor::finalize() {

  debug() << "==> Finalize" << endmsg;


  
  info() << "======================================================================== " <<endreq;
  info() << " L0DUReport Monitoring ran on " << m_evtCntMap.size() <<" Configuration(s) " << endreq;
  info() << "======================================================================== "  << endreq;
  info() << " " << endreq;
  for(std::map<unsigned int,LHCb::L0DUConfig*>::iterator it = m_configs.begin(); it != m_configs.end(); it++){
    unsigned int tck = (*it).first;
    LHCb::L0DUConfig* config =(*it).second;

    m_chanCnt     = m_chanCntMap[tck];
    m_condCnt     = m_condCntMap[tck];
    m_chanRate    = m_chanRateMap[tck];
    m_chanRelRate = m_chanRelRateMap[tck];
    m_condRate    = m_condRateMap[tck];
    m_evtCnt      = m_evtCntMap[tck];
    m_decCnt      = m_decCntMap[tck];
    double rate =  100.* m_decCnt / m_evtCnt ;
    double eRate = 100.* sqrt(m_decCnt)/m_evtCnt;
    
    info() << "   **************************************************** " << endreq;    
    info() << "   ***  Trigger Configuration Key : "  << format("0x%04X", tck)  << " (" << tck << ")"<<  endreq;
    info() << "   **************************************************** " << endreq;    
    debug() << "       REMIND : the corresponding L0DU algorithm description is : " << endreq;
    debug() << config->description() << endreq;
    info() << " " << endreq;

    info() << "   ------------------------------------------------------------------- " <<endreq;
    info() << "   -- L0 Performance on " << m_evtCnt << " events" << endreq;  
    info() << "   ------------------------------------------------------------------- " <<endreq;
    info() << "              *  Accepted L0          : " 
           << format( " %8.0f events :  rate = ( %6.2f +- %6.2f) %% ", m_decCnt, rate, eRate ) 
           << endreq;
    info() << "   ------------------------ CHANNELS --------------------------------- " <<endreq;
    for(CounterMap::iterator ic =  m_chanRate.begin(); m_chanRate.end()!=ic ; ic++){
      std::string name = (*ic).first;
      LHCb::L0DUChannel::Map channels = config->channels();
      LHCb::L0DUChannel* channel = channels[name];
      std::string status="** DISABLED **";
      if(channel->enable() ){
        status ="   ENABLED    ";
        info() << "   * " << status << " channel : " 
             << format( "%20s :  %8.0f events : rate = %6.2f  %%  (rel. rate =  %6.2f %% ) ", 
                        name.c_str(), m_chanCnt[name], m_chanRate[name] , m_chanRelRate[name]) << endreq;
      }else{
        info() << "   * " << status << " channel : " 
             << format( "%20s :  %8.0f events : rate = %6.2f  %%   ", 
                        name.c_str(), m_chanCnt[name], m_chanRate[name]) << endreq;
      }
    }
    info() << "   ------------------------ CONDITIONS ------------------------------ " <<endreq;
    for(CounterMap::iterator ic =  m_condRate.begin(); m_condRate.end()!=ic ; ic++){
      std::string name = (*ic).first;
    info() << "   *  Elementary Condition  " 
             << format( "%20s :  %8.0f events : rate = %6.2f  %%   ", 
                        name.c_str(), m_condCnt[name], m_condRate[name]) << endreq;
    }
    info() << "======================================================================== " <<endreq;
  }

   return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================




