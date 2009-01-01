/*
    Copyright (C) 2005 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef PROPERTIES_H
#define PROPERTIES_H

#include "crystalspace.h"


static bool SetTextureProperties(FileLoader* fileloader, TexturePrototype& texProto)
{
  iObjectRegistry* object_reg = fileloader->GetObjectRegistry();
  csRef<iSyntaxService> SyntaxService = csQueryRegistryOrLoad<iSyntaxService> (object_reg, "crystalspace.syntax.loader.service.text");

  csRef<iTextureWrapper> tex = texProto.texture;
  if (!tex.IsValid()) return false;

  if (texProto.keepImage) tex->SetKeepImage (true);
  if (!texProto.Transparency.IsBlack())
    tex->SetKeyColor (csQint (texProto.Transparency.red * 255.99),
                      csQint (texProto.Transparency.green * 255.99),
                      csQint (texProto.Transparency.blue * 255.99));
  tex->SetTextureClass (texProto.textureClass.c_str());
  tex->GetTextureHandle()->SetAlphaType (texProto.alphaType);

  csRef<iProcTexture> ipt = scfQueryInterface<iProcTexture> (tex);
  if (ipt)
    ipt->SetAlwaysAnimate (texProto.alwaysAnimate);

  // Attach the keyvalue pairs.
  for (size_t i = 0 ; i < texProto.keyValuePairs.GetSize () ; i++)
  {
    csRef<iKeyValuePair> key = SyntaxService->ParseKey (texProto.keyValuePairs[i]);
    if (!key.IsValid())
      return false;
    tex->QueryObject()->ObjAdd(key->QueryObject());
  }

  return true;
}


#endif // PROPERTIES_H
