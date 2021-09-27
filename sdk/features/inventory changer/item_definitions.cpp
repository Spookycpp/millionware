#include "item_definitions.h"

#include "../../engine/security/xorstr.h"
#include "../../source engine/entity.h"

#include <map>

bool is_knife(const int index) {
    return index >= WEAPON_KNIFE_BAYONET && index < GLOVE_STUDDED_BLOODHOUND || index == WEAPON_KNIFE_T || index == WEAPON_KNIFE;
}

bool is_glove(const int index) {
    return index >= GLOVE_STUDDED_BLOODHOUND && index <= GLOVE_HYDRA;
}

const item_definitions::item_info_t *item_definitions::get_item_info(const int definition_idx) {
    const static std::map<int, item_info_t> info = {
        {WEAPON_KNIFE, {"models/weapons/v_knife_default_ct.mdl", "knife_default_ct"}},
        {WEAPON_KNIFE_T, {"models/weapons/v_knife_default_t.mdl", "knife_t"}},
        {WEAPON_KNIFE_BAYONET, {"models/weapons/v_knife_bayonet.mdl", "bayonet"}},
        {WEAPON_KNIFE_CSS, {"models/weapons/v_knife_css.mdl", "knife_css"}},
        {WEAPON_KNIFE_FLIP, {"models/weapons/v_knife_flip.mdl", "knife_flip"}},
        {WEAPON_KNIFE_GUT, {"models/weapons/v_knife_gut.mdl", "knife_gut"}},
        {WEAPON_KNIFE_KARAMBIT, {"models/weapons/v_knife_karam.mdl", "knife_karambit"}},
        {WEAPON_KNIFE_M9_BAYONET, {"models/weapons/v_knife_m9_bay.mdl", "knife_m9_bayonet"}},
        {WEAPON_KNIFE_TACTICAL, {"models/weapons/v_knife_tactical.mdl", "knife_tactical"}},
        {WEAPON_KNIFE_FALCHION, {"models/weapons/v_knife_falchion_advanced.mdl", "knife_falchion"}},
        {WEAPON_KNIFE_SURVIVAL_BOWIE, {"models/weapons/v_knife_survival_bowie.mdl", "knife_survival_bowie"}},
        {WEAPON_KNIFE_BUTTERFLY, {"models/weapons/v_knife_butterfly.mdl", "knife_butterfly"}},
        {WEAPON_KNIFE_PUSH, {"models/weapons/v_knife_push.mdl", "knife_push"}},
        {WEAPON_KNIFE_CORD, {"models/weapons/v_knife_cord.mdl", "knife_cord"}},
        {WEAPON_KNIFE_CANIS, {"models/weapons/v_knife_canis.mdl", "knife_canis"}},
        {WEAPON_KNIFE_URSUS, {"models/weapons/v_knife_ursus.mdl", "knife_ursus"}},
        {WEAPON_KNIFE_GYPSY_JACKKNIFE, {"models/weapons/v_knife_gypsy_jackknife.mdl", "knife_gypsy_jackknife"}},
        {WEAPON_KNIFE_OUTDOOR, {"models/weapons/v_knife_outdoor.mdl", "knife_outdoor"}},
        {WEAPON_KNIFE_STILETTO, {"models/weapons/v_knife_stiletto.mdl", "knife_stiletto"}},
        {WEAPON_KNIFE_WIDOWMAKER, {"models/weapons/v_knife_widowmaker.mdl", "knife_widowmaker"}},
        {WEAPON_KNIFE_SKELETON, {"models/weapons/v_knife_skeleton.mdl", "knife_skeleton"}}};

    const auto entry = info.find(definition_idx);
    return entry == std::end(info) ? nullptr : &entry->second;
}

const std::vector<item_definitions::item_name_t> item_definitions::knife_names = {{WEAPON_KNIFE_BAYONET, "Bayonet"},
                                                                                  {WEAPON_KNIFE_CSS, "Classic Knife"},
                                                                                  {WEAPON_KNIFE_FLIP, "Flip Knife"},
                                                                                  {WEAPON_KNIFE_GUT, "Gut Knife"},
                                                                                  {WEAPON_KNIFE_KARAMBIT, "Karambit"},
                                                                                  {WEAPON_KNIFE_M9_BAYONET, "M9 Bayonet"},
                                                                                  {WEAPON_KNIFE_TACTICAL, "Huntsman Knife"},
                                                                                  {WEAPON_KNIFE_FALCHION, "Falchion Knife"},
                                                                                  {WEAPON_KNIFE_SURVIVAL_BOWIE, "Bowie Knife"},
                                                                                  {WEAPON_KNIFE_BUTTERFLY, "Butterfly Knife"},
                                                                                  {WEAPON_KNIFE_PUSH, "Shadow Daggers"},
                                                                                  {WEAPON_KNIFE_CORD, "Paracord Knife"},
                                                                                  {WEAPON_KNIFE_CANIS, "Survival Knife"},
                                                                                  {WEAPON_KNIFE_URSUS, "Ursus Knife"},
                                                                                  {WEAPON_KNIFE_GYPSY_JACKKNIFE, "Navaja Knife"},
                                                                                  {WEAPON_KNIFE_OUTDOOR, "Nomad Knife"},
                                                                                  {WEAPON_KNIFE_STILETTO, "Stiletto Knife"},
                                                                                  {WEAPON_KNIFE_WIDOWMAKER, "Talon Knife"},
                                                                                  {WEAPON_KNIFE_SKELETON, "Skeleton Knife"}};

const std::vector<item_definitions::item_name_t> item_definitions::glove_names = {{GLOVE_STUDDED_BLOODHOUND, "Bloodhound"},
                                                                                  {GLOVE_T_SIDE, "Default (T)"},
                                                                                  {GLOVE_CT_SIDE, "Default (CT)"},
                                                                                  {GLOVE_SPORTY, "Sporty"},
                                                                                  {GLOVE_SLICK, "Slick"},
                                                                                  {GLOVE_LEATHER_WRAP, "Hand Wraps"},
                                                                                  {GLOVE_MOTORCYCLE, "Motorcycle"},
                                                                                  {GLOVE_SPECIALIST, "Specialist"},
                                                                                  {GLOVE_HYDRA, "Hydra"}};

const std::vector<item_definitions::item_name_t> item_definitions::weapon_names = {
    {7, "AK-47"},      {8, "AUG"},     {9, "AWP"},       {63, "CZ75 Auto"}, {1, "Desert Eagle"}, {2, "Dual Berettas"}, {10, "FAMAS"},
    {3, "Five-SeveN"}, {11, "G3SG1"},  {13, "Galil AR"}, {4, "Glock-18"},   {14, "M249"},        {60, "M4A1-S"},       {16, "M4A4"},
    {17, "MAC-10"},    {27, "MAG-7"},  {23, "MP5-SD"},   {33, "MP7"},       {34, "MP9"},         {28, "Negev"},        {35, "Nova"},
    {32, "P2000"},     {36, "P250"},   {19, "P90"},      {26, "PP-Bizon"},  {64, "R8 Revolver"}, {29, "Sawed-Off"},    {38, "SCAR-20"},
    {40, "SSG 08"},    {39, "SG 553"}, {30, "Tec-9"},    {24, "UMP-45"},    {61, "USP-S"},       {25, "XM1014"}};

const std::vector<item_definitions::quality_name_t> item_definitions::quality_names = {{0, "Normal"},
                                                                                       {1, "Genuine"},
                                                                                       {2, "Vintage"},
                                                                                       {3, "Unusual" /*Knife/Glove*/},
                                                                                       {5, "Community"},
                                                                                       {6, "Developer"},
                                                                                       {7, "Self-Made"},
                                                                                       {8, "Customized"},
                                                                                       {9, "Strange" /*StatTrak*/},
                                                                                       {10, "Completed"},
                                                                                       {12, "Tournament" /*Souvinir*/}};

const std::vector<item_definitions::rarity_name_t> item_definitions::rarity_names = {
    {1, "Consumer grade"}, {2, "Industrial grade"}, {3, "Mil-spec"},  {4, "Restricted"},
    {5, "Classified"},     {6, "Covert"},           {7, "Contraband"}};