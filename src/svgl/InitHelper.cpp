#include <svgl/InitHelper.hpp>

#include <svgl/GLInfo.hpp>
#include <svgl/Context.hpp>
#include <svgl/TimeManager.hpp>
#include <svgl/FakeTimeManager.hpp>
#include <svgl/AnimationManager.hpp>
#include <svgl/AnimationInfo.hpp>
#include <glft/FontManager.hpp>
#include <svgl/DisplayManager.hpp>
#include <svgl/PickManager.hpp>
#include <svgl/ExternalEntityManager.hpp>
#include <svgl/ImageManager.hpp>

#include <w3c/css/CSSStyleDeclaration.hpp>
#include <w3c/svg/strings.hpp>

#if SVGL_USE_GC
extern int GC_dont_gc;
#endif

namespace svg {
  extern
  void
  init_strings();

  extern
  void
  init_colors();
}


namespace svgl {

  InitHelper * InitHelper::_default = 0;

  InitHelper *
  InitHelper::get()
  {
    if( !_default) {
      _default = new InitHelper();
    }
    return _default;
  }

  InitHelper *
  InitHelper::get(Time::Manager* t)
  {
    if( !_default) {
      _default = new InitHelper(t);
    }
    return _default;
  }

  InitHelper *
  InitHelper::get(Time::Manager* t, Animation::RedisplayListener* l)
  {
    if( !_default) {
      _default = new InitHelper(t,l);
    }
    return _default;
  }

  InitHelper::InitHelper()
    : timeManager(new FakeTimeManager), redisplayListener(0)
  {
    init();
  }

  InitHelper::InitHelper(Time::Manager* t)
    : timeManager(t), redisplayListener(0)
  {
    init();
  }

  InitHelper::InitHelper(Time::Manager* t, Animation::RedisplayListener* l)
    : timeManager(t), redisplayListener(l)
  {
    init();
  }


  void
  InitHelper::init()
  {
#if SVGL_USE_GC
    GC_dont_gc=1;
#endif

    css::CSSStyle::init();
    svg::init_strings();
    svg::init_colors();

		context = new Context();

    const char * fontdir = getenv("GLFT_FONTDIR");
    const char * cachedir = getenv("GLFT_CACHEDIR");
#ifdef WIN32
    if (!fontdir) fontdir = "c:/Winnt/Fonts" ;
    if (!cachedir) cachedir = "c:/Temp/glft-cache" ;
#else
    if (!fontdir) fontdir = "/usr/X11R6/lib/X11/fonts/drakfont/ttf" ;
    if (!cachedir) cachedir = "/tmp/glft-cache" ;
#endif
    fontManager = new glft::FontManager(fontdir, cachedir, true, true) ;
    context->fontManager = fontManager;

    imageManager = context->imageManager = new svgl::ImageManager;
    externalEntityManager = context->externalEntityManager = new svgl::ExternalEntityManager;
    ExternalEntityManager::setDefault(externalEntityManager);

    glinfo = new GLInfo();  
    glinfo->glInit();

    pickManager = new svgl::PickManager(context, glinfo);

    animinfo = new AnimationInfo();
    animationManager = animinfo->animationManager = new svgl::Animation::Manager(timeManager, .01);
    animinfo->externalEntityManager = externalEntityManager;
    animationManager->setRedisplayListener(redisplayListener);

    displayManager = new DisplayManager();
    displayManager->init(context, glinfo);

  }

}
