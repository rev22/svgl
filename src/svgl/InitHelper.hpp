#ifndef __svgl_InitHelper__
#define __svgl_InitHelper__

//#include <svgl/AnimationManager.hpp>

namespace glft {
  class FontManager;
}

namespace svgl {

  class GLInfo;
  class Context;
  class AnimationInfo;

  namespace Time {
    class Manager;
  }
	
#if 1
  namespace Animation {
    class Manager;
#if 0
    template <typename EVENT> class EventListener;
#else
		class BeginEventListener;
		class EndEventListener;
		class RedisplayEventListener;
#endif
  }
#endif
	
  class DisplayManager;
  class PickManager;
  class ExternalEntityManager;
  class ImageManager;


  class InitHelper {
  private:
    void init();
    static InitHelper * _default;

  public:
    InitHelper();
		
    InitHelper(Time::Manager*, Animation::RedisplayEventListener * redisplay = 0, Animation::BeginEventListener * begin = 0, Animation::EndEventListener* end = 0);

    static InitHelper * get();
    static InitHelper * get(Time::Manager*);
#if 0
    static InitHelper * get(Time::Manager*, Animation::EventListener<Animation::RedisplayEvent>* redisplay = 0, Animation::EventListener<Animation::BeginEvent>* begin = 0, Animation::EventListener<Animation::EndEvent>* end = 0);
#else
		static InitHelper * get(Time::Manager*, Animation::RedisplayEventListener* redisplay = 0, Animation::BeginEventListener* begin = 0, Animation::EndEventListener* end = 0);
#endif
		
  public:
    GLInfo* glinfo;
    Context* context;
    AnimationInfo* animinfo;
    Time::Manager* timeManager;
    Animation::Manager* animationManager;
#if 0
    Animation::EventListener<Animation::RedisplayEvent> * redisplayListener;
		Animation::EventListener<Animation::BeginEvent>* beginListener;
		Animation::EventListener<Animation::EndEvent>* endListener;
#else
    Animation::RedisplayEventListener * redisplayListener;
		Animation::BeginEventListener * beginListener;
		Animation::EndEventListener * endListener;
#endif
		
    glft::FontManager* fontManager;
    DisplayManager* displayManager;
    PickManager* pickManager;
    ExternalEntityManager* externalEntityManager;
    ImageManager* imageManager;
  };


}

#endif // __svgl_InitHelper__
