////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "SHA256.h"

#ifndef _WARDEN_KEY_GENERATION_H
#define _WARDEN_KEY_GENERATION_H

class SHA1Randx
{
public:
    SHA1Randx(uint8* buff, uint32 size)
    {
        uint32 taken = size/2;

        sh.Initialize();
        sh.UpdateData(buff, taken);
        sh.Finalize();

        memcpy(o1, sh.GetDigest(), SHA256_DIGEST_LENGTH);

        sh.Initialize();
        sh.UpdateData(buff + taken, size - taken);
        sh.Finalize();

        memcpy(o2, sh.GetDigest(), SHA256_DIGEST_LENGTH);

        memset(o0, 0x00, SHA256_DIGEST_LENGTH);

        FillUp();
    }

    void Generate(uint8* buf, uint32 sz)
    {
        for (uint32 i = 0; i < sz; ++i)
        {
            if (taken == SHA256_DIGEST_LENGTH)
                FillUp();

            buf[i] = o0[taken];
            taken++;
        }
    }

private:
    void FillUp()
    {
        sh.Initialize();
        sh.UpdateData(o1, SHA256_DIGEST_LENGTH);
        sh.UpdateData(o0, SHA256_DIGEST_LENGTH);
        sh.UpdateData(o2, SHA256_DIGEST_LENGTH);
        sh.Finalize();

        memcpy(o0, sh.GetDigest(), SHA256_DIGEST_LENGTH);

        taken = 0;
    }

    SHA256Hash sh;
    uint32 taken;
    uint8 o0[SHA256_DIGEST_LENGTH],o1[SHA256_DIGEST_LENGTH],o2[SHA256_DIGEST_LENGTH];
};

#endif
