/*
 * $Id$
 *
Copyright (c) 2001 Stephane Conversy, Jean-Daniel Fekete and Ecole des
Mines de Nantes.
All rights reserved.
 
This software is proprietary information of Stephane Conversy,
Jean-Daniel Fekete and Ecole des Mines de Nantes.  You shall use it
only in accordance with the terms of the license agreement you
accepted when downloading this software.  The license is available in
the file license.txt and at the following URL:
http://www.emn.fr/info/image/Themes/Indigo/licence.html

 */
#if defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__)
#include <windows.h>
#endif

#include <glft/debug.hpp>
#include <glft/glft.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_IMAGE_H

#undef __FTERRORS_H__                                                 
#define FT_ERRORDEF( e, v, s )  { e, s },                         
#define FT_ERROR_START_LIST  {                                    
#define FT_ERROR_END_LIST    { 0, 0 } };                          
  
const struct                                                      
{                                                                 
  int          err_code;                                          
  const char*  err_msg;
} ft_errors[] =                                                   

#include <freetype/fterrors.h>                                    

#include <GL/glu.h>

#include <assert.h>

#include <stdio.h>
/* malloc */
#include <stdlib.h>
/* memset */
#include <string.h>
/* rint */
#include <math.h>



#ifdef __use_gzip__
#include <zlib.h>
#endif

#ifdef __use_bzip2__
#include <bzlib.h>
#endif

#ifdef __use_png__
#include <png.h>
#endif

/*
see FT_Outline_Decompose
*/


#define DBG fprintf(stderr, __FILE__":%d\n", __LINE__);

#define  FLOOR(x)    ((x) & -64)
#define  CEILING(x)  (((x)+63) & -64)

int glft_ft_error;
int glft_gl_error;

struct point2i {
  int x,y;
  point2i(void) : x(0), y(0) {}
};

static int max_num_point=0;
//static int num_point =  0;
static struct point2i* point_list2i = 0;
static char * point_infos = 0;
static int dpi=0;


struct glft_font {
  FT_Face _FT_face;
  int ascent, descent, line_gap;
  int units;
};

struct glft_glyph_info {
  int xmin, ymin, xmax, ymax;
  int xadvance;
  int bearing_x, bearing_y;
};

/*
------------------------------
gl stuff
------------------------------
*/ 

/* CALLBACK is for windoze users... not SGI ones!
 Thanks to G. Lanois (gerard@msi.com)
 */
#if !defined(CALLBACK)
#define CALLBACK
#endif

#define CALLBACKARG 

#define DBG_CALLBACK 0

#if 1
static
void CALLBACK vertexCallback(GLdouble * v)
{
  glVertex2dv(v);
#if DBG_CALLBACK
  fprintf(stderr, "glVertex %f %f %s:%d\n", v[0], v[1], __FILE__, __LINE__);
#endif
}

static
void CALLBACK beginCallback(GLenum which)
{
#if DBG_CALLBACK
  fprintf(stderr, "glBegin %s:%d\n", __FILE__, __LINE__);
#endif
  glBegin(which);
}

static
void CALLBACK endCallback(void)
{
  glEnd();
}
#endif

int dbgCoordTooLarge_numpoint;
char dbgCoordTooLarge_char;
#define DBGTOOLARGE if(dbgCoordTooLarge_numpoint) { fprintf(stderr, "dbgTooLarge %s:%d\n",__FILE__,__LINE__); dbgCoordTooLarge_numpoint=0; }

#define DBG_GLU_TESS_COORD_TOO_LARGE 0

static
void CALLBACK errorCallback(GLenum errorCode)
{
  const GLubyte *estring = gluErrorString(errorCode);
  if(errorCode!=GLU_TESS_COORD_TOO_LARGE) {
    fprintf (stderr, "GLU: %s (try 'man gluTessCallback')\n", estring);
  }
#if DBG_GLU_TESS_COORD_TOO_LARGE
  else {
    fprintf (stderr, "GLU_TESS_COORD_TOO_LARGE char: %c\n", dbgCoordTooLarge_char);
    dbgCoordTooLarge_numpoint=1;
  }
#endif
  // exit (0);
}

static int combineTab_index = 0;	// re-initialised each time glft_glyph_render_plain is called.
#define TAB_INCR 4096

static
void CALLBACK combineCallback(GLdouble coords[3],
						GLdouble* vertex_data[4],
						GLfloat weight[4], GLdouble **dataOut )
{
  static int combineTab_size = 0;
  static GLdouble* combineTab = 0;
  int i;

#if DBG_CALLBACK
  fprintf(stderr,"combine callback %s:%d\n",__FILE__,__LINE__);
#endif

  /* adjust the size of the combine array */
  if (combineTab_index >= combineTab_size) {
	  combineTab_size += TAB_INCR;
	  combineTab = (GLdouble*) realloc (combineTab, combineTab_size * sizeof (double));
	  /*fprintf(stderr, "size = %d  %s:%d\n", combineTab_size, __FILE__, __LINE__);*/
  }

  *dataOut = combineTab + combineTab_index;
  combineTab_index += 4;
  for(i=0;i<4;++i)
    (*dataOut)[i] = coords[i];
}

typedef void (CALLBACK *glu_callback)();

static GLUtesselator* thetess;
static FT_Library _FT_Library;
static int inited=0;

const char *glft_printable, *glft_english_printable;



int
glft_set_gl_context()
{
  gluTessCallback(thetess, GLU_TESS_VERTEX,
                   (glu_callback)vertexCallback);
  gluTessCallback(thetess, GLU_TESS_BEGIN,
                   (glu_callback)beginCallback);
  gluTessCallback(thetess, GLU_TESS_END,
                   (glu_callback)endCallback);
  gluTessCallback(thetess, GLU_TESS_ERROR,
                   (glu_callback)errorCallback);
  gluTessCallback(thetess, GLU_TESS_COMBINE,
		  (glu_callback)combineCallback);
                   
  gluTessProperty(thetess, GLU_TESS_WINDING_RULE,  GLU_TESS_WINDING_NONZERO);
  gluTessNormal(thetess, 0, 0, 1);

  glft_gl_error = glGetError();
  if(glft_gl_error)
    return glft_gl_error;
  return 0;
}

int
glft_init(void)
{
  int error;
  glft_ft_error = 0;
  glft_gl_error = 0;

  point_list2i = 0;
  max_num_point = 3000;

  point_list2i = (struct point2i*) malloc(sizeof(struct point2i) * max_num_point);
  point_infos = (char*) malloc(sizeof(char) * max_num_point);

  glft_printable =
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789"
    "\"&<>?,;.:/\\~#'{}[]()-_¨^+%*£$!@ ="
    "¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾¿"
    "ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß"
    "àáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ"
    ;

  glft_english_printable =
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789"
    "\"&<>?,;.:/\\~#'{}[]()-_¨^+%*£$!@ ="
    "¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾¿"
    ;

  thetess = gluNewTess();
  if(!thetess) {
    glft_gl_error = glGetError();
    return glft_gl_error;
  }

  glft_gl_error = glft_set_gl_context();
  if(glft_gl_error)
    return glft_gl_error;

  error = FT_Init_FreeType( &_FT_Library );
  if (error) {
    fprintf(stderr, "could not create engine instance %s:%d\n", __FILE__, __LINE__);
    return error;
  }

  inited=1;
  return 0;
}

int
glft_done(void)
{
  gluDeleteTess(thetess);
  glft_gl_error = glGetError();
  if(glft_gl_error)
    return glft_gl_error;
  if(point_list2i)
    free(point_list2i);
  if(point_infos)
    free(point_infos);

  return FT_Done_FreeType(_FT_Library);
}


/*
----------------------------------------
font
----------------------------------------
*/


struct glft_font*
glft_font_new(const char* file)
{
  int error=0;
  struct glft_font* thefont = (struct glft_font*) malloc(sizeof(struct glft_font));

  if(!inited) {
    glft_init();
    inited=1;
  }

  error = FT_New_Face( _FT_Library, file, 0, &thefont->_FT_face );

  if (error) {
    fprintf(stderr, "could not open font file %s %s:%d\n", file, __FILE__, __LINE__);
    fprintf(stderr, "%s\n", ft_errors[error].err_msg);
    return 0;
  }
  
  error = FT_Select_Charmap(thefont->_FT_face, ft_encoding_unicode );                              
  error = FT_Set_Char_Size( thefont->_FT_face, 0, 12*64, dpi, dpi );  

  thefont->ascent = ((thefont->_FT_face))->ascender;
  thefont->descent = ((thefont->_FT_face))->descender;
  thefont->line_gap = ((thefont->_FT_face))->height;
  thefont->units = ((thefont->_FT_face))->units_per_EM;
  return thefont;
}


void
glft_font_delete(struct glft_font* thefont)
{
        
  FT_Done_Face(thefont->_FT_face);
  free(thefont);
}


int
glft_font_get_ascent(const struct glft_font * f)
{
  return f->ascent;
}

int
glft_font_get_descent(const struct glft_font * f)
{
  return f->descent;
}


int
glft_font_get_line_gap(const struct glft_font * f)
{
  return f->line_gap;
}

static
int
glft_get_glyph_info(struct glft_font* thefont, unsigned char thechar, int size, struct glft_glyph_info* info)
{
  int error;
  FT_UShort ttcharindex;
  error = FT_Set_Char_Size( thefont->_FT_face, 0, size*64, dpi, dpi );

  if (error) {
    fprintf(stderr, "could not set instance char size\n");
    fprintf(stderr, "%s\n", ft_errors[error].err_msg);
    return -1;
  }

  ttcharindex = FT_Get_Char_Index( thefont->_FT_face, thechar );
  error = FT_Load_Glyph( thefont->_FT_face, ttcharindex, 0);
  if(error) {
    fprintf(stderr, "load glyph '%c' error %s:%d\n", ttcharindex, __FILE__, __LINE__);
    fprintf(stderr, "%s\n", ft_errors[error].err_msg);
    return -1;
  }

  {
    FT_Glyph    glyph;
    FT_BBox   bbox;
    error = FT_Get_Glyph( ((thefont->_FT_face))->glyph, &glyph );
    FT_Glyph_Get_CBox(glyph, 0, &bbox );
    info->xmax = CEILING(bbox.xMax) >> 6;
    info->xmin = FLOOR(bbox.xMin) >> 6;
    info->ymax = CEILING(bbox.yMax) >> 6;
    info->ymin = FLOOR(bbox.yMin) >> 6;
  }

  info->xadvance = ((thefont->_FT_face))->glyph->metrics.horiAdvance >> 6;
  info->bearing_x = ((thefont->_FT_face))->glyph->metrics.horiBearingX >> 6;
  info->bearing_y = ((thefont->_FT_face))->glyph->metrics.horiBearingY >> 6;

  return 0;
}

int
glft_font_get_glyph_metrics(struct glft_font* thefont, unsigned char thechar, int size, int *xadvance)
{
  struct glft_glyph_info info;
  int err;
  
  err = glft_get_glyph_info(thefont, thechar, size, &info);
  if (err) {
    return err;
  }
  
  *xadvance = info.xadvance;
  return 0;
}



/*
---------------------------------------------
glyph
---------------------------------------------
*/

struct glft_contour;

struct glft_glyph {
  struct glft_font* font;
  int num_contours;
  int xadvance;
  int xmin, ymin, xmax, ymax;
  struct glft_contour* contours;
#if GLFT_USE_GLLIST
  GLint gllist;
#endif
};

struct glft_vector_font_glyph_info {
  struct glft_glyph* glyph;
};

struct glft_vector_font {
  struct glft_vector_font_glyph_info* glyph_infos[256];
};



struct glft_font*
glft_glyph_get_font(const struct glft_glyph* g)
{
  return g->font;
}

int
glft_glyph_get_xadvance(const struct glft_glyph * g)
{
  return g->xadvance;
}

void
glft_glyph_get_bounding_box(const struct glft_glyph * g, int* xmin, int* ymin, int* xmax, int* ymax)
{
  *xmin = g->xmin;
  *xmax = g->xmax;
  *ymin = g->ymin;
  *ymax = g->ymax;
}


enum state {
  start=0,
  onstate,
  readingbezier
};

enum transition {
  off=0,
  on
};


struct line2i {
  struct point2i p1,p2;
};


struct bezier3i {
  struct point2i p1, p2, p3;
};

#if 0
static
void
print_point2i(FILE* f, struct point2i p)
{
  fprintf(f, "%d %d\n", p.x, p.y);
}

static
void
print_bezier3i(FILE* f, struct bezier3i b)
{
  fprintf(f, "p1: "); print_point2i(stderr, b.p1);
  fprintf(f, "p2: "); print_point2i(stderr, b.p2);
  fprintf(f, "p3: "); print_point2i(stderr, b.p3);
}
#endif


static
int
subdivide(struct point2i* base)
{

  int a, b;

  base[4].x = base[2].x;
  b = base[1].x;
  a = base[3].x = ( base[2].x + b ) / 2;
  b = base[1].x = ( base[0].x + b ) / 2;
  base[2].x = ( a + b ) / 2;
  
  base[4].y = base[2].y;
  b = base[1].y;
  a = base[3].y = ( base[2].y + b ) / 2;
  b = base[1].y = ( base[0].y + b ) / 2;
  base[2].y = ( a + b ) / 2;

  return 0;
}

/* 
((initpointnum*2-1)*2-1)*2-1...  -1
remove 1 at the end since we don't emit the first point of the bezier curve (point emitted whith last primitive)
initpoint=3
1 4
2 8
3 16
4 32
*/

#define rastnumdiv 3
#define bezier_numpoints 16


#define DBG_BEZIER 0
#define DBG_POINT 0
#define DBG_PRIMITIVE 0
#define DBG_CONTOUR 0


static
void
subdivide_bezier(const struct bezier3i *b, struct point2i *ptab, int n)
{
  struct point2i points_stack[bezier_numpoints+1];
  int points_stack_top=0;
  int divnum[bezier_numpoints+1];
  int divnum_top =0;

  int pcount=0;
  int numdiv=1;
  int max;

#if DBG_BEZIER
  fprintf(stderr, "subdividing bezier p1:%d %d p2:%d %d p3:%d %d %s:%d\n", b->p1.x, b->p1.y, b->p2.x, b->p2.y, b->p3.x, b->p3.y, __FILE__, __LINE__);
#endif

  if(n==0) {
    ptab[0] = b->p1;
    ptab[1] = b->p3;
#if (DBG_BEZIER || DBG_POINT)
    fprintf(stderr, "added point %d %d %s:%d\n", ptab[0].x, ptab[0].y, __FILE__, __LINE__);
    fprintf(stderr, "added point %d %d %s:%d\n", ptab[1].x, ptab[1].y, __FILE__, __LINE__);
#endif
    return;
  }

  max = (n < rastnumdiv) ? n : rastnumdiv;
  pcount=bezier_numpoints-1;

  /* push */
  points_stack[0] = b->p1;
  points_stack[1] = b->p2;
  points_stack[2] = b->p3;
  points_stack_top=0;

  divnum_top=0;
  divnum[0]=0;
  numdiv=0;

  while(pcount>0) {
    while(numdiv < max) {
      /* subdivide_top */
      subdivide(&points_stack[points_stack_top]);
      divnum[divnum_top] = divnum[divnum_top]+1;
      ++divnum_top;
      divnum[divnum_top] = divnum[divnum_top-1];
      points_stack_top+=2;
      ++numdiv;
    }
    
    /* emit */

    ptab[pcount--] = points_stack[points_stack_top+2];
    ptab[pcount--] = points_stack[points_stack_top+1];

#if (DBG_BEZIER || DBG_POINT)
    fprintf(stderr, "added point %d %d %s:%d\n", ptab[pcount+2].x, ptab[pcount+2].y, __FILE__, __LINE__);
    fprintf(stderr, "added point %d %d %s:%d\n", ptab[pcount+1].x, ptab[pcount+1].y, __FILE__, __LINE__);
#endif

    points_stack_top -= 2;
    numdiv = divnum[--divnum_top];
  }
}


struct point {
  double x, y/*, z*/;
  point(void) : x(0.0), y(0.0) {}
};

struct glft_contour {
  int num_points;
  struct point *points;
};

#if 0
static
void
print_point(FILE* f, struct point p)
{
  /*  fprintf(f, "p1: %lf %lf %lf\n", p.x, p.y, p.z);*/
  fprintf(f, "%f %f\n", p.x, p.y);
}
#endif

static
struct point2i
middle_point2i(struct point2i p1, struct point2i p2)
{
  struct point2i p;
  p.x = (p1.x+p2.x) >> 1;
  p.y = (p1.y+p2.y) >> 1;
  return p;
}

static
struct point
point2i_to_point(struct point2i p2i)
{
  struct point p;
  p.x = p2i.x;
  p.y = p2i.y;
  /*  p.z = 0;*/
  return p;
}

static
void
scale(struct point* p, double v)
{
  p->x*=v;
  p->y*=v;
  /*  p->z*=v;*/
}

static
int
new_read_points(FT_Vector *points, char* flags, int first, int last, int * num_pointp)
{
  enum state current = start;
  enum transition trans=off;
  int i, j, end;
  struct line2i theline;
  struct bezier3i thebezier;
  struct point2i p;

  int num_point=0;

  end=0;
  for(i=first; !end && i<=last+1; ++i) {
    int t;
    if(i==last+1) {
      i=first;
      end=1;
    }
    t = flags[i] & 0x01;
    if(t==0)
      trans = off;
    else if(t==1)
      trans = on;
    else {
      fprintf(stderr, "transition error\n");
      return 1;
    }
    switch(current) {
    case start:
      switch (trans) {
      case on:
#if DBG_PRIMITIVE
	fprintf(stderr, "start on\n");
#endif
        p.x = points[i].x;
        p.y = points[i].y;

        /* points */
        if(num_point==max_num_point) {
          fprintf(stderr, "read_point too many points %d\n", i);
          return 1;
        }
        point_list2i[num_point]=p;
#if DBG_POINT
	fprintf(stderr, "added point %d %d %s:%d\n", p.x, p.y, __FILE__, __LINE__);
#endif
        ++num_point;

        current = onstate;
        break;
      case off:
#if DBG_PRIMITIVE
	fprintf(stderr, "start off\n");
#endif

        fprintf(stderr, "read_point error %d  %s:%d\n", i, __FILE__, __LINE__);
        return 1;
      }
      break;
    case onstate:
      switch (trans) {
      case on:
#if DBG_PRIMITIVE
	fprintf(stderr, "onstate on (line)\n");
#endif
        theline.p1 = p;
        theline.p2.x = points[i].x;
        theline.p2.y = points[i].y;

        p = theline.p2;

        /* points */
        if(num_point==max_num_point) {
          fprintf(stderr, "read_point too many points %d\n", i);
          return 1;
        }

        point_list2i[num_point]=p;
	point_infos[num_point]=1;
        ++num_point;
#if DBG_POINT
	fprintf(stderr, "added point %d %d %s:%d\n", p.x, p.y, __FILE__, __LINE__);
#endif

        break;

      case off:
#if DBG_PRIMITIVE
	fprintf(stderr, "onstate off\n");
#endif

        thebezier.p1 = p;
        thebezier.p2.x = points[i].x;
        thebezier.p2.y = points[i].y;
        
        p = thebezier.p2;
        current = readingbezier;
        break;
      }
      break;
    case readingbezier:
      switch (trans) {
      case on:
#if DBG_PRIMITIVE
	fprintf(stderr, "bezier on (simple)\n");
#endif
        thebezier.p3.x = points[i].x;
        thebezier.p3.y = points[i].y;
        p = thebezier.p3;

        /* points */
        if(num_point+bezier_numpoints >=max_num_point) {
          fprintf(stderr, "read_point too many points %d\n", i);
          return 1;
        }

        subdivide_bezier(&thebezier, &point_list2i[num_point], rastnumdiv);
	for(j=num_point; j<num_point+bezier_numpoints; ++j)
	  point_infos[j]=0;
        num_point += bezier_numpoints;

        current = onstate;
        break;
      case off:
#if DBG_PRIMITIVE
	fprintf(stderr, "bezier off (multi)\n");
#endif
        p.x = points[i].x;
        p.y = points[i].y;
        thebezier.p3 = middle_point2i(thebezier.p2,p);

        /* points */
        if(num_point+bezier_numpoints >=max_num_point) {
          fprintf(stderr, "read_point too many points %d\n", i);
          return 1;
        }

        subdivide_bezier(&thebezier, &point_list2i[num_point], rastnumdiv);
	for(j=num_point; j<num_point+bezier_numpoints; ++j)
	  point_infos[j]=0;
        num_point += bezier_numpoints;

        thebezier.p1= thebezier.p3;
        thebezier.p2 = p;
        
        break;
      }
      break;
    default:
      fprintf(stderr, "unknown state: %d\n", i);
      return 1;
      break;
    }
  }

#if 1
  /* don't close contour */
 {
   while( num_point ) { /* should handle only one point, just in case */
     double dx = abs(point_list2i[num_point-1].x-point_list2i[0].x);
     double dy = abs(point_list2i[num_point-1].y-point_list2i[0].y);

     if( (dx<.001) && (dy<.001)) --num_point;
     else break;
   }
 }
  /*
  else
    fprintf(stderr, "not closed contour: %d %d %d %d\n",point_list2i[0].x,point_list2i[num_point-1].x, point_list2i[0].y, point_list2i[num_point-1].y);
  */
#endif

  *num_pointp = num_point;
  return 0;
}

struct glft_glyph*
glft_new_glyph(struct glft_font* thefont, char _thechar, int size)
{
  int error;
  FT_Outline _FT_outline;
  struct glft_glyph theglyph;
  struct glft_glyph* rglyph;
  int i;
  struct glft_glyph_info info;

  error = glft_get_glyph_info(thefont, _thechar, size, &info);
  if(error) {
    fprintf(stderr, "can't get glyph info %s:%d\n", __FILE__, __LINE__);
    return 0;
  }

  theglyph.xmax = info.xmax;
  theglyph.xmin = info.xmin;
  theglyph.ymax = info.ymax;
  theglyph.ymin = info.ymin;
  theglyph.xadvance = info.xadvance;
#if GLFT_USE_GLLIST
  theglyph.gllist = 0;
#endif

  _FT_outline = ((thefont->_FT_face))->glyph->outline;
  rglyph = (struct glft_glyph*) malloc(sizeof(struct glft_glyph));

  {
    int first_point=0;
    theglyph.num_contours = _FT_outline.n_contours;
    theglyph.contours = (struct glft_contour*) malloc( sizeof(struct glft_contour) * theglyph.num_contours);

		double x1=0,y1=0,x2=0,y2=0;
		
    for(i=0; i<_FT_outline.n_contours; ++i) {
			int j;
			int num_point;
			
#if DBG_POINT
			fprintf(stderr, "char:%c contour:%d %s:%d\n",_thechar,i,__FILE__,__LINE__);
#endif
			int err = new_read_points(_FT_outline.points,_FT_outline.tags, first_point,_FT_outline.contours[i], &num_point );
			assert(err==0);
			/*fprintf(stderr, "%d\n", num_point);*/
			
			theglyph.contours[i].num_points = num_point;
			theglyph.contours[i].points = (struct point*) malloc (sizeof (struct point) * theglyph.contours[i].num_points);
			for(j=0; j<num_point; ++j) {
				theglyph.contours[i].points[j] = point2i_to_point(point_list2i[j]);
				scale(&theglyph.contours[i].points[j],1/64.);
				if(theglyph.contours[i].points[j].x<x1) x1=theglyph.contours[i].points[j].x;
				if(theglyph.contours[i].points[j].y<y1) y1=theglyph.contours[i].points[j].y;
				if(theglyph.contours[i].points[j].x>x2) x2=theglyph.contours[i].points[j].x;
				if(theglyph.contours[i].points[j].y>y2) y2=theglyph.contours[i].points[j].y;
			}
#if 0			
			if(_thechar=='p') {
				fprintf(stderr, "'%c' %d %f %f %f %f\n", _thechar, size, x1,y1,x2,y2);
			}
#endif
			first_point = _FT_outline.contours[i]+1;
    }
  }
	
  *rglyph = theglyph;
  return rglyph;
}

void
glft_delete_glyph(struct glft_glyph* theglyph)
{
  int i;
  for(i=0; i<theglyph->num_contours; ++i) {
    free(theglyph->contours[i].points);
  }
  free(theglyph->contours);
  free(theglyph);
}



void
glft_glyph_render_plain(struct glft_glyph* theglyph)
{
  int i;

#if GLFT_USE_GLLIST
  if(theglyph->gllist) {
    glCallList(theglyph->gllist);
  } else {
    theglyph->gllist = glGenLists(1);
    glNewList(theglyph->gllist, GL_COMPILE);
#endif

  gluTessProperty(thetess,  GLU_TESS_BOUNDARY_ONLY, GL_FALSE);
// reinitialise the index of the combine array.
  combineTab_index = 0;
  gluTessBeginPolygon(thetess,0);
  for(i=0; i< theglyph->num_contours; ++i) {
    int j;
    gluTessBeginContour(thetess);
    for(j=0; j<theglyph->contours[i].num_points; ++j) {
#if DBG_CONTOUR
	fprintf(stderr, "contour:%d point:%d x:%f y:%f %s:%d\n", i,j,theglyph->contours[i].points[j].x,theglyph->contours[i].points[j].y,__FILE__,__LINE__);
#endif
      gluTessVertex(thetess,
		    (GLdouble*)&theglyph->contours[i].points[j],
		    &theglyph->contours[i].points[j]);
    }
    gluTessEndContour(thetess);
  }
  gluTessEndPolygon(thetess);

#if GLFT_USE_GLLIST
  glEndList();
  glCallList(theglyph->gllist);
  
  }
#endif
}

void
glft_glyph_render_outline(const struct glft_glyph* theglyph)
{
  int i;
  for(i=0; i< theglyph->num_contours; ++i) {
    int j;
    glBegin(GL_LINE_STRIP);
    for(j=0; j<theglyph->contours[i].num_points; ++j) {
      glVertex2d(theglyph->contours[i].points[j].x, theglyph->contours[i].points[j].y);
    }
    glEnd();
  }
}

int
test_load_save(const struct glft_vector_font* font1)
{
  struct glft_vector_font* font2;
  FILE * file;
  int i,j;

  fprintf(stderr, "test loadsave %s:%d\n", __FILE__, __LINE__);

  file = fopen("testfont", "wb");
  glft_vector_font_save(font1, file);
  fclose(file);
  file = fopen("testfont", "rb");
  font2 = (struct glft_vector_font*) malloc(sizeof(struct glft_vector_font));
  for(i=0;i<256;++i) {
    font2->glyph_infos[i]=0;
  }

  glft_vector_font_load(font2, file);

  for(i=0;i<256;++i) {
    if( (font1->glyph_infos[i] && !font2->glyph_infos[i]) || (!font1->glyph_infos[i] && font2->glyph_infos[i]) ) {
      fprintf(stderr, "glyph %d differ %s:%d\n", i,__FILE__,__LINE__);
      return 0;
    }

    if(font1->glyph_infos[i] && font2->glyph_infos[i] && font1->glyph_infos[i]->glyph && font2->glyph_infos[i]->glyph) {

      if(font1->glyph_infos[i]->glyph->num_contours != font2->glyph_infos[i]->glyph->num_contours) {
	fprintf(stderr, "glyph %d num_contours differ: %d vs %d %s:%d\n", i, font1->glyph_infos[i]->glyph->num_contours, font2->glyph_infos[i]->glyph->num_contours, __FILE__,__LINE__);
	return 0;	
      }

      for(j=0; j<font1->glyph_infos[i]->glyph->num_contours; ++j) {

	if(font1->glyph_infos[i]->glyph->contours[j].num_points != font2->glyph_infos[i]->glyph->contours[j].num_points) {
	  fprintf(stderr, "glyph %d num_points differ in contour %d: %d vs %d %s:%d\n", i, j, font1->glyph_infos[i]->glyph->contours[j].num_points, font2->glyph_infos[i]->glyph->contours[j].num_points, __FILE__,__LINE__);
	  return 0;	
	}

	if(strncmp((const char*)font1->glyph_infos[i]->glyph->contours[j].points, (const char*)font2->glyph_infos[i]->glyph->contours[j].points, font1->glyph_infos[i]->glyph->contours[j].num_points*sizeof(point))) {
	  fprintf(stderr, "glyph %d points differ in contour %d %s:%d\n", i, j, __FILE__,__LINE__);
	  return 0;	
	}
      }
    }

  }
  return 1;
}


int
glft_glyph_save(const struct glft_glyph* glyph, FILE* file)
{
  typedef int vector_glyph_coord_t;
  int i;
  
  vector_glyph_coord_t one=1;
  fwrite((void*)&one, sizeof(vector_glyph_coord_t),1,file);
  fwrite((void*)&glyph->num_contours, sizeof(int), 6, file);
  for(i=0; i<glyph->num_contours; ++i) {
    fwrite((void*)&(glyph->contours[i].num_points), sizeof(int), 1, file);
    fwrite((void*)glyph->contours[i].points, sizeof(struct point), glyph->contours[i].num_points, file);
  }

  return 0;
}

int
glft_glyph_load(struct glft_glyph* glyph, FILE* file)
{
  typedef int vector_glyph_coord_t;
  vector_glyph_coord_t one=1;
  int i;
  int tmp;

  fread((void*)&one, sizeof(vector_glyph_coord_t),1,file);
  tmp = glyph->num_contours;
  fread((void*)&(glyph->num_contours), sizeof(int), 6, file);

  if(glyph->contours) {
    for(i=0; i<tmp;++i)
      free(glyph->contours[i].points);
    free(glyph->contours);
  }

  glyph->contours = (struct glft_contour*) malloc(sizeof(struct glft_contour) * glyph->num_contours);

  for(i=0; i<glyph->num_contours; ++i) {
    fread((void*)&(glyph->contours[i].num_points), sizeof(int), 1, file);
    glyph->contours[i].points = (struct point*) malloc(sizeof(struct point) * glyph->contours[i].num_points);
    fread((void*)glyph->contours[i].points, sizeof(struct point), glyph->contours[i].num_points, file);
  }

#if GLFT_USE_GLLIST
  glyph->gllist=0;
#endif

  return 0;
}


/*
---------------------------------------------
vector font
---------------------------------------------
*/


void
glft_vector_font_glyph_info_delete(struct glft_vector_font_glyph_info* info)
{
  glft_delete_glyph(info->glyph);
  free(info);
}

struct glft_vector_font*
glft_new_vector_font(struct glft_font* thefont, const char* charset, int thesize)
{
  struct glft_vector_font *res;
  const char *charset_iterator=charset;
  char done[256];
  int i;
  
  res = (struct glft_vector_font*)malloc(sizeof(struct glft_vector_font));

  for(i=0;i<256;++i) {
    done[i]=0;
    res->glyph_infos[i]=0;
  }

  while(* charset_iterator) {
    unsigned char thechar = (unsigned int)*charset_iterator;
    /*    fprintf(stderr,"%d %c %d\n", i++, thechar, thechar);*/
    if (thechar==129) {
      ++charset_iterator;
      continue;
    }
    /*    fprintf(stderr, "-- %c\n",*char_it);*/

    if(!done[thechar]) {
      struct glft_glyph* theglyph;
      theglyph = glft_new_glyph(thefont, *(charset_iterator), thesize);
      if(theglyph) {
	   res->glyph_infos[thechar] = (struct glft_vector_font_glyph_info*) malloc(sizeof(struct glft_vector_font_glyph_info));
	   res->glyph_infos[thechar]->glyph = theglyph;
      }
      done[thechar]=1;
    }
    ++charset_iterator;
  }
  return res;
}

struct glft_vector_font*
glft_vector_font_new()
{
  struct glft_vector_font *res;
  int i;
  res = (struct glft_vector_font*)malloc(sizeof(struct glft_vector_font));

  for(i=0;i<256;++i) {
    res->glyph_infos[i]=0;
  }

  return res;
}

void
glft_vector_font_delete(struct glft_vector_font* font)
{
  int i;
  for(i=0;i<256;++i) {
    if(font->glyph_infos[i]) {
      glft_vector_font_glyph_info_delete(font->glyph_infos[i]);
    }
  }
  free(font);
}

void
glft_vector_font_render_outline(const struct glft_vector_font* font, const char* str)
{
  while(*str) {
    unsigned char thechar= *((unsigned char*)str);
    if(font->glyph_infos[thechar]) {
      const struct glft_glyph* glyph = font->glyph_infos[thechar]->glyph;
      glft_glyph_render_outline(glyph);
      glTranslated(glyph->xadvance, 0, 0);
    }    
    ++str;
  }
}

int
test_load_save(const struct glft_vector_font* font1);

void
glft_vector_font_render_plain(const struct glft_vector_font* font, const char* str)
{
  /*test_load_save(font);*/

  while(*str) {
    unsigned char thechar= *((unsigned char*)str);
    if(font->glyph_infos[thechar]) {
      struct glft_glyph* glyph = font->glyph_infos[thechar]->glyph;
#if DBG_CONTOUR
      fprintf(stderr,"rendering char:%c %s:%d\n", *str, __FILE__,__LINE__);
#endif
      dbgCoordTooLarge_char = *str;
      glft_glyph_render_plain(glyph);
      glTranslated(glyph->xadvance, 0, 0);
    }    
    ++str;
  }
}

int
glft_vector_font_get_xadvance(struct glft_vector_font* font, const char* str)
{
  int xadvance=0;
  while(*str) {
    unsigned char thechar= *((unsigned char*)str);
    if(font->glyph_infos[thechar]) {
      const struct glft_glyph* glyph = font->glyph_infos[thechar]->glyph;
      xadvance+=glyph->xadvance;
    }    
    ++str;
  }
  return xadvance;
}

#if 0
int
glft_vector_font_get_xadvance_char(struct glft_vector_font* font, char c)
{
  int xadvance=0;
  unsigned char thechar= c;
  if(font->glyph_infos[thechar]) {
    const struct glft_glyph* glyph = font->glyph_infos[thechar]->glyph;
    xadvance=glyph->xadvance;
  }    
  return xadvance;
}
#endif

void
glft_vector_font_bounding_box(struct glft_vector_font* font, const char* thestring, int* x1, int* y1, int* x2, int* y2)
{
  *x1=*y1=*x2=*y2=0;
  int unset=1;
  while(*thestring) {
    unsigned char thechar = *thestring;
    if(font->glyph_infos[thechar]) {
      const struct glft_glyph* glyph = font->glyph_infos[thechar]->glyph;
      if(!glyph) {
	   ++thestring;
	   continue;
      }
      /*fprintf(stderr, "%d %d\n", glyph->ymin, glyph->ymax);*/
      if(unset) {
	   *y1=glyph->ymin;
	   *y2=glyph->ymax;
	   unset=0;
      } else {
	   if(glyph->ymin<*y1)
		*y1=glyph->ymin;
	   if(glyph->ymax>*y2)
		*y2=glyph->ymax;
      }

      *x2+=glyph->xadvance;
    }
    ++thestring;
  }

}


int
glft_vector_font_save_glyph(const struct glft_vector_font* font, char str, FILE* file)
{
  unsigned char thechar=str;
  if(font->glyph_infos[thechar])
    return glft_glyph_save(font->glyph_infos[thechar]->glyph, file);
  else
    return 1;
}

int
glft_vector_font_load_glyph(struct glft_vector_font* font, char str, FILE* file)
{
  unsigned char thechar=str;
  if(font->glyph_infos[thechar]) {
    glft_vector_font_glyph_info_delete(font->glyph_infos[thechar]);
  }
  font->glyph_infos[thechar] = (struct glft_vector_font_glyph_info*) malloc(sizeof(struct glft_vector_font_glyph_info));

  font->glyph_infos[thechar]->glyph=(struct glft_glyph*) malloc(sizeof(struct glft_glyph));
  font->glyph_infos[thechar]->glyph->contours=0;
  font->glyph_infos[thechar]->glyph->num_contours=0;

  return glft_glyph_load(font->glyph_infos[thechar]->glyph, file);
}


int
glft_vector_font_save(const struct glft_vector_font* font, FILE* file)
{
  typedef int index_t;
  int i;
  int res;
  int num_glyphes=0;

  for(i=0;i<256;++i) {
    if(font->glyph_infos[i]) {
      ++num_glyphes;
    }
  }

  fwrite((void*)&num_glyphes, sizeof(int), 1, file);

  if(!num_glyphes)
    return 0;

  for(i=0;i<256;++i) {
    if(font->glyph_infos[i]) {
      //fprintf(stderr, "save glyph %d\n",i);
      fwrite((void*)&i, sizeof(index_t), 1, file);
      res = glft_glyph_save(font->glyph_infos[i]->glyph, file);
      if(res)
	   return res;
    }
  }

  return 0;
}

int
glft_vector_font_load(struct glft_vector_font* font, FILE* file)
{
  typedef int index_t;
  index_t i;
  int res;

  int num_glyphes=0;
  fread((void*)&num_glyphes, sizeof(int), 1, file);
  if(!num_glyphes)
    return 0;

  for(i=0;i<num_glyphes;++i) {
    index_t num;
    fread((void*)&num, sizeof(index_t), 1, file);
    //fprintf(stderr, "load glyph %d\n",num);
    res = glft_vector_font_load_glyph(font, num, file);
    if(res)
      return res;
  }

  return 0;
}

/*
---------------------------------------------
pixmap glyph
---------------------------------------------
*/


struct glft_pixmap_glyph {
  struct glft_font* font;
  int xadvance;
  int xmin, ymin, xmax, ymax;
  unsigned char* _pixmap;
};


struct glft_font*
glft_pixmap_glyph_get_font(const struct glft_pixmap_glyph* g)
{
  return g->font;
}

int
glft_pixmap_glyph_get_xadvance(const struct glft_pixmap_glyph * g)
{
  return g->xadvance;
}

void
glft_pixmap_glyph_get_bounding_box(const struct glft_pixmap_glyph * g, int* xmin, int* ymin, int* xmax, int* ymax)
{
  *xmin = g->xmin;
  *xmax = g->xmax;
  *ymin = g->ymin;
  *ymax = g->ymax;
}

struct glft_pixmap_glyph*
glft_new_pixmap_glyph(struct glft_font* thefont, char _thechar, int size)
{
  int error;
  struct glft_pixmap_glyph theglyph;
  struct glft_pixmap_glyph* rglyph;
  struct glft_glyph_info info;

  /*  fprintf(stderr,"%d %c\n", ttcharindex, thechar);*/
  error = glft_get_glyph_info(thefont, _thechar, size, &info);
  if(error) {
    fprintf(stderr, "can't get glyph info %s:%d\n", __FILE__, __LINE__);
    return 0;
  }

  theglyph.xmax = info.xmax;
  theglyph.xmin = info.xmin;
  theglyph.ymax = info.ymax;
  theglyph.ymin = info.ymin;
  theglyph.xadvance = info.xadvance;

  error = FT_Render_Glyph( ((thefont->_FT_face))->glyph, ft_render_mode_normal );
  {
    int l = thefont->_FT_face->glyph->bitmap.width * thefont->_FT_face->glyph->bitmap.rows;
    theglyph._pixmap = (unsigned char*)malloc(l);
    memcpy(theglyph._pixmap, thefont->_FT_face->glyph->bitmap.buffer, l);
  }

  rglyph = (struct glft_pixmap_glyph*) malloc(sizeof(struct glft_pixmap_glyph));
  *rglyph = theglyph;

  return rglyph;
}

void
glft_pixmap_glyph_delete(struct glft_pixmap_glyph* theglyph)
{
  free(theglyph->_pixmap);
  free(theglyph);
}

unsigned char*
glft_pixmap_glyph_get_pixmap(const struct glft_pixmap_glyph * g) {
  return g->_pixmap;
}


void
glft_pixmap_glyph_render(struct glft_pixmap_glyph* theglyph)
{ 
  glRasterPos2i(theglyph->xmin, theglyph->ymin);
  glDrawPixels(theglyph->xmax-theglyph->xmin, theglyph->ymax-theglyph->ymin, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, theglyph->_pixmap);
}



/*
---------------------------------------
pixmap font
---------------------------------------
*/


// #define PRECOMPUTED_TEXCOORD

typedef short coord_t;
struct texture_glyph_info {
  coord_t x1, y1, x2, y2, xadvance, bearing_x, bearing_y;
	coord_t xmax; // for bbox
};

#ifdef PRECOMPUTED_TEXCOORD
struct texture_glyph_info_double {
  double x1, y1, x2, y2;
  coord_t xadvance, bearing_x, bearing_y;
};
#endif


struct glft_pixmap_font {
  int width, height;
  unsigned char* pixmap;
#ifdef PRECOMPUTED_TEXCOORD
  struct texture_glyph_info_double* glyph_infos[256];
#else
  struct texture_glyph_info* glyph_infos[256];
#endif
};


int
glft_pixmap_font_get_width(const struct glft_pixmap_font* tex)
{
  return tex->width;
}

int
glft_pixmap_font_get_height(const struct glft_pixmap_font* tex)
{
  return tex->height;
}

unsigned char*
glft_pixmap_font_get_pixmap(const struct glft_pixmap_font* tex)
{
  return tex->pixmap;
}

/* ----------------------------------------------------- */
/* factory */

struct glft_pixmap_font*
glft_new_pixmap_font(struct glft_font* thefont, const char* charset, int thesize)
{
  return glft_new_pixmap_font_maxsize(thefont, charset, thesize, -1);
}


struct glft_pixmap_font*
glft_new_pixmap_font_maxsize(struct glft_font* thefont, const char* charset, int thesize, GLint max_size)
{
  int i=0, err = 0;
  char done[256];
  struct texture_glyph_info infos[256];
  const char * char_it;

  int width=0, height=0;
  int wgap=2, hgap=2;
  int max_size_reached=0;
  int max_height=0;

  struct glft_pixmap_font* thetexture;

  if(max_size==-1)
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);
  else {
    int t = max_size;
    while (t) {
      if(t&0x1) {
	   if (t!=1) {
		fprintf(stderr, "max_size %d is not a power of 2\n", (int)max_size);
		return 0;
	   }
      }
      t>>=1;
    }
  }


  for(i=0; i<256; ++i) {
    done[i]=0;
  }

  /* first find the texture size */
  char_it = charset;
  i=0;
  while(*char_it) {
    unsigned char thechar = (unsigned int)*char_it;
    if (thechar==129) {
      ++char_it;
      continue;
    }
    /*    fprintf(stderr, "-- %c\n",*char_it);*/
    /*    fprintf(stderr,"%d %c %d\n", i++, thechar, thechar);*/

    if(!done[thechar]) {
      int w,h;
      struct glft_glyph_info theinfo;
      /*      fprintf (stderr, "-> %c\n", thechar);*/
      err = glft_get_glyph_info(thefont, thechar, thesize, &theinfo);
      if(err) {
	   fprintf(stderr, "%s:%d - can't get glyph info\n", __FILE__, __LINE__);
	   /*	return 0;*/
	   ++char_it;
	   continue;
      }

      /*      fprintf (stderr, "<-\n");*/
      w = theinfo.xmax - theinfo.xmin ;
      h = theinfo.ymax - theinfo.ymin ;

      infos[thechar].x1 = width;
      infos[thechar].y1 = height;

      infos[thechar].xadvance = theinfo.xadvance;
      infos[thechar].bearing_x = theinfo.bearing_x;
      infos[thechar].bearing_y = theinfo.bearing_y;
      infos[thechar].xmax = theinfo.xmax;
      
      if(width + w + wgap > max_size) {
	   /*	fprintf(stderr, "%s:%d\n", __FILE__, __LINE__);*/
	   width = 0;
	   max_size_reached=1;
	   height += max_height + hgap;
	   if(height>max_size || w>max_size) {
		fprintf(stderr, "texture too large\n");
		break;
	   }
	   infos[thechar].x1 = width;
	   infos[thechar].y1 = height;
	   max_height = h;
      }
      else {
	   if(h>max_height)
		max_height=h;
      }
      width += w + wgap;
      infos[thechar].x2=infos[thechar].x1+w;
      infos[thechar].y2=infos[thechar].y1+h;      

      done[thechar] = 1;
    }
    ++char_it;
  }

  if(max_size_reached)
    width = max_size;
  height+= max_height;
  if(height>max_size) {
    fprintf(stderr, "%s l:%d - texture too large (%dx%d) max:%d\n", __FILE__, __LINE__, width,height,(int)max_size);
    return 0;
  }

  { /* dimensions as a power of 2 */
    int h=1;
    int w=1;
    while(h<height)
      h<<=1;
    while(w<width)
      w<<=1;
    if(w>max_size || h>max_size) {
      fprintf(stderr, "%s l:%d - texture too large (%dx%d) max:%d\n", __FILE__, __LINE__, w,h,(int)max_size);
      return 0;
    }
    width=w;
    height=h;
    /*    fprintf(stderr, "%d %d\n", width, height);*/
  }


  /* then render... */

  thetexture = (struct glft_pixmap_font*) malloc(sizeof(struct glft_pixmap_font));
  thetexture->pixmap = (unsigned char*)malloc (width * height);
  memset( (void*) thetexture->pixmap, 0, width * height);

  thetexture->width=width;
  thetexture->height=height;

  for(i=0; i<256; ++i) {
    if(done[i]) {
      int j,k;
      unsigned char *from, *to;
      struct glft_pixmap_glyph* theglyph;

      theglyph = glft_new_pixmap_glyph(thefont, i, thesize);

      from = theglyph->_pixmap;
      to = thetexture->pixmap + (infos[i].y1 * width + infos[i].x1);
      for(j=0; j<(infos[i].y2-infos[i].y1); ++j) {
	   for(k=0; k<infos[i].x2-infos[i].x1; ++k) {
		*to++ = *from++;
	   }
	   to+= (width - (infos[i].x2-infos[i].x1));
      }

      glft_pixmap_glyph_delete(theglyph);
#ifdef PRECOMPUTED_TEXCOORD
      // ok now scale the coords relative to the texture dimension
      if(1) {
	   struct texture_glyph_info_double * info = (struct texture_glyph_info_double*) malloc(sizeof(struct texture_glyph_info_double));
	   thetexture->glyph_infos[i] = info;

	   info->xadvance = infos[i].xadvance;
	   info->bearing_x = infos[i].bearing_x;
	   info->bearing_y = infos[i].bearing_y;

	   info->x1 = infos[i].x1 / (double) width;
	   info->x2 = infos[i].x2 / (double) width;
	   info->y1 = infos[i].y1 / (double) height;
	   info->y2 = infos[i].y2 / (double) height;
	   //std::cerr << info->x1 << std::endl;
      }
#else
      struct texture_glyph_info * info = (struct texture_glyph_info*) malloc(sizeof(struct texture_glyph_info));
      thetexture->glyph_infos[i] = info;
      *info = infos[i];
#endif

    }
    else {
      thetexture->glyph_infos[i]=0;
    }
  }

  return thetexture;
}

struct glft_pixmap_font*
glft_pixmap_font_new()
{
  int i;
  struct glft_pixmap_font* res = (struct glft_pixmap_font*) malloc(sizeof(struct glft_pixmap_font));
  if(!res)
    return 0;
  res->pixmap = 0;
  for(i=0;i<256;++i)
    res->glyph_infos[i]=0;
  res->width = res->height = 0 ; 
  return res;
}


void
glft_pixmap_font_delete(struct glft_pixmap_font* tex)
{
  int i;

  for(i=0; i<256; ++i)
    if(tex->glyph_infos[i])
      free(tex->glyph_infos[i]);
  if(tex->pixmap)
    free(tex->pixmap);
  free(tex);
}

void
glft_pixmap_font_enable_texture(const struct glft_pixmap_font* thetex)
{
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA8,
			glft_pixmap_font_get_width(thetex),
			glft_pixmap_font_get_height(thetex),
			0, GL_ALPHA, GL_UNSIGNED_BYTE,
			glft_pixmap_font_get_pixmap(thetex));

  glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
#ifndef PRECOMPUTED_TEXCOORD
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glScalef(1./glft_pixmap_font_get_width(thetex), 1./glft_pixmap_font_get_height(thetex),1);
#endif
}

void
glft_pixmap_font_render_texture(const struct glft_pixmap_font* tex)
{
  glDrawPixels(tex->width, tex->height, GL_ALPHA, GL_UNSIGNED_BYTE, tex->pixmap);
}

void
glft_pixmap_font_render(struct glft_pixmap_font* thetex, const char* thestring)
{
#ifdef PRECOMPUTED_TEXCOORD
  struct texture_glyph_info_double* info;
#else
  struct texture_glyph_info* info;
#endif
  int xadvance = 0;

  glBegin(GL_QUADS);
  while(*thestring) {
    unsigned char thechar = *thestring;
    int x;
    float w,h;

    info = (thetex->glyph_infos[thechar]);
    /*    fprintf(stderr,"%d %c %d\n", thechar, thechar, info);*/
    if(!info) {
      ++thestring;
      continue;
    }
    x = xadvance + info->bearing_x;
    /*        
		fprintf(stderr, "%c %p %d %d %d %d - %d\n", *thestring, info,
		info->x1,info->y1, info->x2,info->y2,
		info->bearing_y);
    */
    /*fprintf(stderr, "%c %f ", *thestring, x);*/

    w = info->x2-info->x1;
    h = info->y2-info->y1;

#ifdef PRECOMPUTED_TEXCOORD
    w *= thetex->width;
    h *= thetex->height;
#endif

    glTexCoord2f(info->x2,info->y2);	glVertex2f(x+w, info->bearing_y-h);
    glTexCoord2f(info->x2,info->y1);	glVertex2f(x+w, info->bearing_y);
    glTexCoord2f(info->x1,info->y1);	glVertex2f(x, info->bearing_y);
    glTexCoord2f(info->x1,info->y2);	glVertex2f(x, info->bearing_y-h);

    xadvance += info->xadvance;

    ++thestring;
  }
  glEnd();
  glTranslated(xadvance,0,0);
  /*fprintf(stderr, "\n");*/
}


void
glft_pixmap_font_render_with_other_advance(struct glft_pixmap_font* thetex, struct glft_pixmap_font* theOtherAdvance, double scale, const char* thestring)
{
#ifdef PRECOMPUTED_TEXCOORD
  struct texture_glyph_info_double* info;
#else
  struct texture_glyph_info* info;
#endif
  float xadvanceacc = 0;

  glBegin(GL_QUADS);
  while(*thestring) {
    unsigned char thechar = *thestring;
    int x;
    int texxadvance;
    int otherxadvance;
    float xadvance;
    float w,h;

    info = (thetex->glyph_infos[thechar]);
    /*    fprintf(stderr,"%d %c %d\n", thechar, thechar, info);*/
    if(!info) {
      ++thestring;
      continue;
    }

    x = (int)rint(xadvanceacc+info->bearing_x);
    /*    
		fprintf(stderr, "%c %p %d %d %d %d - %d\n", *thestring, info,
		info->x1,info->y1, info->x2,info->y2,
		info->bearing_y);
    */

    w = info->x2-info->x1;
    h = info->y2-info->y1;

#ifdef PRECOMPUTED_TEXCOORD
    w *= thetex->width;
    h *= thetex->height;
#endif

    glTexCoord2f(info->x2,info->y2);	glVertex2f(x+w, info->bearing_y-h);
    glTexCoord2f(info->x2,info->y1);	glVertex2f(x+w, info->bearing_y);
    glTexCoord2f(info->x1,info->y1);	glVertex2f(x, info->bearing_y);
    glTexCoord2f(info->x1,info->y2);	glVertex2f(x, info->bearing_y-h);


    texxadvance = info->xadvance;
    otherxadvance =  (theOtherAdvance->glyph_infos[thechar])->xadvance;
    /*xadvance = texxadvance*(1-scale) + otherxadvance*scale;*/
    xadvance = otherxadvance * scale;

    xadvanceacc += xadvance;
    /*fprintf(stderr, "%c %d ", *thestring, otherxadvance);*/

    ++thestring;
  }
  glEnd();
  glTranslated(xadvanceacc,0,0);
  /*fprintf(stderr, "%f\n",xadvanceacc);*/
}


int
glft_pixmap_font_get_xadvance(struct glft_pixmap_font* thetex, const char* thestring)
{
#ifdef PRECOMPUTED_TEXCOORD
  struct texture_glyph_info_double* info;
#else
  struct texture_glyph_info* info;
#endif
  int xadvance = 0;

  while(*thestring) {
    unsigned char thechar = *thestring;
    info = (thetex->glyph_infos[thechar]);
    if(info)
      xadvance += info->xadvance;
    ++thestring;
  }
  return xadvance;
}
#if 0
int
glft_pixmap_font_get_xadvance_char(struct glft_pixmap_font* thetex, char c);
{
  struct texture_glyph_info* info;
  unsigned char thechar = c;
  info = (thetex->glyph_infos[thechar]);
  return info->xadvance;
}
#endif

void
glft_pixmap_font_bounding_box(struct glft_pixmap_font* thetex, const char* thestring, int* x1, int* y1, int* x2, int* y2)
{
#ifdef PRECOMPUTED_TEXCOORD
  struct texture_glyph_info_double* info;
#else
  struct texture_glyph_info* info;
#endif
  *x1=*y1=*x2=*y2=0;
	int xadvanceacc=0;
	unsigned char thechar;
  int unset=1;
  while(*thestring) {
    thechar = *thestring;
    info = (thetex->glyph_infos[thechar]);
    if(!info) {
      ++thestring;
      continue;
    }
#ifdef PRECOMPUTED_TEXCOORD
    int ny1 = (int) (info->y1 * thetex->height - (info->y2 * thetex->height - info->bearing_y));
#else
    int ny1 = info->y1 - (info->y2 - info->bearing_y);
#endif

    int ny2 = info->bearing_y;
    //fprintf(stderr, "%d %d\n", ny1, ny2);
    if(unset) {
			*x1=info->bearing_x;
      *y1=ny1;
      *y2=ny2;
      unset=0;
    } else {
      if(ny1<*y1)
	   *y1=ny1;
      if(ny2>*y2)
	   *y2=ny2;
    }

    xadvanceacc += info->xadvance;
    ++thestring;
  }
	if(info) {
		*x2=xadvanceacc-info->xadvance+info->xmax;
		/*fprintf(stderr, "%c %d %d\n", thechar, info->xadvance, info->xmax);*/
	}
	
}

void
glft_pixmap_font_bounding_box_with_other_advance(struct glft_pixmap_font* thetex, struct glft_pixmap_font* theOtherAdvance, double scale, const char* thestring, int* x1, int* y1, int* x2, int* y2)
{
#ifdef PRECOMPUTED_TEXCOORD
  struct texture_glyph_info_double* info;
#else
  struct texture_glyph_info* info;
#endif
  *x1=*y1=*x2=*y2=0;
  int unset=1;
  float xadvanceacc=0;
	float xadvance;

  while(*thestring) {
    unsigned char thechar = *thestring;
    int texxadvance;
    int otherxadvance;

    info = (thetex->glyph_infos[thechar]);

    if(!info) {
      ++thestring;
      continue;
    }
#ifdef PRECOMPUTED_TEXCOORD
    int ny1 = (int) (info->y1 * thetex->height - (info->y2 * thetex->height - info->bearing_y));
#else
    int ny1 = info->y1 - (info->y2 - info->bearing_y);
#endif

    int ny2 = info->bearing_y;
    //fprintf(stderr, "%d %d\n", ny1, ny2);
    if(unset) {
			*x1=info->bearing_x;
      *y1=ny1;
      *y2=ny2;
      unset=0;
    } else {
      if(ny1<*y1)
	   *y1=ny1;
      if(ny2>*y2)
	   *y2=ny2;
    }

    texxadvance = info->xadvance;
    otherxadvance =  (theOtherAdvance->glyph_infos[thechar])->xadvance;
    xadvance = otherxadvance*scale;
    xadvanceacc += xadvance;

    ++thestring;
  }
	if(info) {
		*x2=(int)rint(xadvanceacc-xadvance+info->xmax);
		/* *x2 = (int)rint(xadvanceacc); */
	} else	
		*x2 = (int)rint(xadvanceacc);

}

/* ------------------------------------------------------------------------ */

#if 0
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

void
glft_pixmap_font_save_pixmap_pgm(const struct glft_pixmap_font* thetex, FILE* file)
{
#if 0
    fprintf(file, "P6\n%d %d\n255\n", thetex->width, thetex->height) ;

    unsigned char *ptr = thetex->pixmap ;
    int nbpixels = thetex->width * thetex->height;
    for (int pixel=0; pixel<nbpixels; ++pixel) { 
	 fwrite(ptr, 1, 1, file) ;
	 fwrite(ptr, 1, 1, file) ;
	 fwrite(ptr, 1, 1, file) ;
	 ++ptr ;
    }
#else
  fprintf(file, "P5 %d %d 255", thetex->width, thetex->height);
  putc('\n', file);
  size_t len = thetex->width * thetex->height;
  if (fwrite(thetex->pixmap, 1, len, file) != len) {
    DBG;
  }
#endif
}

int
glft_pixmap_font_load_pixmap_pgm(struct glft_pixmap_font* thetex, FILE* file)
{
  char P5[] = {0,0,0};
  int width, height, maxvalue;
  int res;
  res = fscanf(file, "%2s %d %d %d", P5, &width, &height, &maxvalue);
  if(res!=4) {
    fprintf(stderr, "bad pixmap file %s %d\n", __FILE__, __LINE__);
    return 1;
  }
  fgetc(file);			// skip end of line
  if(thetex->pixmap) {
    free(thetex->pixmap);
  }

  size_t len = width*height;
  thetex->pixmap = (unsigned char*)malloc(len);
  if(!thetex->pixmap) {
    DBG;
    return 1;
  }

  if (fread(thetex->pixmap, 1, len, file) != len) {
    DBG;
    return 1;
  }
  thetex->width=width;
  thetex->height=height;

  return 0;
}

#ifdef __use_gzip__

void
glft_pixmap_font_save_pixmap_pgm_gzip(const struct glft_pixmap_font* thetex, FILE* file)
{
  gzFile f = gzdopen(fileno(file), "wb");
  if(!f) {
    DBG;
    return;
  }
  if(!gzprintf(f, "P5 %d %d 255\n", thetex->width, thetex->height))
    DBG;
  if(gzwrite(f, thetex->pixmap, thetex->width * thetex->height) != thetex->width * thetex->height)
    DBG;
  if(gzclose(f)) {
    DBG;
  }
}

int
glft_pixmap_font_load_pixmap_pgm_gzip(struct glft_pixmap_font* thetex, FILE* file)
{
  char P5[] = {0,0,0};
  int width, height, maxvalue;
  int res;
  gzFile gzf = gzdopen(fileno(file), "rb");
  char header[256];
  
  if(!gzgets(gzf, header, 256)) {
    return 1;
  }
  
  res = sscanf(header, "%2s %d %d %d\n", P5, &width, &height, &maxvalue);
  if(res!=4) {
    return 1;
  }

  if(thetex->pixmap) {
    free(thetex->pixmap);
  }

  thetex->pixmap = (unsigned char*)malloc(width*height);
  if(!thetex->pixmap) {
    DBG;
    return 1;
  }

  gzread(gzf, thetex->pixmap, width*height);
  thetex->width=width;
  thetex->height=height;

  return 0;
}

#endif // __use_gzip__

#ifdef __use_bzip2__

void
glft_pixmap_font_save_pixmap_pgm_bzip2(const struct glft_pixmap_font* thetex, FILE* file)
{
  int err,len;
  char header[256];
  BZFILE* f = bzWriteOpen(&err, file, /*blocksize*/ 9, /*verbosity*/0, /*workFactor*/0);
  if(err!=BZ_OK) {
    DBG;
    bzWriteClose (&err, f, 0, 0, 0);
    return;
  }
  
  len = snprintf(header, 256, "P5 %d %d 255\n", thetex->width, thetex->height);
  if(len==-1) {
    DBG;
    bzWriteClose (&err, f, 0, 0, 0);
    return;
  }

  bzWrite(&err, f, header, len);
  if(err!=BZ_OK) {
    DBG;
    bzWriteClose (&err, f, 0, 0, 0);
    return;
  }

  len = thetex->width * thetex->height;
  bzWrite(&err, f, thetex->pixmap, len);
  if(err!=BZ_OK) {
    DBG;
    bzWriteClose (&err, f, 0, 0, 0);
    return;
  }

  bzWriteClose (&err, f, 0, 0, 0);
}

int
glft_pixmap_font_load_pixmap_pgm_bzip2(struct glft_pixmap_font* thetex, FILE* file)
{
  char P5[] = {0,0,0};
  int width, height, maxvalue;
  int res;
  char header[256];
  int err;

  BZFILE* bzf = bzReadOpen(&err, file, 0, 0, 0, 0);
  if(err!=BZ_OK) {
    DBG;
    bzReadClose (&err, bzf);
    return 1;
  }
  
  {
    int i=0;
    char* c=header;
    *c=0;
    for(i=0; i<256; ++i, ++c) {
      bzRead(&err, bzf, c, 1);
      if(*c=='\n')
	   break;
      if(err!=BZ_OK) {
	   DBG;
	   bzReadClose (&err, bzf);
	   return 1;
      }
    }
    if(*c!='\n') {
      DBG;
      bzReadClose (&err, bzf);
      return 1;
    }
  }
  
  res = sscanf(header, "%2s %d %d %d\n", P5, &width, &height, &maxvalue);
  if(res!=4) {
    fprintf(stderr, header);
    DBG;
    return 1;
  }

  if(thetex->pixmap) {
    free(thetex->pixmap);
  }

  thetex->pixmap = (unsigned char*)malloc(width*height);
  if(!thetex->pixmap) {
    DBG;
    bzReadClose (&err, bzf);
    return 1;
  }

  bzRead(&err, bzf, thetex->pixmap, width*height);
  if(err!=BZ_OK && err!=BZ_STREAM_END) {
    DBG;
    bzReadClose (&err, bzf);
    return 1;
  }

  thetex->width=width;
  thetex->height=height;

  return 0;
}

#endif // __use_bzip2__

#ifdef __use_png__

#if 0
void
png_user_error_fn(void)
{
}

void
png_user_warning_fn(void)
{
}
#endif

void
glft_pixmap_font_save_pixmap_png(const struct glft_pixmap_font* thetex, FILE* file)
{
  png_infop info_ptr;
  png_structp png_ptr = png_create_write_struct
    (PNG_LIBPNG_VER_STRING, 0,
     0, 0);
  
  if (!png_ptr) {
    DBG;
    return;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr,
					    (png_infopp)NULL);
    return;
  }

  if (setjmp(png_ptr->jmpbuf)) {
    DBG;
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(file);
    return;
  }
  png_init_io(png_ptr, file);
  png_set_IHDR(png_ptr, info_ptr, thetex->width, thetex->height,
			8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
#if 0
  {
    png_text text_prt[] = {
      {"Title", "glft font"},
      {"Author", "glft"},
      {"Description",""},
      {"Copyright", "2000 (c) Stephane Conversy"}
    }

    png_set_text(png_ptr, info_ptr, text_ptr, 4);
  }
#endif
  png_write_info(png_ptr, info_ptr);
  {
    int i;
    png_byte *row_pointers[thetex->height];
    for(i=0; i<thetex->height; ++i) {
      row_pointers[i] = (png_byte*) &thetex->pixmap[i*thetex->width];
    }
    //    png_write_image(png_ptr, thetex->pixmap);
    png_write_image(png_ptr, row_pointers);
  }
  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);
}

int
glft_pixmap_font_load_pixmap_png(struct glft_pixmap_font* thetex, FILE* file)
{
  png_structp png_ptr;
  png_infop info_ptr;
  png_infop end_info;

#if 1
  {
    png_byte header[9];
    fread(header, 1, 8, file);
#if 0
    header[8]=0;
    fprintf(stderr, "%s\n", header);
#endif
    if(png_sig_cmp(header, 0, 8)) {
      DBG
	   return 1;
    }
  }
#endif
  png_ptr = png_create_read_struct
    (PNG_LIBPNG_VER_STRING, 0,
     0, 0);
  if (!png_ptr) {
    DBG;
    return 1;
  }
  
  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    DBG;
    png_destroy_read_struct(&png_ptr,
					   (png_infopp)NULL, (png_infopp)NULL);
    return 1;
  }
  
  end_info = png_create_info_struct(png_ptr);
  if (!end_info) {
    DBG;
    png_destroy_read_struct(&png_ptr, &info_ptr,
					   (png_infopp)NULL);
    return 1;
  }

  if (setjmp(png_ptr->jmpbuf)) {
    DBG;
    png_destroy_read_struct(&png_ptr, &info_ptr,
					   &end_info);
    return 1;
  }

  png_init_io(png_ptr, file);
  png_set_sig_bytes(png_ptr, 8);
  png_read_info(png_ptr, info_ptr);

  thetex->width = png_get_image_width(png_ptr, info_ptr);
  thetex->height = png_get_image_height(png_ptr, info_ptr);
  /*  fprintf(stderr, "%d %d \n", thetex->width, thetex->height);*/

  if(
     png_get_bit_depth(png_ptr, info_ptr)!=8 ||
     png_get_color_type(png_ptr, info_ptr)!=PNG_COLOR_TYPE_GRAY
     ) {
    DBG;
    png_destroy_read_struct(&png_ptr, &info_ptr,
					   &end_info);
    return 1;
  }

  if(thetex->pixmap)
    free(thetex->pixmap);

  thetex->pixmap = (unsigned char*)malloc(thetex->width*thetex->height);

  {
    int i;
    for(i=0; i<thetex->height;++i) {
      png_read_row(png_ptr, &thetex->pixmap[i*thetex->width], 0);
    }
  }

  png_read_end(png_ptr, end_info);
  png_destroy_read_struct(&png_ptr, &info_ptr,
					 &end_info);

  return 0;
}

#endif


void
glft_pixmap_font_save_metrics(const struct glft_pixmap_font* thetex, FILE* file)
{
  /*
    we save 256 metrics even if there are some which are not valid
    in order to discrimate them when reloading, the x1 field is set to -1
  */
  coord_t i=1;
  //  size_t res;
  struct texture_glyph_info foo;
  foo.x1=-1;
  /* save size of coord_t and size (in decimal) of the struct because of possible alignement differences */
  /*res = */fprintf(file, "glft %d %d\n", (int)sizeof(coord_t), (int)sizeof(struct texture_glyph_info));
  /* save one to remember the endianess of the following binary data */
  fwrite((const void*)&i, sizeof(coord_t), 1, file);

  for (i=0; i<256;++i) {

    if(thetex->glyph_infos[i]) {
#ifdef PRECOMPUTED_TEXCOORD
      struct texture_glyph_info info;
      struct texture_glyph_info_double info_double;
      info_double = *(thetex->glyph_infos[i]);
      info.xadvance = info_double.xadvance;
      info.bearing_x = info_double.bearing_x;
      info.bearing_y = info_double.bearing_y;
      info.x1 = (coord_t) (info_double.x1 * thetex->width);
      info.x2 = (coord_t) (info_double.x2 * thetex->width);
      info.y1 = (coord_t) (info_double.y1 * thetex->height);
      info.y2 = (coord_t) (info_double.y2 * thetex->height);

      fwrite(&info, sizeof(struct texture_glyph_info), 1, file);
#else
      fwrite(thetex->glyph_infos[i], sizeof(struct texture_glyph_info), 1, file);
#endif
    }
    else
      fwrite(&foo, sizeof(struct texture_glyph_info), 1, file);
  }
}


int
glft_pixmap_font_load_metrics(struct glft_pixmap_font* thetex, FILE* file)
{
  char glft[]={0,0,0,0,0};
  int coordsize, structsize;
  coord_t one;
  size_t res;

  res = fscanf(file, "%4s %d %d\n", glft, &coordsize, &structsize);
  if(res!=3) {
    fprintf(stderr, "bad metrics file %s %d\n", __FILE__, __LINE__);
    return 1;
  }
  res = fread((void*)&one, sizeof(coord_t), 1, file);
  if(res!=1) {
    fprintf(stderr, "bad metrics file %s %d\n", __FILE__, __LINE__);
    return 1;
  }

  /* if it's the same endianess, the the same alignment */
  if(coordsize==sizeof(coord_t) && structsize==sizeof(struct texture_glyph_info)  && one==1) {
    int i;
    struct texture_glyph_info infos[256];
    res = fread(infos, sizeof(struct texture_glyph_info), 256, file);
    if (res!=256) {
      fprintf(stderr, "bad metrics file %s %d\n", __FILE__, __LINE__);
      return 1;
    }
    for(i=0; i<256; ++i) {
      if(thetex->glyph_infos[i]) {
	   free(thetex->glyph_infos[i]);
	   thetex->glyph_infos[i]=0;
      }
      if(infos[i].x1!=-1) {
#ifdef PRECOMPUTED_TEXCOORD
	   thetex->glyph_infos[i] = (struct texture_glyph_info_double*) malloc(sizeof(struct texture_glyph_info_double));

	   struct texture_glyph_info info = infos[i];
	   struct texture_glyph_info_double info_double;
	   info_double.xadvance = info.xadvance;
	   info_double.bearing_x = info.bearing_x;
	   info_double.bearing_y = info.bearing_y;
		 info_double.xmax = info.xmax;
	   info_double.x1 = (coord_t) (info.x1 / (double)thetex->width);
	   info_double.x2 = (coord_t) (info.x2 / (double)thetex->width);
	   info_double.y1 = (coord_t) (info.y1 / (double)thetex->height);
	   info_double.y2 = (coord_t) (info.y2 / (double)thetex->height);

	   *(thetex->glyph_infos[i]) = info_double;
#else
	   thetex->glyph_infos[i] = (struct texture_glyph_info*) malloc(sizeof(struct texture_glyph_info));
	   *(thetex->glyph_infos[i]) = infos[i];
#endif
      }

    }
    return 0;
  }

  /* check the endianness */
  if(one!=1) {
    coord_t one2;
    char *cone = (char*)&one;
    char *cone2 = (char*)&one2;

    unsigned int i;
    for (i=0; i<sizeof(coord_t); ++i) {
      cone2[i]=cone[sizeof(coord_t)-i];
    }
    if (one2!=1) {
      fprintf(stderr, "bad metrics file %s %d\n", __FILE__, __LINE__);
      return 1;
    }
  }

  if(coordsize==sizeof(coord_t) && structsize==sizeof(struct texture_glyph_info)) {
    unsigned int i,j;
    struct texture_glyph_info infos[256];
    res = fread(infos, sizeof(struct texture_glyph_info), 256, file);
    for(j=0; j<sizeof(struct texture_glyph_info) * sizeof(coord_t); ++j) {
      coord_t tmp;
      char *ctmp=(char*)&tmp, *csrc = (char*)&((coord_t*)infos)[j];
      /* swap all coords */
      for (i=0; i<sizeof(coord_t); ++i) {
	   ctmp[i]=csrc[sizeof(coord_t)-i];
      }
      ((coord_t*)infos)[j] = tmp;
    }
    return 0;
  }

  /* not the same alignment */
  /* TODO */

  fprintf(stderr, "bad metrics file %s %d\n", __FILE__, __LINE__);
  return 1;
}
