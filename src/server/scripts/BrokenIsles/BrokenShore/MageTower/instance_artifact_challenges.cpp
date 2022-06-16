////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-Studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "artifact_challenges.hpp"

class instance_the_mage_hunter : public InstanceMapScript
{
    public:
        instance_the_mage_hunter() : InstanceMapScript("instance_the_mage_hunter", 1616) { }

        struct instance_the_mage_hunter_InstanceMapScript : InstanceScript
        {
            instance_the_mage_hunter_InstanceMapScript(Map* p_Map) : InstanceScript(p_Map) { }

            bool m_Initialized = false;

            uint32 m_DungeonID = 0;

            uint32 m_PlayerClass = 0;

            uint32 m_AgathaGuid = 0;

            void Initialize() override
            {
                m_Initialized   = false;

                m_DungeonID     = 0;

                m_PlayerClass   = 0;

                m_AgathaGuid    = 0;
            }

            void OnPlayerEnter(Player* p_Player) override
            {
                InstanceScript::OnPlayerEnter(p_Player);

                if (!m_Initialized)
                {
                    m_Initialized = true;

                    GroupPtr l_Group = p_Player->GetGroup();

                    m_DungeonID = l_Group ? sLFGMgr->GetDungeon(l_Group->GetGUID()) : 0;

                    if (!instance->IsLFR())
                        m_DungeonID = 0;

                    m_PlayerClass = p_Player->getClass();
                }
            }

            void OnPlayerExit(Player* p_Player) override
            {
                InstanceScript::OnPlayerExit(p_Player);

                p_Player->ScheduleDelayedOperation(PlayerDelayedOperations::DELAYED_LEAVE_GROUP);
            }

            void OnCreatureCreate(Creature* p_Creature) override
            {
                p_Creature->DisableHealthRegen();

                if (p_Creature->GetEntry() == eCreatures::CreatureAgatha)
                    m_AgathaGuid = p_Creature->GetGUID();
            }

            uint32 GetData(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eData::DataPlayerClass:
                        return m_PlayerClass;
                    default:
                        return 0;
                }
            }

            uint64 GetData64(uint32 p_ID) override
            {
                if (p_ID == eCreatures::CreatureAgatha)
                    return m_AgathaGuid;

                return 0;
            }

            void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
            {
                WorldSafeLocsEntry const* l_Loc = sWorldSafeLocsStore.LookupEntry(eData::DataResLocID);
                if (l_Loc == nullptr)
                    return;

                p_X     = l_Loc->x;
                p_Y     = l_Loc->y;
                p_Z     = l_Loc->z;
                p_O     = l_Loc->o;
                p_MapID = l_Loc->MapID;
            }

            bool PreventTeleportOutIfNeeded(Player* p_Player) const override
            {
                p_Player->TeleportTo(eData::DataResLocID);
                return true;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* p_Map) const override
        {
            return new instance_the_mage_hunter_InstanceMapScript(p_Map);
        }
};

class instance_feltotems_fall : public InstanceMapScript
{
    public:
        instance_feltotems_fall() : InstanceMapScript("instance_feltotems_fall", 1702) { }

        struct instance_feltotems_fall_InstanceMapScript : InstanceScript
        {
            instance_feltotems_fall_InstanceMapScript(Map* p_Map) : InstanceScript(p_Map) { }

            bool m_Initialized = false;

            uint32 m_DungeonID = 0;

            uint32 m_PlayerClass = 0;

            uint64 m_JormogGuid = 0;
            uint64 m_TugarGuid = 0;
            uint64 m_GasStalker = 0;

            uint8 m_NeededKill = 2;

            Position const m_NavaroggOutroPos = { 5251.59f, 4989.718f, 629.1042f, 2.647457f };

            void Initialize() override
            {
                m_Initialized   = false;

                m_DungeonID     = 0;

                m_PlayerClass   = 0;

                m_JormogGuid    = 0;
                m_TugarGuid     = 0;
                m_GasStalker    = 0;
            }

            void OnPlayerEnter(Player* p_Player) override
            {
                InstanceScript::OnPlayerEnter(p_Player);

                if (!m_Initialized)
                {
                    m_Initialized = true;

                    GroupPtr l_Group = p_Player->GetGroup();

                    m_DungeonID = l_Group ? sLFGMgr->GetDungeon(l_Group->GetGUID()) : 0;

                    if (!instance->IsLFR())
                        m_DungeonID = 0;

                    m_PlayerClass = p_Player->getClass();
                }
            }

            void OnPlayerExit(Player* p_Player) override
            {
                InstanceScript::OnPlayerExit(p_Player);

                p_Player->ScheduleDelayedOperation(PlayerDelayedOperations::DELAYED_LEAVE_GROUP);
            }

            void OnCreatureCreate(Creature* p_Creature) override
            {
                p_Creature->DisableHealthRegen();

                switch (p_Creature->GetEntry())
                {
                    case eCreatures::CreatureJormogTheBehemoth:
                    {
                        p_Creature->SetVisible(false);
                        p_Creature->SetReactState(ReactStates::REACT_DEFENSIVE);

                        m_JormogGuid = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::CreatureTugarBloodtotem:
                    {
                        p_Creature->SetVisible(false);
                        p_Creature->SetReactState(ReactStates::REACT_DEFENSIVE);

                        m_TugarGuid = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::CreatureGasCloudStalker:
                    {
                        m_GasStalker = p_Creature->GetGUID();

                        p_Creature->AddDelayedEvent([this, p_Creature]() -> void
                        {
                            p_Creature->CastSpell(p_Creature, eCommonSpells::SpellGasCloudAura, true);
                        }, 1);

                        break;
                    }
                    default:
                        break;
                }
            }

            void CreatureDiesForScript(Creature* p_Creature, Unit* p_Killer) override
            {
                switch (p_Creature->GetEntry())
                {
                    case eCreatures::CreatureJormogTheBehemoth:
                    case eCreatures::CreatureTugarBloodtotem:
                    {
                        --m_NeededKill;

                        if (m_NeededKill <= 0)
                        {
                            m_NeededKill = 0;

                            if (Creature* l_Creature = instance->GetCreature(m_GasStalker))
                                l_Creature->DespawnOrUnsummon();

                            instance->SummonCreature(eCreatures::CreatureNavaroggOutro, m_NavaroggOutroPos);

                            DoRemoveAurasDueToSpellOnPlayers(eCommonSpells::SpellGasCloudDoT);

                            Player* l_Killer = p_Killer->GetCharmerOrOwnerPlayerOrPlayerItself();
                            if (!l_Killer)
                                break;

                            l_Killer->KilledMonsterCredit(eCreatures::CreatureFinalBossDefeated);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eData::DataPlayerClass:
                        return m_PlayerClass;
                    default:
                        return 0;
                }
            }

            uint64 GetData64(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eCreatures::CreatureJormogTheBehemoth:
                        return m_JormogGuid;
                    case eCreatures::CreatureTugarBloodtotem:
                        return m_TugarGuid;
                    default:
                        break;
                }

                return 0;
            }

            void Update(uint32 p_Diff) override
            {
                InstanceScript::Update(p_Diff);

                if (!m_NeededKill)
                    return;

                Map::PlayerList const& l_List = instance->GetPlayers();
                for (Map::PlayerList::const_iterator l_Iter = l_List.begin(); l_Iter != l_List.end(); ++l_Iter)
                {
                    if (Player* l_Player = l_Iter->getSource())
                    {
                        if (!l_Player->IsInWorld())
                            continue;

                        if (l_Player->isAlive() && l_Player->GetPositionZ() <= 615.0f && !l_Player->HasAura(eCommonSpells::SpellGasCloudDoT))
                            l_Player->CastSpell(l_Player, eCommonSpells::SpellGasCloudDoT, true);
                        else if (l_Player->isDead() || l_Player->GetPositionZ() > 615.0f)
                            l_Player->RemoveAura(eCommonSpells::SpellGasCloudDoT);
                    }
                }
            }

            void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
            {
                WorldSafeLocsEntry const* l_Loc = sWorldSafeLocsStore.LookupEntry(eData::DataResLocID);
                if (l_Loc == nullptr)
                    return;

                p_X     = l_Loc->x;
                p_Y     = l_Loc->y;
                p_Z     = l_Loc->z;
                p_O     = l_Loc->o;
                p_MapID = l_Loc->MapID;
            }

            bool PreventTeleportOutIfNeeded(Player* p_Player) const override
            {
                p_Player->TeleportTo(eData::DataResLocID);
                return true;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* p_Map) const override
        {
            return new instance_feltotems_fall_InstanceMapScript(p_Map);
        }
};

class instance_the_archmages_reckoning : public InstanceMapScript
{
    public:
        instance_the_archmages_reckoning() : InstanceMapScript("instance_the_archmages_reckoning", 1673) { }

        struct instance_the_archmages_reckoning_InstanceMapScript : InstanceScript
        {
            instance_the_archmages_reckoning_InstanceMapScript(Map* p_Map) : InstanceScript(p_Map) { }

            bool m_Initialized = false;

            uint32 m_DungeonID = 0;

            uint32 m_PlayerClass = 0;

            uint64 m_ArchmageGuid = 0;
            uint64 m_InvisibleMan = 0;
            uint64 m_CorruptGuid = 0;

            void Initialize() override
            {
                m_Initialized   = false;

                m_DungeonID     = 0;

                m_PlayerClass   = 0;

                m_ArchmageGuid  = 0;
                m_InvisibleMan  = 0;
                m_CorruptGuid   = 0;
            }

            void OnPlayerEnter(Player* p_Player) override
            {
                InstanceScript::OnPlayerEnter(p_Player);

                if (!m_Initialized)
                {
                    m_Initialized = true;

                    GroupPtr l_Group = p_Player->GetGroup();

                    m_DungeonID = l_Group ? sLFGMgr->GetDungeon(l_Group->GetGUID()) : 0;

                    if (!instance->IsLFR())
                        m_DungeonID = 0;

                    m_PlayerClass = p_Player->getClass();
                }
            }

            void OnPlayerExit(Player* p_Player) override
            {
                InstanceScript::OnPlayerExit(p_Player);

                p_Player->ScheduleDelayedOperation(PlayerDelayedOperations::DELAYED_LEAVE_GROUP);
            }

            void OnCreatureCreate(Creature* p_Creature) override
            {
                p_Creature->DisableHealthRegen();

                switch (p_Creature->GetEntry())
                {
                    case eCreatures::CreatureArchmageXylem:
                    {
                        m_ArchmageGuid = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::CreatureOldInvisibleMan:
                    {
                        m_InvisibleMan = p_Creature->GetGUID();

                        p_Creature->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_SELECTION_DISABLED);

                        p_Creature->SetReactState(ReactStates::REACT_PASSIVE);
                        p_Creature->AddUnitState(UnitState::UNIT_STATE_ROOT);
                        break;
                    }
                    case eCreatures::CreatureCorruptingShadows:
                    {
                        m_CorruptGuid = p_Creature->GetGUID();
                        break;
                    }
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eData::DataPlayerClass:
                        return m_PlayerClass;
                    default:
                        return 0;
                }
            }

            uint64 GetData64(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eCreatures::CreatureArchmageXylem:
                        return m_ArchmageGuid;
                    case eCreatures::CreatureOldInvisibleMan:
                        return m_InvisibleMan;
                    case eCreatures::CreatureCorruptingShadows:
                        return m_CorruptGuid;
                    default:
                        break;
                }

                return 0;
            }

            void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
            {
                WorldSafeLocsEntry const* l_Loc = sWorldSafeLocsStore.LookupEntry(eData::DataResLocID);
                if (l_Loc == nullptr)
                    return;

                p_X     = l_Loc->x;
                p_Y     = l_Loc->y;
                p_Z     = l_Loc->z;
                p_O     = l_Loc->o;
                p_MapID = l_Loc->MapID;
            }

            bool PreventTeleportOutIfNeeded(Player* p_Player) const override
            {
                p_Player->TeleportTo(eData::DataResLocID);
                return true;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* p_Map) const override
        {
            return new instance_the_archmages_reckoning_InstanceMapScript(p_Map);
        }
};

class instance_thwarting_the_twins : public InstanceMapScript
{
    public:
        instance_thwarting_the_twins() : InstanceMapScript("instance_thwarting_the_twins", 1684) { }

        struct instance_thwarting_the_twins_InstanceMapScript : InstanceScript
        {
            instance_thwarting_the_twins_InstanceMapScript(Map* p_Map) : InstanceScript(p_Map) { }

            bool m_Initialized = false;

            uint32 m_DungeonID = 0;

            uint32 m_PlayerClass = 0;

            uint64 m_RaestGuid = 0;
            uint64 m_KaramGuid = 0;

            uint64 m_NetherBreach = 0;

            void Initialize() override
            {
                m_Initialized   = false;

                m_DungeonID     = 0;

                m_PlayerClass   = 0;

                m_RaestGuid     = 0;
                m_KaramGuid     = 0;

                m_NetherBreach  = 0;
            }

            void OnPlayerEnter(Player* p_Player) override
            {
                InstanceScript::OnPlayerEnter(p_Player);

                if (!m_Initialized)
                {
                    m_Initialized = true;

                    GroupPtr l_Group = p_Player->GetGroup();

                    m_DungeonID = l_Group ? sLFGMgr->GetDungeon(l_Group->GetGUID()) : 0;

                    if (!instance->IsLFR())
                        m_DungeonID = 0;

                    m_PlayerClass = p_Player->getClass();

                    AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        instance->SummonGameObject(eGameObjects::GameObjectNetherBreach, { 3482.67f, 1484.36f, 432.581f, 0.0f });
                    });
                }
            }

            void OnPlayerExit(Player* p_Player) override
            {
                InstanceScript::OnPlayerExit(p_Player);

                p_Player->ScheduleDelayedOperation(PlayerDelayedOperations::DELAYED_LEAVE_GROUP);
            }

            void OnCreatureCreate(Creature* p_Creature) override
            {
                p_Creature->DisableHealthRegen();

                switch (p_Creature->GetEntry())
                {
                    case eCreatures::CreatureRaestMagespear:
                    {
                        m_RaestGuid = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::CreatureKaramMagespear:
                    {
                        m_KaramGuid = p_Creature->GetGUID();
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* p_GameObject) override
            {
                switch (p_GameObject->GetEntry())
                {
                    case eGameObjects::GameObjectNetherBreach:
                    {
                        m_NetherBreach = p_GameObject->GetGUID();
                        break;
                    }
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eData::DataPlayerClass:
                        return m_PlayerClass;
                    default:
                        return 0;
                }
            }

            void SetData(uint32 p_ID, uint32 p_Value) override
            {
                switch (p_ID)
                {
                    case eData::DataTwartingTheTwins:
                    {
                        /// Close breach during fight
                        if (p_Value > 0)
                        {
                            if (GameObject* l_Breach = instance->GetGameObject(m_NetherBreach))
                                l_Breach->Delete();
                        }
                        /// Breach is open before/after fight
                        else
                        {
                            AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                auto l_WorldObjects = instance->GetObjectsOnMap();
                                for (auto l_WorldObject = l_WorldObjects.begin(); l_WorldObject != l_WorldObjects.end(); l_WorldObject++)
                                {
                                    if ((*l_WorldObject)->GetTypeId() != TypeID::TYPEID_UNIT)
                                        continue;

                                    Creature* l_Creature = (*l_WorldObject)->ToCreature();

                                    l_Creature->RemoveAllAreasTrigger();
                                    l_Creature->DespawnOrUnsummon();
                                }
                            });

                            instance->SummonGameObject(eGameObjects::GameObjectNetherBreach, { 3482.67f, 1484.36f, 432.581f, 0.0f });
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            uint64 GetData64(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eCreatures::CreatureRaestMagespear:
                        return m_RaestGuid;
                    case eCreatures::CreatureKaramMagespear:
                        return m_KaramGuid;
                    default:
                        break;
                }

                return 0;
            }

            void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
            {
                WorldSafeLocsEntry const* l_Loc = sWorldSafeLocsStore.LookupEntry(eData::DataResLocID);
                if (l_Loc == nullptr)
                    return;

                p_X     = l_Loc->x;
                p_Y     = l_Loc->y;
                p_Z     = l_Loc->z;
                p_O     = l_Loc->o;
                p_MapID = l_Loc->MapID;
            }

            bool PreventTeleportOutIfNeeded(Player* p_Player) const override
            {
                p_Player->TeleportTo(eData::DataResLocID);
                return true;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* p_Map) const override
        {
            return new instance_thwarting_the_twins_InstanceMapScript(p_Map);
        }
};

class instance_the_god_queens_fury : public InstanceMapScript
{
    public:
        instance_the_god_queens_fury() : InstanceMapScript("instance_the_god_queens_fury", 1703) { }

        struct instance_the_god_queens_fury_InstanceMapScript : InstanceScript
        {
            instance_the_god_queens_fury_InstanceMapScript(Map* p_Map) : InstanceScript(p_Map) { }

            bool m_Initialized = false;

            uint32 m_DungeonID = 0;

            uint32 m_PlayerClass = 0;

            uint64 m_BarrierGuid = 0;

            uint64 m_SigrynGuid = 0;
            uint64 m_VelbrandGuid = 0;
            uint64 m_FaljarGuid = 0;

            void Initialize() override
            {
                m_Initialized   = false;

                m_DungeonID     = 0;

                m_PlayerClass   = 0;

                m_BarrierGuid   = 0;

                m_SigrynGuid    = 0;
                m_VelbrandGuid  = 0;
                m_FaljarGuid    = 0;
            }

            void OnPlayerEnter(Player* p_Player) override
            {
                InstanceScript::OnPlayerEnter(p_Player);

                if (!m_Initialized)
                {
                    m_Initialized = true;

                    GroupPtr l_Group = p_Player->GetGroup();

                    m_DungeonID = l_Group ? sLFGMgr->GetDungeon(l_Group->GetGUID()) : 0;

                    if (!instance->IsLFR())
                        m_DungeonID = 0;

                    m_PlayerClass = p_Player->getClass();

                    if (m_PlayerClass == Classes::CLASS_ROGUE)
                        p_Player->CastSpell(p_Player, eCommonSpells::SpellDustyPockets, true);
                }
            }

            void OnPlayerExit(Player* p_Player) override
            {
                InstanceScript::OnPlayerExit(p_Player);

                p_Player->RemoveAura(eCommonSpells::SpellDustyPockets);

                p_Player->ScheduleDelayedOperation(PlayerDelayedOperations::DELAYED_LEAVE_GROUP);
            }

            void OnCreatureCreate(Creature* p_Creature) override
            {
                p_Creature->DisableHealthRegen();

                switch (p_Creature->GetEntry())
                {
                    case eCreatures::CreatureValarjarChampion:
                    case eCreatures::CreatureValarjarThundercaller:
                    case eCreatures::CreatureValarjarRunecarver:
                    case eCreatures::CreatureValarjarMystic:
                    case eCreatures::CreatureStormforgedSentinel:
                    case eCreatures::CreatureValarjarMarksman:
                    {
                        p_Creature->SetReactState(ReactStates::REACT_PASSIVE);

                        p_Creature->AddDelayedEvent([this, p_Creature]() -> void
                        {
                            p_Creature->CastSpell(p_Creature, eCommonSpells::SpellFeignDeath, true);

                            if (roll_chance_i(40))
                                p_Creature->CastSpell(p_Creature, eCommonSpells::SpellFireVisual, true);
                        }, 1);

                        p_Creature->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_UNK_29);
                        break;
                    }
                    case eCreatures::CreatureSigryn:
                    {
                        p_Creature->SetReactState(ReactStates::REACT_DEFENSIVE);

                        m_SigrynGuid = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::CreatureJarlVelbrand:
                    {
                        p_Creature->SetReactState(ReactStates::REACT_DEFENSIVE);

                        m_VelbrandGuid = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::CreatureRuneseerFaljar:
                    {
                        p_Creature->SetReactState(ReactStates::REACT_DEFENSIVE);

                        m_FaljarGuid = p_Creature->GetGUID();
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* p_GameObject) override
            {
                switch (p_GameObject->GetEntry())
                {
                    case eGameObjects::GameObjectBarrier:
                    {
                        m_BarrierGuid = p_GameObject->GetGUID();
                        break;
                    }
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eData::DataPlayerClass:
                        return m_PlayerClass;
                    default:
                        return 0;
                }
            }

            uint64 GetData64(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eGameObjects::GameObjectBarrier:
                        return m_BarrierGuid;
                    case eCreatures::CreatureSigryn:
                        return m_SigrynGuid;
                    case eCreatures::CreatureJarlVelbrand:
                        return m_VelbrandGuid;
                    case eCreatures::CreatureRuneseerFaljar:
                        return m_FaljarGuid;
                    default:
                        return 0;
                }
            }

            void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
            {
                WorldSafeLocsEntry const* l_Loc = sWorldSafeLocsStore.LookupEntry(eData::DataResLocID);
                if (l_Loc == nullptr)
                    return;

                p_X     = l_Loc->x;
                p_Y     = l_Loc->y;
                p_Z     = l_Loc->z;
                p_O     = l_Loc->o;
                p_MapID = l_Loc->MapID;
            }

            bool PreventTeleportOutIfNeeded(Player* p_Player) const override
            {
                p_Player->TeleportTo(eData::DataResLocID);
                return true;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* p_Map) const override
        {
            return new instance_the_god_queens_fury_InstanceMapScript(p_Map);
        }
};

class instance_end_of_the_risen_threat : public InstanceMapScript
{
    public:
        instance_end_of_the_risen_threat() : InstanceMapScript("instance_end_of_the_risen_threat", 1710) { }

        enum eActions
        {
            ActionStage1Completed,
            ActionStage3Triggered,
            ActionStage3Completed
        };

        struct instance_end_of_the_risen_threat_InstanceMapScript : InstanceScript
        {
            instance_end_of_the_risen_threat_InstanceMapScript(Map* p_Map) : InstanceScript(p_Map) { }

            bool m_Initialized = false;

            uint32 m_DungeonID = 0;

            uint32 m_PlayerClass = 0;
            uint64 m_PlayerGuid = 0;

            uint64 m_CommanderJarod = 0;
            uint64 m_CallieCarrington = 0;
            uint64 m_GrannyMarl = 0;
            uint64 m_DreadCorruptor = 0;
            uint64 m_LordErdrisGuid = 0;

            std::map<uint32, uint64> m_DoorsGuids;
            uint64 m_FirstDoorGuid = 0;
            uint64 m_DoorToStage3_1 = 0;
            uint64 m_DoorToStage3_2 = 0;

            std::map<uint32, std::set<uint64>> m_TrashesGuids;

            uint8 m_WaveIDx = 0;

            std::queue<uint64> m_IncomingAdds;
            std::array<std::set<uint64>, eData::DataWavesCount> m_WavesGuids;

            std::array<Position const, eData::DataMaxEyes> m_FlickeringEyes =
            {
                {
                    { 3182.258f, 7398.942f, 225.8774f, 4.244903f },
                    { 3183.403f, 7377.492f, 225.1075f, 2.084694f },
                    { 3166.628f, 7384.113f, 226.1349f, 0.166139f },
                    { 3188.381f, 7388.288f, 224.8576f, 2.923534f },
                    { 3170.016f, 7396.515f, 224.8576f, 5.360666f },
                    { 3173.809f, 7374.997f, 224.8576f, 1.231020f }
                }
            };

            bool m_OtherEyes = false;
            std::set<uint64> m_EyesToKill;

            uint64 m_PortalGuid = 0;
            uint32 m_SavingTimer = 0;

            void Initialize() override
            {
                m_Initialized       = false;

                m_DungeonID         = 0;

                m_PlayerClass       = 0;
                m_PlayerGuid        = 0;

                m_CommanderJarod    = 0;
                m_CallieCarrington  = 0;
                m_GrannyMarl        = 0;
                m_DreadCorruptor    = 0;

                m_FirstDoorGuid     = 0;

                m_WaveIDx           = 0;

                m_OtherEyes         = false;
            }

            void OnPlayerEnter(Player* p_Player) override
            {
                InstanceScript::OnPlayerEnter(p_Player);

                if (!m_Initialized)
                {
                    m_Initialized = true;

                    GroupPtr l_Group = p_Player->GetGroup();

                    m_DungeonID = l_Group ? sLFGMgr->GetDungeon(l_Group->GetGUID()) : 0;

                    if (!instance->IsLFR())
                        m_DungeonID = 0;

                    m_PlayerClass = p_Player->getClass();
                    m_PlayerGuid = p_Player->GetGUID();

                    AddTimedDelayedOperation(7 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        if (Player* l_Player = sObjectAccessor->FindPlayer(m_PlayerGuid))
                        {
                            GroupPtr l_Group = l_Player->GetGroup();
                            if (!l_Group)
                                return;

                            std::array<uint32, 3> l_Entries = { { eCreatures::CreatureJarodShadowsong, eCreatures::CreatureCallieCarrington, eCreatures::CreatureGrannyMarl } };

                            for (uint32 const& l_Entry : l_Entries)
                            {
                                if (Creature* l_Creature = instance->GetCreature(GetData64(l_Entry)))
                                    l_Group->AddCreatureMember(l_Creature);
                            }
                        }
                    });
                }
            }

            void OnPlayerExit(Player* p_Player) override
            {
                InstanceScript::OnPlayerExit(p_Player);

                p_Player->ScheduleDelayedOperation(PlayerDelayedOperations::DELAYED_LEAVE_GROUP);
            }

            void OnCreatureCreate(Creature* p_Creature) override
            {
                p_Creature->DisableHealthRegen();

                switch (p_Creature->GetEntry())
                {
                    case eCreatures::CreatureJarodShadowsong:
                    {
                        m_CommanderJarod = p_Creature->GetGUID();

                        p_Creature->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, Emote::EMOTE_STATE_READY1H);
                        break;
                    }
                    case eCreatures::CreatureCallieCarrington:
                    {
                        m_CallieCarrington = p_Creature->GetGUID();

                        p_Creature->DisableHealthRegen();
                        p_Creature->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, Emote::EMOTE_STATE_READY1H);
                        break;
                    }
                    case eCreatures::CreatureGrannyMarl:
                    {
                        m_GrannyMarl = p_Creature->GetGUID();

                        p_Creature->DisableHealthRegen();
                        p_Creature->SetSheath(SheathState::SHEATH_STATE_RANGED);
                        p_Creature->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, Emote::EMOTE_STATE_READYRIFLE);
                        break;
                    }
                    case eCreatures::CreatureCorruptedRisenSoldier:
                    case eCreatures::CreatureCorruptedRisenMage:
                    case eCreatures::CreatureCorruptedRisenArbalest:
                    {
                        m_TrashesGuids[p_Creature->GetEntry()].insert(p_Creature->GetGUID());
                        break;
                    }
                    case eCreatures::CreatureDreadCorruptor:
                    {
                        p_Creature->SetDisplayId(eData::DataInvisDisplay);

                        m_DreadCorruptor = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::CreatureFelBatPup:
                    {
                        p_Creature->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);
                        break;
                    }
                    case eCreatures::CreatureLordErdrisThorn:
                    {
                        m_LordErdrisGuid = p_Creature->GetGUID();
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* p_GameObject) override
            {
                switch (p_GameObject->GetEntry())
                {
                    case eGameObjects::GameObjectFirstDoor:
                    case eGameObjects::GameObjectDoorToThirdStage:
                    case eGameObjects::GameObjectDoorToThirdStage2:
                    {
                        m_DoorsGuids[p_GameObject->GetEntry()] = p_GameObject->GetGUID();
                        m_FirstDoorGuid = p_GameObject->GetGUID();
                        break;
                    }
                    case eGameObjects::GameObjectPortalToDalaran:
                    {
                        m_PortalGuid = p_GameObject->GetGUID();
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnGameObjectStateChanged(GameObject const* p_GameObject, uint32 p_State) override
            {
                if (p_State != GOState::GO_STATE_ACTIVE)
                    return;

                switch (p_GameObject->GetEntry())
                {
                    case eGameObjects::GameObjectDoorToFirstStairs:
                    {
                        Position const l_Pos = { 3173.646f, 7422.588f, 227.9174f, 3.267374f };
                        Position const l_UpperPos = { 3146.97f, 7364.027f, 264.858f, 0.0f };

                        if (p_GameObject->IsNearPosition(&l_Pos, 0.1f))
                        {
                            std::list<Creature*> l_Souls;

                            p_GameObject->GetCreatureListWithEntryInGrid(l_Souls, eCreatures::CreatureDamagedSoul, 30.0f);

                            for (Creature* l_Soul : l_Souls)
                            {
                                if (l_Soul->IsAIEnabled)
                                    l_Soul->AI()->DoAction(0);
                            }
                        }
                        else if (p_GameObject->IsNearPosition(&l_UpperPos, 0.1f))
                        {
                            if (Creature* l_Corruptor = instance->GetCreature(m_DreadCorruptor))
                            {
                                if (l_Corruptor->IsAIEnabled)
                                    l_Corruptor->AI()->DoAction(0);
                            }
                        }

                        break;
                    }
                    case eGameObjects::GameObjectDoorToLastRoom:
                    {
                        Player* l_Player = sObjectAccessor->FindPlayer(m_PlayerGuid);
                        Scenario* l_Scenario = sScenarioMgr->GetScenario(instance->GetScenarioGuid());

                        if (!l_Scenario || !l_Player)
                            break;

                        l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataStage4Asset1, 0, 0, l_Player, l_Player);

                        GroupPtr l_Group = l_Player->GetGroup();
                        if (!l_Group)
                            break;

                        if (Creature* l_Jarod = instance->GetCreature(m_CommanderJarod))
                        {
                            if (l_Jarod->isAlive() && l_Jarod->IsAIEnabled)
                            {
                                l_Group->AddCreatureMember(l_Jarod);

                                l_Jarod->AI()->DoAction(3);
                            }
                        }

                        if (Creature* l_Granny = instance->GetCreature(m_GrannyMarl))
                        {
                            if (l_Granny->isAlive() && l_Granny->IsAIEnabled)
                            {
                                l_Group->AddCreatureMember(l_Granny);

                                l_Granny->AI()->DoAction(3);
                            }
                        }

                        if (Creature* l_Callie = instance->GetCreature(m_CallieCarrington))
                        {
                            if (l_Callie->isAlive() && l_Callie->IsAIEnabled)
                            {
                                l_Group->AddCreatureMember(l_Callie);

                                l_Callie->AI()->DoAction(3);
                            }
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void CreatureDiesForScript(Creature* p_Creature, Unit* /*p_Killer*/) override
            {
                switch (p_Creature->GetEntry())
                {
                    case eCreatures::CreatureCorruptedRisenSoldier:
                    case eCreatures::CreatureCorruptedRisenMage:
                    case eCreatures::CreatureCorruptedRisenArbalest:
                    {
                        if (m_WaveIDx >= eData::DataWavesCount)
                            break;

                        std::set<uint64>& l_Guids = m_WavesGuids[m_WaveIDx];

                        l_Guids.erase(p_Creature->GetGUID());

                        /// Current wave has been wiped, schedule the next one
                        if (l_Guids.empty())
                        {
                            ++m_WaveIDx;

                            /// Last wave got killed, schedule the next step
                            if (m_WaveIDx >= eData::DataWavesCount)
                            {
                                Scenario* l_Scenario = sScenarioMgr->GetScenario(instance->GetScenarioGuid());
                                Player* l_Player = nullptr;

                                Map::PlayerList const& l_PlayerList = instance->GetPlayers();
                                for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                                {
                                    if (Player* l_Plr = l_Iter->getSource())
                                    {
                                        l_Player = l_Plr;
                                        break;
                                    }
                                }

                                if (!l_Scenario || !l_Player)
                                    return;

                                l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, eData::DataStage1Asset, 0, 0, l_Player, l_Player);

                                if (GameObject* l_Door = instance->GetGameObject(GetData64(eGameObjects::GameObjectFirstDoor)))
                                    l_Door->SetGoState(GOState::GO_STATE_ACTIVE);

                                if (GameObject* l_Door = instance->GetGameObject(GetData64(eGameObjects::GameObjectDoorToThirdStage)))
                                    l_Door->SetGoState(GOState::GO_STATE_ACTIVE);

                                if (GameObject* l_Door = instance->GetGameObject(GetData64(eGameObjects::GameObjectDoorToThirdStage2)))
                                    l_Door->SetGoState(GOState::GO_STATE_READY);

                                if (Creature* l_Jarod = instance->GetCreature(m_CommanderJarod))
                                {
                                    if (l_Jarod->IsAIEnabled)
                                        l_Jarod->AI()->DoAction(eActions::ActionStage1Completed);
                                }

                                if (Creature* l_Callie = instance->GetCreature(m_CallieCarrington))
                                {
                                    if (l_Callie->IsAIEnabled)
                                        l_Callie->AI()->DoAction(eActions::ActionStage1Completed);
                                }

                                if (Creature* l_Granny = instance->GetCreature(m_GrannyMarl))
                                {
                                    if (l_Granny->IsAIEnabled)
                                        l_Granny->AI()->DoAction(eActions::ActionStage1Completed);
                                }

                                break;
                            }

                            for (uint8 l_I = 0; l_I < uint8(m_WavesGuids[m_WaveIDx].size()); ++l_I)
                            {
                                if (m_IncomingAdds.empty())
                                    return;

                                if (Creature* l_Add = instance->GetCreature(m_IncomingAdds.front()))
                                {
                                    m_IncomingAdds.pop();

                                    l_Add->SetReactState(ReactStates::REACT_AGGRESSIVE);

                                    l_Add->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                                    bool l_Engaged = false;

                                    if (Creature* l_Jarod = instance->GetCreature(m_CommanderJarod))
                                    {
                                        if (l_Jarod->isAlive() && l_Add->IsAIEnabled && l_Jarod->IsAIEnabled)
                                        {
                                            if (m_WaveIDx >= 1 && l_Add->GetEntry() == eCreatures::CreatureCorruptedRisenMage)
                                                l_Jarod->AI()->AttackStart(l_Add);
                                            else
                                                l_Jarod->SetInCombatWith(l_Add);

                                            l_Engaged = true;

                                            l_Add->AI()->AttackStart(l_Jarod);
                                        }
                                    }

                                    if (Creature* l_Callie = instance->GetCreature(m_CallieCarrington))
                                    {
                                        if (l_Callie->isAlive() && l_Add->IsAIEnabled && l_Callie->IsAIEnabled)
                                        {
                                            if (m_WaveIDx >= 1 && l_Add->GetEntry() == eCreatures::CreatureCorruptedRisenMage)
                                                l_Callie->AI()->AttackStart(l_Add);
                                            else
                                                l_Callie->SetInCombatWith(l_Add);

                                            if (!l_Engaged)
                                            {
                                                l_Engaged = true;

                                                l_Add->AI()->AttackStart(l_Callie);
                                            }
                                        }
                                    }

                                    if (Creature* l_Granny = instance->GetCreature(m_GrannyMarl))
                                    {
                                        if (l_Granny->isAlive() && l_Add->IsAIEnabled && l_Granny->IsAIEnabled)
                                        {
                                            if (m_WaveIDx >= 1 && l_Add->GetEntry() == eCreatures::CreatureCorruptedRisenMage)
                                                l_Granny->AI()->AttackStart(l_Add);
                                            else
                                                l_Granny->SetInCombatWith(l_Add);

                                            if (!l_Engaged)
                                                l_Add->AI()->AttackStart(l_Granny);
                                        }
                                    }
                                }
                            }

                            break;
                        }

                        break;
                    }
                    case eCreatures::CreatureFlickeringEye:
                    case eCreatures::CreatureDreadCorruptor:
                    {
                        p_Creature->DespawnOrUnsummon(5 * TimeConstants::IN_MILLISECONDS);

                        /// Eyes on the upper floor
                        if (m_EyesToKill.find(p_Creature->GetGUID()) == m_EyesToKill.end())
                            break;

                        m_EyesToKill.erase(p_Creature->GetGUID());

                        if (m_EyesToKill.empty())
                        {
                            if (!m_OtherEyes)
                            {
                                m_OtherEyes = true;

                                if (GameObject* l_Door = instance->GetGameObject(GetData64(eGameObjects::GameObjectDoorToThirdStage2)))
                                    l_Door->SetGoState(GOState::GO_STATE_ACTIVE);
                            }
                            else
                            {
                                if (Creature* l_Jarod = instance->GetCreature(m_CommanderJarod))
                                {
                                    if (l_Jarod->IsAIEnabled)
                                        l_Jarod->AI()->DoAction(2);
                                }
                            }
                        }

                        break;
                    }
                    case eCreatures::CreatureDamagedSoul:
                    {
                        if (Creature* l_Soldier = instance->SummonCreature(eCreatures::CreatureCorruptedRisenSoldier2, p_Creature->GetPosition()))
                        {
                            l_Soldier->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                            if (l_Soldier->IsAIEnabled)
                            {
                                if (Player* l_Player = sObjectAccessor->GetPlayer(*l_Soldier, m_PlayerGuid))
                                    l_Soldier->AI()->AttackStart(l_Player);
                            }
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void SetData(uint32 p_ID, uint32 p_Value) override
            {
                switch (p_ID)
                {
                    case eData::DataRisenThreatMoves:
                    {
                        if (!m_IncomingAdds.empty())
                            break;

                        std::array<uint32, 3> l_Entries = { { eCreatures::CreatureCorruptedRisenSoldier, eCreatures::CreatureCorruptedRisenMage, eCreatures::CreatureCorruptedRisenArbalest } };

                        std::map<uint32, std::vector<Position>> l_PosByEntry =
                        {
                            {
                                eCreatures::CreatureCorruptedRisenArbalest,
                                {
                                    { 3259.087f, 7295.076f, 231.5648f, 5.5216f },
                                    { 3253.344f, 7313.417f, 231.5085f, 5.4898f },
                                    { 3262.880f, 7295.610f, 231.5160f, 5.4757f }
                                }
                            },
                            {
                                eCreatures::CreatureCorruptedRisenSoldier,
                                {
                                    { 3265.830f, 7304.110f, 231.4780f, 5.271845f },
                                    { 3267.276f, 7311.473f, 231.5648f, 5.361923f },
                                    { 3245.503f, 7305.429f, 231.5084f, 5.454912f },
                                    { 3250.530f, 7289.970f, 231.4620f, 5.556091f }
                                }
                            },
                            {
                                eCreatures::CreatureCorruptedRisenMage,
                                {
                                    { 3256.730f, 7293.220f, 231.4770f, 5.379259f },
                                    { 3263.644f, 7300.281f, 231.5648f, 5.272390f },
                                    { 3252.524f, 7292.211f, 231.5648f, 5.298771f },
                                    { 3251.440f, 7307.220f, 231.5090f, 5.480371f }
                                }
                            }
                        };

                        for (uint32 const& l_Entry : l_Entries)
                        {
                            auto l_Guids = m_TrashesGuids.find(l_Entry);
                            auto l_Moves = l_PosByEntry.find(l_Entry);
                            if (l_Guids == m_TrashesGuids.end() || l_Moves == l_PosByEntry.end())
                                continue;

                            uint8 l_IDx = 0;
                            for (uint64 const& l_Guid : l_Guids->second)
                            {
                                if (Creature* l_Creature = instance->GetCreature(l_Guid))
                                {
                                    l_Creature->GetMotionMaster()->Clear();
                                    l_Creature->GetMotionMaster()->MovePoint(1, l_Moves->second[l_IDx++]);
                                }
                            }
                        }

                        AddTimedDelayedOperation(7 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            if (GameObject* l_Door = instance->GetGameObject(GetData64(eGameObjects::GameObjectFirstDoor)))
                                l_Door->SetGoState(GOState::GO_STATE_READY);
                        });

                        break;
                    }
                    case eData::DataRisenThreatWaves:
                    {
                        /// Pre-register the coming waves
                        std::array<std::vector<uint32>, eData::DataWavesCount> l_WavesPattern =
                        {
                            {
                                { eCreatures::CreatureCorruptedRisenArbalest },
                                { eCreatures::CreatureCorruptedRisenArbalest, eCreatures::CreatureCorruptedRisenMage },
                                { eCreatures::CreatureCorruptedRisenMage, eCreatures::CreatureCorruptedRisenSoldier },
                                { eCreatures::CreatureCorruptedRisenMage, eCreatures::CreatureCorruptedRisenSoldier, eCreatures::CreatureCorruptedRisenSoldier },
                                { eCreatures::CreatureCorruptedRisenArbalest, eCreatures::CreatureCorruptedRisenMage, eCreatures::CreatureCorruptedRisenSoldier }
                            }
                        };

                        std::set<uint64> l_SelectedAdds;

                        uint8 l_IDx = 0;
                        for (auto const& l_Itr : l_WavesPattern)
                        {
                            for (uint32 const& l_Entry : l_Itr)
                            {
                                auto const& l_Guids = m_TrashesGuids.find(l_Entry);
                                if (l_Guids == m_TrashesGuids.end())
                                    continue;

                                for (uint64 const& l_Guid : l_Guids->second)
                                {
                                    /// Add already selected for a wave, pick up another one
                                    if (l_SelectedAdds.find(l_Guid) != l_SelectedAdds.end())
                                        continue;

                                    /// Custom case for first wave, it's always the same Arbalest
                                    if (l_IDx == 0)
                                    {
                                        if (Creature* l_Add = instance->GetCreature(l_Guid))
                                        {
                                            Position const l_FirstPos = { 3262.880f, 7295.610f, l_Add->m_positionZ, 5.4757f };
                                            if (!l_Add->IsNearPosition(&l_FirstPos, 0.2f))
                                                continue;
                                        }
                                    }

                                    m_IncomingAdds.push(l_Guid);
                                    l_SelectedAdds.insert(l_Guid);

                                    m_WavesGuids[l_IDx].insert(l_Guid);
                                    break;
                                }
                            }

                            ++l_IDx;
                        }

                        /// Directly schedule the first wave
                        AddTimedDelayedOperation(20 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            if (m_IncomingAdds.empty())
                                return;

                            if (Creature* l_Add = instance->GetCreature(m_IncomingAdds.front()))
                            {
                                m_IncomingAdds.pop();

                                l_Add->SetReactState(ReactStates::REACT_AGGRESSIVE);

                                l_Add->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                                if (Creature* l_Jarod = instance->GetCreature(m_CommanderJarod))
                                {
                                    if (l_Add->IsAIEnabled)
                                        l_Add->AI()->AttackStart(l_Jarod);
                                }
                            }
                        });

                        break;
                    }
                    case eData::DataRisenThreatCapture:
                    {
                        if (Creature* l_Corruptor = instance->GetCreature(m_DreadCorruptor))
                        {
                            if (l_Corruptor->IsAIEnabled)
                                l_Corruptor->AI()->DoAction(eActions::ActionStage3Triggered);
                        }

                        break;
                    }
                    case eData::DataRisenThreatEyes:
                    {
                        for (Position const& l_Pos : m_FlickeringEyes)
                        {
                            if (Creature* l_Eye = instance->SummonCreature(eCreatures::CreatureFlickeringEye, l_Pos))
                                m_EyesToKill.insert(l_Eye->GetGUID());
                        }

                        break;
                    }
                    case eData::DataRisenThreatTimer:
                    {
                        m_SavingTimer = p_Value;
                        break;
                    }
                    default:
                        break;
                }
            }

            void SetData64(uint32 p_ID, uint64 p_Value) override
            {
                switch (p_ID)
                {
                    case eData::DataRisenThreatEyes:
                    {
                        m_EyesToKill.insert(p_Value);
                        break;
                    }
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eData::DataPlayerClass:
                        return m_PlayerClass;
                    default:
                        return 0;
                }
            }

            uint64 GetData64(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eCreatures::CreatureJarodShadowsong:
                        return m_CommanderJarod;
                    case eCreatures::CreatureGrannyMarl:
                        return m_GrannyMarl;
                    case eCreatures::CreatureCallieCarrington:
                        return m_CallieCarrington;
                    case eCreatures::CreatureDreadCorruptor:
                        return m_DreadCorruptor;
                    case eGameObjects::GameObjectFirstDoor:
                    case eGameObjects::GameObjectDoorToThirdStage:
                    case eGameObjects::GameObjectDoorToThirdStage2:
                    {
                        auto const& l_Guid = m_DoorsGuids.find(p_ID);
                        if (l_Guid == m_DoorsGuids.end())
                            return 0;

                        return l_Guid->second;
                    }
                    case eData::DataPlayerGuid:
                        return m_PlayerGuid;
                    case eCreatures::CreatureLordErdrisThorn:
                        return m_LordErdrisGuid;
                    default:
                        break;
                }

                return 0;
            }

            void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
            {
                WorldSafeLocsEntry const* l_Loc = sWorldSafeLocsStore.LookupEntry(eData::DataResLocID);
                if (l_Loc == nullptr)
                    return;

                p_X     = l_Loc->x;
                p_Y     = l_Loc->y;
                p_Z     = l_Loc->z;
                p_O     = l_Loc->o;
                p_MapID = l_Loc->MapID;
            }

            bool PreventTeleportOutIfNeeded(Player* p_Player) const override
            {
                p_Player->TeleportTo(eData::DataResLocID);
                return true;
            }

            void Update(uint32 p_Diff) override
            {
                InstanceScript::Update(p_Diff);

                if (m_SavingTimer)
                {
                    if (m_SavingTimer <= p_Diff)
                    {
                        Wipe();

                        m_SavingTimer = 0;

                        return;
                    }
                    else
                        m_SavingTimer -= p_Diff;
                }

                /// Despawn all creatures after a wipe
                if (IsWipe())
                    Wipe();
            }

            bool IsWipe() override
            {
                /// Check live state of NPCs
                if (Creature* l_Jarod = instance->GetCreature(m_CommanderJarod))
                {
                    if (l_Jarod->isDead())
                        return true;
                }

                if (Creature* l_Callie = instance->GetCreature(m_CallieCarrington))
                {
                    if (l_Callie->isDead())
                        return true;
                }

                if (Creature* l_Granny = instance->GetCreature(m_GrannyMarl))
                {
                    if (l_Granny->isDead())
                        return true;
                }

                return false;
            }

            void Wipe()
            {
                auto l_WorldObjects = instance->GetObjectsOnMap();
                for (auto l_WorldObject = l_WorldObjects.begin(); l_WorldObject != l_WorldObjects.end(); l_WorldObject++)
                {
                    if ((*l_WorldObject)->GetTypeId() != TypeID::TYPEID_UNIT)
                        continue;

                    Creature* l_Creature = (*l_WorldObject)->ToCreature();
                    l_Creature->DespawnOrUnsummon();
                }
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* p_Map) const override
        {
            return new instance_end_of_the_risen_threat_InstanceMapScript(p_Map);
        }
};

class instance_the_highlords_return : public InstanceMapScript
{
    public:
        instance_the_highlords_return() : InstanceMapScript("instance_the_highlords_return", 1698) { }

        struct instance_the_highlords_return_InstanceMapScript : InstanceScript
        {
            instance_the_highlords_return_InstanceMapScript(Map* p_Map) : InstanceScript(p_Map) { }

            bool m_Wiped = false;

            bool m_Initialized = false;

            uint32 m_DungeonID = 0;

            uint32 m_PlayerClass = 0;
            uint64 m_PlayerGuid = 0;

            std::map<uint32, uint64> m_CreaturesGuids;

            void Initialize() override
            {
                m_Initialized   = false;

                m_DungeonID     = 0;

                m_PlayerClass   = 0;
            }

            void OnCreatureCreate(Creature* p_Creature) override
            {
                p_Creature->DisableHealthRegen();

                switch (p_Creature->GetEntry())
                {
                    case eCreatures::CreatureProphetVelen:
                    case eCreatures::CreatureKorvasBloodthorn:
                    {
                        p_Creature->SetVisible(false);

                        m_CreaturesGuids[p_Creature->GetEntry()] = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::CreatureHighlordKruul:
                    {
                        m_CreaturesGuids[p_Creature->GetEntry()] = p_Creature->GetGUID();

                        p_Creature->AddDelayedEvent([p_Creature]() -> void
                        {
                            p_Creature->SetHealth(p_Creature->CountPctFromMaxHealth(33.0f));
                        }, 1);

                        break;
                    }
                    case eCreatures::CreatureInquisitorVariss:
                    {
                        m_CreaturesGuids[p_Creature->GetEntry()] = p_Creature->GetGUID();
                        break;
                    }
                    case eCreatures::CreatureShadowSweeper:
                    {
                        p_Creature->SetReactState(ReactStates::REACT_PASSIVE);
                        break;
                    }
                    case eCreatures::CreatureEmpoweredDoomherald:
                    {
                        p_Creature->AddDelayedEvent([p_Creature]() -> void
                        {
                            p_Creature->CastSpell(p_Creature, eCommonSpells::SpellFelChannel);
                        }, 1);

                        break;
                    }
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* p_GameObject) override
            {
                switch (p_GameObject->GetEntry())
                {
                    case eGameObjects::GameObjectStatue:
                    {
                        p_GameObject->SetAIAnimKitId(eData::DataStatueAnimID);
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnPlayerEnter(Player* p_Player) override
            {
                InstanceScript::OnPlayerEnter(p_Player);

                if (!m_Initialized)
                {
                    m_Initialized = true;

                    GroupPtr l_Group = p_Player->GetGroup();

                    m_DungeonID = l_Group ? sLFGMgr->GetDungeon(l_Group->GetGUID()) : 0;

                    if (!instance->IsLFR())
                        m_DungeonID = 0;

                    m_PlayerClass = p_Player->getClass();
                    m_PlayerGuid = p_Player->GetGUID();
                }
            }

            void OnPlayerExit(Player* p_Player) override
            {
                InstanceScript::OnPlayerExit(p_Player);

                p_Player->ScheduleDelayedOperation(PlayerDelayedOperations::DELAYED_LEAVE_GROUP);
            }

            uint32 GetData(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eData::DataPlayerClass:
                        return m_PlayerClass;
                    default:
                        return 0;
                }
            }

            uint64 GetData64(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eData::DataPlayerGuid:
                        return m_PlayerGuid;
                    case eCreatures::CreatureProphetVelen:
                    case eCreatures::CreatureKorvasBloodthorn:
                    case eCreatures::CreatureHighlordKruul:
                    case eCreatures::CreatureInquisitorVariss:
                    {
                        auto const& l_Iter = m_CreaturesGuids.find(p_ID);
                        if (l_Iter == m_CreaturesGuids.end())
                            return 0;

                        return l_Iter->second;
                    }
                    default:
                        break;
                }

                return 0;
            }

            void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* /*p_Player*/) override
            {
                WorldSafeLocsEntry const* l_Loc = sWorldSafeLocsStore.LookupEntry(eData::DataResLocID);
                if (l_Loc == nullptr)
                    return;

                p_X     = l_Loc->x;
                p_Y     = l_Loc->y;
                p_Z     = l_Loc->z;
                p_O     = l_Loc->o;
                p_MapID = l_Loc->MapID;
            }

            bool PreventTeleportOutIfNeeded(Player* p_Player) const override
            {
                p_Player->TeleportTo(eData::DataResLocID);
                return true;
            }

            void Update(uint32 p_Diff) override
            {
                InstanceScript::Update(p_Diff);

                if (m_Wiped)
                    return;

                Player* l_Player = sObjectAccessor->FindPlayer(m_PlayerGuid);
                if (!l_Player || l_Player->GetMap() != instance)
                    return;

                if (l_Player->isAlive() && l_Player->m_positionZ <= 80.0f && l_Player->IsFalling())
                    l_Player->Kill(l_Player);

                /// NPC death - Stun everything, then teleports out
                if (IsWipe())
                {
                    m_Wiped = true;

                    l_Player->CastSpell(l_Player, eCommonSpells::SpellRestartNPCs, true);
                }
                /// Player death - Despawn everything, then teleports out
                else if (InstanceScript::IsWipe())
                {
                    m_Wiped = true;

                    auto l_WorldObjects = instance->GetObjectsOnMap();
                    for (auto l_WorldObject = l_WorldObjects.begin(); l_WorldObject != l_WorldObjects.end(); l_WorldObject++)
                    {
                        if ((*l_WorldObject)->GetTypeId() != TypeID::TYPEID_UNIT)
                            continue;

                        Creature* l_Creature = (*l_WorldObject)->ToCreature();
                        l_Creature->DespawnOrUnsummon();
                    }

                    l_Player->ResurrectPlayer(100.0f);
                    l_Player->CastSpell(l_Player, eCommonSpells::SpellRestartPlayer, true);
                }
            }

            bool IsWipe() override
            {
                /// Check live state of NPCs
                if (Creature* l_Velen = instance->GetCreature(GetData64(eCreatures::CreatureProphetVelen)))
                {
                    if (l_Velen->isDead())
                        return true;
                }

                if (Creature* l_Korvas = instance->GetCreature(GetData64(eCreatures::CreatureKorvasBloodthorn)))
                {
                    if (l_Korvas->isDead())
                        return true;
                }

                return false;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* p_Map) const override
        {
            return new instance_the_highlords_return_InstanceMapScript(p_Map);
        }
};

#ifndef __clang_analyzer__
void AddSC_instance_artifact_challenges()
{
    new instance_the_mage_hunter();
    new instance_feltotems_fall();
    new instance_the_archmages_reckoning();
    new instance_thwarting_the_twins();
    new instance_the_god_queens_fury();
    new instance_end_of_the_risen_threat();
    new instance_the_highlords_return();
}
#endif
