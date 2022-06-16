*How to update legion_spell:*

#1. Pull legion, and cherry-pick every fix from legion_spell. Use the "merges" branch as base: merge dev, dev_tos, and dev_cos in it.
#2. Clone legion_spell / pull
#3. Open 2 folders: the one from legion on the branch u wish to base the update from (for example, Legion_730) and the one from legion_spell
#4. Open legion folder in /src/server, open command in it, and type those 2 commands consecutively: 	
												del /s *.cpp
												del /s *.txt 	and then copy server folder into legion_spell /src (this will update all headers)
												
#5. Git bash in your legion folder (for example legion_730) and do: git reset --hard (to get everything back)
#6. Cut (not copy) every cpp from legion folder, and paste them in the corresponding folder of legion_spell:
	src/server/scripts/spells		All the CPPs
	src/server/game/entities:		Areatrigger.cpp 	in /Areatrigger
									Creature.cpp		in /Creature
									Item.cpp			in /Item
									ArtifactMgr.cpp 	in /Item/Artifact
									Object.cpp 			in /Object
									Pet.cpp 			in /Pet
									Player.cpp 			in /Player
									KillRewarder.cpp	in /Player
									Totem.cpp 			in /Totem
									Unit.cpp			in /Unit
									StatSystem.cpp		in /Unit

	src/server/game/Handlers:		SpellHandler.cpp AND TransmogrificationHandler.cpp AND NPCHandler.cpp

	src/server/game/Instance:						THE WHOLE FOLDER'S CPPs !!!
	src/server/game/Scripting:						THE WHOLE FOLDER'S CPPs !!!
	src/server/game/spells 							THE WHOLE FOLDER's CPPs !!!
	src/server/game/AI 								THE WHOLE FOLDER's CPPs EXCEPT ScriptedCreature, ScriptedEscordAI and ScriptedFollowerAI in /ScriptedAI (those 3 are included everywhere, it's a mess)

	src/server/scripts/Invasions 					THE WHOLE FOLDER's CPPs !!!
	src/server/scripts/Argus						THE WHOLE FOLDER's CPPs !!!
	src/server/scripts/BrokenIsles/ToS				The CPPs they have already
	src/server/scripts/BrokenIsles/CourtOfStars		THE WHOLE FOLDER's CPPs !!! NB: in court_of_stars.hpp, replace include BrokenIslesPCH.h by all the includes of it: https://gyazo.com/fdb7b8e59fe53e8db2b5c380684f46d0


#7. Open CMakeLists.txt in legion/src/server/scripts and comment the lines "include(Spells/CMakeLists.txt)", "include(Invasions/CMakeLists.txt)" AND "include(Argus/CMakeLists.txt)" by putting a # in front of them, then save the change.
#8. With CMake, Configure, and then Generate the solution of your legion folder (for example, Legion_730).
#9. On Visual Studio build the solution
#10. Update their SQL (only the folder they have) while it's building to save time hehe

#11. Copy the following .lib files from the legion folder to a desktop folder called "libraries":
	build/src/server/collision/RelWithDebInfo					collision.lib
	build/src/server/game/RelWithDebInfo						game.lib
	build/src/server/scripts/BrokenIsles/RelWithDebInfo			scriptsBrokenIsles.lib
	build/src/server/scripts/Draenor/RelWithDebInfo				scriptsDraenor.lib
	build/src/server/scripts/EasternKingdoms/RelWithDebInfo		scriptsEasternKingdoms.lib
	build/src/server/scripts/Kalimdor/RelWithDebInfo			scriptsKalimdor.lib
	build/src/server/scripts/Northrend/RelWithDebInfo			scriptsNothrend.lib
	build/src/server/scripts/Outland/RelWithDebInfo				scriptsOutland.lib
	build/src/server/scripts/Pandaria/RelWithDebInfo			scriptsPandaria.lib
	build/src/server/scripts/RelWithDebInfo						scripts.lib
	build/src/server/shared/RelWithDebInfo						shared.lib
	
#12. Once you have all 11 lib files in the folder, select them all, and add them to archive to make a libs.rar file. Replace the one found in legion_spell/dep/libs folder by it
#13. Extract the libs.rar on the legion_spell folder, use CMake to configure and generate the legion_spell solution, and then compile it (relwithdebuginfo) to see if it works.
#14. If it does work, delete the 11 .libs (just keep the libs.rar file), open GUI on legion_spell repo, commit everything with comment 'update to latest sources' and push. *DO THIS ONLY IF NOTHING HAS BEEN COMMITED ON LEGION_SPELL SINCE YOU STARTED*
#15. In case something has been pushed since, commit the update, then manually re-write what was commited since, and use git push origin $branchname --force *DO THIS ONLY IF YOU ARE ABSOLUTELY SURE OF WHAT YOU ARE DOING*