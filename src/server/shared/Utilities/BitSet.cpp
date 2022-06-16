////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "BitSet.hpp"

namespace MS { namespace Utilities
{
    /// Constructor
    BitSet::BitSet()
        : m_Bits(nullptr), m_Size(0)
    {

    }
    /// Destructor
    BitSet::~BitSet()
    {
        if (m_Bits)
        {
            delete[] m_Bits;
            m_Bits = nullptr;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Set bit set size in octet
    /// @p_Size : New size in octet
    void BitSet::SetSize(uint32 p_Size)
    {
        if (m_Bits)
        {
            delete[] m_Bits;
            m_Bits = nullptr;
        }

        m_Bits = new uint8_t[p_Size];
        memset(m_Bits, 0, p_Size);

        m_Size = p_Size;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Enable bit a position
    /// @p_Index : Bit position
    void BitSet::SetBit(uint32 p_Index)
    {
        ASSERT(p_Index < (8 * m_Size));
        m_Bits[p_Index / 8] |= (1 << (p_Index % 8));
    }
    /// Disable bit a position
    /// @p_Index : Bit position
    void BitSet::UnsetBit(uint32 p_Index)
    {
        ASSERT(p_Index < (8 * m_Size));
        m_Bits[p_Index / 8] &= 0xFF ^ (1 << (p_Index % 8));
    }
    /// Get bit at position
    /// @p_Index : Bit position
    bool BitSet::GetBit(uint32 p_Index) const
    {
        ASSERT(p_Index < (8 * m_Size));
        return m_Bits[p_Index / 8] & (1 << (p_Index % 8));
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    DynamicBitSet32::DynamicBitSet32()
    {

    }
    /// Destructor
    DynamicBitSet32::~DynamicBitSet32()
    {

    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Grow the bitset
    /// @p_Size : New size in dword
    void DynamicBitSet32::Grow(uint32 p_Size)
    {
        m_Bits.resize(p_Size + 1);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get current block size
    std::size_t DynamicBitSet32::GetNumBlocks() const
    {
        return m_Bits.size();
    }

    /// Get actually the max storable value according to the block count
    uint32 DynamicBitSet32::GetMaxStorableValue() const
    {
        return GetNumBlocks() * 32;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Enable bit a position
    /// @p_Index : Bit position
    void DynamicBitSet32::SetBit(uint32 p_Index)
    {
        if (p_Index > (32 * m_Bits.size()))
            Grow((p_Index / 32) + 1);

        m_Bits[p_Index / 32] |= (1 << (p_Index % 32));
    }
    /// Disable bit a position
    /// @p_Index : Bit position
    void DynamicBitSet32::UnsetBit(uint32 p_Index)
    {
        if (p_Index > (32 * m_Bits.size()))
            return;

        m_Bits[p_Index / 32] &= 0xFFFFFFFF ^ (1 << (p_Index % 32));
    }
    /// Get bit at position
    /// @p_Index : Bit position
    bool DynamicBitSet32::GetBit(uint32 p_Index) const
    {
        if (p_Index > (32 * m_Bits.size()))
            return false;

        return m_Bits[p_Index / 32] & (1 << (p_Index % 32));
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Iterate over each bit value
    /// @p_Functor : Value callback
    void DynamicBitSet32::ForEachBlock(std::function<void(uint32 /*p_Value*/)> p_Functor)
    {
        for (uint32 l_I = 0; l_I < m_Bits.size(); ++l_I)
        {
            p_Functor(m_Bits[l_I]);
        }
    }
    /// Set raw block
    /// @p_Index        : Block index
    /// @p_BlockValue   : Block value
    void DynamicBitSet32::SetBlock(uint32 p_Index, uint32 p_BlockValue)
    {
        if (p_Index >= m_Bits.size())
            Grow(p_Index);

        m_Bits[p_Index] = p_BlockValue;
    }

}   ///< namespace Utilities
}   ///< namespace MS
