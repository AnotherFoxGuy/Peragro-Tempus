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

#include "ext/bullet/btBulletDynamicsCommon.h"

#include "server/server.h"
#include "server/database/database.h"
#include "server/database/table-meshes.h"
#include "server/database/table-triangles.h"
#include "server/database/table-vertices.h"
#include "server/entity/entity.h"

#include "common/util/sleep.h"

BulletCD::BulletCD() 
{
  collisionConfiguration = NULL;
  dispatcher = NULL;
  overlappingPairCache = NULL;
  constraintSolver = NULL;
}

BulletCD::~BulletCD() 
{
  delete world;
  delete constraintSolver;
  delete overlappingPairCache;
  delete dispatcher;
  // TODO check if this is enough, just adding it still causes memleak..
  // delete collisionConfiguration;
}

void BulletCD::setup()
{
  Database* db = Server::getServer()->getDatabase();

  Array<VerticesTableVO*> vertices = 
    db->getVerticesTable()->getAll();

  btVector3 worldMin, worldMax;
  for (size_t i = 0; i < vertices.getCount(); i++)
  {
    VerticesTableVO* vertex = vertices.get(i);
    
    if ( i == 0 || worldMin.getX() < vertex->x ) worldMin.setX(vertex->x);
    if ( i == 0 || worldMin.getY() < vertex->y ) worldMin.setY(vertex->y);
    if ( i == 0 || worldMin.getZ() < vertex->z ) worldMin.setZ(vertex->z);

    if ( i == 0 || worldMax.getX() > vertex->x ) worldMax.setX(vertex->x);
    if ( i == 0 || worldMax.getY() > vertex->y ) worldMax.setY(vertex->y);
    if ( i == 0 || worldMax.getZ() > vertex->z ) worldMax.setZ(vertex->z);
  }

  // Setup World
  collisionConfiguration = new btDefaultCollisionConfiguration();
  dispatcher = new btCollisionDispatcher(collisionConfiguration);
  overlappingPairCache = new btAxisSweep3(worldMin,worldMax);
  constraintSolver = new btSequentialImpulseConstraintSolver();
  world = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,constraintSolver,collisionConfiguration);


  // Create Mesh
  Array<MeshesTableVO*> meshes = db->getMeshesTable()->getAll();
  for (size_t i = 0; i < meshes.getCount(); i++)
  {
    btCollisionObject* collObj = new btCollisionObject();

    btMatrix3x3 basis;
    basis.setIdentity();
    collObj->getWorldTransform().setBasis(basis);

    //TODO: reduce amount of queries!
    Array<TrianglesTableVO*> triangles = 
      db->getTrianglesTable()->getAllByMesh(meshes.get(i)->id);

    if (triangles.getCount() == 0) continue;

    Array<VerticesTableVO*> vertices = 
      db->getVerticesTable()->getAllByMesh(meshes.get(i)->id);

    //TODO: Use indexed triangle mesh!
    btTriangleMesh* mesh = new btTriangleMesh();

    mesh->preallocateIndices(triangles.getCount());
    mesh->preallocateVertices(vertices.getCount());

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

    collObj->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);

    world->addCollisionObject(collObj);
  }
}

void BulletCD::Run()
{
  if (world)
  {
    static btClock clock;
    
    float dt = clock.getTimeMicroseconds() * 0.000001f;
    world->stepSimulation(dt);

    pt_sleep(100);
  }
}

void BulletCD::addEntity(const Entity* entity)
{
  printf("Adding entity %d to colldet", entity->getId());

  btBoxShape* box = new btBoxShape(btVector3(1,2,1));

  btRigidBody* body = new btRigidBody(10.0f, 0, box);
  body->setFriction(1);

  cobjs[entity] = body;

  loadPosition(entity);

  world->addRigidBody(body);
}

void BulletCD::removeEntity(const Entity* entity)
{
  printf("Removing entity %d from colldet", entity->getId());

  world->removeRigidBody(cobjs[entity]);

  cobjs.erase(entity);
}

void BulletCD::loadPosition(const Entity* entity)
{
  btRigidBody* body = cobjs[entity];

  const float* pos = entity->getPos();

  btTransform& t = body->getWorldTransform();
  btVector3 p(pos[0], pos[1], pos[2]);
  t.setOrigin(p);
  t.setRotation(btQuaternion(entity->getRotation(), 0, 0));
  body->setWorldTransform(t);
}

void BulletCD::savePosition(const Entity* entity)
{
  btRigidBody* body = cobjs[entity];

  btTransform& t = body->getWorldTransform();
  btVector3 p = t.getOrigin();
  float rot = 0; // TODO: t.getRotation().getYaw();

  Entity* e = entity->getLock();
  e->setPos(p.getX(), p.getY(), p.getZ());
  e->setRotation(rot);
  e->freeLock();
}

void BulletCD::moveEntity(const Entity* entity, float* pos, float speed)
{
  //TODO: Make entity walk from the current position to pos
}

void BulletCD::moveEntity(const Entity* entity, float speed, float rot)
{
  btRigidBody* body = cobjs[entity];

  //TODO: Make entity walk continuously
  btQuaternion qrot(rot, 0, 0);
  //body->setAngularVelocity(qrot.);
  body->setLinearVelocity(btVector3(1, 0, 0) * speed);
}
