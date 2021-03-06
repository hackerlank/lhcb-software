

//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information:
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtLb2Baryonlnu.cc
//
// Description: Routine to implement Lb->N*+ l nu semileptonic decays using form factor predictions based on the quark model.  The form factors are from W. Roberts, M. Pervin, S. Chapstick, (2011). arXiv:nucl-th/0503030v1.  The model can be used for decays to all N*+ states with J^{P} = 1/2^{+}, 1/2^{-}, 3/2^{+}, 3/2^{-} and, in addition, decays to p, Lc+, Lc(2593)+, Lc(2625)+
//              
//
// Modification history:
//
//   William Sutcliffe     21/11/2013        Module created
//
//
//------------------------------------------------------------------------

#include "EvtGenBase/EvtPatches.hh"
#include <stdlib.h>
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenModels/EvtLb2Baryonlnu.hh"
#include "EvtGenBase/EvtConst.hh"
#include "EvtGenBase/EvtIdSet.hh"
#include <string>
#include "EvtGenModels/EvtLb2BaryonlnuFF.hh"

using namespace std;
#ifdef D0
#undef D0
#endif
EvtLb2Baryonlnu::EvtLb2Baryonlnu():
  ffmodel(0)
  ,calcamp(0)
{}


EvtLb2Baryonlnu::~EvtLb2Baryonlnu() {
  delete ffmodel;
  ffmodel=0;
  delete calcamp;
 calcamp=0;
}

std::string EvtLb2Baryonlnu::getName(){
  
  return "Lb2Baryonlnu";     
  
}



EvtDecayBase* EvtLb2Baryonlnu::clone(){
  
  return new EvtLb2Baryonlnu;
  
}

void EvtLb2Baryonlnu::decay( EvtParticle *p ){
  
  //This is a kludge to avoid warnings because the K_2* mass becomes to large.
  static EvtIdSet regenerateMasses("K_2*+","K_2*-","K_2*0","anti-K_2*0",
				   "K_1+","K_1-","K_10","anti-K_10",
				   "D'_1+","D'_1-","D'_10","anti-D'_10");
  
  if (regenerateMasses.contains(getDaug(0))){
    p->resetFirstOrNot();
  }
  
  p->initializePhaseSpace(getNDaug(),getDaugs());
  
  EvtComplex r00(getArg(0), 0.0 );
  EvtComplex r01(getArg(1), 0.0 );
  EvtComplex r10(getArg(2), 0.0 );
  EvtComplex r11(getArg(3), 0.0 );

  calcamp->CalcAmp(p,_amp2,ffmodel, r00, r01, r10, r11);
  
}

void EvtLb2Baryonlnu::initProbMax() {


  static EvtId LAMB=EvtPDL::getId("Lambda_b0");
  static EvtId LAMBB=EvtPDL::getId("anti-Lambda_b0");
  static EvtId PRO=EvtPDL::getId("p+");
  static EvtId PROB=EvtPDL::getId("anti-p-");
  static EvtId N1440=EvtPDL::getId("N(1440)+");
  static EvtId N1440B=EvtPDL::getId("anti-N(1440)-");
  static EvtId N1535=EvtPDL::getId("N(1535)+");
  static EvtId N1535B=EvtPDL::getId("anti-N(1535)-");
  static EvtId N1520=EvtPDL::getId("N(1520)+");
  static EvtId N1520B=EvtPDL::getId("anti-N(1520)-");
  static EvtId N1720=EvtPDL::getId("N(1720)+");
  static EvtId N1720B=EvtPDL::getId("anti-N(1720)-");
  static EvtId N1650=EvtPDL::getId("N(1650)+");
  static EvtId N1650B=EvtPDL::getId("anti-N(1650)-");
  static EvtId N1700=EvtPDL::getId("N(1700)+");
  static EvtId N1700B=EvtPDL::getId("anti-N(1700)-");
  static EvtId N1710=EvtPDL::getId("N(1710)+");
  static EvtId N1710B=EvtPDL::getId("anti-N(1710)-");
  static EvtId N1875=EvtPDL::getId("N(1875)+");
  static EvtId N1875B=EvtPDL::getId("anti-N(1875)-");
  static EvtId N1900=EvtPDL::getId("N(1900)+");
  static EvtId N1900B=EvtPDL::getId("anti-N(1900)-");
  static EvtId LAMCP=EvtPDL::getId("Lambda_c+");
  static EvtId LAMCM=EvtPDL::getId("anti-Lambda_c-");
  static EvtId LAMC1P=EvtPDL::getId("Lambda_c(2593)+");
  static EvtId LAMC1M=EvtPDL::getId("anti-Lambda_c(2593)-");
  static EvtId LAMC2P=EvtPDL::getId("Lambda_c(2625)+");
  static EvtId LAMC2M=EvtPDL::getId("anti-Lambda_c(2625)-");
  
  EvtId parnum,barnum;
  
  parnum = getParentId();
  barnum = getDaug(0);

  if( (parnum==LAMB && barnum==PRO) || (parnum==LAMBB && barnum==PROB) ||
    (parnum==LAMB && barnum==N1440) || (parnum==LAMBB && barnum==N1440B) ||
    (parnum==LAMB && barnum==N1520) || (parnum==LAMBB && barnum==N1520B) ||
    (parnum==LAMB && barnum==N1535) || (parnum==LAMBB && barnum==N1535B) ||
    (parnum==LAMB && barnum==N1720) || (parnum==LAMBB && barnum==N1720B) ||
    (parnum==LAMB && barnum==N1650) || (parnum==LAMBB && barnum==N1650B) ||
    (parnum==LAMB && barnum==N1700) || (parnum==LAMBB && barnum==N1700B) ||
    (parnum==LAMB && barnum==N1710) || (parnum==LAMBB && barnum==N1710B) ||
    (parnum==LAMB && barnum==N1875) || (parnum==LAMBB && barnum==N1875B) ||
    (parnum==LAMB && barnum==N1900) || (parnum==LAMBB && barnum==N1900B) ||
    (parnum==LAMB && barnum==LAMCP) || (parnum==LAMBB && barnum==LAMCM) ||
    (parnum==LAMB && barnum==LAMC1P) || (parnum==LAMBB && barnum==LAMC1M) ||
    (parnum==LAMB && barnum==LAMC2P) || (parnum==LAMBB && barnum==LAMC2M)  
    ) 
  {
      setProbMax(22000.0);
  }
  else
  {
    report(ERROR,"EvtGen") << "Decay does not have acceptable final state baryon for this model setting ProbMax = 0 "
			   <<endl;
    setProbMax(0.0);
  }
  
}

void EvtLb2Baryonlnu::init(){
  
  if (getNArg()!=4) {

    report(ERROR,"EvtGen") << "EvtLb2Baryonlnu generator expected "
			   << " 4 arguments but found:"<<getNArg()<<endl;
    report(ERROR,"EvtGen") << "Will terminate execution!"<<endl;
    ::abort();

  }

  if ( getNDaug()!=3 ) {
     report(ERROR,"EvtGen") 
       << "Wrong number of daughters in EvtLb2plnu.cc " 
       << " 3 daughters expected but found: "<<getNDaug()<<endl;
     report(ERROR,"EvtGen") << "Will terminate execution!"<<endl;
     ::abort();
  }


  //We expect the parent to be a dirac particle 
  //and the daughters to be X lepton neutrino

  EvtSpinType::spintype parenttype=EvtPDL::getSpinType(getParentId());
  EvtSpinType::spintype baryontype=EvtPDL::getSpinType(getDaug(0));
  EvtSpinType::spintype leptontype=EvtPDL::getSpinType(getDaug(1));
  EvtSpinType::spintype neutrinotype=EvtPDL::getSpinType(getDaug(2));

  if ( parenttype != EvtSpinType::DIRAC ) {
    report(ERROR,"EvtGen") << "EvtLb2Baryonlnu generator expected "
                           << " a DIRAC parent, found:"<<
                           EvtPDL::name(getParentId())<<endl;
    report(ERROR,"EvtGen") << "Will terminate execution!"<<endl;
    ::abort();
  }
  if ( leptontype != EvtSpinType::DIRAC ) {
    report(ERROR,"EvtGen") << "EvtLb2Baryonlnu generator expected "
                           << " a DIRAC 2nd daughter, found:"<<
                           EvtPDL::name(getDaug(1))<<endl;
    report(ERROR,"EvtGen") << "Will terminate execution!"<<endl;
    ::abort();
  }
  if ( neutrinotype != EvtSpinType::NEUTRINO ) {
    report(ERROR,"EvtGen") << "EvtLb2Baryonlnu generator expected "
                           << " a NEUTRINO 3rd daughter, found:"<<
                           EvtPDL::name(getDaug(2))<<endl;
    report(ERROR,"EvtGen") << "Will terminate execution!"<<endl;
    ::abort();
  }

  //set ffmodel
  ffmodel = new EvtLb2BaryonlnuFF;

  if ( baryontype==EvtSpinType::DIRAC
	  || baryontype == EvtSpinType::RARITASCHWINGER) 
  { 
    calcamp = new EvtSLBaryonAmp; 
  }
  else {
    report(ERROR,"EvtGen") 
      << "Wrong baryon spin type in EvtLb2Baryonlnu.cc " 
      << "Expected spin type " << EvtSpinType::DIRAC 
      << ", found spin type " << baryontype <<endl;
    report(ERROR,"EvtGen") << "Will terminate execution!" <<endl;
     ::abort();
  }
  
}

