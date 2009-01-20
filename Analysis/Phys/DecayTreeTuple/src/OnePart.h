// $Id: OnePart.h,v 1.2 2009-01-20 10:00:44 pkoppenb Exp $
#ifndef ONEPART_H 
#define ONEPART_H 1

// Include files

/** @class OnePart OnePart.h
 *  
 *  Helper class for DecayTreeTupleBase
 *
 *  Stores info about one particle and its tools
 *
 *  @author Jérémie Borel 
 *  @date   2007-11-01
 *  @author Patrick Koppenburg - extracted from DecayTreeTuple
 *  @date   2009-01-09
 */
namespace LHCb{
  class Particle; // @todo remove
}
class DecayTreeTupleBase ;//@todo replace with Base
class Tuple ; // @todo remove
class IParticleTupleTool ;
namespace Decays{
  
  class OnePart {
  public:
    /// constructor
    OnePart( DecayTreeTupleBase* parent
             , const LHCb::Particle& me
             , const std::string& head );
    /// destructor
    ~OnePart();
    /// Call successively all the tool's fill method @todo Needs to be extracted to get rid of dependency on Particle
    bool fill( Tuples::Tuple&, const LHCb::Particle* mother
               , const LHCb::Particle* ); 
    /// Refers to the tuple column name prefix
    std::string headName();
    /// Refers to the tuple column name prefix
    void headName( const std::string& h );
    /// Refers to the particle's real syntax (not escaped)
    std::string getRealName();
    /// Refers to the particle's real syntax (not escaped)
    const std::string& getRealName() const;
    /// Prints the tree strucutre, if verbose, also prints the tool list 
    void printStructure( std::ostream& os, bool verbose=false ) const ;
    /// depth
    int depth() const;
    /// set mother
    void setMother( const OnePart* );
    /// parent
    DecayTreeTupleBase* parent();
    /// mother
    const OnePart* getMother() const;
    /// add daughter
    void addDaughter( OnePart* );
    /// info string
    std::string info() const;
    /// add a Tuple tool
    void addTool( IParticleTupleTool* tool );
    /// list of tuple tools
    std::vector< IParticleTupleTool* >& tools();
    /// list of tools
    std::vector<std::string> toolList() const;
    /// clear list of tools
    void clearTools();
    
  private:
    std::string m_head, m_realname; //< real name of particle
    OnePart();                      //< constructor
    OnePart( const OnePart& );      //< copy constructor
    DecayTreeTupleBase* m_parent;       //< parent
    const OnePart* m_mother;        //< mother
    std::vector<const OnePart*> m_daughters; //< daughters
    std::vector< IParticleTupleTool* > m_tools; //< tools
  };

  // ===============================================================  
  // ======================= inline & template body ================    
  // ===============================================================
  /** join a container with a separation char. */
  template <class ForwardIterator> std::string join( ForwardIterator first,
                                                     ForwardIterator last,
                                                     const char* sep = ", ",
                                                     const char* ifempty = "none")
  {
    std::stringstream  ret; 
    ForwardIterator it(first), it2( first );
    if( first == last ) return std::string(ifempty);
    ++it2;
    if( it2 == last ){
      ret << *first;
      return ret.str();
    }
    it2=first;
    while( it2!=last ){
      ret << *it << sep;
      ++it;
      it2=it;
      ++it2;
    }
    ret << *it;
    return ret.str();
    
  };
  
}
#endif // ONEPART_H
