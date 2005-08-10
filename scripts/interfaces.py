class interface:
    def __init__(self):
        self.exclude_methods = []
        self.exclude_attributes = []
        self.include_methods = []
        self.include_attributes = []
        self.include_includes = []
        self.include_fwd_decls = []
        self.user_defined_constructor=0
        self.user_defined_destructor=0



interfaces={}

#SVGElement
inter = interface()
interfaces["SVGElement"]=inter
str='''
   public:
      virtual const String getDtdElement() const=0;
      virtual void setOwnerAndViewPort(SVGSVGElement* owner, SVGElement* viewport);
      virtual void print(xmlostream&);
      virtual void updateStyle(SVGStylable* parent);
      virtual void glTraverseChildren(svgl::Context*, svgl::GLInfo*);
      virtual void pickTraverseChildren(svgl::Context*, svgl::GLInfo*);
      virtual void glTraverseBoundingBoxChildren(svgl::Context*, svgl::GLInfo*);
      virtual void animationTraverseChildren(svgl::AnimationInfo*);
      void getPercentageWidthHeight(svgl::GLInfo*, float *percentage_width, float *percentage_height);
      SVGElement(dom::Document* doc, unicode::String* tag);
      ~SVGElement();
      typedef std::vector<svg::SVGElement*> Branch;
      virtual void glTraverseBranch(svgl::Context*, svgl::GLInfo*, Branch::iterator& beg, Branch::iterator& end);

      virtual dom::Node * insertBefore(dom::Node * newChild, dom::Node * refChild) throw(dom::DOMException);
      virtual void raise();
      virtual void lower();
      virtual void above(SVGElement*) throw(dom::DOMException);
      virtual void below(SVGElement*) throw(dom::DOMException);

	void setId(const char*);
	const char *getIdcc();


	virtual dom::Node *	internalInsertBefore(dom::Node * newChild, dom::Node * refChild) throw(dom::DOMException);
	virtual dom::Node *	removeChild(dom::Node * oldChild) throw(dom::DOMException);
	virtual dom::Node *	internalRemoveChild(dom::Node * oldChild) throw(dom::DOMException);
	virtual dom::Node *	replaceChild(dom::Node * newChild, dom::Node * oldChild) throw(dom::DOMException);

	// compilation stuff

	void setDamaged(bool);
	bool getDamaged() const;
	void setStyleDamaged(bool);
	void compile(bool);
	void compileAsATexture(bool yesno);

	virtual bool areChildrenDamaged();
        virtual void damageChildren();

	virtual void registerUse(svgl::Context*);

      protected:
          bool _askForCompileGlTraverse;
	  unsigned int _glList;
	  bool _inGlList;
	bool _damaged;
	svgl::TextureSnapshot *_snap;
	bool _compileAsATexture;
	  virtual bool glCompiled();
	  bool preManageGlCompilation();
	  void postManageGlCompilation();

	  bool _childrenDamaged;
	  bool _aChildIsNotDamagedAnymore;
	  void setChildrenDamaged(bool);
	  void setAChildIsNotDamagedAnymore();
	  bool _canBeCompiled;
	  virtual bool canBeCompiled();
	  void setCanBeCompiled(bool);
	  void setGlCompiled(bool);

	  template <class SimpleShape> friend void svgl::glTraverseSimpleShape(SimpleShape * shape, svgl::Context * svglContext, svgl::GLInfo* glinfo);
	  template <class SimpleShape> friend void svgl::pickTraverseSimpleShape(SimpleShape * shape, svgl::Context * svglContext, svgl::GLInfo* glinfo);
	  template <class SimpleShape> friend void svgl::glTraverseBoundingBoxSimpleShape(SimpleShape * shape, svgl::Context * svglContext, svgl::GLInfo* glinfo);

      public:


'''

inter.user_defined_constructor=1
inter.user_defined_destructor=1


inter.include_attributes.append(str)
inter.include_fwd_decls=["::DOM_Node", "::xmlostream", "SVGSVGElement", "svgl::Context", "svgl::GLInfo", "SVGStylable", "svgl::AnimationInfo", "svgl::TextureSnapshot"]
# glTraverseBranch
inter.include_includes = ["vector","svgl/SimpleShapeTraversal.hpp"]

# SVGSVGElement
inter = interface()
interfaces["SVGSVGElement"]=inter
inter.exclude_methods=["getElementById"]

str='''
\tprivate:
\t\ttypedef std::map<DOMString, SVGElement*> ElementByIDContainer;
\t\tElementByIDContainer _elements_by_id;

\tpublic:
\t\tvoid setElementById(const DOMString& id, SVGElement* e);
\t\tvoid setOwnerAndViewPort(SVGSVGElement* owner, SVGElement* viewport);
\t\tvoid glTraverseFromUse(svgl::Context*, svgl::GLInfo* glinfo, float width, float height);
\t\tvoid pickTraverseFromUse(svgl::Context*, svgl::GLInfo* glinfo, float width, float height);
'''
inter.include_attributes.append(str)
inter.include_includes=["map"]
inter.include_fwd_decls=["::DOM_Node","SVGSVGElement"]

# SVGElementInstance
inter = interface()
interfaces["SVGElementInstance"]=inter

inter.include_attributes.append('''
public:
		SVGElementInstance() {}
''')
inter.user_defined_constructor=1

# SVGSymbolElement

inter = interface()
interfaces["SVGSymbolElement"]=inter

inter.include_attributes.append('''
public:
\t\tvoid setOwnerAndViewPort(SVGSVGElement* owner, SVGElement* viewport);
\tpublic:
\t\tvoid glTraverseFromUse(svgl::Context*, svgl::GLInfo* glinfo, float width, float height);
\t\tvoid pickTraverseFromUse(svgl::Context*, svgl::GLInfo* glinfo, float width, float height);
''')

inter.include_fwd_decls=["::DOM_Node","SVGSVGElement"]

# SVGMarkerElement

inter = interface()
interfaces["SVGMarkerElement"]=inter

inter.include_attributes.append('''
public:
\t\tvoid setOwnerAndViewPort(SVGSVGElement* owner, SVGElement* viewport);
\tpublic:
\t\tvoid glTraverseFromUse(svgl::Context*, svgl::GLInfo* glinfo, float shapeStrokeWidth, float shapeOrient);
\t\tvoid pickTraverseFromUse(svgl::Context*, svgl::GLInfo* glinfo, float shapeStrokeWidth, float shapeOrient);
''')

inter.include_fwd_decls=["::DOM_Node","SVGSVGElement"]


# SVGLength
inter = interface()
interfaces["SVGLength"]=inter

str='''
\tprivate:
bool _computed;
\tpublic:
\t\tSVGLength() : unitType(SVG_LENGTHTYPE_UNKNOWN), value(0), valueInSpecifiedUnits(0), _computed(false) {}
\t\tSVGLength(float v) : unitType(SVG_LENGTHTYPE_NUMBER), value(v), valueInSpecifiedUnits(v), _computed(true) {}
\t\tfloat computeValue(float pdi=90., float percentage=1., float fontSize=1., float xHeight=1.);
\t\tbool isAbsolute() const;
'''
inter.include_attributes.append(str)
#inter.include_includes=["map"]


# SVGDocument
inter = interface()
interfaces["SVGDocument"]=inter

str='''
\tpublic:
\t\tSVGDocument();
\t\tdom::Element * createElement(unicode::String * tagName) throw(dom::DOMException);
\t\tdom::Attr * createAttribute(unicode::String * name) throw(dom::DOMException);
\t\tdom::Element * createElementNS(unicode::String * namespaceURI, unicode::String * qualifiedName) throw(dom::DOMException);
\t\tdom::Attr * createAttributeNS(unicode::String * namespaceURI, unicode::String * qualifiedName) throw(dom::DOMException);
\t\tdom::AttributeMap * createAttributeMap(dom::Element * e, dom::NamedNodeMapImpl* defaults);
\t\tdom::Node * cloneNode(bool deep) const;
\t\tdom::Node * insertBefore(dom::Node * newChild, dom::Node * refChild) throw(dom::DOMException);
\t\tdom::Node * removeChild(dom::Node * oldChild) throw(dom::DOMException);
\t\tdom::Node * replaceChild(dom::Node * newChild, dom::Node * oldChild) throw(dom::DOMException);


\t\tvirtual void print(xmlostream&);
\t\tvoid updateStyle();
'''
inter.include_attributes.append(str)
inter.include_fwd_decls=["::DOM_Node","::xmlostream", "SVGSVGElement","SVGElement"]
#inter.include_includes=["map"]
inter.user_defined_destructor=1



# SVGFilterElement

inter = interface()
interfaces["SVGFilterElement"]=inter

inter.include_attributes.append('''
\tprotected:
\t\tbool _nothingToDo;
\t\tfloat _x,_y,_width,_height;
\t\tsvgl::MultiPassManager _multiPassManager;
\tpublic:
\t\tvirtual void glPreTraverse(svgl::Context*, svgl::GLInfo*, SVGElement* elem);
\t\tvirtual void glPostTraverse(svgl::Context*, svgl::GLInfo*, SVGElement* elem);

''')

for fwd in ["svgl::Context", "svgl::GLInfo", "svg::SVGElement"]:
    if fwd not in inter.include_fwd_decls:
        inter.include_fwd_decls.append(fwd)


inter.include_includes=["svgl/MultiPassManager.hpp"]

# SVGFilterPrimitiveStandardAttributes

inter = interface()
interfaces["SVGFilterPrimitiveStandardAttributes"]=inter

inter.include_attributes.append('''
\t\tvirtual void glTraverse(svgl::Context*, svgl::GLInfo*, SVGElement* elem, svgl::FilterContext*, unicode::String* in1, unicode::String* result);

''')

for fwd in ["svgl::Context", "svgl::GLInfo", "svg::SVGElement", "svgl::FilterContext", "unicode::String"]:
    if fwd not in inter.include_fwd_decls:
        inter.include_fwd_decls.append(fwd)


# SVGLinearGradientElement
inter = interface()
interfaces["SVGLinearGradientElement"]=inter

inter.user_defined_constructor=1
inter.include_attributes.append('''
\t\tSVGLinearGradientElement(dom::Document *);
\t\tvoid glPreTraverse(svgl::Context*, svgl::GLInfo* glinfo);
\t\tvoid glPostTraverse(svgl::Context*, svgl::GLInfo* glinfo, float x, float y, float width, float height);
''')
inter.include_fwd_decls=["svgl::Context", "svgl::GLInfo"]

# SVGRadialGradientElement
inter = interface()
interfaces["SVGRadialGradientElement"]=inter

inter.user_defined_constructor=1
inter.include_attributes.append('''
\t\tSVGRadialGradientElement(dom::Document *);
\t\tvoid glPreTraverse(svgl::Context*, svgl::GLInfo* glinfo);
\t\tvoid glPostTraverse(svgl::Context*, svgl::GLInfo* glinfo, float x, float y, float width, float height);
''')
inter.include_fwd_decls=["svgl::Context", "svgl::GLInfo"]


# SVGGradientElement
inter = interface()
interfaces["SVGGradientElement"]=inter

inter.user_defined_constructor=1
inter.include_attributes.append('''
\t\tSVGGradientElement(dom::Document *, unicode::String *);
\t\tvirtual void glPreTraverse(svgl::Context*, svgl::GLInfo* glinfo) = 0;
\t\tvirtual void glPostTraverse(svgl::Context*, svgl::GLInfo* glinfo, float x, float y, float width, float height) = 0;
\t\tvoid getStopElements(svgl::StopElements*);
''')

inter.include_includes=["svgl/StopElements.hpp"]


# SVGStylable
inter = interface()
interfaces["SVGStylable"]=inter

inter.include_attributes.append('''
\t\tAttribute< css::CSSStyleDeclaration >& getStyle() { return style; }
		SVGClipPathElement* getSVGClipPathElement(svgl::ExternalEntityManager *, dom::Document *);
		SVGFilterElement* getSVGFilterElement(svgl::ExternalEntityManager *, dom::Document *);
		SVGMarkerElement* getSVGMarkerElement(svgl::ExternalEntityManager *, dom::Document *);
		SVGMarkerElement* getSVGMarkerStartElement(svgl::ExternalEntityManager *, dom::Document *);
		SVGMarkerElement* getSVGMarkerEndElement(svgl::ExternalEntityManager *, dom::Document *);
		SVGMarkerElement* getSVGMarkerMidElement(svgl::ExternalEntityManager *, dom::Document *);


\t\tvoid glTraverseMarker(svgl::Context* svglContext, svgl::GLInfo* glinfo, float shapeStrokeWidth);
\t\tvoid glTraverse(svgl::Context* svglContext, svgl::GLInfo* glinfo, bool stencilStrokeHint, bool traverseEvenIfNonVisible);

	virtual void computeGeometry(svgl::Context * svglContext, svgl::GLInfo* glinfo);
	virtual void computeGeometryForDrawing(svgl::Context * svglContext, svgl::GLInfo* glinfo);

private:
	  void glTraverseOpacity(svgl::Context * svglContext,
		 svgl::GLInfo* glinfo,
		 const css::Color * fill,
		 const css::Color * stroke,
		 float fillOpacity,
		 float strokeOpacity,
		 float opacity,
		 float strokeWidth,
                 bool stencilStrokeHint
				 );

	glPoly * fillGlPoly, * strokeGlPoly;

	  friend class SVGElement;
	  virtual void updateChildrenStyle();
                                 
public:

\t\tvirtual ~SVGStylable() {}

#define ATTRDEF(type, name, dtdstr)				\
   virtual css::CSSStyle::type get##name() const;		\
   bool isMy##name() const; /* if specified for this element */	\
   void unsetMy##name();					\
   svg::SVGElement * getElementDefining##name() const;		\
   void set##name(css::CSSStyle::type);
#include <w3c/css/inherited_style_defs.hpp>
#include <w3c/css/not_inherited_style_defs.hpp> //mmm not sure...
#undef ATTRDEF

#define ATTRDEF(type, name, dtdstr)			\
	  void set##name(float r, float g, float b); \
	  void set##name(const char*); // "red"
	  //void setFill(URL);
ATTRDEF(const_PaintType,Color,"color")
ATTRDEF(const_PaintType,Fill,"fill")
ATTRDEF(const_PaintType,Stroke,"stroke")
ATTRDEF(const_PaintType,StopColor,"stop-color")
#undef ATTRDEF

#define ATTRDEF(type, name, dtdstr)			\
	  void set##name(svg::SVGGradientElement*);	\
	  void set##name(svg::SVGPatternElement*);
ATTRDEF(const_PaintType,Color,"color")
ATTRDEF(const_PaintType,Fill,"fill")
ATTRDEF(const_PaintType,Stroke,"stroke")
#undef ATTRDEF

''')

inter.include_fwd_decls=["svg::SVGClipPathElement", "svg::SVGFilterElement", "svg::SVGMarkerElement", "svg::SVGPatternElement","svg::SVGGradientElement","::BezierPath", "svgl::Context", "svgl::GLInfo","svg::SVGElement", "css::Color", "dom::Document", "svgl::ExternalEntityManager", "agg::path_storage", "::glPoly"]
inter.user_defined_destructor=1

# SVGURIReference
inter = interface()
interfaces["SVGURIReference"]=inter

inter.include_attributes.append('''
\tpublic:
\t\tvoid setHref(const char*);
''')

# SVGUseElement
inter = interface()
interfaces["SVGUseElement"]=inter

inter.include_attributes.append('''
\tpublic:
	virtual void registerUse(svgl::Context*);
	virtual bool areChildrenDamaged();
        virtual void damageChildren();
''')

# SVGTextElement
inter = interface()
interfaces["SVGTextElement"]=inter

inter.include_attributes.append('''
\tprotected:
\t\tvoid glTraverseBoundingBoxPCDATA(svgl::Context*, svgl::GLInfo* glinfo, dom::Text* pcdata);
\tpublic:
\t\tvoid setText(const char*);
\t\t//string getText();

\t\tvirtual long  getNumberOfChars();
\t\tvirtual float getComputedTextLength();
\t\tvirtual float getSubStringLength(unsigned long charnum, unsigned long nchars);

//\t\tvirtual SVGPoint *getStartPositionOfChar(unsigned long charnum);		
//\t\tvirtual SVGPoint *getEndPositionOfChar(unsigned long charnum);
//\t\tvirtual SVGRect  *getExtentOfChar(unsigned long charnum);
//\t\tvirtual float     getRotationOfChar(unsigned long charnum);
//\t\tvirtual long      getCharNumAtPosition(const SVGPoint& point);
''')

inter.include_fwd_decls=["dom::Text"]


# SVGDescElement
inter = interface()
interfaces["SVGDescElement"]=inter

inter.include_attributes.append('''
\tpublic:
\t\tvoid setText(const char*);
\t\tString getText();
''')

inter.include_fwd_decls=["unicode::String"]

# SVGTextPathElement
inter = interface()
interfaces["SVGTextPathElement"]=inter

inter.user_defined_constructor=1

inter.include_attributes.append('''
\tpublic:
\tSVGTextPathElement(dom::Document *);
\tprotected:
\t\tBezierPath* _bezier;
\t\tvoid buildBezier(svgl::Context*, svgl::GLInfo*, svgl::TextInfo*);
''')

inter.include_fwd_decls=["svgl::Context", "svgl::GLInfo", "svgl::TextInfo", "::BezierPath"]


# SVGTextContentElement
inter = interface()
interfaces["SVGTextContentElement"]=inter

inter.include_attributes.append('''
\tprotected:
dom::Text * _currentPCDATA;
SVGTextContentElement * _currentTextContent;
svgl::TextInfo * _textinfo;


	void setFont(svgl::Context * svglContext, svgl::GLInfo* glinfo, svgl::TextInfo* textinfo, glft::FontManager::font_type t);
	void setFont(svgl::Context * svglContext, svgl::GLInfo* glinfo, svgl::TextInfo*);
	virtual void computeGeometryForDrawing(svgl::Context * svglContext, svgl::GLInfo* glinfo);

''')

for i in ["svgl::Context", "svgl::GLInfo", "svgl::TextInfo"]:
    if i not in inter.include_fwd_decls:
        inter.include_fwd_decls.append(i)

for i in ["glft/FontManager.hpp"]:
    if i not in inter.include_includes:
        inter.include_includes.append(i)

# SVGPathElement
inter = interface()
interfaces["SVGPathElement"]=inter


inter.user_defined_constructor=1
inter.include_attributes.append('''
\tprotected:
\t\tBezierPath * _bezier;
\t\tint _glFillList;
\t\tint _glStrokeList;
\tpublic:
\t\tSVGPathElement(dom::Document *);
''')

for i in ["::BezierPath"]:
    if i not in inter.include_fwd_decls:
        inter.include_fwd_decls.append(i)



# SVGPreserveAspectRatio
inter = interface()
interfaces["SVGPreserveAspectRatio"]=inter

inter.include_attributes.append('''
\tprotected:
\t\tbool _dostencil;
\tpublic:
\t\tvirtual void glPreTraverse(svgl::Context*, svgl::GLInfo*, const SVGRect& viewbox, float width, float height);
\t\tvirtual void glPostTraverse(svgl::Context*, svgl::GLInfo*, const SVGRect& viewbox, float width, float height);
''')

for i in ["svgl::Context", "svgl::GLInfo", "svg::SVGRect"]:
    if i not in inter.include_fwd_decls:
        inter.include_fwd_decls.append(i)


# SVGAnimationElement
inter = interface()
interfaces["SVGAnimationElement"]=inter

inter.include_attributes.append('''
\tprotected:
\t\tSVGElement* _elementToAnimate;
float _repeatCount, _repeatCounter;

\tpublic:
\t\tvoid started(svgl::Animation::Manager*);
\t\tbool finished(svgl::Animation::Manager*);
''')

# SVGAnimateElement
inter = interface()
interfaces["SVGAnimateElement"]=inter

inter.include_attributes.append('''
\tprotected:
\t\tsvgl::AnimateAdapter* _attributeToAnimate;
\t\ttypedef svgl::KeyFrame<float> KeyFrame;
\t\ttypedef std::vector<KeyFrame> KeyFrames;
\t\tKeyFrames _keyFrames;
''')

for i in ["svgl::AnimateAdapter"]:
    if i not in inter.include_fwd_decls:
        inter.include_fwd_decls.append(i)

for i in ["svgl/KeyFrame.hpp", "vector"]:
    if i not in inter.include_includes:
        inter.include_includes.append(i)

# SVGTransformable
inter = interface()
interfaces["SVGTransformable"]=inter

inter.include_attributes.append('''
\t\tfriend class SVGAnimateMotionElement;
\t\tfriend class SVGAnimateTransformElement;

\tpublic:

\t\tSVGTranslateTransform * getTranslate(bool create=false);
\t\tSVGScaleTransform * getScale(bool create=false);
\t\tSVGRotateTransform * getRotate(bool create=false);
\t\tvoid getConsolidation(SVGMatrix *);

\t\tvoid scale(double sx, double sy);
\t\tvoid scale(double sx);
\t\tvoid translate(double tx, double ty);
\t\tvoid rotate(double angle);

\t\tvoid dscale(double dsx, double dsy);
\t\tvoid dscale(double dsx);
\t\tvoid dtranslate(double dtx, double dty);
\t\tvoid drotate(double dangle);

\t\tvirtual ~SVGTransformable() {}
''')

for i in ["svg::SVGAnimateMotionElement","svg::SVGAnimateTransformElement","svg::SVGTranslateTransform","svg::SVGScaleTransform","svg::SVGRotateTransform"]:
    if i not in inter.include_fwd_decls:
        inter.include_fwd_decls.append(i)

for i in ["iosfwd"]:
    if i not in inter.include_includes:
        inter.include_includes.append(i)


inter.user_defined_destructor=1



# SVGTransform
inter = interface()
interfaces["SVGTransform"]=inter

inter.include_attributes.append('''
\tpublic:
\t\tstruct Exc {};
\t\tstruct WrongNumberOfArguments : Exc {};
\t\tvirtual void setNeutral()=0;
\t\tvirtual void print(std::ostream&) const;
\t\tvirtual void apply(double x, double y, double* x1, double* y1) const = 0;
\t\tvirtual void apply(SVGMatrix*) const = 0;
\t\tvirtual void updateMatrix() {};
''')

for i in ["iosfwd"]:
    if i not in inter.include_includes:
        inter.include_includes.append(i)


inter.exclude_attributes = ["setMatrix"]


# SVGPaint
inter = interface()
interfaces["SVGPaint"]=inter
inter.exclude_attributes = ["setUri"]


# SVGAnimateColorElement
inter = interface()
interfaces["SVGAnimateColorElement"]=inter

inter.include_attributes.append('''
\tprotected:
\t\tsvgl::AnimateAdapter* _attributeToAnimate;
\t\ttypedef svgl::KeyFrame<css::RGBColor> KeyFrame;
\t\ttypedef std::vector<KeyFrame> KeyFrames;
\t\tKeyFrames _keyFrames;
''')

for i in ["svgl::AnimateAdapter"]:
    if i not in inter.include_fwd_decls:
        inter.include_fwd_decls.append(i)

for i in ["svgl/KeyFrame.hpp", "vector", "w3c/css/RGBColor.hpp"]:
    if i not in inter.include_includes:
        inter.include_includes.append(i)


# SVGAnimateMotionElement
inter = interface()
interfaces["SVGAnimateMotionElement"]=inter

inter.include_attributes.append('''
\tprotected:
	  typedef svgl::KeyFrame<std::pair<float,float> > KeyFrame;
	  typedef std::vector<KeyFrame> KeyFrames;
	  KeyFrames _keyFrames;
	  bool _doBezierAnim;
	  svgl::BezierPathDistanceIterator* _distanceIter;
	  float _bezierLength;
	  SVGTranslateTransform* _translateToAnimate;
          SVGRotateTransform* _rotateToAnimate;

          enum RotateType {
          rotate_no=0,
          rotate_auto,
          rotate_auto_reverse,
          rotate_angle
          };
          RotateType _doRotateAnim;
''')

for i in ["svgl::AnimateAdapter", "svg::SVGTranslateTransform", "svg::SVGRotateTransform", "svgl::BezierPathDistanceIterator"]:
    if i not in inter.include_fwd_decls:
        inter.include_fwd_decls.append(i)

for i in ["svgl/AnimationManager.hpp", "svgl/KeyFrame.hpp", "utility", "vector",]:
    if i not in inter.include_includes:
        inter.include_includes.append(i)


# SVGAnimateTransformElement
inter = interface()
interfaces["SVGAnimateTransformElement"]=inter

inter.include_attributes.append('''
\tpublic:
          struct KeyFrameValueType { float first, second, third; };
	  typedef svgl::KeyFrame< KeyFrameValueType > KeyFrame;
	  typedef std::vector<KeyFrame> KeyFrames;
\tprotected:
	  KeyFrames _keyFrames;
	  SVGTranslateTransform* _translateToAnimate;
          SVGRotateTransform* _rotateToAnimate;
	  SVGScaleTransform* _scaleToAnimate;
	  SVGSkewXTransform* _skewXToAnimate;
	  SVGSkewYTransform* _skewYToAnimate;
''')

for i in ["svgl::AnimateAdapter",
          "svg::SVGTranslateTransform",
          "svg::SVGRotateTransform",
          "svg::SVGScaleTransform",
          "svg::SVGSkewXTransform",
          "svg::SVGSkewYTransform",
          "svgl::BezierPathDistanceIterator"]:
    if i not in inter.include_fwd_decls:
        inter.include_fwd_decls.append(i)

for i in ["svgl/AnimationManager.hpp", "svgl/KeyFrame.hpp", "utility", "vector",]:
    if i not in inter.include_includes:
        inter.include_includes.append(i)


# SVGPathSeg
inter = interface()
interfaces["SVGPathSeg"]=inter

inter.include_attributes.append('''
\tpublic:
\t\tSVGPathSeg(PATHSEG t, unicode::String::char_type c) { pathSegType.setValue(t); pathSegTypeAsLetter.setValue(unicode::String::internStringUtf32(&c, 1)); }
virtual void updateBezier(BezierPath&) const=0;
virtual void updateBezier(agg::path_storage&) const=0;
''')
inter.include_fwd_decls=["::BezierPath", "agg::path_storage"]
inter.include_includes.append("utils/String.hpp")

pathsegcmds=[
("SVGPathSegArcAbs", "A", "PATHSEG_ARC_ABS"),
("SVGPathSegArcRel", "a", "PATHSEG_ARC_REL"),
("SVGPathSegClosePath", "z", "PATHSEG_CLOSEPATH"),
("SVGPathSegCurvetoCubicAbs", "C", "PATHSEG_CURVETO_CUBIC_ABS"),
("SVGPathSegCurvetoCubicRel", "c", "PATHSEG_CURVETO_CUBIC_REL"),
("SVGPathSegCurvetoCubicSmoothAbs", "S", "PATHSEG_CURVETO_CUBIC_SMOOTH_ABS"),
("SVGPathSegCurvetoCubicSmoothRel", "s", "PATHSEG_CURVETO_CUBIC_SMOOTH_REL"),
("SVGPathSegCurvetoQuadraticAbs", "Q", "PATHSEG_CURVETO_QUADRATIC_ABS"),
("SVGPathSegCurvetoQuadraticRel", "q", "PATHSEG_CURVETO_QUADRATIC_REL"),
("SVGPathSegCurvetoQuadraticSmoothAbs", "T", "PATHSEG_CURVETO_QUADRATIC_SMOOTH_ABS"),
("SVGPathSegCurvetoQuadraticSmoothRel", "t", "PATHSEG_CURVETO_QUADRATIC_SMOOTH_REL"),
("SVGPathSegLinetoAbs", "L", "PATHSEG_LINETO_ABS"),
("SVGPathSegLinetoRel", "l", "PATHSEG_LINETO_REL"),
("SVGPathSegLinetoHorizontalAbs", "H", "PATHSEG_LINETO_HORIZONTAL_ABS"),
("SVGPathSegLinetoHorizontalRel", "h", "PATHSEG_LINETO_HORIZONTAL_REL"),
("SVGPathSegLinetoVerticalAbs", "V", "PATHSEG_LINETO_VERTICAL_ABS"),
("SVGPathSegLinetoVerticalRel", "v", "PATHSEG_LINETO_VERTICAL_REL"),
("SVGPathSegMovetoAbs", "M", "PATHSEG_MOVETO_ABS"),
("SVGPathSegMovetoRel", "m", "PATHSEG_MOVETO_REL"),
]

for (classname, cmd, cmdenum) in pathsegcmds:
    inter = interface()
    interfaces[classname]=inter
    inter.include_attributes.append('''
\tpublic:
\t\t%s() : SVGPathSeg(%s,'%s') {}
\t\tvirtual ~%s() {}
\t\tvirtual void updateBezier(BezierPath&) const;
\t\tvirtual void updateBezier(agg::path_storage&) const;
'''%(classname, cmdenum, cmd, classname))
    inter.include_fwd_decls=["::BezierPath", "agg::path_storage"]
    inter.user_defined_destructor=1

# glTraverse

def add_glTraverse(name, const="", virtual="", pure_virtual=""):
    try:
        inter = interfaces[name]
    except KeyError:
        inter = interface()
        interfaces[name] = inter

    inter.include_attributes.append('''
\t\t%s void glTraverse(svgl::Context*, svgl::GLInfo*) %s %s;
'''%(virtual, const, pure_virtual))
    for fwd in ["svgl::Context", "svgl::GLInfo"]:
        if fwd not in inter.include_fwd_decls:
            inter.include_fwd_decls.append(fwd)

for elem in ["", "Rect", "Line", "Circle", "Ellipse", "Polygon", "Path", "G", "LinearGradient", "Defs", "Pattern", "SVG", "ClipPath", "Text", "Use", "Image", "Symbol", "Marker", "Polyline"]:
    name = "SVG%sElement"%elem
    add_glTraverse(name, virtual="virtual")


add_glTraverse("SVGTransform", const="const", virtual="virtual", pure_virtual="=0")
add_glTraverse("SVGTransformable", const="const")

# glTraverseBoundingBox

def add_glTraverseBoundingBox(name, const="", virtual="", pure_virtual=""):
    try:
        inter = interfaces[name]
    except KeyError:
        inter = interface()
        interfaces[name] = inter

    inter.include_attributes.append('''
\t\t%s void glTraverseBoundingBox(svgl::Context*, svgl::GLInfo*) %s %s;
'''%(virtual, const, pure_virtual))
    for fwd in ["svgl::Context", "svgl::GLInfo"]:
        if fwd not in inter.include_fwd_decls:
            inter.include_fwd_decls.append(fwd)

for elem in ["", "Use"]:
    name = "SVG%sElement"%elem
    add_glTraverseBoundingBox(name, virtual="virtual")

# glTraverseBranch

def add_glTraverseBranch(name, const="", virtual="", pure_virtual=""):
    try:
        inter = interfaces[name]
    except KeyError:
        inter = interface()
        interfaces[name] = inter

    inter.include_attributes.append('''
\t\t%s void glTraverseBranch(svgl::Context*, svgl::GLInfo*, Branch::iterator& beg, Branch::iterator& end)  %s %s;
'''%(virtual, const, pure_virtual))
    for fwd in ["svgl::Context", "svgl::GLInfo"]:
        if fwd not in inter.include_fwd_decls:
            inter.include_fwd_decls.append(fwd)

for elem in ["G", "SVG"]:
    name = "SVG%sElement"%elem
    add_glTraverseBranch(name, virtual="virtual")



# pickTraverse

def add_pickTraverse(name, const="", virtual="", pure_virtual=""):
    try:
        inter = interfaces[name]
    except KeyError:
        inter = interface()
        interfaces[name] = inter

    inter.include_attributes.append('''
\t\t%s void pickTraverse(svgl::Context*, svgl::GLInfo*) %s %s;
'''%(virtual, const, pure_virtual))
    for fwd in ["svgl::Context", "svgl::GLInfo"]:
        if fwd not in inter.include_fwd_decls:
            inter.include_fwd_decls.append(fwd)

for elem in ["", "SVG", "Symbol", "Use", "Polygon", "Line", "Polyline", "Path", "Rect", "Text", "Circle", "Ellipse", "G", "ClipPath", "Marker", "Image", "Defs"]:
    name = "SVG%sElement"%elem
    add_pickTraverse(name, virtual="virtual")

add_pickTraverse("SVGStylable")

# glPreTraverse glPostTraverse

def add_glPrePostTraverse(name, const="", pure_virtual=""):
    try:
        inter = interfaces[name]
    except KeyError:
        inter = interface()
        interfaces[name] = inter

    inter.include_attributes.append('''
\t\tvirtual void glPreTraverse(svgl::Context*, svgl::GLInfo*) %s %s;
\t\tvirtual void glPostTraverse(svgl::Context*, svgl::GLInfo*) %s %s;
'''%(const, pure_virtual, const, pure_virtual))
    for fwd in ["svgl::Context", "svgl::GLInfo"]:
        if fwd not in inter.include_fwd_decls:
            inter.include_fwd_decls.append(fwd)


for elem in ["ClipPath"]:
    name = "SVG%sElement"%elem
    add_glPrePostTraverse(name)

#add_glPrePostTraverse("SVGFilterPrimitiveStandardAttributes")


# text stuff

def add_glTextTraverse(name, pure_virtual=""):
    try:
        inter = interfaces[name]
    except KeyError:
        inter = interface()
        interfaces[name] = inter

    inter.include_attributes.append('''
    public:
\t\tvirtual void glTextTraverse(svgl::Context*, svgl::GLInfo*, svgl::TextInfo*) %s;
\t\tvirtual void glTextTraversePCDATA(svgl::Context*, svgl::GLInfo*, svgl::TextInfo*, dom::Text*) %s;

'''%(pure_virtual,pure_virtual))
    for fwd in ["svgl::Context", "svgl::GLInfo", "svgl::TextInfo", "dom::Text"]:
        if fwd not in inter.include_fwd_decls:
            inter.include_fwd_decls.append(fwd)

for elem in ["Text", "TSpan", "TextPath", "TRef", "AltGlyph"]:
    name = "SVG%sElement"%elem
    add_glTextTraverse(name)

add_glTextTraverse("SVGTextContentElement", pure_virtual="=0")


# animationTraverse

def add_animationTraverse(name, const="", virtual="", pure_virtual=""):
    try:
        inter = interfaces[name]
    except KeyError:
        inter = interface()
        interfaces[name] = inter

    inter.include_attributes.append('''
\tpublic:
\t\t%s void animationTraverse(svgl::AnimationInfo*) %s %s;
'''%(virtual, const, pure_virtual))
    if "svgl::AnimationInfo" not in inter.include_fwd_decls:
        inter.include_fwd_decls.append("svgl::AnimationInfo")

for elem in ["", "Animation", "Animate", "AnimateMotion", "AnimateColor", "AnimateTransform"]:
    name = "SVG%sElement"%elem
    add_animationTraverse(name, virtual="virtual")

def add_animationStuff(name):
    try:
        inter = interfaces[name]
    except KeyError:
        inter = interface()
        interfaces[name] = inter

    inter.include_attributes.append('''
\tpublic:
\t\tvirtual void tick(svgl::Animation::TickInfo*);
\t\tvirtual void reset();
''')
    for i in ["svgl/AnimationManager.hpp"]:
        if i not in inter.include_includes:
            inter.include_includes.append(i)

for elem in ["Animation", "Animate", "AnimateMotion", "AnimateColor", "AnimateTransform"]:
    name = "SVG%sElement"%elem
    add_animationStuff(name)


# stylable stuff

def add_stylableStuff(name, pure_virtual=""):
    try:
        inter = interfaces[name]
    except KeyError:
        inter = interface()
        interfaces[name] = inter

    inter.include_attributes.append('''
	  virtual void getBoundingBox(svgl::Context* svglContext, svgl::GLInfo* glinfo, float*, float*, float*, float*) %s;
	  //virtual void glTraverseFill(svgl::Context* svglContext, svgl::GLInfo* glinfo) %s;
	  //virtual void glTraverseStroke(svgl::Context* svglContext, svgl::GLInfo* glinfo, float sw) %s;
          virtual void glTraverseBoundingBox(svgl::Context * svglContext, svgl::GLInfo* glinfo) %s;
          virtual void asBezier(svgl::Context* svglContext, svgl::GLInfo* glinfo, BezierPath*);
          virtual void asBezier(svgl::Context* svglContext, svgl::GLInfo* glinfo, agg::path_storage*);
'''%(pure_virtual,pure_virtual, pure_virtual, pure_virtual))
    for fwd in ["svgl::Context", "svgl::GLInfo", "::BezierPath", "agg::path_storage"]:
        if fwd not in inter.include_fwd_decls:
            inter.include_fwd_decls.append(fwd)

add_stylableStuff("SVGStylable", pure_virtual="")

for elem in ["Polygon", "Path", "Rect", "Circle", "Ellipse", "Line", "Polyline", "Image", "Text", "TSpan", "TextPath"]:
    name = "SVG%sElement"%elem
    add_stylableStuff(name)

def add_fillStrokeStuff(name):
    try:
        inter = interfaces[name]
    except KeyError:
        inter = interface()
        interfaces[name] = inter

    inter.include_attributes.append('''
	  virtual void glTraverseFill(svgl::Context* svglContext, svgl::GLInfo* glinfo);
	  virtual void glTraverseStroke(svgl::Context* svglContext, svgl::GLInfo* glinfo, float sw);
''')
    for fwd in ["svgl::Context", "svgl::GLInfo"]:
        if fwd not in inter.include_fwd_decls:
            inter.include_fwd_decls.append(fwd)

add_fillStrokeStuff("SVGStylable")

for elem in ["Line", "Image", "Text", "TSpan", "TextPath"]:
    name = "SVG%sElement"%elem
    add_fillStrokeStuff(name)

def add_simpleShapeStuff(name):
    try:
        inter = interfaces[name]
    except KeyError:
        inter = interface()
        interfaces[name] = inter

    inter.include_attributes.append('''
	bool canBeCompiled();
''')

for elem in ["Rect", "Circle", "Ellipse", "Line", "Image"]:
    name = "SVG%sElement"%elem
    add_simpleShapeStuff(name)




# glPreTraverse glPostTraverse for filters

def add_glPrePostTraverseFilters(name, const="", pure_virtual=""):
    try:
        inter = interfaces[name]
    except KeyError:
        inter = interface()
        interfaces[name] = inter

    inter.include_attributes.append('''
\t\tvirtual void glPreTraverse(svgl::Context*, svgl::GLInfo*, SVGElement* elem, svgl::FilterContext*) %s %s;
\t\tvirtual void glPostTraverse(svgl::Context*, svgl::GLInfo*, SVGElement* elem, svgl::FilterContext*) %s %s;
\t\tvirtual unicode::String* getOptionalIn1();
\t\tvirtual unicode::String* getOptionalIn2();
\t\tvirtual unicode::String* getOptionalResult();

'''%(const, pure_virtual, const, pure_virtual))
    for fwd in ["svgl::Context", "svgl::GLInfo", "svg::SVGElement", "svgl::FilterContext"]:
        if fwd not in inter.include_fwd_decls:
            inter.include_fwd_decls.append(fwd)


for elem in ["FEBlend", "FEColorMatrix", "FEGaussianBlur", "FEOffset", "FESpecularLighting", "FEComposite", "FEMerge", "FEMergeNode", "FEFlood"]:
    name = "SVG%sElement"%elem
    add_glPrePostTraverseFilters(name)

add_glPrePostTraverseFilters("SVGFilterPrimitiveStandardAttributes")


# customParseAttributes

def add_customParseAttributes(name):
    try:
        inter = interfaces[name]
    except KeyError:
        inter = interface()
        interfaces[name] = inter

    inter.include_attributes.append('''
  int customParseAttributes(const String& attrName, const String& attrValue);
''')

for elem in ["MarkerElement", "FEGaussianBlurElement", "Stylable"]:
    name = "SVG%s"%elem
    add_customParseAttributes(name)

# virtual destructor

def add_virtualDestructor(name):
    try:
        inter = interfaces[name]
    except KeyError:
        inter = interface()
        interfaces[name] = inter

    inter.include_attributes.append('''
\tpublic:
\t\tvirtual ~%s() {}
'''%name)

    inter.user_defined_destructor=1

for i in ["SVGTests", "SVGExternalResourcesRequired", "SVGLangSpace", "SVGFitToViewBox","SVGZoomAndPan", "SVGViewSpec", "SVGFilterPrimitiveStandardAttributes",
          "SVGURIReference", "SVGAnimatedPathData", "SVGAnimatedPoints", "SVGTransform", "SVGPreserveAspectRatio"]:
    add_virtualDestructor(i)

# SVGClipPathElement
#inter = interface()
#interfaces["SVGClipPathElement"]=inter

#inter.include_attributes.append('''
#\t\tvoid glPreTraverse(svgl::GLInfo* glinfo);
#\t\tvoid glPostTraverse(svgl::GLInfo* glinfo);
#''')
#inter.include_fwd_decls=["svgl::GLInfo"]

