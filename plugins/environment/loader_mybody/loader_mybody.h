// file : myloaderbody.h

#ifndef _H_MY_LOADER_TEMPLATE___
#define _H_MY_LOADER_TEMPLATE___

#include <csutil/scf_implementation.h>
#include <csutil/strhash.h>
#include <csutil/csstring.h>
#include <iutil/comp.h>
#include <iutil/plugin.h>
#include <imap/reader.h>
#include <iengine/sector.h>

#include <iengine/engine.h>

#include "imybody.h"

class myLoaderBody:
  public scfImplementation2<myLoaderBody, iComponent, iLoaderPlugin>
{

protected:
  iObjectRegistry* object_reg;
  csRef<iSyntaxService> synldr;

  csStringHash xmltokens;

  bool ParseXML (iDocumentNode* node);

public:

  myLoaderBody (iBase *p);
  virtual ~myLoaderBody ();

  virtual bool Initialize(iObjectRegistry *object_reg);

  virtual csPtr<iBase> Parse (iDocumentNode* node, 
    iStreamSource* ss, iLoaderContext* ldr_context, iBase* context, iStringArray* failed = 0);

  csRef<iMyBody> ParseBody (iDocumentNode* node, 
                      iStreamSource* ss,
                      iLoaderContext* ldr_context,
                      iBase* context,
                      csRef<iMyBody>& parent,
                      iStringArray* failed = 0
                      );

  virtual bool IsThreadSafe(void) { return true; }

private:
  bool ParseLight (iDocumentNode* node, csRef<iMyBody> obj);
};  


#endif   //  _H_MY_LOADER_TEMPLATE___
