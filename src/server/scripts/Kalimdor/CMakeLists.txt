#
#  MILLENIUM-STUDIO
#  Copyright 2016 Millenium-studio SARL
#  All Rights Reserved.
#
if (USE_SCRIPTPCH)
  set(scriptsKM_STAT_PCH_HDR PrecompiledHeaders/KalimdorPCH.h)
  set(scriptsKM_STAT_PCH_SRC PrecompiledHeaders/KalimdorPCH.cpp)
endif ()

file(GLOB_RECURSE sources_Firelands Firelands/*.hpp Firelands/*.h Firelands/*.cpp Firelands/*.hxx)
file(GLOB_RECURSE sources_DragonSoul CavernsOfTime/DragonSoul/*.hpp CavernsOfTime/DragonSoul/*.h CavernsOfTime/DragonSoul/*.cpp CavernsOfTime/DragonSoul/*.hxx)

set(scriptsKM_STAT_SRCS
  ${scriptsKM_STAT_SRCS}
  ../../game/AI/ScriptedAI/ScriptedEscortAI.cpp
  ../../game/AI/ScriptedAI/ScriptedCreature.cpp
  ../../game/AI/ScriptedAI/ScriptedFollowerAI.cpp
  stonetalon_mountains.cpp
  silithus.cpp
  moonglade.cpp
  RazorfenDowns/razorfen_downs.cpp
  RazorfenDowns/instance_razorfen_downs.cpp
  RazorfenDowns/boss_amnennar_the_coldbringer.cpp
  RazorfenDowns/razorfen_downs.h
  ZulFarrak/zulfarrak.h
  ZulFarrak/zulfarrak.cpp
  ZulFarrak/instance_zulfarrak.cpp
  CavernsOfTime/EscapeFromDurnholdeKeep/boss_epoch_hunter.cpp
  CavernsOfTime/EscapeFromDurnholdeKeep/old_hillsbrad.h
  CavernsOfTime/EscapeFromDurnholdeKeep/boss_leutenant_drake.cpp
  CavernsOfTime/EscapeFromDurnholdeKeep/old_hillsbrad.cpp
  CavernsOfTime/EscapeFromDurnholdeKeep/instance_old_hillsbrad.cpp
  CavernsOfTime/EscapeFromDurnholdeKeep/boss_captain_skarloc.cpp
  CavernsOfTime/BattleForMountHyjal/boss_archimonde.cpp
  CavernsOfTime/BattleForMountHyjal/hyjalAI.h
  CavernsOfTime/BattleForMountHyjal/hyjal_trash.h
  CavernsOfTime/BattleForMountHyjal/boss_kazrogal.cpp
  CavernsOfTime/BattleForMountHyjal/hyjal_trash.cpp
  CavernsOfTime/BattleForMountHyjal/hyjal.cpp
  CavernsOfTime/BattleForMountHyjal/hyjalAI.cpp
  CavernsOfTime/BattleForMountHyjal/instance_hyjal.cpp
  CavernsOfTime/BattleForMountHyjal/boss_rage_winterchill.cpp
  CavernsOfTime/BattleForMountHyjal/hyjal.h
  CavernsOfTime/BattleForMountHyjal/boss_azgalor.cpp
  CavernsOfTime/BattleForMountHyjal/boss_anetheron.cpp
  CavernsOfTime/CullingOfStratholme/boss_infinite.cpp
  CavernsOfTime/CullingOfStratholme/boss_salramm.cpp
  CavernsOfTime/CullingOfStratholme/boss_meathook.cpp
  CavernsOfTime/CullingOfStratholme/boss_mal_ganis.cpp
  CavernsOfTime/CullingOfStratholme/boss_epoch.cpp
  CavernsOfTime/CullingOfStratholme/culling_of_stratholme.cpp
  CavernsOfTime/CullingOfStratholme/instance_culling_of_stratholme.cpp
  CavernsOfTime/CullingOfStratholme/culling_of_stratholme.h
  CavernsOfTime/DarkPortal/dark_portal.h
  CavernsOfTime/DarkPortal/instance_dark_portal.cpp
  CavernsOfTime/DarkPortal/boss_chrono_lord_deja.cpp
  CavernsOfTime/DarkPortal/dark_portal.cpp
  CavernsOfTime/DarkPortal/boss_aeonus.cpp
  CavernsOfTime/DarkPortal/boss_temporus.cpp
  CavernsOfTime/EndTime/boss_echo_of_tyrande.cpp
  CavernsOfTime/EndTime/boss_echo_of_baine.cpp
  CavernsOfTime/EndTime/boss_echo_of_sylvanas.cpp
  CavernsOfTime/EndTime/boss_echo_of_jaina.cpp
  CavernsOfTime/EndTime/boss_murozond.cpp
  CavernsOfTime/EndTime/end_time_teleport.cpp
  CavernsOfTime/EndTime/instance_end_time.cpp
  CavernsOfTime/EndTime/end_time.cpp
  CavernsOfTime/EndTime/end_time.h
  CavernsOfTime/WellofEternity/boss_perotharn.cpp
  CavernsOfTime/WellofEternity/boss_queen_azshara.cpp
  CavernsOfTime/WellofEternity/boss_mannoroth.cpp
  CavernsOfTime/WellofEternity/instance_well_of_eternity.cpp
  CavernsOfTime/WellofEternity/well_of_eternity_teleport.cpp
  CavernsOfTime/WellofEternity/well_of_eternity.cpp
  CavernsOfTime/WellofEternity/well_of_eternity.h
  CavernsOfTime/HourofTwilight/boss_arcurion.cpp
  CavernsOfTime/HourofTwilight/boss_asira_dawnslayer.cpp
  CavernsOfTime/HourofTwilight/boss_archbishop_benedictus.cpp
  CavernsOfTime/HourofTwilight/instance_hour_of_twilight.cpp
  CavernsOfTime/HourofTwilight/hour_of_twilight.cpp
  CavernsOfTime/HourofTwilight/hour_of_twilight.h
  BlackfathomDeeps/boss_kelris.cpp
  BlackfathomDeeps/instance_blackfathom_deeps.cpp
  BlackfathomDeeps/boss_gelihast.cpp
  BlackfathomDeeps/blackfathom_deeps.cpp
  BlackfathomDeeps/boss_aku_mai.cpp
  BlackfathomDeeps/blackfathom_deeps.h
  azuremyst_isle.cpp
  orgrimmar.cpp
  desolace.cpp
  feralas.cpp
  Maraudon/boss_princess_theradras.cpp
  Maraudon/boss_landslide.cpp
  Maraudon/boss_celebras_the_cursed.cpp
  Maraudon/boss_noxxion.cpp
  TempleOfAhnQiraj/boss_fankriss.cpp
  TempleOfAhnQiraj/boss_huhuran.cpp
  TempleOfAhnQiraj/instance_temple_of_ahnqiraj.cpp
  TempleOfAhnQiraj/mob_anubisath_sentinel.cpp
  TempleOfAhnQiraj/boss_viscidus.cpp
  TempleOfAhnQiraj/boss_twinemperors.cpp
  TempleOfAhnQiraj/boss_sartura.cpp
  TempleOfAhnQiraj/boss_cthun.cpp
  TempleOfAhnQiraj/temple_of_ahnqiraj.h
  TempleOfAhnQiraj/boss_skeram.cpp
  TempleOfAhnQiraj/boss_ouro.cpp
  TempleOfAhnQiraj/boss_bug_trio.cpp
  darkshore.cpp
  RuinsOfAhnQiraj/boss_buru.cpp
  RuinsOfAhnQiraj/instance_ruins_of_ahnqiraj.cpp
  RuinsOfAhnQiraj/boss_rajaxx.cpp
  RuinsOfAhnQiraj/boss_ossirian.cpp
  RuinsOfAhnQiraj/boss_ayamiss.cpp
  RuinsOfAhnQiraj/boss_moam.cpp
  RuinsOfAhnQiraj/ruins_of_ahnqiraj.h
  RuinsOfAhnQiraj/boss_kurinnaxx.cpp
  mulgore.cpp
  bloodmyst_isle.cpp
  thunder_bluff.cpp
  azshara.cpp
  RazorfenKraul/razorfen_kraul.h
  RazorfenKraul/instance_razorfen_kraul.cpp
  RazorfenKraul/razorfen_kraul.cpp
  the_barrens.cpp
  ungoro_crater.cpp
  WailingCaverns/wailing_caverns.h
  WailingCaverns/instance_wailing_caverns.cpp
  WailingCaverns/wailing_caverns.cpp
  TheVortexPinnacle/boss_grand_vizier_ertan.cpp
  TheVortexPinnacle/boss_altairus.cpp
  TheVortexPinnacle/boss_asaad.cpp
  TheVortexPinnacle/instance_the_vortex_pinnacle.cpp
  TheVortexPinnacle/the_vortex_pinnacle.cpp
  TheVortexPinnacle/the_vortex_pinnacle.h
  LostCityOfTheTolvir/boss_general_husam.cpp
  LostCityOfTheTolvir/boss_lockmaw_augh.cpp
  LostCityOfTheTolvir/boss_high_prophet_barim.cpp
  LostCityOfTheTolvir/boss_siamat.cpp
  LostCityOfTheTolvir/instance_lost_city_of_the_tolvir.cpp
  LostCityOfTheTolvir/lost_city_of_the_tolvir.cpp
  LostCityOfTheTolvir/lost_city_of_the_tolvir.h
  HallsOfOrigination/boss_temple_guardian_anhuur.cpp
  HallsOfOrigination/boss_earthrager_ptah.cpp
  HallsOfOrigination/boss_anraphet.cpp
  HallsOfOrigination/boss_ammunae.cpp
  HallsOfOrigination/boss_setesh.cpp
  HallsOfOrigination/boss_isiset.cpp
  HallsOfOrigination/boss_rajh.cpp
  HallsOfOrigination/instance_halls_of_origination.cpp
  HallsOfOrigination/halls_of_origination.cpp
  HallsOfOrigination/halls_of_origination.h
  durotar.cpp
  felwood.cpp
  tanaris.cpp
  dustwallow_marsh.cpp
  winterspring.cpp
  thousand_needles.cpp
  ashenvale.cpp
  teldrassil.cpp
  OnyxiasLair/boss_onyxia.cpp
  OnyxiasLair/onyxias_lair.h
  OnyxiasLair/instance_onyxias_lair.cpp
  southern_barrens.cpp
  uldum.cpp
  RagefireChasm/instance_ragefire_chasm.cpp
  RagefireChasm/ragefire_chasm.cpp
  RagefireChasm/ragefire_chasm.h
  RagefireChasm/boss_lava_guard_gordoth.cpp
  ThroneOfTheFourWinds/boss_alakir.cpp
  ThroneOfTheFourWinds/boss_conclave_of_wind.cpp
  ThroneOfTheFourWinds/instance_throne_of_the_four_winds.cpp
  ThroneOfTheFourWinds/throne_of_the_four_winds.cpp
  ThroneOfTheFourWinds/throne_of_the_four_winds.h
  ${sources_Firelands}
  ${sources_DragonSoul}
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

auto_group("${CMAKE_CURRENT_SOURCE_DIR}" "${scriptsKM_STAT_SRCS}")

set(scriptsEK_STAT_SRCS
  ${scriptsEK_STAT_SRCS}
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

add_library(scriptsKalimdor STATIC ${scriptsKM_STAT_SRCS} ${scriptsKM_STAT_PCH_SRC})
add_dependencies(scriptsKalimdor revision_data.h)

set_property(TARGET scriptsKalimdor PROPERTY FOLDER "scripts")

if (USE_SCRIPTPCH)
  add_cxx_pch(scriptsKalimdor ${scriptsKM_STAT_PCH_HDR} ${scriptsKM_STAT_PCH_SRC})
endif()

message("  -> Prepared: Kalimdor")