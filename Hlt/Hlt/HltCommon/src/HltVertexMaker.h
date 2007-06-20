// $Id: HltVertexMaker.h,v 1.1 2007-06-20 12:17:38 hernando Exp $
#ifndef HLTVERTEXMAKER_H 
#define HLTVERTEXMAKER_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"

/** @class HltVertexMaker HltVertexMaker.h
 *  
 *
 *  @author Hugo Ruiz
 *  @date   2006-02-21
 */


class HltVertexMaker : public HltAlgorithm {
 public: 

  /// Standard constructor
  HltVertexMaker( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~HltVertexMaker( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
    
 protected:
  
  bool haveOverlaps(const LHCb::Track& track1, const LHCb::Track& track2 );

 private:

  // PatOutputVertices
  std::string m_patOutputVerticesName;
  PatVertexContainer* m_patOutputVertices;

  bool m_checkForOverlaps;
  bool m_twoContainers;

protected:

  StringArrayProperty m_filterDescriptor;

  std::vector<std::string> m_filterNames;
  std::vector<int> m_filterIDs;
  std::vector<Hlt::TrackBiFunction*> m_functions;
  std::vector<Hlt::Filter*> m_filters;
  std::vector<HltHisto> m_histos;
  std::vector<int> m_tcounters;

  std::vector<double> m_vals;

  Hlt::VertexCreator _makeVertex;

};
#endif // HLTMAKEVERTICES_H 

