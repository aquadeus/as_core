////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef SPELL_GENERIC_HPP
#define SPELL_GENERIC_HPP

class SummonHati : public BasicEvent
{
    public:
        SummonHati(uint64 p_GUID) : BasicEvent(), m_GUID(p_GUID) { }
        virtual ~SummonHati() {}

        enum eSpells
        {
            Stormbound          = 197388,
            StormboundBlack     = 211145,
            StormboundGray      = 211146,
            StormboundBrown     = 211147,
            StormboundDarkBlue  = 211148,
            TitanStrike         = 197344
        };

        const uint32 m_HatiEntries[5] =
        {
            100324,
            106548,
            106549,
            106550,
            106551
        };

        bool Execute(uint64 /*p_EndTime*/, uint32 /*p_Time*/);

    private:
        uint64 m_GUID;
};

#endif // SPELL_GENERIC_HPP