// $Id: DeMuonDetector.cpp,v 1.4 2005-11-04 16:05:29 asarti Exp $

// Include files
#include "MuonDet/DeMuonDetector.h"
#include "MuonDet/DeMuonGasGap.h"
#include "MuonDet/MuonChamberGrid.h"
#include "MuonDet/MuonChamberLayout.h"

//Detector description
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"

//Muon Kernel
#include "MuonKernel/MuonTileID.h"

/** @file DeMuonDetector.cpp
 * 
 * Implementation of class : DeMuonDetector
 *
 * @author Alessia Satta
 * @author Alessio Sarti, asarti@lnf.infn.it
 *
 */

#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef XmlUserDetElemCnv<DeMuonDetector>       XmlDeMuonDetector;
static CnvFactory<XmlDeMuonDetector>           s_XmlDeMuonDetectorFactory;
const ICnvFactory&  XmlDeMuonDetectorFactory = s_XmlDeMuonDetectorFactory;

/// Standard Constructor
DeMuonDetector::DeMuonDetector() {

  bool debug = false;
  if(debug) std::cout<< "Building the Detector !!!" <<std::endl;
  m_detSvc = this->dataSvc();
}

/// Standard Destructor
DeMuonDetector::~DeMuonDetector()
{
}
  
const CLID& DeMuonDetector::clID () const 
{ 
  return DeMuonDetector::classID() ; 
}

StatusCode DeMuonDetector::initialize()  
{

  MsgStream msg( msgSvc(), name() );
  msg << MSG::VERBOSE << "Initializing the detector" <<endreq;

  StatusCode sc = DetectorElement::initialize();
  if( sc.isFailure() ) { 
    msg << MSG::ERROR << "Failure to initialize DetectorElement" << endreq;
    return sc ; 
  }

  //Initialize the maximum number of allowed chambers per region
  int myDum[4] = {12,24,48,192};
  for(int dum = 0; dum<4; dum++) {MaxRegions[dum] = myDum[dum];}

  //Initializing the Layout
  m_chamberLayout.initialize();
  m_chamberLayout.setDataProvider(m_detSvc);
  m_chamberLayout.fillChambersVector(m_detSvc);

  //Fills the chambers pointer
  m_ChmbPtr.resize(1380);
  fillChmbPtr();

  return sc;
}


StatusCode DeMuonDetector::Hit2GapNumber(HepPoint3D myPoint, 
					 int station, int & gapNumber,
					 int & chamberNumber, int& regNum){
  
  //This methods sets the gap = 0 if only the
  //station is provided [HitPoint.z() = 0]
  StatusCode sc = StatusCode::FAILURE;
  bool isIn = false; bool debug = false;

  MsgStream msg( msgSvc(), name() );
  sc = Hit2ChamberNumber(myPoint,station,chamberNumber,regNum);
  DeMuonChamber * theChmb = getChmbPtr(station,regNum,chamberNumber);

  //Set SC to failure until gap is found
  sc = StatusCode::FAILURE;

  double zPoi = myPoint.z(); int Igap;
  double zChmb = (theChmb->geometry())->toGlobal(HepPoint3D(0,0,0)).z();  
  for (Igap = 0; Igap <4; Igap++) {
    //Use z information to catch the gap.
    if((zPoi-zChmb)<-20+Igap*16) break;
  }
  if(debug)   std::cout<<"Z difference: "<<zPoi<<" "<<zChmb<<" "<<zPoi-zChmb<<" "<<Igap<<std::endl;
  if(Igap <4) { 	
    sc = StatusCode::SUCCESS;
    gapNumber = Igap; 
  } else {
    //Is the chamber returned containing the hit?
    IDetectorElement::IDEContainer::iterator itGap= theChmb->childBegin();
    for(itGap=theChmb->childBegin(); itGap<theChmb->childEnd(); itGap++){
      IGeometryInfo* geoGap = (*itGap)->geometry();  
      isIn = geoGap->isInside(myPoint);
      if(isIn) {
	DeMuonGasGap*  myGap =  dynamic_cast<DeMuonGasGap*>( *itGap ) ;
	gapNumber = myGap->gasGapNumber();
	sc = StatusCode::SUCCESS;
	break;
      }
    }
  }

  return sc;
}


StatusCode DeMuonDetector::Hit2ChamberNumber(HepPoint3D myPoint, 
					     int station, 
					     int & chamberNumber, int& regNum){
  
  StatusCode sc = StatusCode::FAILURE;
  bool isIn = false; bool debug = false;
  
  MsgStream msg( msgSvc(), name() );
  double x = myPoint.x();  double y = myPoint.y(); double z = myPoint.z();
  //Returning the most likely chamber
  m_chamberLayout.chamberMostLikely(x,y,station,chamberNumber,regNum);
  
  //Providing all 3 numbers identifies a chamber
  IGeometryInfo* geoChm = (getChmbPtr(station,regNum,chamberNumber))->geometry();  

  //For calls not providing hit z take the z of chamber center
  if(!z) {myPoint.setZ(geoChm->toGlobal(HepPoint3D(0,0,0)).z());}

  //Is the chamber returned containing the hit?
  isIn = geoChm->isInside(myPoint);

  if(isIn) {
    msg << MSG::DEBUG << "Hit found" <<endreq;
    sc = StatusCode::SUCCESS;
  } else {

    msg << MSG::DEBUG <<"Starting point::  "<<station<<" "<<chamberNumber<<" "<<regNum<<endreq;

    //Find the vector of chambers near the one under investigation
    double x_ref(0),y_ref(0); std::vector<DeMuonChamber*> myChams;
    x_ref = geoChm->toGlobal(HepPoint3D(0,0,0)).x();
    y_ref = geoChm->toGlobal(HepPoint3D(0,0,0)).y();

    int x_sgn(0),y_sgn(0);
    //Avoid unnecessary checks for resolution problems
    if(fabs(x-x_ref)>0.01) {x_sgn = (x-x_ref)/fabs(x-x_ref);}
    if(fabs(y-y_ref)>0.01) {y_sgn = (y-y_ref)/fabs(y-y_ref);}

    msg << MSG::DEBUG << "Hit not found Try to look in corner "<<x_ref<< " "<<x<<" "<<x_sgn<<" "<<y_ref<<" "<<y<<" "<<y_sgn<<endreq;    


    myChams = m_chamberLayout.neighborChambers(chamberNumber,station,regNum,x_sgn,y_sgn);

    std::vector<DeMuonChamber*>::iterator aChamber;
    //Loops on found chambers
    for(aChamber = myChams.begin(); aChamber<myChams.end(); aChamber++){

      int tmpChn(0), tmpRen(0), tmpStn(0);
      tmpChn = (*aChamber)->chamberNumber();
      tmpRen = (*aChamber)->regionNumber();
      tmpStn = (*aChamber)->stationNumber();

      //Accessing Geometry Info
      IGeometryInfo* geoOthChm = (getChmbPtr(tmpStn,tmpRen,tmpChn))->geometry();  
      if(geoOthChm) {
	//For calls not providing hit z take the z of chamber center
	if(!z) {myPoint.setZ(geoOthChm->toGlobal(HepPoint3D(0,0,0)).z());}
	
	isIn = geoOthChm->isInside(myPoint);

	if(isIn) {
	  sc = StatusCode::SUCCESS;
	  //Returns the correct region and chamber number
	  chamberNumber = (*aChamber)->chamberNumber();
	  regNum = (*aChamber)->regionNumber();
	  break;
	}
      } else {
	msg << MSG::WARNING << "Could not find Geometry info of a given chamber!"<<endreq;    
      }
    }

    //Debug the chambers returned
    if(debug) {
      std::cout<<"Chmb test n.  :"<<chamberNumber+1<<"  ";
      std::cout<<"Chmb ret.  : ";
      for(aChamber = myChams.begin(); aChamber<myChams.end(); aChamber++){
	std::cout<<(*aChamber)->chamberNumber()+1<<" ";
      }
      std::cout<<" "<<std::endl;
    }
  }

  if(!isIn) {
    msg << MSG::WARNING << "Smart seek didn't work. Perform loop on all chambers :( !!! " <<endreq;
    int msta(0),mreg(0),mchm(0); 
    //Getting stations
    IDetectorElement::IDEContainer::iterator itSt=this->childBegin();
    for(itSt=this->childBegin(); itSt<this->childEnd(); itSt++){
      if(msta == station) {
	//Getting regions
	mreg = 0;
	IDetectorElement::IDEContainer::iterator itRg=(*itSt)->childBegin();
	for(itRg=(*itSt)->childBegin(); itRg<(*itSt)->childEnd(); itRg++){
	  //Getting chambers
	  mchm = 0;
	  IDetectorElement::IDEContainer::iterator itCh=(*itRg)->childBegin();
	  for(itCh=(*itRg)->childBegin(); itCh<(*itRg)->childEnd(); itCh++){
	    IGeometryInfo* geoAllChm = (*itCh)->geometry();  

	    //For calls not providing hit z take the z of chamber center
	    if(!z) {myPoint.setZ(geoAllChm->toGlobal(HepPoint3D(0,0,0)).z());}

	    isIn = geoAllChm->isInside(myPoint);
	    if(isIn) {
	      sc = StatusCode::SUCCESS;
	      //Returns the correct region and chamber number
	      chamberNumber = mchm;  regNum = mreg;
	      msg << MSG::DEBUG << "Hit found in chamber C: " <<chamberNumber<<" , R: "<<regNum<<" ,S: "<<station<<endreq;
	      return sc;
	    }
	    mchm++;
	  }//Chamber Loop
	  mreg++;
	}//Region Loop
      }
      msta++;
    }//Stations Loop
  }  
  return sc;
}


StatusCode DeMuonDetector::Pos2ChamberNumber(const double x,
					     const double y,
					     int station,
					     int & chamberNumber, int& regNum){
  StatusCode sc = StatusCode::FAILURE;
  //Hit Z is not know (giving only the station).
  //Take the chamber Z to update the hit later on.
  HepPoint3D hitPoint(x,y,0); 

  sc = Hit2ChamberNumber(hitPoint,station,chamberNumber,regNum);

  return sc;
}

StatusCode DeMuonDetector::Pos2GapNumber(const double x,
					 const double y,
					 int station, int & gapNumber,
					 int & chamberNumber, int& regNum){
  StatusCode sc = StatusCode::FAILURE;
  //Hit Z is not know (giving only the station).
  //Take the chamber Z to update the hit later on.
  HepPoint3D hitPoint(x,y,0); 

  sc = Hit2GapNumber(hitPoint,station,gapNumber,chamberNumber,regNum);

  return sc;
}


StatusCode DeMuonDetector::Pos2ChamberPointer(const double x,
					      const double y,
					      int station,
					      DeMuonChamber* & chamberPointer){
  StatusCode sc = StatusCode::FAILURE;
  //Hit Z is not know (giving only the station).
  //Take the chamber Z to update the hit later on.
  HepPoint3D hitPoint(x,y,0); 
  int chamberNumber(-1),regNum(-1);
  sc = Hit2ChamberNumber(hitPoint,station,chamberNumber,regNum);
  if((regNum > -1) && (chamberNumber>-1)) {
    DeMuonChamber*  myPtr =  getChmbPtr(station,regNum,chamberNumber) ;
    chamberPointer = myPtr;
  }

  return sc;
}


StatusCode DeMuonDetector::Pos2ChamberPointer(const double x,
					      const double y,
					      const double z,
					      DeMuonChamber* & chamberPointer){
  //Dummy conversion z <-> station  
  StatusCode sc = Pos2ChamberPointer(x,y,getStation(z),chamberPointer);
  return sc;
}


StatusCode DeMuonDetector::Pos2ChamberNumber(const double x,
					     const double y,
					     const double z,
					     int & chamberNumber, int& regNum){
  StatusCode sc = StatusCode::FAILURE;
  //Z is know/provided.
  HepPoint3D hitPoint(x,y,z);   int sta = getStation(z);

  sc = Hit2ChamberNumber(hitPoint,sta,chamberNumber,regNum);

  return sc;
}



StatusCode DeMuonDetector::Pos2GapNumber(const double x,
					     const double y,
					     const double z, int & gapNumber,
					     int & chamberNumber, int& regNum){
  StatusCode sc = StatusCode::FAILURE;
  //Z is know/provided.
  HepPoint3D hitPoint(x,y,z);   int sta = getStation(z);

  sc = Hit2GapNumber(hitPoint,sta,gapNumber,chamberNumber,regNum);

  return sc;
}


StatusCode DeMuonDetector::Pos2ChamberTile(const double x,
					   const double y,
					   const double z,
					   MuonTileID& tile){
  StatusCode sc = StatusCode::SUCCESS;
  int dumChmb(-1), reg(-1);

  //Return the chamber number
  sc = Pos2ChamberNumber(x,y,z,dumChmb,reg);

  //Convert chamber number into a tile
  tile = m_chamberLayout.tileChamberNumber(getStation(z),reg,dumChmb);

  return sc;
}

IDetectorElement* DeMuonDetector::ReturnADetElement(int lsta, int lreg, int lchm) {
  
  IDetectorElement* myDet = (IDetectorElement*)0;
  int msta(0),mreg(0),mchm(0); 

  //Getting stations
  IDetectorElement::IDEContainer::iterator itSt=this->childBegin();
  for(itSt=this->childBegin(); itSt<this->childEnd(); itSt++){
    if((msta == lsta) && (lreg == -1) && (lchm == -1)) {
      myDet = *itSt;
      return myDet;
    } 
    //Getting regions
    mreg = 0;
    IDetectorElement::IDEContainer::iterator itRg=(*itSt)->childBegin();
    for(itRg=(*itSt)->childBegin(); itRg<(*itSt)->childEnd(); itRg++){
      if((msta == lsta) && (mreg == lreg) && (lchm == -1)) {
	myDet = *itRg;
        return myDet;
      } 
      //Getting chambers
      mchm = 0;
      IDetectorElement::IDEContainer::iterator itCh=(*itRg)->childBegin();
      for(itCh=(*itRg)->childBegin(); itCh<(*itRg)->childEnd(); itCh++){
	
	if((msta == lsta) && (mreg == lreg) && (mchm == lchm)) {
	  myDet = *itCh;
	  return myDet;
	} 
	mchm++;
      }//Chamber Loop
      mreg++;
    }//Region Loop
    msta++;
  }//Stations Loop
  
  return myDet;
  
}


StatusCode DeMuonDetector::getPadSize(const int station, const int region,
				      double &sizeX, double &sizeY){

  StatusCode sc = StatusCode::SUCCESS;

  std::string myMothPt = "/dd/Structure/Grid";
  std::string myGridPt = "/dd/Structure/Grid";
  
  DeMuonChamber* myPtr;
  //Gets a chamber in the requested region/station
  for(int ich = 0; ich<MaxRegions[region]; ich++) {
    //Gets the chamber
    myPtr = getChmbPtr(station,region,ich);
    if(myPtr) break;
  }

  //Getting LHCb
  IDetectorElement* lhcb = this->parentIDetectorElement();
  
  //Getting the structure
  IDetectorElement* struc = lhcb->parentIDetectorElement();

  //  std::cout<<"Seek! "<<std::endl;
  /*
  //Diving to the grids
  IDetectorElement::IDEContainer::iterator itGmoth=struc->childBegin();
  for(itGmoth=struc->childBegin(); itGmoth<struc->childEnd(); itGmoth++){
    //Gets the mother layer (Grid)
    if(!strcmp(((*itGmoth)->name()).data(),myMothPt.data())) {
      //Now the various grids
      IDetectorElement::IDEContainer::iterator itGrds=(*itGmoth)->childBegin();
      for(itGrds=(*itGmoth)->childBegin(); itGrds<(*itGmoth)->childEnd(); itGrds++){
	myGridPt = myMothPt + "/" + myPtr->getGridName();
	if(!strcmp(((*itGrds)->name()).data(),myGridPt.data())) {
	  
	  std::cout<<"Found my grid! "<<myPtr->getGridName()<<std::endl;
	  
	}
      }
    }
  }
  */
  return sc;
}

DeMuonChamber* DeMuonDetector::getChmbPtr(const int station, const int region,
			    const int chmb) {
  DeMuonChamber* myPtr;
  int encode, re(0);
  encode = 276*station+chmb;
  if(region) {
    re = region;
    while(re >= 1) {encode += MaxRegions[re-1]; re--;}
  }
  myPtr = m_ChmbPtr.at(encode);
  return myPtr;
}

void DeMuonDetector::fillChmbPtr() {

  int encode(0);
  bool debug = false;
  //Stations
  for(int iS = 0; iS<5; iS++){ 
    //Regions
    for(int iR = 0; iR<4; iR++){ 
      //Chambers
      for(int iC = 0; iC<MaxRegions[iR]; iC++){ 

	IDetectorElement* det  = ReturnADetElement(iS,iR,iC);
	DeMuonChamber*  myPtr =  dynamic_cast<DeMuonChamber*>( det ) ;

	if(debug) std::cout<<"Filling chamber Pointer: "<<encode<<" "<<iS<<" "<<MaxRegions[iR]<<" "<<iR<<" "<<iC<<std::endl;
	if(myPtr) {
	  m_ChmbPtr.at(encode) = myPtr;
	} else {
	  m_ChmbPtr.at(encode) = (DeMuonChamber*)0;
	}
	encode++;
      }
    }
  }
  if(debug)  std::cout<<" Filled the chamber pointer "<<std::endl;
  return;
}

std::vector< std::pair<MuonFrontEndID, std::vector<float> > > DeMuonDetector::listOfPhysChannels(HepPoint3D my_entry, HepPoint3D my_exit) {
  
  int regNum(0),chamberNumber(0);
  int station = getStation(my_entry.z());

  MsgStream msg( msgSvc(), name() );

  //Hit entry
  Hit2ChamberNumber(my_entry,station,chamberNumber,regNum);

  //Hit exit
  int regNum1(0),chamberNumber1(0);
  int station1 = getStation(my_exit.z());
  Hit2ChamberNumber(my_exit,station1,chamberNumber1,regNum1);
  
  if(station != station1 || chamberNumber != chamberNumber1 || regNum != regNum1) { msg << MSG::ERROR <<"Could not return LoC for entry & exit in !=  chambers "<<endreq; }
  
  //Getting the chamber pointer.
  DeMuonChamber*  myChPtr =  getChmbPtr(station,regNum,chamberNumber) ;

  //Chamber Geometry info  
  IGeometryInfo*  geoCh=myChPtr->geometry();
  
  const SolidBox *box = dynamic_cast<const SolidBox *>(geoCh->lvolume()->solid());
  float dx = box->xHalfLength();  float dy = box->yHalfLength();
  float mygX = geoCh->toGlobal(HepPoint3D(0,0,0)).x();
  float mygY = geoCh->toGlobal(HepPoint3D(0,0,0)).y();

  //Getting the grid pointer
  std::string gName = "/dd/Structure/Grid/"+myChPtr->getGridName();
  SmartDataPtr<MuonChamberGrid> theGrid (m_detSvc,gName.data());

  //Define relative dimensions
  float mod_xen = (my_entry.x()-(mygX-dx))/(2*dx);
  float mod_yen = (my_entry.y()-(mygY-dy))/(2*dy);
  float mod_xex = (my_exit.x()-(mygX-dx))/(2*dx);
  float mod_yex = (my_exit.y()-(mygY-dy))/(2*dy);

  //Gets list of channels
  std::vector< std::pair<MuonFrontEndID, std::vector<float> > > myPair;
  myPair = theGrid->listOfPhysChannels(mod_xen,mod_yen,mod_xex,mod_yex);

  return myPair;
}
  
