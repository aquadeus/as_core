////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _TILEASSEMBLER_H_
#define _TILEASSEMBLER_H_

#include <G3D/Vector3.h>
#include <G3D/Matrix3.h>
#include <deque>
#include "Common.h"

#include "ModelInstance.h"
#include "WorldModel.h"

namespace VMAP
{
    /**
    This Class is used to convert raw vector data into balanced BSP-Trees.
    To start the conversion call convertWorld().
    */
    //===============================================

    class ModelPosition
    {
        private:
            G3D::Matrix3 iRotation;
        public:
            ModelPosition(): iScale(0.0f) { }
            G3D::Vector3 iPos;
            G3D::Vector3 iDir;
            float iScale;
            void init()
            {
                iRotation = G3D::Matrix3::fromEulerAnglesZYX(G3D::pif()*iDir.y/180.0f, G3D::pif()*iDir.x/180.0f, G3D::pif()*iDir.z/180.0f);
            }
            G3D::Vector3 transform(const G3D::Vector3& pIn) const;
            void moveToBasePos(const G3D::Vector3& pBasePos) { iPos -= pBasePos; }
    };

    struct TileSpawn
    {
        TileSpawn() : Id(0), Flags(0) { }
        TileSpawn(uint32 id, uint32 flags) : Id(id), Flags(flags) { }

        uint32 Id;
        uint32 Flags;
        bool operator<(TileSpawn const& right) const { return Id < right.Id; }
    };

    struct MapSpawns
    {
        MapSpawns() { }

        uint32 MapId;
        std::map<uint32, ModelSpawn> UniqueEntries;
        std::map<uint32 /*packedTileId*/, std::set<TileSpawn>> TileEntries;
        std::map<uint32 /*packedTileId*/, std::set<TileSpawn>> ParentTileEntries;
    };

    typedef std::deque<MapSpawns> MapData;
    //===============================================

    struct GroupModel_Raw
    {
        uint32 mogpflags;
        uint32 GroupWMOID;

        G3D::AABox bounds;
        uint32 liquidflags;
        std::vector<MeshTriangle> triangles;
        std::vector<G3D::Vector3> vertexArray;
        class WmoLiquid* liquid;

        GroupModel_Raw() : mogpflags(0), GroupWMOID(0), liquidflags(0),
            liquid(NULL) { }
        ~GroupModel_Raw();

        bool Read(FILE* f);
    };

    struct WorldModel_Raw
    {
        uint32 RootWMOID;
        std::vector<GroupModel_Raw> groupsArray;

        bool Read(const char * path);
    };

    class TileAssembler
    {
        private:
            std::string iDestDir;
            std::string iSrcDir;
            unsigned int iCurrentUniqueNameId;
            MapData mapData;
            std::set<std::string> spawnedModelFiles;

        public:
            TileAssembler(const std::string& pSrcDirName, const std::string& pDestDirName);
            virtual ~TileAssembler();

            bool convertWorld2();
            bool readMapSpawns();
            bool calculateTransformedBound(ModelSpawn &spawn);
            void exportGameobjectModels();

            bool convertRawFile(const std::string& pModelFilename);
    };

}                                                           // VMAP
#endif                                                      /*_TILEASSEMBLER_H_*/
