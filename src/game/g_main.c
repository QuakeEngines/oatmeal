/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2005-2009 Team OpenArena

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

/*
  ____       _______ __  __ ______          _      
 / __ \   /\|__   __|  \/  |  ____|   /\   | |     
| |  | | /  \  | |  | \  / | |__     /  \  | |     
| |  | |/ /\ \ | |  | |\/| |  __|   / /\ \ | |     
| |__| / ____ \| |  | |  | | |____ / ____ \| |____ 
 \____/_/    \_\_|  |_|  |_|______/_/    \_\______|

OpenArena Toolkit & Mod Environment for ALl
Copyright (C) 2009 Dan Hetrick

This file contains modifications to the Quake III Arena source code.
New code is for the "OpenArena Toolkit & Mod Environment for ALl"
(or "OATMEAL") modification for OpenArena.

OATMEAL source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

OATMEAL source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OATMEAL source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Alterned or new code is marked with "// oatmeal begin" before the code and
"// oatmeal end" after the code.

*/

//

#include "g_local.h"

level_locals_t	level;

typedef struct {
	vmCvar_t	*vmCvar;
	char		*cvarName;
	char		*defaultString;
	int			cvarFlags;
	int			modificationCount;  // for tracking changes
	qboolean	trackChange;	    // track this variable, and announce if changed
  qboolean teamShader;        // track and if changed, update shader state
} cvarTable_t;

gentity_t		g_entities[MAX_GENTITIES];
gclient_t		g_clients[MAX_CLIENTS];

vmCvar_t	g_gametype;
vmCvar_t	g_dmflags;
vmCvar_t	g_fraglimit;
vmCvar_t	g_timelimit;
vmCvar_t	g_capturelimit;
vmCvar_t	g_friendlyFire;
vmCvar_t	g_password;
vmCvar_t	g_needpass;
vmCvar_t	g_maxclients;
vmCvar_t	g_maxGameClients;
vmCvar_t	g_dedicated;
vmCvar_t	g_speed;
vmCvar_t	g_gravity;
vmCvar_t	g_cheats;
vmCvar_t	g_knockback;
vmCvar_t	g_quadfactor;
vmCvar_t	g_forcerespawn;
vmCvar_t	g_inactivity;
vmCvar_t	g_debugMove;
vmCvar_t	g_debugDamage;
vmCvar_t	g_debugAlloc;
vmCvar_t	g_weaponRespawn;
vmCvar_t	g_weaponTeamRespawn;
vmCvar_t	g_motd;
vmCvar_t	g_synchronousClients;
vmCvar_t	g_warmup;
vmCvar_t	g_doWarmup;
vmCvar_t	g_restarted;
vmCvar_t	g_logfile;
vmCvar_t	g_logfileSync;
vmCvar_t	g_blood;
vmCvar_t	g_podiumDist;
vmCvar_t	g_podiumDrop;
vmCvar_t	g_allowVote;
vmCvar_t	g_teamAutoJoin;
vmCvar_t	g_teamForceBalance;
vmCvar_t	g_banIPs;
vmCvar_t	g_filterBan;
vmCvar_t	g_smoothClients;
vmCvar_t	pmove_fixed;
vmCvar_t	pmove_msec;
vmCvar_t	g_rankings;
vmCvar_t	g_listEntity;
//#ifdef MISSIONPACK
vmCvar_t	g_obeliskHealth;
vmCvar_t	g_obeliskRegenPeriod;
vmCvar_t	g_obeliskRegenAmount;
vmCvar_t	g_obeliskRespawnDelay;
vmCvar_t	g_cubeTimeout;
#ifdef MISSIONPACK
vmCvar_t	g_redteam;
vmCvar_t	g_blueteam;
vmCvar_t	g_singlePlayer;
vmCvar_t	g_enableDust;
vmCvar_t	g_enableBreath;
#endif
vmCvar_t	g_proxMineTimeout;
vmCvar_t	g_music;
//#endif
//Following for elimination:
vmCvar_t	g_elimination_selfdamage;
vmCvar_t	g_elimination_startHealth;
vmCvar_t	g_elimination_startArmor;
vmCvar_t	g_elimination_bfg;
vmCvar_t	g_elimination_roundtime;
vmCvar_t	g_elimination_warmup;
vmCvar_t	g_elimination_activewarmup;
//more weapons in beta 9:
vmCvar_t	g_elimination_machinegun;
vmCvar_t	g_elimination_shotgun;
vmCvar_t	g_elimination_grenade;
vmCvar_t	g_elimination_rocket;
vmCvar_t	g_elimination_railgun;
vmCvar_t	g_elimination_lightning;
vmCvar_t	g_elimination_plasmagun;
//#ifdef MISSIONPACK
vmCvar_t	g_elimination_chain;
vmCvar_t	g_elimination_mine;
vmCvar_t	g_elimination_nail;
//#endif

vmCvar_t	g_rockets;

//dmn_clowns suggestions (with my idea of implementing):
vmCvar_t	g_instantgib;
vmCvar_t	g_vampire;
vmCvar_t	g_vampireMaxHealth;
//Regen
vmCvar_t	g_regen;
//Beta 4
int	g_ffa_gt; //Are this a FFA gametype even if gametype is high?
//beta 5
vmCvar_t	g_lms_lives;

//beta 8
vmCvar_t	g_lms_mode;

//beta 10
vmCvar_t	g_elimination_ctf_oneway;

//beta 18 (oa 0.7.6)
vmCvar_t        g_awardpushing; //The server can decide if players are awarded for pushing people in lave etc.

//new in beta 19
vmCvar_t        g_voteNames;
vmCvar_t        g_voteMaps;
vmCvar_t        g_voteGametypes;
vmCvar_t        g_voteMinTimelimit;
vmCvar_t        g_voteMaxTimelimit;
vmCvar_t        g_voteMinFraglimit;
vmCvar_t        g_voteMaxFraglimit;
//new in beta 19 end

//new in beta 23
vmCvar_t        g_humanplayers;
//new in beta 23 end

//unlagged - server options
vmCvar_t	g_delagHitscan;
vmCvar_t	g_truePing;
vmCvar_t	sv_fps;
//unlagged - server options

// oatmeal begin

// Custom weapon rate-of-fire cvars
vmCvar_t		om_lightning_rof;
vmCvar_t		om_chaingun_rof;
vmCvar_t		om_proxmine_rof;
vmCvar_t		om_rocket_rof;
vmCvar_t		om_bfg_rof;
vmCvar_t		om_plasma_rof;
vmCvar_t		om_grenade_rof;
vmCvar_t		om_machinegun_rof;
vmCvar_t		om_shotgun_rof;
vmCvar_t		om_railgun_rof;
vmCvar_t		om_nailgun_rof;
vmCvar_t		om_gauntlet_rof;
vmCvar_t		om_grappling_hook_rof;

// Machinegun cvars
vmCvar_t		om_machinegun_damage;
vmCvar_t		om_machinegun_team_damage;
vmCvar_t		om_machinegun_spread;

// Chaingun cvars
vmCvar_t		om_chaingun_damage;
vmCvar_t		om_chaingun_spread;

// Lightning Gun cvars
vmCvar_t		om_lightning_damage;
vmCvar_t		om_lightning_range;

// Damage Cvars
vmCvar_t		om_damage_multiplier;
vmCvar_t		om_self_damage_multiplier;

// Knockback Cvars
vmCvar_t		om_no_knockback;
vmCvar_t		om_knockback_minimum;
vmCvar_t		om_knockback_maximum;
vmCvar_t		om_knockback_multiplier;
vmCvar_t		om_knockback_mass;

// Player Cvars
vmCvar_t		om_player_weapons;
vmCvar_t		om_player_health;
vmCvar_t		om_player_armor;
vmCvar_t		om_countdown_health;
vmCvar_t		om_countdown_armor;

// Railgun Cvars
vmCvar_t		om_railgun_damage;
vmCvar_t		om_railgun_range;
vmCvar_t		om_railgun_maximum_hits;

// Shotgun Cvars
vmCvar_t		om_shotgun_damage;

// Gauntlet Cvars
vmCvar_t		om_gauntlet_damage;
vmCvar_t		om_gauntlet_range;

// Plasma Cvars
vmCvar_t		om_plasma_damage;
vmCvar_t		om_plasma_splash_damage;
vmCvar_t		om_plasma_splash_radius;
vmCvar_t		om_plasma_velocity;
vmCvar_t		om_plasma_homing;
vmCvar_t		om_plasma_vortex;
vmCvar_t		om_plasma_vortex_range;
vmCvar_t		om_plasma_vortex_strength;

// Grenade Cvars
vmCvar_t		om_grenade_damage;
vmCvar_t		om_grenade_splash_damage;
vmCvar_t		om_grenade_splash_radius;
vmCvar_t		om_grenade_velocity;

// BFG Cvars
vmCvar_t		om_bfg_damage;
vmCvar_t		om_bfg_splash_damage;
vmCvar_t		om_bfg_splash_radius;
vmCvar_t		om_bfg_velocity;

// Rocket Cvars
vmCvar_t		om_rocket_damage;
vmCvar_t		om_rocket_splash_damage;
vmCvar_t		om_rocket_splash_radius;
vmCvar_t		om_rocket_velocity;

// Grappling Hook Cvars
vmCvar_t		om_grappling_hook_velocity;

// Nailgun Cvars
vmCvar_t		om_nailgun_damage;
vmCvar_t		om_nailgun_spread;
vmCvar_t		om_nailgun_velocity;

// Proxmine Cvars
vmCvar_t		om_proximity_mine_damage;
vmCvar_t		om_proximity_mine_splash_damage;
vmCvar_t		om_proximity_mine_splash_radius;
vmCvar_t		om_proximity_mine_velocity;

vmCvar_t		om_rocket_homing;
vmCvar_t		om_rocket_vortex;
vmCvar_t		om_rocket_vortex_range;
vmCvar_t		om_rocket_vortex_strength;



vmCvar_t		om_grenade_homing;
vmCvar_t		om_grenade_vortex;
vmCvar_t		om_grenade_vortex_range;
vmCvar_t		om_grenade_vortex_strength;

vmCvar_t		om_bfg_homing;
vmCvar_t		om_bfg_vortex;
vmCvar_t		om_bfg_vortex_range;
vmCvar_t		om_bfg_vortex_strength;

vmCvar_t		om_nailgun_homing;
vmCvar_t		om_nailgun_vortex;
vmCvar_t		om_nailgun_vortex_range;
vmCvar_t		om_nailgun_vortex_strength;

vmCvar_t		om_proximity_mine_homing;
vmCvar_t		om_proximity_mine_vortex;
vmCvar_t		om_proximity_mine_vortex_range;
vmCvar_t		om_proximity_mine_vortex_strength;

vmCvar_t		om_plasma_linear;
vmCvar_t		om_grenade_linear;
vmCvar_t		om_rocket_linear;
vmCvar_t		om_bfg_linear;
vmCvar_t		om_nailgun_linear;
vmCvar_t		om_proximity_mine_linear;

vmCvar_t		om_plasma_lifetime;
vmCvar_t		om_grenade_lifetime;
vmCvar_t		om_rocket_lifetime;
vmCvar_t		om_bfg_lifetime;
vmCvar_t		om_nailgun_lifetime;
vmCvar_t		om_proximity_mine_lifetime;

vmCvar_t		om_lightning_unlimited_ammo;
vmCvar_t		om_chaingun_unlimited_ammo;
vmCvar_t		om_proxmine_unlimited_ammo;
vmCvar_t		om_rocket_unlimited_ammo;
vmCvar_t		om_bfg_unlimited_ammo;
vmCvar_t		om_plasma_unlimited_ammo;
vmCvar_t		om_grenade_unlimited_ammo;
vmCvar_t		om_machinegun_unlimited_ammo;
vmCvar_t		om_shotgun_unlimited_ammo;
vmCvar_t		om_railgun_unlimited_ammo;
vmCvar_t		om_nailgun_unlimited_ammo;

vmCvar_t		om_health_regen;
vmCvar_t		om_health_regen_amount;
vmCvar_t		om_health_regen_maximum;

vmCvar_t		om_armor_regen;
vmCvar_t		om_armor_regen_amount;
vmCvar_t		om_armor_regen_maximum;

vmCvar_t		om_lightning_vampire;
vmCvar_t		om_chaingun_vampire;
vmCvar_t		om_proxmine_vampire;
vmCvar_t		om_rocket_vampire;
vmCvar_t		om_bfg_vampire;
vmCvar_t		om_plasma_vampire;
vmCvar_t		om_grenade_vampire;
vmCvar_t		om_machinegun_vampire;
vmCvar_t		om_shotgun_vampire;
vmCvar_t		om_railgun_vampire;
vmCvar_t		om_nailgun_vampire;
vmCvar_t		om_vampire_maximum;

vmCvar_t		om_spawn_ammo;

vmCvar_t		om_lightning_spawn_ammo;
vmCvar_t		om_chaingun_spawn_ammo;
vmCvar_t		om_proxmine_spawn_ammo;
vmCvar_t		om_rocket_spawn_ammo;
vmCvar_t		om_bfg_spawn_ammo;
vmCvar_t		om_plasma_spawn_ammo;
vmCvar_t		om_grenade_spawn_ammo;
vmCvar_t		om_machinegun_spawn_ammo;
vmCvar_t		om_shotgun_spawn_ammo;
vmCvar_t		om_railgun_spawn_ammo;
vmCvar_t		om_nailgun_spawn_ammo;

vmCvar_t		om_spawn_health;
vmCvar_t		om_spawn_armor;

vmCvar_t		om_spawn_weapons;

vmCvar_t		om_lightning_spawn_weapon;
vmCvar_t		om_chaingun_spawn_weapon;
vmCvar_t		om_proxmine_spawn_weapon;
vmCvar_t		om_rocket_spawn_weapon;
vmCvar_t		om_bfg_spawn_weapon;
vmCvar_t		om_plasma_spawn_weapon;
vmCvar_t		om_grenade_spawn_weapon;
vmCvar_t		om_machinegun_spawn_weapon;
vmCvar_t		om_shotgun_spawn_weapon;
vmCvar_t		om_railgun_spawn_weapon;
vmCvar_t		om_nailgun_spawn_weapon;

vmCvar_t		om_spawn_powerups;

vmCvar_t		om_locational_damage;
vmCvar_t		om_location_face;
vmCvar_t		om_location_head;
vmCvar_t		om_location_throat;
vmCvar_t		om_location_shoulders;
vmCvar_t		om_location_torso;
vmCvar_t		om_location_arms;
vmCvar_t		om_location_stomach;
vmCvar_t		om_location_groin;
vmCvar_t		om_location_legs;
vmCvar_t		om_location_feet;

vmCvar_t		om_drop_ammo;

vmCvar_t		om_frag_drop_weapon;
vmCvar_t		om_frag_drop_powerup;

vmCvar_t		om_drop_health;

vmCvar_t		om_drop_armor;

// oatmeal end

// bk001129 - made static to avoid aliasing
static cvarTable_t		gameCvarTable[] = {
	// don't override the cheat state set by the system
	{ &g_cheats, "sv_cheats", "", 0, 0, qfalse },

	// noset vars
	{ NULL, "gamename", GAMEVERSION , CVAR_SERVERINFO | CVAR_ROM, 0, qfalse  },
	{ NULL, "gamedate", __DATE__ , CVAR_ROM, 0, qfalse  },
	{ &g_restarted, "g_restarted", "0", CVAR_ROM, 0, qfalse  },
	{ NULL, "sv_mapname", "", CVAR_SERVERINFO | CVAR_ROM, 0, qfalse  },

	// latched vars
	{ &g_gametype, "g_gametype", "0", CVAR_SERVERINFO | CVAR_USERINFO | CVAR_LATCH, 0, qfalse  },

	{ &g_maxclients, "sv_maxclients", "8", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse  },
	{ &g_maxGameClients, "g_maxGameClients", "0", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse  },

	// oatmeal begin

	// Custom weapon rate-of-fire cvars
	{ &om_lightning_rof, "lightning_rof", "50", 0, 0, qtrue  },
	{ &om_chaingun_rof, "chaingun_rof", "30", 0, 0, qtrue  },
	{ &om_proxmine_rof, "proxmine_rof", "800", 0, 0, qtrue  },
	{ &om_rocket_rof, "rocket_rof", "800", 0, 0, qtrue  },
	{ &om_bfg_rof, "bfg_rof", "200", 0, 0, qtrue  },
	{ &om_plasma_rof, "plasma_rof", "100", 0, 0, qtrue  },
	{ &om_grenade_rof, "grenade_rof", "800", 0, 0, qtrue  },
	{ &om_machinegun_rof, "machinegun_rof", "100", 0, 0, qtrue  },
	{ &om_shotgun_rof, "shotgun_rof", "1000", 0, 0, qtrue  },
	{ &om_railgun_rof, "railgun_rof", "1500", 0, 0, qtrue  },
	{ &om_nailgun_rof, "nailgun_rof", "1000", 0, 0, qtrue  },
	{ &om_gauntlet_rof, "gauntlet_rof", "400", 0, 0, qtrue  },
	{ &om_grappling_hook_rof, "grappling_hook_rof", "400", 0, 0, qtrue  },

	// Machinegun cvars
	{ &om_machinegun_damage, "machinegun_damage", "7", 0, 0, qtrue  },
	{ &om_machinegun_team_damage, "machinegun_team_damage", "5", 0, 0, qtrue  },
	{ &om_machinegun_spread, "machinegun_spread", "200", 0, 0, qtrue  },

	// Chaingun cvars
	{ &om_chaingun_damage, "chaingun_damage", "5", 0, 0, qtrue  },
	{ &om_chaingun_spread, "chaingun_spread", "600", 0, 0, qtrue  },

	// Lightning Gun cvars
	{ &om_lightning_damage, "lightning_damage", "8", 0, 0, qtrue  },
	{ &om_lightning_range, "lightning_range", "768", 0, 0, qtrue  },

	// Damage cvars
	{ &om_damage_multiplier, "damage_multiplier", "1.0", 0, 0, qtrue  },
	{ &om_self_damage_multiplier, "self_damage_multiplier", "0.5", 0, 0, qtrue  },

	// Knockback cvars
	{ &om_no_knockback, "no_knockback", "0", 0, 0, qtrue  },
	{ &om_knockback_minimum, "knockback_minimum", "0", 0, 0, qtrue  },
	{ &om_knockback_maximum, "knockback_maximum", "200", 0, 0, qtrue  },
	{ &om_knockback_multiplier, "knockback_multiplier", "1.0", 0, 0, qtrue  },
	{ &om_knockback_mass, "knockback_mass", "200", 0, 0, qtrue  },

	// Player cvars
	{ &om_player_weapons, "player_weapons", "1", 0, 0, qtrue  },
	{ &om_player_health, "player_health", "100", 0, 0, qtrue  },
	{ &om_player_armor, "player_armor", "0", 0, 0, qtrue  },
	{ &om_countdown_health, "countdown_health", "1", 0, 0, qtrue  },
	{ &om_countdown_armor, "countdown_armor", "1", 0, 0, qtrue  },

	// Railgun cvars
	{ &om_railgun_damage, "railgun_damage", "100", 0, 0, qtrue  },
	{ &om_railgun_range, "railgun_range", "8192", 0, 0, qtrue  },
	{ &om_railgun_maximum_hits, "railgun_maximum_hits", "4", 0, 0, qtrue  },

	// Shotgun cvars
	{ &om_shotgun_damage, "shotgun_damage", "10", 0, 0, qtrue  },

	// Gauntlet cvars
	{ &om_gauntlet_damage, "gauntlet_damage", "50", 0, 0, qtrue  },
	{ &om_gauntlet_range, "gauntlet_range", "32", 0, 0, qtrue  },

	// Plasma cvars
	{ &om_plasma_damage, "plasma_damage", "20", 0, 0, qtrue  },
	{ &om_plasma_velocity, "plasma_velocity", "2000", 0, 0, qtrue  },
	{ &om_plasma_splash_damage, "plasma_splash_damage", "15", 0, 0, qtrue  },
	{ &om_plasma_splash_radius, "plasma_splash_radius", "20", 0, 0, qtrue  },

	// Grenade cvars
	{ &om_grenade_damage, "grenade_damage", "100", 0, 0, qtrue  },
	{ &om_grenade_velocity, "grenade_velocity", "700", 0, 0, qtrue  },
	{ &om_grenade_splash_damage, "grenade_splash_damage", "100", 0, 0, qtrue  },
	{ &om_grenade_splash_radius, "grenade_splash_radius", "150", 0, 0, qtrue  },

	// BFG cvars
	{ &om_bfg_damage, "bfg_damage", "100", 0, 0, qtrue  },
	{ &om_bfg_velocity, "bfg_velocity", "2000", 0, 0, qtrue  },
	{ &om_bfg_splash_damage, "bfg_splash_damage", "100", 0, 0, qtrue  },
	{ &om_bfg_splash_radius, "bfg_splash_radius", "120", 0, 0, qtrue  },

	// Rocket cvars
	{ &om_rocket_damage, "rocket_damage", "100", 0, 0, qtrue  },
	{ &om_rocket_velocity, "rocket_velocity", "900", 0, 0, qtrue  },
	{ &om_rocket_splash_damage, "rocket_splash_damage", "100", 0, 0, qtrue  },
	{ &om_rocket_splash_radius, "rocket_splash_radius", "120", 0, 0, qtrue  },

	// Grappling Hook cvars
	{ &om_grappling_hook_velocity, "grappling_hook_velocity", "800", 0, 0, qtrue  },

	// Nailgun cvars
	{ &om_nailgun_damage, "nailgun_damage", "20", 0, 0, qtrue  },
	{ &om_nailgun_spread, "nailgun_spread", "500", 0, 0, qtrue  },
	{ &om_nailgun_velocity, "nailgun_velocity", "555", 0, 0, qtrue  },

	// Proxmine cvars
	{ &om_proximity_mine_damage, "proximity_mine_damage", "0", 0, 0, qtrue  },
	{ &om_proximity_mine_velocity, "proximity_mine_velocity", "700", 0, 0, qtrue  },
	{ &om_proximity_mine_splash_damage, "proximity_mine_splash_damage", "100", 0, 0, qtrue  },
	{ &om_proximity_mine_splash_radius, "proximity_mine_splash_radius", "150", 0, 0, qtrue  },

	{ &om_plasma_homing, "plasma_homing", "0", 0, 0, qtrue  },
	{ &om_plasma_vortex, "plasma_vortex", "0", 0, 0, qtrue  },
	{ &om_plasma_vortex_range, "plasma_vortex_range", "500", 0, 0, qtrue  },
	{ &om_plasma_vortex_strength, "plasma_vortex_strength", "200", 0, 0, qtrue  },

	{ &om_rocket_homing, "rocket_homing", "0", 0, 0, qtrue  },
	{ &om_rocket_vortex, "rocket_vortex", "0", 0, 0, qtrue  },
	{ &om_rocket_vortex_range, "rocket_vortex_range", "500", 0, 0, qtrue  },
	{ &om_rocket_vortex_strength, "rocket_vortex_strength", "200", 0, 0, qtrue  },


	{ &om_grenade_homing, "grenade_homing", "0", 0, 0, qtrue  },
	{ &om_grenade_vortex, "grenade_vortex", "0", 0, 0, qtrue  },
	{ &om_grenade_vortex_range, "grenade_vortex_range", "500", 0, 0, qtrue  },
	{ &om_grenade_vortex_strength, "grenade_vortex_strength", "200", 0, 0, qtrue  },

	{ &om_bfg_homing, "bfg_homing", "0", 0, 0, qtrue  },
	{ &om_bfg_vortex, "bfg_vortex", "0", 0, 0, qtrue  },
	{ &om_bfg_vortex_range, "bfg_vortex_range", "500", 0, 0, qtrue  },
	{ &om_bfg_vortex_strength, "bfg_vortex_strength", "200", 0, 0, qtrue  },

	{ &om_nailgun_homing, "nailgun_homing", "0", 0, 0, qtrue  },
	{ &om_nailgun_vortex, "nailgun_vortex", "0", 0, 0, qtrue  },
	{ &om_nailgun_vortex_range, "nailgun_vortex_range", "500", 0, 0, qtrue  },
	{ &om_nailgun_vortex_strength, "nailgun_vortex_strength", "200", 0, 0, qtrue  },

	{ &om_proximity_mine_homing, "proximity_mine_homing", "0", 0, 0, qtrue  },
	{ &om_proximity_mine_vortex, "proximity_mine_vortex", "0", 0, 0, qtrue  },
	{ &om_proximity_mine_vortex_range, "proximity_mine_vortex_range", "500", 0, 0, qtrue  },
	{ &om_proximity_mine_vortex_strength, "proximity_mine_vortex_strength", "200", 0, 0, qtrue  },

	{ &om_plasma_linear, "plasma_linear", "1", 0, 0, qtrue  },
	{ &om_rocket_linear, "rocket_linear", "1", 0, 0, qtrue  },
	{ &om_grenade_linear, "grenade_linear", "0", 0, 0, qtrue  },
	{ &om_bfg_linear, "bfg_linear", "1", 0, 0, qtrue  },
	{ &om_nailgun_linear, "nailgun_linear", "1", 0, 0, qtrue  },
	{ &om_proximity_mine_linear, "proximity_mine_linear", "0", 0, 0, qtrue  },

	{ &om_plasma_lifetime, "plasma_lifetime", "10000", 0, 0, qtrue  },
	{ &om_rocket_lifetime, "rocket_lifetime", "10000", 0, 0, qtrue  },
	{ &om_grenade_lifetime, "grenade_lifetime", "10000", 0, 0, qtrue  },
	{ &om_bfg_lifetime, "bfg_lifetime", "10000", 0, 0, qtrue  },
	{ &om_nailgun_lifetime, "nailgun_lifetime", "10000", 0, 0, qtrue  },
	{ &om_proximity_mine_lifetime, "proximity_mine_lifetime", "10000", 0, 0, qtrue  },

	{ &om_lightning_unlimited_ammo, "lightning_unlimited_ammo", "0", 0, 0, qtrue  },
	{ &om_chaingun_unlimited_ammo, "chaingun_unlimited_ammo", "0", 0, 0, qtrue  },
	{ &om_proxmine_unlimited_ammo, "proxmine_unlimited_ammo", "0", 0, 0, qtrue  },
	{ &om_rocket_unlimited_ammo, "rocket_unlimited_ammo", "0", 0, 0, qtrue  },
	{ &om_bfg_unlimited_ammo, "bfg_unlimited_ammo", "0", 0, 0, qtrue  },
	{ &om_plasma_unlimited_ammo, "plasma_unlimited_ammo", "0", 0, 0, qtrue  },
	{ &om_grenade_unlimited_ammo, "grenade_unlimited_ammo", "0", 0, 0, qtrue  },
	{ &om_machinegun_unlimited_ammo, "machinegun_unlimited_ammo", "0", 0, 0, qtrue  },
	{ &om_shotgun_unlimited_ammo, "shotgun_unlimited_ammo", "0", 0, 0, qtrue  },
	{ &om_railgun_unlimited_ammo, "railgun_unlimited_ammo", "0", 0, 0, qtrue  },
	{ &om_nailgun_unlimited_ammo, "nailgun_unlimited_ammo", "0", 0, 0, qtrue  },

	{ &om_health_regen, "health_regen", "0", 0, 0, qtrue  },
	{ &om_health_regen_amount, "health_regen_amount", "1", 0, 0, qtrue  },
	{ &om_health_regen_maximum, "health_regen_maximum", "100", 0, 0, qtrue  },

	{ &om_armor_regen, "armor_regen", "0", 0, 0, qtrue  },
	{ &om_armor_regen_amount, "armor_regen_amount", "1", 0, 0, qtrue  },
	{ &om_armor_regen_maximum, "armor_regen_maximum", "100", 0, 0, qtrue  },

	{ &om_lightning_vampire, "lightning_vampire", "0", 0, 0, qtrue  },
	{ &om_chaingun_vampire, "chaingun_vampire", "0", 0, 0, qtrue  },
	{ &om_proxmine_vampire, "proxmine_vampire", "0", 0, 0, qtrue  },
	{ &om_rocket_vampire, "rocket_vampire", "0", 0, 0, qtrue  },
	{ &om_bfg_vampire, "bfg_vampire", "0", 0, 0, qtrue  },
	{ &om_plasma_vampire, "plasma_vampire", "0", 0, 0, qtrue  },
	{ &om_grenade_vampire, "grenade_vampire", "0", 0, 0, qtrue  },
	{ &om_machinegun_vampire, "machinegun_vampire", "0", 0, 0, qtrue  },
	{ &om_shotgun_vampire, "shotgun_vampire", "0", 0, 0, qtrue  },
	{ &om_railgun_vampire, "railgun_vampire", "0", 0, 0, qtrue  },
	{ &om_nailgun_vampire, "nailgun_vampire", "0", 0, 0, qtrue  },

	{ &om_vampire_maximum, "vampire_maximum", "100", 0, 0, qtrue  },

	{ &om_spawn_ammo, "spawn_ammo", "1", 0, 0, qtrue  },

	{ &om_lightning_spawn_ammo, "lightning_spawn_ammo", "1", 0, 0, qtrue  },
	{ &om_chaingun_spawn_ammo, "chaingun_spawn_ammo", "1", 0, 0, qtrue  },
	{ &om_proxmine_spawn_ammo, "proxmine_spawn_ammo", "1", 0, 0, qtrue  },
	{ &om_rocket_spawn_ammo, "rocket_spawn_ammo", "1", 0, 0, qtrue  },
	{ &om_bfg_spawn_ammo, "bfg_spawn_ammo", "1", 0, 0, qtrue  },
	{ &om_plasma_spawn_ammo, "plasma_spawn_ammo", "1", 0, 0, qtrue  },
	{ &om_grenade_spawn_ammo, "grenade_spawn_ammo", "1", 0, 0, qtrue  },
	{ &om_machinegun_spawn_ammo, "machinegun_spawn_ammo", "1", 0, 0, qtrue  },
	{ &om_shotgun_spawn_ammo, "shotgun_spawn_ammo", "1", 0, 0, qtrue  },
	{ &om_railgun_spawn_ammo, "railgun_spawn_ammo", "1", 0, 0, qtrue  },
	{ &om_nailgun_spawn_ammo, "nailgun_spawn_ammo", "1", 0, 0, qtrue  },

	{ &om_spawn_health, "spawn_health", "1", 0, 0, qtrue  },
	{ &om_spawn_armor, "spawn_armor", "1", 0, 0, qtrue  },

	{ &om_spawn_weapons, "spawn_weapons", "1", 0, 0, qtrue  },

	{ &om_lightning_spawn_weapon, "lightning_spawn_weapon", "1", 0, 0, qtrue  },
	{ &om_chaingun_spawn_weapon, "chaingun_spawn_weapon", "1", 0, 0, qtrue  },
	{ &om_proxmine_spawn_weapon, "proxmine_spawn_weapon", "1", 0, 0, qtrue  },
	{ &om_rocket_spawn_weapon, "rocket_spawn_weapon", "1", 0, 0, qtrue  },
	{ &om_bfg_spawn_weapon, "bfg_spawn_weapon", "1", 0, 0, qtrue  },
	{ &om_plasma_spawn_weapon, "plasma_spawn_weapon", "1", 0, 0, qtrue  },
	{ &om_grenade_spawn_weapon, "grenade_spawn_weapon", "1", 0, 0, qtrue  },
	{ &om_machinegun_spawn_weapon, "machinegun_spawn_weapon", "1", 0, 0, qtrue  },
	{ &om_shotgun_spawn_weapon, "shotgun_spawn_weapon", "1", 0, 0, qtrue  },
	{ &om_railgun_spawn_weapon, "railgun_spawn_weapon", "1", 0, 0, qtrue  },
	{ &om_nailgun_spawn_weapon, "nailgun_spawn_weapon", "1", 0, 0, qtrue  },

	{ &om_spawn_powerups, "spawn_powerups", "1", 0, 0, qtrue  },

	{ &om_locational_damage, "locational_damage", "0", 0, 0, qtrue  },
	{ &om_location_face, "location_face", "5.0", 0, 0, qtrue  },
	{ &om_location_head, "location_head", "1.8", 0, 0, qtrue  },
	{ &om_location_throat, "location_throat", "1.4", 0, 0, qtrue  },
	{ &om_location_shoulders, "location_shoulders", "1.1", 0, 0, qtrue  },
	{ &om_location_torso, "location_torso", "1.3", 0, 0, qtrue  },
	{ &om_location_arms, "location_arms", "0.8", 0, 0, qtrue  },
	{ &om_location_stomach, "location_stomach", "1.2", 0, 0, qtrue  },
	{ &om_location_groin, "location_groin", "1.3", 0, 0, qtrue  },
	{ &om_location_legs, "location_legs", "0.7", 0, 0, qtrue  },
	{ &om_location_feet, "location_feet", "0.5", 0, 0, qtrue  },

	{ &om_drop_ammo, "drop_ammo", "0", 0, 0, qtrue  },

	{ &om_frag_drop_weapon, "frag_drop_weapon", "1", 0, 0, qtrue  },
	{ &om_frag_drop_powerup, "frag_drop_powerup", "1", 0, 0, qtrue  },

	{ &om_drop_health, "drop_health", "0", 0, 0, qtrue  },

	{ &om_drop_armor, "drop_armor", "0", 0, 0, qtrue  },

	// oatmeal end

	// change anytime vars
	{ &g_dmflags, "dmflags", "0", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qtrue  },
	{ &g_fraglimit, "fraglimit", "20", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
	{ &g_timelimit, "timelimit", "0", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
	{ &g_capturelimit, "capturelimit", "8", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },

	{ &g_synchronousClients, "g_synchronousClients", "0", CVAR_SYSTEMINFO, 0, qfalse  },

	{ &g_friendlyFire, "g_friendlyFire", "0", CVAR_ARCHIVE, 0, qtrue  },

	{ &g_teamAutoJoin, "g_teamAutoJoin", "0", CVAR_ARCHIVE  },
	{ &g_teamForceBalance, "g_teamForceBalance", "0", CVAR_ARCHIVE  },

	{ &g_warmup, "g_warmup", "20", CVAR_ARCHIVE, 0, qtrue  },
	{ &g_doWarmup, "g_doWarmup", "0", 0, 0, qtrue  },
	{ &g_logfile, "g_log", "games.log", CVAR_ARCHIVE, 0, qfalse  },
	{ &g_logfileSync, "g_logsync", "0", CVAR_ARCHIVE, 0, qfalse  },

	{ &g_password, "g_password", "", CVAR_USERINFO, 0, qfalse  },

	{ &g_banIPs, "g_banIPs", "", CVAR_ARCHIVE, 0, qfalse  },
	{ &g_filterBan, "g_filterBan", "1", CVAR_ARCHIVE, 0, qfalse  },

	{ &g_needpass, "g_needpass", "0", CVAR_SERVERINFO | CVAR_ROM, 0, qfalse },

	{ &g_dedicated, "dedicated", "0", 0, 0, qfalse  },

	{ &g_speed, "g_speed", "320", 0, 0, qtrue  },
	{ &g_gravity, "g_gravity", "800", 0, 0, qtrue  },
	{ &g_knockback, "g_knockback", "1000", 0, 0, qtrue  },
	{ &g_quadfactor, "g_quadfactor", "3", 0, 0, qtrue  },
	{ &g_weaponRespawn, "g_weaponrespawn", "5", 0, 0, qtrue  },
	{ &g_weaponTeamRespawn, "g_weaponTeamRespawn", "30", 0, 0, qtrue },
	{ &g_forcerespawn, "g_forcerespawn", "20", 0, 0, qtrue },
	{ &g_inactivity, "g_inactivity", "0", 0, 0, qtrue },
	{ &g_debugMove, "g_debugMove", "0", 0, 0, qfalse },
	{ &g_debugDamage, "g_debugDamage", "0", 0, 0, qfalse },
	{ &g_debugAlloc, "g_debugAlloc", "0", 0, 0, qfalse },
	{ &g_motd, "g_motd", "", 0, 0, qfalse },
	{ &g_blood, "com_blood", "1", 0, 0, qfalse },

	{ &g_podiumDist, "g_podiumDist", "80", 0, 0, qfalse },
	{ &g_podiumDrop, "g_podiumDrop", "70", 0, 0, qfalse },

	{ &g_allowVote, "g_allowVote", "1", CVAR_ARCHIVE, 0, qfalse },
        
        //new in beta 19
        { &g_voteNames, "g_voteNames", "/map_restart/nextmap/map/g_gametype/kick/clientkick/g_doWarmup/timelimit/fraglimit/", CVAR_ARCHIVE, 0, qfalse }, //clientkick g_doWarmup timelimit fraglimit
        { &g_voteMaps, "g_voteMaps", "*", CVAR_ARCHIVE, 0, qfalse },
        { &g_voteGametypes, "g_voteGametypes", "/0/1/3/4/8/9/10/11/", CVAR_ARCHIVE, 0, qfalse },
        { &g_voteMaxTimelimit, "g_voteMaxTimelimit", "0", CVAR_ARCHIVE, 0, qfalse },
        { &g_voteMinTimelimit, "g_voteMinTimelimit", "0", CVAR_ARCHIVE, 0, qfalse },
        { &g_voteMaxFraglimit, "g_voteMaxFraglimit", "0", CVAR_ARCHIVE, 0, qfalse },
        { &g_voteMinFraglimit, "g_voteMinFraglimit", "0", CVAR_ARCHIVE, 0, qfalse },
        //new in beta 19 end
        
	{ &g_listEntity, "g_listEntity", "0", 0, 0, qfalse },

//#ifdef MISSIONPACK
	{ &g_obeliskHealth, "g_obeliskHealth", "2500", 0, 0, qfalse },
	{ &g_obeliskRegenPeriod, "g_obeliskRegenPeriod", "1", 0, 0, qfalse },
	{ &g_obeliskRegenAmount, "g_obeliskRegenAmount", "15", 0, 0, qfalse },
	{ &g_obeliskRespawnDelay, "g_obeliskRespawnDelay", "10", CVAR_SERVERINFO, 0, qfalse },

	{ &g_cubeTimeout, "g_cubeTimeout", "30", 0, 0, qfalse },
        #ifdef MISSIONPACK
	{ &g_redteam, "g_redteam", "Stroggs", CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_USERINFO , 0, qtrue, qtrue },
	{ &g_blueteam, "g_blueteam", "Pagans", CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_USERINFO , 0, qtrue, qtrue  },
	{ &g_singlePlayer, "ui_singlePlayerActive", "", 0, 0, qfalse, qfalse  },

	{ &g_enableDust, "g_enableDust", "0", CVAR_SERVERINFO, 0, qtrue, qfalse },
	{ &g_enableBreath, "g_enableBreath", "0", CVAR_SERVERINFO, 0, qtrue, qfalse },
        #endif
	{ &g_proxMineTimeout, "g_proxMineTimeout", "20000", 0, 0, qfalse },
//#endif
	{ &g_smoothClients, "g_smoothClients", "1", 0, 0, qfalse},
	{ &pmove_fixed, "pmove_fixed", "0", CVAR_SYSTEMINFO, 0, qfalse},
	{ &pmove_msec, "pmove_msec", "8", CVAR_SYSTEMINFO, 0, qfalse},

//unlagged - server options
	{ &g_delagHitscan, "g_delagHitscan", "0", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qtrue },
	{ &g_truePing, "g_truePing", "0", CVAR_ARCHIVE, 0, qtrue },
	// it's CVAR_SYSTEMINFO so the client's sv_fps will be automagically set to its value
	{ &sv_fps, "sv_fps", "20", CVAR_SYSTEMINFO | CVAR_ARCHIVE, 0, qfalse },
//unlagged - server options

	{ &g_rankings, "g_rankings", "0", 0, 0, qfalse},
        { &g_music, "g_music", "", 0, 0, qfalse},
	//Now for elimination stuff:
	{ &g_elimination_selfdamage, "elimination_selfdamage", "0", 0, 0, qtrue },
	{ &g_elimination_startHealth, "elimination_startHealth", "200", CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_startArmor, "elimination_startArmor", "200", CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_bfg, "elimination_bfg", "0", CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_roundtime, "elimination_roundtime", "90", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_warmup, "elimination_warmup", "7", 0, 0, qtrue },
	{ &g_elimination_activewarmup, "elimination_activewarmup", "5", 0, 0, qtrue },
//more weapons in beta 9:
	{ &g_elimination_machinegun, "elimination_machinegun", "200", CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_shotgun, "elimination_shotgun", "200", CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_grenade, "elimination_grenade", "200", CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_rocket, "elimination_rocket", "200", CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_railgun, "elimination_railgun", "200", CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_lightning, "elimination_lightning", "200", CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_plasmagun, "elimination_plasmagun", "200", CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },
//#ifdef MISSIONPACK
	{ &g_elimination_chain, "elimination_chain", "0", CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_mine, "elimination_mine", "0", CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_nail, "elimination_nail", "0", CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },
//#endif
	//Beta 10
	{ &g_elimination_ctf_oneway, "elimination_ctf_oneway", "0", CVAR_SERVERINFO | CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },

        
        { &g_awardpushing, "g_awardpushing", "0", CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },    
        
	//nexuiz style rocket arena
	{ &g_rockets, "g_rockets", "0", CVAR_SERVERINFO | CVAR_LATCH | CVAR_NORESTART, 0, qfalse },

	//Instantgib and Vampire thingies
	{ &g_instantgib, "g_instantgib", "0", CVAR_SERVERINFO | CVAR_LATCH | CVAR_NORESTART, 0, qfalse },
	{ &g_vampire, "g_vampire", "0.0", CVAR_NORESTART, 0, qtrue },
	{ &g_regen, "g_regen", "0", CVAR_NORESTART, 0, qtrue },
	{ &g_vampireMaxHealth, "g_vampire_max_health", "500", CVAR_NORESTART, 0, qtrue },
	//beta 5
	{ &g_lms_lives, "g_lms_lives", "1", CVAR_NORESTART, 0, qtrue },
	//beta 8
	{ &g_lms_mode, "g_lms_mode", "0", CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
        //beta 23
        { &g_humanplayers, "g_humanplayers", "0", CVAR_SERVERINFO | CVAR_NORESTART, 0, qfalse },
};

// bk001129 - made static to avoid aliasing
static int gameCvarTableSize = sizeof( gameCvarTable ) / sizeof( gameCvarTable[0] );


void G_InitGame( int levelTime, int randomSeed, int restart );
void G_RunFrame( int levelTime );
void G_ShutdownGame( int restart );
void CheckExitRules( void );


/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .q3vm file
================
*/
intptr_t vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11  ) {
	switch ( command ) {
	case GAME_INIT:
		G_InitGame( arg0, arg1, arg2 );
		return 0;
	case GAME_SHUTDOWN:
		G_ShutdownGame( arg0 );
		return 0;
	case GAME_CLIENT_CONNECT:
		return (intptr_t)ClientConnect( arg0, arg1, arg2 );
	case GAME_CLIENT_THINK:
		ClientThink( arg0 );
		return 0;
	case GAME_CLIENT_USERINFO_CHANGED:
		ClientUserinfoChanged( arg0 );
		return 0;
	case GAME_CLIENT_DISCONNECT:
		ClientDisconnect( arg0 );
		return 0;
	case GAME_CLIENT_BEGIN:
		ClientBegin( arg0 );
		return 0;
	case GAME_CLIENT_COMMAND:
		ClientCommand( arg0 );
		return 0;
	case GAME_RUN_FRAME:
		G_RunFrame( arg0 );
		return 0;
	case GAME_CONSOLE_COMMAND:
		return ConsoleCommand();
	case BOTAI_START_FRAME:
		return BotAIStartFrame( arg0 );
	}

	return -1;
}


void QDECL G_Printf( const char *fmt, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, fmt);
	Q_vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	trap_Printf( text );
}

void QDECL G_Error( const char *fmt, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, fmt);
	Q_vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	trap_Error( text );
}

/*
================
G_FindTeams

Chain together all entities with a matching team field.
Entity teams are used for item groups and multi-entity mover groups.

All but the first will have the FL_TEAMSLAVE flag set and teammaster field set
All but the last will have the teamchain field set to the next one
================
*/
void G_FindTeams( void ) {
	gentity_t	*e, *e2;
	int		i, j;
	int		c, c2;

	c = 0;
	c2 = 0;
	for ( i=1, e=g_entities+i ; i < level.num_entities ; i++,e++ ){
		if (!e->inuse)
			continue;
		if (!e->team)
			continue;
		if (e->flags & FL_TEAMSLAVE)
			continue;
		e->teammaster = e;
		c++;
		c2++;
		for (j=i+1, e2=e+1 ; j < level.num_entities ; j++,e2++)
		{
			if (!e2->inuse)
				continue;
			if (!e2->team)
				continue;
			if (e2->flags & FL_TEAMSLAVE)
				continue;
			if (!strcmp(e->team, e2->team))
			{
				c2++;
				e2->teamchain = e->teamchain;
				e->teamchain = e2;
				e2->teammaster = e;
				e2->flags |= FL_TEAMSLAVE;

				// make sure that targets only point at the master
				if ( e2->targetname ) {
					e->targetname = e2->targetname;
					e2->targetname = NULL;
				}
			}
		}
	}

	G_Printf ("%i teams with %i entities\n", c, c2);
}

void G_RemapTeamShaders( void ) {
#ifdef MISSIONPACK
	char string[1024];
	float f = level.time * 0.001;
	Com_sprintf( string, sizeof(string), "team_icon/%s_red", g_redteam.string );
	AddRemap("textures/ctf2/redteam01", string, f); 
	AddRemap("textures/ctf2/redteam02", string, f); 
	Com_sprintf( string, sizeof(string), "team_icon/%s_blue", g_blueteam.string );
	AddRemap("textures/ctf2/blueteam01", string, f); 
	AddRemap("textures/ctf2/blueteam02", string, f); 
	trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
#endif
}


/*
=================
G_RegisterCvars
=================
*/
void G_RegisterCvars( void ) {
	int			i;
	cvarTable_t	*cv;
	qboolean remapped = qfalse;

	for ( i = 0, cv = gameCvarTable ; i < gameCvarTableSize ; i++, cv++ ) {
		trap_Cvar_Register( cv->vmCvar, cv->cvarName,
			cv->defaultString, cv->cvarFlags );
		if ( cv->vmCvar )
			cv->modificationCount = cv->vmCvar->modificationCount;

		if (cv->teamShader) {
			remapped = qtrue;
		}
	}

	if (remapped) {
		G_RemapTeamShaders();
	}

	// check some things
	if ( g_gametype.integer < 0 || g_gametype.integer >= GT_MAX_GAME_TYPE ) {
		G_Printf( "g_gametype %i is out of range, defaulting to 0\n", g_gametype.integer );
		trap_Cvar_Set( "g_gametype", "0" );
	}

	//set FFA status for high gametypes:
	if ( g_gametype.integer == GT_LMS ) {
		g_ffa_gt = 1;	//Last Man standig is a FFA gametype
	} else {
		g_ffa_gt = 0;	//If >GT_CTF use bases
	}

	level.warmupModificationCount = g_warmup.modificationCount;
}

/*
=================
G_UpdateCvars
=================
*/
void G_UpdateCvars( void ) {
	int			i;
	cvarTable_t	*cv;
	qboolean remapped = qfalse;

	for ( i = 0, cv = gameCvarTable ; i < gameCvarTableSize ; i++, cv++ ) {
		if ( cv->vmCvar ) {
			trap_Cvar_Update( cv->vmCvar );

			if ( cv->modificationCount != cv->vmCvar->modificationCount ) {
				cv->modificationCount = cv->vmCvar->modificationCount;

				if ( cv->trackChange ) {
					trap_SendServerCommand( -1, va("print \"Server: %s changed to %s\n\"", 
						cv->cvarName, cv->vmCvar->string ) );
				}

				if (cv->teamShader) {
					remapped = qtrue;
				}
			}
		}
	}

	if (remapped) {
		G_RemapTeamShaders();
	}
}

// oatmeal begin

// oatmeal_script_exists
// returns 1 if file exists, 0 if not
int oatmeal_script_exists( char *filename ){
	int				len;
	fileHandle_t	f;

	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( !f ) {
		trap_FS_FCloseFile( f );
		return 0;
	} else {
		trap_FS_FCloseFile( f );
		return 1;
	}
}

// oatmeal_scan_and_load_mods
// Scans a directory (mods/) for config files
// and executes them
void oatmeal_scan_and_load_mods( void ) {
	int		numdirs;
	char	dirlist[2048];
	char*	dirptr;
	int		i;
	int		dirlen;
	char*		filename;
	int		numfiles;
	char	filelist[2048];
	char*	fileptr;
	int		j;
	int		filelen;

	G_Printf ("Scanning for OATMEAL mods...\n\n");

	numdirs = trap_FS_GetFileList("mods", "/", dirlist, 2048 );
	dirptr  = dirlist;
	for (i=0; i<numdirs; i++,dirptr+=dirlen+1)
	{
		dirlen = strlen(dirptr);
		
		if (dirlen && dirptr[dirlen-1]=='/') dirptr[dirlen-1]='\0';

		if (!strcmp(dirptr,".") || !strcmp(dirptr,"..") || !strcmp(dirptr,""))
			continue;

		//G_Printf ("%s\n",dirptr);

		//filename = va("/mods/%s/oatmeal.cfg",dirptr);
		//if(oatmeal_script_exists(filename)==1){
		//	G_Printf ("Loading %s\n",filename);
		//	trap_SendConsoleCommand( EXEC_APPEND, va("exec %s\n",filename) );
		//}

		G_Printf ("Mod found: \"%s\"\n",dirptr);

		numfiles = trap_FS_GetFileList( va("mods/%s",dirptr), "cfg", filelist, 2048 );
		fileptr  = filelist;
		for (j=0; j<numfiles;j++,fileptr+=filelen+1)
		{
			filelen = strlen(fileptr);

			filename = va("/mods/%s/%s",dirptr,fileptr);
			if(oatmeal_script_exists(filename)==1){
				G_Printf ("     Loading %s\n",filename);
				trap_SendConsoleCommand( EXEC_APPEND, va("exec %s\n",filename) );
			}
		}

		G_Printf ("\n");
	}
}

// oatmeal end

/*
============
G_InitGame

============
*/
void G_InitGame( int levelTime, int randomSeed, int restart ) {
	int					i;

	// oatmeal begin

	G_Printf ("\n---------- OATMEAL Begin ----------\n");

	G_Printf ("  ____       _______ __  __ ______          _\n");      
	G_Printf (" / __ \\   /\\|__   __|  \\/  |  ____|   /\\   | |\n");     
	G_Printf ("| |  | | /  \\  | |  | \\  / | |__     /  \\  | |\n");     
	G_Printf ("| |  | |/ /\\ \\ | |  | |\\/| |  __|   / /\\ \\ | |\n");     
	G_Printf ("| |__| / ____ \\| |  | |  | | |____ / ____ \\| |____\n"); 
	G_Printf (" \\____/_/    \\_\\_|  |_|  |_|______/_/    \\_\\______|\n\n");

	G_Printf ("OpenArena Toolkit & Mod Environment for ALl\n");

	G_Printf ("http://oatmeal.linuxthugs.com\n\n");

	G_Printf ("Copyright (C) 2009 Dan Hetrick\n\n");

	G_Printf ("This program is free software: you can redistribute it and/or modify\n");
	G_Printf ("it under the terms of the GNU General Public License as published by\n");
	G_Printf ("the Free Software Foundation, either version 2 of the License, or\n");
	G_Printf ("(at your option) any later version.\n\n");

	G_Printf ("This program is distributed in the hope that it will be useful,\n");
	G_Printf ("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
	G_Printf ("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
	G_Printf ("GNU General Public License for more details.\n\n");


	oatmeal_scan_and_load_mods();

	G_Printf ("----------- OATMEAL End -----------\n\n");

	// oatmeal end

	G_Printf ("------- Game Initialization -------\n");
	G_Printf ("gamename: %s\n", GAMEVERSION);
	G_Printf ("gamedate: %s\n", __DATE__);

	srand( randomSeed );

	G_RegisterCvars();
        
        //disable unwanted cvars
        if( g_gametype.integer == GT_SINGLE_PLAYER )
        {
            g_instantgib.integer = 0;
            g_rockets.integer = 0;
            g_vampire.value = 0.0f;
        }

	G_ProcessIPBans();

	G_InitMemory();

	// set some level globals
	memset( &level, 0, sizeof( level ) );
	level.time = levelTime;
	level.startTime = levelTime;

	level.snd_fry = G_SoundIndex("sound/player/fry.wav");	// FIXME standing in lava / slime

	if ( g_gametype.integer != GT_SINGLE_PLAYER && g_logfile.string[0] ) {
		if ( g_logfileSync.integer ) {
			trap_FS_FOpenFile( g_logfile.string, &level.logFile, FS_APPEND_SYNC );
		} else {
			trap_FS_FOpenFile( g_logfile.string, &level.logFile, FS_APPEND );
		}
		if ( !level.logFile ) {
			G_Printf( "WARNING: Couldn't open logfile: %s\n", g_logfile.string );
		} else {
			char	serverinfo[MAX_INFO_STRING];

			trap_GetServerinfo( serverinfo, sizeof( serverinfo ) );

			G_LogPrintf("------------------------------------------------------------\n" );
			G_LogPrintf("InitGame: %s\n", serverinfo );
		}
	} else {
		G_Printf( "Not logging to disk.\n" );
	}

	G_InitWorldSession();

	// initialize all entities for this game
	memset( g_entities, 0, MAX_GENTITIES * sizeof(g_entities[0]) );
	level.gentities = g_entities;

	// initialize all clients for this game
	level.maxclients = g_maxclients.integer;
	memset( g_clients, 0, MAX_CLIENTS * sizeof(g_clients[0]) );
	level.clients = g_clients;

	// set client fields on player ents
	for ( i=0 ; i<level.maxclients ; i++ ) {
		g_entities[i].client = level.clients + i;
	}

	// always leave room for the max number of clients,
	// even if they aren't all used, so numbers inside that
	// range are NEVER anything but clients
	level.num_entities = MAX_CLIENTS;

	// let the server system know where the entites are
	trap_LocateGameData( level.gentities, level.num_entities, sizeof( gentity_t ), 
		&level.clients[0].ps, sizeof( level.clients[0] ) );

	// reserve some spots for dead player bodies
	InitBodyQue();

	ClearRegisteredItems();

	// parse the key/value pairs and spawn gentities
	G_SpawnEntitiesFromString();

	// general initialization
	G_FindTeams();

	// make sure we have flags for CTF, etc
	if( g_gametype.integer >= GT_TEAM && (g_ffa_gt!=1)) {
		G_CheckTeamItems();
	}

	SaveRegisteredItems();

	G_Printf ("-----------------------------------\n");

	if( g_gametype.integer == GT_SINGLE_PLAYER || trap_Cvar_VariableIntegerValue( "com_buildScript" ) ) {
		G_ModelIndex( SP_PODIUM_MODEL );
		G_SoundIndex( "sound/player/gurp1.wav" );
		G_SoundIndex( "sound/player/gurp2.wav" );
	}

	if ( trap_Cvar_VariableIntegerValue( "bot_enable" ) ) {
		BotAISetup( restart );
		BotAILoadMap( restart );
		G_InitBots( restart );
	}

	G_RemapTeamShaders();

	//elimination:
	level.roundNumber = 1;
	level.roundNumberStarted = 0;
	level.roundStartTime = level.time+g_elimination_warmup.integer*1000;
	level.roundRespawned = qfalse;
	level.eliminationSides = rand()%2; //0 or 1

	if(g_gametype.integer == GT_DOUBLE_D)
		Team_SpawnDoubleDominationPoints();

	if(g_gametype.integer == GT_DOMINATION ){
		level.dom_scoreGiven = 0;
		for(i=0;i<MAX_DOMINATION_POINTS;i++)
			level.pointStatusDom[i] = TEAM_NONE;
		level.domination_points_count = 0; //make sure its not too big
	}
}



/*
=================
G_ShutdownGame
=================
*/
void G_ShutdownGame( int restart ) {
	G_Printf ("==== ShutdownGame ====\n");

	if ( level.logFile ) {
		G_LogPrintf("ShutdownGame:\n" );
		G_LogPrintf("------------------------------------------------------------\n" );
		trap_FS_FCloseFile( level.logFile );
	}

	// write all the client session data so we can get it back
	G_WriteSessionData();

	if ( trap_Cvar_VariableIntegerValue( "bot_enable" ) ) {
		BotAIShutdown( restart );
	}
}



//===================================================================

void QDECL Com_Error ( int level, const char *error, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, error);
	Q_vsnprintf (text, sizeof(text), error, argptr);
	va_end (argptr);

	G_Error( "%s", text);
}

void QDECL Com_Printf( const char *msg, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, msg);
	Q_vsnprintf (text, sizeof(text), msg, argptr);
	va_end (argptr);

	G_Printf ("%s", text);
}

/*
========================================================================

PLAYER COUNTING / SCORE SORTING

========================================================================
*/

/*
=============
AddTournamentPlayer

If there are less than two tournament players, put a
spectator in the game and restart
=============
*/
void AddTournamentPlayer( void ) {
	int			i;
	gclient_t	*client;
	gclient_t	*nextInLine;

	if ( level.numPlayingClients >= 2 ) {
		return;
	}

	// never change during intermission
	if ( level.intermissiontime ) {
		return;
	}

	nextInLine = NULL;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		client = &level.clients[i];
		if ( client->pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( client->sess.sessionTeam != TEAM_SPECTATOR ) {
			continue;
		}
		// never select the dedicated follow or scoreboard clients
		if ( client->sess.spectatorState == SPECTATOR_SCOREBOARD || 
			client->sess.spectatorClient < 0  ) {
			continue;
		}

		if ( !nextInLine || client->sess.spectatorTime < nextInLine->sess.spectatorTime ) {
			nextInLine = client;
		}
	}

	if ( !nextInLine ) {
		return;
	}

	level.warmupTime = -1;

	// set them to free-for-all team
	SetTeam( &g_entities[ nextInLine - level.clients ], "f" );
}

/*
=======================
RemoveTournamentLoser

Make the loser a spectator at the back of the line
=======================
*/
void RemoveTournamentLoser( void ) {
	int			clientNum;

	if ( level.numPlayingClients != 2 ) {
		return;
	}

	clientNum = level.sortedClients[1];

	if ( level.clients[ clientNum ].pers.connected != CON_CONNECTED ) {
		return;
	}

	// make them a spectator
	SetTeam( &g_entities[ clientNum ], "s" );
}

/*
=======================
RemoveTournamentWinner
=======================
*/
void RemoveTournamentWinner( void ) {
	int			clientNum;

	if ( level.numPlayingClients != 2 ) {
		return;
	}

	clientNum = level.sortedClients[0];

	if ( level.clients[ clientNum ].pers.connected != CON_CONNECTED ) {
		return;
	}

	// make them a spectator
	SetTeam( &g_entities[ clientNum ], "s" );
}

/*
=======================
AdjustTournamentScores
=======================
*/
void AdjustTournamentScores( void ) {
	int			clientNum;

	clientNum = level.sortedClients[0];
	if ( level.clients[ clientNum ].pers.connected == CON_CONNECTED ) {
		level.clients[ clientNum ].sess.wins++;
		ClientUserinfoChanged( clientNum );
	}

	clientNum = level.sortedClients[1];
	if ( level.clients[ clientNum ].pers.connected == CON_CONNECTED ) {
		level.clients[ clientNum ].sess.losses++;
		ClientUserinfoChanged( clientNum );
	}

}

/*
=============
SortRanks

=============
*/
int QDECL SortRanks( const void *a, const void *b ) {
	gclient_t	*ca, *cb;

	ca = &level.clients[*(int *)a];
	cb = &level.clients[*(int *)b];

	// sort special clients last
	if ( ca->sess.spectatorState == SPECTATOR_SCOREBOARD || ca->sess.spectatorClient < 0 ) {
		return 1;
	}
	if ( cb->sess.spectatorState == SPECTATOR_SCOREBOARD || cb->sess.spectatorClient < 0  ) {
		return -1;
	}

	// then connecting clients
	if ( ca->pers.connected == CON_CONNECTING ) {
		return 1;
	}
	if ( cb->pers.connected == CON_CONNECTING ) {
		return -1;
	}


	// then spectators
	if ( ca->sess.sessionTeam == TEAM_SPECTATOR && cb->sess.sessionTeam == TEAM_SPECTATOR ) {
		if ( ca->sess.spectatorTime < cb->sess.spectatorTime ) {
			return -1;
		}
		if ( ca->sess.spectatorTime > cb->sess.spectatorTime ) {
			return 1;
		}
		return 0;
	}
	if ( ca->sess.sessionTeam == TEAM_SPECTATOR ) {
		return 1;
	}
	if ( cb->sess.sessionTeam == TEAM_SPECTATOR ) {
		return -1;
	}

	// then sort by score
	if ( ca->ps.persistant[PERS_SCORE]
		> cb->ps.persistant[PERS_SCORE] ) {
		return -1;
	}
	if ( ca->ps.persistant[PERS_SCORE]
		< cb->ps.persistant[PERS_SCORE] ) {
		return 1;
	}
	return 0;
}

/*
============
CalculateRanks

Recalculates the score ranks of all players
This will be called on every client connect, begin, disconnect, death,
and team change.
============
*/
void CalculateRanks( void ) {
	int		i;
	int		rank;
	int		score;
	int		newScore;
        int             humanplayers;
	gclient_t	*cl;

	level.follow1 = -1;
	level.follow2 = -1;
	level.numConnectedClients = 0;
	level.numNonSpectatorClients = 0;
	level.numPlayingClients = 0;
	level.numVotingClients = 0;	 // don't count bots
        humanplayers = 0; // don't count bots
	for ( i = 0; i < TEAM_NUM_TEAMS; i++ ) {
		level.numteamVotingClients[i] = 0;
	}
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected != CON_DISCONNECTED ) {
			level.sortedClients[level.numConnectedClients] = i;
			level.numConnectedClients++;
                        
                        if ( !(g_entities[i].r.svFlags & SVF_BOT) ) {
                            humanplayers++;
                        }

			if ( level.clients[i].sess.sessionTeam != TEAM_SPECTATOR ) {
				level.numNonSpectatorClients++;
			
				// decide if this should be auto-followed
				if ( level.clients[i].pers.connected == CON_CONNECTED ) {
					level.numPlayingClients++;
					if ( !(g_entities[i].r.svFlags & SVF_BOT) ) {
						level.numVotingClients++;
						if ( level.clients[i].sess.sessionTeam == TEAM_RED )
							level.numteamVotingClients[0]++;
						else if ( level.clients[i].sess.sessionTeam == TEAM_BLUE )
							level.numteamVotingClients[1]++;
					}
					if ( level.follow1 == -1 ) {
						level.follow1 = i;
					} else if ( level.follow2 == -1 ) {
						level.follow2 = i;
					}
				}
			}
		}
	}

	qsort( level.sortedClients, level.numConnectedClients, 
		sizeof(level.sortedClients[0]), SortRanks );

	// set the rank value for all clients that are connected and not spectators
	if ( g_gametype.integer >= GT_TEAM && g_ffa_gt!=1) {
		// in team games, rank is just the order of the teams, 0=red, 1=blue, 2=tied
		for ( i = 0;  i < level.numConnectedClients; i++ ) {
			cl = &level.clients[ level.sortedClients[i] ];
			if ( level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE] ) {
				cl->ps.persistant[PERS_RANK] = 2;
			} else if ( level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE] ) {
				cl->ps.persistant[PERS_RANK] = 0;
			} else {
				cl->ps.persistant[PERS_RANK] = 1;
			}
		}
	} else {	
		rank = -1;
		score = 0;
		for ( i = 0;  i < level.numPlayingClients; i++ ) {
			cl = &level.clients[ level.sortedClients[i] ];
			newScore = cl->ps.persistant[PERS_SCORE];
			if ( i == 0 || newScore != score ) {
				rank = i;
				// assume we aren't tied until the next client is checked
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank;
			} else {
				// we are tied with the previous client
				level.clients[ level.sortedClients[i-1] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
			}
			score = newScore;
			if ( g_gametype.integer == GT_SINGLE_PLAYER && level.numPlayingClients == 1 ) {
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
			}
		}
	}

	// set the CS_SCORES1/2 configstrings, which will be visible to everyone
	if ( g_gametype.integer >= GT_TEAM && g_ffa_gt!=1) {
		trap_SetConfigstring( CS_SCORES1, va("%i", level.teamScores[TEAM_RED] ) );
		trap_SetConfigstring( CS_SCORES2, va("%i", level.teamScores[TEAM_BLUE] ) );
	} else {
		if ( level.numConnectedClients == 0 ) {
			trap_SetConfigstring( CS_SCORES1, va("%i", SCORE_NOT_PRESENT) );
			trap_SetConfigstring( CS_SCORES2, va("%i", SCORE_NOT_PRESENT) );
		} else if ( level.numConnectedClients == 1 ) {
			trap_SetConfigstring( CS_SCORES1, va("%i", level.clients[ level.sortedClients[0] ].ps.persistant[PERS_SCORE] ) );
			trap_SetConfigstring( CS_SCORES2, va("%i", SCORE_NOT_PRESENT) );
		} else {
			trap_SetConfigstring( CS_SCORES1, va("%i", level.clients[ level.sortedClients[0] ].ps.persistant[PERS_SCORE] ) );
			trap_SetConfigstring( CS_SCORES2, va("%i", level.clients[ level.sortedClients[1] ].ps.persistant[PERS_SCORE] ) );
		}
	}

	// see if it is time to end the level
	CheckExitRules();

	// if we are at the intermission, send the new info to everyone
	if ( level.intermissiontime ) {
		SendScoreboardMessageToAllClients();
	}
        
        if(g_humanplayers.integer != humanplayers) //Presume all spectators are humans!
            trap_Cvar_Set( "g_humanplayers", va("%i", humanplayers) );
}


/*
========================================================================

MAP CHANGING

========================================================================
*/

/*
========================
SendScoreboardMessageToAllClients

Do this at BeginIntermission time and whenever ranks are recalculated
due to enters/exits/forced team changes
========================
*/
void SendScoreboardMessageToAllClients( void ) {
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED ) {
			DeathmatchScoreboardMessage( g_entities + i );
			EliminationMessage( g_entities + i );
		}
	}
}

/*
========================
SendElimiantionMessageToAllClients

Used to send information important to Elimination
========================
*/
void SendEliminationMessageToAllClients( void ) {
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED ) {
			EliminationMessage( g_entities + i );
		}
	}
}

/*
========================
SendDDtimetakenMessageToAllClients

Do this if a team just started dominating.
========================
*/
void SendDDtimetakenMessageToAllClients( void ) {
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED ) {
			DoubleDominationScoreTimeMessage( g_entities + i );
		}
	}
}

/*
========================
SendAttackingTeamMessageToAllClients

Used for CTF Elimination oneway
========================
*/
void SendAttackingTeamMessageToAllClients( void ) {
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED ) {
			AttackingTeamMessage( g_entities + i );
		}
	}
}

/*
========================
SendDominationPointsStatusMessageToAllClients

Used for Standard domination
========================
*/
void SendDominationPointsStatusMessageToAllClients( void ) {
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED ) {
			DominationPointStatusMessage( g_entities + i );
		}
	}
}

/*
========================
MoveClientToIntermission

When the intermission starts, this will be called for all players.
If a new client connects, this will be called after the spawn function.
========================
*/
void MoveClientToIntermission( gentity_t *ent ) {
	// take out of follow mode if needed
	if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW ) {
		StopFollowing( ent );
	}


	// move to the spot
	VectorCopy( level.intermission_origin, ent->s.origin );
	VectorCopy( level.intermission_origin, ent->client->ps.origin );
	VectorCopy (level.intermission_angle, ent->client->ps.viewangles);
	ent->client->ps.pm_type = PM_INTERMISSION;

	// clean up powerup info
	memset( ent->client->ps.powerups, 0, sizeof(ent->client->ps.powerups) );

	ent->client->ps.eFlags = 0;
	ent->s.eFlags = 0;
	ent->s.eType = ET_GENERAL;
	ent->s.modelindex = 0;
	ent->s.loopSound = 0;
	ent->s.event = 0;
	ent->r.contents = 0;
}

/*
==================
FindIntermissionPoint

This is also used for spectator spawns
==================
*/
void FindIntermissionPoint( void ) {
	gentity_t	*ent, *target;
	vec3_t		dir;

	// find the intermission spot
	ent = G_Find (NULL, FOFS(classname), "info_player_intermission");
	if ( !ent ) {	// the map creator forgot to put in an intermission point...
		SelectSpawnPoint ( vec3_origin, level.intermission_origin, level.intermission_angle );
	} else {
		VectorCopy (ent->s.origin, level.intermission_origin);
		VectorCopy (ent->s.angles, level.intermission_angle);
		// if it has a target, look towards it
		if ( ent->target ) {
			target = G_PickTarget( ent->target );
			if ( target ) {
				VectorSubtract( target->s.origin, level.intermission_origin, dir );
				vectoangles( dir, level.intermission_angle );
			}
		}
	}

}

/*
==================
BeginIntermission
==================
*/
void BeginIntermission( void ) {
	int			i;
	gentity_t	*client;

	if ( level.intermissiontime ) {
		return;		// already active
	}

	// if in tournement mode, change the wins / losses
	if ( g_gametype.integer == GT_TOURNAMENT ) {
		AdjustTournamentScores();
	}

	level.intermissiontime = level.time;
	FindIntermissionPoint();

#ifdef MISSIONPACK
	if (g_singlePlayer.integer) {
		trap_Cvar_Set("ui_singlePlayerActive", "0");
		UpdateTournamentInfo();
	}
#else
	// if single player game
	if ( g_gametype.integer == GT_SINGLE_PLAYER ) {
		UpdateTournamentInfo();
		SpawnModelsOnVictoryPads();
	}
#endif

	// move all clients to the intermission point
	for (i=0 ; i< level.maxclients ; i++) {
		client = g_entities + i;
		if (!client->inuse)
			continue;
		// respawn if dead
		if (client->health <= 0) {
			respawn(client);
		}
		MoveClientToIntermission( client );
	}

	// send the current scoring to all clients
	SendScoreboardMessageToAllClients();

}


/*
=============
ExitLevel

When the intermission has been exited, the server is either killed
or moved to a new level based on the "nextmap" cvar 

=============
*/
void ExitLevel (void) {
	int		i;
	gclient_t *cl;
	char nextmap[MAX_STRING_CHARS];
	char d1[MAX_STRING_CHARS];

	//bot interbreeding
	BotInterbreedEndMatch();

	// if we are running a tournement map, kick the loser to spectator status,
	// which will automatically grab the next spectator and restart
	if ( g_gametype.integer == GT_TOURNAMENT  ) {
		if ( !level.restarted ) {
			RemoveTournamentLoser();
			trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
			level.restarted = qtrue;
			level.changemap = NULL;
			level.intermissiontime = 0;
		}
		return;	
	}

	trap_Cvar_VariableStringBuffer( "nextmap", nextmap, sizeof(nextmap) );
	trap_Cvar_VariableStringBuffer( "d1", d1, sizeof(d1) );

	if( !Q_stricmp( nextmap, "map_restart 0" ) && Q_stricmp( d1, "" ) ) {
		trap_Cvar_Set( "nextmap", "vstr d2" );
		trap_SendConsoleCommand( EXEC_APPEND, "vstr d1\n" );
	} else {
		trap_SendConsoleCommand( EXEC_APPEND, "vstr nextmap\n" );
	}

	level.changemap = NULL;
	level.intermissiontime = 0;

	// reset all the scores so we don't enter the intermission again
	level.teamScores[TEAM_RED] = 0;
	level.teamScores[TEAM_BLUE] = 0;
	for ( i=0 ; i< g_maxclients.integer ; i++ ) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		cl->ps.persistant[PERS_SCORE] = 0;
	}

	// we need to do this here before chaning to CON_CONNECTING
	G_WriteSessionData();

	// change all client states to connecting, so the early players into the
	// next level will know the others aren't done reconnecting
	for (i=0 ; i< g_maxclients.integer ; i++) {
		if ( level.clients[i].pers.connected == CON_CONNECTED ) {
			level.clients[i].pers.connected = CON_CONNECTING;
		}
	}

}

/*
=================
G_LogPrintf

Print to the logfile with a time stamp if it is open
=================
*/
void QDECL G_LogPrintf( const char *fmt, ... ) {
	va_list		argptr;
	char		string[1024];
	int			min, tens, sec;

	sec = level.time / 1000;

	min = sec / 60;
	sec -= min * 60;
	tens = sec / 10;
	sec -= tens * 10;

	Com_sprintf( string, sizeof(string), "%3i:%i%i ", min, tens, sec );

	va_start( argptr, fmt );
	Q_vsnprintf(string + 7, sizeof(string) - 7, fmt, argptr);
	va_end( argptr );

	if ( g_dedicated.integer ) {
		G_Printf( "%s", string + 7 );
	}

	if ( !level.logFile ) {
		return;
	}

	trap_FS_Write( string, strlen( string ), level.logFile );
}

/*
================
LogExit

Append information about this game to the log file
================
*/
void LogExit( const char *string ) {
	int				i, numSorted;
	gclient_t		*cl;
#ifdef MISSIONPACK // bk001205
	qboolean won = qtrue;
#endif
	G_LogPrintf( "Exit: %s\n", string );

	level.intermissionQueued = level.time;

	// this will keep the clients from playing any voice sounds
	// that will get cut off when the queued intermission starts
	trap_SetConfigstring( CS_INTERMISSION, "1" );

	// don't send more than 32 scores (FIXME?)
	numSorted = level.numConnectedClients;
	if ( numSorted > 32 ) {
		numSorted = 32;
	}

	if ( g_gametype.integer >= GT_TEAM && g_ffa_gt!=1) {
		G_LogPrintf( "red:%i  blue:%i\n",
			level.teamScores[TEAM_RED], level.teamScores[TEAM_BLUE] );
	}

	for (i=0 ; i < numSorted ; i++) {
		int		ping;

		cl = &level.clients[level.sortedClients[i]];

		if ( cl->sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		}
		if ( cl->pers.connected == CON_CONNECTING ) {
			continue;
		}

		ping = cl->ps.ping < 999 ? cl->ps.ping : 999;

		G_LogPrintf( "score: %i  ping: %i  client: %i %s\n", cl->ps.persistant[PERS_SCORE], ping, level.sortedClients[i],	cl->pers.netname );
#ifdef MISSIONPACK
		if (g_singlePlayer.integer && g_gametype.integer == GT_TOURNAMENT) {
			if (g_entities[cl - level.clients].r.svFlags & SVF_BOT && cl->ps.persistant[PERS_RANK] == 0) {
				won = qfalse;
			}
		}
#endif

	}

#ifdef MISSIONPACK
	if (g_singlePlayer.integer) {
		if (g_gametype.integer >= GT_CTF && g_ffa_gt==0) {
			won = level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE];
		}
		trap_SendConsoleCommand( EXEC_APPEND, (won) ? "spWin\n" : "spLose\n" );
	}
#endif


}


/*
=================
CheckIntermissionExit

The level will stay at the intermission for a minimum of 5 seconds
If all players wish to continue, the level will then exit.
If one or more players have not acknowledged the continue, the game will
wait 10 seconds before going on.
=================
*/
void CheckIntermissionExit( void ) {
	int			ready, notReady, playerCount;
	int			i;
	gclient_t	*cl;
	int			readyMask;

	if ( g_gametype.integer == GT_SINGLE_PLAYER ) {
		return;
	}

	// see which players are ready
	ready = 0;
	notReady = 0;
	readyMask = 0;
        playerCount = 0;
	for (i=0 ; i< g_maxclients.integer ; i++) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( g_entities[cl->ps.clientNum].r.svFlags & SVF_BOT ) {
			continue;
		}

                playerCount++;
		if ( cl->readyToExit ) {
			ready++;
			if ( i < 16 ) {
				readyMask |= 1 << i;
			}
		} else {
			notReady++;
		}
	}

	// copy the readyMask to each player's stats so
	// it can be displayed on the scoreboard
	for (i=0 ; i< g_maxclients.integer ; i++) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		cl->ps.stats[STAT_CLIENTS_READY] = readyMask;
	}

	// never exit in less than five seconds
	if ( level.time < level.intermissiontime + 5000 ) {
		return;
	}

	// only test ready status when there are real players present
	if ( playerCount > 0 ) {
		// if nobody wants to go, clear timer
		if ( !ready ) {
			level.readyToExit = qfalse;
			return;
		}

		// if everyone wants to go, go now
		if ( !notReady ) {
			ExitLevel();
			return;
		}
	}

	// the first person to ready starts the ten second timeout
	if ( !level.readyToExit ) {
		level.readyToExit = qtrue;
		level.exitTime = level.time;
	}

	// if we have waited ten seconds since at least one player
	// wanted to exit, go ahead
	if ( level.time < level.exitTime + 10000 ) {
		return;
	}

	ExitLevel();
}

/*
=============
ScoreIsTied
=============
*/
qboolean ScoreIsTied( void ) {
	int		a, b;

	if ( level.numPlayingClients < 2 ) {
		return qfalse;
	}
        
        //Sago: In Elimination and Oneway Flag Capture teams must win by two points.
        if ( g_gametype.integer == GT_ELIMINATION || 
                (g_gametype.integer == GT_CTF_ELIMINATION && g_elimination_ctf_oneway.integer)) {
            return (level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE] || 
                    level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE]+1 ||
                    level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE]-1);
        }
	
	if ( g_gametype.integer >= GT_TEAM && g_ffa_gt!=1) {
		return level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE];
	}

	a = level.clients[level.sortedClients[0]].ps.persistant[PERS_SCORE];
	b = level.clients[level.sortedClients[1]].ps.persistant[PERS_SCORE];

	return a == b;
}

/*
=================
CheckExitRules

There will be a delay between the time the exit is qualified for
and the time everyone is moved to the intermission spot, so you
can see the last frag.
=================
*/
void CheckExitRules( void ) {
 	int			i;
	gclient_t	*cl;
	// if at the intermission, wait for all non-bots to
	// signal ready, then go to next level
	if ( level.intermissiontime ) {
		CheckIntermissionExit ();
		return;
	}

	if ( level.intermissionQueued ) {
#ifdef MISSIONPACK
		int time = (g_singlePlayer.integer) ? SP_INTERMISSION_DELAY_TIME : INTERMISSION_DELAY_TIME;
		if ( level.time - level.intermissionQueued >= time ) {
			level.intermissionQueued = 0;
			BeginIntermission();
		}
#else
		if ( level.time - level.intermissionQueued >= INTERMISSION_DELAY_TIME ) {
			level.intermissionQueued = 0;
			BeginIntermission();
		}
#endif
		return;
	}

	// check for sudden death
	if ( ScoreIsTied() ) {
		// always wait for sudden death
		return;
	}

	if ( g_timelimit.integer && !level.warmupTime ) {
		if ( level.time - level.startTime >= g_timelimit.integer*60000 ) {
			trap_SendServerCommand( -1, "print \"Timelimit hit.\n\"");
			LogExit( "Timelimit hit." );
			return;
		}
	}

	if ( level.numPlayingClients < 2 ) {
		return;
	}

	if ( (g_gametype.integer < GT_CTF || g_ffa_gt>0 ) && g_fraglimit.integer ) {
		if ( level.teamScores[TEAM_RED] >= g_fraglimit.integer ) {
			trap_SendServerCommand( -1, "print \"Red hit the fraglimit.\n\"" );
			LogExit( "Fraglimit hit." );
			return;
		}

		if ( level.teamScores[TEAM_BLUE] >= g_fraglimit.integer ) {
			trap_SendServerCommand( -1, "print \"Blue hit the fraglimit.\n\"" );
			LogExit( "Fraglimit hit." );
			return;
		}

		for ( i=0 ; i< g_maxclients.integer ; i++ ) {
			cl = level.clients + i;
			if ( cl->pers.connected != CON_CONNECTED ) {
				continue;
			}
			if ( cl->sess.sessionTeam != TEAM_FREE ) {
				continue;
			}

			if ( cl->ps.persistant[PERS_SCORE] >= g_fraglimit.integer ) {
				LogExit( "Fraglimit hit." );
				trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " hit the fraglimit.\n\"",
					cl->pers.netname ) );
				return;
			}
		}
	}

	if ( (g_gametype.integer >= GT_CTF && g_ffa_gt<1) && g_capturelimit.integer ) {

		if ( level.teamScores[TEAM_RED] >= g_capturelimit.integer ) {
			trap_SendServerCommand( -1, "print \"Red hit the capturelimit.\n\"" );
			LogExit( "Capturelimit hit." );
			return;
		}

		if ( level.teamScores[TEAM_BLUE] >= g_capturelimit.integer ) {
			trap_SendServerCommand( -1, "print \"Blue hit the capturelimit.\n\"" );
			LogExit( "Capturelimit hit." );
			return;
		}
	}
}

//LMS - Last man Stading functions:
void StartLMSRound(void) {
	int		countsLiving;
	countsLiving = TeamLivingCount( -1, TEAM_FREE );
	if(countsLiving<2) {
		trap_SendServerCommand( -1, "print \"Not enough players to start the round\n\"");
		level.roundNumberStarted = level.roundNumber-1;
		level.roundStartTime = level.time+1000*g_elimination_warmup.integer;
		return;
	}

	//If we are enough to start a round:
	level.roundNumberStarted = level.roundNumber; //Set numbers

        
        
	SendEliminationMessageToAllClients();
	EnableWeapons();
}

//the elimination start function
void StartEliminationRound(void) {

	int		countsLiving[TEAM_NUM_TEAMS];
	countsLiving[TEAM_BLUE] = TeamLivingCount( -1, TEAM_BLUE );
	countsLiving[TEAM_RED] = TeamLivingCount( -1, TEAM_RED );
	if((countsLiving[TEAM_BLUE]==0) || (countsLiving[TEAM_RED]==0))
	{
		trap_SendServerCommand( -1, "print \"Not enough players to start the round\n\"");
		level.roundNumberStarted = level.roundNumber-1;
		level.roundRespawned = qfalse;
		//Remember that one of the teams is empty!
		level.roundRedPlayers = countsLiving[TEAM_RED];
		level.roundBluePlayers = countsLiving[TEAM_BLUE];
		level.roundStartTime = level.time+1000*g_elimination_warmup.integer;
		return;
	}
	
	//If we are enough to start a round:
	level.roundNumberStarted = level.roundNumber; //Set numbers
	level.roundRedPlayers = countsLiving[TEAM_RED];
	level.roundBluePlayers = countsLiving[TEAM_BLUE];
	if(g_gametype.integer == GT_CTF_ELIMINATION) {
		Team_ReturnFlag( TEAM_RED );
		Team_ReturnFlag( TEAM_BLUE );
	}
        if(g_gametype.integer == GT_ELIMINATION) {
            G_LogPrintf( "ELIMINATION: %i %i %i: Round %i has started!\n", level.roundNumber, -1, 0, level.roundNumber );
        } else if(g_gametype.integer == GT_CTF_ELIMINATION) {
            G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: Round %i has started!\n", level.roundNumber, -1, -1, 4, level.roundNumber );
        }
	SendEliminationMessageToAllClients();
	if(g_elimination_ctf_oneway.integer)
		SendAttackingTeamMessageToAllClients(); //Ensure that evaryone know who should attack.
	EnableWeapons();
}

//things to do at end of round:
void EndEliminationRound(void)
{
	DisableWeapons();
	level.roundNumber++;
	level.roundStartTime = level.time+1000*g_elimination_warmup.integer;
	SendEliminationMessageToAllClients();
	level.roundRespawned = qfalse;
	if(g_elimination_ctf_oneway.integer)
		SendAttackingTeamMessageToAllClients();
}

//Things to do if we don't want to move the roundNumber
void RestartEliminationRound(void) {
	DisableWeapons();
	level.roundNumberStarted = level.roundNumber-1;
	level.roundStartTime = level.time+1000*g_elimination_warmup.integer;
	SendEliminationMessageToAllClients();
	level.roundRespawned = qfalse;
	if(g_elimination_ctf_oneway.integer)
		SendAttackingTeamMessageToAllClients();
}

//Things to do during match warmup
void WarmupEliminationRound(void) {
	EnableWeapons();
	level.roundNumberStarted = level.roundNumber-1;
	level.roundStartTime = level.time+1000*g_elimination_warmup.integer;
	SendEliminationMessageToAllClients();
	level.roundRespawned = qfalse;
	if(g_elimination_ctf_oneway.integer)
		SendAttackingTeamMessageToAllClients();
}

/*
========================================================================

FUNCTIONS CALLED EVERY FRAME

========================================================================
*/

/*
CheckDoubleDomination
*/

void CheckDoubleDomination( void ) {
	if ( level.numPlayingClients < 1 ) {
		return;
	}

	if ( level.warmupTime != 0)
		return;

	if(g_gametype.integer != GT_DOUBLE_D)
		return;

	//Don't score if we are in intermission. Both points might have been taken when we went into intermission
	if(level.intermissiontime)
		return;

	if(level.pointStatusA == TEAM_RED && level.pointStatusB == TEAM_RED && level.timeTaken + 10*1000 <= level.time) {
		//Red scores
		trap_SendServerCommand( -1, "print \"Red team scores!\n\"");
		AddTeamScore(level.intermission_origin,TEAM_RED,1);
                G_LogPrintf( "DD: %i %i %i: %s scores!\n", -1, TEAM_RED, 2, TeamName(TEAM_RED) );
		Team_ForceGesture(TEAM_RED);
		Team_DD_bonusAtPoints(TEAM_RED);
		Team_RemoveDoubleDominationPoints();
		//We start again in 10 seconds:
		level.roundStartTime = level.time + 10*1000;
		SendScoreboardMessageToAllClients();
		CalculateRanks();
	}

	if(level.pointStatusA == TEAM_BLUE && level.pointStatusB == TEAM_BLUE && level.timeTaken + 10*1000 <= level.time) {
		//Blue scores
		trap_SendServerCommand( -1, "print \"Blue team scores!\n\"");
		AddTeamScore(level.intermission_origin,TEAM_BLUE,1);
                G_LogPrintf( "DD: %i %i %i: %s scores!\n", -1, TEAM_BLUE, 2, TeamName(TEAM_BLUE) );
		Team_ForceGesture(TEAM_BLUE);
		Team_DD_bonusAtPoints(TEAM_BLUE);
		Team_RemoveDoubleDominationPoints();
		//We start again in 10 seconds:
		level.roundStartTime = level.time + 10*1000;
		SendScoreboardMessageToAllClients();
		CalculateRanks();
	}

	if((level.pointStatusA == TEAM_NONE || level.pointStatusB == TEAM_NONE) && level.time>level.roundStartTime) {
		trap_SendServerCommand( -1, "print \"A new round has started\n\"");
		Team_SpawnDoubleDominationPoints();
		SendScoreboardMessageToAllClients();
	}
}

/*
CheckLMS
*/

void CheckLMS(void) {
	int mode;
	mode = g_lms_mode.integer;
	if ( level.numPlayingClients < 1 ) {
		return;
	}

	

	//We don't want to do anything in intermission
	if(level.intermissiontime) {
		if(level.roundRespawned) {
			RestartEliminationRound();
		}
		level.roundStartTime = level.time; //so that a player might join at any time to fix the bots+no humans+autojoin bug
		return;
	}

	if(g_gametype.integer == GT_LMS)
	{
		int		countsLiving[TEAM_NUM_TEAMS];
		//trap_SendServerCommand( -1, "print \"This is LMS!\n\"");
		countsLiving[TEAM_FREE] = TeamLivingCount( -1, TEAM_FREE );
		if(countsLiving[TEAM_FREE]==1 && level.roundNumber==level.roundNumberStarted)
		{
			if(mode <=1 )
			LMSpoint();
			trap_SendServerCommand( -1, "print \"We have a winner!\n\"");
			EndEliminationRound();
			Team_ForceGesture(TEAM_FREE);
		}

		if(countsLiving[TEAM_FREE]==0 && level.roundNumber==level.roundNumberStarted)
		{
			trap_SendServerCommand( -1, "print \"All death... how sad\n\"");
			EndEliminationRound();
		}

		if((level.roundNumber==level.roundNumberStarted)&&(g_lms_mode.integer == 1 || g_lms_mode.integer==3)&&(level.time>=level.roundStartTime+1000*g_elimination_roundtime.integer))
		{
			trap_SendServerCommand( -1, "print \"Time up - Overtime disabled\n\"");
			if(mode <=1 )
			LMSpoint();
			EndEliminationRound();
		}

		//This might be better placed another place:
		if(g_elimination_activewarmup.integer<2)
			g_elimination_activewarmup.integer=2; //We need at least 2 seconds to spawn all players
		if(g_elimination_activewarmup.integer >= g_elimination_warmup.integer) //This must not be true
			g_elimination_warmup.integer = g_elimination_activewarmup.integer+1; //Increase warmup

		//Force respawn
		if(level.roundNumber != level.roundNumberStarted && level.time>level.roundStartTime-1000*g_elimination_activewarmup.integer && !level.roundRespawned)
		{
			level.roundRespawned = qtrue;
			RespawnAll();
			DisableWeapons();
			SendEliminationMessageToAllClients();
		}

		if(level.time<=level.roundStartTime && level.time>level.roundStartTime-1000*g_elimination_activewarmup.integer)
		{
			RespawnDead();
			//DisableWeapons();
		}

		if(level.roundNumber == level.roundNumberStarted)
		{
			EnableWeapons();
		}

		if((level.roundNumber>level.roundNumberStarted)&&(level.time>=level.roundStartTime))
			StartLMSRound();
	
		if(level.time+1000*g_elimination_warmup.integer-500>level.roundStartTime && level.numPlayingClients < 2)
		{
			RespawnDead(); //Allow player to run around anyway
			WarmupEliminationRound(); //Start over
			return;
		}

		if(level.warmupTime != 0) {
			if(level.time+1000*g_elimination_warmup.integer-500>level.roundStartTime)
			{
				RespawnDead();
				WarmupEliminationRound();
			}
		}

	}
}

/*
=============
CheckElimination
=============
*/
void CheckElimination(void) {
	if ( level.numPlayingClients < 1 ) {
		if( (g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION) &&
			( level.time+1000*g_elimination_warmup.integer-500>level.roundStartTime ))
			RestartEliminationRound(); //For spectators
		return;
	}	

	//We don't want to do anything in itnermission
	if(level.intermissiontime) {
		if(level.roundRespawned)
			RestartEliminationRound();
		level.roundStartTime = level.time+1000*g_elimination_warmup.integer;
		return;
	}	

	if(g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION)
	{
		int		counts[TEAM_NUM_TEAMS];
		int		countsLiving[TEAM_NUM_TEAMS];
		int		countsHealth[TEAM_NUM_TEAMS];
		counts[TEAM_BLUE] = TeamCount( -1, TEAM_BLUE );
		counts[TEAM_RED] = TeamCount( -1, TEAM_RED );

		countsLiving[TEAM_BLUE] = TeamLivingCount( -1, TEAM_BLUE );
		countsLiving[TEAM_RED] = TeamLivingCount( -1, TEAM_RED );

		countsHealth[TEAM_BLUE] = TeamHealthCount( -1, TEAM_BLUE );
		countsHealth[TEAM_RED] = TeamHealthCount( -1, TEAM_RED );

		if(level.roundBluePlayers != 0 && level.roundRedPlayers != 0) { //Cannot score if one of the team never got any living players
			if((countsLiving[TEAM_BLUE]==0)&&(level.roundNumber==level.roundNumberStarted))
			{
				//Blue team has been eliminated!
				trap_SendServerCommand( -1, "print \"Blue Team eliminated!\n\"");
				AddTeamScore(level.intermission_origin,TEAM_RED,1);
                                if(g_gametype.integer == GT_ELIMINATION) {
                                    G_LogPrintf( "ELIMINATION: %i %i %i: %s wins round %i by eleminating the enemy team!\n", level.roundNumber, TEAM_RED, 1, TeamName(TEAM_RED), level.roundNumber );
                                } else {
                                    G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: %s wins round %i by eleminating the enemy team!\n", level.roundNumber, -1, TEAM_RED, 6, TeamName(TEAM_RED), level.roundNumber );
                                }
				EndEliminationRound();
				Team_ForceGesture(TEAM_RED);
			}
			else if((countsLiving[TEAM_RED]==0)&&(level.roundNumber==level.roundNumberStarted))
			{
				//Red team eliminated!
				trap_SendServerCommand( -1, "print \"Red Team eliminated!\n\"");
				AddTeamScore(level.intermission_origin,TEAM_BLUE,1);
                                if(g_gametype.integer == GT_ELIMINATION) {
                                    G_LogPrintf( "ELIMINATION: %i %i %i: %s wins round %i by eleminating the enemy team!\n", level.roundNumber, TEAM_BLUE, 1, TeamName(TEAM_BLUE), level.roundNumber );
                                } else {
                                    G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: %s wins round %i by eleminating the enemy team!\n", level.roundNumber, -1, TEAM_BLUE, 6, TeamName(TEAM_BLUE), level.roundNumber );
                                }
				EndEliminationRound();
				Team_ForceGesture(TEAM_BLUE);
			}
		}

		//Time up
		if((level.roundNumber==level.roundNumberStarted)&&(level.time>=level.roundStartTime+1000*g_elimination_roundtime.integer))
		{
			trap_SendServerCommand( -1, "print \"No teams eliminated!\n\"");

			if(level.roundBluePlayers != 0 && level.roundRedPlayers != 0) {//We don't want to divide by zero. (should not be possible)
				if(g_gametype.integer == GT_CTF_ELIMINATION && g_elimination_ctf_oneway.integer) {
					//One way CTF, make defensice team the winner.
					if ( (level.eliminationSides+level.roundNumber)%2 == 0 ) { //Red was attacking
						trap_SendServerCommand( -1, "print \"Blue team defended the base\n\"");
						AddTeamScore(level.intermission_origin,TEAM_BLUE,1);
					}
					else {
						trap_SendServerCommand( -1, "print \"Red team defended the base\n\"");
						AddTeamScore(level.intermission_origin,TEAM_RED,1);
					}
				}
				else if(((double)countsLiving[TEAM_RED])/((double)level.roundRedPlayers)>((double)countsLiving[TEAM_BLUE])/((double)level.roundBluePlayers))
				{
					//Red team has higher procentage survivors
					trap_SendServerCommand( -1, "print \"Red team has most survivers!\n\"");
					AddTeamScore(level.intermission_origin,TEAM_RED,1);
                                        if(g_gametype.integer == GT_ELIMINATION) {
                                            G_LogPrintf( "ELIMINATION: %i %i %i: %s wins round %i due to more survivors!\n", level.roundNumber, TEAM_RED, 2, TeamName(TEAM_RED), level.roundNumber );
                                        } else {
                                            G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: %s wins round %i due to more survivors!\n", level.roundNumber, -1, TEAM_RED, 7, TeamName(TEAM_RED), level.roundNumber );
                                        }
				}
				else if(((double)countsLiving[TEAM_RED])/((double)level.roundRedPlayers)<((double)countsLiving[TEAM_BLUE])/((double)level.roundBluePlayers))
				{
					//Blue team has higher procentage survivors
					trap_SendServerCommand( -1, "print \"Blue team has most survivers!\n\"");
					AddTeamScore(level.intermission_origin,TEAM_BLUE,1);	
                                        if(g_gametype.integer == GT_ELIMINATION) {
                                            G_LogPrintf( "ELIMINATION: %i %i %i: %s wins round %i due to more survivors!\n", level.roundNumber, TEAM_BLUE, 2, TeamName(TEAM_BLUE), level.roundNumber );
                                        } else {
                                            G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: %s wins round %i due to more survivors!\n", level.roundNumber, -1, TEAM_BLUE, 7, TeamName(TEAM_BLUE), level.roundNumber );
                                        }
				}
				else if(countsHealth[TEAM_RED]>countsHealth[TEAM_BLUE])
				{
					//Red team has more health
					trap_SendServerCommand( -1, "print \"Red team has more health left!\n\"");
					AddTeamScore(level.intermission_origin,TEAM_RED,1);
                                        if(g_gametype.integer == GT_ELIMINATION) {
                                            G_LogPrintf( "ELIMINATION: %i %i %i: %s wins round %i due to more health left!\n", level.roundNumber, TEAM_RED, 3, TeamName(TEAM_RED), level.roundNumber );
                                        } else {
                                            G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: %s wins round %i due to more health left!\n", level.roundNumber, -1, TEAM_RED, 8, TeamName(TEAM_RED), level.roundNumber );
                                        }
				}
				else if(countsHealth[TEAM_RED]<countsHealth[TEAM_BLUE])
				{
					//Blue team has more health
					trap_SendServerCommand( -1, "print \"Blue team has more health left!\n\"");
					AddTeamScore(level.intermission_origin,TEAM_BLUE,1);
                                        if(g_gametype.integer == GT_ELIMINATION) {
                                            G_LogPrintf( "ELIMINATION: %i %i %i: %s wins round %i due to more health left!\n", level.roundNumber, TEAM_BLUE, 3, TeamName(TEAM_BLUE), level.roundNumber );
                                        } else {
                                            G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: %s wins round %i due to more health left!\n", level.roundNumber, -1, TEAM_BLUE, 8, TeamName(TEAM_BLUE), level.roundNumber );
                                        }
				}
			}
                        //Draw
                        if(g_gametype.integer == GT_ELIMINATION) {
                            G_LogPrintf( "ELIMINATION: %i %i %i: Round %i ended in a draw!\n", level.roundNumber, -1, 4, level.roundNumber );
                        } else {
                            G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: Round %i ended in a draw!\n", level.roundNumber, -1, -1, 9, level.roundNumber );
                        }
			EndEliminationRound();
		}

		//This might be better placed another place:
		if(g_elimination_activewarmup.integer<1)
			g_elimination_activewarmup.integer=1; //We need at least 1 second to spawn all players
		if(g_elimination_activewarmup.integer >= g_elimination_warmup.integer) //This must not be true
			g_elimination_warmup.integer = g_elimination_activewarmup.integer+1; //Increase warmup

		//Force respawn
		if(level.roundNumber!=level.roundNumberStarted && level.time>level.roundStartTime-1000*g_elimination_activewarmup.integer && !level.roundRespawned)
		{
			level.roundRespawned = qtrue;
			RespawnAll();
			SendEliminationMessageToAllClients();
		}

		if(level.time<=level.roundStartTime && level.time>level.roundStartTime-1000*g_elimination_activewarmup.integer)
		{
			RespawnDead();
		}
			

		if((level.roundNumber>level.roundNumberStarted)&&(level.time>=level.roundStartTime))
			StartEliminationRound();
	
		if(level.time+1000*g_elimination_warmup.integer-500>level.roundStartTime)
		if(counts[TEAM_BLUE]<1 || counts[TEAM_RED]<1)
		{
			RespawnDead(); //Allow players to run around anyway
			WarmupEliminationRound(); //Start over
			return;
		}

		if(level.warmupTime != 0) {
			if(level.time+1000*g_elimination_warmup.integer-500>level.roundStartTime)
			{
				RespawnDead();
				WarmupEliminationRound();
			}
		}
	}
}

/*
=============
CheckDomination
=============
*/
void CheckDomination(void) {
	int i;
        int scoreFactor = 1;

	if ( (level.numPlayingClients < 1) || (g_gametype.integer != GT_DOMINATION) ) {
		return;
	}

	//Do nothing if warmup
	if(level.warmupTime != 0)
		return; 

	//Don't score if we are in intermission. Just plain stupid
	if(level.intermissiontime)
		return;

	//Sago: I use if instead of while, since if the server stops for more than 2 seconds people should not be allowed to score anyway
	if(level.domination_points_count>3)
            scoreFactor = 2; //score more slowly if there are many points
        if(level.time>=level.dom_scoreGiven*DOM_SECSPERPOINT*scoreFactor) {
		for(i=0;i<level.domination_points_count;i++) {
			if ( level.pointStatusDom[i] == TEAM_RED )
				AddTeamScore(level.intermission_origin,TEAM_RED,1);
			if ( level.pointStatusDom[i] == TEAM_BLUE )
				AddTeamScore(level.intermission_origin,TEAM_BLUE,1);
		}
		level.dom_scoreGiven++;
		while(level.time>level.dom_scoreGiven*DOM_SECSPERPOINT*scoreFactor)
			level.dom_scoreGiven++;
		CalculateRanks();
	}
}

/*
=============
CheckTournament

Once a frame, check for changes in tournement player state
=============
*/
void CheckTournament( void ) {
	// check because we run 3 game frames before calling Connect and/or ClientBegin
	// for clients on a map_restart
	if ( level.numPlayingClients == 0 ) {
		return;
	}

	if ( g_gametype.integer == GT_TOURNAMENT ) {

		// pull in a spectator if needed
		if ( level.numPlayingClients < 2 ) {
			AddTournamentPlayer();
		}

		// if we don't have two players, go back to "waiting for players"
		if ( level.numPlayingClients != 2 ) {
			if ( level.warmupTime != -1 ) {
				level.warmupTime = -1;
				trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
				G_LogPrintf( "Warmup:\n" );
			}
			return;
		}

		if ( level.warmupTime == 0 ) {
			return;
		}

		// if the warmup is changed at the console, restart it
		if ( g_warmup.modificationCount != level.warmupModificationCount ) {
			level.warmupModificationCount = g_warmup.modificationCount;
			level.warmupTime = -1;
		}

		// if all players have arrived, start the countdown
		if ( level.warmupTime < 0 ) {
			if ( level.numPlayingClients == 2 ) {
				// fudge by -1 to account for extra delays
				if ( g_warmup.integer > 1 ) {
					level.warmupTime = level.time + ( g_warmup.integer - 1 ) * 1000;
				} else {
					level.warmupTime = 0;
				}

				trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
			}
			return;
		}

		// if the warmup time has counted down, restart
		if ( level.time > level.warmupTime ) {
			level.warmupTime += 10000;
			trap_Cvar_Set( "g_restarted", "1" );
			trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
			level.restarted = qtrue;
			return;
		}
	} else if ( g_gametype.integer != GT_SINGLE_PLAYER && level.warmupTime != 0 ) {
		int		counts[TEAM_NUM_TEAMS];
		qboolean	notEnough = qfalse;

		if ( g_gametype.integer > GT_TEAM && !g_ffa_gt ) {
			counts[TEAM_BLUE] = TeamCount( -1, TEAM_BLUE );
			counts[TEAM_RED] = TeamCount( -1, TEAM_RED );

			if (counts[TEAM_RED] < 1 || counts[TEAM_BLUE] < 1) {
				notEnough = qtrue;
			}
		} else if ( level.numPlayingClients < 2 ) {
			notEnough = qtrue;
		}

		if ( notEnough ) {
			if ( level.warmupTime != -1 ) {
				level.warmupTime = -1;
				trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
				G_LogPrintf( "Warmup:\n" );
			}
			return; // still waiting for team members
		}

		if ( level.warmupTime == 0 ) {
			return;
		}

		// if the warmup is changed at the console, restart it
		if ( g_warmup.modificationCount != level.warmupModificationCount ) {
			level.warmupModificationCount = g_warmup.modificationCount;
			level.warmupTime = -1;
		}

		// if all players have arrived, start the countdown
		if ( level.warmupTime < 0 ) {
			// fudge by -1 to account for extra delays
			level.warmupTime = level.time + ( g_warmup.integer - 1 ) * 1000;
			trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
			return;
		}

		// if the warmup time has counted down, restart
		if ( level.time > level.warmupTime ) {
			level.warmupTime += 10000;
			trap_Cvar_Set( "g_restarted", "1" );
			trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
			level.restarted = qtrue;
			return;
		}
	}
}


/*
==================
CheckVote
==================
*/
void CheckVote( void ) {
	if ( level.voteExecuteTime && level.voteExecuteTime < level.time ) {
		level.voteExecuteTime = 0;
		trap_SendConsoleCommand( EXEC_APPEND, va("%s\n", level.voteString ) );
	}
	if ( !level.voteTime ) {
		return;
	}
	if ( level.time - level.voteTime >= VOTE_TIME ) {
		trap_SendServerCommand( -1, "print \"Vote failed.\n\"" );
	} else {
		// ATVI Q3 1.32 Patch #9, WNF
		if ( level.voteYes > level.numVotingClients/2 ) {
			// execute the command, then remove the vote
			trap_SendServerCommand( -1, "print \"Vote passed.\n\"" );
			level.voteExecuteTime = level.time + 3000;
		} else if ( level.voteNo >= level.numVotingClients/2 ) {
			// same behavior as a timeout
			trap_SendServerCommand( -1, "print \"Vote failed.\n\"" );
		} else {
			// still waiting for a majority
			return;
		}
	}
	level.voteTime = 0;
	trap_SetConfigstring( CS_VOTE_TIME, "" );

}

/*
==================
PrintTeam
==================
*/
void PrintTeam(int team, char *message) {
	int i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if (level.clients[i].sess.sessionTeam != team)
			continue;
		trap_SendServerCommand( i, message );
	}
}

/*
==================
SetLeader
==================
*/
void SetLeader(int team, int client) {
	int i;

	if ( level.clients[client].pers.connected == CON_DISCONNECTED ) {
		PrintTeam(team, va("print \"%s is not connected\n\"", level.clients[client].pers.netname) );
		return;
	}
	if (level.clients[client].sess.sessionTeam != team) {
		PrintTeam(team, va("print \"%s is not on the team anymore\n\"", level.clients[client].pers.netname) );
		return;
	}
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if (level.clients[i].sess.sessionTeam != team)
			continue;
		if (level.clients[i].sess.teamLeader) {
			level.clients[i].sess.teamLeader = qfalse;
			ClientUserinfoChanged(i);
		}
	}
	level.clients[client].sess.teamLeader = qtrue;
	ClientUserinfoChanged( client );
	PrintTeam(team, va("print \"%s is the new team leader\n\"", level.clients[client].pers.netname) );
}

/*
==================
CheckTeamLeader
==================
*/
void CheckTeamLeader( int team ) {
	int i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if (level.clients[i].sess.sessionTeam != team)
			continue;
		if (level.clients[i].sess.teamLeader)
			break;
	}
	if (i >= level.maxclients) {
		for ( i = 0 ; i < level.maxclients ; i++ ) {
			if (level.clients[i].sess.sessionTeam != team)
				continue;
			if (!(g_entities[i].r.svFlags & SVF_BOT)) {
				level.clients[i].sess.teamLeader = qtrue;
				break;
			}
		}
		for ( i = 0 ; i < level.maxclients ; i++ ) {
			if (level.clients[i].sess.sessionTeam != team)
				continue;
			level.clients[i].sess.teamLeader = qtrue;
			break;
		}
	}
}

/*
==================
CheckTeamVote
==================
*/
void CheckTeamVote( int team ) {
	int cs_offset;

	if ( team == TEAM_RED )
		cs_offset = 0;
	else if ( team == TEAM_BLUE )
		cs_offset = 1;
	else
		return;

	if ( !level.teamVoteTime[cs_offset] ) {
		return;
	}
	if ( level.time - level.teamVoteTime[cs_offset] >= VOTE_TIME ) {
		trap_SendServerCommand( -1, "print \"Team vote failed.\n\"" );
	} else {
		if ( level.teamVoteYes[cs_offset] > level.numteamVotingClients[cs_offset]/2 ) {
			// execute the command, then remove the vote
			trap_SendServerCommand( -1, "print \"Team vote passed.\n\"" );
			//
			if ( !Q_strncmp( "leader", level.teamVoteString[cs_offset], 6) ) {
				//set the team leader
				SetLeader(team, atoi(level.teamVoteString[cs_offset] + 7));
			}
			else {
				trap_SendConsoleCommand( EXEC_APPEND, va("%s\n", level.teamVoteString[cs_offset] ) );
			}
		} else if ( level.teamVoteNo[cs_offset] >= level.numteamVotingClients[cs_offset]/2 ) {
			// same behavior as a timeout
			trap_SendServerCommand( -1, "print \"Team vote failed.\n\"" );
		} else {
			// still waiting for a majority
			return;
		}
	}
	level.teamVoteTime[cs_offset] = 0;
	trap_SetConfigstring( CS_TEAMVOTE_TIME + cs_offset, "" );

}


/*
==================
CheckCvars
==================
*/
void CheckCvars( void ) {
	static int lastMod = -1;

	if ( g_password.modificationCount != lastMod ) {
		lastMod = g_password.modificationCount;
		if ( *g_password.string && Q_stricmp( g_password.string, "none" ) ) {
			trap_Cvar_Set( "g_needpass", "1" );
		} else {
			trap_Cvar_Set( "g_needpass", "0" );
		}
	}
}

/*
=============
G_RunThink

Runs thinking code for this frame if necessary
=============
*/
void G_RunThink (gentity_t *ent) {
	float	thinktime;

	thinktime = ent->nextthink;
	if (thinktime <= 0) {
		return;
	}
	if (thinktime > level.time) {
		return;
	}
	
	ent->nextthink = 0;
	if (!ent->think) {
		G_Error ( "NULL ent->think");
	}
	ent->think (ent);
}

/*
================
G_RunFrame

Advances the non-player objects in the world
================
*/
void G_RunFrame( int levelTime ) {
	int			i;
	gentity_t	*ent;
	int			msec;
int start, end;

	// if we are waiting for the level to restart, do nothing
	if ( level.restarted ) {
		return;
	}

	level.framenum++;
	level.previousTime = level.time;
	level.time = levelTime;
	msec = level.time - level.previousTime;

	// get any cvar changes
	G_UpdateCvars();

	//
	// go through all allocated objects
	//
	start = trap_Milliseconds();
	ent = &g_entities[0];
	for (i=0 ; i<level.num_entities ; i++, ent++) {
		if ( !ent->inuse ) {
			continue;
		}

		// clear events that are too old
		if ( level.time - ent->eventTime > EVENT_VALID_MSEC ) {
			if ( ent->s.event ) {
				ent->s.event = 0;	// &= EV_EVENT_BITS;
				if ( ent->client ) {
					ent->client->ps.externalEvent = 0;
					// predicted events should never be set to zero
					//ent->client->ps.events[0] = 0;
					//ent->client->ps.events[1] = 0;
				}
			}
			if ( ent->freeAfterEvent ) {
				// tempEntities or dropped items completely go away after their event
				G_FreeEntity( ent );
				continue;
			} else if ( ent->unlinkAfterEvent ) {
				// items that will respawn will hide themselves after their pickup event
				ent->unlinkAfterEvent = qfalse;
				trap_UnlinkEntity( ent );
			}
		}

		// temporary entities don't think
		if ( ent->freeAfterEvent ) {
			continue;
		}

		if ( !ent->r.linked && ent->neverFree ) {
			continue;
		}

//unlagged - backward reconciliation #2
		// we'll run missiles separately to save CPU in backward reconciliation
/*
		if ( ent->s.eType == ET_MISSILE ) {
			G_RunMissile( ent );
			continue;
		}
*/
//unlagged - backward reconciliation #2

		if ( ent->s.eType == ET_ITEM || ent->physicsObject ) {
			G_RunItem( ent );
			continue;
		}

		if ( ent->s.eType == ET_MOVER ) {
			G_RunMover( ent );
			continue;
		}

		if ( i < MAX_CLIENTS ) {
			G_RunClient( ent );
			continue;
		}

		G_RunThink( ent );
	}

//unlagged - backward reconciliation #2
	// NOW run the missiles, with all players backward-reconciled
	// to the positions they were in exactly 50ms ago, at the end
	// of the last server frame
	G_TimeShiftAllClients( level.previousTime, NULL );

	ent = &g_entities[0];
	for (i=0 ; i<level.num_entities ; i++, ent++) {
		if ( !ent->inuse ) {
			continue;
		}

		// temporary entities don't think
		if ( ent->freeAfterEvent ) {
			continue;
		}

		if ( ent->s.eType == ET_MISSILE ) {
			G_RunMissile( ent );
		}
	}

	G_UnTimeShiftAllClients( NULL );
//unlagged - backward reconciliation #2

end = trap_Milliseconds();

start = trap_Milliseconds();
	// perform final fixups on the players
	ent = &g_entities[0];
	for (i=0 ; i < level.maxclients ; i++, ent++ ) {
		if ( ent->inuse ) {
			ClientEndFrame( ent );
		}
	}
end = trap_Milliseconds();

	// see if it is time to do a tournement restart
	CheckTournament();

	//Check Elimination state
	CheckElimination();
	CheckLMS();

	//Check Double Domination
	CheckDoubleDomination();

	CheckDomination();

	//Sago: I just need to think why I placed this here... they should only spawn once
	if(g_gametype.integer == GT_DOMINATION)
		Team_Dom_SpawnPoints();

	// see if it is time to end the level
	CheckExitRules();

	// update to team status?
	CheckTeamStatus();

	// cancel vote if timed out
	CheckVote();

	// check team votes
	CheckTeamVote( TEAM_RED );
	CheckTeamVote( TEAM_BLUE );

	// for tracking changes
	CheckCvars();

	if (g_listEntity.integer) {
		for (i = 0; i < MAX_GENTITIES; i++) {
			G_Printf("%4i: %s\n", i, g_entities[i].classname);
		}
		trap_Cvar_Set("g_listEntity", "0");
	}

//unlagged - backward reconciliation #4
	// record the time at the end of this frame - it should be about
	// the time the next frame begins - when the server starts
	// accepting commands from connected clients
	level.frameStartTime = trap_Milliseconds();
//unlagged - backward reconciliation #4
}

