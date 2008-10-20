//File: `mybody.h'
	  
#ifndef _H_MYBODYFACTORY___
#define _H_MYBODYFACTORY___
  
#include <cssysdef.h>
#include <csutil/scf_implementation.h>
#include <iutil/comp.h>

/*#include "crystalspace.h"*/
#include "imybody.h"
//#include "body.h"

class MyBodyFactory : public scfImplementation2<MyBodyFactory, iMyBodyFactory, iComponent>
{
private:
	iObjectRegistry* object_reg;
public: 
	MyBodyFactory (iBase* p);
 
	csPtr<iMyBody> CreateObject ();
	bool Initialize (iObjectRegistry*);


};
#endif // _H_MYBODYFACTORY___
