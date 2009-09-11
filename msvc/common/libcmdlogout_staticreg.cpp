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
static char const metainfo_cmdlogout[] =
"<?xml version=\"1.0\"?>"
"<!-- cmdlogout.csplugin -->"
"<plugin>"
"  <scf>"
"    <classes>"
"      <class>"
"        <name>peragro.commands.logout</name>"
"        <implementation>cmdLogout</implementation>"
"        <description>Logout Command</description>"
"      </class>"
"    </classes>"
"  </scf>"
"</plugin>"
;
  #ifndef cmdLogout_FACTORY_REGISTER_DEFINED 
  #define cmdLogout_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(cmdLogout) 
  #endif

class cmdlogout
{
SCF_REGISTER_STATIC_LIBRARY(cmdlogout,metainfo_cmdlogout)
  #ifndef cmdLogout_FACTORY_REGISTERED 
  #define cmdLogout_FACTORY_REGISTERED 
    cmdLogout_StaticInit cmdLogout_static_init__; 
  #endif
public:
 cmdlogout();
};
cmdlogout::cmdlogout() {}

}
