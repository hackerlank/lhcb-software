// $Id: $
// ============================================================================
#include "BBDTSimpleTool.h"
// ============================================================================
BBDTSimpleTool::BBDTSimpleTool(const std::string& type, 
			       const std::string& name, 
			       const IInterface* parent) 
  : base_class(type,name,parent), m_dist(0), m_dva(0), m_vars(){
  // declare configurable properties
  declareProperty("Cuts", m_cuts, "Simple tree of cuts");
}
// ===========================================================================
StatusCode BBDTSimpleTool::initialize() {

  // initialize the base class  (the first action)
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure()) return sc; 

  // get tools and algs
  m_dist = tool<IDistanceCalculator>("LoKi::DistanceCalculator",this);
  m_dva = Gaudi::Utils::getDVAlgorithm(contextSvc());
  if (0 == m_dva) 
    return Error("Couldn't get parent DVAlgorithm", StatusCode::FAILURE);  

  // display cuts 
  std::vector<std::map<std::string,std::pair<double,double> > > 
    ::const_iterator iter = m_cuts.begin();
  info() << "Initialized w/ Cuts = [";
  while(iter != m_cuts.end()){
    info() << "(";
    std::map<std::string,std::pair<double,double> >::const_iterator it
      = iter->begin();
    while(it != iter->end()){      
      double min_cut = it->second.first;
      double max_cut = it->second.second;
      if(min_cut > 0)
	info() << "(" << it->first << " > " << min_cut << ")";
      if(max_cut > 0){
	if(min_cut > 0) info() << "&";
	info() << "(" << it->first << " < " << max_cut << ")";
      }
      it++;
      if(it != iter->end()) info() << "&";
    }
    info() << ")";
    iter++;
    if(iter != m_cuts.end()) info() << "|";
  }
  info() << "]" << endmsg;
  return StatusCode::SUCCESS;
}
// ============================================================================
bool BBDTSimpleTool::operator()(const LHCb::Particle* p) const {

  if(0 == p) {
    Error("LHCb::Particle* points to NULL, return false");
    return false ;
  }
  if(!m_vars.set(p,m_dva,m_dist)) return false;

  std::vector<std::map<std::string,std::pair<double,double> > > 
    ::const_iterator iter = m_cuts.begin();
  while(iter != m_cuts.end()){
    if(m_vars.cut(*iter)) return true;
    iter++;
  }
  return false;
}
// ============================================================================
/// declare & implement the factory 
DECLARE_TOOL_FACTORY(BBDTSimpleTool);
// ============================================================================
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Grammars.h"
// FIXME: Move to the new boost::spirit::classic namespace
#if !defined(BOOST_SPIRIT_USE_OLD_NAMESPACE)
#define BOOST_SPIRIT_USE_OLD_NAMESPACE
#endif
#include <boost/spirit/include/classic.hpp>
#include <boost/spirit/include/phoenix1.hpp>
namespace Gaudi { 
  namespace Parsers {

    StatusCode parse(std::vector<std::map<std::string, 
		     std::pair<double,double> > >& result, 
		     const std::string& input){
 
      typedef RealGrammar<double> RG;
      typedef PairGrammar<RG,RG> PG;
      typedef MapGrammar<StringGrammar,PG> MG;
      typedef VectorGrammar<MG> VG;
      VG grammar ;

      typedef boost::spirit::position_iterator<std::string::const_iterator> 
	IteratorT;
 
      return parse(IteratorT(input.begin(),input.end()),IteratorT(),
		   grammar[var(result)=arg1],SkipperGrammar()).full;
    }
  }
}
// ============================================================================
