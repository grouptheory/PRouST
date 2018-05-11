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
#ifndef __TEXFONT_H__
#define __TEXFONT_H__
#ifndef LINT
static char const _textfont_h_rcsid_[] =
"$Id: TexFont.h,v 1.1 1999/01/04 16:23:30 mountcas Exp $";
#endif

/* Copyright (c) Mark J. Kilgard, 1997. */

/* This program is freely distributable without licensing fees  and is
   provided without guarantee or warrantee expressed or implied. This
   program is -not- in the public domain. */

#include <GL/gl.h>

#define TXF_FORMAT_BYTE		0
#define TXF_FORMAT_BITMAP	1

typedef struct {
  unsigned short c;       /* Potentially support 16-bit glyphs. */
  unsigned char width;
  unsigned char height;
  signed char xoffset;
  signed char yoffset;
  signed char advance;
  char dummy;           /* Space holder for alignment reasons. */
  short x;
  short y;
} TexGlyphInfo;

typedef struct {
  GLfloat t0[2];
  GLshort v0[2];
  GLfloat t1[2];
  GLshort v1[2];
  GLfloat t2[2];
  GLshort v2[2];
  GLfloat t3[2];
  GLshort v3[2];
  GLfloat advance;
} TexGlyphVertexInfo;

typedef struct {
  GLuint texobj;
  int tex_width;
  int tex_height;
  int max_ascent;
  int max_descent;
  int num_glyphs;
  int min_glyph;
  int range;
  unsigned char *teximage;
  TexGlyphInfo *tgi;
  TexGlyphVertexInfo *tgvi;
  TexGlyphVertexInfo **lut;
} TexFont;

extern char *txfErrorString(void);

extern TexFont *txfLoadFont(
  char *filename);

extern void txfUnloadFont(
  TexFont * txf);

extern GLuint txfEstablishTexture(
  TexFont * txf,
  GLuint texobj,
  GLboolean setupMipmaps);

extern void txfBindFontTexture(
  TexFont * txf);

extern void txfGetStringMetrics(
  TexFont * txf,
  char *string,
  int len,
  int *width,
  int *max_ascent,
  int *max_descent);

extern void txfRenderGlyph(
  TexFont * txf,
  int c);

extern void txfRenderString(
  TexFont * txf,
  char *string,
  int len);

extern void txfRenderFancyString(
  TexFont * txf,
  char *string,
  int len);

#endif // __TEXFONT_H__
