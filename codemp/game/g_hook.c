#include "g_local.h"

static void hook_grapple_think(gentity_t * hook) {
	if (level.time >= hook->genericValue1) {
		G_FreeEntity(hook);
		return;
	}
	vec3_t hookPos, parentPos, diff;
	hook->nextthink = level.time;
	VectorCopy(hook->r.currentOrigin, hookPos);
	VectorCopy(hook->parent->client->ps.origin, parentPos);
	VectorSubtract(hookPos, parentPos, diff);
	VectorNormalize(diff);
	VectorScale(diff, 100, diff);
	VectorCopy(diff, hook->parent->client->ps.velocity);
}

static void hook_fly_think(gentity_t * hook) {
	vec3_t		origin, oldOrg;
	trace_t		tr;
	gentity_t	*traceEnt = NULL;

	hook->nextthink = level.time + FRAMETIME;

	VectorCopy( hook->r.currentOrigin, oldOrg );
	BG_EvaluateTrajectory( &hook->s.pos, level.time, origin );

	trap->Trace( &tr, hook->r.currentOrigin, hook->r.mins, hook->r.maxs, origin, hook->parent ? hook->parent->s.number : hook->s.number, hook->clipmask, qfalse, 0, 0 );
	if ( !tr.startsolid && !tr.allsolid && tr.fraction ) {
		VectorCopy( tr.endpos, hook->r.currentOrigin );
		trap->LinkEntity( (sharedEntity_t *)hook );
	} else {
		tr.fraction = 0;
	}

	if ( tr.fraction == 1 ) return;

	traceEnt = &g_entities[tr.entityNum];
	if ( tr.fraction && traceEnt->client && traceEnt->takedamage ) G_Damage(traceEnt, hook, hook->parent, NULL, traceEnt->client->ps.origin, 2, DAMAGE_NO_KNOCKBACK, MOD_MELEE);

	VectorCopy(hook->r.currentOrigin, hook->s.origin);
	G_SetOrigin(hook, hook->s.origin);
	hook->think = hook_grapple_think;
	hook->genericValue1 = level.time + 10000;
}

void G_GrappleHook(gentity_t * ent) {
	gentity_t * hook;

	if (!ent->client) return;

	hook = G_Spawn();
	if(!hook) return;

	VectorCopy(ent->client->ps.origin, hook->s.origin);
	VectorCopy(ent->client->ps.viewangles, hook->s.angles);
	hook->s.origin[2] += 10;
	G_SetOrigin(hook, hook->s.origin);
	G_SetAngles(hook, hook->s.angles);

	hook->s.owner = ent->s.number;
	hook->r.ownerNum = ent->s.number;
	hook->parent = ent;

	hook->s.modelindex = G_ModelIndex("/models/weapons2/merr_sonn/projectile.md3");
	hook->s.eType = ET_GENERAL;

	VectorSet(hook->r.mins, -8, -8, -8);
	VectorSet(hook->r.maxs, 8, 8, 8);

	vec3_t delta;
	AngleVectors(ent->client->ps.viewangles, delta, NULL, NULL);
	VectorScale(delta, 250, delta);
	VectorCopy(delta, hook->s.pos.trDelta);
	hook->s.pos.trTime = level.time;
	hook->s.pos.trType = TR_LINEAR;

	hook->clipmask = MASK_SHOT;

	hook->think = hook_fly_think;
	hook->nextthink = level.time + FRAMETIME;

	hook->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	trap->LinkEntity((sharedEntity_t *)hook);
}
