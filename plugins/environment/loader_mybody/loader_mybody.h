/*
    Copyright (C) 2008 by Mogue Carpenter

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
// file : myloaderbody.h

#ifndef _H_MY_LOADER_TEMPLATE___
#define _H_MY_LOADER_TEMPLATE___

#define report_lvl false

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
    iStreamSource* ss, iLoaderContext* ldr_context, iBase* context );

  virtual csRef<iMyBody> ParseBody (iDocumentNode* node,
                      iStreamSource* ss,
                      iLoaderContext* ldr_context,
                      iBase* context,
                      csRef<iMyBody>& parent
                      );

  virtual bool IsThreadSafe(void) { return true; }

private:
  bool ParseLight (iDocumentNode* node, csRef<iMyBody> obj);
};


#endif   //  _H_MY_LOADER_TEMPLATE___
