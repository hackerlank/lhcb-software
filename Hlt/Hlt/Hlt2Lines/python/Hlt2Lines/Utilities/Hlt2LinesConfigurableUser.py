from copy import deepcopy
from itertools import izip
from Utilities import allCuts, makeStages
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

from HltTracking.Hlt2TrackingConfigurations import (
    Hlt2BiKalmanFittedDownstreamTracking, Hlt2BiKalmanFittedForwardTracking)
from HltTracking.Hlt2ProbeTrackingConfigurations import (
    Hlt2MuonTTTracking, Hlt2VeloMuonTracking, Hlt2FullDownstreamTracking)
from Configurables import HltJetConf


class Hlt2LinesConfigurableUser(HltLinesConfigurableUser):
    # python configurables to be applied before me
    __queried_configurables__ = [
        Hlt2MuonTTTracking,
        Hlt2VeloMuonTracking,
        Hlt2FullDownstreamTracking,
        HltJetConf,
    ]

    __slots__ = {
        '_stages': {},
        '_relatedInfo': {},
        'Prescale': {},
        'Postscale': {},
    }

    def __stages(self, source):
        # External stage sets the configurable to something that is not self.
        if source._configurable():
            conf = source._configurable()
        else:
            conf = self

        cuts = allCuts(conf)
        deps = []
        stage = source.stage(deps, cuts)
        if stage in deps:
            print 'WARNING: Circular dependency %s %s %s.' % (source._name(), source._deps(),
                                                              source._inputs())
        stages = filter(lambda i: bool(i), deps + [stage])
        return stages

    def __relatedInfoSequence(self, stages, algorithms):
        if not stages:
            return ([], stages)

        members = []
        locations = []
        for stage in stages:
            # External stage sets the configurable to something that is not self.
            if stage._configurable():
                conf = stage._configurable()
            else:
                conf = self
            cuts = allCuts(conf)
            infoStage, locs = stage.infoStage(cuts, algorithms[-1])
            members.append(infoStage)
            locations.extend(locs)

        # Consistency check
        if len(locations) != len(set(locations)):
            raise Exception('At least one location in %s appears twice, this will not work!' % locations)

        from HltLine.HltLine import Hlt2SubSequence
        return (locations, [Hlt2SubSequence("RelatedInfoSequence", members,
                                            ModeOR=True, ShortCircuit=False)])

    def algorithms(self, stages):
        from HltLine.HltLine import bindMembers

        def __flatten(l):
            for i in l:
                if isinstance(i, bindMembers):
                    for j in __flatten(i.members()):
                        yield j
                else:
                    yield i

        from Hlt2Lines.Utilities.Utilities import uniqueEverseen
        from Hlt2Stage import Hlt2Stage

        sd = {}
        for name, stg in stages.iteritems():
            sl = []
            for i in stg:
                if isinstance(i, Hlt2Stage):
                    sl.extend(self.__stages(i))
                else:
                    sl.append(i)
            sd[name] = list(uniqueEverseen(__flatten(sl)))

        if hasattr(self, "relatedInfo"):
            ri = self.relatedInfo()
            return [(nn, algs, self.__relatedInfoSequence(ri.get(nn, []), algs)) for nn, algs in sd.iteritems()]
        else:
            return sd.iteritems()
