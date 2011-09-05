#ifndef TALIGNMENT_ALDOFMASK_H
#define TALIGNMENT_ALDOFMASK_H

#include <boost/array.hpp>
#include <vector>
#include <algorithm>

class AlDofMask
{
public:
  typedef std::vector<bool> MaskType ;

  /** Constructors */
  AlDofMask() {}
  AlDofMask(size_t size, bool isactive = true) ;
  template<class Container> AlDofMask(const Container& mask) ;
  
  // number of active (unmasked) parameters
  size_t nActive() const { return m_activetoallmap.size() ; }
  // transform from an 'active' parameter to an 'all' parameter
  int parIndex(size_t iactivepar) const { assert(iactivepar<m_activetoallmap.size()) ; return m_activetoallmap[iactivepar] ; }
  // transform from an 'all' parameter to an 'active' parameter
  int activeParIndex(size_t ipar) const { assert(ipar<m_mask.size()); return m_alltoactivemap[ipar] ; }
  // 
  bool isActive(size_t ipar) const { assert(ipar<m_mask.size()) ; return m_mask[ipar] ; }
  //
  void setActive(size_t ipar, bool b) { if( b != isActive(ipar) ) { m_mask[ipar] = b ; fillIndexMaps() ; } }
  //
  typedef MaskType::const_iterator const_iterator ;
  const_iterator begin() const { return m_mask.begin() ; }
  const_iterator end()   const { return m_mask.end() ; }
  size_t size() const { return m_mask.size() ; }
  void push_back( bool mask ) { m_mask.push_back(mask) ; fillIndexMaps() ; }
  void push_back( const AlDofMask& mask ) ;
  
private:
  // fill the maps that transform betweem 'active' and 'all'
  void   fillIndexMaps() ;
  
private:
  MaskType                   m_mask;
  std::vector<int>           m_alltoactivemap ;
  std::vector<unsigned char> m_activetoallmap; 
} ;

inline AlDofMask::AlDofMask(size_t size, bool active) 
  : m_mask(size,active)
{
  fillIndexMaps() ;
}

template <class Container> 
AlDofMask::AlDofMask(const Container& mask)
  : m_mask(mask.size(),true)
{
  for(size_t i=0; i<m_mask.size(); ++i) m_mask[i] = mask[i] ;
  fillIndexMaps() ;
}

inline void AlDofMask::fillIndexMaps() 
{
  m_activetoallmap.clear() ;
  m_alltoactivemap.clear() ;
  if( m_mask.size()>0) {
    for(unsigned int ipar=0; ipar<m_mask.size(); ++ipar) 
      if( m_mask[ipar] ) m_activetoallmap.push_back(ipar) ;
    m_alltoactivemap.resize(size(),-1) ;
    for(unsigned int iactive=0; iactive<m_activetoallmap.size(); ++iactive)
      m_alltoactivemap[m_activetoallmap[iactive]] = iactive ;
  }
}


inline void AlDofMask::push_back( const AlDofMask& rhs ) 
{
  m_mask.insert(m_mask.end(), rhs.m_mask.begin(), rhs.m_mask.end() ) ;
  fillIndexMaps() ;
}

#endif
