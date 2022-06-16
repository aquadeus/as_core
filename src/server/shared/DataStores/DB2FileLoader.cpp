////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "DB2FileLoader.h"
#include "DatabaseEnv.h"
#include <fstream>

#define READ_UINT(a) if (fread(&a, sizeof(a), 1, f) != 1) { fclose(f); return false; }// EndianConvert(a);
#define READ_BYTES(a, c) if (fread(a, c, 1, f) != 1) { fclose(f); return false; } //EndianConvert(a);

FieldStorageInfo m_DefaultStorage = { m_DefaultStorage.FieldOffsetBits = 0, m_DefaultStorage.FieldSizeBits = 32,  m_DefaultStorage.AdditionalDataSize = 0,  m_DefaultStorage.CompressionType = FieldCompressionNone };

DB2FileLoader::DB2FileLoader()
{
    m_Data = nullptr;
    m_Format = nullptr;
    m_StringTable = nullptr;
    m_CommonData = nullptr;
    m_PalletData = nullptr;
    m_NonSparseFieldCache = nullptr;
    m_StorageInfo = nullptr;
    m_FieldCountIncludingArrayMemebers = 0;
    m_RelationshipInformation.NumRecords = 0;
    m_RelationshipInformation.MaxID = 0;
    m_RelationshipInformation.MinID = 0;
    m_Filename = "";
}


int GetNumberOfDigits(int p_Num)
{
    return p_Num > 0 ? static_cast<int>(log10(static_cast<double>(p_Num))) + 1 : 1;
}

bool DB2FileLoader::Load(const char* filename, const char* fmt)
{
    m_Filename = filename;
    const char l_Version[] = "WDC1";

    char m_Header[4];
    m_Flags = 0;
    m_Format = fmt;

    if (m_Data)
    {
        for (uint32 l_I = 0; l_I < m_RecordsCount; ++l_I)
        {
            if (m_Data[l_I])
            {
                delete[] m_Data[l_I];
            }
        }

        delete[] m_Data;
    }

    if (m_StringTable)
    {
        delete[] m_StringTable;
    }

    FILE * f = fopen(filename, "rb");
    if (!f)
        return false;

    fseek(f, 0, SEEK_END);
    int l_FileSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    READ_BYTES(m_Header, 4);

    if (strncmp(l_Version, m_Header, 4))
    {
        fclose(f);
        return false;
    }

    READ_UINT(m_RecordsCount);
    READ_UINT(m_FieldsCount);
    READ_UINT(m_RecordSize);
    READ_UINT(m_StringTableSize);
    READ_UINT(m_TableHash);
    READ_UINT(m_Build);
    READ_UINT(m_MinID);
    READ_UINT(m_MaxID);
    READ_UINT(m_Locale);
    READ_UINT(m_DuplicatesSize);
    READ_UINT(m_Flags);
    READ_UINT(m_IDIndex);
    READ_UINT(m_TotalFieldsCount);
    READ_UINT(m_BitPackedDataOffset);
    READ_UINT(m_LookupCollumnCount);
    READ_UINT(m_OffsetMapOffset);
    READ_UINT(m_IDListSize);
    READ_UINT(m_FieldStorageInfoSize);
    READ_UINT(m_CommonFieldSize);
    READ_UINT(m_PalletDataSize);
    READ_UINT(m_RelationshipDataSize);

    m_FieldInfoMap.resize(m_TotalFieldsCount + ((m_Flags & 4) ? 1 : 0));

    /// Add Index into the map
    if (m_Flags & 4)
        m_FieldInfoMap[0] = {0, 32};

    for (uint16 l_I = 0; l_I < m_TotalFieldsCount; ++l_I)
    {
        uint16 l_Size;
        uint16 l_Offset;
        READ_UINT(l_Size);
        READ_UINT(l_Offset);

        l_Size = 32 - l_Size;

        m_FieldInfoMap[l_I + (m_Flags & 4u ? 1 : 0)] = {l_Offset + (m_Flags & 4u), l_Size};
    }

    std::unordered_map<uint32, uint32> l_LenByName;
    int32 l_RowCount = (m_RecordsCount + m_DuplicatesSize / 8);

    std::unordered_map<uint32, uint32> m_References;

    if (m_Flags & 1u)
    {
        m_SparseCache.resize(l_RowCount);

        size_t l_ReadPos = ftell(f);
        fseek(f, m_OffsetMapOffset, SEEK_SET);

        #pragma pack(push, 1)
        struct OffsetSize
        {
            uint32 m_Offset;
            uint16 m_Size;
        };
        #pragma pack(pop)

        std::vector<OffsetSize> l_OffsetSizes;
        l_OffsetSizes.resize(m_MaxID - m_MinID + 1);

        READ_BYTES(l_OffsetSizes.data(), (m_MaxID - m_MinID + 1) * sizeof(OffsetSize));


        m_Data = new uint8*[l_RowCount];
        memset(m_Data, 0, l_RowCount * sizeof(uint8*));

        int l_Res = ftell(f);

        int l_Counter = 0;
        std::unordered_set<uint32> l_Duplicates;

        for (auto const& l_Itr : l_OffsetSizes)
        {
            if (!l_Itr.m_Size || !l_Itr.m_Offset)
                continue;

            if (l_Duplicates.find(l_Itr.m_Offset) != l_Duplicates.end())
                continue;

            l_Duplicates.insert({l_Itr.m_Offset});

            fseek(f, l_Itr.m_Offset, SEEK_SET);
            uint32 l_FieldSize = l_Itr.m_Size;
            l_LenByName.insert({l_Counter, l_FieldSize});

            uint8* l_Data = new uint8[l_FieldSize + ((m_Flags & 4) ? 4 : 0)];
            uint8* l_Ptr = l_Data + ((m_Flags & 4) ? 4 : 0);

            READ_BYTES(l_Ptr, l_FieldSize);

            m_Data[l_Counter++] = l_Data;
        }

        fseek(f, l_Res, SEEK_SET);
        m_StringTable = nullptr;
    }
    else
    {
        m_Data = new uint8*[l_RowCount];
        memset(m_Data, 0, l_RowCount * sizeof(uint8*));

        for (uint32 l_I = 0; l_I < m_RecordsCount; ++l_I)
        {
            uint8* l_Data = new uint8[m_RecordSize + (m_Flags & 4u ? 4 : 0)];
            READ_BYTES(l_Data + (m_Flags & 4u ? 4 : 0), m_RecordSize);
            m_Data[l_I] = l_Data;
        }

        /// String table
        m_StringTable = new uint8[m_StringTableSize];

        if (m_StringTableSize)
            READ_BYTES(m_StringTable, m_StringTableSize);
    }

    uint32 l_IDCount = m_IDListSize / 4;
    /// Read Indexes - flag 4 only required if inside the structure
    for (uint32 l_I = 0; l_I < l_IDCount; ++l_I)
    {
        uint32 l_ID;
        READ_UINT(l_ID);

        if (m_Flags & 4)
            *reinterpret_cast<uint32*>(m_Data[l_I]) = l_ID;

        m_References.insert({l_ID, l_I});
    }

    /// Read referenced duplicates
    uint32 l_Idx = 0;

    for (uint32 l_I = 0; l_I < m_DuplicatesSize / 8; ++l_I)
    {
        uint32 l_Id;
        uint32 l_ReferenceId;

        READ_UINT(l_Id);
        READ_UINT(l_ReferenceId);

        if (m_References.find(l_ReferenceId) == m_References.end())
            continue;

        uint32 l_RefferenceIndex = m_References[l_ReferenceId];

        if (l_RefferenceIndex >= m_RecordsCount + l_Idx)
            continue;

        auto l_Itr = l_LenByName.find(l_RefferenceIndex);
        uint32 l_ThisSize = ((l_Itr != l_LenByName.end() ? l_Itr->second : m_RecordSize) + ((m_Flags & 4) ? 4 : 0));

        m_Data[m_RecordsCount + l_Idx] = new uint8[l_ThisSize];

        memcpy(m_Data[m_RecordsCount + l_Idx],  m_Data[l_RefferenceIndex], l_ThisSize);
        *reinterpret_cast<uint32*>(m_Data[m_RecordsCount + l_Idx]) = l_Id;
        ++l_Idx;
    }

    if (m_Flags & 4)
    {
        m_RecordSize += 4;
        ++m_FieldsCount;
        ++m_TotalFieldsCount;
    }

    m_RecordsCount += l_Idx;

    m_StorageInfo =  std::make_unique<FieldStorageInfo[]>(m_FieldStorageInfoSize / sizeof(FieldStorageInfo) + (m_Flags & 4 ? 1 : 0));

    if (m_FieldStorageInfoSize)
        READ_BYTES(m_StorageInfo.get() + (m_Flags & 4 ? 1 : 0), m_FieldStorageInfoSize);

    if (m_Flags & 4)
    {
        memcpy(m_StorageInfo.get(), &m_DefaultStorage, sizeof(FieldStorageInfo));

        for (int l_I = 1; l_I < m_FieldStorageInfoSize / sizeof(FieldStorageInfo) + 1; ++l_I)
        {
            auto& l_Entry = m_StorageInfo[l_I];
            l_Entry.FieldOffsetBits += 32;
        }
    }

    if  (m_PalletDataSize)
    {
        m_PalletData = new char[m_PalletDataSize];
        READ_BYTES(m_PalletData, m_PalletDataSize);
    }

    if (m_CommonFieldSize)
    {
        m_CommonData = new char[m_CommonFieldSize];
        READ_BYTES(m_CommonData, m_CommonFieldSize);
    }

    if (m_RelationshipDataSize)
    {
        READ_UINT(m_RelationshipInformation.NumRecords);
        READ_UINT(m_RelationshipInformation.MinID);
        READ_UINT(m_RelationshipInformation.MaxID);

        m_RelationshipInformation.Relationships.resize(m_RelationshipInformation.NumRecords);
        READ_BYTES(m_RelationshipInformation.Relationships.data(), m_RelationshipInformation.NumRecords * sizeof (RelationshipInformation::RelationshipEntry));
    }

    if (m_RecordsCount)
    {
        uint32 l_ArrayLen = 1;
        uint32 l_Index = 0;
        bool l_WasRelationshipFieldFound = false;

        for (uint32 l_X = 0; fmt[l_X]; ++l_X)
        {
            if (isdigit(fmt[l_X]))
            {
                l_ArrayLen = atoi(fmt + l_X);
                l_X += (GetNumberOfDigits(l_ArrayLen) - 1);
                continue;
            }

            if (IsRelationshipField(fmt[l_X]))
            {
                l_WasRelationshipFieldFound = true;
                break;
            }

            uint32 l_RecSize = 0;

            switch (fmt[l_X])
            {
                case FT_QWORD:
                    l_RecSize = 64 * l_ArrayLen;
                    break;
                case FT_NSTRING:
                case FT_STRING:
                case FT_FLOAT:
                case FT_INT:
                case FT_INDEX:
                    l_RecSize = 32 * l_ArrayLen;
                    break;
                case FT_SHORT:
                case FT_WORD_INDEX:
                    l_RecSize = 16 * l_ArrayLen;
                    break;
                case FT_BYTE:
                case FT_BYTE_INDEX:
                    l_RecSize = 8 * l_ArrayLen;
            }

            auto& l_Entry = m_StorageInfo[l_Index++];
            if (l_RecSize < l_Entry.FieldSizeBits)
            {
                printf("Corrupted structure at %s\n", filename);
                return false;
            }

            l_ArrayLen = 1;
        }

        if (l_WasRelationshipFieldFound && !m_RelationshipInformation.NumRecords)
        {
            printf("Relationship detected in structure, but DB2 has no relationship information!\n");
            return false;
        }
    }

    uint32 l_Size = ftell(f);

    fclose(f);

    m_FieldCountIncludingArrayMemebers = 0;

    int32 l_ArrayLen = 1;
    for (uint32 l_I = 0; fmt[l_I]; ++l_I)
    {
        if (isdigit(fmt[l_I]))
        {
            l_ArrayLen = atoi(fmt + l_I);
            l_I += (GetNumberOfDigits(l_ArrayLen) - 1);
            continue;
        }

        m_FieldCountIncludingArrayMemebers += l_ArrayLen;
        l_ArrayLen = 1;
    }

    PopulateNonSparseCache(fmt);
    return l_Size == l_FileSize;
}

void DB2FileLoader::PopulateNonSparseCache(char const* p_Fmt)
{
    // SPARSE
    if (m_Flags & 1)
        return;

    int32 l_RecordSize = 0;
    int32 l_Index = 0;
    int32 l_ArrayLen = 1;

    m_NonSparseFieldCache = std::make_unique<FieldCache[]>(m_FieldCountIncludingArrayMemebers);
    m_PalletSizes = std::make_unique<uint32[]>(m_FieldCountIncludingArrayMemebers);

    uint32 l_PreIndexSize = 0;

    int32 l_NonArrayIndex = 0;

    for (uint32 l_I = 0; p_Fmt[l_I]; ++l_I)
    {
        if (IsRelationshipField(p_Fmt[l_I]))
            break;

        if (isdigit(p_Fmt[l_I]))
        {
            l_ArrayLen = atoi(p_Fmt + l_I);
            l_I += (GetNumberOfDigits(l_ArrayLen) - 1);
            continue;
        }

        int32 l_ThisLen = GetLenAtIndex(l_Index);

        for (int l_Y = 0; l_Y < l_ArrayLen; ++l_Y)
        {
            m_PalletSizes[l_NonArrayIndex] = l_PreIndexSize;
            m_NonSparseFieldCache[l_NonArrayIndex++] = std::make_tuple(l_RecordSize, l_ThisLen, m_StorageInfo.get() + l_Index);
            l_RecordSize += l_ThisLen;
        }

        if (m_StorageInfo[l_Index].CompressionType == FieldCompressionBitpackedIndexed || m_StorageInfo[l_Index].CompressionType == FieldCompressionBitpackedIndexedArray)
            l_PreIndexSize += m_StorageInfo[l_Index].AdditionalDataSize;

        ++l_Index;
        l_ArrayLen = 1;
    }
}

int32 DB2FileLoader::Record::GetOffset(int32 p_FieldIndex, uint32& p_Size, FieldStorageInfo*& p_FieldInfo) const
{
    p_FieldInfo = std::get<2>(m_Cache[p_FieldIndex]);
    p_Size = std::get<1>(m_Cache[p_FieldIndex]);
    return std::get<0>(m_Cache[p_FieldIndex]);
}

int32 DB2FileLoader::Record::GetOffset(int32 p_FieldIndex) const
{
    return std::get<0>(m_Cache[p_FieldIndex]);
}

template void DB2FileLoader::Record::ReadArray<uint64>(uint64*, size_t, int32) const;
template void DB2FileLoader::Record::ReadArray<uint32>(uint32*, size_t, int32) const;
template void DB2FileLoader::Record::ReadArray<uint16>(uint16*, size_t, int32) const;
template void DB2FileLoader::Record::ReadArray<uint8>(uint8*, size_t, int32) const;

template <typename T>
void DB2FileLoader::Record::ReadArray(T* p_Beginning, size_t p_Field, int32 p_ArraySize) const
{
    uint32 l_Size;
    FieldStorageInfo* l_FieldInfo;
    uint32 l_SubOffset = GetOffset(p_Field, l_Size, l_FieldInfo);

    switch (l_FieldInfo->CompressionType)
    {
        /// Not expected having an array
        case FieldCompressionBitpacked:
        {
            uint64 l_Value = 0;
            int l_ByteCount = (l_FieldInfo->FieldSizeBits + (l_FieldInfo->FieldOffsetBits & 7) + 7) / 8;

            l_SubOffset = l_FieldInfo->FieldOffsetBits / 8;
            memcpy(&l_Value, offset + l_SubOffset, l_ByteCount);
            l_Value = (l_Value >> (l_FieldInfo->FieldOffsetBits & 7)) & (1 << l_FieldInfo->FieldSizeBits) - 1;

            *p_Beginning = l_Value;
            break;
        }
        case FieldCompressionCommonData:
        {
            for (int l_I = 0; l_I < p_ArraySize; ++l_I)
                p_Beginning[l_I] = l_FieldInfo->Data.FieldCompressionComonData.DefaultValue;

            break;
        }
        case FieldCompressionBitpackedIndexed:
        {
            uint32 l_AdditionalSizeOffset = file.m_PalletSizes[p_Field];

            uint64 l_Index = 0;
            int l_ByteCount = (l_FieldInfo->FieldSizeBits + (l_FieldInfo->FieldOffsetBits & 7) + 7) / 8;

            l_SubOffset = l_FieldInfo->FieldOffsetBits / 8;
            memcpy(&l_Index, offset + l_SubOffset, l_ByteCount);
            l_Index = (l_Index >> (l_FieldInfo->FieldOffsetBits & 7)) & (1 << l_FieldInfo->FieldSizeBits) - 1;

            *p_Beginning = reinterpret_cast<uint32*>(file.m_PalletData + l_AdditionalSizeOffset)[l_Index];
            break;
        }
        case FieldCompressionBitpackedIndexedArray:
        {
            uint32 l_AdditionalSizeOffset = file.m_PalletSizes[p_Field];

            uint64 l_Index = 0;
            int l_ByteCount = (l_FieldInfo->FieldSizeBits + (l_FieldInfo->FieldOffsetBits & 7) + 7) / 8;

            l_SubOffset = l_FieldInfo->FieldOffsetBits / 8;
            memcpy(&l_Index, offset + l_SubOffset, l_ByteCount);
            l_Index = (l_Index >> (l_FieldInfo->FieldOffsetBits & 7)) & (1 << l_FieldInfo->FieldSizeBits) - 1;

            for (int l_I = 0; l_I < l_FieldInfo->Data.FieldCompressionBitpackedIndexArray.ArrayCount; ++l_I)
                p_Beginning[l_I] = reinterpret_cast<uint32*>(file.m_PalletData + l_AdditionalSizeOffset)[l_Index * l_FieldInfo->Data.FieldCompressionBitpackedIndexArray.ArrayCount  + l_I];

            break;
        }
        default:
        {
            if (l_Size == sizeof(T))
            {
                memcpy(p_Beginning, offset + l_SubOffset, p_ArraySize * sizeof(T));
                return;
            }

            memset(p_Beginning, 0, p_ArraySize * sizeof(T));

            for (int l_I = 0; l_I < p_ArraySize; ++l_I)
            {
                memcpy(p_Beginning + l_I, offset + (l_SubOffset + l_I * l_Size), l_Size);
            }
        }
    }
}

template <>
void DB2FileLoader::Record::ReadArray(float* p_Beginning, size_t p_Field, int32 p_ArraySize) const
{
    uint32 l_Size;
    FieldStorageInfo* l_FieldInfo;
    uint32 l_SubOffset = GetOffset(p_Field, l_Size, l_FieldInfo);
    switch (l_FieldInfo->CompressionType)
    {
        case FieldCompressionCommonData:
        {
            for (int l_I = 0; l_I < p_ArraySize; ++l_I)
                memcpy(p_Beginning + l_I, &l_FieldInfo->Data.FieldCompressionComonData.DefaultValue, sizeof(float));

            break;
        }
        case FieldCompressionBitpackedIndexed:
        {
            uint32 l_AdditionalSizeOffset = file.m_PalletSizes[p_Field];

            uint64 l_Index = 0;
            int l_ByteCount = (l_FieldInfo->FieldSizeBits + (l_FieldInfo->FieldOffsetBits & 7) + 7) / 8;

            l_SubOffset = l_FieldInfo->FieldOffsetBits / 8;
            memcpy(&l_Index, offset + l_SubOffset, l_ByteCount);
            l_Index = (l_Index >> (l_FieldInfo->FieldOffsetBits & 7)) & (1 << l_FieldInfo->FieldSizeBits) - 1;

            *p_Beginning = reinterpret_cast<float*>(file.m_PalletData + l_AdditionalSizeOffset)[l_Index];
            break;
        }
        case FieldCompressionBitpackedIndexedArray:
        {
            uint32 l_AdditionalSizeOffset = file.m_PalletSizes[p_Field];

            uint64 l_Index = 0;
            int l_ByteCount = (l_FieldInfo->FieldSizeBits + (l_FieldInfo->FieldOffsetBits & 7) + 7) / 8;

            l_SubOffset = l_FieldInfo->FieldOffsetBits / 8;
            memcpy(&l_Index, offset + l_SubOffset, l_ByteCount);
            l_Index = (l_Index >> (l_FieldInfo->FieldOffsetBits & 7)) & (1 << l_FieldInfo->FieldSizeBits) - 1;

            for (int l_I = 0; l_I < l_FieldInfo->Data.FieldCompressionBitpackedIndexArray.ArrayCount; ++l_I)
                p_Beginning[l_I] = reinterpret_cast<float*>(file.m_PalletData + l_AdditionalSizeOffset)[l_Index * l_FieldInfo->Data.FieldCompressionBitpackedIndexArray.ArrayCount  + l_I];

            break;
        }
        default:
        {
            if (l_Size == sizeof(float))
            {
                memcpy(p_Beginning, offset + l_SubOffset, p_ArraySize * sizeof(float));
                return;
            }

            memset(p_Beginning, 0, p_ArraySize * sizeof(float));

            for (int l_I = 0; l_I < p_ArraySize; ++l_I)
            {
                memcpy(p_Beginning + l_I, offset + (l_SubOffset + l_I * l_Size), l_Size);
            }
        }
    }
}

DB2FileLoader::~DB2FileLoader()
{
    if (m_Data)
    {
        for (uint32 l_I = 0; l_I < m_RecordsCount; ++l_I)
        {
            if (m_Data[l_I])
            {
                delete[] m_Data[l_I];
            }
        }

        delete []m_Data;
    }

    if (m_StringTable)
        delete[] m_StringTable;

    if (m_PalletData)
        delete[] m_PalletData;

    if (m_CommonData)
        delete[] m_CommonData;
}

DB2FileLoader::Record::Record(DB2FileLoader& file_, unsigned char* offset_, uint32 p_ID, std::unique_ptr<FieldCache[]> const& p_Cache) :  offset(offset_), file(file_), m_ID(p_ID), m_Cache(p_Cache)
{
}

DB2FileLoader::Record DB2FileLoader::getRecord(size_t id)
{
    assert(m_Data);

    if (m_Flags & 1 && !m_SparseCache[id])
        GenerateSparseCache(id);

    return Record(*this, m_Data[id], id, m_Flags & 1 ? m_SparseCache[id] : m_NonSparseFieldCache);
}

uint32 DB2FileLoader::GetFormatRecordSizeForDB2Loader(const char* format)
{
    uint32 l_RecSize = 0;
    uint32 l_ArrayLen = 1;

    for (uint32 x=0; format[x]; ++x)
    {
        if (isdigit(format[x]))
        {
            l_ArrayLen = atoi(format + x);
            x += (GetNumberOfDigits(l_ArrayLen) - 1);
            continue;
        }

        switch (format[x])
        {
            case FT_QWORD:
                l_RecSize += 8 * l_ArrayLen;
                break;
            case FT_NSTRING:
            case FT_STRING:
                l_RecSize += sizeof(LocalizedString*) * l_ArrayLen;
                break;
            case FT_FLOAT:
            case FT_INT:
            case FT_INT_RELATIONSHIP:
            case FT_INDEX:
                l_RecSize += 4 * l_ArrayLen;
                break;
            case FT_SHORT:
            case FT_WORD_INDEX:
            case FT_WORD_RELATIONSHIP:
                l_RecSize += 2 * l_ArrayLen;
                break;
            case FT_BYTE:
            case FT_BYTE_INDEX:
            case FT_BYTE_RELATIONSHIP:
                l_RecSize += 1 * l_ArrayLen;
                break;
            case FT_END:
                return l_RecSize;
        }

        l_ArrayLen = 1;
    }

    return l_RecSize;
}

void DB2FileLoader::GenerateSparseCache(int32 p_ID)
{
    int32 l_RecordSize = 0;
    int32 l_Index = 0;
    int32 l_ArrayLen = 1;

    m_SparseCache[p_ID] = std::make_unique<FieldCache[]>(m_FieldCountIncludingArrayMemebers);
    int32 l_NonArrayIndex = 0;

    for (uint32 l_I = 0; m_Format[l_I]; ++l_I)
    {
        if (isdigit(m_Format[l_I]))
        {
            l_ArrayLen = atoi(m_Format + l_I);
            l_I += (GetNumberOfDigits(l_ArrayLen) - 1);
            continue;
        }

        if (IsRelationshipField(m_Format[l_I]))
            break;

        int32 l_ThisLen = GetLenAtIndex(l_Index);

        for (int l_Y = 0; l_Y < l_ArrayLen; ++l_Y)
        {
            if (m_Format[l_I] == FT_NSTRING)
                l_ThisLen = strlen(reinterpret_cast<char*>(m_Data[p_ID]) + l_RecordSize) + 1;

             m_SparseCache[p_ID][l_NonArrayIndex++] = std::make_tuple(l_RecordSize, l_ThisLen, m_StorageInfo.get() + l_Index);
            l_RecordSize += l_ThisLen;
        }

        ++l_Index;
        l_ArrayLen = 1;
    }
}

uint32 DB2FileLoader::GetFormatRecordSize(const char * format, uint32 p_Id, int32* index_pos, int32 p_At, bool p_Internal, uint32* p_SizeOfField)
{
    uint32 recordsize = 0;
    int32 i = -1;
    int32 l_Index = 0;
    int32 l_RawIndex = 0;
    int32 l_ArrayLen = 1;

    for (uint32 x=0; format[x]; ++x)
    {
        if (isdigit(format[x]))
        {
            l_ArrayLen = atoi(format + x);
            x += (GetNumberOfDigits(l_ArrayLen) - 1);
            continue;
        }

        for (int l_Y = 0; l_Y < l_ArrayLen; ++l_Y)
        {
            if (l_RawIndex == p_At && p_At != -1)
            {
                if (p_SizeOfField)
                    *p_SizeOfField = GetLenAtIndex(l_Index);

                return recordsize;
            }

            switch(format[x])
            {
                case FT_NA:
                    if (!p_Internal)
                        GetLenAtIndex(l_Index);
                    break;
                case FT_NA_BYTE:
                    if (!p_Internal)
                        recordsize += 1;
                    break;
                case FT_QWORD:
                    recordsize += p_Internal ? 8 : GetLenAtIndex(l_Index);
                    break;
                case FT_INT_RELATIONSHIP:
                    recordsize += p_Internal ? 4 : 0;
                    break;
                case FT_WORD_RELATIONSHIP:
                    recordsize += p_Internal ? 2 : 0;
                    break;
                case FT_BYTE_RELATIONSHIP:
                    recordsize += p_Internal ? 2 : 0;
                    break;
                case FT_FLOAT:
                case FT_INT:
                    recordsize += p_Internal ? 4 : GetLenAtIndex(l_Index);
                    break;
                case FT_STRING:
                    recordsize += p_Internal ? sizeof(LocalizedString*) : GetLenAtIndex(l_Index);
                    break;
                case FT_SORT:
                    if (!p_Internal)
                        recordsize += GetLenAtIndex(l_Index);
                    i = l_RawIndex;
                    break;
                case FT_INDEX:
                    i = l_RawIndex;
                    recordsize += p_Internal ? 4 : GetLenAtIndex(l_Index);
                    break;
                case FT_WORD_INDEX:
                    i = l_RawIndex;
                    recordsize += p_Internal ? 2 : GetLenAtIndex(l_Index);
                    break;
                case FT_BYTE_INDEX:
                    i = l_RawIndex;
                    recordsize += 1;
                    break;
                case FT_BYTE:
                    recordsize += 1;
                    break;
                case FT_SHORT:
                    recordsize += p_Internal ? 2 : GetLenAtIndex(l_Index);
                    break;
                case FT_NSTRING:
                {
                    if (!p_Internal)
                    {
                        uint8* l_Ptr = m_Data[p_Id] + recordsize;
                        ++recordsize;

                        while (*l_Ptr != 0)
                        {
                            ++l_Ptr;
                            ++recordsize;
                        }
                    }
                    else
                        recordsize += sizeof(LocalizedString*);
                    break;
                }
                default:
                    break;
            }
            ++l_RawIndex;
        }

        l_ArrayLen = 1;
        ++l_Index;
    }

    if (index_pos)
        *index_pos = i;

    return recordsize;
}

uint32 DB2FileLoader::GetFormatStringsFields(const char * format)
{
    uint32 stringfields = 0;
    for (uint32 x=0; format[x]; ++x)
        if (format[x] == FT_STRING || format[x] == FT_NSTRING)
            ++stringfields;

    return stringfields;
}

static char const* const nullStr = "";

char* DB2FileLoader::AutoProduceData(const char* format, uint32& records, char**& indexTable, uint32& p_RelationshipTableMaxID, char**& p_RelationshipLookupTable, std::set<LocalizedString*> & p_LocalizedString)
{
    if (GetFieldsCountBasedOnFormatString() != m_TotalFieldsCount && GetFieldsCountBasedOnFormatString() != m_TotalFieldsCount - 1)
        return nullptr;

    //get struct size and index pos
    int32 l_IndexField;
    uint32 l_MemoryEntrySize = GetFormatRecordSize(format, 0, &l_IndexField, -1, true);

    uint32 l_IndexSize = 0;
    std::string l_StringFormat = format;

    if (l_StringFormat.find(FT_INDEX) != std::string::npos)
        l_IndexSize = 4;
    else if (l_StringFormat.find(FT_WORD_INDEX) != std::string::npos)
        l_IndexSize = 2;
    else if (l_StringFormat.find(FT_BYTE_INDEX) != std::string::npos)
        l_IndexSize = 1;

    if (l_IndexField >= 0)
    {
        uint32 l_MaxID = 0;

        if (l_IndexField != m_IDIndex)
        {
            //find max index
            for (uint32 y = 0; y < m_RecordsCount; y++)
            {
                uint32 l_Index;

                switch (l_IndexSize)
                {
                    case 2:
                        l_Index = getRecord(y).Read<uint16>(l_IndexField);
                        break;
                    case 1:
                        l_Index = getRecord(y).Read<uint8>(l_IndexField);
                        break;
                    default:
                        l_Index = getRecord(y).Read<uint32>(l_IndexField);
                        break;
                }

                if (l_Index > l_MaxID)
                    l_MaxID = l_Index;
            }

            ++l_MaxID;
        }
        else
            l_MaxID = m_MaxID + 1;


        records = l_MaxID;
        indexTable = new char*[l_MaxID];
        memset(indexTable, 0, l_MaxID * sizeof(char*));
    }
    else
    {
        records = m_RecordsCount;
        indexTable = new char*[m_RecordsCount];
    }

    if (m_RelationshipInformation.NumRecords)
    {
        p_RelationshipTableMaxID = m_RelationshipInformation.MaxID + 1;
        p_RelationshipLookupTable = new char*[p_RelationshipTableMaxID];
        memset(p_RelationshipLookupTable, 0, p_RelationshipTableMaxID * sizeof(char*));
    }

    char* dataTable = new char[m_RecordsCount * l_MemoryEntrySize];

    uint32 l_Offset = 0;

    bool l_IsPopulated = false;
    std::unordered_map<uint32, std::tuple<uint32, uint32, uint32>> l_FieldInfo;

    for (uint32 y = 0; y < m_RecordsCount; y++)
    {
        if (p_RelationshipTableMaxID)
            p_RelationshipLookupTable[m_RelationshipInformation.Relationships[y].ForeignKey] = dataTable + l_Offset;

        auto const& l_Record = getRecord(y);

        if (!l_IsPopulated && l_FieldInfo.size())
            l_IsPopulated = true;

        uint32 l_FieldIndex = y;

        if (l_IndexField >= 0)
        {
            switch (l_IndexSize)
            {
                case 2:
                    l_FieldIndex = l_Record.Read<uint16>(l_IndexField);
                    break;
                case 1:
                    l_FieldIndex = l_Record.Read<uint8>(l_IndexField);
                    break;
                default:
                    l_FieldIndex = l_Record.Read<uint32>(l_IndexField);
                    break;
            }
        }

        indexTable[l_FieldIndex] = dataTable + l_Offset;

        int32 l_ArrayLen = 1;
        int32 l_RawIndex = 0;
        int32 l_FormatLen = strlen(format);
        int32 l_Index = 0;

        for (uint32 x = 0; x < l_FormatLen; x++)
        {
            if (isdigit(format[x]))
            {
                l_ArrayLen = atoi(format + x);
                x += (GetNumberOfDigits(l_ArrayLen) - 1);
                continue;
            }

            if (!l_IsPopulated)
                l_FieldInfo[l_Index] = std::make_tuple(l_Offset, l_ArrayLen, format[x]);

            switch (format[x])
            {
                case FT_QWORD:
                    l_Record.ReadArray<uint64>(reinterpret_cast<uint64*>(dataTable + l_Offset), l_RawIndex, l_ArrayLen);
                    l_Offset += sizeof(uint64) * l_ArrayLen;
                    break;
                case FT_FLOAT:
                    l_Record.ReadArray<float>(reinterpret_cast<float*>(dataTable + l_Offset), l_RawIndex, l_ArrayLen);
                    l_Offset += sizeof(float) * l_ArrayLen;
                    break;
                case FT_INDEX:
                case FT_INT:
                    l_Record.ReadArray<uint32>(reinterpret_cast<uint32*>(dataTable + l_Offset), l_RawIndex, l_ArrayLen);
                    l_Offset += sizeof(int32) * l_ArrayLen;
                    break;
                case FT_SHORT:
                case FT_WORD_INDEX:
                    l_Record.ReadArray<uint16>(reinterpret_cast<uint16*>(dataTable + l_Offset), l_RawIndex, l_ArrayLen);
                    l_Offset += sizeof(int16) * l_ArrayLen;
                    break;
                case FT_BYTE:
                case FT_BYTE_INDEX:
                    l_Record.ReadArray<uint8>(reinterpret_cast<uint8*>(dataTable + l_Offset), l_RawIndex, l_ArrayLen);
                    l_Offset += sizeof(int8) * l_ArrayLen;
                    break;
                case FT_NSTRING:
                case FT_STRING:
                    for (uint32 l_Y = 0; l_Y < l_ArrayLen; ++l_Y)
                    {
                        LocalizedString* l_Str = new LocalizedString(nullStr);
                        *reinterpret_cast<LocalizedString**>(&dataTable[l_Offset]) = l_Str;   // will be replaces non-empty or "" strings in AutoProduceStrings
                        p_LocalizedString.emplace(l_Str);
                        l_Offset += sizeof(LocalizedString*);
                    }
                    break;
                case FT_INT_RELATIONSHIP:
                {
                    *reinterpret_cast<uint32*>(dataTable + l_Offset) = l_Record.HasRelationshipData() ? l_Record.GetRelationshipKey() : 0;
                    l_Offset += sizeof(int32);
                    break;
                }
                case FT_WORD_RELATIONSHIP:
                {
                    *reinterpret_cast<uint16*>(dataTable + l_Offset) = l_Record.HasRelationshipData() ? l_Record.GetRelationshipKey() : 0;
                    l_Offset += sizeof(int16);
                    break;
                }
                case FT_BYTE_RELATIONSHIP:
                {
                    *reinterpret_cast<uint8*>(dataTable + l_Offset) = l_Record.HasRelationshipData() ? l_Record.GetRelationshipKey() : 0;
                    l_Offset += sizeof(int8);
                    break;
                }
                default:
                    break;
            }
            l_RawIndex += l_ArrayLen;
            ++l_Index;
            l_ArrayLen = 1;
        }
    }

    PopulateCommonData(indexTable, l_FieldInfo);
    return dataTable;
}

void DB2FileLoader::AutoProduceCStrings(const char* p_Fmt, char* p_DataTable, uint32 p_Locale)
{
    uint32 l_Offset = 0;

    if (!m_RecordsCount)
        return;

    for (uint32 y = 0; y < m_RecordsCount; y++)
    {
        int32 l_ArrayLen = 1;
        int32 l_FormatLen = strlen(p_Fmt);
        int32 l_RawIndex = 0;

        for (uint32 x = 0; x < l_FormatLen; x++)
        {
            if (isdigit(p_Fmt[x]))
            {
                l_ArrayLen = atoi(p_Fmt + x);
                x += (GetNumberOfDigits(l_ArrayLen) - 1);
                continue;
            }

            for (uint32 l_Y = 0; l_Y < l_ArrayLen; ++l_Y)
            {
                switch(p_Fmt[x])
                {
                    case FT_QWORD:
                        l_Offset += 8;
                        break;
                    case FT_FLOAT:
                        l_Offset += 4;
                        break;
                    case FT_INDEX:
                    case FT_INT:
                    case FT_INT_RELATIONSHIP:
                        l_Offset += 4;
                        break;
                    case FT_SHORT:
                    case FT_WORD_INDEX:
                    case FT_WORD_RELATIONSHIP:
                        l_Offset += 2;
                        break;
                    case FT_BYTE:
                    case FT_BYTE_INDEX:
                    case FT_BYTE_RELATIONSHIP:
                        l_Offset += 1;
                        break;
                    case FT_STRING:
                    {
                        std::string l_Empty = "";
                        LocalizedString * l_PtrPtr = *((LocalizedString**)(&p_DataTable[l_Offset]));
                        if (l_PtrPtr)
                            l_Empty = l_PtrPtr->Get(0);

                        l_Offset += sizeof(LocalizedString*);
                        break;
                    }
                    case FT_NSTRING:
                    {
                        char const* l_Ref = getRecord(y).getCString(l_RawIndex);

                        if (uint32 l_Len = strlen(l_Ref))
                        {
                            char* l_NewStr = new char[l_Len + 1];
                            memcpy(l_NewStr, l_Ref, l_Len + 1);

                            LocalizedString* l_PtrPtr = *((LocalizedString**)(&p_DataTable[l_Offset]));
                            if (l_PtrPtr)
                                l_PtrPtr->Str[p_Locale] = l_NewStr;
                        }

                        l_Offset += sizeof(LocalizedString*);
                        break;
                    }
                }
                ++l_RawIndex;
            }
            l_ArrayLen = 1;
        }
    }
}

void DB2FileLoader::GenerateSQLOutput(const char* p_Fmt, char* p_DataTable)
{
    uint32 l_Offset = 0;

    if (!m_RecordsCount)
        return;

    std::string l_CustomTableName = "retail_";

    std::string l_FileName = m_Filename;
    l_FileName = l_FileName.substr(l_FileName.find_last_of('/') + 1);

    std::string l_TempName = l_FileName.substr(0, l_FileName.find_last_of('.'));

    for (int l_I = 0; l_I < l_TempName.size(); ++l_I)
    {
        if (l_I > 0 && isupper(l_TempName[l_I]))
            l_CustomTableName += "_";

        char l_Char = ((char)tolower(l_TempName[l_I]));
        if (l_Char == '-')
            l_Char = '_';

        l_CustomTableName += l_Char;
    }

    bool l_Locale = false;

    std::fstream l_Output;
    l_Output.open("hotfixs_dump.sql", std::fstream::out | std::fstream::app);
    l_Output << "REPLACE INTO " << l_CustomTableName << " VALUES ";

    std::string l_IndexValue = "";

    std::map<uint32, std::string> l_Keys;

    for (uint32 y = 0; y < m_RecordsCount; y++)
    {
        l_Output << "(";
        int32 l_ArrayLen = 1;
        int32 l_FormatLen = strlen(p_Fmt);
        int32 l_RawIndex = 0;

        for (uint32 x = 0; x < l_FormatLen; x++)
        {
            if (isdigit(p_Fmt[x]))
            {
                l_ArrayLen = atoi(p_Fmt + x);
                x += (GetNumberOfDigits(l_ArrayLen) - 1);
                continue;
            }

            for (uint32 l_Y = 0; l_Y < l_ArrayLen; ++l_Y)
            {
                bool l_Last = x == l_FormatLen - 1 && l_Y == l_ArrayLen - 1;

                std::string l_EndStr = "', ";
                if (l_Last)
                {
                    if (y == m_RecordsCount - 1)
                        l_EndStr = "', '26365');" ;
                    else
                        l_EndStr = "', '26365'), ";
                }

                switch(p_Fmt[x])
                {
                    case FT_QWORD:
                        l_Offset += 8;
                        l_Output << "'" << *(uint64*)(&p_DataTable[l_Offset]) << l_EndStr;
                        break;
                    case FT_FLOAT:
                        l_Output << "'" << *(float*)(&p_DataTable[l_Offset]) << l_EndStr;
                        l_Offset += 4;
                        break;
                    case FT_INDEX:
                        l_Keys[y] = std::to_string(*(uint32*)(&p_DataTable[l_Offset]));
                        /// no break;
                    case FT_INT:
                    case FT_INT_RELATIONSHIP:
                        l_Output << "'" << *(uint32*)(&p_DataTable[l_Offset]) << l_EndStr;
                        l_Offset += 4;
                        break;
                    case FT_WORD_INDEX:
                        l_Keys[y] = std::to_string(*(uint16*)(&p_DataTable[l_Offset]));
                        /// no break;
                    case FT_SHORT:
                    case FT_WORD_RELATIONSHIP:
                        l_Output << "'" << *(uint16*)(&p_DataTable[l_Offset]) << l_EndStr;
                        l_Offset += 2;
                        break;
                    case FT_BYTE_INDEX:
                        l_Keys[y] = std::to_string((uint32)(*(uint8*)(&p_DataTable[l_Offset])));
                        /// no break;
                    case FT_BYTE:
                    case FT_BYTE_RELATIONSHIP:
                        l_Offset += 1;
                        break;
                    case FT_STRING:
                    {
                        std::string l_Empty = "";
                        LocalizedString * l_PtrPtr = *((LocalizedString**)(&p_DataTable[l_Offset]));
                        if (l_PtrPtr)
                            l_Empty = l_PtrPtr->Get(0);

                        l_Locale = true;

                        HotfixDatabase.EscapeString(l_Empty);
                        l_Output << "'" << l_Empty << l_EndStr;
                        l_Offset += sizeof(LocalizedString*);
                        break;
                    }
                    case FT_NSTRING:
                    {
                        char const* l_Ref = getRecord(y).getCString(l_RawIndex);
                        std::string l_String = "";
                        if (uint32 l_Len = strlen(l_Ref))
                        {
                            l_Locale = true;
                            char* l_NewStr = new char[l_Len + 1];
                            memcpy(l_NewStr, l_Ref, l_Len + 1);
                            l_String = l_NewStr;
                        }

                        HotfixDatabase.EscapeString(l_String);
                        l_Output << "'" << l_String << l_EndStr;
                        l_Offset += sizeof(LocalizedString*);
                        break;
                    }
                }
                ++l_RawIndex;
            }
            l_ArrayLen = 1;
        }
        l_Output << std::endl;
    }

    l_Output.flush();
    l_Output.close();

    if (!l_Locale)
        return;

    l_Offset = 0;

    std::fstream l_OutputLocale;
    l_OutputLocale.open("hotfixs_locale_dump.sql", std::fstream::out | std::fstream::app);
    l_OutputLocale << "REPLACE INTO " << l_CustomTableName << "_locales VALUES ";

    for (uint32 y = 0; y < m_RecordsCount; y++)
    {
        l_OutputLocale << "('" << l_Keys[y] << "'";
        int32 l_ArrayLen = 1;
        int32 l_FormatLen = strlen(p_Fmt);
        int32 l_RawIndex = 0;

        for (uint32 x = 0; x < l_FormatLen; x++)
        {
            if (isdigit(p_Fmt[x]))
            {
                l_ArrayLen = atoi(p_Fmt + x);
                x += (GetNumberOfDigits(l_ArrayLen) - 1);
                continue;
            }

            for (uint32 l_Y = 0; l_Y < l_ArrayLen; ++l_Y)
            {
                bool l_Last = x == l_FormatLen - 1 && l_Y == l_ArrayLen - 1;

                switch (p_Fmt[x])
                {
                    case FT_QWORD:
                        l_Offset += 8;
                        break;
                    case FT_FLOAT:
                        l_Offset += 4;
                        break;
                    case FT_INDEX:
                    case FT_INT:
                    case FT_INT_RELATIONSHIP:
                        l_Offset += 4;
                        break;
                    case FT_WORD_INDEX:
                    case FT_SHORT:
                    case FT_WORD_RELATIONSHIP:
                        l_Offset += 2;
                        break;
                    case FT_BYTE_INDEX:
                    case FT_BYTE:
                    case FT_BYTE_RELATIONSHIP:
                        l_Offset += 1;
                        break;
                    case FT_STRING:
                    {
                        LocalizedString * l_PtrPtr = *((LocalizedString**)(&p_DataTable[l_Offset]));

                        for (uint8 l_Locale = 1; l_Locale < MAX_LOCALES; l_Locale++)
                        {
                            std::string l_String = l_PtrPtr ? l_PtrPtr->Get(l_Locale) : "";
                            HotfixDatabase.EscapeString(l_String);
                            l_OutputLocale << ", '" << l_String << "'";
                        }

                        l_Offset += sizeof(LocalizedString*);
                        break;
                    }
                    case FT_NSTRING:
                    {
                        LocalizedString* l_PtrPtr = *((LocalizedString**)(&p_DataTable[l_Offset]));

                        for (uint8 l_Locale = 1; l_Locale < MAX_LOCALES; l_Locale++)
                        {
                            std::string l_String = l_PtrPtr ? l_PtrPtr->Get(l_Locale) : "";
                            HotfixDatabase.EscapeString(l_String);
                            l_OutputLocale << ", '" << l_String << "'";
                        }

                        l_Offset += sizeof(LocalizedString*);
                        break;
                    }
                }
                ++l_RawIndex;
            }
            l_ArrayLen = 1;
        }

        if (y == m_RecordsCount - 1)
            l_OutputLocale << ");";
        else
            l_OutputLocale << "), ";

        l_OutputLocale << std::endl;
    }

    l_OutputLocale.flush();
    l_OutputLocale.close();
}

void DB2FileLoader::PopulateCommonData(char** indexTable, std::unordered_map<uint32, std::tuple<uint32, uint32, uint32>> const& p_FieldInfo)
{
    if (!m_CommonData)
        return;

    uint32* l_CommonDataItr = reinterpret_cast<uint32*>(m_CommonData);

    for (uint32 field = m_Flags & 4 ? 1 : 0; field < m_TotalFieldsCount; ++field)
    {
        auto const& l_StorageInfo = m_StorageInfo[field];

        if (l_StorageInfo.CompressionType != FieldCompression::FieldCompressionCommonData)
            continue;

        uint32 l_NumValuesForField = l_StorageInfo.AdditionalDataSize / 8;

        if (!l_NumValuesForField)
            continue;

        auto const& l_InfoEntry = p_FieldInfo.at(field);
        uint32 l_Offset = std::get<0>(l_InfoEntry);
        uint32 l_ArrayLen = std::get<1>(l_InfoEntry);
        uint32 l_Format = std::get<2>(l_InfoEntry);

        for (uint32 l_I = 0; l_I < l_NumValuesForField; ++l_I)
        {
            uint32 recordId = *reinterpret_cast<uint32*>(l_CommonDataItr++);
            if (char* recordData = indexTable[recordId])
            {
                switch (l_Format)
                {
                    case FT_SHORT:
                    {
                        uint16 l_Value = *reinterpret_cast<uint16*>(l_CommonDataItr);
                        for (uint32 l_Index = 0; l_Index < l_ArrayLen; ++l_Index)
                            *reinterpret_cast<uint16*>(&recordData[l_Offset + sizeof(uint16) * l_Index]) = l_Value;
                        break;
                    }
                    case FT_BYTE:
                    {
                        uint8 l_Value = *reinterpret_cast<uint8*>(l_CommonDataItr);
                        for (uint32 l_Index = 0; l_Index < l_ArrayLen; ++l_Index)
                            *reinterpret_cast<uint8*>(&recordData[l_Offset + sizeof(uint8) * l_Index]) = l_Value;
                        break;
                    }
                    case FT_FLOAT:
                    case FT_INT:
                    {
                        uint32 l_Value = *reinterpret_cast<uint32*>(l_CommonDataItr);
                        for (uint32 l_Index = 0; l_Index < l_ArrayLen; ++l_Index)
                            *reinterpret_cast<uint32*>(&recordData[l_Offset + sizeof(uint32) * l_Index]) = l_Value;
                        break;
                    }
                    default:
                        break;
                }
            }
            ++l_CommonDataItr;
        }
    }

    m_FieldsCount = m_TotalFieldsCount;
}

char* DB2FileLoader::AutoProduceStrings(const char* format, char* dataTable, uint32 p_Locale)
{
    if (!m_StringTableSize)
        return nullptr;

    char* stringPool= new char[m_StringTableSize];
    memcpy(stringPool, m_StringTable, m_StringTableSize);

    uint32 offset = 0;

    for (uint32 y = 0; y < m_RecordsCount; y++)
    {
        int32 l_ArrayLen = 1;
        int32 l_FormatLen = strlen(format);
        int32 l_RawIndex = 0;

        for (uint32 x = 0; x < l_FormatLen; x++)
        {
            if (isdigit(format[x]))
            {
                l_ArrayLen = atoi(format + x);
                x += (GetNumberOfDigits(l_ArrayLen) - 1);
                continue;
            }

            for (uint32 l_Y = 0; l_Y < l_ArrayLen; ++l_Y)
            {
                switch(format[x])
                {
                    case FT_QWORD:
                        offset += 8;
                        break;
                    case FT_FLOAT:
                    case FT_INDEX:
                    case FT_INT:
                    case FT_INT_RELATIONSHIP:
                        offset += 4;
                        break;
                    case FT_BYTE:
                    case FT_BYTE_INDEX:
                    case FT_BYTE_RELATIONSHIP:
                        offset += 1;
                        break;
                    case FT_SHORT:
                    case FT_WORD_INDEX:
                    case FT_WORD_RELATIONSHIP:
                        offset += 2;
                        break;
                    case FT_STRING:
                    {
                        // fill only not filled entries
                        LocalizedString* l_PtrPtr = *((LocalizedString**)(&dataTable[offset]));
                        if (l_PtrPtr->Str[p_Locale] == nullStr)
                        {
                            const char* st = getRecord(y).getString(l_RawIndex);
                            l_PtrPtr->Str[p_Locale] = stringPool + (st - (const char*)m_StringTable);
                        }

                        offset += sizeof(LocalizedString*);
                        break;
                    }
                }
                ++l_RawIndex;
            }
            l_ArrayLen = 1;
        }
        l_RawIndex = 0;
    }

    return stringPool;
}

uint32 DB2FileLoader::GetLenAtIndex(uint32 p_Index)
{
    return p_Index < m_FieldsCount ? m_FieldInfoMap[p_Index].l_Len / 8 : 0;
}

uint32 DB2FileLoader::GetFieldSizeBasedOnOffset(uint32 p_Offset)
{
    if (p_Offset >= m_FieldInfoMap[m_FieldInfoMap.size() - 1].l_Offset)
        return m_FieldInfoMap[m_FieldInfoMap.size() - 1].l_Len;

    uint32 l_LastSize = 0;
    for (int32 l_I = 0; l_I < m_FieldInfoMap.size(); ++l_I)
    {
        FieldInfo const& l_Info = m_FieldInfoMap[l_I];
        l_LastSize = l_Info.l_Len;

        if (p_Offset < l_Info.l_Offset)
            return l_LastSize;
    }

    return l_LastSize;
}

uint32 DB2FileLoader::GetFieldsCountBasedOnFormatString()
{
    if (!m_Format)
        return 0;

    std::string l_Format = m_Format;

    return std::count_if(l_Format.begin(), l_Format.end(), [](char p_Char) -> bool{
        return !isdigit(p_Char) && !IsRelationshipField(p_Char);
    });
}
