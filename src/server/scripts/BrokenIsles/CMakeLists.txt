#
#  MILLENIUM-STUDIO
#  Copyright 2016 Millenium-studio SARL
#  All Rights Reserved.
#
if (USE_SCRIPTPCH)
  set(scriptsBI_STAT_PCH_HDR PrecompiledHeaders/BrokenIslesPCH.h)
  set(scriptsBI_STAT_PCH_SRC PrecompiledHeaders/BrokenIslesPCH.cpp)
endif ()

file(GLOB_RECURSE sources_ScenarioHavocArtifactFelsoulHold HavocArtifactFelsoulHoldScenario/*.hpp HavocArtifactFelsoulHoldScenario/*.cpp HavocArtifactFelsoulHoldScenario/*.hxx)
file(GLOB_RECURSE sources_EyeOfAzshara EyeOfAzshara/*.hpp EyeOfAzshara/*.cpp EyeOfAzshara/*.hxx)
file(GLOB_RECURSE sources_DarkHeartThicket DarkHeartThicket/*.hpp DarkHeartThicket/*.cpp DarkHeartThicket/*.hxx)
file(GLOB_RECURSE sources_VioletHold VioletHold/*.hpp VioletHold/*.cpp VioletHold/*.hxx)
file(GLOB_RECURSE sources_VaultOfTheWardens VaultOfTheWardens/*.hpp VaultOfTheWardens/*.cpp VaultOfTheWardens/*.hxx)
file(GLOB_RECURSE sources_NeltharionsLair NeltharionsLair/*.hpp NeltharionsLair/*.cpp NeltharionsLair/*.hxx)
file(GLOB_RECURSE sources_MawOfSouls MawOfSouls/*.hpp MawOfSouls/*.cpp MawOfSouls/*.hxx)
file(GLOB_RECURSE sources_HallsOfValor HallsOfValor/*.hpp HallsOfValor/*.cpp HallsOfValor/*.hxx)
file(GLOB_RECURSE sources_TrialOfValor TrialOfValor/*.hpp TrialOfValor/*.cpp TrialOfValor/*.hxx)
file(GLOB_RECURSE sources_BlackRookHold BlackRookHold/*.hpp BlackRookHold/*.cpp BlackRookHold/*.hxx)
file(GLOB_RECURSE sources_ClassHall ClassHall/*.hpp ClassHall/*.cpp ClassHall/*.hxx)
file(GLOB_RECURSE sources_EmeraldNightmare TheEmeraldNightmare/*.hpp TheEmeraldNightmare/*.cpp TheEmeraldNightmare/*.hxx)
file(GLOB_RECURSE sources_Arcway TheArcway/*.hpp TheArcway/*.cpp TheArcway/*.hxx)
file(GLOB_RECURSE sources_CourtOfStars CourtOfStars/*.hpp CourtOfStars/*.cpp CourtOfStars/*.hxx)
file(GLOB_RECURSE sources_ReturnToKarazhan ReturnToKarazhan/*.hpp ReturnToKarazhan/*.cpp ReturnToKarazhan/*.hxx)
file(GLOB_RECURSE sources_TheNighthold TheNighthold/*.hpp TheNighthold/*.cpp TheNighthold/*hxx)
file(GLOB_RECURSE sources_BrokenShore BrokenShore/*.hpp BrokenShore/*.cpp BrokenShore/*hxx)
file(GLOB_RECURSE sources_CathedralOfEternalNight CathedralOfEternalNight/*.hpp CathedralOfEternalNight/*.cpp CathedralOfEternalNight/*hxx)
file(GLOB_RECURSE sources_TombOfSargeras TombOfSargeras/*.hpp TombOfSargeras/*.h TombOfSargeras/*.cpp TombOfSargeras/*.hxx)
file(GLOB_RECURSE sources_SeatOfTriumvirate SeatOfTriumvirate/*.hpp SeatOfTriumvirate/*.h SeatOfTriumvirate/*.cpp SeatOfTriumvirate/*.hxx)
file(GLOB_RECURSE sources_AntorusTheBurningThrone AntorusTheBurningThrone/*.hpp AntorusTheBurningThrone/*.h AntorusTheBurningThrone/*.cpp AntorusTheBurningThrone/*.hxx)
file(GLOB_RECURSE sources_ArtifactQuestlines ArtifactQuestlines/*.cpp)

set(scriptsBI_STAT_SRCS
  ${scriptsBI_STAT_SRCS}
  ../../game/AI/ScriptedAI/ScriptedEscortAI.cpp
  ../../game/AI/ScriptedAI/ScriptedCreature.cpp
  ../../game/AI/ScriptedAI/ScriptedFollowerAI.cpp
  ${sources_ScenarioHavocArtifactFelsoulHold}
  ${sources_EyeOfAzshara}
  ${sources_DarkHeartThicket}
  ${sources_VioletHold}
  ${sources_VaultOfTheWardens}
  ${sources_NeltharionsLair}
  ${sources_MawOfSouls}
  ${sources_HallsOfValor}
  ${sources_TrialOfValor}
  ${sources_BlackRookHold}
  ${sources_ClassHall}
  ${sources_EmeraldNightmare}
  ${sources_Arcway}
  ${sources_CourtOfStars}
  ${sources_ReturnToKarazhan}
  ${sources_TheNighthold}
  ${sources_BrokenShore}
  ${sources_CathedralOfEternalNight}
  ${sources_TombOfSargeras}
  ${sources_AntorusTheBurningThrone}
  ${sources_SeatOfTriumvirate}
  ${sources_ArtifactQuestlines}
  zone_dalaran_legion.cpp
  zone_dalaran_legion.h
  zone_valsharah.cpp
  zone_valsharah.h
  zone_mardum.h
  zone_mardum.cpp
  zone_highmountain.h
  zone_highmountain.cpp
  zone_stormheim.cpp
  legion_worldbosses.cpp
  legion_worldbosses.hpp
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

auto_group("${CMAKE_CURRENT_SOURCE_DIR}" "${scriptsBI_STAT_SRCS}")

set(scriptsBI_STAT_SRCS
  ${scriptsBI_STAT_SRCS}
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
  ${CMAKE_SOURCE_DIR}/src/server/game/Challenge
  ${CMAKE_SOURCE_DIR}/src/server/game/Chat
  ${CMAKE_SOURCE_DIR}/src/server/game/Chat/Channels
  ${CMAKE_SOURCE_DIR}/src/server/game/Conditions
  ${CMAKE_SOURCE_DIR}/src/server/game/Contribution
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
  ${CMAKE_SOURCE_DIR}/src/server/game/POI
  ${CMAKE_SOURCE_DIR}/src/server/game/Quests
  ${CMAKE_SOURCE_DIR}/src/server/game/RecruitAFriend
  ${CMAKE_SOURCE_DIR}/src/server/game/Reputation
  ${CMAKE_SOURCE_DIR}/src/server/game/Scenarios
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

add_library(scriptsBrokenIsles STATIC ${scriptsBI_STAT_SRCS} ${scriptsBI_STAT_PCH_SRC})
add_dependencies(scriptsBrokenIsles revision_data.h)

set_property(TARGET scriptsBrokenIsles PROPERTY FOLDER "scripts")

if (USE_SCRIPTPCH)
  add_cxx_pch(scriptsBrokenIsles ${scriptsBI_STAT_PCH_HDR} ${scriptsBI_STAT_PCH_SRC})
endif()

message("  -> Prepared: BrokenIsles")