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
#ifndef __GlutTimeManager__
#define __GlutTimeManager__

#include <svgl/TimeManager.hpp>

namespace GlutSvgl {
  class GlutTimeManager : public svgl::Time::Manager {
  public:
    GlutTimeManager(float precision=0);
  protected:
    virtual void firstTimerHasChanged();
    virtual void reschedule();
    
  private:
    static GlutTimeManager * theManager;
    void glutTimerCallback(int);
    static void glutTimerCallbackStatic(int);

    int _start;
    int _timerid;
    bool _inuse;
  };

}

#endif // __GlutTimeManager__
