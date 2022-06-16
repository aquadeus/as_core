#include "scarlet_monastery.h"

enum Entry
{
    ENTRY_PUMPKIN_SHRINE    = 186267,
    ENTRY_HORSEMAN          = 23682,
    ENTRY_HEAD              = 23775,
    ENTRY_PUMPKIN           = 23694,
    NPC_THALNOS             = 59789,
    NPC_KORLOFF             = 59223,
    NPC_DURAND              = 60040,
    NPC_WHITEMANE           = 3977,
};

#define MAX_ENCOUNTER 3

class instance_scarlet_monastery : public InstanceMapScript
{
public:
    instance_scarlet_monastery() : InstanceMapScript("instance_scarlet_monastery", 1004) { }

    InstanceScript* GetInstanceScript(InstanceMap* p_Map) const override
    {
        return new instance_scarlet_monastery_InstanceMapScript(p_Map);
    }

    struct instance_scarlet_monastery_InstanceMapScript : public InstanceScript
    {
        instance_scarlet_monastery_InstanceMapScript(Map* p_Map) : InstanceScript(p_Map) {}

        uint64 m_PumpkinShrineGUID;
        uint64 m_HorsemanGUID;
        uint64 m_HeadGUID;
        std::set<uint64> m_HorsemanAdds;

        uint64 m_ThalnosGUID;
        uint64 m_KorloffGUID;
        uint64 m_DurandGUID;
        uint64 m_WhitemaneGUID;

        uint32 m_Encounter[MAX_ENCOUNTER];

        void Initialize() override
        {
            memset(&m_Encounter, 0, sizeof(m_Encounter));

            m_PumpkinShrineGUID  = 0;
            m_HorsemanGUID = 0;
            m_HeadGUID = 0;
            m_HorsemanAdds.clear();

            m_ThalnosGUID = 0;
            m_KorloffGUID = 0;
            m_DurandGUID = 0;
            m_WhitemaneGUID = 0;
        }

        void OnGameObjectCreate(GameObject* go) override
        {
            switch (go->GetEntry())
            {
            case ENTRY_PUMPKIN_SHRINE: m_PumpkinShrineGUID = go->GetGUID();break;
            }
        }

        void OnCreatureCreate(Creature* creature) override
        {
            switch (creature->GetEntry())
            {
                case ENTRY_HORSEMAN:    m_HorsemanGUID = creature->GetGUID(); break;
                case ENTRY_HEAD:        m_HeadGUID = creature->GetGUID(); break;
                case ENTRY_PUMPKIN:     m_HorsemanAdds.insert(creature->GetGUID());break;
                case NPC_THALNOS: m_ThalnosGUID = creature->GetGUID(); break;
                case NPC_KORLOFF: m_KorloffGUID = creature->GetGUID(); break;
                case NPC_DURAND: m_DurandGUID = creature->GetGUID(); break;
                case NPC_WHITEMANE: m_WhitemaneGUID = creature->GetGUID(); break;
            }
        }

        void SetData(uint32 p_Type, uint32 p_Data) override
        {
            switch (p_Type)
            {
                case eEnums::GAMEOBJECT_PUMPKIN_SHRINE:
                    HandleGameObject(m_PumpkinShrineGUID, false);
                    break;
                case eEnums::DATA_HORSEMAN_EVENT:
                    m_Encounter[1] = p_Data;
                    if (p_Data == EncounterState::DONE)
                    {
                        for (std::set<uint64>::const_iterator itr = m_HorsemanAdds.begin(); itr != m_HorsemanAdds.end(); ++itr)
                        {
                            Creature* add = instance->GetCreature(*itr);
                            if (add && add->isAlive())
                                add->Kill(add);
                        }
                        m_HorsemanAdds.clear();
                        HandleGameObject(m_PumpkinShrineGUID, false);
                    }
                    break;
            }
        }

        bool SetBossState(uint32 p_Type, EncounterState p_State) override
        {
            if (!InstanceScript::SetBossState(p_Type, p_State))
                return false;

            switch (p_Type)
            {
                case eEnums::DATA_THALNOS:
                    break;
                case eEnums::DATA_KORLOFF:
                    break;
                case eEnums::DATA_DURAND:
                    break;
                case eEnums::DATA_WHITEMANE:
                    break;
                default:
                    break;
            }

            return true;
        }

        uint64 GetData64(uint32 p_Type) override
        {
            switch (p_Type)
            {
                //case GAMEOBJECT_PUMPKIN_SHRINE:   return m_PumpkinShrineGUID;
                case eEnums::DATA_HORSEMAN:               return m_HorsemanGUID;
                //case DATA_HEAD:                   return m_HeadGUID;
                case eEnums::DATA_THALNOS:                  return m_ThalnosGUID;
                case eEnums::DATA_KORLOFF:                  return m_KorloffGUID;
                case eEnums::DATA_DURAND:                   return m_DurandGUID;
                case eEnums::DATA_WHITEMANE:                return m_WhitemaneGUID;
            }
            return 0;
        }

        uint32 GetData(uint32 p_Type) override
        {
            if (p_Type == eEnums::DATA_HORSEMAN_EVENT)
                return m_Encounter[1];
            return 0;
        }
    };
};

#ifndef __clang_analyzer__
void AddSC_instance_scarlet_monastery()
{
    new instance_scarlet_monastery();
}
#endif