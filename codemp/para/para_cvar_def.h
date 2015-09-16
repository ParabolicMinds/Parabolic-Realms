#ifndef PARA_CVAR_DEF_H
#define PARA_CBAR_DEF_H

#define PCVAR_BG_INFAMMO			bgp_infammo							//infinite Ammo (disaples ammo depletion)
#define PCVAR_BG_INFFORCE			bgp_infforce						//infinite Force (disables force depletion)
#define PCVAR_BG_SABERLEN			bgp_saberlen						//saber length modifier (Players and NPCs)
#define PCVAR_BG_SUPERWEAPONS		bgp_superweapons					//weapon fire rate mod (2.0 -> twice as fast, 1.0 -> default, <= 0.0 -> break the game)

#define PCVAR_G_BODYTIMEMULT		gp_bodytmult						//player and npc dead body lifetime modifier
#define PCVAR_G_GHOOKDAMAGE			gp_ghookdamage						//how much damage the hook does on entity impact
#define PCVAR_G_GHOOKDAMP			gp_ghookdamp						//the dampening on the hook pull speed the closer you get to the hook
#define PCVAR_G_GHOOKSPEED			gp_ghookspeed						//velocity of the grappling hook
#define PCVAR_G_GHOOKPULLSPEED		gp_ghookpullspeed					//maximum velocity of the player being pulled by the hook
#define PCVAR_G_GOLF				gp_golf								//turns thermal detonators into golf balls
#define PCVAR_G_SBDROPVALID			gp_saberDropValidation				//prevents dropped sabers from getting stuck in walls

#define PCVAR_CG_TTS				cgp_tts								//espeak text-to-speech on chat

#endif
