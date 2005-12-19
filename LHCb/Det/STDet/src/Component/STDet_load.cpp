
#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_FACTORY_ENTRIES(STDet) {
  DECLARE_CONVERTER( XmlTTDetectorCnv );
  DECLARE_CONVERTER( XmlITDetectorCnv );
  DECLARE_CONVERTER( XmlTTStationCnv );
  DECLARE_CONVERTER( XmlITStationCnv );
  DECLARE_CONVERTER( XmlTTLayerCnv );
  DECLARE_CONVERTER( XmlITLayerCnv );
  DECLARE_CONVERTER( XmlTTHalfModuleCnv );
  DECLARE_CONVERTER( XmlITBoxCnv );
  DECLARE_CONVERTER( XmlITSectorCnv );
  DECLARE_CONVERTER( XmlTTSectorCnv );
}
