// $Id: $
#ifndef HLT1MUONHIT_H 
#define HLT1MUONHIT_H 1

// Gaudi
#include <GaudiKernel/Range.h>
#include <GaudiKernel/boost_allocator.h>

// LHCb
#include <Kernel/MuonTileID.h>

/** @class Hlt1MuonHit Hlt1MuonHit.h
 *  
 *
 *  @author Roel Aaij
 *  @date   2010-12-02
 */
class Hlt1MuonHit {

public:

   Hlt1MuonHit( const LHCb::MuonTileID& tile,
                const double x, const double dx,
                const double y, const double dy,
                const double z, const double dz )
      : m_tile( tile ), m_station( tile.station() ),
        m_x( x ), m_dx( dx ),
        m_y( y ), m_dy( dy ),
        m_z( z ), m_dz( dz )
   {

   }

   virtual ~Hlt1MuonHit() {}

   const LHCb::MuonTileID& tile() const
   {
      return m_tile;
   }

   unsigned int station() const
   {
      return m_station;
   }

   double x() const
   {
      return m_x;
   }

   double dx() const
   {
      return m_dx;
   }

   double y() const
   {
      return m_y;
   }

   double dy() const
   {
      return m_dy;
   }

   double z() const
   {
      return m_z;
   }

   double dz() const
   {
      return m_dz;
   }

   /// operator new
   static void* operator new ( size_t size )
   {
      return ( sizeof(Hlt1MuonHit) == size ? 
               boost::singleton_pool<Hlt1MuonHit, sizeof(Hlt1MuonHit)>::malloc() :
               ::operator new(size) );
   }
  
   /// operator delete
   static void operator delete ( void* p )
   {
      boost::singleton_pool<Hlt1MuonHit, sizeof(Hlt1MuonHit)>::is_from(p) ?
         boost::singleton_pool<Hlt1MuonHit, sizeof(Hlt1MuonHit)>::free(p) :
         ::operator delete(p);
   }
  
protected:

   LHCb::MuonTileID m_tile;

   unsigned int m_station;
   double m_x;
   double m_dx;
   double m_y;
   double m_dy;
   double m_z;
   double m_dz;

private:
   
   Hlt1MuonHit();
   
};

typedef std::vector< Hlt1MuonHit* > Hlt1MuonHits;
typedef std::vector< const Hlt1MuonHit* > Hlt1ConstMuonHits;
typedef const Gaudi::Range_< Hlt1MuonHits > Hlt1MuonHitRange;
typedef const Gaudi::Range_< Hlt1ConstMuonHits > Hlt1ConstMuonHitRange;

#endif // HLT1MUONHIT_H
