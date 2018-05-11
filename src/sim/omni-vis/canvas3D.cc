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
#ifndef LINT
static char const _canvas3D_cc_rcsid_[] =
"$Id: canvas3D.cc,v 1.37 1999/02/09 15:51:22 mountcas Exp $";
#endif

#include "canvas3D.h"
#include "ov_data.h"
#include "ov_main.h"
#include "WidgetView.h"
#include "OVNode.h"
#include "Event.h"
#include "levelRegistry.h"
#include "eventTypeRegistry.h"

#include <ostream.h>
#include <common/cprototypes.h>
#include <math.h>

#include <qkeycode.h>
#include <GL/glut.h>
#include <DS/random/distributions.h>
#include <codec/pnni_ig/id.h>

#define HEIGHT_DECAY  0.9
#define KEY_MOVE_DIST 10

int counter = 0;

//----------------------------------------
// Methods of class 'canvas3D'
// ---------------------------------------

canvas3D::canvas3D( WidgetView * parent, const char * name, 
		    ov_data * data )
  : QGLWidget( parent, name ), _frustum( true ),
    _data( data ), _parent( parent ),
    _xPressed( -1 ), _yPressed( -1 ), _ctr(0),
    _drawAxes( false ), _drawLeaderEdges( false ),
    _txf( 0 ), _activeNode( 0 )
{
  initializeCLUT();
  setBackgroundColor( black );

  _radius = 2 * ABS(_maxPos - _minPos);
  strcpy(_eventnode,"");
  strcpy(_eventcolor,"");

  // Northeasterly view
  moveCameraTo( deg2rad(45),  deg2rad(45) );
}

//----------------------------------------------------------
void canvas3D::paintGL( )
{
  // ---- SUPER DEBUG MODE ----
  //  cout << "canvas3D::paintGL" << endl;
  //  char buf[64];
  //  sprintf(buf, "gcore -o core-%d %d", counter++, getpid());
  //  system( buf );
  // ---- SUPER DEBUG MODE ----

  glClear( GL_COLOR_BUFFER_BIT | 
	   GL_DEPTH_BUFFER_BIT );

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( ); // clear the matrix

  gluLookAt( _eyeX, _eyeY, _eyeZ,
	     0, 0, 0, 
	     _upX, _upY, _upZ);

  if ( _drawAxes )
    drawAxes();

  // Obtain the list of nodes from _data
  list_item li;
  const list< OVNode * > * nlist = _data->getNodes();

  forall_items(li, *nlist) {
    OVNode * n = nlist->inf(li);

    if ( ! n->isInitialized() )
      calculatePos(n);
  }

  drawNodes( );

  // strcpy(_eventnode,"");
  // strcpy(_eventcolor,"");
  glMaterialfv( GL_FRONT, 
		GL_AMBIENT_AND_DIFFUSE,
		lookupColor("white") );

  drawEdges( );

  assert( doubleBuffer() );
  swapBuffers( );
  //  glFlush();
}

//----------------------------------------------------------
void canvas3D::drawNodes( GLenum mode )
{
  list_item li;
  const list< OVNode * > * nlist = _data->getNodes();

  bool written = false;
  forall_items(li, *nlist) {
    OVNode * n = nlist->inf(li);
    int level = n->getLevel();
    if ( ! theLevelRegistry()->isSelected( level ))
      continue;

    if (mode == GL_SELECT)  // The name of the node is its rank
      glLoadName( nlist->rank( n ) );

    glPushMatrix();
    {
      glTranslatef( n->getX(), n->getY(), n->getZ() );
      glMaterialfv( GL_FRONT, 
		    GL_AMBIENT_AND_DIFFUSE,
		    calculateMaterial( n ) );
      glutSolidSphere( NODE_RADIUS,   // radius
		       30,            // slices
		       30 );          // stacks

      // draw the text under the sphere
      glPushMatrix( );
      {
	NodeID tmp( n->getName() );
	char str[80]; str[0] = '\0';
	abbreviatedPrint(str, &tmp, true);
	str[strlen(str) - 1] = '\0'; // get rid of final period

	int w, a, d;
	txfGetStringMetrics( _txf, str, strlen(str), &w, &a, &d );
	float offset = -(w * 0.5);
	float deltax = _eyeX - n->getX();
	float deltaz = _eyeZ - n->getZ();
	float dist2eye = sqrt(deltax * deltax + deltaz * deltaz);
	float offset_x = deltaz * offset / (1.0 * dist2eye);
	float offset_y = -(a + NODE_RADIUS * 200);
	float offset_z = 0; // - deltax * offset / (2.0 * dist2eye);

	glEnable( GL_TEXTURE_2D );

	glScalef( 1.0 / (w / 2), 1.0 / (w / 2), 1.0 / (w / 2));
	glTranslatef( offset_x, offset_y, offset_z );

	const GLfloat * colorval = lookupColor("yellow");
	assert(colorval != 0);
	glMaterialfv( GL_FRONT,
		      GL_AMBIENT_AND_DIFFUSE,
		      colorval );
	txfRenderString( _txf, str, strlen(str) );

	glDisable( GL_TEXTURE_2D );
      }
      glPopMatrix( );
    }
    glPopMatrix();
  }
}

//----------------------------------------------------------
void canvas3D::drawEdges(void)
{
  list_item li;
  const list< OVNode * > * nlist = _data->getNodes();

  // iterate over all the nodes, drawing the edges
  forall_items(li, *nlist) {
    OVNode * n = nlist->inf(li);
    int level = n->getLevel();

    if ( ! theLevelRegistry()->isSelected( level ))
      continue;

    if ( ! _drawLeaderEdges ) {
      const list< OVNode * > * neighs = n->getEdges();
      
      if (neighs && neighs->empty() == false) {
	// iterate over all the neighbors
	list_item eli;
	forall_items(eli, *neighs) {
	  OVNode * e = neighs->inf(eli);
	  int destlevel = e->getLevel();

	  if (! theLevelRegistry()->isSelected( destlevel ))
	    continue;
	  
	  // Draw an edge between the two nodes
	  glBegin(GL_LINES);   
	  {
	    glColor3f( 0.0, 0.0, 0.0 );
	    glVertex3f( n->getX(), n->getY(), n->getZ() );
	    glVertex3f( e->getX(), e->getY(), e->getZ() );
	  }
	  glEnd();
	}
      }
    } else {
      // draw edges to leaders
      const OVNode * l = n->getLeader();
      int destlevel    = l != 0 ? l->getLevel() : -1;

      if ( !l || !theLevelRegistry()->isSelected( destlevel ))
	continue;
	  
      // Draw an edge between the two nodes
      glBegin(GL_LINES);   
      {
	glColor3f( 0.0, 0.0, 0.0 );
	glVertex3f( n->getX(), n->getY(), n->getZ() );
	glVertex3f( l->getX(), l->getY(), l->getZ() );
      }
      glEnd();
    }
  }
}

//----------------------------------------------------------
void canvas3D::initializeGL()
{
  // background is black with alpha = 0 (transparent)
  // glClearColor( 1.0, 1.0, 1.0, 0.0 ); // white for printing
  glClearColor( 0.0, 0.0, 0.0, 0.0 ); // black for viewing

  initializeLightingOptions();
  initializeSmoothingOptions();

  alphaSelect( GL_BLEND );      // Alpha blending
  minifySelect( GL_NEAREST );   // The textured fonts can use MIP maps

  setFont( "./FONTS/default.txf" );
}

//----------------------------------------------------------
void canvas3D::initializeLightingOptions(void)
{
  // lighting specifications
  GLfloat light_ambient[]  = { 0.1, 0.1, 0.1, 1.0 };
  GLfloat light_diffuse[]  = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat light_position[] = { 0.0, 10.0, 0.0, 0.0 };
  glLightfv( GL_LIGHT0, GL_AMBIENT,  light_ambient );
  glLightfv( GL_LIGHT0, GL_DIFFUSE,  light_diffuse );
  glLightfv( GL_LIGHT0, GL_SPECULAR, light_specular );
  glLightfv( GL_LIGHT0, GL_POSITION, light_position );

  // lighting options
  glEnable( GL_LIGHTING );      // Enable lighting
  glEnable( GL_LIGHT0 );        // Enable the first light
}

//----------------------------------------------------------
void canvas3D::initializeSmoothingOptions(void) 
{
  glEnable( GL_LINE_SMOOTH );   // Smooth those edges
  glHint( GL_LINE_SMOOTH_HINT,  // quickly
	  GL_FASTEST );
  glShadeModel( GL_SMOOTH );    // Smooth shading
  glEnable( GL_DEPTH_TEST );    // depth pruning before rendering
  glDepthFunc( GL_LESS );       // depth function is "less"
}

//----------------------------------------------------------
void canvas3D::drawPerspective( bool p )
{
  _frustum = p;
  resizeEvent( new QResizeEvent( size(), size() ) );
  repaint( false );
}

//----------------------------------------------------------
void canvas3D::resizeGL( int w, int h )
{
  glViewport( 0, 0, (GLint)w, (GLint)h );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();

  if ( _frustum ) {
    glFrustum( -(double)w/800.0,   // left
	        (double)w/800.0,   // right
	       -(double)h/800.0,   // bottom
	        (double)h/800.0,   // top 
	        0.25,              // near clipping plane
	        100.0 );           // far clipping plane
  } else {
    glOrtho( -(double)w/50.0,   // left
	      (double)w/50.0,   // right
	     -(double)h/50.0,   // bottom
	      (double)h/50.0,   // top 
	      0.25,             // near clipping plane
	      100.0 );          // far clipping plane
  }
}

//----------------------------------------------------------
void canvas3D::drawAxes(void)
{
  double hashsize = 1.0;
  double hashinterval = 5.0;

  // x axis

  const GLfloat* colorval = lookupColor("white");
  assert(colorval);
  glTranslatef( 0,0,0 ); 
  glMaterialfv( GL_FRONT,
		GL_AMBIENT_AND_DIFFUSE,
		colorval );
  glShadeModel( GL_FLAT );    // begin flat shading

  glBegin(GL_LINES);
  {
    glVertex3f( -_MAXRANGE, 0, 0 );
    glVertex3f(  _MAXRANGE, 0, 0 );
  }
  glEnd();
  for (double d = -_MAXRANGE; d <= _MAXRANGE ; d+=hashinterval) {
    glBegin(GL_LINES);
    {
      glVertex3f( d, 0, -hashsize );
      glVertex3f( d, 0,  hashsize );
    }
    glEnd();
  }

  // y axis

  glBegin(GL_LINES);
  {
    glVertex3f( 0, 0 , 0 );
    glVertex3f( 0,  _MAXRANGE, 0 );
  }
  glEnd();
  for (double d = 0; d <= _MAXRANGE ; d+=hashinterval) {
    glBegin(GL_LINES);
    {
      glVertex3f( -hashsize, d, 0 );
      glVertex3f(  hashsize, d, 0 );
    }
    glEnd();
  }
  for (double d = 0; d <= _MAXRANGE ; d+=hashinterval) {
    glBegin(GL_LINES);
    {
      glVertex3f( 0, d, -hashsize );
      glVertex3f( 0, d,  hashsize );
    }
    glEnd();
  }

  // z axis

  glBegin(GL_LINES);   
  {
    glVertex3f( 0, 0, -_MAXRANGE );
    glVertex3f( 0, 0,  _MAXRANGE );
  }
  glEnd();
  for (double d = -_MAXRANGE; d <= _MAXRANGE ; d+=hashinterval) {
    glBegin(GL_LINES);
    {
      glVertex3f( -hashsize, 0, d );
      glVertex3f(  hashsize, 0, d );
    }
    glEnd();
  }

  glShadeModel( GL_SMOOTH );    // return to smooth shading
}

//----------------------------------------------------------
void canvas3D::setFont( const char * filename )
{
  TexFont * tmp = 0;
  if (! (tmp = txfLoadFont( (char *)filename )))
    cerr << "Error failed to load font '" << filename << "'." << endl;
  else {
    _txf = tmp;
    txfEstablishTexture( _txf, 1, GL_TRUE );
  }
}

//----------------------------------------------------------
void canvas3D::drawAxes( bool b )
{
  _drawAxes = b;
}

//----------------------------------------------------------
bool canvas3D::axes(void) const
{
  return _drawAxes;
}

//----------------------------------------------------------
void canvas3D::drawLeaderEdges( bool b )
{
  _drawLeaderEdges = b;
}

//----------------------------------------------------------
bool canvas3D::edges(void) const
{
  return _drawLeaderEdges;
}

//----------------------------------------------------------
void canvas3D::moveCameraTo(double phi, double theta) {

  _phi   = phi;
  _theta = theta;

  while ( _theta >= 2 * PI )
    _theta -= 2 * PI;

  while ( _theta < 0 )
    _theta += 2 * PI;

  if ( _phi > ( PI/2 - _capsize ) )
    _phi = PI / 2 - _capsize;
  if ( _phi < _capsize )
    _phi = _capsize;

  recomputeGeometry();
  repaint( false );
}

//----------------------------------------------------------
void canvas3D::drawSphere(double x, double y, double z, char* color) {
  const GLfloat* colorval = lookupColor(color);
  assert(colorval);

  glTranslatef( x, y, z ); 

  glMaterialfv( GL_FRONT,
		GL_AMBIENT_AND_DIFFUSE,
		colorval );

  glutSolidSphere( 1.0,  
		   30, 
		   30 );

  glTranslatef( -x, -y, -z ); 
}

//----------------------------------------------------------
void canvas3D::recomputeGeometry(void) {
  double rx     = _radius * cos(_phi);
  double speedx = rx / (100.0 * _radius);

  double xpos = rx * cos(_theta);
  double zpos = rx * sin(_theta);
  double ypos = _radius * sin(_phi);

  double x0 = -xpos;
  double z0 = -zpos;

  double yder = -rx / ypos;
  double y0   = ypos - rx * yder;

  double L = sqrt( x0 * x0 + y0 * y0 + z0 * z0 );
  double xup = x0 / L;
  double yup = y0 / L;
  double zup = z0 / L;

  _eyeX = xpos;  _eyeY = ypos;  _eyeZ = zpos;
  _upX  = xup;   _upY  = yup;   _upZ  = zup;
}

//----------------------------------------------------------
void canvas3D::keyPressEvent( QKeyEvent * ke )
{
  //cout << "canvas3D::keyPressEvent" << endl;
  bool zoom = false;
  bool move = false;

  double dx = 0, dy = 0;

  switch (ke->key()) {
    case Key_Less:
    case Key_Comma:
      // Zoom Out
      _radius = _radius * _zoomFactor;
      zoom = true;
      break;
    case Key_Greater:
    case Key_Period:
      // Zoom In
      _radius = _radius / _zoomFactor;
      zoom = true;
      break;
    case Key_Left:
      dx -= KEY_MOVE_DIST;
      move = true;
      break;
    case Key_Up:
      dy += KEY_MOVE_DIST;
      move = true;
      break;
    case Key_Right:
      dx += KEY_MOVE_DIST;
      move = true;
      break;
    case Key_Down:
      dy -= KEY_MOVE_DIST;
      move = true;
      break;
  }
 
  if ( zoom ) {
    recomputeGeometry();
    repaint( false );
  } else if ( move ) {
    double rx     = _radius * cos(_phi);
    double speedx = _SPEED * rx / _radius;
    double dtheta = speedx * dx/rx;
    double dphi = _SPEED * dy / _radius;
    moveCameraTo(_phi+dphi,_theta+dtheta);
  }
}

//----------------------------------------------------------
void canvas3D::mousePressEvent( QMouseEvent * me )
{
  // cout << "canvas3D::mousePressEvent" << endl;
  int x = me->pos().x(), y = me->pos().y();

  switch (me->button() ) {
    case LeftButton:
      {
	_xPressed = x;
	_yPressed = y;
      }
      break;
  }
}

//----------------------------------------------------------
void canvas3D::mouseMoveEvent( QMouseEvent * me )
{
  // cout << "canvas3D::mouseMoveEvent" << endl;
  double x = me->pos().x();
  double y = me->pos().y();
  if ( _xPressed != -1 &&
       _yPressed != -1 ) {
    // Check the difference between the current x, y 
    // and the previous x,y and rotate view accordingly
    double dx = (double)_xPressed - (double)x;
    double dy = (double)_yPressed - (double)y;
    double rx     = _radius * cos(_phi);
    double speedx = _SPEED * rx / _radius;
    double dtheta = speedx * dx/rx;
    double dphi = _SPEED * dy / _radius;
    moveCameraTo(_phi+dphi,_theta+dtheta);
  }
  _ctr++;
  if (_ctr%_UPDATE_FREQ == 0) {
    _xPressed = (int)x;
    _yPressed = (int)y;
    _ctr = 1;
  }
}

//----------------------------------------------------------
void canvas3D::mouseReleaseEvent( QMouseEvent * me )
{
  // cout << "canvas3D::mouseReleaseEvent" << endl;
  _moved_node = 0;
  _xPressed = _yPressed = -1;
}

//----------------------------------------------------------
void canvas3D::minifySelect(int value)
{
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, value );
}

//----------------------------------------------------------
void canvas3D::alphaSelect(int value)
{
  assert( value == GL_ALPHA_TEST            ||
	  value == GL_BLEND                 ||
	  value == GL_ALPHA_TEST + GL_BLEND ||
	  value == GL_NONE );
  _alphaMode = value;
}

//----------------------------------------------------------
void canvas3D::alphaModeSet(void)
{
  switch (_alphaMode) {
    case GL_ALPHA_TEST:
      glDisable(GL_BLEND);
      glEnable(GL_ALPHA_TEST);
      glAlphaFunc(GL_GEQUAL, 0.5);
      break;
    case GL_BLEND:
      glDisable(GL_ALPHA_TEST);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      break;
    case GL_ALPHA_TEST + GL_BLEND:
      glEnable(GL_ALPHA_TEST);
      glAlphaFunc(GL_GEQUAL, 0.0625);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      break;
    case GL_NONE:
      glDisable(GL_ALPHA_TEST);
      glDisable(GL_BLEND);
      break;
  }
}

//----------------------------------------------------------
const GLfloat * canvas3D::calculateMaterial( OVNode * n ) const
{
  bool IsSamePG( const NodeID * nid, const char * name );

  if ( !strcmp( "all", _eventnode ) ) 
    return lookupColor( (char *)_eventcolor );
  else if ( !strcmp( "local", _eventnode ) &&
	    _activeNode != 0 &&
	    IsSamePG( _activeNode, n->getName() ) )
    return lookupColor((char *)_eventcolor);
  else if ( !strcmp(n->getName(), _eventnode) )
    return lookupColor((char*)_eventcolor);
  else
    return lookupColor("white");
}

//----------------------------------------------------------
void canvas3D::calculatePos( OVNode * n )
{
  GLfloat x = uniform_distribution::sample( -5.0, 5.0 );
  GLfloat z = uniform_distribution::sample( -5.0, 5.0 );

  int level = n->getLevel();

  double expy = 4.0 * ((1.0 - pow( HEIGHT_DECAY, (96.0-(double)level)/8.0))
		       / (1.0 - HEIGHT_DECAY));
   
  GLfloat y = expy;

  n->setX(x);
  n->setY(y);
  n->setZ(z);

  n->setJiggleX(x);
  n->setJiggleY(z);

  n->setInitialized(true);
}

//----------------------------------------------------------
void canvas3D::initializeCLUT(void) 
{
  insertColor( "red",          1.00, 0.00, 0.00 );
  insertColor( "green",        0.00, 1.00, 0.00 );
  insertColor( "blue",         0.00, 0.00, 1.00 );

  insertColor( "cyan",         0.00, 1.00, 1.00 );
  insertColor( "magenta",      1.00, 0.00, 1.00 );
  insertColor( "yellow",       1.00, 1.00, 0.00 );

  insertColor( "pink",         1.00, 0.54, 0.63 );  
  insertColor( "lightgreen",   0.63, 0.85, 0.59 );  
  insertColor( "lightblue",    0.59, 0.71, 0.85 ); 
  insertColor( "lightyellow",  1.00, 0.93, 0.59 );  
  insertColor( "turquoise",    0.39, 0.85, 0.57 );  

  insertColor( "darkred",      0.44, 0.02, 0.02 );
  insertColor( "darkgreen",    0.06, 0.43, 0.05 );
  insertColor( "darkblue",     0.06, 0.12, 0.43 );
  insertColor( "orange",       1.00, 0.50, 0.00 );
  insertColor( "purple",       0.61, 0.21, 0.75 );  

  insertColor( "brown",        0.72, 0.40, 0.24 );  

  insertColor( "white",        1.00, 1.00, 1.00 );  
  insertColor( "grey",         0.50, 0.50, 0.50 );
  insertColor( "black",        0.00, 0.00, 0.00 );  
}

//----------------------------------------------------------
void canvas3D::insertColor(const char* name, double r, double g, double b) 
{
  char* name_str = strdup(name);
  assert( ! lookupColor(name_str) );
  GLfloat * mat_amb_color = new GLfloat[4];
  mat_amb_color[0] = r;
  mat_amb_color[1] = g;
  mat_amb_color[2] = b;
  mat_amb_color[3] = 1.0;
  _clut.insert(name_str,mat_amb_color);
}

//----------------------------------------------------------
const GLfloat* canvas3D::lookupColor(char* name) const 
{
  dic_item di = _clut.lookup(name);
  if (di) {
    return _clut.inf(di);
  }
  return 0;
}


//----------------------------------------------------------
void canvas3D::noteEvent(Event * e) 
{
  char text[256];
  sprintf(text, "%s", e->getType());

  eventTypeRegistry::event_type t = theEventTypeRegistry().str2type(text);
  theEventTypeRegistry().event2color(_eventcolor, t);
  _activeNode = 0;

  if ( t == eventTypeRegistry::Global_Synchrony_type ||
       t == eventTypeRegistry::Global_Discord_type )
    strcpy( _eventnode, "all" );
  else if ( t == eventTypeRegistry::Local_Synchrony_type ||
	    t == eventTypeRegistry::Local_Discord_type ) {
    strcpy( _eventnode, "local" );
    _activeNode = e->getNode();
  } else if ( e->getNode() )
    strcpy(_eventnode, e->getNode()->Print());
  else
    strcpy(_eventnode, "");

  repaint(false);
}

bool IsSamePG( const NodeID * nid, const char * name )
{
  bool rval = false;
  // This should be a physical NodeID
  NodeID tmp( name );
  int level = nid->GetLevel();

  const u_char * lhs = nid->GetNID();
  const u_char * rhs = tmp.GetNID();

  // skip the first two bytes
  if ( bitcmp( lhs + 2, rhs + 2, level ) == 0 )
    rval = true;
  return rval;
}
