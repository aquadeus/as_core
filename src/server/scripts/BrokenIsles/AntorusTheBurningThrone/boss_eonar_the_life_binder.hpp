////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "antorus_the_burning_throne.hpp"
#include "HelperDefines.h"

struct WavePattern
{
    uint32  Timer;
    uint8   EventID;
};

///             DiffID  Pattern
static std::map<uint32, std::vector<WavePattern>> g_WavePattern =
{
    {
        Difficulty::DifficultyRaidNormal,
        {
            { 5 * TimeConstants::IN_MILLISECONDS,   eCosmeticEvents::EonarWaveNormalFirst   },
            { 30 * TimeConstants::IN_MILLISECONDS,  eCosmeticEvents::EonarWaveNormalSecond  },
            { 64 * TimeConstants::IN_MILLISECONDS,  eCosmeticEvents::EonarWaveNormalThird   },
            { 110 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveNormalFourth  },
            { 115 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveNormalFifth   },
            { 170 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveNormalSixth   },
            { 180 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveNormalSeventh },
            { 205 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveNormalEighth  },
            { 230 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveNormalNinth   },
            { 305 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveNormalTenth   }
        }
    },
    {
        Difficulty::DifficultyRaidHeroic,
        {
            { 5 * TimeConstants::IN_MILLISECONDS,   eCosmeticEvents::EonarWaveHeroicFirst       },
            { 30 * TimeConstants::IN_MILLISECONDS,  eCosmeticEvents::EonarWaveHeroicSecond      },
            { 70 * TimeConstants::IN_MILLISECONDS,  eCosmeticEvents::EonarWaveHeroicThird       },
            { 110 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveHeroicFourth      },
            { 150 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveHeroicFifth       },
            { 185 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveHeroicSixth       },
            { 195 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveHeroicSeventh     },
            { 220 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveHeroicEighth      },
            { 275 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveHeroicNinth       },
            { 315 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveHeroicTenth       },
            { 320 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveHeroicEleventh    }
        }
    },
    {
        Difficulty::DifficultyRaidMythic,
        {
            { 5 * TimeConstants::IN_MILLISECONDS,   eCosmeticEvents::EonarWaveMythic1   },
            { 40 * TimeConstants::IN_MILLISECONDS,  eCosmeticEvents::EonarWaveMythic2   },
            { 65 * TimeConstants::IN_MILLISECONDS,  eCosmeticEvents::EonarWaveMythic3   },
            { 70 * TimeConstants::IN_MILLISECONDS,  eCosmeticEvents::EonarWaveMythic4   },
            { 115 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveMythic5   },
            { 135 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveMythic6   },
            { 165 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveMythic7   },
            { 175 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveMythic8   },
            { 210 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveMythic9   },
            { 265 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveMythic10  },
            { 285 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveMythic11  },
            { 305 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveMythic12  },
            { 335 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveMythic13  },
            { 360 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveMythic14  },
            { 410 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveMythic15  },
            { 430 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveMythic16  },
            { 450 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveMythic17  }
        }
    },
    {
        Difficulty::DifficultyRaidLFR,
        {
            { 5 * TimeConstants::IN_MILLISECONDS,   eCosmeticEvents::EonarWaveNormalFirst   },
            { 30 * TimeConstants::IN_MILLISECONDS,  eCosmeticEvents::EonarWaveNormalSecond  },
            { 64 * TimeConstants::IN_MILLISECONDS,  eCosmeticEvents::EonarWaveNormalThird   },
            { 110 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveNormalFourth  },
            { 115 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveNormalFifth   },
            { 170 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveNormalSixth   },
            { 180 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveNormalSeventh },
            { 205 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveNormalEighth  },
            { 230 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveNormalNinth   },
            { 305 * TimeConstants::IN_MILLISECONDS, eCosmeticEvents::EonarWaveNormalTenth   }
        }
    }
};
