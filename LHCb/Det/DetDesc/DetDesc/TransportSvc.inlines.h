#ifndef    __DETDESC_TRANSPORTSVC_TRANSPORTSVC_INLINES_H__
#define    __DETDESC_TRANSPORTSVC_TRANSPORTSVC_INLINES_H__ 1 




///
///  inmplementation of inline functions of TransportSvc
///
/// Author Vanya Belyaev 
///

///
/// access to Chrono & Stat Service 
///
inline IChronoStatSvc*      TransportSvc::chronoSvc            () const { return m_chronoStatSvc    ; } ; 

///
///
/// access to Magnetic Field  Service 
///
inline IMagneticFieldSvc*   TransportSvc::magFieldSvc          () const { return m_magFieldSvc      ; } ;

///
///
/// access to Detector Data  Service 
///
inline IDataProviderSvc*    TransportSvc::detSvc               () const { return m_detDataSvc       ; } ;


///
///
/// access to the top of standard detector geometry information 
///
inline IGeometryInfo*       TransportSvc::standardGeometry     () const 
{ return ( 0 != m_standardGeometry ) ? m_standardGeometry : m_standardGeometry = findGeometry( m_standardGeometry_address ) ; }

///
///
/// access to the top of standard detector geometry information 
///
inline IGeometryInfo*       TransportSvc::previousGeometry     () const { return m_previousGeometry ; }

///
///
///
inline IGeometryInfo*       TransportSvc::setPreviousGeometry  ( IGeometryInfo* previous ) 
{ m_previousGeometry = previous ; return previousGeometry() ; }


///
/// Assertion !!!
///

inline void TransportSvc::Assert( bool                assertion  , 
				  const std::string&  Message    , 
				  const StatusCode&   statusCode ) const 
{ if( !assertion ) { throw TransportSvcException( Message , statusCode ); } };

///
///
///

inline void TransportSvc::Assert( bool                  assertion  , 
				  const std::string&    Message    ,
                                  const GaudiException& Exception  ,  
				  const StatusCode&     statusCode ) const 
{ if( !assertion ) { throw TransportSvcException( Message , Exception , statusCode ); } };

///
///
///


#endif  // __DETDESC_TRANSPORTSVC_TRANSPORTSVC_INLINES_H__
