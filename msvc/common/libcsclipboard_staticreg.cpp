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
static char const metainfo_csclipboard[] =
"<?xml version=\"1.0\"?>"
"<!-- csclipboard.csplugin -->"
"<plugin>"
"  <scf>"
"    <classes>"
"      <class>"
"        <name>crystalspace.gui.clipboard.win32</name>"
"        <implementation>csClipboard</implementation>"
"        <description>Crystal Space clipboard plugin for Windows</description>"
"      </class>"
"      <class>"
"        <name>crystalspace.gui.clipboard</name>"
"        <implementation>csClipboard</implementation>"
"        <description>Crystal Space clipboard plugin for Windows</description>"
"      </class>"
"    </classes>"
"  </scf>"
"</plugin>"
;
  #ifndef csClipboard_FACTORY_REGISTER_DEFINED 
  #define csClipboard_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(csClipboard) 
  #endif
  #ifndef csClipboard_FACTORY_REGISTER_DEFINED 
  #define csClipboard_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(csClipboard) 
  #endif

class csclipboard
{
SCF_REGISTER_STATIC_LIBRARY(csclipboard,metainfo_csclipboard)
  #ifndef csClipboard_FACTORY_REGISTERED 
  #define csClipboard_FACTORY_REGISTERED 
    csClipboard_StaticInit csClipboard_static_init__; 
  #endif
  #ifndef csClipboard_FACTORY_REGISTERED 
  #define csClipboard_FACTORY_REGISTERED 
    csClipboard_StaticInit csClipboard_static_init__; 
  #endif
public:
 csclipboard();
};
csclipboard::csclipboard() {}

}
