#ifndef __svgl_InitHelper__
#define __svgl_InitHelper__

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

  namespace Animation {
    class Manager;
    class RedisplayListener;
  }

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
    InitHelper(Time::Manager*);
    InitHelper(Time::Manager*, Animation::RedisplayListener*);

    static InitHelper * get();
    static InitHelper * get(Time::Manager*);
    static InitHelper * get(Time::Manager*, Animation::RedisplayListener*);

  public:
    GLInfo* glinfo;
    Context* context;
    AnimationInfo* animinfo;
    Time::Manager* timeManager;
    Animation::Manager* animationManager;
    Animation::RedisplayListener * redisplayListener;
    glft::FontManager* fontManager;
    DisplayManager* displayManager;
    PickManager* pickManager;
    ExternalEntityManager* externalEntityManager;
    ImageManager* imageManager;
  };


}

#endif // __svgl_InitHelper__
