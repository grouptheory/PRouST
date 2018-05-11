// -*- C++ -*-
// +++++++++++++++
//   P R o u S T     ---  PNNI Routing Simulation Toolkit  ---
// +++++++++++++++
// Version: 1.0 (Fri Mar  5 14:35:33 EST 1999)
// 
// 			  Copyright (c) 1998
// 		 Naval Research Laboratory (NRL/CCS)
// 			       and the
// 	  Defense Advanced Research Projects Agency (DARPA)
// 
// 			 All Rights Reserved.
// 
// Permission to use, copy, and modify this software and its
// documentation is hereby granted, provided that both the copyright notice and
// this permission notice appear in all copies of the software, derivative
// works or modified versions, and any portions thereof, and that both notices
// appear in supporting documentation.
// 
// NRL AND DARPA ALLOW FREE USE OF THIS SOFTWARE IN ITS "AS IS" CONDITION AND
// DISCLAIM ANY LIABILITY OF ANY KIND FOR ANY DAMAGES WHATSOEVER RESULTING FROM
// THE USE OF THIS SOFTWARE.
// 
// NRL and DARPA request users of this software to return modifications,
// improvements or extensions that they make to:
// 
//                 proust-dev@cmf.nrl.navy.mil
//                         -or-
//                Naval Research Laboratory, Code 5590
//                Center for Computation Science
//                Washington, D.C.  20375
// 
// and grant NRL and DARPA the rights to redistribute these changes in
// future upgrades.
//

// -*- C++ -*-
#ifndef __CANVAS3D_H__
#define __CANVAS3D_H__
#ifndef LINT
static char const _canvas3d_h_rcsid_[] =
"$Id: canvas3D.h,v 1.14 1999/02/24 21:55:22 mountcas Exp $";
#endif

#include <qgl.h>
#include <DS/containers/dictionary.h>
#include "TexFont.h"

class WidgetView;
class ov_data;
class OVNode;
class Event;
class NodeID;

#define PI         3.14159265358979323846
#define deg2rad(x) ((double)x * PI / 180.0)
#define rad2deg(x) ((double)x * 180.0 / PI)
#define ABS(x)     ((x) < 0 ? -(x) : (x))

#define NODE_RADIUS 0.5

//---------------------------------------------------------
class canvas3D : public QGLWidget {
  friend class ov_main;
  friend class WidgetView;
public:

  canvas3D( WidgetView * parent, 
	    const char * name, 
	    ov_data * data );

  void drawAxes( bool b );
  bool axes( void ) const;
  void drawLeaderEdges( bool b );
  bool edges( void ) const;
  void drawPerspective( bool p );
  void setFont( const char * filename );

  void moveCameraTo(double phi, double theta);

  void noteEvent(Event * e);

protected:

  void		initializeGL( );
  void		paintGL( );
  void		resizeGL( int w, int h );
  
  void keyPressEvent(     QKeyEvent   * ke );
  void mousePressEvent(   QMouseEvent * me );
  void mouseMoveEvent(    QMouseEvent * me );
  void mouseReleaseEvent( QMouseEvent * me );
  
private:

  void initializeLightingOptions(void);
  void initializeSmoothingOptions(void);

  void initializeCLUT(void);
  void insertColor(const char* name, double r, double g, double b);
  const GLfloat* lookupColor(char* name) const;

  void drawNodes(GLenum mode = GL_RENDER);
  void drawEdges(void);
  void calculatePos( OVNode * n );
  const GLfloat * calculateMaterial( OVNode * n ) const;

  void recomputeGeometry(void);

  void drawSphere(double x, double y, double z, char* color);
  void drawAxes(void);

  void minifySelect(int value);
  void alphaSelect(int value);
  void alphaModeSet(void);

  WidgetView * _parent;
  ov_data    * _data;

  int       _xPressed, _yPressed;
  int       _ctr;
  OVNode  * _moved_node;
  int       _alphaMode;

  bool      _drawAxes;
  bool      _drawLeaderEdges;

  double    _eyeX, _eyeY, _eyeZ;
  double    _upX, _upY, _upZ;

  double    _phi, _theta;
  double    _radius;

  const NodeID  * _activeNode;
  char      _eventnode[80];
  char      _eventcolor[50];

  TexFont * _txf;

  dictionary<char *, GLfloat *> _clut;

  bool _frustum;

  // CONSTANTS
  static const double    _minPos      = -2.0;
  static const double    _maxPos      =  2.0;
  static const double    _zoomFactor  =  1.05;
  static const double    _capsize     =  deg2rad(0.1);
  static const double    _MAXRANGE    =  50.0;

  static const int       _UPDATE_FREQ =  10;
  static const double    _SPEED       =  0.02;
};

#endif // __CANVAS3D_H__

