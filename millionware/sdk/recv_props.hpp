#pragma once

#include <cstdint>

#include "typedefs.hpp"
#include "vector.hpp"

#define DISPSURF_FLAG_SURFACE           (1<<0)
#define DISPSURF_FLAG_WALKABLE          (1<<1)
#define DISPSURF_FLAG_BUILDABLE         (1<<2)
#define DISPSURF_FLAG_SURFPROP1         (1<<3)
#define DISPSURF_FLAG_SURFPROP2         (1<<4)

#define CONTENTS_SOLID                0x1
#define CONTENTS_WINDOW               0x2
#define CONTENTS_AUX                  0x4
#define CONTENTS_GRATE                0x8
#define CONTENTS_SLIME                0x10
#define CONTENTS_WATER                0x20
#define CONTENTS_BLOCKLOS             0x40
#define CONTENTS_OPAQUE               0x80
#define LAST_VISIBLE_CONTENTS         CONTENTS_OPAQUE

#define CONTENTS_TESTFOGVOLUME        0x100
#define CONTENTS_UNUSED               0x200
#define CONTENTS_BLOCKLIGHT           0x400
#define CONTENTS_TEAM1                0x800
#define CONTENTS_TEAM2                0x1000
#define CONTENTS_IGNORE_NODRAW_OPAQUE 0x2000
#define CONTENTS_MOVEABLE             0x4000
#define CONTENTS_AREAPORTAL           0x8000
#define CONTENTS_PLAYERCLIP           0x10000
#define CONTENTS_MONSTERCLIP          0x20000
#define CONTENTS_CURRENT_0            0x40000
#define CONTENTS_CURRENT_90           0x80000
#define CONTENTS_CURRENT_180          0x100000
#define CONTENTS_CURRENT_270          0x200000
#define CONTENTS_CURRENT_UP           0x400000
#define CONTENTS_CURRENT_DOWN         0x800000

#define CONTENTS_MONSTER              0x2000000
#define CONTENTS_DEBRIS               0x4000000
#define CONTENTS_DETAIL               0x8000000
#define CONTENTS_TRANSLUCENT          0x10000000
#define CONTENTS_LADDER               0x20000000
#define CONTENTS_HITBOX               0x40000000

#define MASK_ALL                      (0xFFFFFFFF)
#define MASK_SOLID                    (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define MASK_PLAYERSOLID              (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define MASK_NPCSOLID                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define MASK_NPCFLUID                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER)
#define MASK_WATER                    (CONTENTS_WATER|CONTENTS_MOVEABLE|CONTENTS_SLIME)
#define MASK_OPAQUE                   (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_OPAQUE)
#define MASK_OPAQUE_AND_NPCS          (MASK_OPAQUE|CONTENTS_MONSTER)
#define MASK_BLOCKLOS                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_BLOCKLOS)
#define MASK_BLOCKLOS_AND_NPCS        (MASK_BLOCKLOS|CONTENTS_MONSTER)
#define MASK_VISIBLE                  (MASK_OPAQUE|CONTENTS_IGNORE_NODRAW_OPAQUE)
#define MASK_VISIBLE_AND_NPCS         (MASK_OPAQUE_AND_NPCS|CONTENTS_IGNORE_NODRAW_OPAQUE)
#define MASK_SHOT                     (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX)
#define MASK_SHOT_BRUSHONLY           (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_DEBRIS)
#define MASK_SHOT_HULL                (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_GRATE)
#define MASK_SHOT_PORTAL              (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER)
#define MASK_SOLID_BRUSHONLY          (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE)
#define MASK_PLAYERSOLID_BRUSHONLY    (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_PLAYERCLIP|CONTENTS_GRATE)
#define MASK_NPCSOLID_BRUSHONLY       (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
#define MASK_NPCWORLDSTATIC           (CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
#define MASK_NPCWORLDSTATIC_FLUID     (CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP)
#define MASK_SPLITAREAPORTAL          (CONTENTS_WATER|CONTENTS_SLIME)
#define MASK_CURRENT                  (CONTENTS_CURRENT_0|CONTENTS_CURRENT_90|CONTENTS_CURRENT_180|CONTENTS_CURRENT_270|CONTENTS_CURRENT_UP|CONTENTS_CURRENT_DOWN)
#define MASK_DEADSOLID                (CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_GRATE)

enum e_send_prop_type {
	SEND_PROP_TYPE_INT32 = 0,
	SEND_PROP_TYPE_FLOAT,
	SEND_PROP_TYPE_VECTOR3,
	SEND_PROP_TYPE_VECTOR2,
	SEND_PROP_TYPE_STRING,
	SEND_PROP_TYPE_ARRAY,
	SEND_PROP_TYPE_DATA_TABLE,
	SEND_PROP_TYPE_INT64,
};

enum class_id {
	cai_basenpc,
	cak47,
	cbaseanimating,
	cbaseanimatingoverlay,
	cbaseattributableitem,
	cbasebutton,
	cbasecombatcharacter,
	cbasecombatweapon,
	cbasecsgrenade,
	cbasecsgrenadeprojectile,
	cbasedoor,
	cbaseentity,
	cbaseflex,
	cbasegrenade,
	cbaseparticleentity,
	cbaseplayer,
	cbasepropdoor,
	cbaseteamobjectiveresource,
	cbasetempentity,
	cbasetoggle,
	cbasetrigger,
	cbaseviewmodel,
	cbasevphysicstrigger,
	cbaseweaponworldmodel,
	cbeam,
	cbeamspotlight,
	cbonefollower,
	cbrc4target,
	cbreachcharge,
	cbreachchargeprojectile,
	cbreakableprop,
	cbreakablesurface,
	cbumpmine,
	cbumpmineprojectile,
	cc4,
	ccascadelight,
	cchicken,
	ccolorcorrection,
	ccolorcorrectionvolume,
	ccsgamerulesproxy,
	ccsplayer,
	ccsplayerresource,
	ccsragdoll,
	ccsteam,
	cdangerzone,
	cdangerzonecontroller,
	cdeagle,
	cdecoygrenade,
	cdecoyprojectile,
	cdrone,
	cdronegun,
	cdynamiclight,
	cdynamicprop,
	ceconentity,
	ceconwearable,
	cembers,
	centitydissolve,
	centityflame,
	centityfreezing,
	centityparticletrail,
	cenvambientlight,
	cenvdetailcontroller,
	cenvdofcontroller,
	cenvgascanister,
	cenvparticlescript,
	cenvprojectedtexture,
	cenvquadraticbeam,
	cenvscreeneffect,
	cenvscreenoverlay,
	cenvtonemapcontroller,
	cenvwind,
	cfeplayerdecal,
	cfirecrackerblast,
	cfiresmoke,
	cfiretrail,
	cfish,
	cfists,
	cflashbang,
	cfogcontroller,
	cfootstepcontrol,
	cfunc_dust,
	cfunc_lod,
	cfuncareaportalwindow,
	cfuncbrush,
	cfuncconveyor,
	cfuncladder,
	cfuncmonitor,
	cfuncmovelinear,
	cfuncoccluder,
	cfuncreflectiveglass,
	cfuncrotating,
	cfuncsmokevolume,
	cfunctracktrain,
	cgamerulesproxy,
	cgrassburn,
	chandletest,
	chegrenade,
	chostage,
	chostagecarriableprop,
	cincendiarygrenade,
	cinferno,
	cinfoladderdismount,
	cinfomapregion,
	cinfooverlayaccessor,
	citem_healthshot,
	citemcash,
	citemdogtags,
	cknife,
	cknifegg,
	clightglow,
	cmaterialmodifycontrol,
	cmelee,
	cmolotovgrenade,
	cmolotovprojectile,
	cmoviedisplay,
	cparadropchopper,
	cparticlefire,
	cparticleperformancemonitor,
	cparticlesystem,
	cphysbox,
	cphysboxmultiplayer,
	cphysicsprop,
	cphysicspropmultiplayer,
	cphysmagnet,
	cphyspropammobox,
	cphysproplootcrate,
	cphyspropradarjammer,
	cphyspropweaponupgrade,
	cplantedc4,
	cplasma,
	cplayerping,
	cplayerresource,
	cpointcamera,
	cpointcommentarynode,
	cpointworldtext,
	cposecontroller,
	cpostprocesscontroller,
	cprecipitation,
	cprecipitationblocker,
	cpredictedviewmodel,
	cprop_hallucination,
	cpropcounter,
	cpropdoorrotating,
	cpropjeep,
	cpropvehicledriveable,
	cragdollmanager,
	cragdollprop,
	cragdollpropattached,
	cropekeyframe,
	cscar17,
	csceneentity,
	csensorgrenade,
	csensorgrenadeprojectile,
	cshadowcontrol,
	cslideshowdisplay,
	csmokegrenade,
	csmokegrenadeprojectile,
	csmokestack,
	csnowball,
	csnowballpile,
	csnowballprojectile,
	cspatialentity,
	cspotlightend,
	csprite,
	cspriteoriented,
	cspritetrail,
	cstatueprop,
	csteamjet,
	csun,
	csunlightshadowcontrol,
	csurvivalspawnchopper,
	ctablet,
	cteam,
	cteamplayroundbasedrulesproxy,
	ctearmorricochet,
	ctebasebeam,
	ctebeamentpoint,
	ctebeaments,
	ctebeamfollow,
	ctebeamlaser,
	ctebeampoints,
	ctebeamring,
	ctebeamringpoint,
	ctebeamspline,
	ctebloodsprite,
	ctebloodstream,
	ctebreakmodel,
	ctebspdecal,
	ctebubbles,
	ctebubbletrail,
	cteclientprojectile,
	ctedecal,
	ctedust,
	ctedynamiclight,
	cteeffectdispatch,
	cteenergysplash,
	cteexplosion,
	ctefirebullets,
	ctefizz,
	ctefootprintdecal,
	ctefoundryhelpers,
	ctegaussexplosion,
	cteglowsprite,
	cteimpact,
	ctekillplayerattachments,
	ctelargefunnel,
	ctemetalsparks,
	ctemuzzleflash,
	cteparticlesystem,
	ctephysicsprop,
	cteplantbomb,
	cteplayeranimevent,
	cteplayerdecal,
	cteprojecteddecal,
	cteradioicon,
	cteshattersurface,
	cteshowline,
	ctesla,
	ctesmoke,
	ctesparks,
	ctesprite,
	ctespritespray,
	ctest_proxytoggle_networkable,
	ctesttraceline,
	cteworlddecal,
	ctriggerplayermovement,
	ctriggersoundoperator,
	cvguiscreen,
	cvotecontroller,
	cwaterbullet,
	cwaterlodcontrol,
	cweaponaug,
	cweaponawp,
	cweaponbaseitem,
	cweaponbizon,
	cweaponcsbase,
	cweaponcsbasegun,
	cweaponcycler,
	cweaponelite,
	cweaponfamas,
	cweaponfiveseven,
	cweapong3sg1,
	cweapongalil,
	cweapongalilar,
	cweaponglock,
	cweaponhkp2000,
	cweaponm249,
	cweaponm3,
	cweaponm4a1,
	cweaponmac10,
	cweaponmag7,
	cweaponmp5navy,
	cweaponmp7,
	cweaponmp9,
	cweaponnegev,
	cweaponnova,
	cweaponp228,
	cweaponp250,
	cweaponp90,
	cweaponsawedoff,
	cweaponscar20,
	cweaponscout,
	cweaponsg550,
	cweaponsg552,
	cweaponsg556,
	cweaponshield,
	cweaponssg08,
	cweapontaser,
	cweapontec9,
	cweapontmp,
	cweaponump45,
	cweaponusp,
	cweaponxm1014,
	cweaponzonerepulsor,
	cworld,
	cworldvguitext,
	dusttrail,
	movieexplosion,
	particlesmokegrenade,
	rockettrail,
	smoketrail,
	sporeexplosion,
	sporetrail,
};

struct cplane_t {
	vector3_t normal;
	float m_dist;
	char m_type;
	char m_sign_bits;
	char m_pad[2];
};

struct csurface_t {
	const char* name;
	int16_t props;
	uint16_t flags;
};

struct recv_prop_t {
	const char* var_name;
	int recv_type;
	int flags;
	int string_buffer_size;
	bool inside_array;
	const void* extra_data;
	recv_prop_t* array_prop;
	array_length_recv_proxy_fn array_length_proxy;
	recv_var_proxy_fn proxy_fn;
	data_table_recv_var_proxy_fn data_table_proxy_fn;
	struct recv_table_t* data_table;
	int offset;
	int element_stride;
	int elements;
	const char* parent_array_prop_name;
};

struct recv_table_t {
	recv_prop_t* props;
	int prop_count;
	uintptr_t decoder;
	const char* net_table_name;
	bool initialized;
	bool in_main_list;
};
