////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef SC_SYSTEM_H
#define SC_SYSTEM_H

#include "ScriptMgr.h"
#include "Common.h"

#define TEXT_SOURCE_RANGE -1000000                          //the amount of entries each text source has available

struct ScriptPointMove
{
    uint32 uiCreatureEntry;
    uint32 uiPointId;
    float  fX;
    float  fY;
    float  fZ;
    uint32 uiWaitTime;
};

typedef std::vector<ScriptPointMove> ScriptPointVector;

struct StringTextData
{
    uint32 uiSoundId;
    uint8  uiType;
    uint32 uiLanguage;
    uint32 uiEmote;
};

class SystemMgr
{
        friend class ACE_Singleton<SystemMgr, ACE_Null_Mutex>;
        SystemMgr() {}
        ~SystemMgr() {}

    public:
        //Maps and lists
        typedef std::unordered_map<int32, StringTextData> TextDataMap;
        typedef std::unordered_map<uint32, ScriptPointVector> PointMoveMap;

        //Database
        void LoadScriptTexts();
        void LoadScriptTextsCustom();
        void LoadScriptWaypoints();

        //Retrive from storage
        StringTextData const* GetTextData(int32 textId) const
        {
            TextDataMap::const_iterator itr = m_mTextDataMap.find(textId);

            if (itr == m_mTextDataMap.end())
                return NULL;

            return &itr->second;
        }

        ScriptPointVector const& GetPointMoveList(uint32 creatureEntry) const
        {
            PointMoveMap::const_iterator itr = m_mPointMoveMap.find(creatureEntry);

            if (itr == m_mPointMoveMap.end())
                return _empty;

            return itr->second;
        }

    protected:
        TextDataMap     m_mTextDataMap;                     //additional data for text strings
        PointMoveMap    m_mPointMoveMap;                    //coordinates for waypoints

    private:
        static ScriptPointVector const _empty;
};

#define sScriptSystemMgr ACE_Singleton<SystemMgr, ACE_Null_Mutex>::instance()

#endif
