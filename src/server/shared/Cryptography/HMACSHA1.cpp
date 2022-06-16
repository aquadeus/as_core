////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "HMACSHA1.h"
#include "BigNumber.h"
#include "Common.h"
#include "BigNumber.h"

#if defined(OPENSSL_VERSION_NUMBER) && OPENSSL_VERSION_NUMBER < 0x10100000L
HMAC_CTX* HMAC_CTX_new()
{
    HMAC_CTX *ctx = new HMAC_CTX();
    HMAC_CTX_init(ctx);
    return ctx;
}

void HMAC_CTX_free(HMAC_CTX* ctx)
{
    HMAC_CTX_cleanup(ctx);
    delete ctx;
}
#endif

HmacHash::HmacHash(uint32 len, uint8 const* seed) : m_ctx(HMAC_CTX_new())
{
    HMAC_Init_ex(m_ctx, seed, len, EVP_sha1(), nullptr);
}

HmacHash::~HmacHash()
{
    HMAC_CTX_free(m_ctx);
}

void HmacHash::UpdateData(std::string const& str)
{
    HMAC_Update(m_ctx, reinterpret_cast<uint8 const*>(str.c_str()), str.length());
}

void HmacHash::UpdateData(uint8 const* data, size_t len)
{
    HMAC_Update(m_ctx, data, len);
}

void HmacHash::Finalize()
{
    uint32 length = 0;
    HMAC_Final(m_ctx, m_digest, &length);
    ASSERT(length == SHA_DIGEST_LENGTH);
}

uint8 *HmacHash::ComputeHash(BigNumber* bn)
{
    HMAC_Update(m_ctx, bn->AsByteArray(), bn->GetNumBytes());
    Finalize();
    return m_digest;
}

//////////////////////////////////////////////////////////////////////////

HmacHash256::HmacHash256(uint32 len, uint8 const* seed) : m_ctx(HMAC_CTX_new())
{
    HMAC_Init_ex(m_ctx, seed, len, EVP_sha256(), nullptr);
}

HmacHash256::~HmacHash256()
{
    HMAC_CTX_free(m_ctx);
}

void HmacHash256::UpdateData(std::string const& str)
{
    HMAC_Update(m_ctx, reinterpret_cast<uint8 const*>(str.c_str()), str.length());
}

void HmacHash256::UpdateData(uint8 const* data, size_t len)
{
    HMAC_Update(m_ctx, data, len);
}

void HmacHash256::Finalize()
{
    uint32 length = 0;
    HMAC_Final(m_ctx, m_digest, &length);
    ASSERT(length == SHA256_DIGEST_LENGTH);
}

uint8 *HmacHash256::ComputeHash(BigNumber* bn)
{
    HMAC_Update(m_ctx, bn->AsByteArray(), bn->GetNumBytes());
    Finalize();
    return m_digest;
}

uint8 *HmacHash256::ComputeHash(uint8 *seed, uint32 len)
{
    HMAC_Update(m_ctx, seed, len);
    Finalize();
    return m_digest;
}