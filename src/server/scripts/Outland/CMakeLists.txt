#
#  MILLENIUM-STUDIO
#  Copyright 2016 Millenium-studio SARL
#  All Rights Reserved.
#

if (USE_SCRIPTPCH)
  set(scriptsOT_STAT_PCH_HDR PrecompiledHeaders/OutlandPCH.h)
  set(scriptsOT_STAT_PCH_SRC PrecompiledHeaders/OutlandPCH.cpp)
endif ()

set(scriptsOT_STAT_SRCS
  ${scriptsOT_STAT_SRCS}
  ../../game/AI/ScriptedAI/ScriptedEscortAI.cpp
  ../../game/AI/ScriptedAI/ScriptedCreature.cpp
  ../../game/AI/ScriptedAI/ScriptedFollowerAI.cpp
  nagrand.cpp
  HellfireCitadel/MagtheridonsLair/magtheridons_lair.h
  HellfireCitadel/MagtheridonsLair/instance_magtheridons_lair.cpp
  HellfireCitadel/MagtheridonsLair/boss_magtheridon.cpp
  HellfireCitadel/HellfireRamparts/instance_hellfire_ramparts.cpp
  HellfireCitadel/HellfireRamparts/boss_omor_the_unscarred.cpp
  HellfireCitadel/HellfireRamparts/boss_watchkeeper_gargolmar.cpp
  HellfireCitadel/HellfireRamparts/boss_vazruden_the_herald.cpp
  HellfireCitadel/HellfireRamparts/hellfire_ramparts.h
  HellfireCitadel/BloodFurnace/boss_the_maker.cpp
  HellfireCitadel/BloodFurnace/boss_kelidan_the_breaker.cpp
  HellfireCitadel/BloodFurnace/blood_furnace.h
  HellfireCitadel/BloodFurnace/instance_blood_furnace.cpp
  HellfireCitadel/BloodFurnace/boss_broggok.cpp
  HellfireCitadel/ShatteredHalls/shattered_halls.h
  HellfireCitadel/ShatteredHalls/boss_warchief_kargath_bladefist.cpp
  HellfireCitadel/ShatteredHalls/boss_nethekurse.cpp
  HellfireCitadel/ShatteredHalls/instance_shattered_halls.cpp
  HellfireCitadel/ShatteredHalls/boss_warbringer_omrogg.cpp
  CoilfangReservoir/SteamVault/boss_mekgineer_steamrigger.cpp
  CoilfangReservoir/SteamVault/instance_steam_vault.cpp
  CoilfangReservoir/SteamVault/boss_hydromancer_thespia.cpp
  CoilfangReservoir/SteamVault/boss_warlord_kalithresh.cpp
  CoilfangReservoir/SteamVault/steam_vault.h
  CoilfangReservoir/SerpentShrine/boss_hydross_the_unstable.cpp
  CoilfangReservoir/SerpentShrine/boss_fathomlord_karathress.cpp
  CoilfangReservoir/SerpentShrine/instance_serpent_shrine.cpp
  CoilfangReservoir/SerpentShrine/serpent_shrine.h
  CoilfangReservoir/SerpentShrine/boss_lady_vashj.cpp
  CoilfangReservoir/SerpentShrine/boss_leotheras_the_blind.cpp
  CoilfangReservoir/SerpentShrine/boss_lurker_below.cpp
  CoilfangReservoir/SerpentShrine/boss_morogrim_tidewalker.cpp
  CoilfangReservoir/underbog/boss_hungarfen.cpp
  CoilfangReservoir/underbog/boss_the_black_stalker.cpp
  shattrath_city.cpp
  TempestKeep/Mechanar/boss_mechano_lord_capacitus.cpp
  TempestKeep/Mechanar/boss_pathaleon_the_calculator.cpp
  TempestKeep/Mechanar/boss_nethermancer_sepethrea.cpp
  TempestKeep/Mechanar/mechanar.h
  TempestKeep/Mechanar/boss_gatewatcher_gyrokill.cpp
  TempestKeep/Mechanar/instance_mechanar.cpp
  TempestKeep/Mechanar/mechanar.cpp
  TempestKeep/Mechanar/boss_gatewatcher_ironhand.cpp
  TempestKeep/Eye/the_eye.h
  TempestKeep/Eye/instance_the_eye.cpp
  TempestKeep/Eye/boss_void_reaver.cpp
  TempestKeep/Eye/boss_astromancer.cpp
  TempestKeep/Eye/boss_alar.cpp
  TempestKeep/Eye/boss_kaelthas.cpp
  TempestKeep/Eye/the_eye.cpp
  TempestKeep/botanica/boss_high_botanist_freywinn.cpp
  TempestKeep/botanica/boss_warp_splinter.cpp
  TempestKeep/botanica/boss_laj.cpp
  TempestKeep/arcatraz/boss_harbinger_skyriss.cpp
  TempestKeep/arcatraz/boss_dalliah_the_doomsayer.cpp
  TempestKeep/arcatraz/boss_wrath_scryer_soccothrates.cpp
  TempestKeep/arcatraz/instance_arcatraz.cpp
  TempestKeep/arcatraz/arcatraz.h
  TempestKeep/arcatraz/arcatraz.cpp
  Auchindoun/AuchenaiCrypts/boss_shirrak_the_dead_watcher.cpp
  Auchindoun/AuchenaiCrypts/boss_exarch_maladaar.cpp
  Auchindoun/ManaTombs/boss_pandemonius.cpp
  Auchindoun/ManaTombs/boss_nexusprince_shaffar.cpp
  Auchindoun/SethekkHalls/boss_tailonking_ikiss.cpp
  Auchindoun/SethekkHalls/boss_darkweaver_syth.cpp
  Auchindoun/SethekkHalls/instance_sethekk_halls.cpp
  Auchindoun/SethekkHalls/sethekk_halls.h
  Auchindoun/ShadowLabyrinth/boss_murmur.cpp
  Auchindoun/ShadowLabyrinth/boss_ambassador_hellmaw.cpp
  Auchindoun/ShadowLabyrinth/boss_blackheart_the_inciter.cpp
  Auchindoun/ShadowLabyrinth/instance_shadow_labyrinth.cpp
  Auchindoun/ShadowLabyrinth/shadow_labyrinth.h
  Auchindoun/ShadowLabyrinth/boss_grandmaster_vorpil.cpp
  boss_doomwalker.cpp
  terokkar_forest.cpp
  hellfire_peninsula.cpp
  boss_doomlord_kazzak.cpp
  BlackTemple/boss_teron_gorefiend.cpp
  BlackTemple/black_temple.h
  BlackTemple/illidari_council.cpp
  BlackTemple/boss_shade_of_akama.cpp
  BlackTemple/boss_supremus.cpp
  BlackTemple/black_temple.cpp
  BlackTemple/boss_mother_shahraz.cpp
  BlackTemple/instance_black_temple.cpp
  BlackTemple/boss_reliquary_of_souls.cpp
  BlackTemple/boss_warlord_najentus.cpp
  BlackTemple/boss_bloodboil.cpp
  BlackTemple/boss_illidan.cpp
  shadowmoon_valley.cpp
  blades_edge_mountains.cpp
  GruulsLair/boss_high_king_maulgar.cpp
  GruulsLair/boss_gruul.cpp
  GruulsLair/gruuls_lair.h
  GruulsLair/instance_gruuls_lair.cpp
  netherstorm.cpp
  zangarmarsh.cpp
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

auto_group("${CMAKE_CURRENT_SOURCE_DIR}" "${scriptsOT_STAT_SRCS}")

set(scriptsOT_STAT_SRCS
  ${scriptsOT_STAT_SRCS}
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

add_library(scriptsOutland STATIC ${scriptsOT_STAT_SRCS} ${scriptsOT_STAT_PCH_SRC})
add_dependencies(scriptsOutland revision_data.h)

set_property(TARGET scriptsOutland PROPERTY FOLDER "scripts")

if (USE_SCRIPTPCH)
  add_cxx_pch(scriptsOutland ${scriptsOT_STAT_PCH_HDR} ${scriptsOT_STAT_PCH_SRC})
endif()

message("  -> Prepared: Outland")