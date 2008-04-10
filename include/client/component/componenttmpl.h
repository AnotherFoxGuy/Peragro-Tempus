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

#ifndef __FACTTMPL_H__
#define __FACTTMPL_H__

/**
 * This macro declares a factory class for a property class. Note that your
 * Component name must be of the form ComponentMyName if you've given
 * MyName as Factory Name.
 */
#define DECLARE_COMPONENTFACTORY(name)					    \
class ComponentF##name : public scfImplementation2<ComponentF##name,        \
  ComponentFactoryInterface, iComponent>				    \
{									    \
private:								    \
  iObjectRegistry* object_reg;						    \
									    \
public:									    \
  ComponentF##name (iBase* parent) : scfImplementationType (this, parent) { }    \
  virtual ~ComponentF##name () { }				            \
  bool Initialize (iObjectRegistry* object_reg);			    \
  virtual const char* GetName() const;					    \
  virtual csPtr<ComponentInterface> CreateComponent (const char* name);     \
};

/** 
 * This macro implements a Component factory, the first argument is the
 * name you've given at the declaration, the 2nd argument is the string the
 * factory will be registered with.
 * Note: You have to export the classtable yourself as one could have multiple
 * factories in 1 file. The name to export for the factory MyFactory is:
 *   celPfMyFactory
 */
#define IMPLEMENT_COMPONENTFACTORY(name, strname)			    \
SCF_IMPLEMENT_FACTORY (ComponentF##name)				    \
bool ComponentF##name::Initialize (iObjectRegistry* object_reg)		    \
{									    \
  ComponentF##name::object_reg = object_reg;				    \
  return true;								    \
}									    \
const char* ComponentF##name::GetName() const				    \
{									    \
  return strname;							    \
}									    \
csPtr<ComponentInterface> ComponentF##name::CreateComponent(const char* name) \
{									    \
  ComponentInterface* c = new Component##name (object_reg);	    	    \
  c->SetName (name); 							    \
  return csPtr<ComponentInterface> (c); 				    \
}									    \

#endif
