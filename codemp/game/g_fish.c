#include "g_local.h"

typedef enum fish_think_state_e {
	FISHS_RESTING = 0,
	FISHS_ORIENTING,
	FISHS_WANDERING,
	FISHS_GOTO,
	FISHS_BOLTING
} fish_think_state_t;

#define fishState genericValue1

static void FISH_Resting(gentity_t * fish) {

}

static void FISH_Orienting(gentity_t * fish) {

}

static void FISH_Wandering(gentity_t * fish) {

}

static void FISH_GoTo(gentity_t * fish) {

}

static void FISH_Bolting(gentity_t * fish) {

}

static void G_FishThink(gentity_t * fish) {
	switch (fish->fishState) {
	default:
		Com_Printf(S_COLOR_RED"A fish got an invalid state and must be deleted!\n");
		fish->think = G_FreeEntity;
		fish->nextthink = level.time;
		break;
	case FISHS_RESTING:
		FISH_Resting(fish);
		break;
	case FISHS_ORIENTING:
		FISH_Orienting(fish);
		break;
	case FISHS_WANDERING:
		FISH_Wandering(fish);
		break;
	case FISHS_GOTO:
		FISH_GoTo(fish);
		break;
	case FISHS_BOLTING:
		FISH_Bolting(fish);
		break;
	}
}

void G_FishStartThink(gentity_t * fish) {
	fish->fishState = FISHS_RESTING;
	fish->think = G_FishThink;
	fish->nextthink = level.time + 50;
}
