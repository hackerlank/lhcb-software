
// local
#include "Event/PackedCaloHypo.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

using namespace LHCb;

//-----------------------------------------------------------------------------

void CaloHypoPacker::pack( const DataVector & hypos,
                           PackedDataVector & phypos ) const
{
  phypos.hypos().reserve(hypos.size());

  for ( LHCb::CaloHypos::const_iterator itH = hypos.begin();
        hypos.end() != itH; ++itH )
  {
    // make new packed object
    phypos.hypos().push_back( LHCb::PackedCaloHypo() );
    LHCb::PackedCaloHypo & pH = phypos.hypos().back();

    // Save the data
    pH.key        = (*itH)->key();
    pH.hypothesis = (*itH)->hypothesis();
    pH.lh         = m_pack.fltPacked( (*itH)->lh() );
    if ( 0 == (*itH)->position() )
    {
      pH.z      = 0;
      pH.posX   = 0;
      pH.posY   = 0;
      pH.posE   = 0;
      pH.cov00  = 0;
      pH.cov10  = 0;
      pH.cov20  = 0;
      pH.cov11  = 0;
      pH.cov21  = 0;
      pH.cov22  = 0;
      pH.centX  = 0;
      pH.centY  = 0;
      pH.cerr00 = 0;
      pH.cerr10 = 0;
      pH.cerr11 = 0;
    }
    else
    {
      LHCb::CaloPosition* pos = (*itH)->position();
      pH.z    = m_pack.position( pos->z() );
      pH.posX = m_pack.position( pos->x() );
      pH.posY = m_pack.position( pos->y() );
      pH.posE = m_pack.energy  ( pos->e() );

      // convariance Matrix
      const double err0 = std::sqrt( pos->covariance()(0,0) );
      const double err1 = std::sqrt( pos->covariance()(1,1) );
      const double err2 = std::sqrt( pos->covariance()(2,2) );
      pH.cov00 = m_pack.position( err0 );
      pH.cov11 = m_pack.position( err1 );
      pH.cov22 = m_pack.energy  ( err2 );
      pH.cov10 = m_pack.fraction( pos->covariance()(1,0), err1*err0 );
      pH.cov20 = m_pack.fraction( pos->covariance()(2,0), err2*err0 );
      pH.cov21 = m_pack.fraction( pos->covariance()(2,1), err2*err1 );

      pH.centX = m_pack.position( pos->center()(0) );
      pH.centY = m_pack.position( pos->center()(1) );

      const double serr0 = std::sqrt( pos->spread()(0,0) );
      const double serr1 = std::sqrt( pos->spread()(1,1) );
      pH.cerr00 = m_pack.position( serr0 );
      pH.cerr11 = m_pack.position( serr1 );
      pH.cerr10 = m_pack.fraction( pos->spread()(1,0), serr1*serr0 );

    }

    //== Store the CaloDigits
    pH.firstDigit = phypos.refs().size();
    for ( SmartRefVector<LHCb::CaloDigit>::const_iterator itD = (*itH)->digits().begin();
          (*itH)->digits().end() != itD; ++itD )
    {
      if ( *itD )
      {
        phypos.refs().push_back( m_pack.reference( &phypos,
                                                   (*itD)->parent(),
                                                   (*itD)->key().all() ) );
      }
      else
      {
        parent().Warning( "Null CaloDigit SmartRef found" ).ignore();
      }
    }
    pH.lastDigit = phypos.refs().size();

    //== Store the CaloClusters
    pH.firstCluster = phypos.refs().size();
    for ( SmartRefVector<LHCb::CaloCluster>::const_iterator itC = (*itH)->clusters().begin();
          (*itH)->clusters().end() != itC; ++itC )
    {
      if ( *itC )
      {
        phypos.refs().push_back( m_pack.reference( &phypos,
                                                   (*itC)->parent(),
                                                   (*itC)->key() ) );
      }
      else
      {
        parent().Warning( "Null CaloCluster SmartRef found" ).ignore();
      }
    }
    pH.lastCluster = phypos.refs().size();

    //== Store the CaloHypos
    pH.firstHypo = phypos.refs().size();
    for ( SmartRefVector<LHCb::CaloHypo>::const_iterator itO = (*itH)->hypos().begin();
          (*itH)->hypos().end() != itO; ++itO )
    {
      if ( *itO )
      {
        phypos.refs().push_back( m_pack.reference( &phypos,
                                                   (*itO)->parent(),
                                                   (*itO)->key() ) );
      }
      else
      {
        parent().Warning( "Null CaloCluster CaloHypo found" ).ignore();
      }
    }
    pH.lastHypo = phypos.refs().size();

  }

}

void CaloHypoPacker::unpack( const PackedDataVector & phypos,
                             DataVector             & hypos ) const
{
  hypos.reserve( phypos.hypos().size() );

  for ( std::vector<LHCb::PackedCaloHypo>::const_iterator itS = phypos.hypos().begin();
        phypos.hypos().end() != itS; ++itS )
  {
    // packed object
    const LHCb::PackedCaloHypo& src = (*itS);

    // make new unpacked object
    LHCb::CaloHypo* hypo = new LHCb::CaloHypo( );
    hypos.insert( hypo, src.key );

    // fill data objects
    hypo->setHypothesis( (LHCb::CaloHypo::Hypothesis)src.hypothesis );
    hypo->setLh( m_pack.fltPacked( src.lh ) );
    if ( 0 != src.z )
    {
      LHCb::CaloPosition* pos = new LHCb::CaloPosition();
      hypo->setPosition( pos );
      pos->setZ( m_pack.position( src.z ) );
      pos->setParameters( LHCb::CaloPosition::Parameters( m_pack.position(src.posX),
                                                          m_pack.position(src.posY),
                                                          m_pack.energy  (src.posE) ) );

      LHCb::CaloPosition::Covariance & cov = pos->covariance();
      const double err0 = m_pack.position( src.cov00 );
      const double err1 = m_pack.position( src.cov11 );
      const double err2 = m_pack.energy  ( src.cov22 );
      cov(0,0) = err0 * err0;
      cov(1,0) = err1 * err0 * m_pack.fraction( src.cov10 );
      cov(1,1) = err1 * err1;
      cov(2,0) = err2 * err0 * m_pack.fraction( src.cov20 );
      cov(2,1) = err2 * err1 * m_pack.fraction( src.cov21 );
      cov(2,2) = err2 * err2;

      pos->setCenter( LHCb::CaloPosition::Center( m_pack.position(src.centX),
                                                  m_pack.position(src.centY) ) );

      LHCb::CaloPosition::Spread & spr = pos->spread();
      const double serr0 = m_pack.position( src.cerr00 );
      const double serr1 = m_pack.position( src.cerr11 );
      spr(0,0) = serr0 * serr0;
      spr(1,0) = serr1 * serr0 * m_pack.fraction( src.cerr10 );
      spr(1,1) = serr1 * serr1;
    }

    int hintID(0), key(0);
    for ( int kk = src.firstDigit; src.lastDigit > kk; ++kk )
    {
      const int reference = *(phypos.refs().begin()+kk);
      m_pack.hintAndKey( reference, &phypos, &hypos, hintID, key );
      SmartRef<LHCb::CaloDigit> ref( &hypos, hintID, key );
      hypo->addToDigits( ref );
    }
    for ( int kk = src.firstCluster; src.lastCluster > kk; ++kk )
    {
      const int reference = *(phypos.refs().begin()+kk);
      m_pack.hintAndKey( reference, &phypos, &hypos, hintID, key );
      SmartRef<LHCb::CaloCluster> ref( &hypos, hintID, key );
      hypo->addToClusters( ref );
    }
    for ( int kk = src.firstHypo; src.lastHypo > kk; ++kk )
    {
      const int reference = *(phypos.refs().begin()+kk);
      m_pack.hintAndKey( reference, &phypos, &hypos, hintID, key );
      SmartRef<LHCb::CaloHypo> ref( &hypos, hintID, key );
      hypo->addToHypos( ref );
    }

  }

}

StatusCode CaloHypoPacker::check( const DataVector & dataA,
                                  const DataVector & dataB ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  if ( dataA.size() != dataB.size() )
  {
    parent().err() << "Old CaloHypo size " << dataA.size()
                   << " differs form Test " << dataB.size()
                   << endmsg;
    return StatusCode::FAILURE;
  }

  LHCb::CaloHypos::const_iterator itOld  = dataA.begin();
  LHCb::CaloHypos::const_iterator itTest = dataB.begin();
  while ( dataA.end() != itOld )
  {
    LHCb::CaloHypo* oHypo = (*itOld++);
    LHCb::CaloHypo* tHypo = (*itTest++);
    if ( oHypo->key() != tHypo->key() ) 
    {
      parent().warning() << "Wrong key : old " <<  oHypo->key() 
                         << " test " << tHypo->key() << endmsg;
    }

    bool isOK = true;

    if ( oHypo->hypothesis() != tHypo->hypothesis() ) isOK = false;
    if ( 1.e-7 < fabs( (oHypo->lh() - tHypo->lh() ) / oHypo->lh() ) ) isOK = false;
    const LHCb::CaloPosition* oPos = oHypo->position();
    const LHCb::CaloPosition* tPos = tHypo->position();
    std::vector<double> oDiag, tDiag, oFrac, tFrac;
    if ( oPos == 0 && tPos != 0 ) isOK = false;
    if ( oPos != 0 && tPos == 0 ) isOK = false;
    if ( oPos != 0 && tPos != 0 )
    {
      if ( 5.e-5 < fabs( oPos->z() - tPos->z() ) ) isOK = false;
      if ( 5.e-5 < fabs( oPos->x() - tPos->x() ) ) isOK = false;
      if ( 5.e-5 < fabs( oPos->y() - tPos->y() ) ) isOK = false;
      if ( 5.e-3 < fabs( oPos->e() - tPos->e() ) ) isOK = false;

      if ( 5.e-5 < fabs( oPos->center()(0) - tPos->center()(0) ) ) isOK = false;
      if ( 5.e-5 < fabs( oPos->center()(1) - tPos->center()(1) ) ) isOK = false;

      oDiag.push_back( std::sqrt(oPos->covariance()(0,0)) );
      oDiag.push_back( std::sqrt(oPos->covariance()(1,1)) );
      oDiag.push_back( std::sqrt(oPos->covariance()(2,2)) );
      oDiag.push_back( std::sqrt(oPos->spread()(0,0)) );
      oDiag.push_back( std::sqrt(oPos->spread()(1,1)) );

      tDiag.push_back( std::sqrt(tPos->covariance()(0,0)) );
      tDiag.push_back( std::sqrt(tPos->covariance()(1,1)) );
      tDiag.push_back( std::sqrt(tPos->covariance()(2,2)) );
      tDiag.push_back( std::sqrt(tPos->spread()(0,0)) );
      tDiag.push_back( std::sqrt(tPos->spread()(1,1)) );

      if ( 5.e-5  < fabs( oDiag[0] - tDiag[0] ) ) isOK = false;
      if ( 5.e-5  < fabs( oDiag[1] - tDiag[1] ) ) isOK = false;
      if ( 5.e-3  < fabs( oDiag[2] - tDiag[2] ) ) isOK = false;
      if ( 5.e-5  < fabs( oDiag[3] - tDiag[3] ) ) isOK = false;
      if ( 5.e-5  < fabs( oDiag[4] - tDiag[4] ) ) isOK = false;

      oFrac.push_back( oPos->covariance()(1,0) / oDiag[1] / oDiag[0] );
      oFrac.push_back( oPos->covariance()(2,0) / oDiag[2] / oDiag[0] );
      oFrac.push_back( oPos->covariance()(2,1) / oDiag[2] / oDiag[1] );
      oFrac.push_back( oPos->spread()(1,0)     / oDiag[3] / oDiag[4] );

      tFrac.push_back( tPos->covariance()(1,0) / tDiag[1] / tDiag[0] );
      tFrac.push_back( tPos->covariance()(2,0) / tDiag[2] / tDiag[0] );
      tFrac.push_back( tPos->covariance()(2,1) / tDiag[2] / tDiag[1] );
      tFrac.push_back( tPos->spread()(1,0)     / tDiag[3] / tDiag[4] );

      for ( unsigned int kk = 0 ; oFrac.size() > kk ; ++kk ) 
      {
        if ( 2.e-5 < fabs( oFrac[kk] - tFrac[kk] ) ) isOK = false;
      }

    }
    if ( oHypo->digits().size()   != tHypo->digits().size()   ) 
    {
      isOK = false;
    }
    else
    {
      for ( unsigned int kk = 0; oHypo->digits().size() > kk; kk++ ) 
      {
        const LHCb::CaloDigit* dum  = oHypo->digits()[kk];  // convert smartref to pointers
        const LHCb::CaloDigit* dum1 = tHypo->digits()[kk];
        if ( dum != dum1 ) isOK = false;
      }
    }

    if ( oHypo->clusters().size() != tHypo->clusters().size() ) 
    {
      isOK = false;
    }
    else 
    {
      for ( unsigned int kk = 0; oHypo->clusters().size() > kk; kk++ ) 
      {
        const LHCb::CaloCluster* dum  = oHypo->clusters()[kk];  // convert smartref to pointers
        const LHCb::CaloCluster* dum1 = tHypo->clusters()[kk];
        if ( dum != dum1 ) isOK = false;
      }
    }

    if ( oHypo->hypos().size()    != tHypo->hypos().size()    ) 
    {
      isOK = false;
    }
    else
    {
      for ( unsigned int kk = 0; oHypo->hypos().size() > kk; kk++ ) 
      {
        const LHCb::CaloHypo* dum  = oHypo->hypos()[kk];  // convert smartref to pointers
        const LHCb::CaloHypo* dum1 = tHypo->hypos()[kk];
        if ( dum != dum1 ) isOK = false;
      }
    }

    // Force for tests
    //isOK = false;

    if ( !isOK || MSG::DEBUG >= parent().msgLevel() ) 
    {
      const std::string loc = ( dataA.registry() ?
                                dataA.registry()->identifier() : "Not in TES" );
      parent().warning() << "Problem with CaloHypo data packing :-" << endmsg
                         << "  Original CaloHypo key=" << oHypo->key() 
                         << " in '" << loc << "'" << endmsg
                         << *oHypo  << endmsg
                         << "  Unpacked CaloHypo" << endmsg
                         << *tHypo << endmsg;
    }

  }

  return sc;
}
