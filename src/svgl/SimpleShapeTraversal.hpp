#ifndef __svgl_SimpleShapeTraversal__
#define __svgl_SimpleShapeTraversal__

#include <w3c/svg/SVGTransformable.hpp>
#include <w3c/svg/SVGStylable.hpp>

namespace svgl {

  template <class SimpleShape>
  void
  glTraverseSimpleShape(SimpleShape * shape, svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    bool reallyTraverse = shape->preManageGlCompilation();
    if(!reallyTraverse)
      return;

    glPushMatrix();
    shape->svg::SVGTransformable::glTraverse(svglContext, glinfo);
    shape->svg::SVGStylable::glTraverse(svglContext, glinfo, false, false);
    glPopMatrix();
    shape->postManageGlCompilation();
  }

  template <class SimpleShape>
  void
  pickTraverseSimpleShape(SimpleShape * shape, svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    glPushMatrix();
    shape->svg::SVGTransformable::glTraverse(svglContext, glinfo);
    shape->svg::SVGStylable::pickTraverse(svglContext, glinfo);
    glPopMatrix();
  }

  template <class SimpleShape>
  void
  glTraverseBoundingBoxSimpleShape(SimpleShape * shape, svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    glPushMatrix();
    shape->svg::SVGTransformable::glTraverse(svglContext, glinfo);
    shape->svg::SVGStylable::glTraverseBoundingBox(svglContext, glinfo);
    glPopMatrix();
  }

#define SIMPLE_TRAVERSE(shape) 										\
  void 													\
  SVG##shape##Element::glTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo) 			\
  { 													\
    svgl::glTraverseSimpleShape(this, svglContext, glinfo); 						\
  } 													\
													\
  void 													\
  SVG##shape##Element::pickTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo) 			\
  { 													\
    svgl::pickTraverseSimpleShape(this, svglContext, glinfo); 						\
  } 													\
													\
  void 													\
  SVG##shape##Element::glTraverseBoundingBox(svgl::Context * svglContext, svgl::GLInfo* glinfo) 	\
  {													\
    svgl::glTraverseBoundingBoxSimpleShape(this, svglContext, glinfo);					\
  }													\
													\
  bool													\
  SVG##shape##Element::canBeCompiled()									\
  {													\
    return true;											\
  }

}

#endif // __svgl_SimpleShapeTraversal__
