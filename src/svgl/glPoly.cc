#if defined (__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <glPoly.h>

#include <iostream>

#if 0
void
glPoly::updateGlBuffers()
{
  if(contours.empty())
    return;
  GLint buffers[contours.size()];
  glGenBuffers(contours.size, buffers);
  int i=0;
  for(contour_type::iterator it=contours.begin(); it!=contours.end(); ++it, ++i) {
    it->numBuffer = buffers[i];
    glBindBuffer(GL_ARRAY_BUFFER, it->numBuffer);
    glBufferData(GL_ARRAY_BUFFER, it->len*3*sizeof(GLfloat), &points[beg], GL_STATIC_DRAW);
  }
}
#endif



static
void  vertexCallback(GLdouble * v, glPoly * gglPoly)
{
  //glVertex2dv(v);
  glPoly::primitive_info& info = gglPoly->contours[gglPoly->contours.size()-1];

  info.points.push_back(v[0]);
  info.points.push_back(v[1]);
  info.points.push_back(0);
#if DBG_CALLBACK
  //fprintf(stderr, "glVertex %f %f %s:%d\n", v[0], v[1], __FILE__, __LINE__);
#endif
}

static
void  beginCallback(GLenum which, glPoly * gglPoly)
{
  //glBegin(which);
  //std::cerr << "primitive " << which << std::endl;
  glPoly::primitive_info info;
  info.draw_primitive = which;
  //info.beg = gglPoly->points.size();
  gglPoly->contours.push_back(info);
}

static
void  endCallback(glPoly * gglPoly)
{
  //glPoly::primitive_info& info = gglPoly->contours[gglPoly->contours.size()-1];
  //info.len = gglPoly->points.size()-info.beg;
  //std::cerr << "beg: " << info.beg << " len: " << info.len << std::endl;
  //glEnd();
}

static void 
tessError(GLenum err)
{
  std::cerr << gluErrorString(err) << std::endl;
}


// IT DOESN'T WORK SINCE WE SIMPLY CANT REALLOCATE !!!
int combineTab_index = 0;	// re-initialised each time glft_glyph_render_plain is called.
#define TAB_INCR 8192

static
GLvoid
tessCombine(GLdouble coords[3],
						GLdouble* vertex_data[4],
						GLfloat weight[4],
						GLdouble **dataOut )
{
  static int combineTab_size = 0;
  static GLdouble* combineTab = 0;
  int i;

#if DBG_CALLBACK
  fprintf(stderr,"combine callback combineTab_index:%d combineTab_size:%d %s:%d\n",combineTab_index, combineTab_size,__FILE__,__LINE__);
#endif

  /* adjust the size of the combine array */
  if ( (combineTab_index+6) >= combineTab_size) {
	  combineTab_size += TAB_INCR;
		//fprintf(stderr, "size = %d  %s:%d\n", combineTab_size, __FILE__, __LINE__);
	  combineTab = (GLdouble*) realloc (combineTab, combineTab_size * sizeof (double));
  }

  *dataOut = combineTab + combineTab_index;
  combineTab_index += 6;
  for(i=0;i<3;++i)
    (*dataOut)[i] = coords[i];
  for(i=3; i<6; ++i) {
		(*dataOut)[i] = 0;
		for(int j=0; j<4; ++j)
			if(weight[j])
				(*dataOut)[i] += weight[j] * vertex_data[j][i] ;
	}
}

void
glPoly::setTessCallbacks(GLUtesselator * gluTess)
{
  typedef GLvoid (*fn_type)(...);
#if 1
  gluTessCallback(gluTess, GLU_TESS_BEGIN_DATA, reinterpret_cast<fn_type>(beginCallback) );
  gluTessCallback(gluTess, GLU_TESS_END_DATA, reinterpret_cast<fn_type>(endCallback));
  gluTessCallback(gluTess, GLU_TESS_VERTEX_DATA, reinterpret_cast<fn_type>(vertexCallback));
#else
  gluTessCallback(gluTess, GLU_TESS_BEGIN, reinterpret_cast<fn_type>(glBegin) );
  gluTessCallback(gluTess, GLU_TESS_END, reinterpret_cast<fn_type>(glEnd));
  gluTessCallback(gluTess, GLU_TESS_VERTEX, reinterpret_cast<fn_type>(glVertex2dv));
#endif
  gluTessCallback(gluTess, GLU_TESS_ERROR, reinterpret_cast<fn_type>(tessError));
  gluTessCallback(gluTess, GLU_TESS_COMBINE, reinterpret_cast<fn_type>(tessCombine));
  //gluTessProperty(gluTess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
}


void
draw(const glPoly& poly)
{
  //glPoly::point_list_type::size_type beg, end;
  //beg=0;
  for(glPoly::contour_type::size_type i=0; i<poly.contours.size(); ++i) {
    //std::cerr << "i " << i << std::endl; 
    const glPoly::primitive_info& info = poly.contours[i];
    //end = beg+info.len;
#if 0
    glBegin(info.draw_primitive);
    for(; beg<end; ++beg) glVertex2d(poly.points[beg++], poly.points[beg++]);
    glEnd();
#endif
#if 1
    glInterleavedArrays(GL_V3F, 0, &info.points[0]);
    glDrawArrays(info.draw_primitive, 0, info.points.size()/3); //glMultiDrawArrays
    //beg = end;
#endif
#if 0
    glBindBuffers(GL_ARRAY_BUFFER, poly.numBuffer);
    glDrawArrays(info.draw_primitive, 0, info.len/3);
    //beg = end;
#endif

  }
}
