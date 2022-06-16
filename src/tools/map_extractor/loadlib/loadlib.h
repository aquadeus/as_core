////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef LOAD_LIB_H
#define LOAD_LIB_H

#include "Define.h"
#ifdef PLATFORM_WINDOWS
#undef PLATFORM_WINDOWS
#endif
#include "CascLib.h"
#include <map>
#include <string>

#ifndef _WIN32
int GetLastError();
#endif

#define FILE_FORMAT_VERSION    18

#pragma pack(push, 1)

union u_map_fcc
{
    char   fcc_txt[4];
    uint32 fcc;
};

//
// File version chunk
//
struct file_MVER
{
    union{
        uint32 fcc;
        char   fcc_txt[4];
    };
    uint32 size;
    uint32 ver;
};

struct file_MWMO
{
    u_map_fcc fcc;
    uint32 size;
    char FileList[1];
};

class FileChunk
{
public:
    FileChunk(uint8* data_, uint32 size_) : data(data_), size(size_) { }
    ~FileChunk();

    uint8* data;
    uint32 size;

    template<class T>
    T* As() { return (T*)data; }
    void parseSubChunks();
    std::multimap<std::string, FileChunk*> subchunks;
    FileChunk* GetSubChunk(std::string const& name);
};

class ChunkedFile
{
public:
    uint8  *data;
    uint32  data_size;

    uint8 *GetData()     { return data; }
    uint32 GetDataSize() { return data_size; }

    ChunkedFile();
    virtual ~ChunkedFile();
    bool prepareLoadedData();
    bool loadFile(HANDLE mpq, std::string const& fileName, bool log = true);
    void free();

    void parseChunks();
    std::multimap<std::string, FileChunk*> chunks;
    FileChunk* GetChunk(std::string const& name);
};

#pragma pack(pop)

#endif
