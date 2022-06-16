#
#  MILLENIUM-STUDIO
#  Copyright 2016 Millenium-studio SARL
#  All Rights Reserved.
#

if (USE_SCRIPTPCH)
  set(scriptsPD_STAT_PCH_HDR PrecompiledHeaders/PandariaPCH.h)
  set(scriptsPD_STAT_PCH_SRC PrecompiledHeaders/PandariaPCH.cpp)
endif ()

set(scriptsPD_STAT_SRCS
  ${scriptsPD_STAT_SRCS}
  ../../game/AI/ScriptedAI/ScriptedEscortAI.cpp
  ../../game/AI/ScriptedAI/ScriptedCreature.cpp
  ../../game/AI/ScriptedAI/ScriptedFollowerAI.cpp
  boss_sha_of_anger.cpp
  boss_galion.cpp
  boss_chi_ji.cpp
  boss_nalak.cpp
  boss_niuzao.cpp
  boss_ordos.cpp
  boss_xuen.cpp
  boss_yu_lon.cpp
  boss_oondasta.cpp

  MogushanPalace/instance_mogu_shan_palace.cpp
  MogushanPalace/boss_trial_of_the_king.cpp
  MogushanPalace/boss_gekkan.cpp
  MogushanPalace/boss_xin_the_weaponmaster.cpp

  TempleJadeSerpent/instance_temple_of_jade_serpent.cpp
  TempleJadeSerpent/boss_wise_mari.cpp
  TempleJadeSerpent/boss_lorewalker_stonestep.cpp
  TempleJadeSerpent/boss_liu_flameheart.cpp
  TempleJadeSerpent/boss_sha_of_doubt.cpp

  StormstoutBrewery/instance_stormstout_brewery.cpp
  StormstoutBrewery/stormstout_brewery.cpp
  StormstoutBrewery/stormstout_brewery.h
  StormstoutBrewery/boss_ook_ook.cpp

  GateSettingSun/instance_gate_setting_sun.cpp
  GateSettingSun/gate_setting_sun.cpp
  GateSettingSun/gate_setting_sun.h
  GateSettingSun/boss_saboteur_kiptilak.cpp
  GateSettingSun/boss_striker_gadok.cpp
  GateSettingSun/boss_commander_rimok.cpp
  GateSettingSun/boss_raigonn.cpp

  ShadopanMonastery/instance_shadopan_monastery.cpp
  ShadopanMonastery/shadopan_monastery.cpp
  ShadopanMonastery/shadopan_monastery.h
  ShadopanMonastery/boss_gu_cloudstrike.cpp
  ShadopanMonastery/boss_master_snowdrift.cpp
  ShadopanMonastery/boss_sha_of_violence.cpp
  ShadopanMonastery/boss_taran_zhu.cpp

  SiegeNiuzaoTemple/instance_siege_of_the_niuzao_temple.cpp
  SiegeNiuzaoTemple/siege_of_the_niuzao_temple.cpp
  SiegeNiuzaoTemple/siege_of_the_niuzao_temple.h
  SiegeNiuzaoTemple/boss_jinbak.cpp

  MogushanVault/instance_mogu_shan_vault.cpp
  MogushanVault/mogu_shan_vault.cpp
  MogushanVault/mogu_shan_vault.h
  MogushanVault/boss_stone_guard.cpp
  MogushanVault/boss_feng.cpp
  MogushanVault/boss_garajal.cpp
  MogushanVault/boss_spirit_kings.cpp
  MogushanVault/boss_elegon.cpp
  MogushanVault/boss_will_of_emperor.cpp

  HeartOfFear/instance_heart_of_fear.cpp
  HeartOfFear/heart_of_fear.cpp
  HeartOfFear/heart_of_fear.h
  HeartOfFear/boss_zorlok.cpp
  HeartOfFear/boss_tayak.cpp
  HeartOfFear/boss_garalon.cpp
  HeartOfFear/boss_meljarak.cpp
  HeartOfFear/boss_unsok.cpp
  HeartOfFear/boss_shekzeer.cpp

  TerraceOfEndlessSpring/instance_terrace_of_endless_spring.cpp
  TerraceOfEndlessSpring/terrace_of_endless_spring.cpp
  TerraceOfEndlessSpring/terrace_of_endless_spring.h
  TerraceOfEndlessSpring/boss_protectors_of_the_endless.cpp
  TerraceOfEndlessSpring/boss_tsulong.cpp
  TerraceOfEndlessSpring/boss_lei_shi.cpp
  TerraceOfEndlessSpring/boss_sha_of_fear.cpp

  SiegeOfOrgrimmar/instance_siege_of_orgrimmar.cpp
  SiegeOfOrgrimmar/siege_of_orgrimmar.cpp
  SiegeOfOrgrimmar/siege_of_orgrimmar.h
  SiegeOfOrgrimmar/boss_immerseus.cpp
  SiegeOfOrgrimmar/boss_fallen_protectors.cpp
  SiegeOfOrgrimmar/boss_norushen.cpp
  SiegeOfOrgrimmar/boss_sha_of_pride.cpp
  SiegeOfOrgrimmar/boss_galakras.cpp
  SiegeOfOrgrimmar/boss_iron_juggernaut.cpp
  SiegeOfOrgrimmar/boss_korkron_dark_shamans.cpp
  SiegeOfOrgrimmar/boss_general_nazgrim.cpp
  SiegeOfOrgrimmar/boss_malkorok.cpp
  SiegeOfOrgrimmar/boss_spoils_of_pandaria.cpp
  SiegeOfOrgrimmar/boss_thok_the_bloodthirsty.cpp
  SiegeOfOrgrimmar/boss_siegecrafter_blackfuse.cpp
  SiegeOfOrgrimmar/boss_paragons_of_the_klaxxi.cpp
  SiegeOfOrgrimmar/boss_garrosh_hellscream.cpp

  WanderingIsland/WanderingIsland_North.cpp
  WanderingIsland/WanderingIsland_East.cpp
  WanderingIsland/WanderingIsland_West.cpp
  WanderingIsland/WanderingIsland_South.cpp

  ThroneOfThunder/instance_throne_of_thunder.cpp
  ThroneOfThunder/throne_of_thunder.h
  ThroneOfThunder/throne_of_thunder.hpp
  ThroneOfThunder/throne_of_thunder.cpp
  ThroneOfThunder/boss_jin_rokh_the_breaker.cpp
  ThroneOfThunder/boss_council_of_elders.cpp
  ThroneOfThunder/boss_horridon.cpp
  ThroneOfThunder/boss_tortos.cpp
  ThroneOfThunder/boss_megaera.cpp
  ThroneOfThunder/boss_ji_kun.cpp
  ThroneOfThunder/boss_primordius.cpp
  ThroneOfThunder/boss_durumu.cpp
  ThroneOfThunder/boss_dark_animus.cpp
  ThroneOfThunder/boss_iron_qon.cpp
  ThroneOfThunder/boss_twin_consorts.cpp
  ThroneOfThunder/boss_lei_shen.cpp
  ThroneOfThunder/boss_ra_den.cpp

  townlong_steppes.cpp
  townlong_steppes.h
  dread_wastes.cpp
  dread_wastes.h
  valley_of_the_four_winds.cpp
  valley_of_the_four_winds.h
  krasarang_wilds.cpp
  krasarang_wilds.h
  kun_lai_summit.h
  kun_lai_summit.cpp
  jade_forest.cpp
  jade_forest.h
  vale_of_eternal_blossoms.cpp
  vale_of_eternal_blossoms.h
  timeless_isle.cpp
  timeless_isle.h
  the_veiled_stair.cpp
  the_veiled_stair.h
  isle_of_thunder.cpp
  isle_of_thunder.h
)

MACRO( auto_group ProjectDirectory ProjectSources )
    SET( DirectorySources "${ProjectSources}" )
    FOREACH( Source ${DirectorySources} )
        if ("${Source}" MATCHES "(/)+")
            STRING( REGEX REPLACE "${ProjectDirectory}" "" RelativePath "${Source}" )
            STRING( REGEX REPLACE "[\\\\/][^\\\\/]*$" "" RelativePath "${RelativePath}" )
            STRING( REGEX REPLACE "^[\\\\/]" "" RelativePath "${RelativePath}" )
            STRING( REGEX REPLACE "/" "\\\\\\\\" RelativePath "${RelativePath}" )
            SOURCE_GROUP( "${RelativePath}" FILES ${Source} )
        else()
            SOURCE_GROUP( "" FILES ${Source} )
        endif()
    ENDFOREACH( Source )
ENDMACRO( auto_group )

auto_group("${CMAKE_CURRENT_SOURCE_DIR}" "${scriptsPD_STAT_SRCS}")

set(scriptsPD_STAT_SRCS
  ${scriptsPD_STAT_SRCS}
  )

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/dep/g3dlite/include
  ${CMAKE_SOURCE_DIR}/dep/SFMT
  ${CMAKE_SOURCE_DIR}/dep/zlib
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Detour/Include
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Recast/Include
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Detour/
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Recast/
  ${CMAKE_SOURCE_DIR}/src/server/shared
  ${CMAKE_SOURCE_DIR}/src/server/shared/Configuration
  ${CMAKE_SOURCE_DIR}/src/server/shared/Cryptography
  ${CMAKE_SOURCE_DIR}/src/server/shared/Database
  ${CMAKE_SOURCE_DIR}/src/server/shared/DataStores
  ${CMAKE_SOURCE_DIR}/src/server/shared/Debugging
  ${CMAKE_SOURCE_DIR}/src/server/shared/Dynamic/LinkedReference
  ${CMAKE_SOURCE_DIR}/src/server/shared/Dynamic
  ${CMAKE_SOURCE_DIR}/src/server/shared/Logging
  ${CMAKE_SOURCE_DIR}/src/server/shared/Packets
  ${CMAKE_SOURCE_DIR}/src/server/shared/Threading
  ${CMAKE_SOURCE_DIR}/src/server/shared/Utilities
  ${CMAKE_SOURCE_DIR}/src/server/collision
  ${CMAKE_SOURCE_DIR}/src/server/collision/Management
  ${CMAKE_SOURCE_DIR}/src/server/collision/Models
  ${CMAKE_SOURCE_DIR}/src/server/collision/Maps
  ${CMAKE_SOURCE_DIR}/src/server/shared
  ${CMAKE_SOURCE_DIR}/src/server/shared/Database
  ${CMAKE_SOURCE_DIR}/src/server/game/Garrison
  ${CMAKE_SOURCE_DIR}/src/server/game/Accounts
  ${CMAKE_SOURCE_DIR}/src/server/game/Anticheat
  ${CMAKE_SOURCE_DIR}/src/server/game/Achievements
  ${CMAKE_SOURCE_DIR}/src/server/game/Cinematic
  ${CMAKE_SOURCE_DIR}/src/server/game/AI
  ${CMAKE_SOURCE_DIR}/src/server/game/AI/CoreAI
  ${CMAKE_SOURCE_DIR}/src/server/game/AI/ScriptedAI
  ${CMAKE_SOURCE_DIR}/src/server/game/AI/SmartScripts
  ${CMAKE_SOURCE_DIR}/src/server/game/AuctionHouse
  ${CMAKE_SOURCE_DIR}/src/server/game/Battlefield
  ${CMAKE_SOURCE_DIR}/src/server/game/Battlefield/Zones
  ${CMAKE_SOURCE_DIR}/src/server/game/Battlegrounds
  ${CMAKE_SOURCE_DIR}/src/server/game/Battlegrounds/Zones
  ${CMAKE_SOURCE_DIR}/src/server/game/BattlePet
  ${CMAKE_SOURCE_DIR}/src/server/game/BattlePay
  ${CMAKE_SOURCE_DIR}/src/server/game/Calendar
  ${CMAKE_SOURCE_DIR}/src/server/game/Chat
  ${CMAKE_SOURCE_DIR}/src/server/game/Chat/Channels
  ${CMAKE_SOURCE_DIR}/src/server/game/Conditions
  ${CMAKE_SOURCE_DIR}/src/server/shared/Configuration
  ${CMAKE_SOURCE_DIR}/src/server/game/Combat
  ${CMAKE_SOURCE_DIR}/src/server/game/DataStores
  ${CMAKE_SOURCE_DIR}/src/server/game/DungeonFinding
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/AreaTrigger
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Corpse
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Creature
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/DynamicObject
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Item
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Item/Artifact
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Item/Container
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/GameObject
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Object
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Object/Updates
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Pet
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Player
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Transport
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Unit
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Vehicle
  ${CMAKE_SOURCE_DIR}/src/server/game/Events
  ${CMAKE_SOURCE_DIR}/src/server/game/Globals
  ${CMAKE_SOURCE_DIR}/src/server/game/Grids
  ${CMAKE_SOURCE_DIR}/src/server/game/Grids/Cells
  ${CMAKE_SOURCE_DIR}/src/server/game/Grids/Notifiers
  ${CMAKE_SOURCE_DIR}/src/server/game/Groups
  ${CMAKE_SOURCE_DIR}/src/server/game/Guilds
  ${CMAKE_SOURCE_DIR}/src/server/game/Handlers
  ${CMAKE_SOURCE_DIR}/src/server/game/Instances
  ${CMAKE_SOURCE_DIR}/src/server/game/LookingForGroup
  ${CMAKE_SOURCE_DIR}/src/server/game/Loot
  ${CMAKE_SOURCE_DIR}/src/server/game/Mails
  ${CMAKE_SOURCE_DIR}/src/server/game/Miscellaneous
  ${CMAKE_SOURCE_DIR}/src/server/game/Maps
  ${CMAKE_SOURCE_DIR}/src/server/game/Movement
  ${CMAKE_SOURCE_DIR}/src/server/game/Movement/MovementGenerators
  ${CMAKE_SOURCE_DIR}/src/server/game/Movement/Spline
  ${CMAKE_SOURCE_DIR}/src/server/game/Movement/Waypoints
  ${CMAKE_SOURCE_DIR}/src/server/game/Opcodes
  ${CMAKE_SOURCE_DIR}/src/server/game/OutdoorPvP
  ${CMAKE_SOURCE_DIR}/src/server/game/OutdoorPvE
  ${CMAKE_SOURCE_DIR}/src/server/game/Pools
  ${CMAKE_SOURCE_DIR}/src/server/game/PrecompiledHeaders
  ${CMAKE_SOURCE_DIR}/src/server/game/PetBattle
  ${CMAKE_SOURCE_DIR}/src/server/game/Quests
  ${CMAKE_SOURCE_DIR}/src/server/game/Reputation
  ${CMAKE_SOURCE_DIR}/src/server/game/RecruitAFriend
  ${CMAKE_SOURCE_DIR}/src/server/game/Scripting
  ${CMAKE_SOURCE_DIR}/src/server/game/Server
  ${CMAKE_SOURCE_DIR}/src/server/game/Server/Protocol
  ${CMAKE_SOURCE_DIR}/src/server/game/Skills
  ${CMAKE_SOURCE_DIR}/src/server/game/Spells
  ${CMAKE_SOURCE_DIR}/src/server/game/Spells/Auras
  ${CMAKE_SOURCE_DIR}/src/server/game/Spells/SpellLog
  ${CMAKE_SOURCE_DIR}/src/server/game/Texts
  ${CMAKE_SOURCE_DIR}/src/server/game/Tickets
  ${CMAKE_SOURCE_DIR}/src/server/game/Tools
  ${CMAKE_SOURCE_DIR}/src/server/game/Vignette
  ${CMAKE_SOURCE_DIR}/src/server/game/Warden
  ${CMAKE_SOURCE_DIR}/src/server/game/Warden/Modules
  ${CMAKE_SOURCE_DIR}/src/server/game/Weather
  ${CMAKE_SOURCE_DIR}/src/server/game/World
  ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders
  ${ACE_INCLUDE_DIR}
  ${MYSQL_INCLUDE_DIR}
  ${CURL_INCLUDE_DIR}
  ${EASYJSON_INCLUDE_DIR}
  ${MSFRAMEWORK_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/dep/process
)

add_library(scriptsPandaria STATIC ${scriptsPD_STAT_SRCS} ${scriptsPD_STAT_PCH_SRC})
add_dependencies(scriptsPandaria revision_data.h)

set_property(TARGET scriptsPandaria PROPERTY FOLDER "scripts")

if (USE_SCRIPTPCH)
  add_cxx_pch(scriptsPandaria ${scriptsPD_STAT_PCH_HDR} ${scriptsPD_STAT_PCH_SRC})
endif()

message("  -> Prepared: Pandaria")