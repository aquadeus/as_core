#include "throne_of_the_four_winds.h"

#define ENCOUNTERS 2

class instance_throne_of_the_four_winds : public InstanceMapScript
{
public:
    instance_throne_of_the_four_winds() : InstanceMapScript("instance_throne_of_the_four_winds", 754) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_throne_of_the_four_winds_InstanceMapScript(map);
    }

    struct instance_throne_of_the_four_winds_InstanceMapScript: public InstanceScript
    {
        instance_throne_of_the_four_winds_InstanceMapScript(InstanceMap* map) : InstanceScript(map) {}

        uint32 Encounter[ENCOUNTERS];

        uint64 uiAnshal{};
        uint64 uiNezir{};
        uint64 uiRohash{};
        uint64 uiAlakir{};
        uint64 alakirChestGuid{};
        uint8 conclaveDieCounter{};

        void Initialize() override
        {
            uiAnshal = 0;
            uiNezir = 0;
            uiRohash = 0;
            uiAlakir = 0;
            alakirChestGuid = 0;
            conclaveDieCounter = 0;

            for (uint8 i = 0 ; i < ENCOUNTERS; ++i)
                Encounter[i] = NOT_STARTED;
        }

        void OnPlayerEnter(Player* player) override {}

        bool IsEncounterInProgress(int32) const override
        {
            for (uint8 i = 0; i < ENCOUNTERS; ++i)
            {
                if (Encounter[i] == IN_PROGRESS)
                    return true;
            }
            return false;
        }

        void OnCreatureCreate(Creature* creature) override
        {
            switch (creature->GetEntry())
            {
                case BOSS_ANSHAL:
                    uiAnshal = creature->GetGUID();
                    break;
                case BOSS_NEZIR:
                    uiNezir = creature->GetGUID();
                    break;
                case BOSS_ROHASH:
                    uiRohash = creature->GetGUID();
                    break;
                case BOSS_ALAKIR:
                    uiAlakir = creature->GetGUID();
                    break;
            }
        }

        void OnGameObjectCreate(GameObject* go) override
        {
            switch (go->GetEntry())
            {
                case GO_HEART_OF_THE_WIND:
                    go->SetVisible(false);
                    alakirChestGuid = go->GetGUID();
                    break;
                default:
                    break;
            }
        }

        void CreatureDies(Creature* creature, Unit* /*killer*/) override
        {
            switch (creature->GetEntry())
            {
                case BOSS_ANSHAL:
                case BOSS_NEZIR:
                case BOSS_ROHASH:
                    if (++conclaveDieCounter == 3)
                    {
                        SetData(DATA_CONCLAVE_OF_WIND_EVENT, DONE);
                        DoUpdateCriteria(CRITERIA_TYPE_BE_SPELL_TARGET, 88835);
                        SaveToDB();
                    }
                    break;
            }
        }

        uint64 GetData64(uint32 data) override
        {
            switch (data)
            {
                case DATA_ANSHAL:
                    return uiAnshal;
                case DATA_NEZIR:
                    return uiNezir;
                case DATA_ROHASH:
                    return uiRohash;
                case DATA_ALAKIR:
                    return uiAlakir;
                case DATA_ALAKIR_CHEST:
                    return alakirChestGuid;
                default:
                    break;
            }
            return 0;
        }

        void SummonWinds()
        {
            Position const windPos[] =
            {
                {-287.938f, 817.395f, 211.489f, 0.f},
                {-47.953f, 1053.439f, 211.489f, 0.f},
                {189.393f, 812.568f, 211.489f, 0.f},
                {-51.463f, 576.25f, 211.489f, 0.f},
            };

            for (auto pos : windPos)
            {
                if (auto wind = instance->SummonCreature(47066, pos))
                {
                    wind->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    wind->RemoveAllAuras();
                    wind->CastSpell(wind, 87713, true);
                    wind->SetCanFly(true);
                    wind->SetDisableGravity(true);
                    wind->SetDisplayId(35402);
                }
            }
        }

        void SetData(uint32 type, uint32 state) override
        {
            switch (type)
            {
                case DATA_CONCLAVE_OF_WIND_EVENT:
                    if (state != DONE || conclaveDieCounter == 3)
                    {
                        Encounter[0] = state;
                        if (state == DONE)
                        {
                            if (auto alakir = instance->GetCreature(uiAlakir))
                                alakir->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                            SummonWinds();
                        }
                    }
                    break;
                case DATA_ALAKIR_EVENT:
                    Encounter[1] = state;
                    break;
            }

            if (state == DONE)
                SaveToDB();

            if (state == IN_PROGRESS)
            {
                if (Creature* Anshal = instance->GetCreature(uiAnshal))
                    Anshal->RemoveAura(SPELL_PRE_COMBAT_EFFECT_ANSHAL);

                if (Creature* Nezir = instance->GetCreature(uiNezir))
                    Nezir->RemoveAura(SPELL_PRE_COMBAT_EFFECT_NEZIR);

                if (Creature* Rohash = instance->GetCreature(uiRohash))
                    Rohash->RemoveAura(SPELL_PRE_COMBAT_EFFECT_ROHASH);

            }
            else if (state == FAIL || state == NOT_STARTED)
            {
                if (Creature* Anshal = instance->GetCreature(uiAnshal))
                    Anshal->AI()->EnterEvadeMode();

                if (Creature* Nezir = instance->GetCreature(uiNezir))
                    Nezir->AI()->EnterEvadeMode();

                if (Creature* Rohash = instance->GetCreature(uiRohash))
                    Rohash->AI()->EnterEvadeMode();
            }
        }

        uint32 GetData(uint32 type) override
        {
            switch (type)
            {
                case DATA_CONCLAVE_OF_WIND_EVENT:
                    return Encounter[0];
                case DATA_ALAKIR_EVENT:
                    return Encounter[1];
                default:
                    break;
            }
            return 0;
        }

        std::string GetSaveData() override
        {
            OUT_SAVE_INST_DATA;

            std::string str_data;
            std::ostringstream saveStream;
            saveStream << "T W " << Encounter[0] << " " << Encounter[1];
            str_data = saveStream.str();

            OUT_SAVE_INST_DATA_COMPLETE;
            return str_data;
        }

        void Load(const char* in) override
        {
            if (!in)
            {
                OUT_LOAD_INST_DATA_FAIL;
                return;
            }

            OUT_LOAD_INST_DATA(in);

            char dataHead1, dataHead2;
            uint16 data0, data1;

            std::istringstream loadStream(in);
            loadStream >> dataHead1 >> dataHead2 >> data0 >> data1;

            if (dataHead1 == 'T' && dataHead2 == 'W')
            {
                Encounter[0] = data0;
                if (data0 == DONE)
                    conclaveDieCounter = 3;
                Encounter[1] = data1;

                for (uint8 i = 0; i < ENCOUNTERS; ++i)
                {
                    if (Encounter[i] == IN_PROGRESS)
                        Encounter[i] = NOT_STARTED;
                    else
                        SetData(i, Encounter[i]);
                }
            }
            else OUT_LOAD_INST_DATA_FAIL;

            OUT_LOAD_INST_DATA_COMPLETE;
        }
    };
};

void AddSC_instance_throne_of_the_four_winds()
{
    new instance_throne_of_the_four_winds();
}
