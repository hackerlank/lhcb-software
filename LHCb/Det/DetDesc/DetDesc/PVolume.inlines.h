#ifndef     __DETDESC_VOLUMES_PVOLUME_INLINES_H__
#define     __DETDESC_VOLUMES_PVOLUME_INLINES_H__ 1 


///
///   niline methods from PVolume class
///
///  Author:: Vanya Belyaev


inline const std::string&    PVolume::name       () const { return m_pv_name             ;};

inline const std::string&    PVolume::lvolumeName() const { return m_pv_lvname           ;};

inline       ILVolume*       PVolume::lvolume    () const 
{ return  ( (0 != m_pv_lvolume ) ? m_pv_lvolume : findLogical() ); } 

inline const HepTransform3D& PVolume::matrix     () const { return m_pv_matrix           ;}; 

inline       HepTransform3D  PVolume::matrixInv  () const { return matrix().inverse()    ;};

inline       HepPoint3D      PVolume::toLocal    ( const HepPoint3D& PointInMotherFrame ) const { return matrix()*PointInMotherFrame; };

inline       HepPoint3D      PVolume::toMother   ( const HepPoint3D& PointInLocalFrame  ) const { return matrixInv()*PointInLocalFrame; };

inline       bool            PVolume::isInside   ( const HepPoint3D& PointInMotherFrame ) const { return lvolume()->isInside( toLocal(PointInMotherFrame) ) ;  };

inline       std::ostream&   PVolume::printOut   ( std::ostream& os ) const { return os << " PVolume:: name=" << name() << " is a LVolume=" << lvolumeName(); };

inline       MsgStream&      PVolume::printOut   ( MsgStream&    os ) const { return os << " PVolume:: name=" << name() << " is a LVolume=" << lvolumeName(); };


///

inline void              PVolume::Assert( bool assertion , const std::string& name ) const 
{ if( !assertion ) { throw PVolume::PVolumeException( name, this             ); } };

inline void              PVolume::Assert( bool assertion , const std::string& name , const GaudiException& Exception ) const 
{ if( !assertion ) { throw PVolume::PVolumeException( name, Exception , this ); } };

inline IMessageSvc*      PVolume::msgSvc         () const { return m_pv_messageSvc; }; 

inline IDataProviderSvc* PVolume::dataSvc        () const 
{ 
  Assert( 0 != m_pv_dataSvc , " PVolume::dataSvc() is NULL! " );
  return m_pv_dataSvc; 
}; 

//
//
//

///
///
/// reset to the initial state
inline const IPVolume*  PVolume::reset() const
{
  if( 0 != m_pv_lvolume ) { delete m_pv_lvolume; m_pv_lvolume = 0; }
  return this;
};
///
///
///


///
/// intersection of the physical volume with with line.
/// line is parametrized in the local reference system of the mother 
/// logical volume by initial Point and direction Vector.
/// Method returns the number of intersection points("ticks") and 
/// the container of pairs - ticks and pointer to the corresponding 
/// material. The simplification is determined by value of threshold
/// (in units of radiation length) 
/// Method throws PVolumeException in the case, then 
/// logical volume is not accessible    
inline  unsigned int PVolume::intersectLine( const HepPoint3D        & Point         ,   // initial point at the line 
					     const HepVector3D        & Vector        ,   // direction vector of the line 
					     ILVolume::Intersections & intersections ,   // output container 
					     const double              threshold     )   // threshold value 
{
  Assert( 0 != lvolume() , "PVolume::intersectLine:: 0==lvolume() for "+lvolumeName()+"/"+name()); 
  return lvolume()->intersectLine( matrix() * Point , matrix() * Vector , intersections , threshold );
};

///
/// the same as previos method, but the intersection points ("ticks") are searched 
/// in the region  tickMin<= tick <= tickMax 
inline  unsigned int PVolume::intersectLine( const HepPoint3D        & Point         ,   // initial point at the line 
					     const HepVector3D        & Vector        ,   // direction vector of the line 
					     ILVolume::Intersections & intersections ,   // output container 
					     const ISolid::Tick        tickMin       ,   // minimum value of possible Tick
					     const ISolid::Tick        tickMax       ,   // maximum value of possible Tick
					     const double              threshold     )   // threshold value 
{
  Assert( 0 != lvolume() , "PVolume::intersectLine:: 0==lvolume() for "+lvolumeName()+"/"+name()); 
  return lvolume()->intersectLine( matrix() * Point , matrix() * Vector , intersections , tickMin , tickMax , threshold );
};

///
///
///


#endif  //  __DETDESC_VOLUMES_PVOLUME_INLINES_H__
