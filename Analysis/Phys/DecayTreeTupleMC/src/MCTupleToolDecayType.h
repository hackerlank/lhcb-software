// $Id: MCTupleToolDecayType.h,v 1.3 2010-01-26 15:39:25 rlambert Exp $
#ifndef TUPLETOOLMCDECAYTYPE_H
#define TUPLETOOLMCDECAYTYPE_H 1

// Include files
// from Gaudi
#include "DecayTreeTupleBase/TupleToolBase.h"
#include "Kernel/IMCParticleTupleTool.h"            // Interface
#include "Event/GenCollision.h"
#include "Event/GenHeader.h"

//struct Particle2MCLinker;
#include "Kernel/Particle2MCLinker.h"
#include "Kernel/IDaVinciAssociatorsWrapper.h"
#include "MCInterfaces/IMCEventTypeFinder.h"
#include "MCInterfaces/IMCDecayFinder.h"
#include "Kernel/IBackgroundCategory.h"
#include <set>
/** @class MCTupleToolDecayType MCTupleToolDecayType.h jborel/MCTupleToolDecayType.h
 *
 * \brief
 * \sa DecayTreeTuple
 *
 *  @author Robert Lambert
 *  @date   2009-02-25
 *
 *  This TupleTool enables you to output the LHCb EventTypes for the MC particle associated to this candidate
 *  and/or search for a given MCDecay string
 *  The association should be done in TupleToolMCTruth
 *
 *  There are two methods used to find the event type   (Pseudo or Slow)
 *   and several places you can look for the event type (GenHeader, FullEvent, MCAssociate)
 *
 *  CONFIGURATION:

 *  You will also need to configure the EvtTypeSvc(), which takes the list of all possible decays from Gauss

 *  EvtTypeSvc().EvtTypesFile='...fullpath.../table_event.txt'

 *  Usually the file resides in $DECFILESROOT/doc/table_event.txt, copy and ship it with your job

 *  METHODS:

 *

 *  A) The Pseudo method: guesses the event type, by constructing the possible types

 *                        based on the definition of event types in the LHCb note

 *                        this is fast, works on all decays, but is ambiguous.

 *                        To look for inclusive types, this is the best method.

 *

 *  B) The Slow Method  : Finds the event type based on the decay string.

 *                        This is accurate, but relies on the string being correct

 *                        in the .dec file of this decay. Most of the time this is not the case.

 *                        To check the decay file you've written is correct, this is the best method.

 *

 *  WHERE TO LOOK:

 *

 *  1) The GenHeader    : The GenHeader contains the event type which was generated.

 *                        If this is all you need, and you want no information on the other

 *                        possible decays in this event, you can stop here.

 *                        If all you want to do is find the heaviest quark in the event,

 *                        use the TupleToolGeneration.

 *

 *  2) The Full Event   : Every MCParticle in the event is checked to see how it decayed.

 *                        The full list of all possible event types of this event can then be found.

 *

 *  3) The MCAssociate  : The decay of the MCAssociate, or it's mother, or the top of the tree, is checked.

 *                        A categorisation of any fully or partially reconstructed backgrounds

 *                        can then be made.

 *                        This output is complimentary to background category and the MCHierachy and

 *                        can be used as a rudimentary categorisation of backgrounds.

 *

 *  For method (3) use this TupleTool. For methods (1) and (2) use the TupleToolMCEventType

 *

 *

 *  OPTIONS:

 *
 *

 *  mother         bool   Actually categorise the mc mother of this particle, not the associate itself

 *                        by default this is false

 *  top            bool   Actually categorise the ultimate mcmother of this mcparticle, not the associate itself

 *                        by default this is false

 *

 *  UseChi2Method  bool   assosciate using chi2

 *                        by default this is false

 *  InputLocations bool   Associator input location.

 *                        by default this is empty, fine for most cases

 *

 *  fillSlowFind   bool   search through using decay strings, very slow method

 *                        by default this is false

 *  fillPseudoFind bool   construct the event types logically, much faster but less accurate

 *                        by default this is true

 *

 *  allEventTypes std::vector   the full list of all event types to consider. for the slow find.

 *                               This is ignored by the fast find.

 *                               By default this is the whole list in the DekFile doc

 *  hasEventTypes  std::vector   list of event types, the number of these types in the event

 *                               will appear as an entry in the ntuple

 *                               by default this is empty

 *  hasMCDecay     std::string   look for this decay string aswell

 *                               by default this is empty

 *

 *  ENTRIES:

 *

 *  Depending on the options, different entries will be written out.

 *  By default, only the Pseudo event types will be looked for

 *

 *  _MCP_FoundTypes              (farray)            List of all found event types for the assosciate

 *  _MCP_FoundLen                (unsigned int)      Maximum length of this farray

 *  _MCP_numFoundTypes           (unsigned int)      number of types found in this case

 *  _MCP_MatchingTypes           (farray)            List of Event types which match those in hasEventType for the assosciate

 *  _MCP_MatchLen                (unsigned int)      Maximum length of this farray

 *  _MCP_numMatchingTypes        (unsigned int)      How many of hasEventType there are in the Event Types for the assosciate

 *

 *  _MCP_PseudoTypes             (farray)            List of all found event types for the assosciate

 *  _MCP_PseudoLen               (unsigned int)      Maximum length of this farray

 *  _MCP_numPseudoTypes          (unsigned int)      number of pseudo types found in this case

 *  _MCP_MatchingPseudoTypes     (farray)            List of Pseudo Event types which match those in hasEventType for the MCP

 *  _MCP_MatchPseudoLen          (unsigned int)      Maximum length of this farray

 *  _MCP_numMatchingPseudoTypes  (unsigned int)      How many of hasEventType are in the Pseudo Event Types for the assosciate

 *

 *  _MCP_hasGivenDecay           (bool)              does this MCP decay by the string in hasMCDecay?

 *
 */


class MCTupleToolDecayType : public TupleToolBase, virtual public IMCParticleTupleTool
{

public:

  /// Standard constructor
  MCTupleToolDecayType( const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

  virtual ~MCTupleToolDecayType(){}; ///< Destructor

  virtual StatusCode fill( const LHCb::MCParticle*
                           , const LHCb::MCParticle*
                           , const std::string&
                           , Tuples::Tuple& );

  virtual StatusCode initialize();

private:

  //  const GaudiAlgorithm* getParent() const ;

  //Members which are set by the options:

  bool m_mother; //!< set by mother option categorise the mcmother of this mcparticle, not the associate itself
  bool m_fillSlowFind;   //< set by the fillSlowFind   option, search through using decay strings
  bool m_fillPseudoFind; //< set by the fillPseudoFind option, construct the event types logically, much faster but less accurate

  ///m_findEventTypes set by the allEventTypes option, the full list of all event types to consider.
  std::vector<long unsigned int> m_findEventTypes;
  ///m_hasEventType set by the hasEventType option, how many of this list of types appear in this event?
  std::vector<long unsigned int> m_hasEventType;
  std::string m_hasMCDecay;                        //< set by the hasMCDecay option, look for this decay string aswell

  //Other Members

  LHCb::EventTypeSet m_findEventTypeSet; //< loaded from m_findEventTypes
  LHCb::EventTypeSet m_hasEventTypeSet;  //< loaded from m_hasEventType

  IMCEventTypeFinder* m_mcEventType; //< the EventTypeFinder tool
  IMCDecayFinder* m_mcDecay;         //< the DecayTypeFinder tool

  ///convert a std::vector to a std::set
  bool vec2set(std::vector<long unsigned int>& avec,LHCb::EventTypeSet& aset);
  bool set2vec(LHCb::EventTypeSet& aset,std::vector<long unsigned int>& avec);
  //want to cache genEventID/string??

};

#endif // TUPLETOOLMCDECAYTYPE_H
