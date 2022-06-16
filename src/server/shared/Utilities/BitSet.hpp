////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef BITSET_HPP_UTILITIES
# define BITSET_HPP_UTILITIES

# include "Common.h"

namespace MS { namespace Utilities
{
    /// Bit set manipulation helper
    class BitSet
    {
        public:
            /// Constructor
            BitSet();
            /// Destructor
            ~BitSet();

            /// Set bit set size in octet
            /// @p_Size : New size in octet
            void SetSize(uint32 p_Size);

            /// Enable bit a position
            /// @p_Index : Bit position
            void SetBit(uint32 p_Index);
            /// Disable bit a position
            /// @p_Index : Bit position
            void UnsetBit(uint32 p_Index);

            /// get bit at position
            /// @p_Index : Bit position
            bool GetBit(uint32 p_Index) const;

            /// Append this bit set to a packet
            /// @p_Buffer : Destination buffer
            template<class T> void AppendToByteBuffer(T * p_Buffer)
            {
                p_Buffer->append(m_Bits, m_Size);
            }

        private:
            uint8 * m_Bits; ///< Bits array
            uint32  m_Size; ///< Bit set size

    };

    /// Bit set manipulation helper
    class DynamicBitSet32
    {
        public:
            /// Constructor
            DynamicBitSet32();
            /// Destructor
            ~DynamicBitSet32();

            /// Grow the bitset
            /// @p_Size : New size in dword
            void Grow(uint32 p_Size);

            /// Get current block size
            std::size_t GetNumBlocks() const;
            /// Get actually the max storable value according to the block count
            uint32 GetMaxStorableValue() const;

            /// Enable bit a position
            /// @p_Index : Bit position
            void SetBit(uint32 p_Index);
            /// Disable bit a position
            /// @p_Index : Bit position
            void UnsetBit(uint32 p_Index);

            /// Get bit at position
            /// @p_Index : Bit position
            bool GetBit(uint32 p_Index) const;

            /// Iterate over each bit value
            /// @p_Functor : Value callback
            void ForEachBlock(std::function<void(uint32 /*p_Value*/)> p_Functor);
            /// Set raw block
            /// @p_Index        : Block index
            /// @p_BlockValue   : Block value
            void SetBlock(uint32 p_Index, uint32 p_BlockValue);

        private:
            std::vector<uint32> m_Bits; ///< Bits array

    };

}   ///< namespace Utilities
}   ///< namespace MS

#endif  ///< BITSET_HPP_UTILITIES
