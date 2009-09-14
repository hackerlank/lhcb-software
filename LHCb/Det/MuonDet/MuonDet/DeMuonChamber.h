// $Id: DeMuonChamber.h,v 1.9 2009-09-14 08:58:35 jonrob Exp $
// ============================================================================
// CVS tag $Name:
// ============================================================================
//
// ============================================================================
#ifndef MUONDET_DEMUONCHAMBER_H
#define MUONDET_DEMUONCHAMBER_H 1


// Include files
#include <vector>
#include <string>

#include "GaudiKernel/MsgStream.h"

#include "DetDesc/DetectorElement.h"
#include "MuonDet/MuonNamespace.h"

/** @class DeMuonChamber DeMuonChamber.h MuonDet/DeMuonChamber.h
 *  
 *  Detector element class for a single chamber in the muon system
 *
 *  @author David Hutchcroft
 *  @date   21/01/2002
 */

/// Class ID of chambers 
static const CLID CLID_DEMuonChamber = 11006;  

class DeMuonChamber: public DetectorElement {

public:

  /// Constructor (empty)
  DeMuonChamber();

  /// Constructor used by XmlMuonRegionCnv to create chambers
  /// pad sizes in mm 
  DeMuonChamber( int nStation, int nRegion, int nChamber);

  /// Destructor
  ~DeMuonChamber();

  inline static const CLID& classID(){
    return CLID_DEMuonChamber;
  }

  inline virtual const CLID& clID() const {
    return classID();
  }

  //Initialize
  StatusCode initialize();

  /// get Station Number
  inline int stationNumber() const {
    return m_StationNumber;
  }

  /// set Station Number
  void setStationNumber(int nStation){
    m_StationNumber = nStation;
  }

  /// get Region Number
  inline int regionNumber() const {
    return m_RegionNumber;
  }

  /// set Region Number
  void setRegionNumber(int nRegion){
    m_RegionNumber = nRegion;
  }

  /// get Chamber Number
  inline int chamberNumber() const {
    return m_ChamberNumber;
  }

  /// set Chamber Number
  void setChamberNumber(int nChamber){
    m_ChamberNumber = nChamber;
  }

  /// get chamber Grid 
  inline std::string getGridName() const {
    return m_chmbGrid;
  }

  /// set chamber Grid
  void setGridName(std::string grid){
    m_chmbGrid = grid;
  }

private:

  /// Access to Msgstream object
  inline MsgStream & msgStream() const
  {
    if ( !m_msgStream ) m_msgStream = new MsgStream(msgSvc(),name());
    return *m_msgStream;
  }

private:

  mutable MsgStream * m_msgStream;

  /// Chamber Grid
  std::string m_chmbGrid;

  /// Station number
  int m_StationNumber;

  /// Region number in station
  int m_RegionNumber;

  /// Chamber number in region
  int m_ChamberNumber;

};

#endif    // MUONDET_DEMUONCHAMBER_H
