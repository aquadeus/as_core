////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef VMAPEXPORT_H
#define VMAPEXPORT_H

#include <string>

enum ModelFlags
{
    MOD_M2 = 1,
    MOD_WORLDSPAWN = 1<<1,
    MOD_HAS_BOUND = 1 << 2,
    MOD_PARENT_SPAWN = 1 << 3
};

extern const char * szWorkDirWmo;
extern const char * szRawVMAPMagic;                         // vmap magic string for extracted raw vmap data

bool FileExists(const char * file);
void strToLower(char* str);

bool ExtractSingleWmo(std::string& fname);
bool ExtractSingleModel(std::string& fname);

void ExtractGameobjectModels();

#endif
