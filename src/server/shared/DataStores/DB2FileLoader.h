////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DB2_FILE_LOADER_H
#define DB2_FILE_LOADER_H

#include "Define.h"
#include "Utilities/ByteConverter.h"
#include <cassert>

struct FieldInfo
{
    uint16 l_Offset;
    uint16 l_Len;
};

enum FieldCompression : uint32
{
    FieldCompressionNone,
    FieldCompressionBitpacked,
    FieldCompressionCommonData,
    FieldCompressionBitpackedIndexed,
    FieldCompressionBitpackedIndexedArray,
};

struct FieldStorageInfo
{
    uint16 FieldOffsetBits;
    uint16 FieldSizeBits;
    uint32 AdditionalDataSize;
    FieldCompression CompressionType;

    union
    {
        struct FieldCompressionBitpacked
        {
            uint32 BitpackingOffsetbits;
            uint32 BitpackingSizeBits;
            uint32 Flags;
        } FieldCompressionBitpacked;

        struct FieldCompressionComonData
        {
            uint32 DefaultValue;
            uint32 Unused;
            uint32 Unused2;
        } FieldCompressionComonData;

        struct FieldCompressionBitpackedIndex
        {
            uint32 BitpackingOffsetbits;
            uint32 BitpackingSizeBits;
            uint32 Unused;
        } FieldCompressionBitpackedIndex;

        struct FieldCompressionBitpackedIndexArray
        {
            uint32 BitpackingOffsetbits;
            uint32 BitpackingSizeBits;
            uint32 ArrayCount;
        } FieldCompressionBitpackedIndexArray;

        uint32 Default[3];

    } Data;
};

struct RelationshipInformation
{
    struct RelationshipEntry
    {
        uint32 ForeignKey;
        uint32 ForeignIndex;
    };

    uint32 NumRecords;
    uint32 MinID;
    uint32 MaxID;
    std::vector<RelationshipEntry> Relationships;
};

typedef std::vector<FieldInfo> FieldInfoMap;
using FieldCache = std::tuple<int32, int32, FieldStorageInfo*>;

class DB2FileLoader
{
    public:
        DB2FileLoader();
        ~DB2FileLoader();

    bool Load(const char *filename, const char *fmt);
    void PopulateNonSparseCache(char const* p_Fmt);

    class Record
    {
    public:

        int32 GetOffset(int32 p_FieldIndex, uint32& p_Size, FieldStorageInfo*& p_FieldInfo) const;
        int32 GetOffset(int32 p_FieldIndex) const;

        bool HasRelationshipData() const { return file.m_RelationshipInformation.NumRecords; }
        uint32 GetRelationshipKey() const { return file.m_RelationshipInformation.Relationships[m_ID].ForeignKey; }

        template <typename T> void ReadArray(T* p_Beginning, size_t p_Field, int32 p_ArraySize) const;

        template <typename T>
        T Read(size_t p_Field) const
        {
            T l_Value;
            ReadArray(&l_Value, p_Field, 1);
            return l_Value;
        }

        const char *getString(size_t field) const
        {
            size_t stringOffset = Read<uint32>(field);
           // assert(stringOffset < file.m_StringTableSize);
            return reinterpret_cast<char*>(file.m_StringTable + stringOffset);
        }

        const char *getCString(size_t field) const
        {
            return reinterpret_cast<char*>(offset + GetOffset(field));
        }

    private:
        Record(DB2FileLoader &file_, unsigned char *offset_, uint32 p_ID, std::unique_ptr<FieldCache[]> const& p_Cache);

        unsigned char *offset;
        DB2FileLoader &file;
        uint32 m_ID;
        std::unique_ptr<FieldCache[]> const& m_Cache;

        friend class DB2FileLoader;
    };

    // Get record by id
    Record getRecord(size_t id);
    /// Get begin iterator over records

    uint32 GetNumRows() const { return m_RecordsCount;}
    uint32 GetCols() const { return m_FieldsCount; }
    uint32 GetHash() const { return m_TableHash; }
    bool IsLoaded() const { return (m_Data != nullptr); }
    char* AutoProduceData(const char* fmt, uint32& count, char**& indexTable, uint32& p_RelationshipTableMaxID, char**& p_RelationshipLookupTable, std::set<LocalizedString*> & p_LocalizedString);
    char* AutoProduceStrings(const char* fmt, char* dataTable, uint32 p_Locale);
    void PopulateCommonData(char** indexTable, std::unordered_map<uint32, std::tuple<uint32, uint32, uint32>> const& p_FieldInfo);
    void AutoProduceCStrings(const char* p_Fmt, char* p_DataTable, uint32 p_Locale);
    uint32 GetFormatRecordSize(const char * format, uint32 p_Id, int32 * index_pos = nullptr, int32 l_At = -1, bool p_Internal = false, uint32* p_SizeOfField = nullptr);
    static uint32 GetFormatRecordSizeForDB2Loader(const char* format);
    void GenerateSparseCache(int32 p_ID);
    static uint32 GetFormatStringsFields(const char * format);

    uint32 GetLenAtIndex(uint32 p_Index);
    uint32 GetFieldSizeBasedOnOffset(uint32 p_Offset);
    uint32 GetFieldsCountBasedOnFormatString();

    void GenerateSQLOutput(const char* p_Fmt, char* p_DataTable);

private:
    uint32 m_RecordSize;
    uint32 m_RecordsCount;
    uint32 m_FieldsCount;
    uint32 m_StringTableSize;

    // WDB2 / WCH2 fields
    uint32 m_TableHash;    // WDB2
    uint32 m_Build;        // WDB2

    uint32 m_MinID;
    uint32 m_MaxID;
    uint32 m_Locale;
    uint32 m_DuplicatesSize;
    uint16 m_Flags;
    uint16 m_IDIndex;
    uint32 m_TotalFieldsCount;
    uint32 m_BitPackedDataOffset;
    uint32 m_LookupCollumnCount;
    uint32 m_OffsetMapOffset;
    uint32 m_IDListSize;
    uint32 m_FieldStorageInfoSize;
    uint32 m_CommonFieldSize;
    uint32 m_CommonDataCount;
    uint32 m_PalletDataSize;
    uint32 m_RelationshipDataSize;

    uint32 m_FieldCountIncludingArrayMemebers;

    char* m_CommonData;
    char* m_PalletData;

    FieldInfoMap m_FieldInfoMap;

    uint8** m_Data;
    uint8*  m_StringTable;

    char const* m_Format;

    char const* m_Filename;

    RelationshipInformation  m_RelationshipInformation;
    std::unique_ptr<FieldStorageInfo[]> m_StorageInfo;
    std::unique_ptr<FieldCache[]> m_NonSparseFieldCache;
    std::unique_ptr<uint32[]> m_PalletSizes;

    std::vector<std::unique_ptr<FieldCache[]>> m_SparseCache;
};

#endif
