// $Id: ClusterFunctors.h,v 1.3 2002-04-02 10:59:30 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2001/12/02 14:59:49  ibelyaev
//  add new functor for z-position of cluster
//
// Revision 1.1.1.1  2001/11/02 14:39:53  ibelyaev
// New package: The first commit into CVS
//
// Revision 1.5  2001/10/28 19:14:09  ibelyaev
// update for newer CaloEvent package
//
// Revision 1.4  2001/07/17 20:00:48  ibelyaev
// modifications to improve Doxygen documentation
//
// Revision 1.3  2001/07/12 21:54:22  ibelyaev
// adaptation for Win2K
//
// Revision 1.2  2001/07/07 12:58:59  ibelyaev
// new clusterisation algorithm
//
// Revision 1.1  2001/07/04 18:15:53  ibelyaev
// Shared cell algorimth implementation
//
// ============================================================================
#ifndef CALOUTILS_CLUSTERFUNCTORS_H
#define CALOUTILS_CLUSTERFUNCTORS_H 1
// ============================================================================
// Include files
// CaloDet 
#include "CaloDet/DeCalorimeter.h"
// CaloEvent
#include "Event/CaloCluster.h"
#include "Event/CaloDataFunctor.h"

class DeCalorimeter;

/** @namespace ClusterFunctors ClusterFunctors.h CaloUtils/ClusterFunctors.h
 *
 *  collection of useful functors for dealing with
 *  CaloCluster objects
 *
 *  @author Ivan Belyaev
 *  @date   04/07/2001
 */

namespace ClusterFunctors
{
  
  typedef CaloCluster::Entries::iterator            iterator            ;
  typedef CaloCluster::Entries::const_iterator      const_iterator      ;
  typedef std::pair<iterator,iterator>              iterator_pair       ;
  typedef std::pair<const_iterator,const_iterator>  const_iterator_pair ;
  
  /** @fn throwException 
   *  throw the exception
   *  @exception CaloException
   *  @param message exception message
   *  @return status code (fictive)
   */
  StatusCode throwException( const std::string& message );
  
  /** @function energy
   * 
   *  Calculate the "energy" of the cluster as a sum of
   *  energies of its digits, weighted with energy fractions
   *  @param   cl  pointer to cluster
   *  @return      "energy" of cluster
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   xx/xx/xxxx
   */
  double  energy( const CaloCluster* cl );

  /** @function energy
   * 
   *  Calculate the "energy" of the cluster as a sum of
   *  energies of its digits, weighted with energy fractions
   *
   *  "IT" could be either iterator or const_iterator
   *
   *  @param    begin iterator to first element of sequence
   *  @param    end   iterator to last+1 element of sequance
   *  @return      "energy" of sequence
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   xx/xx/xxxx
   */
  template <class IT>
  inline  double  energy( IT begin , IT end ) 
  { 
    return CaloDataFunctor::clusterEnergy( begin, end ); 
  }

  /** @function calculateEXY 
   * 
   *  Calculate the "energy", X and Y position
   *  of the cluster as a sum of energies/x/y of 
   *  its digits, weighted with energy fractions
   *  @param   cl  pointer to cluster object
   *  @param   de  pointer to DeCalorimeter object
   *  @param   e   energy
   *  @param   x   x-position
   *  @param   y   y-position
   *  @return    status code
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   xx/xx/xxxx
   */
  StatusCode  calculateEXY( const  CaloCluster*   cl ,
                            const  DeCalorimeter* de ,
                            double& e                ,
                            double& x                ,
                            double& y                );
  
  /** @function calculateEXY 
   * 
   *  Calculate the "energy", X and Y position
   *  of the cluster as a sum of
   *  energies/x/y of its digits,
   *  weighted with energy fractions
   *
   *  "IT" could be either iterator or const_iterator
   *
   *  @param   begin iterator to first element of sequence
   *  @param   end   iterator to last+1 element of sequance
   *  @param   de  pointer to DeCalorimeter object
   *  @param   e   energy
   *  @param   x   x-position
   *  @param   y   y-position
   *  @return    status code
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   xx/xx/xxxx
   */
  template <class IT, class DE>
  inline StatusCode  calculateEXY( IT begin  , 
                                   IT end    , 
                                   DE de     ,
                                   double& e , 
                                   double& x , 
                                   double& y )
  {
    return CaloDataFunctor::calculateClusterEXY( begin , 
                                                 end   , 
                                                 de    , 
                                                 e     , 
                                                 x     , 
                                                 y     ) ;
  };
  
  /** @function overlapped
   * 
   *  useful function to determine, if clusters have
   *  at least one common cell.
   *
   *  For invalid arguments return "false"
   *
   *  @param   cl1   pointer to first  cluster
   *  @param   cl2   pointer to second cluster
   *  @return "true" if clusters have at least 1 common cell
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   xx/xx/xxxx
   */
  bool overlapped( const CaloCluster* cl1 ,
                   const CaloCluster* cl2 ) ;

  /** @function commonDigit
   *
   *  useful function to find first common digit
   *  from two sequences. It returns the pair of
   *  iterators (first one for first sequence and
   *  the secons one for second sequences).
   *
   *  "IT" could be either iterator or const_iterator
   *
   *  @param  begin1 iterator pointing to 1st    element of 1st sequence
   *  @param  end1   iterator pointing to last+1 element of 1st sequence
   *  @param  begin2 iterator pointing to 1st    element of 2nd sequence
   *  @param  end2   iterator pointing to last+1 element of 2nd sequence
   *  @return pair of iterators
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   xx/xx/xxxx
   */
  template<class IT>
  inline std::pair<IT,IT> commonDigit( IT begin1 , IT end1 ,
                                       IT begin2 , IT end2 )
  {
    return CaloDataFunctor::clusterCommonDigit( begin1 , 
                                                end1   , 
                                                begin2 , 
                                                end2   );
  };
  
  /** @function locateDigit
   *  
   *  useful function to locate the digit within the sequence
   *
   *  "IT" could be either iterator or const_iterator
   *
   *  @param begin iterator pointing to the 1st    element of sequence
   *  @param end   iterator pointing to teh last+1 element of sequence
   *  @param digit pointer to CaloDigit
   *  @return location of digit within the sequence
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   xx/xx/xxxx
   */
  template<class IT>
  inline IT locateDigit( IT begin               ,  
                         IT end                 ,
                         const CaloDigit* digit )
  {
    return CaloDataFunctor::clusterLocateDigit( begin , end , digit );
  };

  /** @function locateDigit
   * 
   *  useful function to locate the digit with given status
   *  from sequence of digits
   *
   *  "IT" could be either iterator or const_iterator
   *
   *  @param begin  iterator pointing to the 1st    element of sequence
   *  @param end    iterator pointing to teh last+1 element of sequence
   *  @param st     status
   *  @return location of digit within the sequence
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   xx/xx/xxxx
   */
  template <class IT>
  inline IT locateDigit( IT begin                          , 
                         IT end                            ,
                         const CaloDigitStatus::Status& st )
  {
    return CaloDataFunctor::clusterLocateDigit( begin, end , st );
  };
  
  /** @class ClusterArea 
   * 
   *  The simple class/function to get the index of area in Calo  
   *  "calo-area" of cluster is defined as "calo-area" index of seed cell
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   02/12/2001
   */
  class  ClusterArea
    : public std::unary_function<const CaloCluster*,const unsigned int>
  {
  public:
    
    /** the only one essential method 
     *  @exception CaloException for invalid cluster 
     *  @param cluster pointer to CaloCluster object 
     *  @return index of calorimeter area for given cluster 
     */
    const unsigned int operator() ( const CaloCluster* cluster ) const 
    {
      if( 0 == cluster ) 
        { Exception( " CaloCluster* points to NULL! "); }
      // find seed cell
      const_iterator seed = 
        locateDigit( cluster->entries().begin () , 
                     cluster->entries().end   () , 
                     CaloDigitStatus::SeedCell   );
      if ( cluster->entries().end() == seed  ) 
        { Exception( " 'SeedCell' is not found!"); }
      const CaloDigit* digit = seed->digit() ;
      if( 0 == digit ) 
        { Exception( " CaloDigit* points to NULL for seed!"); }
      // get the area
      return digit->cellID().area();
    };
  protected:    
    /** throw the exception
     *  @see CaloException 
     *  @exception CaloException 
     *  @param     message exception message
     *  @return    status code (fictive)
     */
    StatusCode Exception( const std::string& message ) const 
    {
      throwException( "ClusterArea() " + message );
      return StatusCode::FAILURE ;
    };
  };
  
  /** @fn clusterArea 
   * 
   *  The simple class/function to get the index of area in Calo  
   *  "calo-area" of cluster is defined as "calo-area" index of seed cell
   *
   *  @see ClusterArea 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   02/12/2001
   */
  inline const unsigned int clusterArea( const CaloCluster* cluster )
  {
    ClusterArea evaluator;
    return evaluator( cluster );
  };

  /** @class ClusterCalo 
   * 
   *  The simple class/function to get the index of calorimeter.
   *  Index of calorimeter for clusers 
   *  is defined as "calo" index of the seed cell
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   02/12/2001
   */
  class  ClusterCalo
    : public std::unary_function<const CaloCluster*,const unsigned int>
  {
  public:
    
    /** the only one essential method 
     *  @exception CaloException for invalid cluster 
     *  @param cluster pointer to CaloCluster object 
     *  @return index of calorimeter area for given cluster 
     */
    const unsigned int operator() ( const CaloCluster* cluster ) const 
    {
      if( 0 == cluster ) 
        { Exception( " CaloCluster* points to NULL! "); }
      // find seed cell
      const_iterator seed = 
        locateDigit( cluster->entries().begin () , 
                     cluster->entries().end   () , 
                     CaloDigitStatus::SeedCell   );
      if ( cluster->entries().end() == seed  ) 
        { Exception( " 'SeedCell' is not found!"); }
      const CaloDigit* digit = seed->digit() ;
      if( 0 == digit ) 
        { Exception( " CaloDigit* points to NULL for seed!"); }
      // get the area
      return digit->cellID().calo();
    };
  protected:    
    /** throw the exception
     *  @see CaloException 
     *  @exception CaloException 
     *  @param     message exception message
     *  @return    status code (fictive)
     */
    StatusCode Exception( const std::string& message ) const 
    {
      throwException( "ClusterCalo() " + message );
      return StatusCode::FAILURE ;
    };
  };
  
  /** @fn clusterCalo
   * 
   *  The simple class/function to get the index of calorimeter.
   *  Index of calorimeter for clusers 
   *  is defined as "calo" index of the seed cell
   *
   *  @see ClusterArea 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   02/12/2001
   */
  inline const unsigned int clusterCalo( const CaloCluster* cluster )
  {
    ClusterCalo evaluator;
    return evaluator( cluster );
  };
  
  /** @class  ClusterFromCalo
   * 
   *  simple predicate/functor to select cluster from given calorimeter 
   *  
   *  @see ClusterCalo 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   02/12/2001
   */
  class ClusterFromCalo
    : public std::unary_function<const CaloCluster*,bool>
  {
  public:
    /** constructor 
     *  @see         CaloCellCode 
     *  @exception   CaloException for invalid calorimeter name
     *  @param calo  name of calorimeter (full or abbreviated)
     */
    ClusterFromCalo( const std::string& calo )
      : m_calo      ( CaloCellCode::CaloNumFromName( calo ) )
      , m_evaluator () 
    { if( 0 > m_calo ) { Exception( "Wrong Calo Name='" + calo + "'"); } };
    /** the only one essential method
     *  @see ClusterCalo
     *  @exception CaloException fro ClusterCalo class 
     *  @param cluster pointer to CaloCluster object 
     *  @return true if cluster belongs to tehselected calorimter 
     */ 
    inline bool operator() ( const CaloCluster* cluster ) const 
    { return (int) m_evaluator( cluster ) == m_calo ; };
    /** set new calorimeter name 
     *  @exception   CaloException for invalid calorimeter name
     *  @param value name of calorimeter (full or abbreviated)
     */
    void setCalo( const std::string& calo ) 
    {
      m_calo = CaloCellCode::CaloNumFromName( calo );
      if( 0 > m_calo ) { Exception( "Wrong Calo Name='" + calo + "'");}
    }
  protected:
    /** throw the exception
     *  @see CaloException 
     *  @exception CaloException 
     *  @param     message exception message
     *  @return    status code (fictive)
     */
    StatusCode Exception( const std::string& message ) const 
    {
      throwException( "ClusterFromCalo() " + message );
      return StatusCode::FAILURE ;
    };
  private:
    /// default constructor is private 
    ClusterFromCalo();    
  private:
    int         m_calo      ;
    ClusterCalo m_evaluator ;
  };
  
  /** @class  ClusterFromArea
   * 
   *  simple predicate/functor to select cluster from given 
   *  area in calorimeter 
   *  
   *  @see ClusterArea 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   02/12/2001
   */
  class ClusterFromArea
    : public std::unary_function<const CaloCluster*,bool>
  {
  public:
    /** constructor 
     *  @param area area index 
     */
    ClusterFromArea( const unsigned int  area )
      : m_area      ( area )
      , m_evaluator () 
    {}
    /** the only one essential method
     *  @see ClusterArea
     *  @exception CaloException from ClusterArea class 
     *  @param cluster pointer to CaloCluster object 
     *  @return true if cluster belongs to the selected area in calorimter 
     */ 
    inline bool operator() ( const CaloCluster* cluster ) const 
    { return m_evaluator( cluster ) == m_area ; };
  private:
    unsigned int m_area      ;
    ClusterArea  m_evaluator ;
  };

  /** @class OnTheBoundary
   *  
   *  Simple utility to locate clusters, placed on the boundaries 
   *  between different calorimeter zones
   *  
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   02/12/2001
   */
  class OnTheBoundary 
    : public std::unary_function<const CaloCluster*,bool>
  {
  public:
    
    /** the only one essential method 
     *  @param cluster pointer to CaloCluster object 
     *  @return true if cluster is on the boundary, 
     *               for empty clusters "false" is returned  
     *  @exception CaloException for invalid clusters
     */
    inline bool operator() ( const CaloCluster* cluster ) const 
    {
      if( 0 == cluster ) { Exception("CaloCluster* points to NULL!"); }
      const CaloCluster::Entries& entries = cluster->entries();
      if( entries.size() <= 1 )  { return false; }   // RETURN !!!
      const_iterator seed = 
        locateDigit( entries.begin ()          , 
                     entries.end   ()          , 
                     CaloDigitStatus::SeedCell );
      if( entries.end() == seed ) { Exception("'SeedCell' is not found!");}
      const CaloDigit* sd = seed->digit() ;
      if( 0 == sd  ) { Exception("CaloDigit* for 'SeedCell' is  NULL!");}
      const unsigned int seedArea = sd->cellID().area() ;
      for( const_iterator entry = entries.begin() ;
           entries.end() != entry ; ++entry ) 
        {
          const CaloDigit* digit = entry->digit() ;
          if( 0 == digit )           { continue ; }
          if( seedArea != digit->cellID().area() ) { return true ; }
        } // end of loop over all cluyster entries  
      //
      return false ;
    };
  protected:
    /** throw the exception
     *  @see CaloException 
     *  @exception CaloException 
     *  @param     message exception message
     *  @return    status code (fictive)
     */
    StatusCode Exception( const std::string& message ) const 
    {
      throwException( "OnTheBoundary() " + message );
      return StatusCode::FAILURE ;
    };    
  };
  
  /** @fn onTheBoundary 
   *
   *  Simple utility to locate clusters, placed on the boundaries 
   *  between different calorimeter zones
   *  
   *  @see OnTheBoundary
   *  @param cluster pointer to CaloCluster object 
   *  @return true if cluster is on the boundary, 
   *               for empty clusters "false" is returned  
   *  @exception CaloException for invalid clusters
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   02/12/2001
   */
  inline const bool onTheBoundary( const CaloCluster* cluster )
  {
    OnTheBoundary evaluator ;
    return evaluator( cluster );
  };
  
  /** @class ZPosition
   *  
   *  The simple function to get the cluster z-posiiton as a z-position of 
   *  "Seed Cell"
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   02/12/2001
   */
  class ZPosition
    : public std::unary_function<const CaloCluster*,double>
  {
  public:    
    /** the explicit constructor
     *  @param detector source of calorimeter detector information
     */
    ZPosition( const DeCalorimeter* detector )
      : m_detector( detector ){};
    /// destructor
    ~ZPosition() { m_detector = 0 ;};
    /** the only one essential method 
     *  @exception CaloException if detector is not valid
     *  @exception CaloException if cluster is NULL
     *  @exception CaloException if cluster is Empty
     *  @exception CaloException if cluster has no SEED cell 
     *  @exception CaloException if SEED digit is NULL 
     *  @param cluster pointer to CaloCluster object 
     *  @return z-position 
     */
    inline double operator() ( const CaloCluster* cluster ) const
    {
      if( 0 == m_detector           ) 
        { return Exception( " DeCalorimeter*     points to NULL! " );}
      if( 0 == cluster              ) 
        { return Exception( " const CaloCluster* points to NULL! " );}
      if( cluster->entries().empty() ) 
        { return Exception( " CaloCluster is empty! " );}
      const_iterator iseed = 
        locateDigit( cluster->entries().begin() ,  
                     cluster->entries().end  () ,  
                     CaloDigitStatus::SeedCell  ) ;
      if( cluster->entries().end() == iseed ) 
        { return Exception( " The Seed Cell is not found! ");}
      ///
      const CaloDigit* seed = iseed->digit();
      if( 0 == seed ) 
        { return Exception( " The Seed Digit points to NULL! ");}
      //
      return m_detector->cellCenter( seed->cellID() ).z() ;
    };
  protected:
    /** throw the exception
     *  @see CaloException 
     *  @exception CaloException 
     *  @param     message exception message
     *  @return    status code (fictive)
     */
    StatusCode Exception( const std::string& message ) const 
    {
      throwException( "ZPosition() " + message );
      return StatusCode::FAILURE ;
    };
    
  private:
    const DeCalorimeter* m_detector; ///< source of geometry information
  };
  
  /** @fn tagTheCluster
   *
   *  Helpful function to tag the sub cluster according to the 
   *  fractionc evaluated by "evaluator"
   * 
   *  Error codes 
   * 
   *          - 225 : CaloCluster* points to NULL
   *          - 226 : Entry with status 'SeedCell' is not found 
   *          - 227 : Entry with status 'SeedCell' is invalid 
   *    
   *
   *  @param cluster pointer to CaloCluster object 
   *  @param matrix  subcluster selector object 
   *  @param modify  flag for modification of energy fractions
   *  @param tag     tag to be set for cells with modified fractions 
   *
   *  @author Vanya Belyaev Ivan Belyaev
   *  @date   01/04/2002
   */
  
  template< class EVALUATOR>
  inline StatusCode tagTheSubCluster
  ( const CaloCluster*             cluster   , 
    const EVALUATOR&               evaluator , 
    const bool                     modify    ,
    const CaloDigitStatus::Status& tag       )
  {
    // check the arguments 
    if( 0 == cluster               ) { return StatusCode( 225 ) ; }
    // get all entries
    CaloCluster::Entries& entries = cluster->entries() ;
    // find seed digit
    CaloCluster::Entries::iterator seedEntry = 
      ClusterFunctors::locateDigit( entries.begin ()          , 
                                  entried.end   ()          , 
                                    CaloDigitStatus::SeedCell );
    // check the seed
    if( entries.end() == seedEntry ) { return StatusCode( 226 ) ; }
    const CaloDigit* seed = seedEntry->digit() ;
    if( 0             == seed      ) { return StatusCode( 227 ) ; }
    // loop over all entried 
    for( CaloCluster::Entries::iterator entry = entries.begin() ; 
         entries.end() != entry ; ++entry )
      {
        // reset existing statuses 
        entry -> removeStatus  ( CaloDigitStatus::UseForEnergy     ) ;
        entry -> removeStatus  ( CaloDigitStatus::UseForPosition   ) ;
        entry -> removeStatus  ( CaloDigitStatus::UseForCovariance ) ;
        // remove the tag
        entry -> removeStatus  ( tag ) ;
        // skip invalid digits 
        const CaloDigit* digit = entry->digit() ;
        if( 0 == digit    )                         { continue ; } // CONTINUE  
        // evaluate the fraction 
        const double fraction = evaluator( seed->cellID() , digit->cellID() );
        if( 0 >= fraction )                         { continue ; } // CONTINUE
        // update statuses
        entry->addStatus ( CaloDigitStatus::UseForEnergy     ) ;
        entry->addStatus ( CaloDigitStatus::UseForPosition   ) ;
        entry->addStatus ( CaloDigitStatus::UseForCovariance ) ;
        if( ! modify      )                         { continue ; } // CONTINUE 
        // modify the fractions 
        entry->setFraction( entry->fraction() * fraction         ) ;
        entry->addStatus  ( tag ) ;
      }
    ///
    return StatusCode::SUCCESS;
  };

  /** @fn untagTheSubCluster
   *
   *  Helpful function to untag the sub cluster according to the 
   *  fractionc evaluated by "evaluator"
   * 
   *  Error codes 
   * 
   *          - 225 : CaloCluster* points to NULL
   *          - 226 : Entry with status 'SeedCell' is not found 
   *          - 227 : Entry with status 'SeedCell' is invalid 
   *    
   *
   *  @param cluster pointer to CaloCluster object 
   *  @param matrix  subcluster selector object 
   *  @param tag     tag to be set for cells with modified fractions 
   *
   *  @see CaloCuster
   *  @see tagTheSubCluster 
   *
   *  @author Vanya Belyaev Ivan Belyaev
   *  @date   01/04/2002
   */
  
  template< class EVALUATOR>
  inline StatusCode untagTheSubCluster
  ( const CaloCluster*             cluster   , 
    const EVALUATOR&               evaluator , 
    const CaloDigitStatus::Status& tag       )
  {
    // check the arguments 
    if( 0 == cluster               ) { return StatusCode( 225 ) ; }
    // get all entries
    CaloCluster::Entries& entries = cluster->entries() ;
    // find seed digit
    CaloCluster::Entries::iterator seedEntry = 
      ClusterFunctors::locateDigit( entries.begin ()          , 
                                  entried.end   ()          , 
                                    CaloDigitStatus::SeedCell );
    // check the seed
    if( entries.end() == seedEntry ) { return StatusCode( 226 ) ; }
    const CaloDigit* seed = seedEntry->digit() ;
    if( 0             == seed      ) { return StatusCode( 227 ) ; }
    // loop over all entried 
    for( CaloCluster::Entries::iterator entry = entries.begin() ; 
         entries.end() != entry ; ++entry )
      {
        // reset existing statuses 
        entry -> addStatus  ( CaloDigitStatus::UseForEnergy     ) ;
        entry -> addStatus  ( CaloDigitStatus::UseForPosition   ) ;
        entry -> addStatus  ( CaloDigitStatus::UseForCovariance ) ;
        // tagged ?
        if( !( tag | entry->status() ) )           { continue ; }  // CONTINUE
        // skip invalid digits 
        const CaloDigit* digit = entry->digit() ;
        if( 0 == digit                 )           { continue ; } // CONTINUE  
        // evaluate the fraction 
        const double fraction = evaluator( seed->cellID() , digit->cellID() );
        if( 0 >= fraction              )           { continue ; } // CONTINUE
        // modify the fractions 
        entry->setFraction( entry->fraction() / fraction         ) ;
        entry->removeStatus  ( tag ) ;
      }
    ///
    return StatusCode::SUCCESS;
  };
  
}; /// end of name space


// ============================================================================
// The End 
// ============================================================================
#endif ///< CALOUTILS_CLUSTERFUNCTORS_H
// ============================================================================
