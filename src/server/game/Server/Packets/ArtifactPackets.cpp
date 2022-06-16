////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ArtifactPackets.h"

void WorldPackets::Artifact::ConfirmRespec::Read()
{
    _worldPacket.readPackGUID(ArtifactGUID);
    _worldPacket.readPackGUID(NpcGUID);
}

WorldPacket const* WorldPackets::Artifact::RespecResult::Write()
{
    _worldPacket.appendPackGUID(ArtifactGUID);
    _worldPacket.appendPackGUID(NpcGUID);

    return &_worldPacket;
}
