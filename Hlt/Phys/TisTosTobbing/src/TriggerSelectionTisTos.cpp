// $Id: TriggerSelectionTisTos.cpp,v 1.1.1.1 2007-08-09 01:31:19 tskwarni Exp $
// Include files 
#include "HltBase/HltUtils.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TriggerSelectionTisTos.h"

#include "HltBase/HltTypes.h"
#include "Event/HltSummary.h"
#include "HltBase/HltConfigurationHelper.h"
#include "Event/Track.h"
#include "Event/Particle.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TriggerSelectionTisTos
//
// 2007-08-06 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TriggerSelectionTisTos );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TriggerSelectionTisTos::TriggerSelectionTisTos( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITriggerSelectionTisTos>(this);

  declareProperty("HltSummaryLocation",
                  m_HltSummaryLocation = LHCb::HltSummaryLocation::Default);
 
  declareProperty("OfflineMuonTrackLocation",
                  m_MuonTracksLocation = LHCb::TrackLocation::Muon);
 
  declareProperty("TOSFracVelo", m_TOSFrac[kVelo] = 0.7 );
  declareProperty("TOSFracTT",   m_TOSFrac[kTT]   = 0.7 );
  declareProperty("TOSFracOTIT", m_TOSFrac[kOTIT] = 0.7 );
  declareProperty("TOSFracMuon", m_TOSFrac[kMuon] = 0.7 );
  declareProperty("TOSFracEcal", m_TOSFrac[kEcal] = -0.01); // makes TOS always true (Ecal not used yet)
  declareProperty("TOSFracHcal", m_TOSFrac[kHcal] = 0.01);

  declareProperty("TISFracVelo", m_TISFrac[kVelo] = 0.01 );
  declareProperty("TISFracTT",   m_TISFrac[kTT]   = 0.01 );
  declareProperty("TISFracOTIT", m_TISFrac[kOTIT] = 0.01 );
  declareProperty("TISFracMuon", m_TISFrac[kMuon] = 0.01 );
  declareProperty("TISFracEcal", m_TISFrac[kEcal] = 1.01 ); // makes TIS always true (Ecal not used yet)
  declareProperty("TISFracHcal", m_TISFrac[kHcal] = 0.01 );

  for( int hitType=0; hitType!=nHitTypes; ++hitType ){
    m_offlineInput[hitType].reserve(500);
  }

  m_track2calo = NULL;
  
  m_warning_count=0;
  
}


//=============================================================================
// Destructor
//=============================================================================
TriggerSelectionTisTos::~TriggerSelectionTisTos() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TriggerSelectionTisTos::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_track2calo = tool<ITrack2Calo>( "Track2Calo","Track2Calo",this);

  setOfflineInput();
  
  info() << " TriggerSelectionTisTos ";
  info() << " HltSummaryLocation=" <<  m_HltSummaryLocation;
  info() << " OfflineMuonTrackLocation=" << m_MuonTracksLocation;
  info() << endmsg;
  
  info() << " TOSFracVelo " <<  m_TOSFrac[kVelo] << " TISFracVelo " << m_TISFrac[kVelo] << endmsg;
  info() << " TOSFracTT "   <<  m_TOSFrac[kTT]   << " TISFracTT "   << m_TISFrac[kTT]   << endmsg;
  info() << " TOSFracOTIT " <<  m_TOSFrac[kOTIT] << " TISFracOTIT " << m_TISFrac[kOTIT] << endmsg;
  info() << " TOSFracMuon " <<  m_TOSFrac[kMuon] << " TISFracMuon " << m_TISFrac[kMuon] << endmsg;
  info() << " TOSFracEcal " <<  m_TOSFrac[kEcal] << " TISFracEcal " << m_TISFrac[kEcal] << endmsg;
  info() << " TOSFracHcal " <<  m_TOSFrac[kHcal] << " TISFracHcal " << m_TISFrac[kHcal] << endmsg;
  

 
  return StatusCode::SUCCESS;
}


// ==================== internal classification of hit type for matching 
// see HitType enum
// =====================================================================
int TriggerSelectionTisTos::hitMatchType(const LHCbID & id)
{  
    int hitType=kNotUsed;
    if( id.isVelo() ){
      hitType=kVelo;
    } else if( id.isTT() ){
      hitType=kTT;
    } else if ( id.isOT() || id.isIT() ){
      hitType=kOTIT;
    } else if ( id.isMuon() ) {
      hitType=kMuon;
    } else if ( id.isCalo() ) {
      if( int( id.caloID().calo() ) == CaloCellCode::CaloNumFromName("Hcal") ){
        hitType=kHcal;
      } else if (  int( id.caloID().calo() ) == CaloCellCode::CaloNumFromName("Ecal") ){
        hitType=kEcal;
      }
    }
    return hitType;    
}

//=============================================================================
//    lowest level function for qualifying match between hits on trigger track
//    and list of hits from off-line objects (could be from more than one 
//    off-line track)
//============================================================================
void TriggerSelectionTisTos::matchIDs(const LHCb::Track & track, 
                         const ClassifiedHits idlist[],
                         double overlap[] ) 
{
 

  int nHitsAll[nHitTypes];
  int nHitsMatched[nHitTypes];  
  for( int hitType=0; hitType!=nHitTypes; ++hitType ){
    nHitsAll[hitType]=0;
    nHitsMatched[hitType]=0;
    overlap[hitType]=2.0; // default value indicating not present on trigger object
  }
  
  const std::vector<LHCbID>& ids = track.lhcbIDs();
  for( std::vector<LHCbID>::const_iterator id=ids.begin();id!=ids.end();++id){      

    int hitType=hitMatchType(*id);
    if( hitType==kNotUsed ){ continue; }
  
    nHitsAll[hitType] += 1;
    
    for( std::vector<LHCbID>::const_iterator id2=(idlist[hitType]).begin(); id2!=(idlist[hitType]).end(); ++id2){
      if( *id == *id2 ){ 
        nHitsMatched[hitType] += 1;
        break;
      }
    }
  }

  for( int hitType=0; hitType!=nHitTypes; ++hitType ){
    if( nHitsAll[hitType]>0 ){
      overlap[hitType] = double(nHitsMatched[hitType])/double(nHitsAll[hitType]);
    }
  }
}


//=============================================================================  
// final state dughters for given particle 
//=============================================================================  
std::vector<const LHCb::Particle*> TriggerSelectionTisTos::finalStateParticles(const LHCb::Particle* particle) 
{
  std::vector<const LHCb::Particle*> outputParticles;
  if( particle == NULL )return outputParticles;
  std::vector<const LHCb::Particle*> daughters = particle->daughtersVector();  
  if( daughters.size() >0 ){
    for( std::vector<const LHCb::Particle*>::const_iterator p = daughters.begin(); p!=daughters.end(); ++p){
      std::vector<const LHCb::Particle*> daughterParticles = finalStateParticles(*p);
      outputParticles.insert(outputParticles.end(),daughterParticles.begin(),daughterParticles.end());
    }
  } else {
    outputParticles.push_back(particle);
  }
  return outputParticles;
}
 
//=============================================================================

//=============================================================================  
// TIS/TOS based on single-track list:   
//=============================================================================  
void TriggerSelectionTisTos::trackListTISTOS(const std::vector<Track*> & ontracks,
                                const ClassifiedHits offidlist[],
                                bool& TIS,bool& TOS) const
{

    TIS=false;TOS=false;

    for (std::vector<Track*>::const_iterator onit = ontracks.begin();onit != ontracks.end(); ++onit) {
       const Track& ontrack = *(*onit);
       double overlap[nHitTypes];
       matchIDs(ontrack,offidlist,overlap);
       if( m_muonTracks == NULL ){ overlap[kMuon]=2.0; }
       if( !TOS ){
         // effectively .and. requirement between all types
         for( int hitType=0; hitType!=nHitTypes; ++hitType ){
           TOS= overlap[hitType] >= m_TOSFrac[hitType];
           if( !TOS )break;
         }
       }
       if( !TIS ){ 
         // effectively .and. requirement between all types
         for( int hitType=0; hitType!=nHitTypes; ++hitType ){
           TIS= (overlap[hitType] > 1.1) || ( overlap[hitType]<=m_TISFrac[hitType] );
           if( !TIS )break;
         }
       }
        // don't waste time checking more trigger-tracks if TIS and TOS already
       if( TIS && TOS ){ break; }
    }
}


//=============================================================================  
// TIS/TOS based on vertex list;
//=============================================================================  
void TriggerSelectionTisTos::vertexListTISTOS(const std::vector<RecVertex*> & onvertices,
                                 const ClassifiedHits offidlist[],
                                 bool& TIS,bool& TOS) const
{

    TIS=false;TOS=false;

    for (std::vector<RecVertex*>::const_iterator oniv = onvertices.begin();oniv != onvertices.end(); ++oniv) {
         RecVertex& onvertex = *(*oniv);
         const SmartRefVector<LHCb::Track> & ontracks=onvertex.tracks();
         bool thisTIS(true),thisTOS(true);
         for (SmartRefVector<LHCb::Track>::const_iterator onit = ontracks.begin();onit != ontracks.end(); ++onit) {
            const Track& ontrack = *(*onit);
            double overlap[nHitTypes];
            matchIDs(ontrack,offidlist,overlap);
            if( m_muonTracks == NULL ){ overlap[kMuon]=2.0; }
            if( thisTOS ){
               // effectively .and. requirement between all types
               for( int hitType=0; hitType!=nHitTypes; ++hitType ){
                  thisTOS= overlap[hitType] >= m_TOSFrac[hitType];
                  if( !thisTOS )break;
               }
            }
            if( thisTIS ){ 
               // effectively .and. requirement between all types
               for( int hitType=0; hitType!=nHitTypes; ++hitType ){
                  thisTIS= (overlap[hitType] > 1.1) || ( overlap[hitType]<=m_TISFrac[hitType] );
                  if( !thisTIS )break;
               }
            }
            // don't waste time checking more tracks if not-TIS and not-TOS already
            if( (!thisTIS) && (!thisTOS) ){ break; }
         }
         
         if( !TOS ){ TOS=thisTOS; }
         if( !TIS ){ TIS=thisTIS; }
           // don't waste time checking more trigger-vertices if TIS and TOS already
         if( TIS && TOS ){ break; }
    }
}



//=============================================================================  
// TIS/TOS based on particle list;
//=============================================================================  
void TriggerSelectionTisTos::particleListTISTOS(const SmartRefVector<LHCb::Particle> & onparticles,
                                 const ClassifiedHits offidlist[],
                                 bool& TIS,bool& TOS) const
{
    TIS=false;TOS=false;

    for(SmartRefVector<LHCb::Particle>::const_iterator onp = onparticles.begin();onp != onparticles.end(); ++onp) {
      const Particle* onparticle = *onp;
      std::vector<const LHCb::Particle*> fdaughters= finalStateParticles( onparticle );
      bool thisTIS(true),thisTOS(true);
      for(std::vector<const LHCb::Particle*>::const_iterator pfp =fdaughters.begin(); pfp!=fdaughters.end(); ++pfp){
        const ProtoParticle* pp = (*pfp)->proto();
        if( 0!=pp ){
          const Track* onit=pp->track();
          if( 0!=onit ){
            const Track& ontrack = *onit;
            double overlap[nHitTypes];
            matchIDs(ontrack,offidlist,overlap);
            if( m_muonTracks == NULL ){ overlap[kMuon]=2.0; }
            if( thisTOS ){
               // effectively .and. requirement between all types
               for( int hitType=0; hitType!=nHitTypes; ++hitType ){
                  thisTOS= overlap[hitType] >= m_TOSFrac[hitType];
                  if( !thisTOS )break;
               }
            }
            if( thisTIS ){ 
               // effectively .and. requirement between all types
               for( int hitType=0; hitType!=nHitTypes; ++hitType ){
                  thisTIS= (overlap[hitType] > 1.1) || ( overlap[hitType]<=m_TISFrac[hitType] );
                  if( !thisTIS )break;
               }
            }
            // don't waste time checking more tracks if not-TIS and not-TOS already
            if( (!thisTIS) && (!thisTOS) ){ break; }
          }
        }
      }
      if( !TOS ){ TOS=thisTOS; }
      if( !TIS ){ TIS=thisTIS; }
      // don't waste time checking more trigger-vertices if TIS and TOS already
      if( TIS && TOS ){ break; }
    }
}
	
// ------------------------------------------------------------------------------------

void TriggerSelectionTisTos::getHltSummary()
{
  if( (m_summary == NULL) || (m_hltconf==NULL) ){
    m_summary = get<HltSummary>(m_HltSummaryLocation);
    if( m_summary == NULL ){ 
      error() << " No HLT Summary at " << m_HltSummaryLocation << " No TisTosing possible " << endmsg; 
    } else {
      std::string loca = m_HltSummaryLocation+"/Configuration";
      Hlt::DataHolder< Hlt::Configuration >* holder = get< Hlt::DataHolder<Hlt::Configuration> >(loca);
      m_hltconf = &(holder->object());
      if( m_hltconf == NULL ){ info() << " No HLT Configuration at " << loca << " No TisTosing possible " << endmsg; 
      }
    }
  }
}


//==================== PUBLIC ==================================================

//=============================================================================
// -------------- offline inputs -----------------------------------------------
//=============================================================================

// erase previous input ---------------------------------------------------------
void TriggerSelectionTisTos::setOfflineInput( )
{
  for( int hitType=0; hitType!=nHitTypes; ++hitType ){
    m_offlineInput[hitType].clear();
  }
  m_hcalDeCal = NULL;
  m_ecalDeCal = NULL;

  m_muonTracks = NULL;

  m_summary = NULL;
  m_hltconf = NULL;

}
   
//    hit list input ---------------------------------------------------------------
void TriggerSelectionTisTos::addToOfflineInput( const std::vector<LHCb::LHCbID> & hitlist, ClassifiedHits hitidlist[] )
{
  for( std::vector<LHCbID>::const_iterator id=hitlist.begin();id!=hitlist.end();++id){

    int hitType=hitMatchType(*id);
    if( hitType==kNotUsed ){ continue; }

    bool duplicate(false);    
    for( std::vector<LHCbID>::const_iterator id2=(hitidlist[hitType]).begin(); 
                                             id2!=(hitidlist[hitType]).end(); ++id2){
      if( *id == *id2 ){ 
        duplicate=true;
        break;
      }
      
    }
    if( duplicate )continue;

    (hitidlist[hitType]).push_back(*id); 
    
  }
}

//    Track input ---------------------------------------------------------------
void TriggerSelectionTisTos::addToOfflineInput( const LHCb::Track & track, ClassifiedHits hitidlist[]) 
{

  //   add hits saved directly on the track ------------------------------------------------------------
  const std::vector<LHCbID>& ids = track.lhcbIDs();
  addToOfflineInput( ids, hitidlist);

  // ------------------ add expected Hcal hits (3x3 group around cell crossed at shower max) ------------
  // do it only if needed 
  if( ( m_TOSFrac[kHcal] > 0.0 ) && ( m_TISFrac[kHcal] < 1.0 ) ){
    if( m_track2calo != NULL ){
      if( m_hcalDeCal==NULL ){ m_hcalDeCal = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal ); }
      if( m_hcalDeCal != NULL ){
        if( m_track2calo->match( &track, DeCalorimeterLocation::Hcal) ){ 
          if( m_track2calo->isValid() ){ // is matched cellid valid
            LHCb::CaloCellID centerCell  = m_track2calo->caloCellID();
            std::vector<LHCb::CaloCellID> cells3x3;
            cells3x3.push_back(centerCell);
            const std::vector<LHCb::CaloCellID> & neighbors = m_hcalDeCal->neighborCells( centerCell );
            cells3x3.insert( cells3x3.end(), neighbors.begin(),  neighbors.end() );      
            for( std::vector<LHCb::CaloCellID>::const_iterator id=cells3x3.begin();id!=cells3x3.end();++id){
              int hitType=hitMatchType(*id);
              if( hitType==kNotUsed ){ continue; }
              bool duplicate(false);    
              for( std::vector<LHCbID>::const_iterator id2=(hitidlist[hitType]).begin(); 
                   id2!=(hitidlist[hitType]).end(); ++id2){
                if( *id == id2->caloID() ){ 
                  duplicate=true;
                  break;
                }
              }
              if( duplicate )continue;
              (hitidlist[hitType]).push_back(*id); 
            }
          }
        }
      }
    }
  }
  // ------------------ add expected Ecal hits (3x3 group around cell crossed at shower max) ------------
  // do it only if needed 
  if( ( m_TOSFrac[kEcal] > 0.0 ) && ( m_TISFrac[kEcal] < 1.0 ) ){
    if( m_track2calo != NULL ){
      if( m_ecalDeCal==NULL ){ m_ecalDeCal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal ); }
      if( m_ecalDeCal != NULL ){
        if( m_track2calo->match( &track, DeCalorimeterLocation::Ecal) ){ // play with 3rd parameter?
          if( m_track2calo->isValid() ){ // is matched cellid valid
            LHCb::CaloCellID centerCell  = m_track2calo->caloCellID();
            std::vector<LHCb::CaloCellID> cells3x3;
            cells3x3.push_back(centerCell);
            const std::vector<LHCb::CaloCellID> & neighbors = m_ecalDeCal->neighborCells( centerCell );
            cells3x3.insert( cells3x3.end(), neighbors.begin(),  neighbors.end() );      
            for( std::vector<LHCb::CaloCellID>::const_iterator id=cells3x3.begin();id!=cells3x3.end();++id){
              int hitType=hitMatchType(*id);
              if( hitType==kNotUsed ){ continue; }
              bool duplicate(false);    
              for( std::vector<LHCbID>::const_iterator id2=(hitidlist[hitType]).begin(); 
                   id2!=(hitidlist[hitType]).end(); ++id2){
                if( *id == id2->caloID() ){ 
                  duplicate=true;
                  break;
                }
              }
              if( duplicate )continue;
              (hitidlist[hitType]).push_back(*id); 
            }
          }
        }
      }
    }
  }

}

//    Proto-particle input -----------------------------------------------------------------------
void TriggerSelectionTisTos::addToOfflineInput( const LHCb::ProtoParticle & protoParticle,
                                      ClassifiedHits hitidlist[] ) 
{
  const Track* t=protoParticle.track();
  if( 0!=t ){
    // add hits from track itself (possibly projected hcal and ecal hits too)
    addToOfflineInput(*t,hitidlist);
    // add muon hits only if needed 
    if( ( m_TOSFrac[kMuon] > 0.0 ) && ( m_TISFrac[kMuon] < 1.0 ) ){
      if( m_muonTracks==NULL ){ 
        if( exist<LHCb::Tracks>(m_MuonTracksLocation) ){
          m_muonTracks = get<LHCb::Tracks>(LHCb::TrackLocation::Muon);
        } else {
          if( m_warning_count++ < 10 ){
            warning() << " No offline muon tracks at " << m_MuonTracksLocation << " thus, muon hits will be ignored on trigger tracks. " << endmsg;
            if( m_warning_count==10 ){
              warning() << " The above warning was printed for the last time " << endmsg;
            }
          }
        }
      }
      if( m_muonTracks != NULL ){
        const LHCb::MuonPID* muid = protoParticle.muonPID();
        if( muid!=0 ){
          if( muid->IsMuon() ){
            const Track* mu= m_muonTracks->object(t->key());
            if( mu!=0 ){                                                                   
              const std::vector<LHCbID>& ids = mu->lhcbIDs();
              addToOfflineInput( ids, hitidlist);
            }
          }
        }
      }
    }
  }
}



//    Particle input; for composite particles loop over daughters will be executed ------------------------------
void TriggerSelectionTisTos::addToOfflineInput( const LHCb::Particle & particle, ClassifiedHits hitidlist[]  )
{
  const SmartRefVector<LHCb::Particle> & daughters = particle.daughters();  
  if( daughters.size() >0 ){
    for(SmartRefVector<LHCb::Particle>::const_iterator p = daughters.begin(); p!=daughters.end(); ++p){
      const LHCb::Particle & part = *(*p);
      addToOfflineInput(part,hitidlist);
    }
  } else {
    const ProtoParticle* pp = particle.proto();
    if( 0!=pp ){
      addToOfflineInput(*pp,hitidlist);
    }
  }
}

//=============================================================================
// -------------- outputs:
//=============================================================================

  //
  //    decision =  was trigger satisfied ? (this is independent of offline input passed to this tool!)
  //    tis      =  Trigger Independent of Signal  
  //                (trigger selection was satisified independently of the offline input passed to this tool)
  //    tos      =  Trigger On Signal  
  //                (the offline input passed to this tool was sufficient to satisfy this trigger selection)
  //   
  //    comments:
  //         if decision==false then tis=false and tos=false 
  //
  //         tis, tos are not mutually exclusive ( tis==true and tos=true is possible, TOE in TriggerSource terminology)
  //
  //         TOB (Trigger On Both ) =  decision && !tis && !tos  
  //           


// ------------ single selection summary TisTos
void TriggerSelectionTisTos::selectionTisTos( std::string selectionName,  
					      bool & decision, bool & tis, bool & tos )
{
  decision=false; tis=false; tos=false;
  getHltSummary();
  if( (m_summary==NULL)||(m_hltconf==NULL) )return;

  int selID = HltConfigurationHelper::getID(*m_hltconf,"SelectionID",selectionName);

  decision= m_summary->hasSelectionSummary( selID );
  if( !decision )return;

  const LHCb::HltSelectionSummary& sel = m_summary->selectionSummary( selID );

  decision= sel.decision();
  if( !decision )return;

  if( sel.data().size() > 0 ){
    
  ContainedObject* obj = sel.data().front();

  // try as track trigger
  Hlt::DataSizeHolder< std::vector<Track*> >* holder = 
    dynamic_cast< Hlt::DataSizeHolder< std::vector<Track*> >*>(obj);
  if (holder) {
    const std::vector<Track*> & tracks = holder->object();
    trackListTISTOS(tracks,m_offlineInput,tis,tos);
    return;
  } 

  // try as vertex trigger
  Hlt::DataSizeHolder< std::vector<RecVertex*> >* holderv = 
    dynamic_cast< Hlt::DataSizeHolder< std::vector<RecVertex*> >*>(obj);
  if (holderv) {
    const std::vector<RecVertex*> & vertices = holderv->object();
    vertexListTISTOS(vertices,m_offlineInput,tis,tos);
    return;
  } 

  }

  // must be particle trigger
  const SmartRefVector< LHCb::Particle > & particles = sel.particles();         
  if( particles.size() > 0 ){
    particleListTISTOS( particles,m_offlineInput,tis,tos);
  }

}

// ------------ multiple selections summary TisTos
void TriggerSelectionTisTos::selectionTisTos( std::vector< std::string > selectionNames,
                                bool & decision, bool & tis, bool & tos ,
                                bool selectionOR)
{
  if( selectionOR ){
    //            OR between various trigger selections
    decision=false; tis=false; tos=false;
    for( std::vector< std::string >::const_iterator pselnam=selectionNames.begin();
         pselnam!=selectionNames.end();
         ++pselnam){
      bool decision_thisSel,tis_thisSel,tos_thisSel;
      selectionTisTos( *pselnam,decision_thisSel,tis_thisSel,tos_thisSel);
      decision = decision || decision_thisSel;
      tis = tis || tis_thisSel;
      tos = tos || tos_thisSel;
      if( tis && tos )break; // don't waste time if tis && tos already
    }
  } else {
    //            AND between various trigger selections
    decision=true; tis=true; tos=true;
    for( std::vector< std::string >::const_iterator pselnam=selectionNames.begin();
         pselnam!=selectionNames.end();
         ++pselnam){
      bool decision_thisSel,tis_thisSel,tos_thisSel;
      selectionTisTos( *pselnam,decision_thisSel,tis_thisSel,tos_thisSel);
      decision = decision && decision_thisSel;
      tis = tis && tis_thisSel;
      tos = tos && tos_thisSel;
      if( !decision )break; // don't waste time if failed trigger already (tis,tos are false too!)      
    }
  } 
}

// ------------ auxiliary output:  list of LHCbIDs corresponding to present offline input
std::vector<LHCb::LHCbID> TriggerSelectionTisTos::offlineLHCbIDs() 
{
  std::vector<LHCb::LHCbID> flatHitList;
  for( int hitType=0; hitType!=nHitTypes; ++hitType ){
    if( m_offlineInput[hitType].size()>0 ){
      flatHitList.insert(flatHitList.end(),m_offlineInput[hitType].begin(),m_offlineInput[hitType].end());
    }
  }
  return flatHitList;
}

// ------------  additional functionality:  lists of tracks/vertices/particles from selection summary
//               satisfying TOS, ordered according to TOS quality (best first)
//               return empty vector in case of a mismatch between the output type and the selection summary

std::vector<const LHCb::Track*>     TriggerSelectionTisTos::matchedTOSTracks( std::string selectionName )
{
  std::vector<const LHCb::Track*> matchedTracks;

  getHltSummary();
  if( (m_summary==NULL)||(m_hltconf==NULL) )return matchedTracks;

  int selID = HltConfigurationHelper::getID(*m_hltconf,"SelectionID",selectionName);

  bool decision= m_summary->hasSelectionSummary( selID );
  if( !decision )return matchedTracks;

  const LHCb::HltSelectionSummary& sel = m_summary->selectionSummary( selID );

  decision= sel.decision();
  if( !decision )return matchedTracks;

  if( sel.data().size() <= 0 )return matchedTracks;
    
  ContainedObject* obj = sel.data().front();

  // try as track trigger
  Hlt::DataSizeHolder< std::vector<Track*> >* holder = 
    dynamic_cast< Hlt::DataSizeHolder< std::vector<Track*> >*>(obj);
  if (!holder) return matchedTracks;

  const std::vector<Track*> & tracks = holder->object();

  std::vector<double> aveovlp;

  for (std::vector<Track*>::const_iterator onit = tracks.begin();onit != tracks.end(); ++onit) {
       const Track& ontrack = *(*onit);
       double overlap[nHitTypes];
       matchIDs(ontrack, m_offlineInput,overlap);
       if( m_muonTracks == NULL ){ overlap[kMuon]=2.0; }
       bool TOS(false); double ovlp(0.0); int n(0);
       // effectively .and. requirement between all types
       for( int hitType=0; hitType!=nHitTypes; ++hitType ){
         TOS= (overlap[hitType] >= m_TOSFrac[hitType]);
         if( !TOS ){ break;}
         if( overlap[hitType] <1.1 ){ 
           ++n;	   
           ovlp+=overlap[hitType];
         }
       }
       if( TOS && (n>0) ){
         ovlp = ovlp/n;
         aveovlp.push_back(ovlp);
         matchedTracks.push_back(*onit);
       }
  }

  if( aveovlp.size()>1 ){
    bool swap(true);
    while(swap){
      swap=false;
      for( unsigned int i1=0; i1<(aveovlp.size()-1);++i1 ){
        for( unsigned int i2=i1+1; i2<aveovlp.size();++i2) {
          if( aveovlp[i1]<aveovlp[i2] ){
            double tmp=aveovlp[i1]; aveovlp[i1]=aveovlp[i2]; aveovlp[i2]=tmp;
            const LHCb::Track* ptmp=matchedTracks[i1];matchedTracks[i1]=matchedTracks[i2];matchedTracks[i2]=ptmp;
            swap=true;
          }
        }
      }
    }
  }

  return matchedTracks;
}

std::vector<const LHCb::RecVertex*> TriggerSelectionTisTos::matchedTOSVertices( std::string selectionName )
{
  std::vector<const  LHCb::RecVertex*> matchedVertices;

  getHltSummary();
  if( (m_summary==NULL)||(m_hltconf==NULL) )return matchedVertices;

  int selID = HltConfigurationHelper::getID(*m_hltconf,"SelectionID",selectionName);

  bool decision= m_summary->hasSelectionSummary( selID );
  if( !decision )return matchedVertices;

  const LHCb::HltSelectionSummary& sel = m_summary->selectionSummary( selID );

  decision= sel.decision();
  if( !decision )return matchedVertices;

  if( sel.data().size() <= 0 )return matchedVertices;
    
  ContainedObject* obj = sel.data().front();

  // try as vertex trigger
  Hlt::DataSizeHolder< std::vector<RecVertex*> >* holderv = 
    dynamic_cast< Hlt::DataSizeHolder< std::vector<RecVertex*> >*>(obj);
  if (!holderv) return matchedVertices;

  const std::vector<RecVertex*> & onvertices = holderv->object();

  std::vector<double> aveovlp;

    for (std::vector<RecVertex*>::const_iterator oniv = onvertices.begin();oniv != onvertices.end(); ++oniv) {
         RecVertex& onvertex = *(*oniv);
         const SmartRefVector<LHCb::Track> & ontracks=onvertex.tracks();
         bool thisTOS(true); double ovlp(0.0); int n(0);
         for (SmartRefVector<LHCb::Track>::const_iterator onit = ontracks.begin();onit != ontracks.end(); ++onit) {
            const Track& ontrack = *(*onit);
            double overlap[nHitTypes];
            matchIDs(ontrack,m_offlineInput,overlap);
            if( m_muonTracks == NULL ){ overlap[kMuon]=2.0; }
            if( thisTOS ){
               // effectively .and. requirement between all types
               for( int hitType=0; hitType!=nHitTypes; ++hitType ){
                  thisTOS= (overlap[hitType] >= m_TOSFrac[hitType]);
                  if( !thisTOS )break;
                  if( overlap[hitType] <1.1 ){ 
                    ++n;	   
                    ovlp+=overlap[hitType];
                  }
               }
               if( !thisTOS )break;
            }
         }
         if( thisTOS && (n>0) ){
           ovlp = ovlp/n;
           aveovlp.push_back(ovlp);
           matchedVertices.push_back(*oniv);
         }
    }

  if( aveovlp.size()>1 ){
    bool swap(true);
    while(swap){
      swap=false;
      for( unsigned int i1=0; i1<(aveovlp.size()-1);++i1 ){
        for( unsigned int i2=i1+1; i2<aveovlp.size();++i2) {
          if( aveovlp[i1]<aveovlp[i2] ){
            double tmp=aveovlp[i1]; aveovlp[i1]=aveovlp[i2]; aveovlp[i2]=tmp;
            const RecVertex* ptmp=matchedVertices[i1];matchedVertices[i1]=matchedVertices[i2];matchedVertices[i2]=ptmp;
            swap=true;
          }
        }
      }
    }
  }

  return matchedVertices;
}


std::vector<const LHCb::Particle*>  TriggerSelectionTisTos::matchedTOSParticles( std::string selectionName )
{
  std::vector<const  LHCb::Particle*> matchedParticles;

  getHltSummary();
  if( (m_summary==NULL)||(m_hltconf==NULL) )return matchedParticles;

  int selID = HltConfigurationHelper::getID(*m_hltconf,"SelectionID",selectionName);

  bool decision= m_summary->hasSelectionSummary( selID );
  if( !decision )return matchedParticles;

  const LHCb::HltSelectionSummary& sel = m_summary->selectionSummary( selID );

  decision= sel.decision();
  if( !decision )return matchedParticles;

  const SmartRefVector< LHCb::Particle > & onparticles = sel.particles();  

  if( onparticles.size() <= 0 )return matchedParticles;
    
  std::vector<double> aveovlp;

    for(SmartRefVector<LHCb::Particle>::const_iterator onp = onparticles.begin();onp != onparticles.end(); ++onp) {
      const Particle* onparticle = *onp;
      std::vector<const LHCb::Particle*> fdaughters= finalStateParticles( onparticle );
      bool thisTOS(true); double ovlp(0.0); int n(0);
      for(std::vector<const LHCb::Particle*>::const_iterator pfp =fdaughters.begin(); pfp!=fdaughters.end(); ++pfp){
        const ProtoParticle* pp = (*pfp)->proto();
        if( 0!=pp ){
          const Track* onit=pp->track();
          if( 0!=onit ){
            const Track& ontrack = *onit;
            double overlap[nHitTypes];
            matchIDs(ontrack,m_offlineInput,overlap);
            if( m_muonTracks == NULL ){ overlap[kMuon]=2.0; }
            if( thisTOS ){
               // effectively .and. requirement between all types
              for( int hitType=0; hitType!=nHitTypes; ++hitType ){
                thisTOS= overlap[hitType] >= m_TOSFrac[hitType];
                if( !thisTOS )break;
                if( overlap[hitType] <1.1 ){ 
                  ++n;	   
                  ovlp+=overlap[hitType];
                }
              }
              if( !thisTOS )break;
            }
          }
        }
      }
      if( thisTOS && (n>0) ){
        ovlp = ovlp/n;
        aveovlp.push_back(ovlp);
        matchedParticles.push_back(*onp);
      }
    }

  if( aveovlp.size()>1 ){
    bool swap(true);
    while(swap){
      swap=false;
      for( unsigned int i1=0; i1<(aveovlp.size()-1);++i1 ){
        for( unsigned int i2=i1+1; i2<aveovlp.size();++i2) {
          if( aveovlp[i1]<aveovlp[i2] ){
            double tmp=aveovlp[i1]; aveovlp[i1]=aveovlp[i2]; aveovlp[i2]=tmp;
            const Particle* ptmp=matchedParticles[i1];matchedParticles[i1]=matchedParticles[i2];matchedParticles[i2]=ptmp;
            swap=true;
          }
        }
      }
    }
  }

  return matchedParticles;
}

 
