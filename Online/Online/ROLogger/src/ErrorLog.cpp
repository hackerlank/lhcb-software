// $Id: ErrorLog.cpp,v 1.4 2008-05-07 16:22:21 frankb Exp $
//====================================================================
//  ROLogger
//--------------------------------------------------------------------
//
//  Package    : ROLogger
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/ErrorLog.cpp,v 1.4 2008-05-07 16:22:21 frankb Exp $

// Framework include files
#include <sstream>
#include <cerrno>
#include "RTL/rtl.h"
#include "UPI/upidef.h"
#include "CPP/Event.h"
#include "CPP/IocSensor.h"
#include "ROLogger/Logger.h"
#include "ROLogger/ErrorLog.h"
#include "ROLogger/PartitionDisplay.h"
#include "ROLogger/PartitionListener.h"

#include "ROLoggerDefs.h"
extern "C" {
#include "dic.h"
#include "dis.h"
}

using namespace ROLogger;

/// Standard constructor
ErrorLog::ErrorLog(int argc, char** argv) 
{
  RTL::CLI cli(argc, argv, help_fun);
  std::string name, info;
  cli.getopt("service",1,name);
  if ( name.empty() ) {
    ::upic_write_message("You have to supply a service name to display its data.","");
    ::lib_rtl_sleep(10000);
    ::exit(2);
  }
  m_messageLog = new Logger(RTL::processName()+"_display");
  m_historyLog = new Logger(RTL::processName()+"_history");
  m_partDisplay = new PartitionDisplay(this,m_messageLog,m_historyLog,name);
  m_partListener = new PartitionListener(this,name);
}

/// Standard destructor
ErrorLog::~ErrorLog()  {
  delete m_partListener;
  delete m_partDisplay;
  delete m_messageLog;
  delete m_historyLog;
  ::upic_write_message("Close window.","");
  ::upic_quit();
  ::lib_rtl_sleep(200);
  ::exit(0);
}

void ErrorLog::help_fun() {
}

void ErrorLog::handle(const Event& ev) {
  IocSensor& ioc = IocSensor::instance();
  switch(ev.eventtype) {
  case IocEvent:
    switch(ev.type) {
    case CMD_UPDATE_NODES: // From PartitionListener
      ioc.send(m_partDisplay,CMD_UPDATE_NODES,ev.data);
      return;
    case CMD_UPDATE_FARMS: // From PartitionListener
      ioc.send(m_partDisplay,CMD_UPDATE_FARMS,ev.data);
      return;
    case CMD_DELETE_PART_DISPLAY:
      delete this;
      ::lib_rtl_sleep(200);
      ::exit(0);
      return;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

#include "dis.hxx"
#include "UPI/UpiSensor.h"
extern "C" int romon_display_server(int argc, char** argv) {
  int srows, scols, cols, rows, col, row;
  UpiSensor& s = UpiSensor::instance();
  ErrorLog mon(argc, argv);
  ::dis_start_serving((char*)RTL::processName().c_str());
  ::upic_get_screen_size(&srows, &scols);
  ::upic_get_message_window(&rows, &cols,&row, &col);
  ::upic_set_message_window (srows-15, scols-2, 16, 2);
  s.run();
  return 1;
}
