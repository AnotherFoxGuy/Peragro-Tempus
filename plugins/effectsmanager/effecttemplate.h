/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#ifndef PT_EFFECTTEMPLATE_H
#define PT_EFFECTTEMPLATE_H

#include <cssysdef.h>
#include <iutil/document.h>
#include <csgeom/vector3.h>
#include <csutil/refcount.h>

#include <string>

static csVector3& operator<< (csVector3& vector, iDocumentNode* node)
{
  vector.x = node->GetAttributeValueAsFloat("x");
  vector.y = node->GetAttributeValueAsFloat("y");
  vector.z = node->GetAttributeValueAsFloat("z");
  return vector;
}

/**
 * @ingroup effects
 * Helper class that contains basic information about effect type. Usually not
 * created directly, but accessed via EffectManager instance. Use the
 * Set/Get methods to access the effect properties.
 * @see EffectManager
 */
class EffectTemplate : public csRefCount
{
private:
  ///Unique ID of the effect type (ie '3').
  unsigned int id;
  ///Name of the effect (ie 'movemarker').
  std::string name;
  ///relative path to the mesh file containing the effect mesh
  ///(ie '/peragro/meshes/effects/movemarker.xml').
  std::string meshFile;
  ///meshName - name of the mesh in the mesh file used for this effect (ie
  ///'movemarkerfact').
  std::string meshName;
  ///Duration of the effect in ms (ie '1000' == 1 second).
  int duration;
  ///Relative offset for the effect.
  csVector3 offset;

public:
  EffectTemplate() : id(0), duration(0), offset(0.0f) {}

  EffectTemplate(iDocumentNode* node) : id(0), duration(0), offset(0.0f) 
  {
    SetId(node->GetNode("id")->GetContentsValueAsInt());
    SetName(node->GetNode("name")->GetContentsValue());
    SetMeshFile(node->GetNode("file")->GetContentsValue());
    SetMeshName(node->GetNode("mesh")->GetContentsValue());
    SetDuration(node->GetNode("duration")->GetContentsValueAsInt());

    csVector3 pos; pos << node->GetNode("offset");
    SetOffset(pos);
  }

  void SetId(unsigned int value) { id = value; }
  unsigned int GetId() const { return id; }

  void SetName(const std::string& value) { name=value; }
  const std::string& GetName() const { return name; }

  void SetMeshFile(const std::string& value) { meshFile=value; }
  const std::string& GetMeshFile() const { return meshFile; }

  void SetMeshName(const std::string& value) { meshName=value; }
  const std::string& GetMeshName() const { return meshName; }

  void SetDuration(int value) { duration = value; }
  int GetDuration() const { return duration; }

  void SetOffset(float x, float y, float z)
    { offset = csVector3(x,y,z); }
  void SetOffset(const csVector3& value) { offset = value;}
  const csVector3& GetOffset() const { return offset; }
};

#endif // PT_EFFECTTEMPLATE_H
