#ifndef    __DETDESC_VOLUMES_LVOLUMEINLINES_H__ 
#define    __DETDESC_VOLUMES_LVOLUMEINLINES_H__  1 



///
///
///  implementation of inline methods from LVolume class 
///
///


///
///  from IValidity Interface:
///

inline bool LVolume::isValid ()                 { return validSince() <           validTill() ;};
inline bool LVolume::isValid ( const ITime& t ) { return validSince() < t &&  t < validTill() ;};

///
///
///
inline const  ISolid*      LVolume::solid       () const { return m_lv_solid        ;};
inline const  std::string& LVolume::materialName() const { return m_lv_materialName ;}; 
inline const  Material*    LVolume::material    ()
{       
  if ( 0 != m_lv_material ) { return m_lv_material; }
  ///
  m_lv_material = findMaterial() ; 
  ///
  Assert( 0 != m_lv_material , "LVolume::material(), could not locate material name="+materialName() );
  ///
  return m_lv_material;
};


///
/// number of daughters 
///

inline ILVolume::ReplicaType  LVolume::noPVolumes() const { return m_lv_pvolumes.size(); }  
///

///
/// get daughter by index 
///

inline IPVolume*  LVolume::operator[]( const ILVolume::ReplicaType& index ) const
{ return ( ( index < noPVolumes() ) ? *(pvBegin()+index) : 0 );  };
///

///
/// get daughter by name 
///

inline IPVolume*  LVolume::operator[]( const std::string&           name  ) const
{ 
  ILVolume::PVolumes::const_iterator pvi = std::find_if( pvBegin() , pvEnd()   , IPVolume_byName( name ) );
  return ( ( pvi != pvEnd() ) ? *pvi : 0 ); 
};
/// 

///
/// get daughter by index 
///

inline IPVolume* LVolume::pvolume( const ILVolume::ReplicaType& index ) const { return (*this)[ index ]; }; 

///
/// get daughter by name 
/// 
inline IPVolume* LVolume::pvolume( const std::string&           name  ) const { return (*this)[ name  ]; }; 

///
/// translate replica path into PVolume path 
///

inline StatusCode LVolume::traverse ( const ILVolume::ReplicaPath&  path , ILVolume::PVolumePath& pVolumePath )
{ return traverse( path.begin() , path.end() , pVolumePath ); }
///

///
/// translate replicapath into PVolume path 
///

inline StatusCode LVolume::traverse ( ILVolume::ReplicaPath::const_iterator pathBegin,
				      ILVolume::ReplicaPath::const_iterator pathEnd  ,
				      ILVolume::PVolumePath&                pVolumePath )
{
  // optimized implementation, using functional IPVolume_fromReplica(); 
  // if( pathBegin ==  pathEnd ){ return StatusCode::SUCCESS; }  
  // IPVolume_fromReplica PV (this); 
  // pVolumePath.clear();
  // std::transform( pathBegin, pathEnd, std::back_inserter(pVolumePath), PV ); 
  // if( pVolumePath.end() != std::find( pVolumePath.begin() , pVolumePath.end() , (IPVolume* ) 0 ) )
  //  { pVolumePath.clear() ; return StatusCode::FAILURE; } 
  // return StatusCode::SUCCESS; 
  
  // standard implementation 
  if( pathBegin ==  pathEnd ){ return StatusCode::SUCCESS; }  
  //
  ILVolume::ReplicaType replica = *pathBegin;
  if( replica >= noPVolumes()       ) { pVolumePath.clear() ; return StatusCode::FAILURE; } 
  //
  IPVolume* pv =  (*this)[replica]; 
  if( 0 == pv || 0 == pv->lvolume() ) { pVolumePath.clear() ; return StatusCode::FAILURE; }
  //
  pVolumePath.push_back( pv );  
  return pv->lvolume()->traverse( pathBegin+1 , pathEnd , pVolumePath ); 
};


///
///
/// the notorious "isInside" method
///

inline bool LVolume::isInside ( const HepPoint3D& LocalPoint ) const { return solid()->isInside( LocalPoint ); };

///
///
///

///
///
///

inline StatusCode LVolume::belongsTo( const HepPoint3D&        LocalPoint ,
				      const int                Level      , 
				      ILVolume::PVolumePath&   pVolumePath )
{
  //
  if( 0 == Level ) { return StatusCode::SUCCESS; } // no searched is required
  // 
  if( !isInside(LocalPoint) )         { pVolumePath.clear() ; return StatusCode::FAILURE; } 
  //
  ILVolume::PVolumes::const_iterator ppi = 
    std::find_if( pvBegin() , pvEnd  () , IPVolume_isInside( LocalPoint ) ); 
  // 
  if( pvEnd() == ppi )                { return StatusCode::SUCCESS; }  // the last level :-))) 
  //
  IPVolume* pv = *ppi;
  if( 0 == pv || 0 == pv->lvolume() ) { pVolumePath.clear() ; return StatusCode::FAILURE; } 
  //
  pVolumePath.push_back( pv ) ;
  return pv->lvolume()->belongsTo( pv->matrix()*LocalPoint , Level - 1 , pVolumePath );
};


///
///
///

inline StatusCode LVolume::belongsTo( const HepPoint3D&        localPoint ,
				      const int                level      , 
				      ILVolume::ReplicaPath&   replicaPath )
{
  //
  if( 0 == level ) { return StatusCode::SUCCESS; } // no searched is required
  // 
  if( !isInside(localPoint) ){ replicaPath.clear() ; return StatusCode::FAILURE; } 
  //
  ILVolume::PVolumes::const_iterator ppi = 
    std::find_if( pvBegin() , pvEnd  () , IPVolume_isInside( localPoint ) ); 
  // 
  if( pvEnd() == ppi ) { return StatusCode::SUCCESS; }  // the last level :-))) 
  //
  ILVolume::ReplicaType replica = ppi - pvBegin();
  //
  IPVolume* pv = *ppi;
  if( 0 == pv || 0 == pv->lvolume() ) { replicaPath.clear() ; return StatusCode::FAILURE; } 
  //
  replicaPath.push_back( replica ) ;
  return pv->lvolume()->belongsTo( pv->matrix() * localPoint , level - 1 , replicaPath );
};


///
/// pritout to std::ostream 
///

inline std::ostream& LVolume::printOut( std::ostream& os ) const
{
  os << " LVolume:: name=" << fullpath    () 
     << " material="      << materialName() 
     << " solid=("        << solid       () << ")"
     << " #PVolumes="     << noPVolumes  ()
     << "(" ;
  std::copy( pvBegin() , pvEnd(), std::ostream_iterator<IPVolume*>(os,";") );
  os << ")";
  //
  return os; 
};

///
///
/// reset to the initial state
inline const ILVolume* LVolume::reset () const 
{
  /// reset solid
  if( 0 != solid() ) { solid()->reset(); } 
  /// reset material
  m_lv_material = 0 ; 
  /// reset physical pholumes
  //
  /// due to stupid NT-compiler we are forced to write ugli stuff instead nice lines!
  // NT must die!!! 
  /// std::for_each( pvBegin() , pvEnd() , std::mem_fun(&IPVolume::reset) );
  for( ILVolume::PVolumes::const_iterator iter = pvBegin() ; iter != pvEnd() ; ++iter ) { if( 0 != (*iter) ) { (*iter)->reset(); } }
  /// 
  return this;
};
///
///
///


//
// pure internal technical 
//
inline ILVolume::PVolumes::const_iterator LVolume::pvBegin() const { return m_lv_pvolumes.begin(); };
inline ILVolume::PVolumes::iterator       LVolume::pvBegin()       { return m_lv_pvolumes.begin(); };
inline ILVolume::PVolumes::const_iterator LVolume::pvEnd  () const { return m_lv_pvolumes.end  (); };  
inline ILVolume::PVolumes::iterator       LVolume::pvEnd  ()       { return m_lv_pvolumes.end  (); };  

///
/// assertion 
///

inline void LVolume::Assert( bool assertion , const std::string& name , const StatusCode& sc ) const
{ if( !assertion ) throw LVolume::LVolumeException( name, this , sc ); };

///
/// assertion 
///

inline void LVolume::Assert( bool assertion , const std::string& name , const GaudiException& Exception , const StatusCode& sc ) const
{ if( !assertion ) throw LVolume::LVolumeException( name, Exception , this , sc ); };


///
///
/// get the message service 
inline IMessageSvc*      LVolume::msgSvc() const { return m_lv_messSvc; } 

///
///
///

#endif  // __DETDESC_VOLUMES_LVOLUMEINLINES_H__ 
