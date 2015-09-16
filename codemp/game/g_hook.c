#include "g_local.h"

static void hook_grapple_think(gentity_t * hook) {
	if (hook->parent2) {
		vec3_t pold, pnew;
		VectorCopy(hook->s.origin2, pold);
		VectorCopy(hook->parent2->r.currentOrigin, pnew);
		if (!VectorCompare(pold, pnew)) {
			vec3_t diff;
			VectorSubtract(pnew, pold, diff);
			VectorAdd(hook->parent->client->ps.hookPos, diff, hook->parent->client->ps.hookPos);
			VectorCopy(hook->parent->client->ps.hookPos, hook->s.origin);
			G_SetOrigin(hook, hook->s.origin);
			VectorCopy(pnew, hook->s.origin2);
		}
	}
	if (hook->parent->client->pers.cmd.buttons & BUTTON_USE) {
		hook->parent->client->ps.hookState = 0;
		VectorClear(hook->parent->client->ps.hookPos);
		hook->parent->client->ps.hookDist = 0.0f;
		G_FreeEntity(hook);
		return;
	}
	hook->nextthink = level.time;
	if (hook->parent->client->pers.cmd.buttons & BUTTON_GRAPPLE_HOOK) {
		hook->parent->client->ps.hookState = 3;
		vec3_t hookPos, parentPos, diff;
		VectorCopy(hook->r.currentOrigin, hookPos);
		VectorCopy(hook->parent->client->ps.origin, parentPos);
		VectorSubtract(hookPos, parentPos, diff);
		hook->parent->client->ps.hookDist = VectorLength(diff);
		float mag = VectorNormalize(diff);
		mag *= 1 / PCVAR_G_GHOOKDAMP.value;
		if (mag > PCVAR_G_GHOOKPULLSPEED.value) mag = PCVAR_G_GHOOKPULLSPEED.value;
		VectorScale(diff, mag, diff);
		VectorCopy(diff, hook->parent->client->ps.velocity);
	} else if (hook->parent->client->pers.cmd.buttons & BUTTON_WALKING) {
		hook->parent->client->ps.hookState = 5;
		vec3_t hookPos, parentPos, diff;
		VectorCopy(hook->r.currentOrigin, hookPos);
		VectorCopy(hook->parent->client->ps.origin, parentPos);
		VectorSubtract(hookPos, parentPos, diff);
		hook->parent->client->ps.hookDist = VectorLength(diff);
	} else {
		hook->parent->client->ps.hookState = 2;
	}
}

static void hook_fly_think(gentity_t * hook) {
	vec3_t		origin, oldOrg;
	trace_t		tr;
	gentity_t	*traceEnt = NULL;

	if (hook->parent->client->pers.cmd.buttons & BUTTON_USE) {
		hook->parent->client->ps.hookState = 0;
		VectorClear(hook->parent->client->ps.hookPos);
		hook->parent->client->ps.hookDist = 0.0f;
		G_FreeEntity(hook);
		return;
	}

	hook->nextthink = level.time;

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
	if ( tr.fraction && traceEnt->client && traceEnt->takedamage ) G_Damage(traceEnt, hook, hook->parent, NULL, traceEnt->client->ps.origin, PCVAR_G_GHOOKDAMAGE.integer, DAMAGE_NORMAL, MOD_MELEE);

	hook->parent2 = traceEnt;
	VectorCopy(hook->parent2->r.currentOrigin, hook->s.origin2);

	VectorCopy(hook->r.currentOrigin, hook->s.origin);
	G_SetOrigin(hook, hook->s.origin);

	VectorCopy(hook->r.currentOrigin, hook->parent->client->ps.hookPos);
	vec3_t dist;
	VectorSubtract(hook->r.currentOrigin, hook->parent->client->ps.origin, dist);
	hook->parent->client->ps.hookDist = VectorLength(dist);

	hook->think = hook_grapple_think;
	hook->think(hook);
}

void G_GrappleHook(gentity_t * ent) {
	gentity_t * hook;

	if (!ent->client) return;

	gentity_t * from = NULL;
	while ( (from = G_Find( from, FOFS(classname), "grapple_hook" )) != NULL ) {
		if (from->parent == ent) {
			return;
		}
	}

	hook = G_Spawn();
	if(!hook) return;

	ent->client->ps.hookState = 1;
	VectorClear(ent->client->ps.hookPos);
	ent->client->ps.hookDist = 0.0f;

	hook->classname = "grapple_hook";

	VectorCopy(ent->client->ps.origin, hook->s.origin);
	VectorCopy(ent->client->ps.viewangles, hook->s.angles);
	hook->s.origin[2] += 10;
	G_SetOrigin(hook, hook->s.origin);
	G_SetAngles(hook, hook->s.angles);

	hook->s.owner = ent->s.number;
	hook->r.ownerNum = ent->s.number;
	hook->parent = ent;

	hook->s.modelindex = G_ModelIndex("models/items/ghook/ghook.obj");
	hook->s.eType = ET_GHOOK;

	VectorSet(hook->r.mins, -4, -4, -4);
	VectorSet(hook->r.maxs, 4, 4, 4);

	vec3_t delta;
	AngleVectors(ent->client->ps.viewangles, delta, NULL, NULL);
	VectorScale(delta, PCVAR_G_GHOOKSPEED.value, delta);
	VectorCopy(delta, hook->s.pos.trDelta);
	hook->s.pos.trTime = level.time;
	hook->s.pos.trType = TR_LINEAR;

	hook->clipmask = MASK_SHOT;

	hook->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	trap->LinkEntity((sharedEntity_t *)hook);

	hook->think = hook_fly_think;
	hook->think(hook);
}
