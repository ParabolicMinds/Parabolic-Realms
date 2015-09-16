#include "g_local.h"

typedef enum fish_think_state_e {
	FISHS_RESTING = 0,
	FISHS_ORIENTING,
	FISHS_WANDERING,
	FISHS_GOTO,
	FISHS_BOLTING
} fish_think_state_t;

#define fishState genericValue1

static void FISH_HardUpdate(gentity_t * fish) {
	VectorCopy(fish->r.currentAngles, fish->s.angles);
	VectorCopy(fish->r.currentAngles, fish->s.apos.trBase);
	VectorCopy(fish->r.currentOrigin, fish->s.origin);
	VectorCopy(fish->r.currentOrigin, fish->s.pos.trBase);
}

static void FISH_ForwardMove(gentity_t * fish, float mag) {
	fish->s.pos.trTime = level.time;
	fish->s.pos.trType = TR_LINEAR;
	vec3_t ang;
	VectorAdd(fish->s.angles, fish->s.angles2, ang);
	AngleVectors(ang, fish->s.pos.trDelta, NULL, NULL);
	VectorScale(fish->s.pos.trDelta, mag, fish->s.pos.trDelta);
}

static int FISH_Resting(gentity_t * fish) {
	if (!Q_irand(0, 10)) {
		fish->fishState = FISHS_ORIENTING;
		return 0;
	} else return 1000;
}

static int FISH_Orienting(gentity_t * fish) {
	Com_Printf("Fish begin orienting!\n");
	G_SetAngles(fish, fish->r.currentAngles);
	fish->s.apos.trTime = level.time;
	fish->s.apos.trType = TR_LINEAR;
	switch (Q_irand(1, 3)) {
	default:
	case 1:
		VectorSet(fish->s.apos.trDelta, 0, 20, 0);
		break;
	case 2:
		VectorSet(fish->s.apos.trDelta, 0, 20, 0);
		break;
	case 3:
		VectorSet(fish->s.apos.trDelta, 20, 0, 0);
		break;
	}
	return 1000;
}

static int FISH_Wandering(gentity_t * fish) {
	return 1000;
}

static int FISH_GoTo(gentity_t * fish) {
	return 1000;
}

static int FISH_Bolting(gentity_t * fish) {
	FISH_ForwardMove(fish, 20);
	return 1000;
}

static void G_FishThink(gentity_t * fish) {
	FISH_HardUpdate(fish);
	switch (fish->fishState) {
	default:
		Com_Printf(S_COLOR_RED"A fish got an invalid state and must be deleted!\n");
		fish->think = G_FreeEntity;
		fish->nextthink = level.time;
		break;
	case FISHS_RESTING:
		fish->nextthink = level.time + FISH_Resting(fish);
		break;
	case FISHS_ORIENTING:
		fish->nextthink = level.time + FISH_Orienting(fish);
		break;
	case FISHS_WANDERING:
		fish->nextthink = level.time + FISH_Wandering(fish);
		break;
	case FISHS_GOTO:
		fish->nextthink = level.time + FISH_GoTo(fish);
		break;
	case FISHS_BOLTING:
		fish->nextthink = level.time + FISH_Bolting(fish);
		break;
	}
}

void G_FishStartThink(gentity_t * fish) {
	fish->fishState = FISHS_BOLTING;
	fish->think = G_FishThink;
	fish->nextthink = level.time + 2000;
}
