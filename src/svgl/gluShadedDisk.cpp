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
the file licence.txt and at the following URL:
http://www.emn.fr/info/image/Themes/Indigo/licence.html

 */
/*
** this file is based on SGI's glu implementation
*/

/*
** License Applicability. Except to the extent portions of this file are
** made subject to an alternative license as permitted in the SGI Free
** Software License B, Version 1.1 (the "License"), the contents of this
** file are subject only to the provisions of the License. You may not use
** this file except in compliance with the License. You may obtain a copy
** of the License at Silicon Graphics, Inc., attn: Legal Services, 1600
** Amphitheatre Parkway, Mountain View, CA 94043-1351, or at:
** 
** http://oss.sgi.com/projects/FreeB
** 
** Note that, as provided in the License, the Software is distributed on an
** "AS IS" basis, with ALL EXPRESS AND IMPLIED WARRANTIES AND CONDITIONS
** DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY IMPLIED WARRANTIES AND
** CONDITIONS OF MERCHANTABILITY, SATISFACTORY QUALITY, FITNESS FOR A
** PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
** 
** Original Code. The Original Code is: OpenGL Sample Implementation,
** Version 1.2.1, released January 26, 2000, developed by Silicon Graphics,
** Inc. The Original Code is Copyright (c) 1991-2000 Silicon Graphics, Inc.
** Copyright in any portions created by third parties is as indicated
** elsewhere herein. All Rights Reserved.
** 
** Additional Notice Provisions: The application programming interfaces
** established by SGI in conjunction with the Original Code are The
** OpenGL(R) Graphics System: A Specification (Version 1.2.1), released
** April 1, 1999; The OpenGL(R) Graphics System Utility Library (Version
** 1.3), released November 4, 1998; and OpenGL(R) Graphics with the X
** Window System(R) (Version 1.3), released October 19, 1998. This software
** was created using the OpenGL(R) version 1.2.1 Sample Implementation
** published by SGI, but has not been independently verified as being
** compliant with the OpenGL(R) version 1.2.1 Specification.
**
*/


#include <svgl/gluShadedDisk.hpp>

#include <cmath>

namespace svgl {

  static void gluQuadricError(GLUquadric *qobj, GLenum which)
  {
    if (qobj->errorCallback) {
      qobj->errorCallback(which);
    }
  }

  void
  gluShadedDisk(GLUquadric *qobj, GLdouble innerRadius, GLdouble* innerColor, GLdouble outerRadius, GLdouble* outerColor,
	    GLint slices, GLint loops)
{
    gluShadedPartialDisk(qobj, innerRadius, innerColor, outerRadius, outerColor, slices, loops, 0.0, 360.0);
}

  void
  gluShadedPartialDisk(GLUquadric *qobj, GLdouble innerRadius, GLdouble* innerColor, 
		       GLdouble outerRadius, GLdouble* outerColor, GLint slices, GLint loops,
		       GLdouble startAngle, GLdouble sweepAngle)
{
    GLint i,j;
    //GLint max;
    //#define CACHE_SIZE	240
    const int CACHE_SIZE=240;
    GLfloat sinCache[CACHE_SIZE];
    GLfloat cosCache[CACHE_SIZE];
    GLfloat angle;
    //    GLfloat x, y;
    GLfloat sintemp, costemp;
    GLfloat deltaRadius;
    GLfloat radiusLow, radiusHigh;
    GLfloat texLow=0, texHigh=0;
    GLfloat angleOffset;
    GLint slices2;
    GLint finish;

    if (slices >= CACHE_SIZE) slices = CACHE_SIZE-1;
    if (slices < 2 || loops < 1 || outerRadius <= 0.0 || innerRadius < 0.0 ||
	    innerRadius > outerRadius) {
	gluQuadricError(qobj, GLU_INVALID_VALUE);
	return;
    }

    if (sweepAngle < -360.0) sweepAngle = 360.0;
    if (sweepAngle > 360.0) sweepAngle = 360.0;
    if (sweepAngle < 0) {
	startAngle += sweepAngle;
	sweepAngle = -sweepAngle;
    }

    if (sweepAngle == 360.0) {
	slices2 = slices;
    } else {
	slices2 = slices + 1;
    }

    /* Compute length (needed for normal calculations) */
    deltaRadius = outerRadius - innerRadius;

    /* Cache is the vertex locations cache */

    angleOffset = startAngle / 180.0 * M_PI;
    for (i = 0; i <= slices; i++) {
	angle = angleOffset + ((M_PI * sweepAngle) / 180.0) * i / slices;
	sinCache[i] = sin(angle);
	cosCache[i] = cos(angle);
    }

    if (sweepAngle == 360.0) {
	sinCache[slices] = sinCache[0];
	cosCache[slices] = cosCache[0];
    }

    switch(qobj->normals) {
      case GLU_FLAT:
      case GLU_SMOOTH:
	if (qobj->orientation == GLU_OUTSIDE) {
	    glNormal3f(0.0, 0.0, 1.0);
	} else {
	    glNormal3f(0.0, 0.0, -1.0);
	}
	break;
      default:
      case GLU_NONE:
	break;
    }

    switch (qobj->drawStyle) {
      case GLU_FILL:
	if (innerRadius == 0.0) {

	    finish = loops - 1;
	    /* Triangle strip for inner polygons */
	    glBegin(GL_TRIANGLE_FAN);
	    if (qobj->textureCoords) {
		glTexCoord2f(0.5, 0.5);
	    }
	    glColor4dv(innerColor);
	    glVertex3f(0.0, 0.0, 0.0);
	    glColor4dv(outerColor);
	    radiusLow = outerRadius - 
		    deltaRadius * ((float) (loops-1) / loops);
	    if (qobj->textureCoords) {
		texLow = radiusLow / outerRadius / 2;
	    }

	    if (qobj->orientation == GLU_OUTSIDE) {
		for (i = slices; i >= 0; i--) {
		    if (qobj->textureCoords) {
			glTexCoord2f(texLow * sinCache[i] + 0.5,
				texLow * cosCache[i] + 0.5);
		    }
		    glVertex3f(radiusLow * sinCache[i], 
			    radiusLow * cosCache[i], 0.0);
		}
	    } else {
		for (i = 0; i <= slices; i++) {
		    if (qobj->textureCoords) {
			glTexCoord2f(texLow * sinCache[i] + 0.5,
				texLow * cosCache[i] + 0.5);
		    }
		    glVertex3f(radiusLow * sinCache[i], 
			    radiusLow * cosCache[i], 0.0);
		}
	    }
	    glEnd();
	} else {
	    finish = loops;
	}
	for (j = 0; j < finish; j++) {

	    radiusLow = outerRadius - deltaRadius * ((float) j / loops);
	    radiusHigh = outerRadius - deltaRadius * ((float) (j + 1) / loops);
	    if (qobj->textureCoords) {
		texLow = radiusLow / outerRadius / 2;
		texHigh = radiusHigh / outerRadius / 2;
	    }

	    glBegin(GL_QUAD_STRIP);
	    for (i = 0; i <= slices; i++) {
		if (qobj->orientation == GLU_OUTSIDE) {
		    if (qobj->textureCoords) {
			glTexCoord2f(texLow * sinCache[i] + 0.5,
				texLow * cosCache[i] + 0.5);
		    }
		    glColor4dv(outerColor);
		    glVertex3f(radiusLow * sinCache[i], 
			    radiusLow * cosCache[i], 0.0);

		    if (qobj->textureCoords) {
			glTexCoord2f(texHigh * sinCache[i] + 0.5,
				texHigh * cosCache[i] + 0.5);
		    }
		    glColor4dv(innerColor);
		    glVertex3f(radiusHigh * sinCache[i], 
			    radiusHigh * cosCache[i], 0.0);
		} else {
		    if (qobj->textureCoords) {
			glTexCoord2f(texHigh * sinCache[i] + 0.5,
				texHigh * cosCache[i] + 0.5);
		    }
		    glColor4dv(innerColor);
		    glVertex3f(radiusHigh * sinCache[i], 
			    radiusHigh * cosCache[i], 0.0);

		    if (qobj->textureCoords) {
			glTexCoord2f(texLow * sinCache[i] + 0.5,
				texLow * cosCache[i] + 0.5);
		    }
		    glColor4dv(outerColor);
		    glVertex3f(radiusLow * sinCache[i], 
			    radiusLow * cosCache[i], 0.0);
		}
	    }
	    glEnd();
	}
	break;
      case GLU_POINT:
	glBegin(GL_POINTS);
	for (i = 0; i < slices2; i++) {
	    sintemp = sinCache[i];
	    costemp = cosCache[i];
	    for (j = 0; j <= loops; j++) {
		radiusLow = outerRadius - deltaRadius * ((float) j / loops);

		if (qobj->textureCoords) {
		    texLow = radiusLow / outerRadius / 2;

		    glTexCoord2f(texLow * sinCache[i] + 0.5,
			    texLow * cosCache[i] + 0.5);
		}
		glVertex3f(radiusLow * sintemp, radiusLow * costemp, 0.0);
	    }
	}
	glEnd();
	break;
      case GLU_LINE:
	if (innerRadius == outerRadius) {
	    glBegin(GL_LINE_STRIP);

	    for (i = 0; i <= slices; i++) {
		if (qobj->textureCoords) {
		    glTexCoord2f(sinCache[i] / 2 + 0.5,
			    cosCache[i] / 2 + 0.5);
		}
		glVertex3f(innerRadius * sinCache[i],
			innerRadius * cosCache[i], 0.0);
	    }
	    glEnd();
	    break;
	}
	for (j = 0; j <= loops; j++) {
	    radiusLow = outerRadius - deltaRadius * ((float) j / loops);
	    if (qobj->textureCoords) {
		texLow = radiusLow / outerRadius / 2;
	    }

	    glBegin(GL_LINE_STRIP);
	    for (i = 0; i <= slices; i++) {
		if (qobj->textureCoords) {
		    glTexCoord2f(texLow * sinCache[i] + 0.5,
			    texLow * cosCache[i] + 0.5);
		}
		glVertex3f(radiusLow * sinCache[i], 
			radiusLow * cosCache[i], 0.0);
	    }
	    glEnd();
	}
	for (i=0; i < slices2; i++) {
	    sintemp = sinCache[i];
	    costemp = cosCache[i];
	    glBegin(GL_LINE_STRIP);
	    for (j = 0; j <= loops; j++) {
		radiusLow = outerRadius - deltaRadius * ((float) j / loops);
		if (qobj->textureCoords) {
		    texLow = radiusLow / outerRadius / 2;
		}

		if (qobj->textureCoords) {
		    glTexCoord2f(texLow * sinCache[i] + 0.5,
			    texLow * cosCache[i] + 0.5);
		}
		glVertex3f(radiusLow * sintemp, radiusLow * costemp, 0.0);
	    }
	    glEnd();
	}
	break;
      case GLU_SILHOUETTE:
	if (sweepAngle < 360.0) {
	    for (i = 0; i <= slices; i+= slices) {
		sintemp = sinCache[i];
		costemp = cosCache[i];
		glBegin(GL_LINE_STRIP);
		for (j = 0; j <= loops; j++) {
		    radiusLow = outerRadius - deltaRadius * ((float) j / loops);

		    if (qobj->textureCoords) {
			texLow = radiusLow / outerRadius / 2;
			glTexCoord2f(texLow * sinCache[i] + 0.5,
				texLow * cosCache[i] + 0.5);
		    }
		    glVertex3f(radiusLow * sintemp, radiusLow * costemp, 0.0);
		}
		glEnd();
	    }
	}
	for (j = 0; j <= loops; j += loops) {
	    radiusLow = outerRadius - deltaRadius * ((float) j / loops);
	    if (qobj->textureCoords) {
		texLow = radiusLow / outerRadius / 2;
	    }

	    glBegin(GL_LINE_STRIP);
	    for (i = 0; i <= slices; i++) {
		if (qobj->textureCoords) {
		    glTexCoord2f(texLow * sinCache[i] + 0.5,
			    texLow * cosCache[i] + 0.5);
		}
		glVertex3f(radiusLow * sinCache[i], 
			radiusLow * cosCache[i], 0.0);
	    }
	    glEnd();
	    if (innerRadius == outerRadius) break;
	}
	break;
      default:
	break;
    }
}

}
