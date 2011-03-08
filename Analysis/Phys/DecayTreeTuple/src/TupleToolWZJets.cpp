#include "GaudiKernel/ToolFactory.h"
#include "TupleToolWZJets.h"
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "Event/Particle.h"
#include "Kernel/IJetMaker.h"
#include "LoKi/AParticleCuts.h"
#include "boost/lexical_cast.hpp"
#include "boost/assign.hpp"
#include <iterator>
#include <cmath>
#include <map>

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolWZJets
// Autor: Albert Bursche
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolWZJets )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolWZJets::TupleToolWZJets( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : TupleToolBase ( type, name , parent ),
    m_ppSvc(NULL),
    m_magic(528),
    charge(LoKi::Cuts::ONE),
    positiveParticles(LoKi::Cuts::ONE ),
    negativeParticles(LoKi::Cuts::ONE ),
    neutralParticles(LoKi::Cuts::ONE ),
    maxPT(LoKi::Cuts::ONE ),
    m_M(LoKi::Cuts::ONE ),
    m_MM(LoKi::Cuts::ONE ),
    m_DPHI(LoKi::Cuts::ONE ),
    m_DETA(LoKi::Cuts::ONE ),
    m_DR2(LoKi::Cuts::ONE )
{
  declareInterface<IParticleTupleTool>(this);
  declareProperty( "BaseName", m_BaseName = "jet",
                   "Configure this if you want to run this tool more than once in a job." );
  declareProperty( "ParticleContainer",m_PartContainer = std::vector<std::string>(),
                   "Where to find the particles to make the jets of" );
  declareProperty( "MaxJets",m_MaxJets = 4,
                   "Maximal number of jets to be stored in the tuple" );
  declareProperty( "IsoJetAbsID",m_IsoJetAbsID = false,
                   "Use AbsID for naming the isojet variables (change this if you get spuriuos segfaults)" );
}

//=============================================================================
StatusCode TupleToolWZJets::initialize()
{
  if( ! TupleToolBase::initialize() )
    return StatusCode::FAILURE;
  // PP Svc
  m_ppSvc = svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc", true );
  //get LoKi objects
  m_AdditionalJetMaker = tool<IJetMaker>( "LoKi::FastJetMaker","AdditionalJetMaker", this );
  m_IsoJetMaker = tool<IJetMaker>( "LoKi::FastJetMaker","IsoJetMaker", this );
  m_LokiAddJetFilter = tool<IParticleFilter>("LoKi::Hybrid::FilterCriterion","AdditionalJetsFilter",this);
  m_LokiIsoJetFilter = tool<IParticleFilter>("LoKi::Hybrid::FilterCriterion","IsolationJetsFilter",this);
  charge = LoKi::Cuts::SUMTREE (LoKi::Cuts::Q, LoKi::Cuts::ALL, 0.0 ) ;
  positiveParticles = LoKi::Cuts::NINTREE (  LoKi::Cuts::Q > 0 );
  negativeParticles = LoKi::Cuts::NINTREE (  LoKi::Cuts::Q < 0 );
  neutralParticles  = LoKi::Cuts::NINTREE (  LoKi::Cuts::Q == 0 );
  maxPT = LoKi::Cuts::MAXTREE(LoKi::Cuts::PT,LoKi::Cuts::BASIC,-1);
  m_M = LoKi::Particles::Mass();
  m_MM = LoKi::Particles::MeasuredMass();
  return StatusCode::SUCCESS ;
}
StatusCode TupleToolWZJets::fill( const LHCb::Particle *top,
                                  const LHCb::Particle* P,
                                  const std::string& /* head */,
                                  Tuples::Tuple& tuple )
{
  m_tuple = &tuple;
  m_prefix=m_BaseName;//+fullName(head);

  //intitalise LoKi comparators to reference particle
  m_DPHI = LoKi::Cuts::DPHI(P->momentum().Phi());
  m_DETA = LoKi::Cuts::DETA(P->momentum().Eta());
  m_DR2  = LoKi::Cuts::DR2(P->momentum());
  bool test = true;
  LHCb::Particles& myParts =GetParticles();
  
  // Get particles in decay
  m_decayParticles.clear();
  if ( msgLevel(MSG::DEBUG) ) debug() << "Filling particle with ID " << top->particleID().pid() << endmsg;
  m_decayParticles.push_back( P->clone() );
  SaveDecayChainParticles( P );
   IJetMaker::Jets AddJets; //jets in the event additional to the particle in question
  IJetMaker::Jets IsoJets; //jets containg the particle in question. Used to make an isolation criterion
  StatusCode scAdd = m_AdditionalJetMaker->makeJets(myParts.begin(), myParts.end(),AddJets );
  if ( msgLevel(MSG::DEBUG) ) debug() << "Recieved" << AddJets.size()  << "additional jets from the jet algorithm."<< endmsg;
  AddDecProducts(myParts);
  StatusCode scIso = m_IsoJetMaker->makeJets(myParts.begin(), myParts.end(), IsoJets);
  if ( msgLevel(MSG::DEBUG) ) debug() << "Recieved " << AddJets.size()  << " isolation jet canditates from the jet algorithm."<< endmsg;
 
  // match IsoJets to particles
  test &= StoreAdditionalJets(AddJets);
  test &= MatchAndStoreIsoJets(IsoJets);
 
  if (!exist<LHCb::Particles>("/Event/Phys/"+m_BaseName+"IsoCollection/Particles"))
    put(&myParts, "/Event/Phys/"+m_BaseName+"IsoCollection/Particles");
  else
    if(msgLevel(MSG::WARNING))
      warning() <<  "Cannot write to TES at /Event/Phys/"+m_BaseName+"IsoCollection/Particles" << endmsg;
  if (!exist< LHCb::Particles>("/Event/Phys/"+m_BaseName+"AddJets/Particles"))
  {
    LHCb::Particles * jets =new  LHCb::Particles();
    for ( IJetMaker::Jets::iterator iJet = AddJets.begin() ; AddJets.end() != iJet ; ++iJet )
      jets->insert(*iJet);
    put(jets, "/Event/Phys/"+m_BaseName+"AddJets/Particles");
  }
  else
    if(msgLevel(MSG::WARNING))
      warning() <<  "Cannot write to TES at /Event/Phys/"+m_BaseName+"AddJets/Particles" << endmsg;
  if (!exist< LHCb::Particles>("/Event/Phys/"+m_BaseName+"IsoJets/Particles"))
  {
    LHCb::Particles * jets =new  LHCb::Particles();
    for ( IJetMaker::Jets::iterator iJet = IsoJets.begin() ; IsoJets.end() != iJet ; ++iJet )
      jets->insert(*iJet);
    put(jets, "/Event/Phys/"+m_BaseName+"IsoJets/Particles");
  }
  else
    if(msgLevel(MSG::WARNING))
      warning() << "Cannot write to TES at /Event/Phys/"+m_BaseName+"IsoJets/Particles" << endmsg;
  //In case the objects cannot be saved in TES some memory gets lost! 
  //But I can not call delete on these objects yet because I don't know what happens later.

  return StatusCode(test);
}

//=============================================================================
// Save the particles in the decay chain (recursive function)
//=============================================================================
void TupleToolWZJets::SaveDecayChainParticles( const LHCb::Particle *top){

  // -- Get the daughters of the top particle
  const SmartRefVector< LHCb::Particle > daughters = top->daughters();

  // -- Fill all the daugthers in m_decayParticles
  for( SmartRefVector< LHCb::Particle >::const_iterator idau = daughters.begin() ; idau != daughters.end() ; ++idau){

    // -- If the particle is stable, save it in the vector, or...
    if( (*idau)->isBasicParticle() ){
      if ( msgLevel(MSG::DEBUG) ) debug() << "Filling particle with ID " << (*idau)->particleID().pid() << endmsg;
      m_decayParticles.push_back( (*idau)->clone() );
    }else{
      // -- if it is not stable, call the function recursively
      m_decayParticles.push_back( (*idau)->clone() );
      if ( msgLevel(MSG::DEBUG) ) debug() << "Filling particle with ID " << (*idau)->particleID().pid() << endmsg;
      SaveDecayChainParticles( (*idau) );
    }
  }
}

//=============================================================================
// Check if the particle is already in the decay
//=============================================================================
bool TupleToolWZJets::isParticleInDecay(const LHCb::Particle* part){
  bool isInDecay = false;
  const LHCb::ProtoParticle* proto1 = part->proto();
  const LHCb::Track* track = proto1->track();
  for(  std::vector<LHCb::Particle*>::iterator it = m_decayParticles.begin() ; it != m_decayParticles.end() ; ++it ){
    const LHCb::ProtoParticle* proto = (*it)->proto();
    if(proto){
      const LHCb::Track* myTrack = proto->track();
      if(myTrack == track){
        if ( msgLevel(MSG::DEBUG) ) debug() << "Track is in decay, skipping it" << endmsg;
        isInDecay = true;
      }
    }
  }
  return isInDecay;
}

#define SAVEPOINT(POINT,FUN)                    \
  (POINT?POINT->FUN:-1.0)

bool TupleToolWZJets::WriteJetToTuple(const LHCb::Particle*jet,std::string prefix)
{
  //filter plus and minus signs out to the prefix
  std::map<std::string,std::string> replacements ;
  replacements["+"]="plus";
  replacements["-"]="minus";
  for (std::map<std::string,std::string>::iterator it = replacements.begin();it != replacements.end();++it)
  {
    size_t point = 0;
    while((point = prefix.find(it->first))!=std::string::npos)
      prefix = prefix.replace(point,it->second.size(),it->second);
  }

  if ( msgLevel(MSG::DEBUG) ) debug() << (jet?"Filling jet object ":"Filling dummy jet object ") << prefix << endmsg;
  //std::cout << prefix << std::endl;
  bool result = true;
  result &= (*m_tuple)->column( prefix+"_PX",  SAVEPOINT(jet,momentum().Px()));
  result &= (*m_tuple)->column( prefix+"_PY",  SAVEPOINT(jet,momentum().Py()));
  result &= (*m_tuple)->column( prefix+"_PZ",  SAVEPOINT(jet,momentum().Pz()));
  result &= (*m_tuple)->column( prefix+"_PE",  SAVEPOINT(jet,momentum().e()));
  result &= (*m_tuple)->column( prefix+"_PT",  SAVEPOINT(jet,momentum().Pt()));
  result &= (*m_tuple)->column( prefix+"_P",   SAVEPOINT(jet,momentum().P()));
  result &= (*m_tuple)->column( prefix+"_Eta", SAVEPOINT(jet,momentum().Eta()));
  result &= (*m_tuple)->column( prefix+"_Phi", SAVEPOINT(jet,momentum().Phi()));
  result &= (*m_tuple)->column( prefix+"_M", SAVEPOINT(jet,momentum().mass()));
  result &= (*m_tuple)->column( prefix+"_MLoKi",jet?m_M(jet):-99.0);
  result &= (*m_tuple)->column( prefix+"_MMLoKi", jet?m_MM(jet):-99.0);
  if (m_verbose)
    {
      result &= (*m_tuple)->column( prefix+"_ParticleMultiplicity",  SAVEPOINT(jet,daughters().size()));
      result &= (*m_tuple)->column( prefix+"_Charge", jet?charge(jet):-1.0);
      result &= (*m_tuple)->column( prefix+"_positiveParticleMultiplicity", jet?positiveParticles(jet):-1.0);
      result &= (*m_tuple)->column( prefix+"_negativeParticleMultiplicity", jet?negativeParticles(jet):-1.0);
      result &= (*m_tuple)->column( prefix+"_neutralParticleMultiplicity", jet?neutralParticles(jet):-1.0);
      result &= (*m_tuple)->column( prefix+"_chargedParticleMultiplicity", jet?positiveParticles(jet)+negativeParticles(jet):-1.0);
      result &= (*m_tuple)->column( prefix+"_maxPT", jet?maxPT(jet):-1.0);
      result &= (*m_tuple)->column( prefix+"_DETA", jet?m_DETA(jet):-10.0);
      result &= (*m_tuple)->column( prefix+"_DPHI", jet?m_DPHI(jet):-10.0);
      result &= (*m_tuple)->column( prefix+"_DR", jet?sqrt(m_DR2(jet)):-1.0);
      SmartRefVector< LHCb::Particle > SortedDaughters; 
      //buffer to store intermediate result to speed things up a bit
      result &= (*m_tuple)->column( prefix+"_PT2",  jet?MaxSumNPart(jet,2, LoKi::Cuts::PT,&SortedDaughters):-1.0);
      result &= (*m_tuple)->column( prefix+"_PT3",  jet?MaxSumNPart(jet,3, LoKi::Cuts::PT,&SortedDaughters):-1.0);
      result &= (*m_tuple)->column( prefix+"_PT4",  jet?MaxSumNPart(jet,4, LoKi::Cuts::PT,&SortedDaughters):-1.0);
      result &= (*m_tuple)->column( prefix+"_PT5",  jet?MaxSumNPart(jet,5, LoKi::Cuts::PT,&SortedDaughters):-1.0);
      result &= (*m_tuple)->column( prefix+"_PT10", jet?MaxSumNPart(jet,10,LoKi::Cuts::PT,&SortedDaughters):-1.0);
      result &= (*m_tuple)->column( prefix+"_PT15", jet?MaxSumNPart(jet,15,LoKi::Cuts::PT,&SortedDaughters):-1.0);
      result &= (*m_tuple)->column( prefix+"_PT25", jet?MaxSumNPart(jet,25,LoKi::Cuts::PT,&SortedDaughters):-1.0);
    }
  return result;
}

double TupleToolWZJets::MaxSumNPart(const LHCb::Particle* jet,unsigned int n,const LoKi::Types::Fun& fun,SmartRefVector< LHCb::Particle >* SortedDaughters)
{
  //  fun == LoKi::Cuts::ONE;
  if (jet->daughters().size() <= n)
    return LoKi::Cuts::SUMTREE ( LoKi::Cuts::BASIC, fun)(jet);
  if (SortedDaughters&&SortedDaughters->size()) // Vector given and filled
    return LoKi::Cuts::ASUM (fun)(LHCb::Particle::ConstVector(SortedDaughters->begin(),SortedDaughters->begin()+n));
  SmartRefVector< LHCb::Particle > daughters; //empty vector to take address of in case SortedDaughters == NULL
  if (!SortedDaughters)  
    SortedDaughters = &daughters;
  SortedDaughters->assign(jet->daughters().begin(),jet->daughters().end()); 
  sort(SortedDaughters->begin(),SortedDaughters->end(),Comperator<const LHCb::Particle*,const LHCb::Particle*>(fun));
  return LoKi::Cuts::ASUM (fun)(LHCb::Particle::ConstVector(SortedDaughters->begin(),SortedDaughters->begin()+n));
}

LHCb::Particles& TupleToolWZJets::GetParticles()
{
  // -- Get all particles in the event to cluster jets
  LHCb::Particle::ConstVector inputParts;
  for(std::vector<std::string>::const_iterator InputContainer= m_PartContainer.begin();
      InputContainer!=m_PartContainer.end();++InputContainer)
  {
    if ( msgLevel(MSG::DEBUG) ) debug() << "Getting particles from: " << *InputContainer << endmsg;
    if (exist<LHCb::Particle::Range>(*InputContainer))
    {
      LHCb::Particle::Range  addInp = get<LHCb::Particle::Range>(*InputContainer);
      inputParts.insert(inputParts.begin(),addInp.begin(),addInp.end());
    }
  }
  //filter the decay products from the collection 
  //this filter only looks for overlag with the decay products 
  //(other filters can be applied with a filter desktop before running the tuple)
  LHCb::Particles&  myParts = *new  LHCb::Particles();
  std::map<int,int> particleCharges = boost::assign::map_list_of(-1,0)(0,0)(1,0); //counter
  for(LHCb::Particle::ConstVector::const_iterator parts = inputParts.begin(); parts != inputParts.end(); ++parts)
    if (!isParticleInDecay(*parts))
    {
      if ( msgLevel(MSG::DEBUG) ) debug() << "Adding Particle of type " << (*parts)->particleID ()  << " to the collection."<< endmsg;
      myParts.insert(new LHCb::Particle(**parts));
      particleCharges[(*parts)->charge()]++;
    }
  if (m_verbose)
    {
      (*m_tuple)->column( m_prefix+"_ChargedTracksUsed",particleCharges[-1]+particleCharges[+1]);
      (*m_tuple)->column( m_prefix+"_NeutralObjectsUsed",particleCharges[0]);
      (*m_tuple)->column( m_prefix+"_PositiveTracksUsed",particleCharges[+1]);
      (*m_tuple)->column( m_prefix+"_NegativeTracksUsed",particleCharges[-1]);
    }
  
  return myParts;
}
void TupleToolWZJets::AddDecProducts(LHCb::Particles& myParts)
{
  int PartID = 0; // counter for decay product (needed later when matching IsoJets)
  for( std::vector< LHCb::Particle* >::iterator idau = m_decayParticles.begin() ;
       idau != m_decayParticles.end() ; ++idau)
    if((*idau)->isBasicParticle())
      {
        (*idau)->addInfo(LHCb::Particle::LastGlobal + m_magic,PartID++);
        //myParts.push_back(&**idau);
        myParts.insert(new LHCb::Particle(**idau));
      }
  if ( msgLevel(MSG::DEBUG) ) debug() << "Added " << PartID  << " particles to the collection."<< endmsg;
}
bool TupleToolWZJets::StoreAdditionalJets(const IJetMaker::Jets& AddJets)
{
  bool test = true;
  unsigned int numJets = 0;
  for ( IJetMaker::Jets::const_iterator iJet = AddJets.begin() ; AddJets.end() != iJet ; ++iJet )
    if((*m_LokiAddJetFilter)(*iJet))
      {
	numJets = numJets+1;
	if (numJets <=  m_MaxJets)
        test &= WriteJetToTuple(*iJet,m_prefix+"_Jet"+ boost::lexical_cast<std::string>(numJets));
      }
    else
      if ( msgLevel(MSG::DEBUG) ) debug() << "Jet number " << std::distance( (IJetMaker::Jets::const_iterator) AddJets.begin(),iJet)  << " failed selection."<< endmsg;
    for(unsigned int i = numJets+1;i<=m_MaxJets;i++)// fill remaining jets slots in tuple with dummys
      test &= WriteJetToTuple(NULL,m_prefix+"_Jet"+ boost::lexical_cast<std::string>(i));
    test &= (*m_tuple)->column( m_prefix+"_NumJets", numJets);
    return test;
}
bool TupleToolWZJets::MatchAndStoreIsoJets(const IJetMaker::Jets& IsoJets)
{
  bool test = true; 
  std::vector<bool> isoJetFound(m_decayParticles.size()) ;
  for (std::vector<bool>::iterator it = isoJetFound.begin();it!=isoJetFound.end();++it)
    *it=false;
  for ( IJetMaker::Jets::const_iterator iJet = IsoJets.begin() ; IsoJets.end() != iJet ; ++iJet )
    {
      std::vector<bool>::iterator ThisIsoJetFound = isoJetFound.begin();
      for (std::vector<LHCb::Particle*>::const_iterator DecProduct=m_decayParticles.begin();
	   DecProduct!=m_decayParticles.end();
	   ++DecProduct)
	{
	  for(SmartRefVector< LHCb::Particle >::const_iterator daughter = (*iJet)->daughters().begin();
	      daughter!=(*iJet)->daughters().end();++daughter )
	    if((*DecProduct)->info(LHCb::Particle::LastGlobal + m_magic, 1000)==
	       (*daughter)  ->info(LHCb::Particle::LastGlobal + m_magic,-1000)&&
	       (*m_LokiIsoJetFilter)(*iJet))
	      {
		if (m_IsoJetAbsID)
		  test &= WriteJetToTuple(*iJet,m_BaseName+"IsoJet"+m_ppSvc->find((LHCb::ParticleID)((*DecProduct)->particleID().abspid()))->name());
		else
		  test &= WriteJetToTuple(*iJet,m_BaseName+"IsoJet"+m_ppSvc->find((LHCb::ParticleID)((*DecProduct)->particleID()))->name());
		if ((*ThisIsoJetFound)&&msgLevel(MSG::WARNING))
		  warning() << "Found more than one IsoJet!" << endmsg;
		*ThisIsoJetFound = true;
	      }
	  ++ThisIsoJetFound;
	}
    }
  {
    std::vector<bool>::iterator ThisIsoJetFound = isoJetFound.begin();
    for (std::vector<LHCb::Particle*>::const_iterator DecProduct=m_decayParticles.begin();
         DecProduct!=m_decayParticles.end();
         ++DecProduct)
      if (!*ThisIsoJetFound++)//write default value to tuple
      {
        if (m_IsoJetAbsID)
          test &= WriteJetToTuple(NULL,m_BaseName+"IsoJet"+m_ppSvc->find((LHCb::ParticleID)((*DecProduct)->particleID().abspid()))->name());
        else
          test &= WriteJetToTuple(NULL,m_BaseName+"IsoJet"+m_ppSvc->find((LHCb::ParticleID)((*DecProduct)->particleID()))->name());
        if(msgLevel(MSG::DEBUG))
          debug() << "IsoJet missing!" << endmsg;
      }
  }
  return test;
}
