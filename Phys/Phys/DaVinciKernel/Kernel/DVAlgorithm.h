// $Id: DVAlgorithm.h,v 1.54 2010-05-14 15:07:10 ibelyaev Exp $ 
// ============================================================================
#ifndef DAVINCIKERNEL_DVALGORITHM_H
#define DAVINCIKERNEL_DVALGORITHM_H 1
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/VectorMap.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
// ============================================================================
// from EventSys
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/SelResult.h"
// ============================================================================
// from DaVinciKernel
// ============================================================================
#include "Kernel/IDVAlgorithm.h"
#include "Kernel/IPhysDesktop.h"
#include "Kernel/IOnOffline.h"
#include "Kernel/IVertexFit.h"
#include "Kernel/IGeomDispCalculator.h"
#include "Kernel/IParticleFilter.h"
#include "Kernel/IFilterCriterion.h"
#include "Kernel/ICheckOverlap.h"
#include "Kernel/IBTaggingTool.h"
#include "Kernel/IParticleDescendants.h"
#include "Kernel/IWriteSelResult.h"
#include "Kernel/IDistanceCalculator.h"
#include "Kernel/IPVReFitter.h"
#include "Kernel/IRelatedPVFinder.h"
// ============================================================================
#include "Kernel/IMassFit.h"
#include "Kernel/ILifetimeFitter.h"
#include "Kernel/IDirectionFit.h"
//
 #include  <boost/type_traits/remove_pointer.hpp> 
// ============================================================================
/** @class DVAlgorithm DVAlgorithm.h Kernel/DVAlgorithm.h
 *  Base Class for DaVinci Selection Algorithms:
 *  Does the retrieval of all necessary DaVinci Tools
 *
 *  The specific configuration properties of the base class: 
 *
 *  - <b>InputLocations</b> : from where to take the particles? 
 *    Forwarded to PhysDesktop.
 *    @see IPhysDesktop
 *
 *  - <b>VertexFitters</b> : the map for possible vertex fitters.
 *      @see IVertexFit 
 *      The semantic is 
 *   @code
 *      MyAgl.VertexFitters = { "" : "OfflineVertexFitter" , 
 *                              "Trigger" : "TrgVertexFitter" , 
 *                              "special" : "MySpecialVertexFitter" } ;
 *   @endcode 
 *   Note: if the type/name of default fitter (<c>""</c>)  is not specified 
 *   it is picked up from IOnOfflineTool. 
 *    
 *   - <b>Distance Tools</b>  : the map of possible geometry tools  
 *   @see IDistanceCalculator
 *   Note: if the type/name of default tool (<c>""</c>)  is not specified 
 *   it is picked up from IOnOfflineTool 
 *
 *  - <b>ParticleFilters</b> : the map for possible particle filters 
 *     @see IParticleFilter 
 *
 *  - <b>ParticleCombiners</b> : the map for possible particle combiners
 *     @see IParticleCombiner
 *  
 *  - <b>ParticleReFitters</b> : the map for possible particle re-fitters 
 *     @see IParticleReFit
 *
 *  - <b>PVReFitters</b> : the map for possible primary vertex re-fitters 
 *     @see IPVReFitter
 *
 *  - <b>ReFitPVs</b> : bool. Perform automatic PV re-fitting when asking for best PV.
 *
 *  - <b>DecayDescriptor</b>  : the decay descriptor ofthe algorithm 
 *               in the spirit of (MC)DecayFinder tool by Olivier Dormond.
 *   
 *
 *  - <b>AvoidSelResult</b>    : the boolean flag (default value id <c>false</c>)
 *    to avoid the publishing of selection results in TES 
 * 
 *  - <b>PrintSelResults</b> : the boolean flag (default value is <c>false</c>)
 *    to print the selection resulst 
 *
 *  - <b>AvoidForcedOutput</b> : the boolean flag (default value is <c>false</c>)
 *    to avoid the wrinting of empty containers if no output data are required.
 *  
 *  - <b>PreloadTools</b> : the boolean flag (default valeu is <c>true</c>) 
 *    to force the preloading of all nesessary tools at the initialization time
 *
 *
 *  @author Claudia Pereira Nunes
 *  @date   19/04/2002
 *  04/03/2004: Hugo Ruiz: automatically produce SelResult object
 *  use always algorithm name as output location in TES
 *  16/07/2004: P. Koppenburg: Make it a GaudiTupleAlg
 *  11/11/2004: P. Koppenburg: Adapt to next get<> and put<>. Merge with PreDV.
 */
class DVAlgorithm : public extends1<GaudiTupleAlg,IDVAlgorithm>
{ 
public:
  // ==========================================================================
  /// Standard constructor
  DVAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  /// destructor 
  virtual ~DVAlgorithm( ){ }                                      // Destructor
  // ==========================================================================
public: // IDVAlgorithm
  // ==========================================================================
  /** Handle to the concrete implementation, or to the parent if
   *  implementation does not derive from GaudiAlgorithm 
   */
  virtual const GaudiAlgorithm* gaudiAlg() const { return this; }
  /// get the best related primary vertex 
  virtual const LHCb::VertexBase* bestVertex(const LHCb::Particle* particle) const 
  {
    return this->bestPV(particle);
  }
  /// get all loaded particles 
  virtual const LHCb::Particle::Range particles() const 
  {
    return LHCb::Particle::Range ( desktop()->particles().begin() ,
                                   desktop()->particles().end()   ) ;
    
  }
  /** direct const access to container of input primary vertices.
   *  @author Juan Palacios juan.palacios@nikhef.nl
   */
  inline const LHCb::RecVertex::Range primaryVertices() const
  {
    return ( exist<LHCb::RecVertex::Range>( m_PVLocation ) ) ? 
      get<LHCb::RecVertex::Range>( m_PVLocation ) : 
      LHCb::RecVertex::Range();
  }
  /** Accessor for IDistanceCalculator tools by name/typename/nickname
   *  @see IDistanceCalculator
   *  @param name the tool name/typename/nickname
   *  @return pointer to aquired tool 
   */
  const IDistanceCalculator* distanceCalculator 
  ( const std::string& name = "" ) const 
  {
    return getTool<IDistanceCalculator>
      ( name , 
        m_distanceCalculatorNames , 
        m_distanceCalculators     , this ) ;
  }
  /** Accessor for ILifetimeFitter tools by name/typename/nickname
   *  @see ILifetimeFitter
   *  @param name the tool name/typename/nickname
   *  @return pointer to aquired tool 
   */
  const ILifetimeFitter* 
  lifetimeFitter ( const std::string& name = "" ) const 
  {
    return getTool<ILifetimeFitter>
      ( name                  , 
        m_lifetimeFitterNames ,
        m_lifetimeFitters     , this ) ; 
  }
  /** Accessor for IVertexFit tools by name/typename/nickname
   *  @see IVertexFit
   *  @param name the tool name/typename/nickname
   *  @return pointer to aquired tool 
   */
  const IVertexFit* 
  vertexFitter ( const std::string& name = "" ) const
  {
    return getTool<IVertexFit> 
      ( name , 
        m_vertexFitNames ,
        m_vertexFits     , this ) ;
  } 
  /** Accessor for IParticleReFitter tools by name/typename/nickname
   *  @see IParticleReFitter
   *  @param name the tool name/typename/nickname
   *  @return pointer to aquired tool 
   */
  const IParticleReFitter* 
  particleReFitter ( const std::string& name = "" ) const 
  {
    return getTool<IParticleReFitter>
      ( name , 
        m_particleReFitterNames ,
        m_particleReFitters     , this ) ; 
  }
  /** Accessor for IParticleCombiner tools by name/typename/nickname
   *  @see IParticleCombiner
   *  @param name the tool name/typename/nickname
   *  @return pointer to aquired tool 
   */
  const IParticleCombiner* 
  particleCombiner ( const std::string& name = "" ) const 
  {
    return getTool<IParticleCombiner> 
      ( name , 
        m_particleCombinerNames ,
        m_particleCombiners     , this ) ;
  }
  /** Accessor for IMassFit tools by name/typename/nickname
   *  @see IMassFit
   *  @param name the tool name/typename/nickname
   *  @return pointer to aquired tool 
   */
  const IMassFit* 
  massFitter ( const std::string& name = "" ) const 
  {
    return getTool<IMassFit>
      ( name              , 
        m_massFitterNames ,
        m_massFitters     , this ) ; 
  }
  /** Accessor for IDirectionFit tools by name/typename/nickname
   *  @see IDirectionFit
   *  @param name the tool name/typename/nickname
   *  @return pointer to aquired tool 
   */
  const IDirectionFit* 
  directionFitter ( const std::string& name = "" ) const 
  {
    return getTool<IDirectionFit>
      ( name                   , 
        m_directionFitterNames ,
        m_directionFitters     , this ) ; 
  }
  /** Accessor for IPVReFitter tools by name/typename/nickname
   *  @see IPVReFitter
   *  @param name the tool name/typename/nickname
   *  @return pointer to aquired tool 
   */
  const  IPVReFitter* 
  primaryVertexReFitter ( const std::string& name = "" ) const 
  {
    return getTool<IPVReFitter>
      ( name , 
        m_pvReFitterNames ,
        m_pvReFitters     , this ) ; 
  }
  // ==========================================================================
public:
  // ==========================================================================
  /// Overridden from Gaudi Algo to produce a warning if not called by user
  virtual void setFilterPassed (bool state);  

  /// Overridden from Gaudi Algorithm
  virtual StatusCode sysExecute ();  

  // Overridden from Gaudi Algorithm
  virtual StatusCode initialize ();  

  // Overridden from Gaudi Algorithm
  virtual StatusCode finalize ();  

  // Get decay descriptor
  std::string getDecayDescriptor(){return m_decayDescriptor;};  

  // Set decay descriptor
  void setDecayDescriptor(std::string dd){m_decayDescriptor = dd;};  

  /// Imposes an output location for desktop different from algo name
  /// Should be avoided!
  void imposeOutputLocation (const std::string& outputLocationString);  

  /// Accessor for PhysDesktop Tool
  inline IPhysDesktop* desktop()const
  {
    return getTool<IPhysDesktop>(m_desktopName,m_desktop,this) ;
  }

  /// accessor for IOnOffline tool
  inline IOnOffline* onOffline() const
  {
    return getTool<IOnOffline>("OnOfflineTool", m_onOffline, this) ;    
  }
  
  /**
   * direct const access to the tool that calculates the Particle->PV
   * weighted relations
   * 
   * @author Juan Palacios juan.palacios@nikhef.nl
   **/
  inline const IRelatedPVFinder* relatedPVFinder() const
  {
    if ( 0!=m_pvRelator ) return m_pvRelator;
    const std::string& pvRelatorName = onOffline()->relatedPVFinderType();
    return getTool<IRelatedPVFinder>(pvRelatorName, m_pvRelator, this) ;      
  }

  /**
   *
   *
   *
   **/
  inline const LHCb::VertexBase* bestPV(const LHCb::Particle* p) const 
  {
    return useP2PV() ? _getRelatedPV(p) : calculateRelatedPV(p);
  }

  inline const LHCb::VertexBase* getRelatedPV(const LHCb::Particle* p) const 
  {
    return useP2PV() ? _getRelatedPV(p) : calculateRelatedPV(p);
  }
  

  /**
   *
   * Calculate the best related PV for a particle and return it to the user
   * If property "ReFitPVs" is set to true, this triggers a re-fit of the PVs
   * after removing tracks coming from the particle in question. If not, then 
   * it uses the container of PVs obtained from method primaryVertices().
   * Returns a newed pointer to a vertex either from the TES or stored in the
   * PhysDesktop local array.
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @param p LHCb::Particle to be related
   * @return newed pointer to related vertex. TES or PhysDesktop in charge of
   * memory management.
   *
   **/
  const LHCb::VertexBase* calculateRelatedPV(const LHCb::Particle* p) const;

  /**
   *
   * Get the related PV from the relations table, or call calculateRelatedPV
   * if it isn't already there.
   *
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date 10/02/2009
   * 
   **/
  const LHCb::VertexBase* _getRelatedPV(const LHCb::Particle* p) const;

  /**
   *
   **/
  inline void storeRelationWithOverwrite(const Particle2Vertex::LightTable& table) 
  {
    desktop()->overWriteRelations(table.i_relations().begin(), 
                                  table.i_relations().end());
  }
  
  /**
   *
   **/
  inline void relateWithOverwrite(const LHCb::Particle*   part, 
                                  const LHCb::VertexBase* vert) const
  {
    if (0==part || 0== vert ) return;
    (this->desktop()->Particle2VertexRelations().i_removeFrom(part)).ignore();
    this->desktop()->relate(part, vert);
  }
public:
  
  
  /// Accessor for Geometrical Displacement Calculation Tool
  inline IGeomDispCalculator* 
  obsoleteGeomDispCalculator ( const std::string& name = "" ) const
  {
    Warning("You are using the obsolete IGeomDispCalculator interface. Use IDistanceCalculator instead.",
            1,StatusCode::SUCCESS).ignore();
    return getTool<IGeomDispCalculator>
      ( name , 
        m_geomToolNames , 
        m_geomTools     , this ) ;
  }
  
  /// Accessor for Particle Filter Tool
  inline IParticleFilter* 
  particleFilter ( const std::string& name = "" ) const
  {
    return getTool<IParticleFilter>
      ( name          , 
        m_filterNames , 
        m_filters     , this ) ;
  }


public:
  
  /// Accessor for CheckOverlap Tool
  inline ICheckOverlap* checkOverlap()const
  {
    return getTool<ICheckOverlap>(m_checkOverlapName,m_checkOverlap);
  }
  
  /* @todo move to this signature when maps in job options become available
  /// Accessor for CheckOverlap Tool
  inline ICheckOverlap* checkOverlap(const std::string& name="") const{
    return getTool<ICheckOverlap>(m_checkOverlapName,m_checkOverlap);
  }
  */

  /// Accessor for WriteSelResult Tool
  inline IWriteSelResult* writeSelResult()const
  {
    return getTool<IWriteSelResult>(m_writeSelResultName,
                                    m_writeSelResult);
  }
  /// Tagging Tool
  inline IBTaggingTool* flavourTagging()const{
    return getTool<IBTaggingTool>(m_taggingToolName, 
                                  m_taggingTool, 
                                  this );
  }
  
  /// Descnedants
  inline IParticleDescendants* descendants()const{
    return getTool<IParticleDescendants>(m_descendantsName,
                                         m_descendants);
  }
  // ==========================================================================
  /** Accessor for ParticlePropertySvc
   *  @see LHCb::ParticlePropertySvc 
   *  @return pointer to Particle Property Service 
   */
  inline const LHCb::IParticlePropertySvc* ppSvc() const ;
  /** helper method to get a proper ParticleProperty for the given name  
   *
   *  @code 
   *  
   *   const LHCb::ParticleProperty* pion = pid ( "pi+" ) ;
   *   
   *  @endcode 
   *
   *  @see LHCb::ParticleProperty
   *  @see LHCb::IParticlePropertySvc
   *  @param name the particle name 
   *  @return pointer to particle property 
   */
  inline const LHCb::ParticleProperty* pid ( const std::string& name ) const ;
  /** helper method to get a proper ParticleProperty for the given pid 
   *
   *  @code 
   *
   *   const LHCb::Particle* p = ... ;
   *
   *   const LHCb::ParticleProperty* pp = pid ( p->particleID() ) ;
   *   
   *  @endcode 
   *
   *  @see LHCb::ParticleProperty
   *  @see LHCb::IParticlePropertySvc
   *  @see LHCb::ParticleID
   *  @param  p the particle pid 
   *  @return pointer to particle property 
   */
  inline const LHCb::ParticleProperty* pid ( const LHCb::ParticleID& p ) const ;
  // ==========================================================================  
protected:
  
  
  template<class T> 
  bool container_exist(const std::string& location) 
  {
    typedef typename boost::remove_pointer<typename T::value_type>::type ContainedType;
    return exist<T>(location) ||
      exist<typename ContainedType::Container>(location) || 
      exist<typename ContainedType::Range>(location);
    
  }
  
  
  /** helper protected function to load the tool on-demand  
   *  @param name name of tool
   *  @param t    tool 
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
  } 

  /**
   * Access to the list of TES input locations given by the InputLocations
   * property
   *
   * @return vector or strings with TES input locations
   */
  inline const std::vector<std::string>& inputLocations() 
  {
    return m_inputLocations;
  }

protected:
  
  /// the actual tyep for mapping "tool nickname -> the actual type/name"
  typedef std::map<std::string,std::string> ToolMap     ;
  // typedef SimpleProperty<ToolMap>           ToolMapProp ;
  
  /** helper method to locate the tool by nickname 
   *
   *  @attention it is for internal usage ONLY, 
   *             used for implementation of 
   *             concrete accessor functions 
   *
   *  It is assumed that the map "nickname -> type/name" is performed 
   *  through the algoithm properties 
   * 
   *  @param nickName the nickname for the tool
   *  @param nameMap  the actual mapping "nickname -> type/name" 
   *                 (to be specified through the properties)
   *  @param toolMap  the actual storage of located tool, e.g. 
   *         std::map<std::string,TYPE*> or 
   *         Gaudi::Utils::VectorMap<std::string,TYPE*> 
   *  @param parent the parent of the tools 
   *  @return the located tool 
   */
  template <class TYPE, class STORAGE>
  TYPE* getTool ( const std::string& nickName      , 
                  const ToolMap&     nameMap       , 
                  STORAGE&           toolMap       ,  
                  const IInterface*  parent = NULL )  const
  {
    // look within the local list of already located tools of given type 
    typename STORAGE::iterator ifind = toolMap.find ( nickName ) ;
    // tool is in the list?
    if ( toolMap.end() != ifind ) 
    {
      TYPE* tool = ifind->second ;
      if ( 0 == tool ) 
      { Exception ( "getTool<" + System::typeinfoName( typeid ( TYPE ) ) 
                    + ">('" + nickName + "'): tool points to NULL" ) ; }
      return tool ;
    }
    // get the actual tool type 
    ToolMap::const_iterator iname = nameMap.find ( nickName ) ;
    // locate the tool 
    TYPE* t = tool<TYPE>
      ( nameMap.end() != iname ? iname->second : nickName , parent ) ;
    // add the located tool into the container 
    typename STORAGE::value_type value( nickName , t ) ;
    toolMap.insert( value ) ;
    //
    return t ;                                               // RETURN 
  } ;

  virtual StatusCode writeEmptyContainerIfNeeded() ;
  
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
  
  /// Mapping of "nickname -> type/name" for Vertex Fitters:
  ToolMap                                                    m_vertexFitNames ;
  /// The actual map of "nickname -> tool" for Vertex Fitters 
  mutable GaudiUtils::VectorMap<std::string,IVertexFit*>         m_vertexFits ;
  
protected: 

  /// Mapping of "nickname ->type/name" for Geometry Tools
  ToolMap                                                     m_geomToolNames ;
  /// The actual map of "nickname -> tool" for Geometry Tools 
  mutable GaudiUtils::VectorMap<std::string,IGeomDispCalculator*> m_geomTools ;
  
  /// Mapping of "nickname ->type/name" for Particle Filters
  ToolMap                                                       m_filterNames ;
  /// The actual map of "nickname -> tool" for Particle Filters 
  mutable GaudiUtils::VectorMap<std::string,IParticleFilter*>       m_filters ;
  
  /// Mapping of "nickname ->type/name" for Particle Combiners
  ToolMap                                             m_particleCombinerNames ;
  /// The actual map of "nickname -> tool" for Particle Combiners 
  mutable GaudiUtils::VectorMap<std::string,IParticleCombiner*> m_particleCombiners ;
  
  /// Mapping of "nickname ->type/name" for Particle Refitters
  ToolMap                                             m_particleReFitterNames ;
  /// The actual map of "nickname -> tool" for Particle Refitters 
  mutable GaudiUtils::VectorMap<std::string,IParticleReFitter*> m_particleReFitters ;

  /// Mapping of "nickname ->type/name" for Particle Refitters
  ToolMap                                             m_pvReFitterNames ;
  /// The actual map of "nickname -> tool" for Particle Refitters 
  mutable GaudiUtils::VectorMap<std::string,IPVReFitter*> m_pvReFitters ;

  
protected:
  
  /// Mapping of "nickname ->type/name" for mass-constrained fitters 
  ToolMap                                              m_massFitterNames ;
  /// The actual map of "nickname -> tool" for mass-constrained fitters 
  mutable GaudiUtils::VectorMap<std::string,IMassFit*> m_massFitters     ;
  
  /// Mapping of "nickname ->type/name" for lifetime fitters 
  ToolMap                                              m_lifetimeFitterNames   ;
  /// The actual map of "nickname -> tool" for lifetime fitters 
  mutable GaudiUtils::VectorMap<std::string,ILifetimeFitter*> m_lifetimeFitters ;

  /// Mapping of "nickname ->type/name" for direction fitters 
  ToolMap                                              m_directionFitterNames   ;
  /// The actual map of "nickname -> tool" for direction fitters 
  mutable GaudiUtils::VectorMap<std::string,IDirectionFit*> m_directionFitters ;
  
  /// mapping of "nickname -> tool" for distance calculators 
  ToolMap                                                         m_distanceCalculatorNames ;
  /// the actual map of "nickname -> distance calculator" 
  mutable GaudiUtils::VectorMap<std::string,IDistanceCalculator*> m_distanceCalculators     ;
  
protected: 
  
  /// Concrete type of CheckOverlap tool
  std::string m_checkOverlapName;
  /// Reference to CheckOverlap
  mutable ICheckOverlap* m_checkOverlap;
  
  /// Concrete Type of FlavourTagging tool
  std::string m_taggingToolName;
  /// Reference to FlavourTagging
  mutable IBTaggingTool* m_taggingTool;

  /// Reference to ParticleDescendants
  mutable IParticleDescendants* m_descendants;
  /// Concrete Type of ParticleDescendants  tool
  std::string m_descendantsName;

  /// Concrete type of CheckOverlap tool
  std::string m_writeSelResultName;
  /// Reference to CheckOverlap
  mutable IWriteSelResult* m_writeSelResult;

  /// 
  mutable IOnOffline* m_onOffline;

  ///
  mutable IRelatedPVFinder* m_pvRelator;
  
  
  /// Reference to ParticlePropertySvc
  mutable const LHCb::IParticlePropertySvc* m_ppSvc;

protected:

  /// Get the best related PV from the local relations table. Return 0 if 
  /// nothing is there. Does not invoke any calculations.
  const LHCb::VertexBase* getStoredBestPV(const LHCb::Particle* particle) const;

private:

  bool hasStoredRelatedPV(const LHCb::Particle* particle) const;

  inline bool multiPV() const 
  {
    return this->primaryVertices().size() > 1;
  }
  
  inline bool refitPVs() const 
  {
    return m_refitPVs;
  }

public:
  inline bool useP2PV() const 
  {
    return m_refitPVs ? true : ( !multiPV() ? false : m_useP2PV );
  }

  
private:
  /// Decay description (Property)
  std::string m_decayDescriptor;
  /// Avoid writing SelResult object in TES (Property)
  bool m_avoidSelResult;
  /// Avoid printing SelResult statistics 
  /// (cannot be switched off by OutputLevel)
  bool m_printSelResult;
  /// avoid the writeup of empty containers 
  bool m_avoidEmptyOutput ;

  /// Has setFilterPassed() already been called in current event?
  bool m_setFilterCalled;
  /// Count number of times selection filter is used 
  /// (= number of times alg is called)
  int m_countFilterWrite ;
  /// Number of passing events
  int m_countFilterPassed ;

  /// Re-fit PVs. Default: false.
  bool m_refitPVs;

  /// Do we have more than one PV in the event?
  bool m_multiPV;
  
  /// Do we want to use the Particle -> PV relations? Default: true.
  bool m_useP2PV;

  /// Do we want to write the Particle -> PV relations table to the TES?
  /// Default: true
  bool m_writeP2PV;

  /// Switch PreloadTools to false no to preload any tools.
  /// This will have the effect that they will be loaded on demand, when needed,
  /// at any event. This option is thus only recommended for use of DVAlgorithm
  /// to do something unrelated to physics analysis.
  bool m_preloadTools;
  /// InputLocations
  std::vector<std::string> m_inputLocations ;
  /// User-defined Particle->PV relations locations
  std::vector<std::string> m_p2PVInputLocations ;
  /// TES location of input PVs.
  std::string m_PVLocation;
  /// Don't use PVs
  bool m_noPVs;
};
// ==========================================================================
/*  Accessor for ParticlePropertySvc
 *  @see LHCb::ParticlePropertySvc 
 *  @return pointer to Particle Property Service 
 */
// ============================================================================
inline 
const LHCb::IParticlePropertySvc* 
DVAlgorithm::ppSvc() const 
{ 
  if ( 0 != m_ppSvc ) { return m_ppSvc ; }
  m_ppSvc = svc<LHCb::IParticlePropertySvc> ( "LHCb::ParticlePropertySvc" , true ) ;
  return m_ppSvc ;
}
// ============================================================================
/*  helper method to get a proper ParticleProperty for the given name  
 *  @param name the particle name 
 *  @return pointer to particle property 
 */
// ===========================================================================
inline 
const LHCb::ParticleProperty* 
DVAlgorithm::pid ( const std::string& name ) const 
{
  const LHCb::ParticleProperty* pp = ppSvc()->find( name ) ;
  if ( 0 == pp ) 
  { Error ( "pid('" + name + "') : invalid LHCb::ParticleProperty!" ) ; }
  return pp ;
} 
// ============================================================================
/** helper method to get a proper ParticleProperty for the given pid 
 *  @param  p the particle pid 
 *  @return pointer to particle property 
 */
inline 
const LHCb::ParticleProperty* 
DVAlgorithm::pid ( const LHCb::ParticleID& id ) const 
{
  const LHCb::ParticleProperty* pp = ppSvc()->find ( id ) ;
  if ( 0 == pp ) { Error ( "pid() : invalid LHCb::ParticleProperty!" ) ; }
  return pp ;
}
// ==========================================================================
// The END 
// ==========================================================================
#endif // DAVINCIKERNEL_DVALGORITHM_H
// ==========================================================================
