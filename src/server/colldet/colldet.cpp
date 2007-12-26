#include "ext/bullet/btBulletCollisionCommon.h"

void cd()
{
  // Setup World
  btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
  btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
  btSimpleBroadphase* broadphase = new btSimpleBroadphase();

  btCollisionWorld* collisionWorld = new btCollisionWorld(dispatcher,broadphase,collisionConfiguration);

  // Create Mesh
  btCollisionObject* collObj = new btCollisionObject();

  btMatrix3x3 basis;
  basis.setIdentity();
  collObj->getWorldTransform().setBasis(basis);

  btTriangleIndexVertexArray* mesharray = new btTriangleIndexVertexArray();

  btIndexedMesh mesh;

  mesh.m_numTriangles = 0;
  mesh.m_triangleIndexBase = (const unsigned char *)0;
  mesh.m_triangleIndexStride = 0;
  mesh.m_numVertices = 0;
  mesh.m_vertexBase = (const unsigned char *)0;
  mesh.m_vertexStride = 0;

  mesharray->addIndexedMesh(mesh);

  btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(mesharray, true);

  collObj->setCollisionShape(shape);

  collisionWorld->addCollisionObject(collObj);
}