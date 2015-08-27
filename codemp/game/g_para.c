#include "g_para.h"

void G_PCVAR_GOLF_Switchfunc() {
	if (!PCVAR_G_GOLF.integer) {
		gentity_t * ent = NULL;
		while ( (ent = G_Find( ent, FOFS(classname), "golf_ball" )) != NULL ) {
			G_FreeEntity(ent);
		}
	}
}

static void * manifests [G_PSE_MAX_MANIFESTS];

pse_loadstatus_t G_PSE_LoadManifest(char const * path) {
	size_t i;
	for (i = 0; i < G_PSE_MAX_MANIFESTS; i++) {
		if (!manifests[i]) {
			void * handle = trap->PARA_CreateScriptingContext(path);
			if (!handle) return G_PSM_FNF_NOPLUG;
			manifests[i] = handle;
			return G_PSM_OK;
		}
	}
	return G_PSM_LIMIT;
}

void G_PSE_PurgeManifests() {
	size_t i;
	for (i = 0; i < G_PSE_MAX_MANIFESTS; i++) {
		if (manifests[i]) trap->PARA_DeleteScriptingContext(manifests[i]);
		manifests[i] = NULL;
	}
}
