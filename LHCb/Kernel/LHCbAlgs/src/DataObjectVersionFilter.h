// $Id: DataObjectVersionFilter.h,v 1.1 2009-11-06 18:04:23 jonrob Exp $
#ifndef DATAOBJECTVERSIONFILTER_H 
#define DATAOBJECTVERSIONFILTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class DataObjectVersionFilter DataObjectVersionFilter.h
 *  
 *  Checks the version of a given DataObject in the TES, and sets FilterPassed according
 *  to a min and max version number. Useful to allow reprocessing of some data depending
 *  on the version number
 *
 *  @author Chris Jones
 *  @date   2009-11-06
 */
class DataObjectVersionFilter : public GaudiAlgorithm 
{

public: 

  /// Standard constructor
  DataObjectVersionFilter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DataObjectVersionFilter( ); ///< Destructor

  virtual StatusCode execute();    ///< Algorithm execution

private:

  int m_minV;          ///< Min version number
  int m_maxV;          ///< Max version number
  std::string m_loc;   ///< DataObject location in TES

};

#endif // DATAOBJECTVERSIONFILTER_H
