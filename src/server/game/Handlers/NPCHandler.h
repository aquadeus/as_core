////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __NPCHANDLER_H
#define __NPCHANDLER_H

struct QEmote
{
    uint32 _Emote;
    uint32 _Delay;
};

#define MAX_GOSSIP_TEXT_EMOTES 3
#define MAX_GOSSIP_TEXT_OPTIONS 8

struct GossipText
{
    uint32 SoundID;
    std::string Text_0;
    std::string Text_1;
    uint32 Language;
    QEmote Emotes[MAX_GOSSIP_TEXT_EMOTES];

    uint32 BroadcastTextID[MAX_GOSSIP_TEXT_OPTIONS];
    float Probability[MAX_GOSSIP_TEXT_OPTIONS];
};

struct PageTextLocale
{
    StringVector Text;
};

struct NpcTextLocale
{
    NpcTextLocale() { Text_0.resize(MAX_LOCALES); Text_1.resize(MAX_LOCALES); }

    std::vector<StringVector> Text_0;
    std::vector<StringVector> Text_1;
};
#endif

