#include "cm_local.h"

#include <vector>
#include <glm/matrix.hpp>

#define VECBANY( vect ) ( vect.x || vect.y || vect.z )

static glm::vec3 CM_GetIntersectingPoint(cplane_t * a, cplane_t * b, cplane_t * c) {
	glm::mat3x3 sysmat;
	sysmat[0][0] = a->normal[0];
	sysmat[0][1] = b->normal[0];
	sysmat[0][2] = c->normal[0];
	sysmat[1][0] = a->normal[1];
	sysmat[1][1] = b->normal[1];
	sysmat[1][2] = c->normal[1];
	sysmat[2][0] = a->normal[2];
	sysmat[2][1] = b->normal[2];
	sysmat[2][2] = c->normal[2];
	return glm::inverse(sysmat) * glm::vec3(a->dist, b->dist, c->dist);
}

void CM_NumData(int * brushes, int * patches) {
	*brushes = cmg.numBrushes;
	*patches = cmg.numSurfaces;
}

void CM_FWBRecurse (cNode_t * node, int & i, int * brushes) {
	if (node->children[0] < 0) {
		cLeaf_t * leaf = &cmg.leafs[-1-node->children[0]];
		for (int j = 0; j < leaf->numLeafBrushes; j++) {
			bool good = true;
			for (int k = 0; k < i && good; k++) {
				if (brushes[k] == cmg.leafbrushes[leaf->firstLeafBrush + j]) good = false;
			}
			if (good) {
				brushes[i] = cmg.leafbrushes[leaf->firstLeafBrush + j];
				i++;
			}
		}
	} else {
		CM_FWBRecurse(&cmg.nodes[node->children[0]], i, brushes);
	}
	if (node->children[1] < 0) {
		cLeaf_t * leaf = &cmg.leafs[-1-node->children[1]];
		for (int j = 0; j < leaf->numLeafBrushes; j++) {
			bool good = true;
			for (int k = 0; k < i && good; k++) {
				if (brushes[k] == cmg.leafbrushes[leaf->firstLeafBrush + j]) good = false;
			}
			if (good) {
				brushes[i] = cmg.leafbrushes[leaf->firstLeafBrush + j];
				i++;
			}
		}
	} else {
		CM_FWBRecurse(&cmg.nodes[node->children[1]], i, brushes);
	}
}

int CM_SubmodelIndicies(int submodel, int * brushes) {
	cmodel_t * wmod = &cmg.cmodels[submodel];
	int i = 0;
	if (wmod->firstNode < 0) {
		for (int j = 0; j < wmod->leaf.numLeafBrushes; j++, i++) {
			brushes[i] = cmg.leafbrushes[wmod->leaf.firstLeafBrush + j];
		}
	} else {
		cNode_t * node = &cmg.nodes[wmod->firstNode];
		CM_FWBRecurse(node, i, brushes);
	}
	return i;
}

int CM_BrushContentFlags(int brushnum) {
	if (brushnum > cmg.numBrushes) return 0;
	return cmg.brushes[brushnum].contents;
}

int CM_CalculateHull(int brushnum, vec3_t * points, int points_size) {
	if (brushnum >= cmg.numBrushes) return -1;
	std::vector<glm::vec3> points_vec;
	points_vec.reserve(points_size);
	cbrush_t * brush = cmg.brushes + brushnum;
	int i, j, k, size;
	size = 0;
	for (i = 0; i < brush->numsides; i++) {
		for (j = 0; j < i; j++) {
			for (k = 0; k < j; k++) {
				if (i == j || j == k || i == k) continue;
				if (size == points_size) break;
				bool legal = true;
				glm::vec3 vec = CM_GetIntersectingPoint((brush->sides + i)->plane, (brush->sides + j)->plane, (brush->sides + k)->plane);
				if (VECBANY(glm::isinf(vec)) || VECBANY(glm::isnan(vec))) legal = false;
				if (legal) for (int l = 0; l < brush->numsides; l++) {
					if (l == i || l == j || l == k) continue;
					if (glm::dot(glm::vec3{brush->sides[l].plane->normal[0], brush->sides[l].plane->normal[1], brush->sides[l].plane->normal[2]}, vec) > brush->sides[l].plane->dist + 0.01f) {
						legal = false;
						break;
					}
				}
				if (legal) {
					points_vec.push_back(vec);
					size++;
				}
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

