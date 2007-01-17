// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OnlineHistDB.h,v 1.2 2007-01-17 15:30:02 ggiacomo Exp $
#ifndef ONLINEHISTDB_H
#define ONLINEHISTDB_H 1
/** @class  OnlineHistDB OnlineHistDB.h OnlineHistDB/OnlineHistDB.h
 * 
 *  header for the C++ interface to the Online Monitoring Histogram DB
 *  @author Giacomo Graziani (INFN Firenze)
 *  @date 1/12/2006
 */
#include "OnlineHistDB/OnlineHistDBEnv.h"
#include "OnlineHistDB/OnlineHistogram.h"
#include "OnlineHistDB/OnlineHistPage.h"


class  OnlineHistDB : public OnlineHistDBEnv
{
  public:

  OnlineHistDB (std::string passwd, 
		std::string user="lhcb_mon_giacomo", 
		std::string db="devdb10");
  virtual ~OnlineHistDB();
  // DB session methods
  void commit();
  void setHistogramBufferDepth(int N);
  void setDebug(int dl);

  // declarations of main DB objects
  
  /// create or update a Task definition
  void declareTask(std::string Name, 
		   std::string SubDet1="NULL", 
		   std::string SubDet2="NULL", 
		   std::string SubDet3="NULL",
		   bool RunsOnPhysics=false, 
		   bool RunsOnCalib=false, 
		   bool RunsOnEmpty=false);
  /// declare a  subsystem
  void declareSubSystem(std::string SubSys);
  /// declare an Histogram to the DB by its DIM service name
  void declareHistByServiceName(const std::string &ServiceName);
  /// declare an Histogram to be produced at analysis level
  OnlineHistogram* declareAnalysisHistogram(std::string Algorithm,
				std::string Title,
				std::vector<OnlineHistogram*> &Sources);
  /// declare to the DB an Analysis algorithm implemented in the Analysis library
  void declareCheckAlgorithm(std::string Name, 
			     int Npars, 
			     std::string* pars=NULL, 
			     std::string doc="NONE");
  /// declare to the DB an available algorithm to produce histograms at analysis time
  void declareCreatorAlgorithm(std::string Name, 
			       int Ninput=0, 
			       std::string doc="NONE");

  // access methods
  /// get an OnlineHistPage object, to create a new page or view/edit an existing one
  OnlineHistPage* getPage(std::string Name, std::string Folder="");
  /// get an OnlineHistogram object, holding informations of an existing histogram, that can be used to view/edit an histogram record
  OnlineHistogram* getHistogram(std::string Name,
				std::string Page="_NONE_");
  /// get number of parameters needed by algorithm AlgName
  int getAlgorithmNpar(std::string AlgName) const;
  /// get the name of parameter Ipar of algorithm AlgName
  std::string getAlgParName(std::string AlgName,
		       int Ipar) const;


  // query functions
  /// get the list of histograms on which some check analysis has to be performed 
  int getHistogramsWithAnalysis(std::vector<OnlineHistogram*>& list);
  /// get the list of histograms that have to be produced by analysis task
  int getAnalysisHistograms(std::vector<OnlineHistogram*>& list);
  /// get the list of histograms related to subsystem SubSys
  int getHistogramsBySubsystem(std::string SubSys,std::vector<OnlineHistogram*>& list);
  /// get the list of histograms related to task Task
  int getHistogramsByTask(std::string Task,std::vector<OnlineHistogram*>& list);
  /// get the list of histograms displayed on page Page
  int getHistogramsByPage(std::string Page,std::vector<OnlineHistogram*>& list);
  /// get the list of histograms in a Set
  int getHistogramsBySet(std::string SetName,std::vector<OnlineHistogram*>& list);
  int getHistogramsBySet(const OnlineHistogram& Set,std::vector<OnlineHistogram*>& list);

 private:
  // private dummy copy constructor and assignment operator
  OnlineHistDB(const OnlineHistDB&) : OnlineHistDBEnv() {}
  OnlineHistDB& operator= (const OnlineHistDB&)  {return *this;}
  Statement *m_stmt;
  int m_nit;
  int m_maxNit;
  int m_maxSNsize;
  int m_debug;
  std::vector<OnlineHistPage*> m_myPage;
  std::vector<OnlineHistogram*> m_myHist;
  void HistoStmtExec();
  int getHistograms(std::string query,std::vector<OnlineHistogram*>& list);
};

#endif // ONLINEHISTDB_H
