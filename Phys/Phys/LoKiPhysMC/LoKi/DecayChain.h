// $Id: DecayChain.h,v 1.1 2006-05-27 11:47:14 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_DECAYCHAIN_H 
#define LOKI_DECAYCHAIN_H 1
// ============================================================================
// Include files
// ============================================================================
#include "GaudiKernel/MsgStream.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/HepMCEvent.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/Kinematics.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/MCTypes.h"
// ============================================================================

namespace LoKi
{
  namespace Colors
  {
    /** set the colors for output stream (not active for WIN32)
     *  @param  stream  reference for stream 
     *  @param  fg      color for foreground 
     *  @param  fg      color for foreground 
     *  @return true if color are changed 
     */
    bool setColor ( std::ostream&     stream              , 
                    const MSG::Color& fg     = MSG::BLACK , 
                    const MSG::Color& bg     = MSG::WHITE ) ;
    /** set the colors for output stream (not actiev for WIN32)
     *  @param  stream  reference for stream 
     *  @param  fg      color for foreground 
     *  @param  fg      color for foreground 
     *  @return true if color are changed 
     */
    bool setColor ( MsgStream&        stream              , 
                    const MSG::Color& fg     = MSG::BLACK , 
                    const MSG::Color& bg     = MSG::WHITE ) ;
  }; 
  
  /** @class MarkStrem 
   *  helper utility class to "mark" the stream 
   *  @author Vanya BELYAEV ibelyaev@phsyics.syr.edu
   */
  template <class STREAM> 
  class MarkStream
  {
  public:
    MarkStream ( STREAM&           stream , 
                 const bool        mark   ,
                 const MSG::Color& fg     , 
                 const MSG::Color& bg     )
      : m_stream ( stream )
      , m_mark   ( mark   )
      , m_fg     ( fg     )
      , m_bg     ( bg     )
    { if ( m_mark )  {  LoKi::Colors::setColor ( m_stream , m_fg , m_bg ) ; } }
    //
    ~MarkStream  () { if( m_mark ) { LoKi::Colors::setColor ( m_stream ) ; } }
  private:
    MarkStream   ()       ;
  private:
    STREAM&     m_stream  ;
    bool        m_mark    ;
    MSG::Color  m_fg      ;
    MSG::Color  m_bg      ;
  };
  
  /** @class DecayChain DecayChain.h LoKi/DecayChain.h
   *
   *  The simple utility to print a decay chain for
   *  given (MC) particle or to sequence of (MC) particles.
   *  
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2003-05-14
   */
  class DecayChain 
  {
  public:
    // modes to print 4-vectors 
    enum Mode    
      { 
        LV_STD    = 0 , // default: as 4-values (E/px/py/pz) 
        LV_MASS       , //          as 5 values (M/E/px/py/pz)
        LV_WITHPT     , //          as 6 values (M/PT/E/px/py/pz) 
        LV_ONLYP      , //          as 3 values (px/py/pz) 
        LV_SHORT        //          as 3 values (M/PT/E) 
      } ;
  public: 
    typedef LoKi::BooleanConstant<const LHCb::Particle*>   Accept   ;    
    typedef LoKi::BooleanConstant<const LHCb::MCParticle*> AcceptMC ;
  public:
    
    /** Standard constructor
     *  @param maxDepth  maximal decay depth 
     *  @param vertex    vertex info to be printed 
     *  @param vertexe   end-vertex infor for MC particles to be printed 
     *  @param mode      mode for printout of 4-vectors 
     *  @param fg        color for foreground for 'marked' lines 
     *  @param bg        color for background for 'marked' lines 
     */
    DecayChain ( const size_t          maxDepth = 5            , 
                 const bool            vertex   = true         , 
                 const bool            vertexe  = false        ,
                 const Mode            mode     = LV_WITHPT    ,
                 const MSG::Color&     fg       = MSG::YELLOW  ,
                 const MSG::Color&     bg       = MSG::RED     ) ;
    
    virtual ~DecayChain() ;
    
  public:
    
    /// max depth 
    size_t            maxDepth    () const { return m_maxDepth ; }
    void              setMaxDepth (  const size_t      value )  
    { m_maxDepth   = value ; }
    /// print vertex info ?
    bool              vertex      () const { return m_vertex   ; }
    void              setVertex   (  const bool value ) 
    { m_vertex     = value ; }
    /// print momentum info ?
    bool              vertexe     () const { return m_vertexe  ; }
    void              setVertexE  (  const bool value ) 
    { m_vertexe    = value ; }
    /// color for foreground (for marked entities)
    const MSG::Color& fg          () const { return m_fg ; }
    void              setFG       (  const MSG::Color& value ) 
    { m_fg         = value ; }
    /// color for background (for marked entities)
    const MSG::Color& bg          () const { return m_bg ; }
    void              setBG       (  const MSG::Color& value ) 
    { m_bg         = value ; }
    const Mode        mode        () const { return m_mode ; }
    void              setMode     (  const Mode&  value ) 
    { m_mode = value ; }
    
    void setFmt_M  ( const std::string& value ) { m_fmt_m  = value ; }
    void setFmt_P  ( const std::string& value ) { m_fmt_p  = value ; }
    void setFmt_PT ( const std::string& value ) { m_fmt_pt = value ; }
    void setFmt_V  ( const std::string& value ) { m_fmt_v  = value ; }
    void setFmt_D  ( const std::string& value ) { m_fmt_d  = value ; }
    void setFmt_I  ( const std::string& value ) { m_fmt_i  = value ; }
    
  public:
    
    /** print the decay chain for Particle
     * 
     *  @code 
     *
     *    DecayChain dc ;
     *    const LHCb::Particle* head = ... ; 
     *
     *    /// print decay chain to standard output 
     *    dc.print ( head , std::cout , std::endl ) ;
     *    
     *    /// print decay chain to gausdi message stream
     *    MsgStream log( msgSvc() , name() ) ;
     *    dc.print ( head , log , endreq          ) ;
     *
     *  @endcode 
     * 
     *  @param particle pointer to the particle 
     *  @param stream   reference to output stream 
     *  @param term     stream terminator 
     */
    template < class STREAM     , 
               class TERMINATOR , 
               class ACCEPT     , 
               class MARK       > 
    STREAM& print ( const LHCb::Particle* particle       , 
                    STREAM&               stream         , 
                    TERMINATOR            term           ,
                    const ACCEPT&         accept         ,
                    const MARK&           mark           ,
                    const std::string&    prefix  = " "  ,
                    const size_t          depth   = 0    ) const 
    {
      // invalid particle 
      if ( 0 == particle ) 
      {
        Error ( " Particle* points to NULL" ) ; 
        stream << term  ; return stream ; 
      }
      // marked? 
      const bool marked = mark ( particle ) ;
      
      { // use colors :-))
        MarkStream<STREAM> ms ( stream , marked , fg() , bg() ) ;
        stream << ( marked ? '*' : ' ' )
               << depth 
               << prefix 
               << "|->" 
               << name( particle->particleID() ) 
               << std::string( 1 + 3 * labs( maxDepth() - depth ) , ' ' ) ;
      }
      
      if ( !marked && maxDepth() < depth   ) 
      { stream << " ... (max depth)     " << term ; return stream ; }
      if ( !marked && !accept ( particle ) ) 
      { stream << " ... (skip  by cuts) " << term ; return stream ; }
      
      stream << toString( particle->momentum() ) ;
      if ( particle -> hasKey() )  
      { stream << " #" << toString ( particle->key() ) ; }
      ///
      const LHCb::Vertex* vertex = particle->endVertex ();
      
      if ( m_vertex && 0 != vertex ) 
      { stream << " EndVertex " << toString( vertex->position() ) ; }
      // use the terminator 
      stream << term ;  
      
      typedef SmartRefVector<LHCb::Particle> Products     ;
      const Products& products = particle->daughters() ;
      print ( products.begin() , products.end() , 
              stream           , term           , 
              accept           , mark           ,              
              prefix + "   "   , depth + 1      ) ;     // RECURSION 
      
      return stream ;
    };
    
    /** print the decay chain for MCParticle
     * 
     *  @code 
     *
     *    DecayChain dc ;
     *    const LHCb::MCParticle* head = ... ; 
     *
     *    /// print decay chain to standard output 
     *    dc.print ( head , std::cout , std::endl ) ;
     *    
     *    /// print decay chain to gausdi message stream
     *    MsgStream log( msgSvc() , name() ) ;
     *    dc.print ( head , log , endreq          ) ;
     *
     *  @endcode 
     * 
     *  @param particle pointer to the particle 
     *  @param stream   reference to output stream 
     *  @param term     stream terminator 
     */
    template < class STREAM     , 
               class TERMINATOR , 
               class ACCEPT     ,
               class MARK       > 
    STREAM& print ( const LHCb::MCParticle*  particle       , 
                    STREAM&                  stream         , 
                    TERMINATOR               term           ,
                    const ACCEPT&            accept         ,
                    const MARK&              mark           ,
                    const std::string&       prefix  = " "  ,
                    const size_t             depth   = 0    ) const    
    {
      // invalid particle 
      if ( 0 == particle ) 
      { 
        Error ( " MCParticle* points to NULL" ) ; 
        stream << term  ; return stream ; 
      }
      const bool marked = mark( particle ) ;
      
      { // use colors :-))
        MarkStream<STREAM> ms ( stream , marked , fg() , bg() ) ;
        stream << ( marked ? '*' : ' ' )
               << depth 
               << prefix 
               << "|->" 
               << name( particle->particleID() ) 
               << std::string( 1 + 3 * labs( maxDepth() - depth ) , ' ' ) ;
      }
      
      if ( !marked && maxDepth() < depth   ) 
      { stream << " ... (max depth)     " << term ; return stream ; }
      if ( !marked && !accept ( particle ) ) 
      { stream << " ... (skip  by cuts) " << term ; return stream ; }
      
      stream << toString( particle->momentum() ) ;
      if ( particle -> hasKey() )  
      { stream << " #" << toString ( particle->key() ) ; }
      ///
      
      const LHCb::MCVertex* vertex = particle->originVertex ();
      if     ( m_vertex && 0 != vertex ) 
      {
        stream << " Origin x/y/z " << toString ( vertex -> position() ) ;
        if ( vertex -> hasKey() )  
        { stream << " #" << toString ( vertex -> key() ) ; }
      }
      
      // use the terminator 
      stream << term ;  
      
      typedef SmartRefVector<LHCb::MCVertex>   EndVertices ;
      const EndVertices& endVertices = particle->endVertices() ;
      // print all end-vertices 
      for ( EndVertices::const_iterator ev = endVertices.begin() ; 
            endVertices.end() != ev ; ++ev ) 
      {
        const LHCb::MCVertex* vertex = *ev ;
        if ( 0 == vertex ) { continue ; }
        if ( m_vertex && m_vertexe ) 
        {
          stream << std::string( 92 , ' ' ) ;          
          stream << " EndVtx " << toString( vertex -> position() ) ; 
          stream << " type "   << vertex->type()  ;
          if ( vertex -> hasKey() )  
          { stream << " #" << toString ( vertex -> key() ) ; }
          // use terminator 
          stream << term ; 
        }
        print ( vertex->products().begin ()  ,
                vertex->products().end   ()  ,  
                stream           , term      , 
                accept           , mark      ,
                prefix + "   "   , depth + 1 ) ;   // RECURSION 
      }
      
      return stream ;
    };    

    /** print the decay chain for HepMC::GenParticle
     * 
     *  @code 
     *
     *    DecayChain dc ;
     *    const HepMC::GenParticle* head = ... ; 
     *
     *    /// print decay chain to standard output 
     *    dc.print ( head , std::cout , std::endl ) ;
     *    
     *    /// print decay chain to gausdi message stream
     *    MsgStream log( msgSvc() , name() ) ;
     *    dc.print ( head , log , endreq          ) ;
     *
     *  @endcode 
     * 
     *  @param particle pointer to the particle 
     *  @param stream   reference to output stream 
     *  @param term     stream terminator 
     */
    template < class STREAM     , 
               class TERMINATOR , 
               class ACCEPT     ,
               class MARK       > 
    STREAM& print ( const HepMC::GenParticle*  particle       , 
                    STREAM&                    stream         , 
                    TERMINATOR                 term           ,
                    const ACCEPT&              accept         ,
                    const MARK&                mark           ,
                    const std::string&         prefix  = " "  ,
                    const size_t               depth   = 0    ) const    
    {
      // invalid particle 
      if ( 0 == particle ) 
      { 
        Error ( " HepMC::GenParticle* points to NULL" ) ; 
        stream << term  ; return stream ; 
      }
      const bool marked = mark( particle ) ;
      
      { // use colors :-))
        MarkStream<STREAM> ms ( stream , marked , fg() , bg() ) ;
        stream << ( marked ? '*' : ' ' )
               << depth 
               << prefix 
               << "|->" 
               << name( LHCb::ParticleID ( particle->pdg_id()  ) ) 
               << std::string( 1 + 3 * labs( maxDepth() - depth ) , ' ' ) ;
      }
      
      if ( !marked && maxDepth() < depth   ) 
      { stream << " ... (max depth)     " << term ; return stream ; }
      if ( !marked && !accept ( particle ) ) 
      { stream << " ... (skip  by cuts) " << term ; return stream ; }
      
      stream << toString( particle->momentum() ) ;
      stream << " #" << toString ( particle->barcode() ) ; 
      ///
      
      HepMC::GenVertex* vertex = particle->production_vertex ();
      if     ( m_vertex && 0 != vertex ) 
      {
        stream << " Origin x/y/z " 
               << toString ( vertex -> point3d() ) ;
        stream << " #" << toString ( vertex -> barcode() ) ; 
      }
      
      // use the terminator 
      stream << term ;  
      
      // print all end-vertices 
      HepMC::GenVertex* evertex = particle->end_vertex() ;
      if ( m_vertex && m_vertexe && 0 != evertex ) 
      { 
        stream << std::string( 92 , ' ' ) ;          
        stream << " EndVtx " 
               << toString( evertex -> point3d() ) ; 
        stream << " #" << toString ( evertex -> barcode() ) ; 
        // use terminator 
        stream << term ;
      }
      if ( 0 != evertex ) 
      {
        HepMC::GenVertex::particle_iterator begin = 
          evertex->particles_begin ( HepMC::children ) ;
        HepMC::GenVertex::particle_iterator end   = 
          evertex->particles_end   ( HepMC::children ) ;
        print ( begin  , end  , 
                stream , term , 
                accept , mark , 
                prefix + "   " , depth + 1 ) ; 
      }
      return stream ;
    };
    
    /** print decay chain for sequence of Particles/MCParticles/HepMCParticles
     * 
     *  @param first    begin of sequence to (MC/HepMC) particles 
     *  @param last     end   of sequence to (MC/HepMC) particles 
     *  @param stream   reference to output stream 
     *  @param term     stream terminator 
     */
    template < class PARTICLE    ,
               class STREAM      ,
               class TERMINATOR  ,
               class ACCEPT      ,
               class MARK        > 
    STREAM& print ( PARTICLE           first          ,
                    PARTICLE           last           ,
                    STREAM&            stream         , 
                    TERMINATOR         term           ,
                    const ACCEPT&      accept         ,
                    const MARK&        mark           ,
                    const std::string& prefix  = " "  ,
                    const size_t       depth   = 0    ) const    
    {
      for ( ; first != last ; ++first ) 
      { print ( *first , stream , term , accept , mark , prefix , depth ) ; }
      return stream ;
    };
    
  protected:
    
    /** particle name by ID
     *  @par pid particle ID 
     *  @return particle name 
     */
    const std::string name ( const LHCb::ParticleID& pid ) const ;
    
    // double as string
    const std::string toString ( const long              v ) const ;
    // int    as string
    const std::string toString ( const int               v ) const ;
    // long as string
    const std::string toString ( const double            v ) const ;
    
    // Lorentz vector as string
    const std::string toString ( const LoKi::LorentzVector& v , 
                                 const Mode&             m ) const ;
    // Lorentz vector as string
    const std::string toString ( const LoKi::LorentzVector& v ) const ;
    // 3D-point as string
    const std::string toString ( const LoKi::Point3D&       v ) const ;
    // 3D-vector as string
    const std::string toString ( const LoKi::Vector3D&      v ) const ;
    
    /** print error message 
     *  @param msg error message 
     *  @param sc status code
     *  @param mx number of prints 
     */
    StatusCode Error
    ( const std::string&  msg                       , 
      const StatusCode&   sc  = StatusCode::FAILURE ,
      const size_t        mx  = 10                  ) const ;
    
  protected:  /// @todo remove CLHEP methods
    
    /// Lorentz vector as string : due to HepMC
    inline const std::string toString 
    ( const CLHEP::HepLorentzVector& v ) const 
    { return toString 
        ( LoKi::LorentzVector ( v.x() , v.y() , v.z() , v.e() ) ) ; }
    /// 3D point  as string : due to HepMC
    inline const std::string toString 
    ( const HepGeom::Point3D<double>& v ) const 
    { return toString ( LoKi::Point3D( v.x() , v.y() , v.z() ) )  ; }
    
  private: 
    
    /// copy constructor is disabled
    DecayChain( const DecayChain& );
    
  private: 
    
    size_t                m_maxDepth ;
    bool                  m_vertex   ;
    bool                  m_vertexe  ;
    MSG::Color            m_fg       ;
    MSG::Color            m_bg       ;
    Mode                  m_mode     ;
    // format for "mass" 
    std::string           m_fmt_m    ; 
    // format for "PT" 
    std::string           m_fmt_pt   ;
    // format for "P/E" 
    std::string           m_fmt_p    ;
    // format for vertices 
    std::string           m_fmt_v    ;
    // general format for doubles 
    std::string           m_fmt_d    ;
    // general format for integers  
    std::string           m_fmt_i    ;
  };
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_DECAYCHAIN_H
// ============================================================================
