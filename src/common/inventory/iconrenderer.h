
#ifndef ICONRENDERER_H
#define ICONRENDERER_H

//#include <CEGUI.h>
#include <CEGUIBase.h>
#include <CEGUIWindow.h>


#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4251)
#endif

namespace CEGUI
{
  class Object;

  /*!
  \brief
    Interface for IconRenderer
  */
  struct IconRenderer
  {
    virtual ~IconRenderer() {}

    virtual bool Register(Object*) = 0;
    virtual bool UnRegister(Object*) = 0;
  };
} // end CEGUI

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif // ICONRENDERER_H
