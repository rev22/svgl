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
#ifndef __AnimationManager__
#define __AnimationManager__

#include <svgl/config.hpp>
#include <svgl/TimeManager.hpp>
#include <vector>
//#include <queue>
#include <map>


// timeval
#ifdef WIN32
#include <windows.h>
#include <sys/time.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif


namespace svg {
  class SVGAnimationElement;
}

namespace svgl {

  namespace Animation {

    class Manager;

    struct ClockValue {
      ClockValue() : relative(true), time(0) {}
      ClockValue(int) : relative(true), time(0) {} // for map 
      bool relative;
      svgl::Time::Unit time;
    };

    struct TickInfo {
      svgl::Time::Unit dur, totalElapsed, lastElapsed;
    };

    struct Event {
    };
    
    struct RedisplayEvent : Event {
    };
    
    struct BeginEvent : Event {
    };
    
    struct RedisplayListener {
      virtual void doit(const RedisplayEvent& e)=0;
    };

    class Manager : public svgl::Time::Timer {
    public:
      Manager(Time::Manager* timeManager, float precision);
      
      void subscribe(svg::SVGAnimationElement*);
      void unsubscribe(svg::SVGAnimationElement*);
      void unsubscribe_all();

      void start();
      void stop();
      void pause();
      void continue_();

      // Timer interface
      void doit(const Time::Unit& actualtime);

      struct TimeInfo {
	TimeInfo(const ClockValue& b=0, const ClockValue& e=0, svg::SVGAnimationElement* elem=0);
	ClockValue originalBegin;
	ClockValue originalEnd;
	ClockValue begin;
	ClockValue end;
	bool isScheduled;
	svg::SVGAnimationElement* animationElement;
	svg::SVGAnimationElement *_onBeginElement, *_onEndElement;
	void reset();
	bool beginOnEvent();
	bool endOnEvent();
      };

    private:

      Time::Manager* _timeManager;
      float _precision;
      bool _started;
      bool _paused;
      timeval _lastRestartTime;
      svgl::Time::Unit _nextDelay, _totalTime;

      typedef std::map<svg::SVGAnimationElement*, TimeInfo> AnimationElements;
      typedef std::vector<TimeInfo*> ScheduledElements;
      typedef ScheduledElements ActiveElements;

      AnimationElements _animationElements;

      ScheduledElements _scheduledElements;
      ActiveElements _activeElements;

      typedef std::vector< svg::SVGAnimationElement* > ToWarnList;
      typedef std::map< svg::SVGAnimationElement*,  ToWarnList > ToWarn;

      ToWarn _warnOnBegin;
      ToWarn _warnOnEnd;

      ScheduledElements::iterator _nextActiveCursor;
      bool _scheduledElementsHasChanged;

      void schedule(TimeInfo*);
      void activate(TimeInfo*);

    public:
      void setRedisplayListener(RedisplayListener * l) { _redisplayListener = l; }

    private:
      RedisplayListener * _redisplayListener;
    };

  }

}

#endif // __AnimationManager__
