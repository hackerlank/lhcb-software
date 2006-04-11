#ifndef DAVINCIKERNEL_DVALGORITHM_H
#define DAVINCIKERNEL_DVALGORITHM_H 1

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/AlgFactory.h"

// from EventSys
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/SelResult.h"

// from DaVinciTools
#include "Kernel/IPhysDesktop.h"
#include "Kernel/IVertexFit.h"
#include "Kernel/IGeomDispCalculator.h"
#include "Kernel/IParticleFilter.h"
#include "Kernel/ICheckOverlap.h"
#include "Kernel/IAlgorithm2ID.h"
#include "Kernel/IBTaggingTool.h"
#include "Kernel/IParticleDescendants.h"

// from Boost
#include <boost/lexical_cast.hpp>

/** @class DVAlgorithm DVAlgorithm.h Kernel/DVAlgorithm.h
 *  Base Class for DaVinci Selection Algorithms:
 *  Does the retrieval of all necessary DaVinci Tools
 *  
 *  @author Claudia Pereira Nunes
 *  @date   19/04/2002
 *  04/03/2004: Hugo Ruiz: automatically produce SelResult object
 *  use always algorithm name as output location in TES
 *  16/07/2004: P. Koppenburg: Make it a GaudiTupleAlg
 *  11/11/2004: P. Koppenburg: Adapt to next get<> and put<>. Merge with PreDV.
 */
class DVAlgorithm : public GaudiTupleAlg {
public:

  /// Standard constructor
  DVAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DVAlgorithm( ){ }; ///< Destructor
  
  /// Overridden from Gaudi Algo to produce a warning if not called by user
  virtual StatusCode setFilterPassed (bool state);  

  /// Overridden from Gaudi Algorithm
  virtual StatusCode sysExecute ();  

  // Overridden from Gaudi Algorithm
  virtual StatusCode initialize ();  

  // Overridden from Gaudi Algorithm
  virtual StatusCode finalize ();  

  // Overridden from Gaudi Algorithm
  std::string getDecayDescriptor(){return m_decayDescriptor;};  

  /// Imposes an output location for desktop different from algo name
  /// Should be avoided!
  void imposeOutputLocation (const std::string& outputLocationString);  

  /// get algorithm ID
  int getAlgorithmID();
 
  /// Accessor for PhysDesktop Tool
  inline IPhysDesktop* desktop()const{
    return getTool<IPhysDesktop>(m_desktopName,m_desktop,this) ;
  }

  /// Accessor for Vertex Fitter Tool
  inline IVertexFit* vertexFitter(int index=0)const{
    return getTool<IVertexFit>(index,m_vertexFitNames,m_vertexFit,this);
  }

  /* @todo move to this signature when maps in job options become available
  /// Accessor for Vertex Fitter Tool
  inline IVertexFit* vertexFitter(const std::string & name="") const {
    return getTool<IVertexFit>(m_vertexFitName.at(0),
                               m_vertexFit.at(0), this);
  }
  */

  /// Accessor for Geometrical Displacement Calculation Tool
  inline IGeomDispCalculator* geomDispCalculator(int index=0)const{
    return getTool<IGeomDispCalculator>(index,m_geomToolNames, m_geomTools,
                                        this);
  }

  /* @todo move to this signature when maps in job options become available
  /// Accessor for Geometrical Displacement Calculation Tool
  inline IGeomDispCalculator* geomDispCalculator(const std::string& name="") const
  {
    return getTool<IGeomDispCalculator>(m_geomToolNames.at(0),
                                        m_geomTools.at(0), this);
  }
  */

  /// Accessor for Particle Filter Tool
  inline IParticleFilter* particleFilter(int index=0)const{
    info() << "Hello " << index << endmsg ;
    return DVAlgorithm::getTool<IParticleFilter>(index,
                                                 m_filterNames,
                                                 m_filters,this);
  }

  /* @todo move to this signature when maps in job options become available
  /// Accessor for Particle Filter Tool
  inline IParticleFilter* particleFilter(const std::string& name="")const{
    info() << "Hello " << index << endmsg ;
    return DVAlgorithm::getTool<IParticleFilter>(m_filterNames.at(0),
                                                 m_filters.at(0), this);
  }
  */

  /// Accessor for CheckOverlap Tool
  inline ICheckOverlap* checkOverlap()const{
    return getTool<ICheckOverlap>(m_checkOverlapName,m_checkOverlap);
  }

  /* @todo move to this signature when maps in job options become available
  /// Accessor for CheckOverlap Tool
  inline ICheckOverlap* checkOverlap(const std::string& name="") const{
    return getTool<ICheckOverlap>(m_checkOverlapName,m_checkOverlap);
  }
  */

  /// Accessor for Algorithm2ID Tool
  inline IAlgorithm2ID* algorithmID()const{
    return getTool<IAlgorithm2ID>(m_algorithm2IDToolName,m_algorithm2IDTool);
  }

  /* @todo move to this signature when maps in job options become available
  /// Accessor for Algorithm2ID Tool
  inline IAlgorithm2ID* algorithmID(const std::string& name="") const{
    return getTool<IAlgorithm2ID>(m_algorithm2IDToolName,m_algorithm2IDTool);
  }
  */

  /// Accessor for ParticleCombiner tool
  inline IParticleCombiner* particleCombiner(const std::string name = "") const 
  {
    return getTool<IParticleCombiner>(m_particleCombinerName,
                                      m_particleCombiner);
  }
  
  inline IParticleReFitter* particleReFitter(const std::string name = "") const 
  
  {
    return getTool<IParticleReFitter>(m_particleReFitterName,
                                      m_particleReFitter); 
  }
  

  /// Tagging Tool
  inline IBTaggingTool* flavourTagging()const{
    return getTool<IBTaggingTool>(m_taggingToolName,m_taggingTool);
  }

  /// Descnedants
  inline IParticleDescendants* descendants()const{
    return getTool<IParticleDescendants>(m_descendantsName,m_descendants);
  }

  /// Accessor for ParticlePropertySvc
  inline IParticlePropertySvc* ppSvc() const {return m_ppSvc;};

protected:

  /** helper protected function to load the tool on-demand 
   *  by index 
   *  @param index tool index 
   *  @param names list of tools typ/names 
   *  @param tools the list of tools 
   *  @param ptr the pointer to this or NULL for private or common tools
   *  @return tool with given index 
   */
  template<class TYPE> 
  TYPE* getTool ( const size_t index, 
                  const std::vector<std::string>& names , 
                  std::vector<TYPE*>& tools,
                  const IInterface* ptr=NULL ) const {
    // the tool is already located properly?
    if ( index < tools.size() ) { return tools[index] ; }
    
    // the tool need to be located 
    
    // is it possible to locate the tool in principle?
    if ( index < names.size() ){
      Assert( index < names.size() , 
              "DVAlgorithm::getTools: The tool of type '"  
              + System::typeinfoName( typeid(TYPE)) + "'/index='"
              + boost::lexical_cast<std::string>(index) + 
              "' could not be located" ) ;
    }
    
    // locate only the minimal amount of tools
    const size_t nT = tools.size() ;
    for ( std::vector<std::string>::const_iterator iname = names.begin() + nT ; 
          names.end() != iname ; ++iname ){
      // have we load enough tools? 
      if ( index < tools.size() ) { break ; }
      TYPE* t = NULL;
      t = getTool<TYPE>( *iname, t, ptr ) ;
      tools.push_back( t ) ; 
    } ;
    
    return tools[index] ;
  } 

  /** helper protected function to load the tool on-demand  
   *  @param name of tool
   *  @param tool 
   *  @param ptr the pointer to this or NULL for private or common tools
   *  @return tool 
   */
  template<class TYPE> 
  TYPE* getTool ( const std::string& name, 
                  TYPE*& t,
                  const IInterface* ptr=NULL ) const {
    if ( 0==t ) {  // the tool is already located properly?
      t = tool<TYPE>( name, ptr )  ;// else get it
    }
    return t ;
  } ;


private:

  /// Method to load all tools. 
  /// The base class provides an instance of all type of tools
  StatusCode loadTools() ;

  /// Method to create SelResult container
  StatusCode fillSelResult() ;

  /// Reference to desktop tool
  mutable IPhysDesktop* m_desktop;
  /// Concrete type desktop
  std::string m_desktopName;

protected:
  /// Reference to Vertex Fitter
  mutable std::vector<IVertexFit*> m_vertexFit;
  /// Concrete type of vertex fitter
  std::vector<std::string> m_vertexFitNames;

  /// Reference to geometrical displacement Calculation.
  mutable std::vector<IGeomDispCalculator*> m_geomTools;  
  /// Concrete type of geom tool
  std::vector<std::string> m_geomToolNames;

  /// Reference to CheckOverlap
  mutable ICheckOverlap* m_checkOverlap;
  /// Concrete type of CheckOverlap tool
  std::string m_checkOverlapName;

  /// Reference to ParticleFilter
  mutable std::vector<IParticleFilter*> m_filters;
  /// Concrete Type of ParticleFilter tool
  std::vector<std::string> m_filterNames;  

  /// Reference to Algorithm2ID
  mutable IAlgorithm2ID* m_algorithm2IDTool;
  /// Concrete Type of IAlgorithm2ID tool
  std::string m_algorithm2IDToolName;

  /// Reference to ParticleCombiner
  mutable IParticleCombiner* m_particleCombiner;
  /// Concrete Type of ParticleCombiner tool
  std::string m_particleCombinerName;

  /// Reference to ParticleReFitter
  mutable IParticleReFitter* m_particleReFitter;
  /// Concrete Type of ParticleReFitter tool
  std::string m_particleReFitterName;


  /// Reference to FlavourTagging
  mutable IBTaggingTool* m_taggingTool;
  /// Concrete Type of FlavourTagging tool
  std::string m_taggingToolName;

  /// Reference to ParticleDescendants
  mutable IParticleDescendants* m_descendants;
  /// Concrete Type of ParticleDescendants  tool
  std::string m_descendantsName;

  /// Reference to ParticlePropertySvc
  mutable IParticlePropertySvc* m_ppSvc;

private:
  /// Decay description (Property)
  std::string m_decayDescriptor;
  /// Avoid writing SelResult object in TES (Property)
  bool m_avoidSelResult;
  /// Avoid printing SelResult statistics 
  /// (cannot be switched off by OutputLevel)
  bool m_printSelResult;
  

  /// Has setFilterPassed() already been called in current event?
  bool m_setFilterCalled;
  /// Count number of times selection filter is used 
  /// (= number of times alg is called)
  int m_countFilterWrite ;
  /// Number of passing events
  int m_countFilterPassed ;

  /// - For GaudiAlgorithm -
  /// Algorithm ID
  int m_algorithmID ;

  /// Switch PreloadTools to false no to preload any tools.
  /// This will have the effect that they will be loaded on demand, when needed,
  /// at any event. This option is thus only recommended for use of DVAlgorithm
  /// to do something unrelated to physics analysis.
  bool m_preloadTools;

};
// ---------------------------------------------------------------------------
//   end of class
// ---------------------------------------------------------------------------


#endif // DAVINCIKERNEL_DVALGORITHM_H
