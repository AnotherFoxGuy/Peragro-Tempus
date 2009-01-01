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
/**
 * @file modelmanager.h
 *
 * @brief Manages model factories.
 */

#ifndef PT_WORLD_UTILS_H
#define PT_WORLD_UTILS_H

#include <cssysdef.h>
#include <iutil/document.h>
#include <csutil/xmltiny.h>
#include <iutil/document.h>
#include <csutil/documenthelper.h>

#include <string>

namespace PT
{
  struct Utils
  {
    static bool FileExists(iVFS* vfs, const std::string& pathFile)
    {
      return vfs->Exists(pathFile.c_str());
    } // PathHelper()

    static std::string RealPath(iVFS* vfs, const std::string& path, const std::string& fileName)
    {
      //printf("BLAH1 %s %s\n",  path.c_str(), fileName.c_str());
      csRef<iDataBuffer> buf = vfs->GetRealPath(path.c_str());
      if (!buf) return "";
      std::string realPath = buf->GetData();
      //printf("BLAH2 %s %s\n",  realPath.c_str(), fileName.c_str());
      /*char* tmp = csPathsUtilities::ExpandPath(realPath.c_str());
      if (tmp) realPath = tmp;
      printf("BLAH3 %s %s\n",  realPath.c_str(), fileName.c_str());
      delete[] tmp;*/

      return realPath+"/"+fileName;
    } // RealPath()

    static std::string RealPath(iVFS* vfs, const std::string& pathFile)
    {
      // Seperate the path of the filename.
      size_t p = pathFile.find_last_of("/");
      std::string path = pathFile.substr(0,p+1);
      std::string file = pathFile.substr(p+1,pathFile.length());
      return RealPath(vfs, path, file);
    } // RealPath()

    static std::string PathHelper(iVFS* vfs, const std::string& path, const std::string& fileName)
    {
      csArray<std::string> paths;
      paths.Push("");
      paths.Push("/peragro/");
      paths.Push("/peragro/tiles/");
      paths.Push("/peragro/factories/");

      for (size_t i = 0; i < paths.GetSize(); i++)
      {
        std::string pathFile = paths[i] + path + fileName;
        if (vfs->Exists(pathFile.c_str()))
          return pathFile;
      }

      for (size_t i = 0; i < paths.GetSize(); i++)
      {
        std::string pathFile = paths[i] + fileName;
        if (vfs->Exists(pathFile.c_str()))
          return pathFile;
      }

      return "";
    } // PathHelper()

    /**
     * Put a document node inside another node.
     * @param node The node to wrap.
     * @return A new node with the original inside.
     */
    static csPtr<iDocumentNode> WrapNode(iDocumentNode* node)
    {
      csRef<iDocument> doc;
      csRef<iDocumentSystem> docsys; docsys.AttachNew(new csTinyDocumentSystem ());
      doc = docsys->CreateDocument ();
      csRef<iDocumentNode> root = doc->CreateRoot();
      csRef<iDocumentNode> newNode = root->CreateNodeBefore(node->GetType());
      CS::DocSystem::CloneNode(node, newNode);

      return csPtr<iDocumentNode> (root);
    } // end WrapNode()

    static csPtr<iDocumentNode> WrapAsLibNode(iDocumentNode* node, const std::string& parentName="")
    {
      csRef<iDocument> doc;
      csRef<iDocumentSystem> docsys; docsys.AttachNew(new csTinyDocumentSystem ());
      doc = docsys->CreateDocument ();
      csRef<iDocumentNode> root = doc->CreateRoot();
      csRef<iDocumentNode> libNode = root->CreateNodeBefore(CS_NODE_ELEMENT);
      libNode->SetValue("library");
      csRef<iDocumentNode> parentNode;
      if (parentName.empty())
        parentNode = libNode;
      else
      {
        parentNode = libNode->CreateNodeBefore(CS_NODE_ELEMENT);
        parentNode->SetValue(parentName.c_str());
      }
      csRef<iDocumentNode> newNode = parentNode->CreateNodeBefore(node->GetType());
      CS::DocSystem::CloneNode(node, newNode);

      return csPtr<iDocumentNode> (root);
    } // end WrapNode()

  }; // Utils namespace
} // PT namespace

#endif // PT_WORLD_UTILS_H
