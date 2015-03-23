#include "cm_local.h"

#include <vector>
#include <glm/matrix.hpp>

#define VECBANY( vect ) ( vect.x || vect.y || vect.z )

static glm::dvec3 CM_GetIntersectingPoint(cplane_t * a, cplane_t * b, cplane_t * c) {
	glm::dmat3x3 sysmat;
	sysmat[0][0] = a->normal[0];
	sysmat[0][1] = b->normal[0];
	sysmat[0][2] = c->normal[0];
	sysmat[1][0] = a->normal[1];
	sysmat[1][1] = b->normal[1];
	sysmat[1][2] = c->normal[1];
	sysmat[2][0] = a->normal[2];
	sysmat[2][1] = b->normal[2];
	sysmat[2][2] = c->normal[2];
	glm::dmat3x3 sysmati = glm::inverse(sysmat);
	return sysmati * glm::dvec3(a->dist, b->dist, c->dist);
}

void CM_NumData(int * brushes, int * brushsides, int * planes) {
	*brushes = cmg.numBrushes;
	*brushsides = cmg.numBrushSides;
	*planes = cmg.numPlanes;
}

int CM_BrushContentFlags(int brushnum) {
	if (brushnum > cmg.numBrushes) return 0;
	return cmg.brushes[brushnum].contents;
}

int CM_CalculateHull(int brushnum, vec3_t * points, int points_size) {
	if (brushnum >= cmg.numBrushes) return -1;
	std::vector<glm::vec3> points_vec;
	cbrush_t * brush = cmg.brushes + brushnum;
	int i, j, k;
	for (i = 0; i < brush->numsides; i++) {
		for (j = 0; j < brush->numsides; j++) {
			for (k = 0; k < brush->numsides; k++) {
				if (i == j || j == k || i == k) continue;
				if (points_size == (int)points_vec.size()) break;
				bool legal = true;
				glm::vec3 vec = CM_GetIntersectingPoint((brush->sides + i)->plane, (brush->sides + j)->plane, (brush->sides + k)->plane);
				if (VECBANY(glm::isinf(vec)) || VECBANY(glm::isnan(vec))) legal = false;
				if (legal) for (glm::vec3 const & svec : points_vec) { if (vec == svec) { legal = false; break; } }
				if (legal) for (int l = 0; l < brush->numsides; l++) {
					if (glm::dot(glm::vec3{brush->sides[l].plane->normal[0], brush->sides[l].plane->normal[1], brush->sides[l].plane->normal[2]}, vec) > brush->sides[l].plane->dist + 0.01f) {
						legal = false;
						break;
					}
				}
				if (legal) points_vec.push_back(vec);
			}
		}
	}
	int index = 0;
	for (glm::vec3 const & v : points_vec) {
		points[index][0] = v.x;
		points[index][1] = v.y;
		points[index][2] = v.z;
		index++;
	}
	return index;
}
