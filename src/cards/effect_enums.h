#ifndef __CARDS_EFFECT_ENUMS_H__
#define __CARDS_EFFECT_ENUMS_H__

#include "utils/enums.h"

#include "filter_enums.h"

namespace banggame {

    DEFINE_ENUM_FWD_TYPES(effect_type,
        (none)
        (mth_add)
        (ctx_add)
        (max_usages,            effect_max_usages)
        (pass_turn,             effect_pass_turn)
        (resolve,               effect_resolve)
        (bang,                  effect_bang)
        (bangcard,              effect_bangcard)
        (banglimit,             effect_banglimit)
        (missed,                effect_missed)
        (barrel,                effect_barrel)
        (discard,               effect_discard)
        (destroy,               effect_destroy)
        (startofturn,           effect_startofturn)
        (while_drawing,         effect_while_drawing)
        (reset_drawing,         effect_reset_drawing)
        (end_drawing,           effect_end_drawing)
        (steal,                 effect_steal)
        (duel,                  effect_duel)
        (beer,                  effect_beer)
        (heal,                  effect_heal)
        (heal_notfull,          effect_heal_notfull)
        (indians,               effect_indians)
        (draw,                  effect_draw)
        (draw_discard,          effect_draw_discard)
        (draw_to_discard,       effect_draw_to_discard)
        (draw_one_less,         effect_draw_one_less)
        (generalstore,          effect_generalstore)
        (deathsave,             effect_deathsave)
        (backfire,              effect_backfire)
        (bandidos,              effect_bandidos)
        (aim,                   effect_aim)
        (poker,                 effect_poker)
        (tornado,               effect_tornado)
        (damage,                effect_damage)
        (saved,                 effect_saved)
        (escape,                effect_escape)
        (sell_beer,             effect_sell_beer)
        (discard_black,         effect_discard_black)
        (add_gold,              effect_add_gold)
        (pay_gold,              effect_pay_gold)
        (rum,                   effect_rum)
        (goldrush,              effect_goldrush)
        (forced_play,           effect_forced_play)
        (forced_equip,          effect_forced_equip)
        (card_choice,           effect_card_choice)
        (pay_cube,              effect_pay_cube)
        (add_cube,              effect_add_cube)
        (rust,                  effect_rust)
        (doublebarrel,          effect_doublebarrel)
        (thunderer,             effect_thunderer)
        (buntlinespecial,       effect_buntlinespecial)
        (bigfifty,              effect_bigfifty)
        (move_bomb,             effect_move_bomb)
        (tumbleweed,            effect_tumbleweed)
        (sniper,                effect_sniper)
        (ricochet,              effect_ricochet)
        (ranch,                 effect_ranch)
        (greygory_deck,         effect_greygory_deck)
        (lemonade_jim,          effect_lemonade_jim)
        (josh_mccloud,          effect_josh_mccloud)
        (frankie_canton,        effect_frankie_canton)
        (evelyn_shebang,        effect_evelyn_shebang)
        (al_preacher,           effect_al_preacher)
        (ms_abigail,            effect_ms_abigail)
        (graverobber,           effect_graverobber)
        (mirage,                effect_mirage)
        (disarm,                effect_disarm)
        (sacrifice,             effect_sacrifice)
        (lastwill,              effect_lastwill)
        (ladyrosaoftexas,       effect_ladyrosaoftexas)
        (changewws,             effect_changewws)
        (cactus,                effect_cactus)
        (evaded,                effect_evaded)
        (full_steam,            effect_full_steam)
        (knife_revolver,        effect_knife_revolver)
        (most_wanted,           effect_most_wanted)
        (next_stop,             effect_next_stop)
        (train_robbery)
        (shade_oconnor,         effect_shade_oconnor)
        (lounge_car)
        (cattle_truck)
        (circus_wagon)
        (express_car)
        (mail_car)
    )

    DEFINE_ENUM_FWD_TYPES(equip_type,
        (none)
        (mustang,               equip_mustang)
        (scope,                 equip_scope)
        (predraw_check,         equip_predraw_check)
        (jail,                  equip_jail)
        (dynamite,              equip_dynamite)
        (horse,                 equip_horse)
        (weapon,                equip_weapon)
        (volcanic,              equip_volcanic)
        (pixie_pete,            equip_pixie_pete)
        (apache_kid,            equip_apache_kid)
        (bart_cassidy,          equip_bart_cassidy)
        (ghost,                 equip_ghost)
        (snake,                 equip_snake)
        (shotgun,               equip_shotgun)
        (bounty,                equip_bounty)
        (el_gringo,             equip_el_gringo)
        (lucky_duke,            equip_lucky_duke)
        (luckycharm,            equip_luckycharm)
        (gunbelt,               equip_gunbelt)
        (initialcards,          equip_initialcards)
        (wanted,                equip_wanted)
        (bomb,                  equip_bomb)
        (tumbleweed,            equip_tumbleweed)
        (bronco,                equip_bronco)
        (calamity_janet,        equip_calamity_janet)
        (black_jack,            equip_black_jack)
        (kit_carlson,           equip_kit_carlson)
        (claus_the_saint,       equip_claus_the_saint)
        (bill_noface,           equip_bill_noface)
        (slab_the_killer,       equip_slab_the_killer)
        (suzy_lafayette,        equip_suzy_lafayette)
        (vulture_sam,           equip_vulture_sam)
        (johnny_kisch,          equip_johnny_kisch)
        (bellestar,             equip_bellestar)
        (greg_digger,           equip_greg_digger)
        (herb_hunter,           equip_herb_hunter)
        (molly_stark,           equip_molly_stark)
        (tequila_joe,           equip_tequila_joe)
        (vera_custer,           equip_vera_custer)
        (tuco_franziskaner,     equip_tuco_franziskaner)
        (colorado_bill,         equip_colorado_bill)
        (henry_block,           equip_henry_block)
        (big_spencer,           equip_big_spencer)
        (teren_kill,            equip_teren_kill)
        (gary_looter,           equip_gary_looter)
        (john_pain,             equip_john_pain)
        (youl_grinner,          equip_youl_grinner)
        (don_bell,              equip_don_bell)
        (dutch_will,            equip_dutch_will)
        (madam_yto,             equip_madam_yto)
        (greygory_deck,         equip_greygory_deck)
        (lemonade_jim,          equip_lemonade_jim)
        (mick_defender,         equip_mick_defender)
        (julie_cutter,          equip_julie_cutter)
        (bloody_mary,           equip_bloody_mary)
        (red_ringo,             equip_red_ringo)
        (al_preacher,           equip_al_preacher)
        (ms_abigail,            equip_ms_abigail)
        (blessing,              equip_blessing)
        (curse,                 equip_curse)
        (thedaltons,            equip_thedaltons)
        (thedoctor,             equip_thedoctor)
        (trainarrival,          equip_trainarrival)
        (thirst,                equip_thirst)
        (highnoon,              equip_highnoon)
        (shootout,              equip_shootout)
        (invert_rotation,       equip_invert_rotation)
        (reverend,              equip_reverend)
        (hangover,              equip_hangover)
        (sermon,                equip_sermon)
        (ghosttown,             equip_ghosttown)
        (handcuffs,             equip_handcuffs)
        (ranch,                 equip_ranch)
        (ambush,                equip_ambush)
        (lasso,                 equip_lasso)
        (judge,                 equip_judge)
        (peyote,                equip_peyote)
        (russianroulette,       equip_russianroulette)
        (abandonedmine,         equip_abandonedmine)
        (deadman,               equip_deadman)
        (fistfulofcards,        equip_fistfulofcards)
        (packmule,              equip_packmule)
        (indianguide,           equip_indianguide)
        (taxman,                equip_taxman)
        (brothel,               equip_brothel)
        (lastwill,              equip_lastwill)
        (newidentity,           equip_newidentity)
        (lawofthewest,          equip_lawofthewest)
        (vendetta,              equip_vendetta)
        (bone_orchard,          equip_bone_orchard)
        (darling_valentine,     equip_darling_valentine)
        (helena_zontero,        equip_helena_zontero)
        (miss_susanna,          equip_miss_susanna)
        (showdown,              equip_showdown)
        (sacagaway,             equip_sacagaway)
        (wildwestshow,          equip_wildwestshow)
        (dragoon,               equip_dragoon)
        (map,                   equip_map)
        (refund,                equip_refund)
        (strongbox,             equip_strongbox)
        (benny_brawler,         equip_benny_brawler)
        (manuelita,             equip_manuelita)
        (shade_oconnor,         equip_shade_oconnor)
        (ironhorse,             equip_ironhorse)
        (leland,                equip_leland)
        (prisoner_car,          equip_prisoner_car)
        (private_car,           equip_private_car)
        (dining_car,            equip_dining_car)
        (lumber_flatcar,        equip_lumber_flatcar)
        (ghost_car)
    )

    DEFINE_ENUM_FWD_TYPES(mth_type,
        (none)
        (play_as_bang,          handler_play_as_bang)
        (fanning,               handler_fanning)
        (doc_holyday,           handler_doc_holyday)
        (flint_westwood,        handler_flint_westwood)
        (draw_multi,            handler_draw_multi)
        (heal_multi,            handler_heal_multi)
        (flintlock,             handler_flintlock)
        (duck,                  handler_duck)
        (squaw,                 handler_squaw)
        (red_ringo,             handler_red_ringo)
        (lastwill,              handler_lastwill)
        (switch_cards,          handler_switch_cards)
        (evan_babbit,           handler_evan_babbit)
    )

    DEFINE_ENUM_FWD_TYPES(modifier_type,
        (none)
        (bangmod,               modifier_bangmod)
        (doublebarrel,          modifier_doublebarrel)
        (bandolier,             modifier_bandolier)
        (belltower,             modifier_belltower)
        (card_choice,           modifier_card_choice)
        (discount,              modifier_discount)
        (leevankliff,           modifier_leevankliff)
        (moneybag,              modifier_moneybag)
        (skip_player,           modifier_skip_player)
        (traincost,             modifier_traincost)
        (locomotive,            modifier_locomotive)
    )

}

#endif