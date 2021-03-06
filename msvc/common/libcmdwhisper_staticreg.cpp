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
static char const metainfo_cmdwhisper[] =
"<?xml version=\"1.0\"?>"
"<!-- cmdwhisper.csplugin -->"
"<plugin>"
"  <scf>"
"    <classes>"
"      <class>"
"        <name>peragro.commands.whisper</name>"
"        <implementation>cmdWhisper</implementation>"
"        <description>whisper command</description>"
"      </class>"
"    </classes>"
"  </scf>"
"</plugin>"
;
  #ifndef cmdWhisper_FACTORY_REGISTER_DEFINED 
  #define cmdWhisper_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(cmdWhisper) 
  #endif

class cmdwhisper
{
SCF_REGISTER_STATIC_LIBRARY(cmdwhisper,metainfo_cmdwhisper)
  #ifndef cmdWhisper_FACTORY_REGISTERED 
  #define cmdWhisper_FACTORY_REGISTERED 
    cmdWhisper_StaticInit cmdWhisper_static_init__; 
  #endif
public:
 cmdwhisper();
};
cmdwhisper::cmdwhisper() {}

}
