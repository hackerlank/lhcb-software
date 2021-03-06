// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OHDB_ocitypes.h,v 1.6 2010-06-08 17:18:05 ggiacomo Exp $
#ifndef OHDB_OCITYPES_H
#define OHDB_OCITYPES_H 1
struct dispopt_s
{
  OCIString * LABEL_X;
  OCIString * LABEL_Y;
  OCIString * LABEL_Z;
  OCINumber YMIN;
  OCINumber YMAX;
  OCINumber STATS;
  OCINumber FILLSTYLE;
  OCINumber FILLCOLOR;
  OCINumber LINESTYLE;
  OCINumber LINECOLOR;
  OCINumber LINEWIDTH;
  OCIString * DRAWOPTS;
  OCINumber XMIN;
  OCINumber XMAX;
  OCINumber ZMIN;
  OCINumber ZMAX;
  OCINumber LOGX;
  OCINumber LOGY;
  OCINumber LOGZ;
  OCIString * TIMAGE;
  OCIString * REF;
  OCINumber REFRESH;
  OCINumber TIT_X_SIZE;
  OCINumber TIT_X_OFFS;
  OCINumber TIT_Y_SIZE;
  OCINumber TIT_Y_OFFS;
  OCINumber TIT_Z_SIZE;
  OCINumber TIT_Z_OFFS;
  OCINumber LAB_X_SIZE;
  OCINumber LAB_X_OFFS;
  OCINumber LAB_Y_SIZE;
  OCINumber LAB_Y_OFFS;
  OCINumber LAB_Z_SIZE;
  OCINumber LAB_Z_OFFS;
  OCINumber GRIDX;
  OCINumber GRIDY;
  OCINumber THETA;
  OCINumber PHI;
  OCIString * CNTPLOT;
  OCIString * DRAWPATTERN;
  OCINumber STAT_X_SIZE;
  OCINumber STAT_X_OFFS;
  OCINumber STAT_Y_SIZE;
  OCINumber STAT_Y_OFFS;
  OCINumber HTIT_X_SIZE;
  OCINumber HTIT_X_OFFS;
  OCINumber HTIT_Y_SIZE;
  OCINumber HTIT_Y_OFFS;
  OCINumber NDIVX;
  OCINumber NDIVY;
  OCINumber MARKERSIZE;
  OCINumber MARKERCOLOR;
  OCINumber MARKERSTYLE;
  OCINumber NORM;
  OCINumber TICK_X;
  OCINumber TICK_Y;
  OCINumber MARGIN_TOP;
  OCINumber MARGIN_BOTTOM;
  OCINumber MARGIN_LEFT;
  OCINumber MARGIN_RIGHT;
  OCINumber PADCOLOR;
  OCINumber STATTRANSP;
  OCIString * REFDRAWOPTS;
  OCINumber SPAREI1;
  OCINumber SPAREI2;
  OCINumber SPAREF1;
  OCINumber SPAREF2;
  OCIString * SPARES;
  OCINumber NOTITLE;
  OCIString * SHOWTITLE;
};
typedef struct dispopt_s dispopt;

struct dispopt_ind
{
  OCIInd _atomic;
  OCIInd LABEL_X;
  OCIInd LABEL_Y;
  OCIInd LABEL_Z;
  OCIInd YMIN;
  OCIInd YMAX;
  OCIInd STATS;
  OCIInd FILLSTYLE;
  OCIInd FILLCOLOR;
  OCIInd LINESTYLE;
  OCIInd LINECOLOR;
  OCIInd LINEWIDTH;
  OCIInd DRAWOPTS;
  OCIInd XMIN;
  OCIInd XMAX;
  OCIInd ZMIN;
  OCIInd ZMAX;
  OCIInd LOGX;
  OCIInd LOGY;
  OCIInd LOGZ;
  OCIInd TIMAGE;
  OCIInd REF;
  OCIInd REFRESH;
  OCIInd TIT_X_SIZE;
  OCIInd TIT_X_OFFS;
  OCIInd TIT_Y_SIZE;
  OCIInd TIT_Y_OFFS;
  OCIInd TIT_Z_SIZE;
  OCIInd TIT_Z_OFFS;
  OCIInd LAB_X_SIZE;
  OCIInd LAB_X_OFFS;
  OCIInd LAB_Y_SIZE;
  OCIInd LAB_Y_OFFS;
  OCIInd LAB_Z_SIZE;
  OCIInd LAB_Z_OFFS;
  OCIInd GRIDX;
  OCIInd GRIDY;
  OCIInd THETA;
  OCIInd PHI;
  OCIInd CNTPLOT;
  OCIInd DRAWPATTERN;
  OCIInd STAT_X_SIZE;
  OCIInd STAT_X_OFFS;
  OCIInd STAT_Y_SIZE;
  OCIInd STAT_Y_OFFS;
  OCIInd HTIT_X_SIZE;
  OCIInd HTIT_X_OFFS;
  OCIInd HTIT_Y_SIZE;
  OCIInd HTIT_Y_OFFS;
  OCIInd NDIVX;
  OCIInd NDIVY;
  OCIInd MARKERSIZE;
  OCIInd MARKERCOLOR;
  OCIInd MARKERSTYLE;
  OCIInd NORM;
  OCIInd TICK_X;
  OCIInd TICK_Y;
  OCIInd MARGIN_TOP;
  OCIInd MARGIN_BOTTOM;
  OCIInd MARGIN_LEFT;
  OCIInd MARGIN_RIGHT;
  OCIInd PADCOLOR;
  OCIInd STATTRANSP;
  OCIInd REFDRAWOPTS;
  OCIInd SPAREI1;
  OCIInd SPAREI2;
  OCIInd SPAREF1;
  OCIInd SPAREF2;
  OCIInd SPARES;
  OCIInd NOTITLE;
  OCIInd SHOWTITLE;
};
typedef struct dispopt_ind dispoptInd;

#endif // OHDB_OCITYPES_H
