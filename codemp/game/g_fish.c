#include "g_local.h"

typedef enum fish_think_state_e {
	FISHS_RESTING = 0,
	FISHS_ORIENTING,
	FISHS_WANDERING,
	FISHS_GOTO,
	FISHS_BOLTING
} fish_think_state_t;

#define fishState genericValue1

static int FISH_Resting(gentity_t * fish) {
	if (!Q_irand(0, 10)) {
		fish->fishState = FISHS_ORIENTING;
		return 0;
	} else return 1000;
}

static int FISH_Orienting(gentity_t * fish) {
	return 1000;
}

static int FISH_Wandering(gentity_t * fish) {
	return 1000;
}

static int FISH_GoTo(gentity_t * fish) {
	return 1000;
}

static int FISH_Bolting(gentity_t * fish) {
	return 1000;
}

static void G_FishThink(gentity_t * fish) {
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
	fish->fishState = FISHS_WANDERING;
	fish->think = G_FishThink;
	fish->nextthink = level.time + 50;
}
