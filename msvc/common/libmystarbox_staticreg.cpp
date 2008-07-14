// This file is automatically generated.
#include "cssysdef.h"
#include "csutil/scf.h"

// Put static linking stuff into own section.
// The idea is that this allows the section to be swapped out but not
// swapped in again b/c something else in it was needed.
#if !defined(CS_DEBUG) && defined(CS_COMPILER_MSVC)
#pragma const_seg(".CSmetai")
#pragma comment(linker, "/section:.CSmetai,r")
#pragma code_seg(".CSmeta")
#pragma comment(linker, "/section:.CSmeta,er")
#pragma comment(linker, "/merge:.CSmetai=.CSmeta")
#endif

namespace csStaticPluginInit
{
static char const metainfo_mystarbox[] =
"<?xml version=\"1.0\"?>"
"<plugin>"
"  <scf>"
"    <classes>"
"      <class>"
"        <name>peragro.environment.starboxfactory</name>"
"        <implementation>MyStarboxFactory</implementation>"
"        <description>Recon69s Starbox factory, displays a starscape by drawing 2d points on the 2d canvas before 3d objects drawn. It clears the screen before drawing stars.</description>"
"      </class>     "
"    </classes>"
"  </scf>"
"</plugin>"
;
  #ifndef MyStarboxFactory_FACTORY_REGISTER_DEFINED 
  #define MyStarboxFactory_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(MyStarboxFactory) 
  #endif

class mystarbox
{
SCF_REGISTER_STATIC_LIBRARY(mystarbox,metainfo_mystarbox)
  #ifndef MyStarboxFactory_FACTORY_REGISTERED 
  #define MyStarboxFactory_FACTORY_REGISTERED 
    MyStarboxFactory_StaticInit MyStarboxFactory_static_init__; 
  #endif
public:
 mystarbox();
};
mystarbox::mystarbox() {}

}
