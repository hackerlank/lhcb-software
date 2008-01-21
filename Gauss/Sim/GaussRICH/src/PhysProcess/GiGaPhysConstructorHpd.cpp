// $Id: GiGaPhysConstructorHpd.cpp,v 1.10 2008-01-21 16:56:57 seaso Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/PropertyMgr.h"

// G4 
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleWithCuts.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTable.hh"
#include "G4Material.hh"
#include "G4ios.hh"
#include "G4Transportation.hh"
#include "G4MultipleScattering.hh"
#include "G4LossTableManager.hh"

// local
#include "GiGaPhysConstructorHpd.h"
#include "RichPhotoElectron.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GiGaPhysConstructorHpd
//
// 2002-12-12 : Witek Pokorski
// 2003-04-29 : Sajan Easo
// 2007-01-11 : Gloria Corti, modified for Gaudi v19
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GiGaPhysConstructorHpd );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GiGaPhysConstructorHpd::GiGaPhysConstructorHpd
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaPhysConstructorBase( type , name , parent ),
    m_RichHpdSiDetEfficiency(0.85),
    m_RichHpdPixelChipEfficiency(1.0),
    m_RichHpdPeBackScatterProb(0.005823) /*RWL change 8th Nov 06*/ 
{ 
  declareProperty("RichHpdSiDetEfficiency", m_RichHpdSiDetEfficiency);
  declareProperty("RichHpdPixelChipEfficiency", m_RichHpdPixelChipEfficiency);
  declareProperty("RichHpdBackScatterProb" , m_RichHpdPeBackScatterProb);

 }


//=============================================================================
// Destructor
//=============================================================================
GiGaPhysConstructorHpd::~GiGaPhysConstructorHpd(){}


//=============================================================================
// ConstructParticle
//=============================================================================
void GiGaPhysConstructorHpd::ConstructParticle()
{
      RichPhotoElectron::PhotoElectronDefinition(); 

}

//=============================================================================
// ConstructProcess
//=============================================================================
void GiGaPhysConstructorHpd::ConstructProcess()
{  
  ConstructPeGenericProcess();
  ConstructHpdSiEnLoss();

}

//=============================================================================
// ConstructPeGenericProcess
//=============================================================================
void GiGaPhysConstructorHpd::ConstructPeGenericProcess() {
  // first remove any process assigned to this particle from elsewhere.
  // then add the Transportation, HpdSiEnergyloss process to it.
  // More processes to be added later.
  MsgStream msg(msgSvc(), name());

  //        G4double aPeCut=10.0*km;
  // G4double aPeCut=0.1*mm;
  //  G4ParticleDefinition* photoelectronDef = 
  //  RichPhotoElectron::PhotoElectron();
  G4Transportation* theTransportationProcess= new G4Transportation();
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    if(  particle->GetParticleName() == "pe-" )
      {
        G4ProcessManager* pmanager =  particle->GetProcessManager();
        G4ProcessVector* pVector = 
          (particle->GetProcessManager())->GetProcessList();
        // msg << MSG::DEBUG << "size of ProcList for pe-  so far  "
        //  <<(G4int)  pVector->size() << endreq;
        if( (G4int)  pVector->size() > 0 ) { 
          //  msg << MSG::DEBUG 
          //  <<" For pe-  disassociating following processes "<<  endreq; 
          //  pmanager->DumpInfo();
          for(G4int ip=0; ip < (G4int)  pVector->size() ; ++ip ){
       	    pmanager->RemoveProcess(ip);
          }
          //  msg << MSG::DEBUG <<" For pe- end of process cleanup "<<  endreq; 
          pmanager ->AddProcess(theTransportationProcess,-1,1,2);
         } else {
           //  msg << MSG::DEBUG 
           //   <<"  pe-  only has no process so far"<<  endreq;
        
           pmanager ->AddProcess(theTransportationProcess,-1,1,2);          
         }
        
        
        //  pmanager->DumpInfo();
          //particle->SetCuts(aPeCut);
          // particle->SetApplyCutsFlag(true);
        // particle-> DumpTable() ;
      }
  }
}

//=============================================================================
#include "RichHpdSiEnergyLoss.h"

//=============================================================================
// ConstructHpdSiEnLoss
//=============================================================================
void GiGaPhysConstructorHpd::ConstructHpdSiEnLoss()
{
  // Add Decay Process
  //  G4Decay* theDecayProcess = new G4Decay();

  RichHpdSiEnergyLoss* theRichHpdSiEnergyLossProcess =
    new RichHpdSiEnergyLoss("RichHpdSiEnergyLossProcess", fUserDefined );
  theRichHpdSiEnergyLossProcess->setHpdSiDetEff ( m_RichHpdSiDetEfficiency);
  theRichHpdSiEnergyLossProcess->setHpdSiPixelChipEff (m_RichHpdPixelChipEfficiency);
  theRichHpdSiEnergyLossProcess->setHpdPeBackScaProb(m_RichHpdPeBackScatterProb);
  theRichHpdSiEnergyLossProcess->InitializeHpdProcParam();
  

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    //    G4cout<<"ConstructHpdSiEnLoss: Now at particle:  "<< particle->GetParticleName()<<G4endl;
    
    pmanager->SetVerboseLevel(0);
    
    if( theRichHpdSiEnergyLossProcess->IsApplicable(*particle) ) 
      {
        //    RichHpdSiEnergyLoss* theRichHpdSiEnergyLossProcess =
        //  new RichHpdSiEnergyLoss("RichHpdSiEnergyLossProcess");
        //  G4cout<<"Now HpdSiLoss adding for "<< particle->GetParticleName()<<G4endl;
        
	// add this process for any charged particle.

	//    if( ( particle->GetParticleName() == "pe-") || ( particle->GetParticleName() =="e-" ))
        //  {
            //            G4cout<<"Now at particle "<< particle->GetParticleName()<<G4endl;
            pmanager->AddProcess( theRichHpdSiEnergyLossProcess ,-1,2,2);
	    //   }
        
	    //  if(particle->GetParticleName() == "pe-")
	    //  {
              //   G4cout<<" All the processes for pe- " <<G4endl;
              //  pmanager->DumpInfo();  
            
            //  (RichPhotoElectron::PhotoElectron())->SetProcessManager(pmanager);
            // (RichPhotoElectron::PhotoElectron())->GetProcessManager()->DumpInfo();
	    //  }
      }
    //    if (theDecayProcess->IsApplicable(*particle)) { 
    //      pmanager ->AddProcess(theDecayProcess);
    // set ordering for PostStepDoIt and AtRestDoIt
    //      pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep);
    //     pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
    //}
  }
}

//=============================================================================
