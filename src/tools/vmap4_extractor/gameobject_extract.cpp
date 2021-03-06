/*
 * Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "DB2.h"
#include "model.h"
#include "adtfile.h"
#include "vmapexport.h"

#include "fmt/printf.h"

namespace Trinity
{
    /// Default TC string format function.
    template<typename Format, typename... Args>
    inline std::string StringFormat(Format&& fmt, Args&&... args)
    {
        return fmt::sprintf(std::forward<Format>(fmt), std::forward<Args>(args)...);
    }

    /// Returns true if the given char pointer is null.
    inline bool IsFormatEmptyOrNull(const char* fmt)
    {
        return fmt == nullptr;
    }

    /// Returns true if the given std::string is empty.
    inline bool IsFormatEmptyOrNull(std::string const& fmt)
    {
        return fmt.empty();
    }
}

#include <algorithm>
#include <stdio.h>

bool ExtractSingleModel(std::string& fname)
{
    if (fname.substr(fname.length() - 4, 4) == ".mdx")
    {
        fname.erase(fname.length() - 2, 2);
        fname.append("2");
    }

    std::string originalName = fname;

    char* name = GetPlainName((char*)fname.c_str());
    FixNameCase(name, strlen(name));
    FixNameSpaces(name, strlen(name));

    std::string output(szWorkDirWmo);
    output += "/";
    output += name;

    if (FileExists(output.c_str()))
        return true;

    Model mdl(originalName);
    if (!mdl.open())
        return false;

    return mdl.ConvertToVMAPModel(output.c_str());
}

extern HANDLE CascStorage;

struct GameObjectDisplayInfoMeta
{
    static DB2Meta const* Instance()
    {
        static char const* types = "ifffh";
        static uint8 const arraySizes[5] = { 1, 6, 1, 1, 1 };
        static DB2Meta instance(-1, 5, 0xE2D6FAB7, types, arraySizes);
        return &instance;
    }
};

struct CascFileHandleDeleter
{
    typedef HANDLE pointer;
    void operator()(HANDLE handle) const { CascCloseFile(handle); }
};

enum ModelTypes : uint32
{
    MODEL_MD20 = '02DM',
    MODEL_MD21 = '12DM',
    MODEL_WMO  = 'MVER'
};

bool GetHeaderMagic(std::string const& fileName, uint32* magic)
{
    *magic = 0;
    HANDLE file;
    if (!CascOpenFile(CascStorage, fileName.c_str(), CASC_LOCALE_ALL, 0, &file))
        return false;

    std::unique_ptr<HANDLE, CascFileHandleDeleter> modelFile(file);
    DWORD bytesRead = 0;
    if (!CascReadFile(file, magic, 4, &bytesRead) || bytesRead != 4)
        return false;

    return true;
}

void ExtractGameobjectModels()
{
    printf("Extracting GameObject models...");
    HANDLE dbcFile;
    if (!CascOpenFile(CascStorage, "DBFilesClient\\GameObjectDisplayInfo.db2", CASC_LOCALE_NONE, 0, &dbcFile))
    {
        printf("Fatal error: Cannot find GameObjectDisplayInfo.db2 in archive!\n");
        exit(1);
    }

    DB2FileLoader db2;
    if (!db2.Load(dbcFile, GameObjectDisplayInfoMeta::Instance()))
    {
        printf("Fatal error: Invalid GameObjectDisplayInfo.db2 file format!\n");
        exit(1);
    }

    std::string basepath = szWorkDirWmo;
    basepath += "/";

    std::string modelListPath = basepath + "temp_gameobject_models";
    FILE* model_list = fopen(modelListPath.c_str(), "wb");
    if (!model_list)
    {
        printf("Fatal error: Could not open file %s\n", modelListPath.c_str());
        return;
    }

    for (uint32 rec = 0; rec < db2.GetNumRows(); ++rec)
    {
        uint32 fileId = db2.getRecord(rec).getUInt(0, 0);
        if (!fileId)
            continue;

        std::string fileName = Trinity::StringFormat("FILE%08X.xxx", fileId);
        bool result = false;
        uint32 header;
        if (!GetHeaderMagic(fileName, &header))
            continue;

        if (header == MODEL_WMO)
            result = ExtractSingleWmo(fileName);
        else if (header == MODEL_MD20 || header == MODEL_MD21)
            result = ExtractSingleModel(fileName);
        else
            ASSERT(false, "%s header: %d - %c%c%c%c", fileName.c_str(), header, (header >> 24) & 0xFF, (header >> 16) & 0xFF, (header >> 8) & 0xFF, header & 0xFF);

        if (result)
        {
            uint32 displayId = db2.getId(rec);
            uint32 path_length = fileName.length();
            fwrite(&displayId, sizeof(uint32), 1, model_list);
            fwrite(&path_length, sizeof(uint32), 1, model_list);
            fwrite(fileName.c_str(), sizeof(char), path_length, model_list);
        }
    }

    fclose(model_list);

    printf("Done!\n");
}
