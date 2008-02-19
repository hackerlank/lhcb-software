// $Id: SubfarmDisplay.cpp,v 1.2 2008-02-19 10:24:21 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/SubfarmDisplay.cpp,v 1.2 2008-02-19 10:24:21 frankb Exp $

// C++ include files
#include <cstdlib>
#include <iostream>

// Framework include files
#include "RTL/Lock.h"
#define MBM_IMPLEMENTATION
#include "ROMon/ROMon.h"
#include "ROMon/SubfarmDisplay.h"

using namespace ROMon;
static const char *sstat[17] = {" nl", "   ", "*SL","*EV","*SP","WSL","WEV","WSP","wsl","wev","wsp"," ps"," ac", "SPR", "WER", "   "};
namespace {
  struct TaskIO {
    int in, out;
    size_t st_in, st_out;
    TaskIO() : in(0), out(0), st_in(0), st_out(0) {}
    TaskIO(const TaskIO& m) : in(m.in), out(m.out), st_in(m.st_in), st_out(m.st_out) {}
    TaskIO& operator=(const TaskIO& m) { in=m.in; out=m.out; st_in=m.st_in; st_out=m.st_out; return *this;}
  };
}
typedef Nodeset::Nodes               Nodes;
typedef Node::Buffers                Buffers;
typedef MBMBuffer::Clients           Clients;
typedef Node::Tasks                  Tasks;
#define BUILDER_TASK 'M'
#define SENDER_TASK  'D'
#define MOORE_TASK   'G'
#define MEP_BUFFER   'M'
#define RES_BUFFER   'R'
#define EVT_BUFFER   'E'
#define SND_BUFFER   'S'

static void getBuffInfo(const MBMBuffer::Control& c, int info[3], int tot[3]) {
  tot[0] += (info[0] = c.tot_produced);
  tot[1] += (info[1] = c.p_emax-c.i_events);
  tot[2] += (info[2] = (c.i_space*c.bytes_p_Bit)/1024);
  return;
}

static void help() {
  std::cout <<"  romon_storage -option [-option]" << std::endl
	    <<"       -h[eaderheight]=<number>     Height of the header        display.                      " << std::endl
	    <<"       -n[odesheight]=<number>      Height of the Nodes         display.                      " << std::endl
	    <<"       -m[ooresheight]=<number>     Height of the MOORE tasks   display.                      " << std::endl
	    <<"       -d[elay]=<number>            Time delay in millisecond between 2 updates.              " << std::endl
	    <<"       -s[ervicename]=<name>        Name of the DIM service  providing monitoring information." << std::endl
	    << std::endl;

}

/// Standard constructor
SubfarmDisplay::SubfarmDisplay(int argc, char** argv)   {
  RTL::CLI cli(argc,argv,help);
  int hdr_height, nodes_height, moores_height;
  cli.getopt("headerheight",  1, hdr_height    =   5);
  cli.getopt("nodesheight",   1, nodes_height  =  15);
  cli.getopt("mooresheight",  1, moores_height =  18);
  cli.getopt("delay",         1, m_delay       = 1000);
  cli.getopt("servicename",   1, m_svcName     = "RONodePublish_hlte07");

  setup_window();
  int width    = m_area.width;
  int eb_width = width/3;
  m_nodes    = createSubDisplay(Position(0,hdr_height),                   Area(width,nodes_height),"Node Information");
  m_moores   = createSubDisplay(Position(0,m_nodes->bottom()-1),          Area(width,moores_height),"Moore Processes");
  m_builders = createSubDisplay(Position(0,m_moores->bottom()-1),         Area(eb_width,m_area.height-m_moores->bottom()+1),"Event builders");
  m_holders  = createSubDisplay(Position(eb_width,m_moores->bottom()-1),  Area(eb_width,m_area.height-m_moores->bottom()+1),"Event Producers");
  m_senders  = createSubDisplay(Position(2*eb_width,m_moores->bottom()-1),Area(width-2*eb_width,m_area.height-m_moores->bottom()+1),"Event Senders");
  end_update();
}

/// Standard destructor
SubfarmDisplay::~SubfarmDisplay()  {
}

/// Display the node information
void SubfarmDisplay::showNodes(const Nodeset& ns)  {
  MonitorDisplay* disp = m_nodes;
  char* p;
  const char* fmt = " %-12s%5d %12d%6d%6d %12d%11d%6d%6d %12d%6d%6d %12d";
  int mep_tot[3] = {0,0,0}, evt_tot[3] = {0,0,0}, res_tot[3] = {0,0,0}, accept_tot=0, cons_tot=0, ntsk_tot=0;

  disp->draw_line_reverse("                     --------- MEP --------   ------------- EVENT -------------   ------- RESULT -------");
  disp->draw_line_bold(   " Node        Tasks     Produced Slots Space     Produced   Consumed Slots Space     Produced Slots Space     Accepted");

  for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    int mep_info[3] = {0,0,0}, evt_info[3] = {0,0,0}, res_info[3] = {0,0,0}, out=0, consumed=0, ntsk=0;
    const Buffers& buffs = *(*n).buffers();
    for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      char b = (*ib).name[0];
      const Clients& clients = (*ib).clients;
      switch(b) {
      case RES_BUFFER:
      case SND_BUFFER: getBuffInfo((*ib).ctrl, res_info, res_tot); break;
      case MEP_BUFFER: getBuffInfo((*ib).ctrl, mep_info, mep_tot); break;
      case EVT_BUFFER: getBuffInfo((*ib).ctrl, evt_info, evt_tot); break;
      default:                                                                  break;
      }
      for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
	++ntsk;
	p = strchr((*ic).name,'_');
	if ( p ) {
	  switch(*(++p)) {
	  case SENDER_TASK:
	    if(b==RES_BUFFER||b==SND_BUFFER)  {
	      out   += (*ic).events;
	      accept_tot += (*ic).events;
	    }
	    break;
	  case MOORE_TASK:
	    if(b==EVT_BUFFER)  {
	      consumed += (*ic).events;
	      cons_tot += (*ic).events;
	    }
	    break;
	  default:
	    break;
	  }
	}
      }
    }
    disp->draw_line_normal(fmt, (*n).name, ntsk, mep_info[0], mep_info[1], mep_info[2],
			   evt_info[0], consumed, evt_info[1], evt_info[2],
			   res_info[0], res_info[1], res_info[2], out);
  }
  disp->draw_line_normal("");
  disp->draw_line_bold(fmt, "Total:", ntsk_tot, mep_tot[0], mep_tot[1], mep_tot[2],
		       evt_tot[0], cons_tot, evt_tot[1], evt_tot[2],
		       res_tot[0], res_tot[1], res_tot[2], accept_tot);
}

/// Show the event builder information
void SubfarmDisplay::showTasks(const Nodeset& ns) {
  std::map<std::string,TaskIO> moores;
  std::string nam;
  char txt[1024];
  int nTsk = 0;
  int eb_width = m_area.width/3;

  m_builders->draw_line_reverse (" EVENTBUILDER     Produced [%] MEP       ");
  //m_holders->draw_line_reverse(" HOLDER            Free                  ");
  m_holders->draw_line_reverse  (" PRODUCER         Seen   Produced MEP EVT");
  m_senders->draw_line_reverse  (" SENDER            Sent Pend RES         ");

  ::memset(txt,' ',sizeof(txt));
  for(int i=0; i<3;++i ) {
    ::sprintf(txt+eb_width*i," MOORE              Seen  Accept EVT RES   ");
    txt[strlen(txt)] = ' ';
  }
  txt[3*eb_width+3] = 0;
  m_moores->draw_line_reverse(txt);

  for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    const Buffers& buffs = *(*n).buffers();
    std::string prod_name;
    TaskIO prod;
    for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      const Buffers::value_type::Control& c = (*ib).ctrl;
      const Clients& clients = (*ib).clients;
      char b = (*ib).name[0];
      for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
	Clients::const_reference cl = (*ic);
	if(strncmp(cl.name,"MEPRx",5)==0 && b == MEP_BUFFER ) {
	  std::string nn = (*n).name;
	  nn += '_';
	  nn += cl.name+5;
	  float perc = c.tot_produced>0 ? 100*float(cl.events)/float(c.tot_produced) : 0;
	  m_builders->draw_line_normal(" %-12s %12d %3.0f %3s",nn.c_str(),cl.events,perc,sstat[size_t(cl.state)]);
	  continue;
	}
	char* p = strchr(cl.name,'_');
	if ( p ) {
	  nam = std::string(cl.name).substr(0,p-cl.name);
	  nam += '_';
	  char* q = strchr(p+1,'_');
	  if ( q ) nam += q+1;
	  ++p;
	  switch(*p) {
	  case SENDER_TASK:
	    if ( b == RES_BUFFER || b== SND_BUFFER ) {
	      m_senders->draw_line_normal( " %-12s %9d%5d %3s        ",nam.c_str(),cl.events,c.i_events,sstat[size_t(cl.state)]);
	    }
	    break;
	  case 'E':
	    if ( b == MEP_BUFFER && strncmp(p,"EvtHolder",9)==0) {
	      // m_holders->draw_line_normal( " %-12s %9d        ",nam.c_str(),cl.events);
	    }
	    else if ( b == MEP_BUFFER && strncmp(p,"EvtProd",7)==0) {
	      prod_name = nam;
	      prod.in += cl.events;
	      prod.st_in = cl.state;
	    }
	    else if ( b == EVT_BUFFER && strncmp(p,"EvtProd",7)==0) {
	      prod_name = nam;
	      prod.out += cl.events;
	      prod.st_out = cl.state;
	    }
	    break;
	  case MOORE_TASK:
	    if ( b == EVT_BUFFER ) {
	      moores[nam].in += cl.events;
	      moores[nam].st_in = cl.state;
	    }
	    else if ( b == RES_BUFFER || b == SND_BUFFER ) {
	      moores[nam].out += cl.events;
	      moores[nam].st_out = cl.state;
	    }
	    break;
	  default:
	    break;
	  }
	}
      }
    }
    if ( !prod_name.empty() ) {
      m_holders->draw_line_normal(" %-12s%9d%11d %3s %3s  ",prod_name.c_str(),prod.in,prod.out,
				  sstat[prod.st_in],sstat[prod.st_out]);
    }
  }
  ::memset(txt,' ',sizeof(txt));
  txt[m_area.width] = 0;
  for(std::map<std::string,TaskIO>::const_iterator i=moores.begin(); i!=moores.end();++i) {
    const TaskIO& m = (*i).second;
    ::sprintf(txt+eb_width*nTsk," %-12s%11d%8d %3s %3s   ",(*i).first.c_str(),m.in,m.out,sstat[m.st_in],sstat[m.st_out]);
    if ( ++nTsk == 3 ) {
      txt[3*eb_width+3] = 0;
      m_moores->draw_line_normal(txt);
      ::memset(txt,' ',sizeof(txt));
      txt[sizeof(txt)-1] = 0;
      nTsk = 0;
    }
    else {
      txt[strlen(txt)] = ' ';
    }
  }
  txt[m_area.width] = 0;
  if ( nTsk>0 ) m_moores->draw_line_normal(txt);
}

/// Update header information
void SubfarmDisplay::showHeader(const Nodeset& ns)   {
  char b1[64], b2[64];
  Nodeset::TimeStamp frst=ns.firstUpdate(), last=ns.lastUpdate();
  ::strftime(b1,sizeof(b1),"%H:%M:%S",::localtime(&frst.first));
  ::strftime(b2,sizeof(b1),"%H:%M:%S",::localtime(&last.first));
  draw_line_normal ("");
  draw_line_reverse("         HLT monitoring on %s   [%s]", ns.name, ::lib_rtl_timestr());    
  draw_line_bold   ("         Information updates date between: %s.%03d and %s.%03d",b1,frst.second,b2,last.second);
}

/// Update all displays
void SubfarmDisplay::updateDisplay(const Nodeset& ns)   {
  begin_update();
  m_nodes->begin_update();
  m_moores->begin_update();
  m_builders->begin_update();
  m_holders->begin_update();
  m_senders->begin_update();

  showHeader(ns);
  showNodes(ns);
  showTasks(ns);

  m_nodes->end_update();
  m_moores->end_update();
  m_builders->end_update();
  m_holders->end_update();
  m_senders->end_update();
  end_update();
}

extern "C" int romon_subfarm(int argc,char** argv) {
  SubfarmDisplay disp(argc,argv);
  disp.initialize();
  disp.run();
  return 1;
}
