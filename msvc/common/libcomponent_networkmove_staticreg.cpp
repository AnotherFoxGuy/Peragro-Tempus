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
static char const metainfo_component_networkmove[] =
"<?xml version=\"1.0\"?>"
"<!-- NetworkMove.csplugin -->"
"<plugin>"
"  <scf>"
"    <classes>"
"      <class>"
"        <name>peragro.entity.move.networkmove</name>"
"        <implementation>ComponentFNetworkMove</implementation>"
"        <description>Perago entity movement controlled by network component</description>"
"      </class>"
"    </classes>"
"  </scf>"
"</plugin>"
;
  #ifndef ComponentFNetworkMove_FACTORY_REGISTER_DEFINED 
  #define ComponentFNetworkMove_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(ComponentFNetworkMove) 
  #endif

class component_networkmove
{
SCF_REGISTER_STATIC_LIBRARY(component_networkmove,metainfo_component_networkmove)
  #ifndef ComponentFNetworkMove_FACTORY_REGISTERED 
  #define ComponentFNetworkMove_FACTORY_REGISTERED 
    ComponentFNetworkMove_StaticInit ComponentFNetworkMove_static_init__; 
  #endif
public:
 component_networkmove();
};
component_networkmove::component_networkmove() {}

}
