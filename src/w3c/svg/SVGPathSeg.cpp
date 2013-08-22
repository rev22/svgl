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
#include <w3c/svg/SVGPathSeg.hpp>
#include <svgl/bezierpath.hpp>
#include <svgl/getattr.hpp>
#include <svgl/debug.hpp>

#include <agg/agg_path_storage.h>


#include <w3c/svg/SVGPathSegArcAbs.hpp>
#include <w3c/svg/SVGPathSegArcRel.hpp>
#include <w3c/svg/SVGPathSegClosePath.hpp>
#include <w3c/svg/SVGPathSegCurvetoCubicAbs.hpp>
#include <w3c/svg/SVGPathSegCurvetoCubicRel.hpp>
#include <w3c/svg/SVGPathSegCurvetoCubicSmoothAbs.hpp>
#include <w3c/svg/SVGPathSegCurvetoCubicSmoothRel.hpp>
#include <w3c/svg/SVGPathSegCurvetoQuadraticAbs.hpp>
#include <w3c/svg/SVGPathSegCurvetoQuadraticRel.hpp>
#include <w3c/svg/SVGPathSegCurvetoQuadraticSmoothAbs.hpp>
#include <w3c/svg/SVGPathSegCurvetoQuadraticSmoothRel.hpp>
#include <w3c/svg/SVGPathSegLinetoAbs.hpp>
#include <w3c/svg/SVGPathSegLinetoRel.hpp>
#include <w3c/svg/SVGPathSegLinetoHorizontalAbs.hpp>
#include <w3c/svg/SVGPathSegLinetoHorizontalRel.hpp>
#include <w3c/svg/SVGPathSegLinetoVerticalAbs.hpp>
#include <w3c/svg/SVGPathSegLinetoVerticalRel.hpp>
#include <w3c/svg/SVGPathSegMovetoAbs.hpp>
#include <w3c/svg/SVGPathSegMovetoRel.hpp>

#include <cmath>

namespace svg {

#define DBG std::cerr << __FILE__ ":" << __LINE__ << std::endl; return;
  //#define DBGMETH(name) std::cerr << #name ":updateBezier" << std::endl;
#define DBGMETH(name)
//#define DBGVAR(name) " " #name " " << name

  bool
  tess_arc( BezierPath& b,
	    BezierPath::Coord x1,
	    BezierPath::Coord y1,
	    BezierPath::Coord x2,
	    BezierPath::Coord y2,
	    BezierPath::Coord rx,
	    BezierPath::Coord ry,
	    BezierPath::Coord angle,
	    bool largeArc,
	    bool sweep
	    )
  {
    // be sure rx or ry are non-zero
    if( rx==0 || ry==0) {
      b.line_to(x2,y2);
      return true;
    }

    // make sure rx and ry are positive
    if(rx<0)
      rx=-rx;
    if(ry<0)
      ry=-ry;

    const BezierPath::Coord cosa = cos(angle*M_PI/180.);
    const BezierPath::Coord sina = sin(angle*M_PI/180.);


    // compute the center (see svg.pdf)
    const BezierPath::Coord xp1 = cosa * (x1-x2)/2. + sina * (y1-y2)/2.;
    const BezierPath::Coord yp1 = -sina * (x1-x2)/2. + cosa * (y1-y2)/2.;

    BezierPath::Coord rx2 = rx*rx, ry2 = ry*ry, xp12=xp1*xp1, yp12=yp1*yp1;

    // make sure rx and ry are large enough
    {
      BezierPath::Coord tmp = xp12/rx2 + yp12/ry2;
      if (tmp>1) {
				rx2*=tmp;
				ry2*=tmp;
				tmp = sqrt(tmp);
				rx*=tmp;
				ry*=tmp;
      }
    }

    BezierPath::Coord fact = ( rx2*ry2 / (rx2*yp12 + ry2*xp12))-1;
    if (fact<0) {
      std::cerr << DBGVAR(fact) << __FILE__":" << __LINE__ << std::endl;
      fact=0;
    }
    fact = sqrt(fact);
    if (largeArc==sweep)
      fact = -fact;

    BezierPath::Coord xpc = rx * yp1 / ry;
    BezierPath::Coord ypc =  - ry * xp1 / rx;

    xpc *= fact;
    ypc *= fact;

    BezierPath::Coord xc = xpc * cosa - ypc * sina + (x1+x2)/2.;
    BezierPath::Coord yc = xpc * sina + ypc * cosa + (y1+y2)/2.;

    // determine t1 and t2, limits given by (x1, y1) (x2, y2)

    BezierPath::Coord t1;
    BezierPath::Coord deltat;
    //    BezierPath::Coord t2;
    {
      BezierPath::Coord xv1 = (xp1-xpc)/rx;
      BezierPath::Coord yv1 = (yp1-ypc)/ry;
      BezierPath::Coord norm1 = xv1*xv1 + yv1*yv1;

      BezierPath::Coord cosangle1 = xv1/sqrt(norm1);
      t1 = acos(cosangle1);
      if(yv1<0)
	t1 = -t1;

      BezierPath::Coord xv2 = (-xp1-xpc)/rx;
      BezierPath::Coord yv2 = (-yp1-ypc)/ry;
      BezierPath::Coord norm2 = xv2*xv2 + yv2*yv2;
      deltat = xv1*xv2+yv1*yv2;
      deltat = acos(deltat/sqrt(norm1*norm2));
      if ( (xv1*yv2-yv1*xv2)<0)
				deltat = -deltat;

      if (sweep==false && deltat>0)
				deltat -= 2*M_PI;

      if (sweep==true && deltat<0)
				deltat += 2*M_PI;

      //      t2=t1+deltat;
    }
#if 0
    {
      BezierPath::Coord C = -.5*rx*(sina*(x2-x1)+cosa*(y2-y1));
      BezierPath::Coord D = -.5*ry*(cosa*(x2-x1)+sina*(y2-y1));
      t1 = std::atan( (.5*(C*C+D*D)-C)/D );
      t2 = std::acos(C+std::cos(t1));
      xc = x1 - .5*rx*sina*std::cos(t1) - .5*ry*cosa*std::sin(t1);
      yc = y1 + .5*rx*cosa*std::cos(t1) - .5*ry*sina*std::sin(t1);
      std::cerr << "center " << xc << " " << yc << std::endl;
      std::cerr << DBGVAR(t1) << DBGVAR(t2) << std::endl;
      BezierPath::Coord t3 = t1;
      if (t3>0)
				t3-=M_PI/2.;
      else
				t3+=M_PI/2.;
      BezierPath::Coord t4 = std::acos(C+std::cos(t1));
      std::cerr << DBGVAR(t3) << DBGVAR(t4) << std::endl;
    }
#endif


    // compute vertices
    /*
      fast incremental cos and sin generation

      cos(a+dt) = cos(a)cos(dt)-sin(a)sin(dt);
      sin(a+dt) = cos(a)sin(dt) + cos(dt)sin(a);

      <=>
      cos(a+2*dt) = cos(a+dt)cos(dt) - sin(a+dt)sin(dt)
      sin(a+2*dt) = cos(a+dt)sin(dt) + cos(a+dt)sin(a)
    */

    const int nb = 20;
    const BezierPath::Coord dt = (deltat)/nb;
    const BezierPath::Coord cosdt = cos(dt);
    const BezierPath::Coord sindt = sin(dt);
    BezierPath::Coord cost1_plus_ndt = cos(t1+dt);
    BezierPath::Coord sint1_plus_ndt = sin(t1+dt);


    for(int i=0; i<nb; ++i) {
      /*
	ellipsoid:
	x(t) = rx.cos(t)
	y(t) = ry.cos(t)
      */

      BezierPath::Coord x = rx * cost1_plus_ndt;
      BezierPath::Coord y = ry * sint1_plus_ndt;

      {
	// compute cos and sin
				BezierPath::Coord tmp = cost1_plus_ndt * cosdt - sint1_plus_ndt * sindt;
				sint1_plus_ndt = cost1_plus_ndt * sindt + sint1_plus_ndt * cosdt; 
				cost1_plus_ndt = tmp;
      }
      
      // rotate
      // angle=-aangle, I don't knwo why...
      {
				BezierPath::Coord tmp = x * cosa - y * sina;
				y = x * sina + y * cosa;
				x = tmp;
      }
        
      // translate
      x+=xc;
      y+=yc;
      
      b.line_to(x,y);
    }
    return true;
  }

  void
  SVGPathSegArcAbs::updateBezier(BezierPath& b) const
  {
    DBGMETH(SVGPathSegArcAbs);
    BezierPath::Coord x;
    BezierPath::Coord y;
    if( !b.current_point(x, y)) {DBG}
    if( !tess_arc(
		  b,
		  x,y,
		  GET_SIMPLE_VAL(X), GET_SIMPLE_VAL(Y),
		  GET_SIMPLE_VAL(R1), GET_SIMPLE_VAL(R2),
		  GET_SIMPLE_VAL(Angle),
		  GET_SIMPLE_VAL(LargeArcFlag),
		  GET_SIMPLE_VAL(SweepFlag)
		  )) {DBG}
  }

  void
  SVGPathSegArcRel::updateBezier(BezierPath& b) const
  {
    DBGMETH(SVGPathSegArcRel);
    BezierPath::Coord x;
    BezierPath::Coord y;
    if( !b.current_point(x, y)) {DBG}
    if( !tess_arc(
		  b,
		  x,y,
		  x+GET_SIMPLE_VAL(X), y+GET_SIMPLE_VAL(Y),
		  GET_SIMPLE_VAL(R1), GET_SIMPLE_VAL(R2),
		  GET_SIMPLE_VAL(Angle),
		  GET_SIMPLE_VAL(LargeArcFlag),
		  GET_SIMPLE_VAL(SweepFlag)
		  )) {DBG}
  }

  void
  SVGPathSegClosePath::updateBezier(BezierPath& b) const
  {
    DBGMETH(SVGPathSegClosePath);
    b.close_path();
		//if( !b.current_point(x, y)) {DBG}
		//std::cerr << DBGVAR(x) << DBGVAR(Y) << __FL__;
  }

  void
  SVGPathSegCurvetoCubicAbs::updateBezier(BezierPath& b) const
  {
    DBGMETH(SVGPathSegCurvetoCubicRel);
    if(!b.cubic_to(
		   GET_SIMPLE_VAL(X1), GET_SIMPLE_VAL(Y1),
		   GET_SIMPLE_VAL(X2), GET_SIMPLE_VAL(Y2),
		   GET_SIMPLE_VAL(X), GET_SIMPLE_VAL(Y)
	       )) {DBG}
  }
  
  void
  SVGPathSegCurvetoCubicRel::updateBezier(BezierPath& b) const
  {
    DBGMETH(SVGPathSegCurvetoCubicAbs);
    BezierPath::Coord x;
    BezierPath::Coord y;
    if( !b.current_point(x, y)) {DBG}
    if(!b.cubic_to(
		   x+GET_SIMPLE_VAL(X1), y+GET_SIMPLE_VAL(Y1),
		   x+GET_SIMPLE_VAL(X2), y+GET_SIMPLE_VAL(Y2),
		   x+GET_SIMPLE_VAL(X), y+GET_SIMPLE_VAL(Y)
	       )) {DBG}
  }
  
  void
  SVGPathSegCurvetoCubicSmoothAbs::updateBezier(BezierPath& b) const
  {
    BezierPath::Coord x;
    BezierPath::Coord y;
    DBGMETH(SVGPathSegCurvetoCubicSmoothRel);
    if( !b.current_point(x, y)) {DBG}
    BezierPath::Coord xb;
    BezierPath::Coord yb;
    if( !b.before_current_point(xb, yb)) {DBG}

    xb = x+x-xb;
    yb = y+y-yb;

    if(!b.cubic_to(
		   xb, yb,
		   GET_SIMPLE_VAL(X2), GET_SIMPLE_VAL(Y2),
		   GET_SIMPLE_VAL(X), GET_SIMPLE_VAL(Y)
	       )) {DBG}
  }

  void
  SVGPathSegCurvetoCubicSmoothRel::updateBezier(BezierPath& b) const
  {
    BezierPath::Coord x;
    BezierPath::Coord y;
    DBGMETH(SVGPathSegCurvetoCubicSmoothAbs);
    if( !b.current_point(x, y)) {DBG}
    BezierPath::Coord xb;
    BezierPath::Coord yb;
    if( !b.before_current_point(xb, yb)) {DBG}

    xb = x+x-xb;
    yb = y+y-yb;

    if(!b.cubic_to(
	       xb, yb,
	       x+GET_SIMPLE_VAL(X2), y+GET_SIMPLE_VAL(Y2),
	       x+GET_SIMPLE_VAL(X), y+GET_SIMPLE_VAL(Y)
	       )) {DBG}
  }
  
  void
  SVGPathSegCurvetoQuadraticAbs::updateBezier(BezierPath& b) const
  {
    DBGMETH(SVGPathSegCurvetoQuadraticRel);
    BezierPath::Coord x0;
    BezierPath::Coord y0;
    if( !b.current_point(x0, y0)) {DBG}

    BezierPath::Coord xp1,yp1,xp2,yp2;
    xp1 = GET_SIMPLE_VAL(X1);
    yp1 = GET_SIMPLE_VAL(Y1);
    xp2 = GET_SIMPLE_VAL(X);
    yp2 = GET_SIMPLE_VAL(Y);

    if(!b.parabole_to(
		      xp1, yp1,
		      xp2, yp2
		  )) {DBG}
  }
  
  void
  SVGPathSegCurvetoQuadraticRel::updateBezier(BezierPath& b) const
  {
    DBGMETH(SVGPathSegCurvetoQuadraticAbs);
    BezierPath::Coord x0;
    BezierPath::Coord y0;
    if( !b.current_point(x0, y0)) {DBG}

    BezierPath::Coord xp1,yp1,xp2,yp2;
    xp1 = x0+GET_SIMPLE_VAL(X1);
    yp1 = y0+GET_SIMPLE_VAL(Y1);
    xp2 = x0+GET_SIMPLE_VAL(X);
    yp2 = y0+GET_SIMPLE_VAL(Y);

    if(!b.parabole_to(
		      xp1, yp1,
		      xp2, yp2		  		  
		  )) {DBG}
  }
  
  void
  SVGPathSegCurvetoQuadraticSmoothAbs::updateBezier(BezierPath& b) const
  {
    DBGMETH(SVGPathSegCurvetoQuadraticSmoothAbs);
    BezierPath::Coord x0;
    BezierPath::Coord y0;
    if( !b.current_point(x0, y0)) {DBG}
    BezierPath::Coord xp1;
    BezierPath::Coord yp1;
    if( !b.before_current_point(xp1, yp1)) {DBG}

    xp1 = x0+x0-xp1;
    yp1 = y0+y0-yp1;

    BezierPath::Coord xp2 = GET_SIMPLE_VAL(X);
    BezierPath::Coord yp2 = GET_SIMPLE_VAL(Y);
    if(!b.parabole_to(
		      xp1, yp1,
		      xp2, yp2
		  )) {DBG}
  }
  
  void
  SVGPathSegCurvetoQuadraticSmoothRel::updateBezier(BezierPath& b) const
  {
    DBGMETH(SVGPathSegCurvetoQuadraticSmoothRel);
    BezierPath::Coord x0;
    BezierPath::Coord y0;
    if( !b.current_point(x0, y0)) {DBG}
    BezierPath::Coord xp1;
    BezierPath::Coord yp1;
    if( !b.before_current_point(xp1, yp1)) {DBG}

    xp1 = x0+x0-xp1;
    yp1 = y0+y0-yp1;

    BezierPath::Coord xp2 = x0+GET_SIMPLE_VAL(X);
    BezierPath::Coord yp2 = y0+GET_SIMPLE_VAL(Y);

    if(!b.parabole_to(
		      xp1, yp1,
		      xp2, yp2		  		  
		  )) {DBG}
  }
  
  void
  SVGPathSegLinetoAbs::updateBezier(BezierPath& b) const
  {
    DBGMETH(SVGPathSegLinetoAbs);
    if(!b.line_to(GET_SIMPLE_VAL(X), GET_SIMPLE_VAL(Y))) {DBG}
  }
  
  void
  SVGPathSegLinetoRel::updateBezier(BezierPath& b) const
  {
    DBGMETH(SVGPathSegLinetoRel);
    BezierPath::Coord x;
    BezierPath::Coord y;
    if( !b.current_point(x, y)) {DBG}
    if(!b.line_to(x+GET_SIMPLE_VAL(X), y+GET_SIMPLE_VAL(Y))) {DBG}
  }
  
  void
  SVGPathSegLinetoHorizontalAbs::updateBezier(BezierPath& b) const
  {
    DBGMETH(SVGPathSegLinetoHorizontalAbs);
    BezierPath::Coord x;
    BezierPath::Coord y;
    if( !b.current_point(x, y)) {DBG}
    if(!b.line_to(GET_SIMPLE_VAL(X), y)) {DBG}
  }
  
  void
  SVGPathSegLinetoHorizontalRel::updateBezier(BezierPath& b) const
  {
    DBGMETH(SVGPathSegLinetoHorizontalRel);
    BezierPath::Coord x;
    BezierPath::Coord y;
    if( !b.current_point(x, y)) {DBG}
    if(!b.line_to(x+GET_SIMPLE_VAL(X), y)) {DBG}
  }
  
  void
  SVGPathSegLinetoVerticalAbs::updateBezier(BezierPath& b) const
  {
    DBGMETH(SVGPathSegLinetoVerticalAbs);
    BezierPath::Coord x;
    BezierPath::Coord y;
    if( !b.current_point(x, y)) {DBG}
    if(!b.line_to(x, GET_SIMPLE_VAL(Y))) {DBG}
  }
  
  void
  SVGPathSegLinetoVerticalRel::updateBezier(BezierPath& b) const
  {
    DBGMETH(SVGPathSegLinetoVerticalRel);
    BezierPath::Coord x;
    BezierPath::Coord y;
    if( !b.current_point(x, y)) {DBG}
    if( !b.line_to(x, y+GET_SIMPLE_VAL(Y))) {DBG}
  }
  
  void
  SVGPathSegMovetoAbs::updateBezier(BezierPath& b) const
  {
    DBGMETH(SVGPathSegMovetoAbs);
    if(!b.move_to(GET_SIMPLE_VAL(X), GET_SIMPLE_VAL(Y))) {DBG}
  }
  
  void
  SVGPathSegMovetoRel::updateBezier(BezierPath& b) const
  {
    DBGMETH(SVGPathSegMovetoRel);
    BezierPath::Coord x;
    BezierPath::Coord y;
    if( !b.current_point(x, y)) {
      x=0;
      y=0;
    }
    if(!b.move_to(x+GET_SIMPLE_VAL(X), y+GET_SIMPLE_VAL(Y))) {DBG}
  }

#define USEAGG 1
	// -------------------------------------------------------------------------------------
	// agg:path_storage
	
	
	bool
  tess_arc( agg::path_storage& b,
						double x1,
						double y1,
						double x2,
						double y2,
						double rx,
						double ry,
						double angle,
						bool largeArc,
						bool sweep
						)
{
#if USEAGG
    // be sure rx or ry are non-zero
    if( rx==0 || ry==0) {
      b.line_to(x2,y2);
      return true;
    }
		
    // make sure rx and ry are positive
    if(rx<0)
      rx=-rx;
    if(ry<0)
      ry=-ry;
		
    const double cosa = cos(angle*M_PI/180.);
    const double sina = sin(angle*M_PI/180.);
		
		
    // compute the center (see svg.pdf)
    const double xp1 = cosa * (x1-x2)/2. + sina * (y1-y2)/2.;
    const double yp1 = -sina * (x1-x2)/2. + cosa * (y1-y2)/2.;
		
    double rx2 = rx*rx, ry2 = ry*ry, xp12=xp1*xp1, yp12=yp1*yp1;
		
    // make sure rx and ry are large enough
    {
      double tmp = xp12/rx2 + yp12/ry2;
      if (tmp>1) {
				rx2*=tmp;
				ry2*=tmp;
				tmp = sqrt(tmp);
				rx*=tmp;
				ry*=tmp;
      }
    }
		
    double fact = ( rx2*ry2 / (rx2*yp12 + ry2*xp12))-1;
    if (fact<0) {
      std::cerr << DBGVAR(fact) << __FILE__":" << __LINE__ << std::endl;
      fact=0;
    }
    fact = sqrt(fact);
    if (largeArc==sweep)
      fact = -fact;
		
    double xpc = rx * yp1 / ry;
    double ypc =  - ry * xp1 / rx;
		
    xpc *= fact;
    ypc *= fact;
		
    double xc = xpc * cosa - ypc * sina + (x1+x2)/2.;
    double yc = xpc * sina + ypc * cosa + (y1+y2)/2.;
		
    // determine t1 and t2, limits given by (x1, y1) (x2, y2)
		
    double t1;
    double deltat;
    //    double t2;
    {
      double xv1 = (xp1-xpc)/rx;
      double yv1 = (yp1-ypc)/ry;
      double norm1 = xv1*xv1 + yv1*yv1;
			
      double cosangle1 = xv1/sqrt(norm1);
      t1 = acos(cosangle1);
      if(yv1<0)
				t1 = -t1;
			
      double xv2 = (-xp1-xpc)/rx;
      double yv2 = (-yp1-ypc)/ry;
      double norm2 = xv2*xv2 + yv2*yv2;
      deltat = xv1*xv2+yv1*yv2;
      deltat = acos(deltat/sqrt(norm1*norm2));
      if ( (xv1*yv2-yv1*xv2)<0)
				deltat = -deltat;
			
      if (sweep==false && deltat>0)
				deltat -= 2*M_PI;
			
      if (sweep==true && deltat<0)
				deltat += 2*M_PI;
			
      //      t2=t1+deltat;
    }
#if 0
    {
      double C = -.5*rx*(sina*(x2-x1)+cosa*(y2-y1));
      double D = -.5*ry*(cosa*(x2-x1)+sina*(y2-y1));
      t1 = std::atan( (.5*(C*C+D*D)-C)/D );
      t2 = std::acos(C+std::cos(t1));
      xc = x1 - .5*rx*sina*std::cos(t1) - .5*ry*cosa*std::sin(t1);
      yc = y1 + .5*rx*cosa*std::cos(t1) - .5*ry*sina*std::sin(t1);
      std::cerr << "center " << xc << " " << yc << std::endl;
      std::cerr << DBGVAR(t1) << DBGVAR(t2) << std::endl;
      double t3 = t1;
      if (t3>0)
				t3-=M_PI/2.;
      else
				t3+=M_PI/2.;
      double t4 = std::acos(C+std::cos(t1));
      std::cerr << DBGVAR(t3) << DBGVAR(t4) << std::endl;
    }
#endif
		
		
    // compute vertices
    /*
		 fast incremental cos and sin generation
		 
		 cos(a+dt) = cos(a)cos(dt)-sin(a)sin(dt);
		 sin(a+dt) = cos(a)sin(dt) + cos(dt)sin(a);
		 
		 <=>
		 cos(a+2*dt) = cos(a+dt)cos(dt) - sin(a+dt)sin(dt)
		 sin(a+2*dt) = cos(a+dt)sin(dt) + cos(a+dt)sin(a)
		 */
		
    const int nb = 20;
    const double dt = (deltat)/nb;
    const double cosdt = cos(dt);
    const double sindt = sin(dt);
    double cost1_plus_ndt = cos(t1+dt);
    double sint1_plus_ndt = sin(t1+dt);
		
		
    for(int i=0; i<nb; ++i) {
      /*
			 ellipsoid:
			 x(t) = rx.cos(t)
			 y(t) = ry.cos(t)
			 */
			
      double x = rx * cost1_plus_ndt;
      double y = ry * sint1_plus_ndt;
			
      {
				// compute cos and sin
				double tmp = cost1_plus_ndt * cosdt - sint1_plus_ndt * sindt;
				sint1_plus_ndt = cost1_plus_ndt * sindt + sint1_plus_ndt * cosdt; 
				cost1_plus_ndt = tmp;
      }
      
      // rotate
      // angle=-aangle, I don't knwo why...
      {
				double tmp = x * cosa - y * sina;
				y = x * sina + y * cosa;
				x = tmp;
      }
			
      // translate
      x+=xc;
      y+=yc;
      
      b.line_to(x,y);
    }
    return true;
#endif
}

// A
void
SVGPathSegArcAbs::updateBezier(agg::path_storage& b) const
{
#if USEAGG
	DBGMETH(SVGPathSegArcAbs);
	double x;
	double y;
	//if( !b.current_point(x, y)) {DBG}	
	unsigned int tv = b.total_vertices();
	if(!tv) {DBG; return;}
	b.vertex(tv-1, &x, &y);
	if( !tess_arc(
								b,
								x,y,
								GET_SIMPLE_VAL(X), GET_SIMPLE_VAL(Y),
								GET_SIMPLE_VAL(R1), GET_SIMPLE_VAL(R2),
								GET_SIMPLE_VAL(Angle),
								GET_SIMPLE_VAL(LargeArcFlag),
								GET_SIMPLE_VAL(SweepFlag)
								)) {DBG}
#endif
}


// a
void
SVGPathSegArcRel::updateBezier(agg::path_storage& b) const
{
#if USEAGG
	DBGMETH(SVGPathSegArcRel);
	double x;
	double y;
	//if( !b.current_point(x, y)) {DBG}
	unsigned int tv = b.total_vertices();
	if(tv<1) {DBG; return;}
	b.vertex(tv-1, &x, &y);
	if( !tess_arc(
								b,
								x,y,
								x+GET_SIMPLE_VAL(X), y+GET_SIMPLE_VAL(Y),
								GET_SIMPLE_VAL(R1), GET_SIMPLE_VAL(R2),
								GET_SIMPLE_VAL(Angle),
								GET_SIMPLE_VAL(LargeArcFlag),
								GET_SIMPLE_VAL(SweepFlag)
								)) {DBG}
#endif
}

// z
void
SVGPathSegClosePath::updateBezier(agg::path_storage& b) const
{
#if USEAGG
	DBGMETH(SVGPathSegClosePath);
	unsigned int tv = b.total_vertices();
	if(tv<1) {DBG; return;}
	double x,y;
	--tv;
	unsigned int cmd = b.vertex(tv, &x, &y);
	// with agg, the vertex corrsponding to the close command is not the last vertex of the shape (which is also the first)
	// so we find the first point and add a fake move_to command to the first point...
	while(!agg::is_next_poly(cmd) && tv) {
		--tv;
		cmd = b.vertex(tv, &x, &y);
	}
	//if(agg::is_next_poly(cmd))
	//std::cerr << DBGVAR(cmd) << DBGVAR(tv) << __FL__;
	//++tv;	
	//b.vertex(tv, &x, &y);
	b.end_poly(agg::path_flags_close);
	b.move_to(x,y);
	//std::cerr << DBGVAR(x) << DBGVAR(y) << __FL__;
#endif
}

// 
void
SVGPathSegCurvetoCubicAbs::updateBezier(agg::path_storage& b) const
{
#if USEAGG
	DBGMETH(SVGPathSegCurvetoCubicRel);
	b.curve4(
					 GET_SIMPLE_VAL(X1), GET_SIMPLE_VAL(Y1),
					 GET_SIMPLE_VAL(X2), GET_SIMPLE_VAL(Y2),
					 GET_SIMPLE_VAL(X), GET_SIMPLE_VAL(Y)
					 );
#endif
}

void
SVGPathSegCurvetoCubicRel::updateBezier(agg::path_storage& b) const
{
#if USEAGG
	DBGMETH(SVGPathSegCurvetoCubicAbs);
	double x;
	double y;
	//if( !b.current_point(x, y)) {DBG}
	unsigned int tv = b.total_vertices();
	if(tv<1) {DBG; return;}
	b.vertex(tv-1, &x, &y);
	b.curve4(
					 x+GET_SIMPLE_VAL(X1), y+GET_SIMPLE_VAL(Y1),
					 x+GET_SIMPLE_VAL(X2), y+GET_SIMPLE_VAL(Y2),
					 x+GET_SIMPLE_VAL(X), y+GET_SIMPLE_VAL(Y)
					 );
#endif
}

void
SVGPathSegCurvetoCubicSmoothAbs::updateBezier(agg::path_storage& b) const
{
#if USEAGG
	double x;
	double y;
	DBGMETH(SVGPathSegCurvetoCubicSmoothRel);
	//if( !b.current_point(x, y)) {DBG}
	double xb;
	double yb;
	//if( !b.before_current_point(xb, yb)) {DBG}

	unsigned int tv = b.total_vertices();
	if(tv<2) {DBG; return;}
	b.vertex(tv-1, &x, &y);
	b.vertex(tv-2, &xb, &yb);
	
	xb = x+x-xb;
	yb = y+y-yb;
	
	b.curve4(
					 xb, yb,
					 GET_SIMPLE_VAL(X2), GET_SIMPLE_VAL(Y2),
					 GET_SIMPLE_VAL(X), GET_SIMPLE_VAL(Y)
					 );
#endif
}

void
SVGPathSegCurvetoCubicSmoothRel::updateBezier(agg::path_storage& b) const
{
#if USEAGG
	double x;
	double y;
	DBGMETH(SVGPathSegCurvetoCubicSmoothAbs);
	//if( !b.current_point(x, y)) {DBG}
	double xb;
	double yb;
	//if( !b.before_current_point(xb, yb)) {DBG}

	unsigned int tv = b.total_vertices();
	if(tv<2) {DBG; return;}
	b.vertex(tv-1, &x, &y);
	b.vertex(tv-2, &xb, &yb);
		
	xb = x+x-xb;
	yb = y+y-yb;
	
	b.curve4(
					 xb, yb,
					 x+GET_SIMPLE_VAL(X2), y+GET_SIMPLE_VAL(Y2),
					 x+GET_SIMPLE_VAL(X), y+GET_SIMPLE_VAL(Y)
					 );
#endif
}

void
SVGPathSegCurvetoQuadraticAbs::updateBezier(agg::path_storage& b) const
{
#if USEAGG
	DBGMETH(SVGPathSegCurvetoQuadraticRel);
	//double x0;
	//double y0;
	//if( !b.current_point(x0, y0)) {DBG}
	
	double xp1,yp1,xp2,yp2;
	xp1 = GET_SIMPLE_VAL(X1);
	yp1 = GET_SIMPLE_VAL(Y1);
	xp2 = GET_SIMPLE_VAL(X);
	yp2 = GET_SIMPLE_VAL(Y);
	
	b.curve3(
					 xp1, yp1,
					 xp2, yp2
					 );
#endif
}

void
SVGPathSegCurvetoQuadraticRel::updateBezier(agg::path_storage& b) const
{
#if USEAGG
	DBGMETH(SVGPathSegCurvetoQuadraticAbs);
	double x;
	double y;
	//if( !b.current_point(x, y)) {DBG}
	unsigned int tv = b.total_vertices();
	if(!tv) {DBG; return;}
	b.vertex(tv-1, &x, &y);
	
	double xp1,yp1,xp2,yp2;
	xp1 = x+GET_SIMPLE_VAL(X1);
	yp1 = y+GET_SIMPLE_VAL(Y1);
	xp2 = x+GET_SIMPLE_VAL(X);
	yp2 = y+GET_SIMPLE_VAL(Y);
	
	b.curve3(
					 xp1, yp1,
					 xp2, yp2		  		  
					 );
#endif
}

void
SVGPathSegCurvetoQuadraticSmoothAbs::updateBezier(agg::path_storage& b) const
{
#if USEAGG
	DBGMETH(SVGPathSegCurvetoQuadraticSmoothAbs);
	double x0;
	double y0;
	//if( !b.current_point(x0, y0)) {DBG}
	double xp1;
	double yp1;
	//if( !b.before_current_point(xp1, yp1)) {DBG}
	
	unsigned int tv = b.total_vertices();
	if(tv<2) {DBG; return;}
	b.vertex(tv-1, &x0, &y0);
	b.vertex(tv-2, &xp1, &yp1);
	
	xp1 = x0+x0-xp1;
	yp1 = y0+y0-yp1;
	
	double xp2 = GET_SIMPLE_VAL(X);
	double yp2 = GET_SIMPLE_VAL(Y);
	b.curve3(
					 xp1, yp1,
					 xp2, yp2
					 );
#endif
}

void
SVGPathSegCurvetoQuadraticSmoothRel::updateBezier(agg::path_storage& b) const
{
#if USEAGG
	DBGMETH(SVGPathSegCurvetoQuadraticSmoothRel);
	double x0;
	double y0;
	//if( !b.current_point(x0, y0)) {DBG}
	double xp1;
	double yp1;
	//if( !b.before_current_point(xp1, yp1)) {DBG}
	
	unsigned int tv = b.total_vertices();
	if(tv<2) {DBG; return;}
	b.vertex(tv-1, &x0, &y0);
	b.vertex(tv-2, &xp1, &yp1);
	
	xp1 = x0+x0-xp1;
	yp1 = y0+y0-yp1;
	
	double xp2 = x0+GET_SIMPLE_VAL(X);
	double yp2 = y0+GET_SIMPLE_VAL(Y);
	
	b.curve3(
					 xp1, yp1,
					 xp2, yp2		  		  
					 );
#endif
}

void
SVGPathSegLinetoAbs::updateBezier(agg::path_storage& b) const
{
#if USEAGG
	DBGMETH(SVGPathSegLinetoAbs);
	b.line_to(GET_SIMPLE_VAL(X), GET_SIMPLE_VAL(Y));
#endif
}

void
SVGPathSegLinetoRel::updateBezier(agg::path_storage& b) const
{
#if USEAGG
	DBGMETH(SVGPathSegLinetoRel);
	double x;
	double y;
	//if( !b.current_point(x, y)) {DBG}
	unsigned int tv = b.total_vertices();
	if(!tv) {DBG; return;}
	b.vertex(tv-1, &x, &y);
	b.line_to(x+GET_SIMPLE_VAL(X), y+GET_SIMPLE_VAL(Y));
#endif
}

void
SVGPathSegLinetoHorizontalAbs::updateBezier(agg::path_storage& b) const
{
#if USEAGG
	DBGMETH(SVGPathSegLinetoHorizontalAbs);
	double x;
	double y;
	//if( !b.current_point(x, y)) {DBG}
	unsigned int tv = b.total_vertices();
	if(!tv) {DBG; return;}
	b.vertex(tv-1, &x, &y);
	
	b.line_to(GET_SIMPLE_VAL(X), y);
#endif
}

void
SVGPathSegLinetoHorizontalRel::updateBezier(agg::path_storage& b) const
{
#if USEAGG
	DBGMETH(SVGPathSegLinetoHorizontalRel);
	double x;
	double y;
	//if( !b.current_point(x, y)) {DBG}
	unsigned int tv = b.total_vertices();
	if(!tv) {DBG; return;}
	b.vertex(tv-1, &x, &y);
	b.line_to(x+GET_SIMPLE_VAL(X), y);
#endif
}

void
SVGPathSegLinetoVerticalAbs::updateBezier(agg::path_storage& b) const
{
#if USEAGG
	DBGMETH(SVGPathSegLinetoVerticalAbs);
	double x;
	double y;
	//if( !b.current_point(x, y)) {DBG}
	unsigned int tv = b.total_vertices();
	if(!tv) {DBG; return;}
	b.vertex(tv-1, &x, &y);
	b.line_to(x, GET_SIMPLE_VAL(Y));
#endif	
}

void
SVGPathSegLinetoVerticalRel::updateBezier(agg::path_storage& b) const
{
#if USEAGG
	DBGMETH(SVGPathSegLinetoVerticalRel);
	double x;
	double y;
	//if( !b.current_point(x, y)) {DBG}
	unsigned int tv = b.total_vertices();
	if(!tv) {DBG; return;}
	b.vertex(tv-1, &x, &y);
	b.line_to(x, y+GET_SIMPLE_VAL(Y));
#endif	
}

void
SVGPathSegMovetoAbs::updateBezier(agg::path_storage& b) const
{
#if USEAGG
	DBGMETH(SVGPathSegMovetoAbs);
	b.move_to(GET_SIMPLE_VAL(X), GET_SIMPLE_VAL(Y));
#endif	
}

void
SVGPathSegMovetoRel::updateBezier(agg::path_storage& b) const
{
#if USEAGG
	DBGMETH(SVGPathSegMovetoRel);
	double x=0;
	double y=0;
	unsigned int tv = b.total_vertices();
	if(tv<1) { x=0; y=0; }
	else b.vertex(tv-1, &x, &y);
	//b.rel_to_abs(&x,&y);
	/*
	if( !b.current_point(x, y)) {
		x=0;
		y=0;
	}
	 */
	b.move_to(x+GET_SIMPLE_VAL(X), y+GET_SIMPLE_VAL(Y));
#endif	
}


	
}
