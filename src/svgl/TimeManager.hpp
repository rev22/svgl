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
#ifndef __TimeManager__
#define __TimeManager__

#include <svgl/config.hpp>
#include <list>

namespace svgl {

  namespace Time {

    typedef float Unit;

    class Manager;
    class Timer;

    class Manager {
    public:
      Manager(float precision=0);
      virtual ~Manager() {}

      struct exc {
      };
      struct TimerAlreadyScheduled : exc {
      };

      void after(Timer* timer, Unit) throw(TimerAlreadyScheduled);
      void cancel(Timer* timer);
      void timeElapsed(Unit);

    protected:
      virtual void firstTimerHasChanged()=0;
      virtual void reschedule()=0;

      typedef std::list<Timer*> Timers;
      const Timers& getTimers() const { return _timers; }

    private:
      Timers _timers;
      Unit _precision;
      bool _dontCallTimerHasChanged;
    };

    class Timer {
    public:
      virtual ~Timer() {}
      virtual void doit(const Unit& actualtime)=0;
      Unit getTime() const { return _time; }
      Unit getDelta() const { return _delta; }

    private:
      Unit _time;
      Unit _delta;
      void setTime(Unit d) { _time=d; }
      void setDelta(Unit d) { _delta=d; }

      friend class Manager;

    };
  }

}

#endif // __TimeManager__

