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
#ifndef __svgl_FakeTimeManager__
#define __svgl_FakeTimeManager__

#include <svgl/TimeManager.hpp>

namespace svgl {
  class FakeTimeManager : public svgl::Time::Manager {
  public:
    FakeTimeManager(float precision=0);
  protected:
    virtual void firstTimerHasChanged();
    virtual void reschedule();
  };

}

#endif // __svgl_FakeTimeManager__
