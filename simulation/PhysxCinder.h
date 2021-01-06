#ifndef PhysxCinder_H
#define PhysxCinder_H
#include <iostream>
#include "Simulation.h"
#include "PhysxIntegration.h"

// references physx\snippets\snippetrender\SnippetRender.cpp
#define MAX_NUM_MESH_VEC3S 1024
void Simulation::renderGeometry(std::string name, PxRigidActor* actor, PxShape* shape, vec3 pose)
{
	
	const PxGeometryHolder h = shape->getGeometry();
	const PxGeometry& geom = h.any();
	switch(geom.getType())
	{
		case PxGeometryType::eBOX:
		{
			const PxBoxGeometry& boxGeom = static_cast<const PxBoxGeometry&>(geom);
			glScalef(boxGeom.halfExtents.x, boxGeom.halfExtents.y, boxGeom.halfExtents.z);
			gl::drawCube(pose, vec3(2));
		}
		break;

		case PxGeometryType::eSPHERE:
		{
			const PxSphereGeometry& sphereGeom = static_cast<const PxSphereGeometry&>(geom);
			gl::drawSphere(pose, sphereGeom.radius, -1);
		}
		break;

		case PxGeometryType::eCONVEXMESH:
		{
			const PxConvexMeshGeometry& convexGeom = static_cast<const PxConvexMeshGeometry&>(geom);

			//Compute triangles for each polygon.
			const PxVec3& scale = convexGeom.scale.scale;
			PxConvexMesh* mesh = convexGeom.convexMesh;
			const PxU32 nbPolys = mesh->getNbPolygons();
			const PxU8* polygons = mesh->getIndexBuffer();
			const PxVec3* verts = mesh->getVertices();
			PxU32 nbVerts = mesh->getNbVertices();
			PX_UNUSED(nbVerts);

			TriMesh triMeshShape = TriMesh(  TriMesh::Format()
            .positions()
			.normals()
            .colors(3));
			PxU32 numTotalTriangles = 0;
			for(PxU32 i = 0; i < nbPolys; i++)
			{
				PxHullPolygon data;
				mesh->getPolygonData(i, data);

				const PxU32 nbTris = PxU32(data.mNbVerts - 2);
				const PxU8 vref0 = polygons[data.mIndexBase + 0];
				PX_ASSERT(vref0 < nbVerts);
				for(PxU32 j=0;j<nbTris;j++)
				{
					const PxU32 vref1 = polygons[data.mIndexBase + 0 + j + 1];
					const PxU32 vref2 = polygons[data.mIndexBase + 0 + j + 2];

					//generate face normal:
					PxVec3 e0 = verts[vref1] - verts[vref0];
					PxVec3 e1 = verts[vref2] - verts[vref0];

					PX_ASSERT(vref1 < nbVerts);
					PX_ASSERT(vref2 < nbVerts);

					PxVec3 fnormal = e0.cross(e1);
					fnormal.normalize();
			
					if(numTotalTriangles*6 < MAX_NUM_MESH_VEC3S)
					{
						PxVec3 vertex = verts[vref0];
						triMeshShape.appendPosition(vec3(vertex.x, vertex.y, vertex.z));
						triMeshShape.appendNormal(vec3(fnormal.x, fnormal.y, fnormal.z));
						vertex = verts[vref1];
						triMeshShape.appendPosition(vec3(vertex.x, vertex.y, vertex.z));
						triMeshShape.appendNormal(vec3(fnormal.x, fnormal.y, fnormal.z));
						vertex = verts[vref2];
						triMeshShape.appendPosition(vec3(vertex.x, vertex.y, vertex.z));
						triMeshShape.appendNormal(vec3(fnormal.x, fnormal.y, fnormal.z));
						numTotalTriangles++;
					}
				}
			}
			geom::SourceMods geomGroundPlaneShape = triMeshShape >> geom::Scale(glm::vec3(scale.x, scale.y, scale.z)) >> geom::Translate(pose);
			ci::ColorA color = ci::ColorA( CM_RGB, 1, 0, 1, 1 );
			createSimulationObject("convexMesh " + name, actor, geomGroundPlaneShape, NULL, &color);
		}
		break;
		case PxGeometryType::ePLANE:
        {
			geom::SourceMods geomGroundPlaneShape = geom::Cube() >> geom::Scale(glm::vec3(0.1, 200, 200));
			ci::ColorA whiteColor = ci::ColorA( CM_RGB, 1, 1, 1, 1 );
			createSimulationObject("plane " + name, actor, geomGroundPlaneShape, NULL, &whiteColor);
        }
		break;
		case PxGeometryType::eINVALID:
		case PxGeometryType::eHEIGHTFIELD:
		case PxGeometryType::eGEOMETRY_COUNT:
		break;	
	}
}


void Simulation::drawSceneActors() {
	PxU32 nbActors = gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
	if(nbActors)
	{
		std::vector<PxRigidActor*> actors(nbActors);
		gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
		const int numActors = static_cast<int>(actors.size());
		
		for(int i=0; i<numActors;i++)
		{
			const int nbShapes = actors[i]->getNbShapes();
			PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
            physx::PxShape** shapes = (physx::PxShape**)gAllocator.allocate(sizeof(physx::PxShape*)*nbShapes, nullptr, __FILE__, __LINE__);
			actors[i]->getShapes(shapes, nbShapes);

			for(int j=0;j<nbShapes;j++)
			{
				const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
				const PxGeometryHolder h = shapes[j]->getGeometry();
				
				physx::PxTransform localPose = shapes[j]->getLocalPose();
				vec3 pose = vec3(localPose.p.x, localPose.p.y, localPose.p.z);
				renderGeometry("act"+std::to_string(i)+"shp"+std::to_string(j),actors[i], shapes[j], pose);
			}
		}
	}

}

#endif