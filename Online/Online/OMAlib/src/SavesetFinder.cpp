// $Id: SavesetFinder.cpp,v 1.9 2009-06-09 17:34:10 ggiacomo Exp $

#include "OMAlib/SavesetFinder.h"
#include "OMAlib/AnalysisTask.h"

SavesetFinder::SavesetFinder(AnalysisTask* Ana, 
                             std::string &TaskName,
                             std::string &PartitionName) :
  DimInfo(std::string(PartitionName + "/" + TaskName + OMAconstants::SavesetServicePath).c_str(), 
          (char*) "SAVESETLOCATION" ),
  m_analysis(Ana), m_taskname(TaskName)
{
  m_lastSaveset = "SAVESETLOCATION";
}

void SavesetFinder::infoHandler() {
  std::string saveset( getString() );
  if( saveset.find("SAVESETLOCATION") == std::string::npos &&
      saveset.find("Zombie") == std::string::npos) {
    if (saveset.size() >0  && saveset != m_lastSaveset) {
      m_analysis->openDBSession();
      m_analysis->info() << "calling analyze for task "<<m_taskname <<
        " on saveset "<<saveset << endreq;
      m_analysis->resetMessages(m_taskname);
      m_analysis->analyze( saveset, m_taskname );
      m_analysis->refreshMessageList(m_taskname);
      m_lastSaveset = saveset;
      m_analysis->closeDBSession();
    }
  }
}

SavesetFinder::~SavesetFinder() {
} 


