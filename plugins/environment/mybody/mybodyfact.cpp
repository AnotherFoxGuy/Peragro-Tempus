//File: `myobject.cpp'

#include "mybodyfact.h"

#include "mybody.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY(MyBodyFactory);

MyBodyFactory::MyBodyFactory(iBase* parent)
  : scfImplementationType (this, parent)
{
}

csPtr<iMyBody> MyBodyFactory::CreateObject ()
{
  return new MyBody(object_reg);
}

bool MyBodyFactory::Initialize(iObjectRegistry* r)
{
  object_reg = r;
  return true;
}
