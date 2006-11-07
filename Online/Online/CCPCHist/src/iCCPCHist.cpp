#include <string.h>
#include <stdlib.h>
#include "dimhist.h"
#include "CCPCHist.h"

#include <math.h>
#include <iterator>
 
#define MIN( x , y)  =   (( (x) < (y) ) ? (x) : (y) ) 
#define MAX( x , y)  =   (( (x) > (y) ) ? (x) : (y) )

static DimHistbuff1 dumbuf1;
static DimHistbuff2 dumbuf2;
static int mpty;

CCPCHSys::CCPCHSys()
{
  serv = new HistServer();
}
CCPCHSys::~CCPCHSys()
{
}
void CCPCHSys::setname (char *n)
{
  char *nodename;
#ifdef WIN32
  nodename  = getenv("COMPUTERNAME");
#else
  nodename  = getenv("HOSTNAME");
#endif
  if (nodename != 0)
  {
    strcpy(name,nodename);
    strcat(name,"_");
    strcat(name,n);
  }
  else
  {
    strcpy(name,n);
  }
  return;
}

CCPCHSys& CCPCHSys::instance()  {
  static CCPCHSys s;
  return s;
}

void CCPCHSys::start()
{
  char nam[1024];
  strcpy(nam,name);
  strcat(nam,"/HistCommand");
  rpc = new HistRPC(this, nam, "I:1;C","F");
  strcpy(nam,name);
  strcat(nam,"/HistData");
  genSrv = new DimService(nam, "F",(void*)&mpty, 4);

  serv->start(name);
}
void CCPCHSys::add(CCPCHisto* h)
{
  hists.push_back(h);
}

CCPCHisto *CCPCHSys::findhisto(char *nam)
{
  std::vector <int>::size_type i;
  int namlen;
  namlen  = strlen(nam);
  for (i =0;i<hists.size();i++)
  {
    CCPCHisto *h = hists[i];
    if (h->nameeq(nam,namlen))
    {
      return h;
    }
  }
  return 0;
}
void CCPCHisto::setup(HTYPE typ, Histo *ext,char *name, char *title, 
                     int nx, float xmin, float xmax, 
                     int ny, float ymin, float ymax )
{
	_type	= typ;
	nentries	= 0;
	this->nx	= 0;
	this->xmin	= 0.0;
	this->xmax	= 0.0;
	this->binsx	= 0.0;
	this->ny	= 0;
	this->ymin	= 0.0;
	this->ymax	= 0.0;
	this->binsy	= 0.0;
	this->titlen = 0;
	this->title = 0;
	this->contsiz		= 0;
	this->contents	= 0;
  this->dimservname  = 0;
  this->Tdimservname  = 0;
  sumw = 0.0;
  sumx = 0.0;
  sumx2 = 0.0;
  sumx3 = 0.0;
  sumx4 = 0.0;
  sumy = 0.0;
  sumy2 = 0.0;
  sumy3 = 0.0;
  sumy4 = 0.0;

  extid = ext;
  if (nx == 0) return;
  //switch (ny)
  //{
  //case 0:
  //  {
  //  _type = H_1DIM;
  //  break;
  //  }
  //default: 
  //  {
  //    _type = H_2DIM;
  //  }
  //}
	setname(name);
	Init(title,nx,xmin,xmax, ny, ymin, ymax);
	makedimname(name,&dimservname);
  serv = new HistService (this, dimservname,"F", &dumbuf1,sizeof(dumbuf1));
  int l = strlen(dimservname)+strlen("/gauchocomment")+1;
  Tdimservname  = (char*)malloc(l);
  strcpy(Tdimservname,dimservname);
  strcat(Tdimservname,"/gauchocomment");
  Tdimservname[l] = 0;
  Tserv = new DimService(Tdimservname,this->title);
  return;
}
CCPCHisto::CCPCHisto()
{
  setup(H_ILLEGAL, 0,"","",0,0.0,0.0,0,0.0,0.0);
}
CCPCHisto::CCPCHisto(Histo *ext)
{
  setup(H_ILLEGAL, ext,"","",0,0.0,0.0,0,0.0,0.0);
}
CCPCHisto::CCPCHisto(char *name, char *title, int nx, float xmin, float xmax )
{
  setup(H_1DIM, 0,name, title, nx, xmin, xmax,0, 0.0,0.0); 
}
CCPCHisto::CCPCHisto(Histo *ext, char *name, char *title, int nx, float xmin, float xmax )
{
  setup(H_1DIM, ext, name, title, nx, xmin, xmax,0, 0.0,0.0); 
}
CCPCHisto::CCPCHisto(char *name, char *title, int nx, float xmin, float xmax, 
					int ny, float ymin, float ymax )
{
  setup (H_2DIM, 0, name, title, nx, xmin, xmax, ny, ymin, ymax );
}
CCPCHisto::CCPCHisto(Histo *ext, char *name, char *title, 
                     int nx, float xmin, float xmax, 
                     int ny, float ymin, float ymax )
{
  setup (H_2DIM, ext, name, title, nx, xmin, xmax, ny, ymin, ymax );
}
int CCPCHisto::Init(char *title, int nx, float xmin, float xmax )
{
  Init(title, nx, xmin, xmax, 0, 0.0, 0.0);
	return 0;
}
int CCPCHisto::Init(char *title, int nx, float xmin, float xmax, 
					int ny, float ymin, float ymax )
{
  CCPCHSys::instance().add(this);
	nentries	= 0;
	this->nx	= nx;
	this->xmin	= xmin;
	this->xmax	= xmax;
  binsx = 0.0;
  if (nx>0)
  {
	  binsx	= (xmax-xmin)/nx;
  }
	this->ny	= ny;
	this->ymin	= ymin;
	this->ymax	= ymax;
  binsy = 0.0;
  if (ny >0)
  {
	  binsy	= (ymax-ymin)/ny;
  }
	titlen = strlen(title);
	this->title = (char*)malloc(titlen+1);
	strcpy(this->title,title);
	this->title[titlen]	= 0;
  contents  = 0;
  if (_type == H_2DIM)
  {
	  contsiz		= (ny+2)*(nx+2)*sizeof(bintype);
	  contents	= (bintype*)malloc(contsiz);
  }
  else if (_type == H_1DIM)
  {
	  contsiz		= (nx+2)*sizeof(bintype);
	  contents	= (bintype*)malloc(contsiz);
  }
  else if (_type == H_PROFILE)
  {
	  contsiz		= (nx+2)*sizeof(bindesc);
	  contents	= (bintype*)malloc(contsiz);
  }
  if (contents != 0)
  {
	  memset(contents,0,contsiz);
  }
  dimservname   = 0;
	return 0;
}
CCPCHisto::~CCPCHisto()
{
	if (titlen != 0)
	{
		free(title);
		titlen	= 0;
	}
	if (contsiz != 0)
	{
		free(contents);
		contsiz	= 0;
	}
  if (dimservname != 0)
  {
    free (dimservname);
  }
  if (Tdimservname != 0)
  {
    free (Tdimservname);
  }
}
int CCPCHisto::setname ( char* name)
{
	memset(this->name,0,sizeof(this->name));
	strncpy(this->name,name,sizeof(this->name)-1);
  namelen = strlen(this->name);
	return 0;
}

void CCPCHisto::clear(void)
{
  nentries  = 0;
  sumw = 0.0;
  sumx = 0.0;
  sumx2 = 0.0;
  sumx3 = 0.0;
  sumx4 = 0.0;
  sumy = 0.0;
  sumy2 = 0.0;
  sumy3 = 0.0;
  sumy4 = 0.0;

	if (contents != 0)
	{
		memset(contents,0,contsiz);
	}
	return;
}
int CCPCHisto::put (float *from )
{
	memcpy(contents,from,contsiz);
	return 0;
}
int CCPCHisto::put (double *from )
{
	memcpy(contents,from,contsiz);
	return 0;
}
int CCPCHisto::putnents (int ne )
{
  nentries  = ne;
	return 0;
}
void CCPCHisto::CopyData (float *to, float *c )
{
	memcpy(to,c,contsiz);
}
void CCPCHisto::CopyData (double *to, double *c )
{
	memcpy(to,c,contsiz);
}
void CCPCHisto::CopyData (float *to, double *c )
{
  unsigned int i;
	for (i=0;i<contsiz>>3;i++)
  {
    to[i] = (float)c[i];
  }
}
void CCPCHisto::CopyData (double *to, float *c )
{
  unsigned int i;
	for (i=0;i<contsiz>>2;i++)
  {
    to[i] = c[i];
  }
}
int CCPCHisto::get (float *to )
{
  CopyData(to, contents);
	return 0;
}
int CCPCHisto::get (double *to )
{
  CopyData(to, contents);
	return 0;
}
int CCPCHisto::getnents ( )
{
	return nentries;
}
int CCPCHisto::geterr (float *to )
{
  int i;
  for (i=0; i<nx+2; i++)
  {
    to[i]= (float)sqrt(contents[i]);
  }
  return 0;
}
int CCPCHisto::geterr (double *to )
{
  int i;
  for (i=0; i<nx+2; i++)
  {
    to[i]=sqrt(contents[i]);
  }
  return 0;
}
int CCPCHisto::info (char *title, int *nx, float *xmin, float *xmax, float *bins)
{
	strcpy(title,this->title);
	*nx	= this->nx;
	*xmin	= this->xmin;
	*xmax	= this->xmax;
	*bins	= this->binsx;
	return 0;
}
int CCPCHisto::info (char *title, int * nx, float *xmin,float *xmax, float *binsx,
						int * ny, float *ymin,float *ymax, float *binsy)
{
	strcpy(title,this->title);
	*nx	= this->nx;
	*xmin	= this->xmin;
	*xmax	= this->xmax;
	*binsx	= this->binsx;
	*ny	= this->ny;
	*ymin	= this->ymin;
	*ymax	= this->ymax;
	*binsy	= this->binsy;
	return 0;
}

int CCPCHisto::modify (char *title, int nx, float xmin, float xmax)
{
	if (titlen != 0)
	{
		free(title);
		titlen	= 0;
	}
	if (contsiz != 0)
	{
		free(contents);
		contsiz	= 0;
	}
	Init(title,nx,xmin,xmax);
	return 0;
}
int CCPCHisto::modify (char *title, int nx, float xmin, float xmax,
							int ny, float ymin, float ymax)
{
	if (titlen != 0)
	{
		free(title);
		titlen	= 0;
	}
	if (contsiz != 0)
	{
		free(contents);
		contsiz	= 0;
	}
	Init(title,nx,xmin,xmax,ny,ymin,ymax);
	return 0;
}
int CCPCHisto::fill(float x)
{
	fill (x,1.0);
	return 0;
}
int CCPCHisto::fill (float x, bintype weight)
{
	int xbinnr;
	int binnr;
	if (_type == H_1DIM)
	{
		if (x < xmin)
		{
			xbinnr	= 0;
		}
		else if (x>xmax)
		{
			xbinnr	= nx+1;
		}
		else
		{
			xbinnr	= (int)((x-xmin)/binsx + 1);
		}
		binnr	= xbinnr;
		contents[xbinnr] += weight;
		nentries++;
    {
      sumw  += weight;
      double xn = weight*x;
      sumx += xn;
      xn *= x;
      sumx2 += xn;
      xn  *= x;
      sumx3 += xn;
      xn  *= x;
      sumx4 += xn;
    }
		return 0;
	}
	if (_type == H_2DIM)
	{
		fill(x,(float)weight,1.0);
		return 0;
	}
	else
	{
		return 1;
	}
}
int CCPCHisto::fill (float x, float y, bintype weight)
{
	int xbinnr;
	int ybinnr;
	int binnr;
	if (_type == H_2DIM)
	{
		if (x < xmin)
		{
			xbinnr	= 0;
		}
		else if (x>xmax)
		{
			xbinnr	= nx+1;
		}
		else
		{
			xbinnr	= (int)((x-xmin)/binsx + 1);
		}
		if (y < ymin)
		{
			ybinnr	= 0;
		}
		else if (y>ymax)
		{
			ybinnr	= ny+1;
		}
		else
		{
			ybinnr	= (int)((y-ymin)/binsy + 1);
		}
		binnr	= xbinnr*(ny+2)+ybinnr;
		contents[binnr] += weight;
		nentries++;
    sumw  += weight;
    {
      double xn = weight*x;
      sumx += xn;
      xn *= x;
      sumx2 += xn;
      xn  *= x;
      sumx3 += xn;
      xn  *= x;
      sumx4 += xn;
    }
    {
      double yn = weight*y;
      sumy += yn;
      yn *= y;
      sumy2 += yn;
      yn  *= x;
      sumy3 += yn;
      yn  *= x;
      sumy4 += yn;
    }
	}
	return 0;
}
void *CCPCHisto::getextid (void)
{
  return extid;
}

bool CCPCHisto::nameeq(char *nam, int namlen)
{
  bool r;
  r = (namlen == namelen);
  r = r && (strcmp(name,nam) == 0);
  return r;
}

//int CCPCHisto::SetupPublishing(char *servicename)
//{
//	return 0;
//}
void CCPCHisto::makedimname(char *name, char **outp)
{
  int olen=0;
  char *out;
  olen += strlen ("H1D/");
  olen += strlen (CCPCHSys::instance().name);
  olen += strlen ("/CCPCAlg/");
  olen += strlen (name);
  out   = (char *)malloc(olen);
  *outp = out;
	out[0]=0;
	if (_type == H_1DIM)
	{
		strcpy(out,"H1D/");
	}
	else if (_type == H_2DIM)
	{
		strcpy(out,"H2D/");
	}
	else if (_type == H_PROFILE)
	{
		strcpy(out,"HPD/");
	}
  strcat(out,CCPCHSys::instance().name);
	strcat(out,"/");
	strcat(out,"CCPCAlg/");
  strcat(out,name);
}

CCPCPHisto::CCPCPHisto(char *name, char *title, int nx, float xmin, float xmax )
{
  setup(H_PROFILE, 0, name, title, nx, xmin,xmax,0, 0.0, 0.0);

}
CCPCPHisto::CCPCPHisto(PHisto *ext, char *name, char *title, int nx, float xmin, float xmax )
{
  setup(H_PROFILE, (Histo*)ext, name, title, nx, xmin,xmax,0, 0.0, 0.0);
}
CCPCPHisto::~CCPCPHisto()
{
}
int CCPCPHisto::fill(float x, float y)
{
	bindesc *pcont = (bindesc*)contents;
	int xbinnr;
	int binnr;
	if (_type == H_PROFILE)
	{
		if (x < xmin)
		{
			xbinnr	= 0;
		}
		else if (x>xmax)
		{
			xbinnr	= nx+1;
		}
		else
		{
			xbinnr	= (int)((x-xmin)/binsx + 1);
		}
		binnr	= xbinnr;
		pcont[xbinnr].netries++;
		pcont[xbinnr].sum += y;
		pcont[xbinnr].sum2 += (y*y);
		nentries++;
		return 0;
	}
	else
	{
		return 1;
	}
}
int CCPCPHisto::getsums (float *to) 
  {
	  bindesc *pcont = (bindesc*)contents;
    int i;
    for (i=0; i<nx+2; i++)
    {
      to[i] = (float)pcont[i].sum;
    }
    return 0;
  }
int CCPCPHisto::getsum2s(float *to )
  {
	  bindesc *pcont = (bindesc*)contents;
    int i;
    for (i=0; i<nx+2; i++)
    {
      to[i] = (float)pcont[i].sum2;
    }
    return 0;
  }
int CCPCPHisto::getsums (double *to) 
  {
	  bindesc *pcont = (bindesc*)contents;
    int i;
    for (i=0; i<nx+2; i++)
    {
      to[i] = pcont[i].sum;
    }
    return 0;
  }
int CCPCPHisto::getsum2s(double *to )
  {
	  bindesc *pcont = (bindesc*)contents;
    int i;
    for (i=0; i<nx+2; i++)
    {
      to[i] = pcont[i].sum2;
    }
    return 0;
  }
int CCPCPHisto::getentries(float *to)
  {
	  bindesc *pcont = (bindesc*)contents;
    int i;
    for (i=0; i<nx+2; i++)
    {
      to[i] = (float)pcont[i].netries;
    }
    return 0;
  }
HSys::HSys()
  {
    sys = &CCPCHSys::instance();
  }

  Histo *HSys::findhisto(char *name)
  {
    CCPCHisto *h;
    h = sys->findhisto(name);
    if (h != 0)
    {
      return (Histo*)h->getextid();
    }
    else
    {
      return 0;
    }
  }
void *hccpc_init(char *nam)
  {
    static int inited=0;
    static HSys *hsys;
    if (inited == 0)
    {
      hsys  = new HSys();
      CCPCHSys &hsi=CCPCHSys::instance();
      hsi.setname(nam);
      hsi.serv->autoStartOn();
      hsi.start();
      inited =1;
    }
    return hsys;
  }

  Histo::Histo(char *name, char *title, int nx, float xmin, float xmax )
  {
    h = new CCPCHisto(this, name, title, nx,xmin,xmax);
  }
//Constructor for 2-dim histogram
	Histo::Histo(char *name, char *title, int nx, float xmin, float xmax, 
					   int ny, float ymin, float ymax )
  {
    h = new CCPCHisto(this, name, title, nx, xmin, xmax,ny,ymin,ymax);
  }
//Constructor generic histogram
	Histo::Histo()
{
	h = new CCPCHisto(this);
}
	Histo::~Histo()
  {
    delete h;
  }
	//int Histo::setname ( char* name)
  //{
  //  return h->setname(name);
  //}
	void Histo::clear(void)
  {
    h->clear();
  }
	int Histo::put (float *from)
  {
    return h->put(from);
  }
	int Histo::put (double *from)
  {
    return h->put(from);
  }
  int Histo::putnents (int ne) 
  {
    return h->putnents(ne);
  }
	int Histo::get (float *to)
  {
    return h->get(to);
  }
	int Histo::geterr (float *to)
  {
    return h->geterr(to);
  }
	int Histo::get (double *to)
  {
    return h->get(to);
  }
	int Histo::geterr (double *to)
  {
    return h->geterr(to);
  }
	int Histo::getnents()
  {
    return h->getnents();
  }

	int Histo::fill (float x)
  {
    return h->fill(x);
  }
	int Histo::fill (float x, bintype weight)
  {
    return h->fill(x, weight);
  }
	int Histo::fill (float x,float y, bintype weight)
  {
    return h->fill(x, y, weight);
  }
 	PHisto::PHisto(char *name, char *title, int nx, float xmin, float xmax )
    {
      h = new CCPCPHisto(this, name, title, nx, xmin, xmax);
    }
	PHisto::~PHisto()
  {
    delete h;
  }
	int PHisto::fill(float x, float y)
  {
    return h->fill(x,y);
  }
	int PHisto::getsums (float *to) 
  {
    return h->getsums(to);
  }
  int PHisto::getsum2s(float *to )
  {
    return h->getsum2s(to);
  }
	int PHisto::getsums (double *to) 
  {
    return h->getsums(to);
  }
  int PHisto::getsum2s(double *to )
  {
    return h->getsum2s(to);
  }
  int PHisto::getentries(float *to)
  {
    return h->getentries(to);
  }

  void PHisto::clear(void)
  {
    return h->clear();
  }

  void *hccpc_book1(char *name, char *title, int nx, float xmin, float xmax )
  {
    CCPCHisto *h;
    h = new CCPCHisto(name,title,nx,xmin,xmax);
    return h;
  }
  void *hccpc_profile(char *name, char *title, int nx, float xmin, float xmax )
  {
    CCPCPHisto *h;
    h = new CCPCPHisto(name,title,nx,xmin,xmax);
    return h;
  }
  void *hccpc_book2(char *name, char *title, int nx, float xmin, float xmax, 
					   int ny, float ymin, float ymax )
  {
    CCPCHisto *h;
    h = new CCPCHisto(name,title,nx,xmin,xmax, ny, ymin,ymax);
    return h;
  }
	int hfill1 (void *id, float x, bintype weight)
  {
    CCPCHisto *h = (CCPCHisto *)id;
    if (h->type() == H_1DIM)
    {
      return h->fill(x, weight);
    }
    return H_IllegalID;
  }
	int hfill2 (void *id, float x,float y, bintype weight)
  {
    CCPCHisto *h = (CCPCHisto *)id;
    if (h->type() == H_2DIM)
    {
      return h->fill(x, y, weight);
    }
    return H_IllegalID;
  }
  int hfillp (void *id, float x, float y)
  {
    CCPCPHisto *h = (CCPCPHisto *)id;
    if (h->type() == H_PROFILE)
    {
      return h->fill(x, y);
    }
    return H_IllegalID;
  }

