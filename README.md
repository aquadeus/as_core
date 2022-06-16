## Overview
World of Warcraft Legion (7.X)

[![Build Status](https://drone.fat.sh/api/badges/MilleniumStudio/legion/status.svg)](https://drone.fat.sh/MilleniumStudio/legion)
## Requirements

+ Platform: Linux, Windows or Mac
+ Git ≥ 2.18+
+ ACE ≥ 6.2.0
+ MySQL ≥ 5.5.0+ max 5.7
+ CMake ≥ 3.12+
+ OpenSSL ≥ 1.0.0+
+ Boost ≥ 1.63.0 max
+ MS Visual Studio ≥ 2017 15.6.4+
+ Xcode 10 (Mac only)

## Install

Detailed installation guides are available in the wiki for
[Windows](http://collab.kpsn.org/display/tc/Win),
[Linux](http://collab.kpsn.org/display/tc/Linux) and
[Mac OSX](http://collab.kpsn.org/display/tc/Mac).

In order to complete the installation of the core, you need some other dependencies, they are:
- LibCurl, under linux its the package libcurl4-openssl-dev, under windows you may have to compile it your self,
- EasyJson headers, for that you only need to execute in your git repo : 

```
git submodule init && git submodule update --remote
```

- If you have the message `Git was NOT FOUND on your system` please uninstall and update your git version (you can also change the path for the binary on yout PATH)
 Windows user you need the mysql server installed, download the last version (5.7+) here : [MySQL Community Server](http://dev.mysql.com/downloads/mysql/) if you need the x86 build just change the include path on cmake
### Commit naming convention
- When fixing bug, by all means include the issue number.
- When completing feature if you have some task identifier, include that.
- If the identifier included goes with subject, it often makes sense to just reuse it. (**Use Core/XXX: "Commit name" format**)
- If the project is large, mention which component is affected.
- **Describe the change in functionality** , **never** the code (that can be seen from the diff).

### Scripts naming convention
- When adding a script, add a prefix related to the dungeon, raid or boss :

```
npc_skorpyron_arcane_tether
spell_skorpyron_arcane_tether
npc_nighthold_talysra_helper
spell_nighthold_talysra_helper
```

### Database data convention
- Most of data come from retail sniff, the field `VerifiedBuild` must be used in this case

```
"-1" mean data don't come from parse or are outdated
"-BuildNumber" mean they come from sniff and are changed
"BuildNumber" they come mean from sniff
``` 

### Hotfix Database data convention
- All data come from retail sniff, you must set the field `VerifiedBuild` to `-1`
- You must add a comment in  `hotfix` and specify `CUSTOM - YourChange`

if you don't follow rules your hotfix can be deleted on next update

### Branch naming convention
- When you need a new branch the name must be base branch and explicit name like 
```BaseBranch_MyAwesomeFeature```

### How to Cppcheck
run ```cppcheck --enable=all --xml-version=2 > PathToFile/cppcheck.xml --inconclusive path PathToFile/legion/src/ --verbose```

and run ```./cppcheck-htmlreport --file=PathToFile/cppcheck.xml --source-dir=PathToFile/legion/src --report-dir=PathToFile/legion/cppcheck/``` to get the html out
