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
#include <svgl/AnimationManager.hpp>
#include <w3c/svg/SVGAnimationElement.hpp>
#include <utils/StringUtils.hpp>
#include <svgl/timeval_helpers.hpp>

#include <algorithm>

#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>

#if defined(WIN32) && ! defined(__CYGWIN__)
#include <sys/timeb.h>

int
gettimeofday(struct timeval* tp, void* tzp)
{
  LARGE_INTEGER t;
	
  if(QueryPerformanceCounter(&t)) {
		/* hardware supports a performance counter */
		LARGE_INTEGER f;
		QueryPerformanceFrequency(&f);
		tp->tv_sec = t.QuadPart/f.QuadPart;
		tp->tv_usec = ((float)t.QuadPart/f.QuadPart*1000*1000) - (tp->tv_sec*1000*1000);
  } else {
		/* hardware doesn't support a performance counter,
		so get the time in a more traditional way. */
		DWORD t;
		t = timeGetTime();
		tp->tv_sec = t / 1000;
		tp->tv_usec = t % 1000;
  }
	
  /* 0 indicates that the call succeeded. */
  return 0;
}
#endif


inline
std::ostream&
operator<<(std::ostream& out, const svgl::Animation::Manager::TimeInfo& t)
{
  out << "(" << DBGVAR(t.begin.time) << DBGVAR(t.end.time) << ")";
  return out;
}

namespace svgl {
	
  namespace Animation {
		
    bool
    get_clock_value(const unicode::String* s, ClockValue* clock,
										unicode::String::size_type beg=0,
										unicode::String::size_type end=0,
										unicode::String::size_type* after=0)
	{
      unicode::String::size_type len=s->getLength();
      if (beg>=len)
				return false;
      if(end>len)
				end=len;
      
      static const unicode::String::char_type doublecolumn = ':';
      static const unicode::String
				*hour_string_ptr = unicode::String::internString("h"),
				*minute_string_ptr = unicode::String::internString("min"),
				*second_string_ptr = unicode::String::internString("second"),
				*millisecond_string_ptr = unicode::String::internString("ms");
			
      const unicode::String
				&hour_string = *hour_string_ptr,
				&minute_string = *minute_string_ptr,
				&second_string = *second_string_ptr,
				&millisecond_string = *millisecond_string_ptr;
			
      unicode::String::size_type pos = s->indexOf(doublecolumn);
      unicode::String::size_type after_;
			
      if(pos!=unicode::String::npos) {
				// absolute
				std::cerr << "parsing of absolute clock value is not implemented" << __FL__;
				return false;
      }
      else {
				// relative
				float value;
				bool res = unicode::get_float(*s, &value, beg, end, &after_);
				if(!res)
					return false;
				
				float fact=1;
				
				if (after_<end) {
					unicode::String *unit_ptr = s->substring(after_, end);
					unicode::String& unit = *unit_ptr;
					
#define HANDLEUNIT(unitstring, f) \
					if(unit==unitstring##_string) { \
						/*std::cerr << value << unitstring##_string << __FL__;*/ \
						fact = f; \
					} \
						else
							
							HANDLEUNIT(hour, 3600)
								HANDLEUNIT(minute, 60)
								HANDLEUNIT(second, 1)
								HANDLEUNIT(millisecond, .001)
								
#undef HANDLEUNIT
								
								// else
							{
									STDDBG;
									return false;
							}
					
				}
				
				// std::cerr << "in seconds: " << value*fact << __FL__;
				clock->relative=true;
				clock->time=value*fact;
				if(after)
					*after=end;
      }
      
      return true;
	}
		
    Manager::TimeInfo::TimeInfo(const ClockValue& b, const ClockValue& e, svg::SVGAnimationElement* elem) :
		originalBegin(b), originalEnd(e), begin(b), end(e), isScheduled(false), animationElement(elem),
		_onBeginElement(0), _onEndElement()
	{
	}
    
    
    bool
    Manager::TimeInfo::beginOnEvent()
	{
      return false;
	}
		
    bool
    Manager::TimeInfo::endOnEvent()
	{
      return false;
	}
    
    void
    Manager::TimeInfo::reset()
	{
      begin = originalBegin;
      end = originalEnd;
      isScheduled=false;
      //      animationElement->setActive(false);
	}
		
    struct TimeInfoCompareBegin {
      TimeInfoCompareBegin(const Manager::TimeInfo& i) : _info(i) {}
      const Manager::TimeInfo& _info;
      bool operator()(const Manager::TimeInfo* t) {
				return _info.begin.time>t->begin.time;
      }
    };
		
    struct TimeInfoCompareEnd {
      TimeInfoCompareEnd(const Manager::TimeInfo& i) : _info(i) {}
      const Manager::TimeInfo& _info;
      bool operator()(const Manager::TimeInfo* t) {
				return _info.end.time>t->end.time;
      }
    };
		
		
    Manager::Manager(Time::Manager* timeManager, float precision)
      : _timeManager(timeManager), _precision(precision), _started(false), _paused(false), _totalTime(0), _redisplayListener(0), _beginListener(0), _endListener(0)
    {
    }
		
    void
			Manager::subscribe(svg::SVGAnimationElement* elem)
    {
				const unicode::String* begin = elem->GET_SIMPLE_VAL(Begin);
				const unicode::String* end = elem->GET_SIMPLE_VAL(End);
				const unicode::String* dur = elem->GET_SIMPLE_VAL(Dur);
				// std::cerr << DBGVAR(begin) << DBGVAR(end) << DBGVAR(dur) << __FL__;
				ClockValue beginClock, endClock, durClock;
				bool beginb=false, endb=false, durb=false;
				if(begin)
					beginb = get_clock_value(begin, &beginClock);
				if(dur)
					durb = get_clock_value(dur, &durClock);
				if(end)
					endb = get_clock_value(end, &endClock);
				
				if( !(durb||endb) ) {
					std::cerr << "invalid animation 'dur' and 'end' attributes" << __FL__;
				}
				
				if(!endb) {
					endClock.time=beginClock.time+durClock.time;
				}
				
#if 0
				beginClock.time += _totalTime;
				endClock.time += _totalTime;
#endif
				
				TimeInfo t(beginClock, endClock, elem);
				//std::cerr << t << __FL__;
				
				//      elem->setActive(false);
				_animationElements[elem] = t;
    }
		
    void
			Manager::unsubscribe(svg::SVGAnimationElement* elem)
    {
    }
		
    void
			Manager::unsubscribe_all()
    {
				stop();
				_animationElements.clear();
    }
		
    void
			Manager::start()
    {
				if(_started) {
					stop();
				}
				
				if(_animationElements.empty())
					return;
				
				for(AnimationElements::iterator i=_animationElements.begin();
						i!=_animationElements.end();
						++i) {
					schedule(&(i->second));
				}
				
				_totalTime = 0;
				_nextDelay = _scheduledElements.back()->begin.time;
#if 0
				std::cerr << __FL__;
				for(ScheduledElements::iterator i=_scheduledElements.begin(); i!=_scheduledElements.end(); ++i) {
					std::cerr << **i << " ";
				}
				std::cerr << DBGVAR(_nextDelay) << __FL__;
#endif
				_started=true;
				_paused=false;
				_timeManager->after(this, _nextDelay);
				gettimeofday(&_lastRestartTime, 0);
    }
		
    void
			Manager::schedule(TimeInfo *info)
    {
				ScheduledElements::iterator i =  std::find_if(_scheduledElements.begin(), _scheduledElements.end(), TimeInfoCompareBegin(*info));
				if(i!=_scheduledElements.end()) {
					_scheduledElements.insert(i, info);
				}
				else {
					_scheduledElements.push_back(info);
				}
				//      info->animationElement->setActive(true);
    }
		
    void
			Manager::activate(TimeInfo *info)
    {
				ActiveElements::iterator i =  std::find_if(_activeElements.begin(), _activeElements.end(), TimeInfoCompareEnd(*info));
				if(i!=_activeElements.end()) {
					_activeElements.insert(i, info);
				}
				else {
					_activeElements.push_back(info);
				}
				info->animationElement->started(this);
    }
		
    void
			Manager::stop()
    {
				for(ActiveElements::iterator i= _activeElements.begin(); i!=_activeElements.end(); ++i) {
					(*i)->reset();
				}
				_activeElements.clear();
				for(ScheduledElements::iterator j= _scheduledElements.begin(); j!=_scheduledElements.end(); ++j) {
					(*j)->reset();
				}
				_scheduledElements.clear();
				
				_started=false;
				_timeManager->cancel(this);
    }
		
    void
			Manager::pause()
    {
				if(_paused)
					return;
				_paused=true;
				timeval now;
				gettimeofday(&now, 0);
				timeval diff = now-_lastRestartTime;
				float delay = diff.tv_sec + .000001*diff.tv_usec;
				_totalTime+=delay;
				_nextDelay-=delay;
				_timeManager->cancel(this);
    }
		
    inline
			void
			tick(const Manager::TimeInfo* t, svgl::Time::Unit totalTime, svgl::Time::Unit lastElapsed)
    {
				TickInfo info;
				info.dur = t->end.time-t->begin.time;
				info.totalElapsed = totalTime - t->begin.time;
				if(info.totalElapsed>info.dur)
					info.totalElapsed=info.dur;
				else if (info.totalElapsed<0)
					info.totalElapsed=0;
				info.lastElapsed = lastElapsed;
				t->animationElement->tick(&info);
    }
		
    void
			Manager::continue_()
    {
				if(!_paused)
					return;
#if 0
				std::cerr << DBGVAR(now.tv_sec) << DBGVAR(now.tv_usec) << __FL__;
				std::cerr << DBGVAR(_pauseTime.tv_sec) << DBGVAR(_pauseTime.tv_usec) << __FL__;
				std::cerr << DBGVAR(diff.tv_sec) << DBGVAR(diff.tv_usec) << __FL__;
				std::cerr << DBGVAR(_nextDelay) << std::endl;
#endif
				
				_timeManager->after(this, _nextDelay);
				gettimeofday(&_lastRestartTime, 0);
    }
		
    // Timer interface
    void
			Manager::doit(const Time::Unit& actualtime)
    {
				svgl::Time::Unit delay;
				if(_paused) {
					// if it's paused and doit() is called
					// then continue_() has been called
					// hence actualtime is not the next delay to add
					// to _totalTime
					delay = _nextDelay;
					_paused=false;
				}
				else {
					delay = actualtime;
				}
				_totalTime += delay;
				// std::cerr << DBGVAR(_totalTime) << DBGVAR(actualtime) << std::endl;
				
				bool redisplay=false;
				if(!_activeElements.empty())
					redisplay=true;

				// trigger active
				for(ActiveElements::iterator i=_activeElements.begin();
						i!=_activeElements.end();
						++i) {
					tick(*i, _totalTime, delay);
				}

				if(redisplay && _redisplayListener) {
					RedisplayEvent e;
					//std::cerr << DBGVAR(_redisplayListener) << __FL__;
					_redisplayListener->doit(e);
				}
				
				// insert scheduled and trigger them
				if(!_scheduledElements.empty()) {
					TimeInfo* t = _scheduledElements.back();
					//std::cerr << *t << DBGVAR(_totalTime+_precision) << __FL__;
					while (t->begin.time < _totalTime+_precision) {
						if(_beginListener) {
							BeginEvent e;
							e.animationElement = t->animationElement;
							_beginListener->doit(e);
						}
						activate(t);
						// lastElapsed is 0
						tick(t, _totalTime, 0);
						_scheduledElements.pop_back();
						if(_scheduledElements.empty())
							break;
						t = _scheduledElements.back();
					}
				}
				
				// remove finished
				if(!_activeElements.empty()) {
					TimeInfo* t = _activeElements.back();
					while (t->end.time < _totalTime+_precision) {
						_activeElements.pop_back();
						bool reschedule = t->animationElement->finished(this);
						if (reschedule) {
							t->begin.time = _totalTime + t->originalBegin.time;
							t->end.time = _totalTime + t->originalEnd.time;
							schedule(t);
						}
						else {
							t->reset();
						}
						if(_endListener) {
							EndEvent e;
							e.animationElement = t->animationElement;
							_endListener->doit(e);
						}
						if(_activeElements.empty())
							break;
						t = _activeElements.back();
					}
				}
				
				// reschedule a timer
				if(!_activeElements.empty()) {
					_nextDelay = 1/30.; // 30 images/seconde
					_timeManager->after(this, _nextDelay);
				}
				else {
					if(!_scheduledElements.empty()) {
						_nextDelay = _scheduledElements.back()->begin.time-_totalTime;
#if 0
						std::cerr << __FL__;
						for(ScheduledElements::iterator i=_scheduledElements.begin(); i!=_scheduledElements.end(); ++i) {
							std::cerr << **i << " ";
						}
						std::cerr << DBGVAR(_totalTime) << DBGVAR(_nextDelay) << __FL__;
#endif
						_timeManager->after(this, _nextDelay);
					}
					else {
						stop();
					}
				}
    }
		
		
		void
			Manager::testRedisplay()
		{
				RedisplayEvent e;
				_redisplayListener->doit(e);
		}
  } // namespace Animation
	
} // namespace svgl

