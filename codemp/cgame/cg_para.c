#include "cg_para.h"

#include "cg_local.h"

char cvb[4096];

int CG_GetCvarInt(char const * cvar) {
	memset(cvb, 0, sizeof(cvb));
	trap->Cvar_VariableStringBuffer(cvar, cvb, sizeof(cvb));
	return (int)strtol(cvb, 0, 10);
}
