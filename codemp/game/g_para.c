#include "g_local.h"
#include "g_para.h"

void G_PCVAR_GOLF_Switchfunc() {
	if (!PCVAR_G_GOLF.integer) {
		gentity_t * ent = NULL;
		while ( (ent = G_Find( ent, FOFS(classname), "golf_ball" )) != NULL ) {
			G_FreeEntity(ent);
		}
	}
}
