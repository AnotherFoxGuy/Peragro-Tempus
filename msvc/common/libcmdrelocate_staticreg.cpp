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
static char const metainfo_cmdrelocate[] =
"<?xml version=\"1.0\"?>"
"<!-- cmdrelocate.csplugin -->"
"<plugin>"
"  <scf>"
"    <classes>"
"      <class>"
"        <name>peragro.commands.relocate</name>"
"        <implementation>cmdRelocate</implementation>"
"        <description>Relocate Command</description>"
"      </class>"
"    </classes>"
"  </scf>"
"</plugin>"
;
  #ifndef cmdRelocate_FACTORY_REGISTER_DEFINED 
  #define cmdRelocate_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(cmdRelocate) 
  #endif

class cmdrelocate
{
SCF_REGISTER_STATIC_LIBRARY(cmdrelocate,metainfo_cmdrelocate)
  #ifndef cmdRelocate_FACTORY_REGISTERED 
  #define cmdRelocate_FACTORY_REGISTERED 
    cmdRelocate_StaticInit cmdRelocate_static_init__; 
  #endif
public:
 cmdrelocate();
};
cmdrelocate::cmdrelocate() {}

}
