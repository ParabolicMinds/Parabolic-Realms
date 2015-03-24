#include "cm_local.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static Assimp::Importer imp;

int CM_GetModelVerticies(char const * name, vec3_t * points, int points_num) {
	fileHandle_t file;
	long len = FS_FOpenFileRead(name, &file, qfalse);
	if (len < 1) {
		FS_FCloseFile(file);
		return -1;
	}
	char * buf = new char[len];
	FS_Read(buf, len, file);
	aiScene const * scene = imp.ReadFileFromMemory(buf, len, aiProcess_JoinIdenticalVertices);
	int p = 0;
	for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
		if (p == points_num) break;
		aiMesh const * mesh = scene->mMeshes[m];
		for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
			if (p == points_num) break;
			points[p][0] = mesh->mVertices[v].x;
			points[p][1] = mesh->mVertices[v].y;
			points[p][2] = mesh->mVertices[v].z;
			p++;
		}
	}
	FS_FCloseFile(file);
	return p;
}
