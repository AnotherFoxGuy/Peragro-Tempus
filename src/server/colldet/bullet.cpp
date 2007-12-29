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

#include "bullet.h"

#include "ext/bullet/btBulletCollisionCommon.h"

#include "server/server.h"
#include "server/database/database.h"
#include "server/database/table-meshes.h"
#include "server/database/table-triangles.h"
#include "server/database/table-vertices.h"
#include "server/entity/entity.h"

void BulletCD::setup()
{
  Database* db = Server::getServer()->getDatabase();

  // Setup World
  btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
  btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
  btSimpleBroadphase* broadphase = new btSimpleBroadphase();

  collisionWorld = new btCollisionWorld(dispatcher,broadphase,collisionConfiguration);

  // Create Mesh
  Array<MeshesTableVO*> meshes = db->getMeshesTable()->getAll();
  for (size_t i = 0; i < meshes.getCount(); i++)
  {
    btCollisionObject* collObj = new btCollisionObject();

    btMatrix3x3 basis;
    basis.setIdentity();
    collObj->getWorldTransform().setBasis(basis);

    //TODO: Use indexed triangle mesh!
    btTriangleMesh* mesh = new btTriangleMesh();

    //TODO: reduce amount of queries!
    Array<TrianglesTableVO*> triangles = 
      db->getTrianglesTable()->getAllByMesh(meshes.get(i)->id);

    Array<VerticesTableVO*> vertices = 
      db->getVerticesTable()->getAllByMesh(meshes.get(i)->id);

    for (size_t j = 0; j < triangles.getCount(); j++)
    {
      TrianglesTableVO* t = triangles.get(j);

      VerticesTableVO* v0 = vertices.get(t->a);
      btVector3 btV0(v0->x, v0->y, v0->z);

      VerticesTableVO* v1 = vertices.get(t->b);
      btVector3 btV1(v1->x, v1->y, v1->z);

      VerticesTableVO* v2 = vertices.get(t->c);
      btVector3 btV2(v2->x, v2->y, v2->z);

      mesh->addTriangle(btV0, btV1, btV2);
    }

    btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(mesh, true);

    collObj->setCollisionShape(shape);

    collisionWorld->addCollisionObject(collObj);
  }
}

void BulletCD::Run()
{
  if (collisionWorld)
    collisionWorld->performDiscreteCollisionDetection();
}

void BulletCD::addEntity(const Entity* entity)
{
  printf("Adding entity %d to colldet", entity->getId());

  btBoxShape* box = new btBoxShape(btVector3(1,2,1));

  btCollisionObject* collObj = new btCollisionObject();
  collObj->setCollisionShape(box);

  btTransform& t = collObj->getWorldTransform();
  btVector3 p(entity->getPos()[0], entity->getPos()[1], entity->getPos()[2]);
  t.setOrigin(p);
  t.setRotation(btQuaternion(entity->getRotation(), 0, 0));
  collObj->setWorldTransform(t);

  cobjs[entity] = collObj;

  collisionWorld->addCollisionObject(collObj);
}

void BulletCD::removeEntity(const Entity* entity)
{
  printf("Removing entity %d from colldet", entity->getId());

  collisionWorld->removeCollisionObject(cobjs[entity]);

  cobjs.erase(entity);
}

void BulletCD::moveEntity(const Entity* entity, float* pos, float speed)
{
  btCollisionObject* collObj = cobjs[entity];

  //TODO: Make entity walk from the current position to pos
}
