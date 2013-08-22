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
#include "GlutTimeManager.hpp"
#include <svgl/timeval_helpers.hpp>
#if defined (__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <iostream>

#define DBG std::cerr << __FILE__ ":" << __LINE__ << std::endl;
#define DBGVAL(x) #x ": " << x << " "
#define __FL__ " " __FILE__ ":" << __LINE__ << std::endl;

//#define DBGMETH std::cerr << METHODNAME << __FL__;

namespace GlutSvgl {

  GlutTimeManager* GlutTimeManager::theManager;

  int
  mygettimeofday()
  {
#if 0
      timeval t;
      gettimeofday(&t,0);
      int res = t.tv_sec * 1000 + t.tv_usec / 1000;
#else
      int res = glutGet(GLUT_ELAPSED_TIME);
#endif
      //std::cerr << DBGVAL(res) << __FL__;
      return res;
  }

  GlutTimeManager::GlutTimeManager(float precision)
    : svgl::Time::Manager(precision), _timerid(1), _inuse(false)
  {
    theManager=this;
  }

  //#define METHODNAME "GlutTimeManager::firstTimerHasChanged"

  void
  GlutTimeManager::firstTimerHasChanged()
  {
    //DBGMETH;
    if(getTimers().empty()) {
      //DBG;
      if (_inuse) {
	++_timerid;
	_inuse = false;
      }
      return;
    }

    Timers::const_iterator i = getTimers().begin();
    int ms = static_cast<int>((*i)->getDelta()*1000.);
    //std::cerr << DBGVAL(ms) << __FL__;
    if(_inuse) {
      // FIXME
      ++_timerid;
    }
    _inuse=true;
    //DBG;
    _start = mygettimeofday();
    glutTimerFunc(ms, glutTimerCallbackStatic, _timerid);
  }

  //#define METHODNAME "GlutTimeManager::reschedule"
  void
  GlutTimeManager::reschedule()
  {
    //DBGMETH;
    if(getTimers().empty()) {
      if (_inuse) {
	++_timerid;
	_inuse = false;
      }
      return;
    }
    if(_inuse) {
      // FIXME
      ++_timerid;
    }
    _inuse=true;

    Timers::const_iterator i = getTimers().begin();
    int ms = static_cast<int>((*i)->getDelta()*1000.);
    //std::cerr << DBGVAL(ms) << __FL__;
    //DBG;
    _start = mygettimeofday();
    glutTimerFunc(ms, glutTimerCallbackStatic, _timerid);    
  }

#define METHODNAME "GlutTimeManager::gluttimerCallback"

  void
  GlutTimeManager::glutTimerCallback(int timerid)
  {
    //DBGMETH;
    //std::cerr << DBGVAL(timerid) << DBGVAL(_timerid) << __FL__;
    if(timerid==_timerid) {
      int end;
      //DBG;
      end = mygettimeofday();
      int diff = end-_start;
      svgl::Time::Unit tmp = diff * 0.001;
      //std::cerr << DBGVAL(tmp) << __FL__;
      timeElapsed(tmp);
    }
  }

  void
  GlutTimeManager::glutTimerCallbackStatic(int timerid)
  {
    theManager->glutTimerCallback(timerid);
  }

}
