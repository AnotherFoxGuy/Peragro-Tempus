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
static char const metainfo_cmddate[] =
"<?xml version=\"1.0\"?>"
"<!-- cmddate.csplugin -->"
"<plugin>"
"  <scf>"
"    <classes>"
"      <class>"
"        <name>peragro.commands.date</name>"
"        <implementation>cmdDate</implementation>"
"        <description>Date Command</description>"
"      </class>"
"    </classes>"
"  </scf>"
"</plugin>"
;
  #ifndef cmdDate_FACTORY_REGISTER_DEFINED 
  #define cmdDate_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(cmdDate) 
  #endif

class cmddate
{
SCF_REGISTER_STATIC_LIBRARY(cmddate,metainfo_cmddate)
  #ifndef cmdDate_FACTORY_REGISTERED 
  #define cmdDate_FACTORY_REGISTERED 
    cmdDate_StaticInit cmdDate_static_init__; 
  #endif
public:
 cmddate();
};
cmddate::cmddate() {}

}
