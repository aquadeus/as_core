////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DB2STORE_H
#define DB2STORE_H

#include "DB2FileLoader.h"
#include "Log.h"
#include "Field.h"
#include "DatabaseWorkerPool.h"
#include "Implementation/HotfixDatabase.h"
#include "DatabaseEnv.h"
#include "Common.h"
#include "DBStorageIterator.h"

struct SqlDb2
{
    std::string m_IndexName;
    std::string m_SQLTableName;

    SqlDb2(const std::string p_FileName, const std::string p_IndexName, const char * p_DB2Fmt)
        :  m_IndexName(p_IndexName)
    {
        m_SQLTableName = p_FileName;
    }
};

class DB2StorageBase
{
    public:
        /// Constructor
        /// @p_Format :  DB2 format
        DB2StorageBase(const char * p_Format)
            : m_TableHash(0), m_MaxID(0), m_FieldCount(0), m_Format(p_Format), m_RelationshipTableMaxID(0)
        {

        }

        /// Get DB2 hash
        uint32 GetHash() const
        {
            return m_TableHash;
        }
        /// Get num rows
        uint32 GetNumRows() const
        {
            return m_MaxID;
        }
        /// Get format descriptor
        char const* GetFormat() const
        {
            return m_Format;
        }
        /// Get field count
        uint32 GetFieldCount() const
        {
            return m_FieldCount;
        }

        /// Get raw entry by index
        /// @p_ID : Entry index
        virtual char * LookupEntryRaw(uint32 p_ID) const = 0;

        /// Write record into a ByteBuffer
        /// @p_ID     : Record ID
        /// @p_Buffer : Destination buffer
        /// @p_Locale : Client locale
        virtual bool WriteRecord(uint32 p_ID, ByteBuffer & p_Buffer, uint32 p_Locale, bool p_HaveIndexInFields)
        {
            char const* l_Raw = LookupEntryRaw(p_ID);

            if (!l_Raw)
                return false;

            std::size_t l_FieldCount = strlen(m_Format);
            std::size_t l_ArrayLen = 1;

            auto l_GetNumberOfDigits = [](int p_Num) -> int
            {
                return p_Num > 0 ? (int)log10((double)p_Num) + 1 : 1;
            };

            uint32 l_I = 0;

            if (!p_HaveIndexInFields)
            {
                l_I += 1;
                l_Raw += 4;
            }

            for (; l_I < l_FieldCount; ++l_I)
            {
                if (isdigit(m_Format[l_I]))
                {
                    l_ArrayLen = atoi(m_Format + l_I);
                    l_I += (l_GetNumberOfDigits(l_ArrayLen) - 1);
                    continue;
                }

                for (uint32 l_Y = 0; l_Y < l_ArrayLen; ++l_Y)
                {
                    switch (m_Format[l_I])
                    {
                        case FT_QWORD:
                            p_Buffer << *(uint64*)l_Raw;
                            l_Raw += 8;
                            break;
                        case FT_INDEX:
                        case FT_INT:
                        case FT_INT_RELATIONSHIP:
                            p_Buffer << *(uint32*)l_Raw;
                            l_Raw += 4;
                            break;
                        case FT_FLOAT:
                            p_Buffer << *(float*)l_Raw;
                            l_Raw += 4;
                            break;
                        case FT_BYTE:
                        case FT_BYTE_INDEX:
                        case FT_BYTE_RELATIONSHIP:
                            p_Buffer << *(uint8*)l_Raw;
                            l_Raw += 1;
                            break;
                        case FT_SHORT:
                        case FT_WORD_INDEX:
                        case FT_WORD_RELATIONSHIP:
                            p_Buffer << *(uint16*)l_Raw;
                            l_Raw += 2;
                            break;
                        case FT_STRING:
                        case FT_NSTRING:
                        {
                            LocalizedString * l_LocalizedString = *((LocalizedString**)(l_Raw));

                            char const* locStr = l_LocalizedString->Get(p_Locale);
                            l_Raw += sizeof(char*);

                            std::string l_String(locStr);
                            p_Buffer << l_String.c_str();

                            break;
                        }
                    }
                }

                l_ArrayLen = 1;
            }

            return true;
        }

        /// Reload the data store
        virtual bool Reload(std::string& p_OutMessage) = 0;

        std::string GetDbcFileName() const
        {
            return m_DbcFileName;
        }

    protected:
        std::string m_DbcFileName;
        uint32 m_TableHash;
        uint32 m_MaxID;
        uint32 m_RelationshipTableMaxID;
        uint32 m_FieldCount;
        char const* m_Format;

};

template<class T> class DB2Storage : public DB2StorageBase
{
    static_assert(std::is_standard_layout<T>::value, "T in DB2Storage must have standard layout.");

    using StringPoolList = std::list<char*>;
    using DataTableEx = std::vector<T*>;
    using iterator = DBStorageIterator<T>;

    public:
        /// Constructor
        /// @p_Format :  DB2 format
        explicit DB2Storage(char const* p_Format, bool p_LoadNew = false)
            : DB2StorageBase(p_Format), m_IndexTable(nullptr), m_DataTable(nullptr), m_SQL(nullptr), m_LoadNew(p_LoadNew), m_RelationshipLookupTable(nullptr)
        {

        }
        /// Destructor
        ~DB2Storage()
        {
            Clear();
        }

        /// Get entry by index
        /// @p_ID : Entry index
        T const* LookupEntry(uint32 p_ID) const
        {
            return p_ID >= m_MaxID ? nullptr : m_IndexTable[p_ID];
        }

        /// Get entry by relationship IP
        /// DO NOT USE UNLESS YOU KNOW HOW IT WORKS!!!!
        T const* LookupByRelationshipID(uint32 p_ID)
        {
            return p_ID >= m_RelationshipTableMaxID ? nullptr : m_RelationshipLookupTable[p_ID];
        }

        /// Get entry by index - same as LookupEntry
        /// @p_ID : Entry index
        inline T const* operator[](uint32 p_ID) const
        {
            return LookupEntry(p_ID);
        }

        /// Get raw entry by index
        /// @p_ID : Entry index
        virtual char * LookupEntryRaw(uint32 p_ID) const override
        {
            return (char*)LookupEntry(p_ID);
        }

        /// Copies the provided entry and stores it.
        /// @p_ID       : Entry index
        /// @p_Entry    : The entry
        /// @p_DontCopy : Do not copy the entry
        void AddEntry(uint32 p_ID, T const* p_Entry, bool p_DontCopy = false)
        {
            if (LookupEntry(p_ID))
                return;

            if (p_ID >= m_MaxID)
            {
                // reallocate index table
                char** l_TempIndexTable = new char*[p_ID+1];
                memset(l_TempIndexTable, 0, (p_ID+1) * sizeof(char*));
                memcpy(l_TempIndexTable, (char*)m_IndexTable, m_MaxID * sizeof(char*));
                delete[] ((char*)m_IndexTable);

                m_MaxID      = p_ID + 1;
                m_IndexTable = (T**)l_TempIndexTable;
            }

            if (!p_DontCopy)
            {
                T* l_EntryCopy = new T;
                memcpy((char*)l_EntryCopy, (char*)p_Entry, sizeof(T));
                m_DataTableEx.push_back(l_EntryCopy);
                m_IndexTable[p_ID] = l_EntryCopy;
            }
            else
            {
                m_DataTableEx.push_back(const_cast<T*>(p_Entry));
                m_IndexTable[p_ID] = const_cast<T*>(p_Entry);
            }
        }
        /// Erase an entry
        /// @p_ID       : Entry index
        void EraseEntry(uint32 p_ID)
        {
            m_IndexTable[p_ID] = nullptr;
        }

        /// FOR UPGRADE PURPOSES ONLY
        bool LoadNew() const
        {
            return m_LoadNew;
        }

        /// Get index position and size
        /// @p_OutIndexSize : Output index size
        int GetIndexPositionAndSize(int & p_OutIndexSize)
        {
            std::size_t l_FieldCount    = strlen(m_Format);
            std::size_t l_ArrayLen      = 1;
            std::size_t l_IndexPosition = 0;

            auto l_GetNumberOfDigits = [](int p_Num) -> int
            {
                return p_Num > 0 ? (int)log10((double)p_Num) + 1 : 1;
            };

            for (uint32 l_I = 0; l_I < l_FieldCount; ++l_I)
            {
                if (isdigit(m_Format[l_I]))
                {
                    l_ArrayLen = atoi(m_Format + l_I);
                    l_I += (l_GetNumberOfDigits(l_ArrayLen) - 1);
                    continue;
                }

                for (uint32 l_Y = 0; l_Y < l_ArrayLen; ++l_Y)
                {
                    switch (m_Format[l_I])
                    {
                        case FT_INDEX:
                            p_OutIndexSize = 4;
                            return l_IndexPosition;

                        case FT_BYTE_INDEX:
                            p_OutIndexSize = 1;
                            return l_IndexPosition;

                        case FT_WORD_INDEX:
                            p_OutIndexSize = 2;
                            return l_IndexPosition;

                    }

                    l_IndexPosition++;
                }

                l_ArrayLen = 1;
            }

            return l_IndexPosition;
        }

        /// Load the DB2 from a file
        /// @p_FileName : DB2 file path
        /// @p_SQL      : SQL options
        bool Load(char const* p_FileName, SqlDb2 * p_SQL, uint32 p_Locale)
        {
            auto l_GetNumberOfDigits = [](int p_Num) -> int
            {
                return p_Num > 0 ? (int)log10((double)p_Num) + 1 : 1;
            };

            std::size_t l_FieldCount = strlen(m_Format);

            m_DbcFileName = p_FileName;

            DB2FileLoader l_DB2Reader;
            m_SQL = p_SQL;

            /// Check if load was successful, only then continue
            if (!l_DB2Reader.Load(p_FileName, m_Format))
                return false;

            Field* l_SQLFields = nullptr;

            QueryResult l_SQLQueryResult = QueryResult(nullptr);

            int l_IndexPosition = 0;
            int l_IndexSize = 0;
            l_IndexPosition = GetIndexPositionAndSize(l_IndexSize);

            /// Load data from SQL
            if (p_SQL)
            {
                l_SQLQueryResult = HotfixDatabase.PQuery("SHOW TABLES LIKE '%s'", p_SQL->m_SQLTableName.c_str());

                if (!l_SQLQueryResult || !l_SQLQueryResult->GetRowCount())
                {
                    l_SQLQueryResult = QueryResult(nullptr);
                    p_SQL = nullptr;
                    m_SQL = nullptr;
                }
                else
                {
                    std::string l_SQLQueryStr = "SELECT * FROM " + p_SQL->m_SQLTableName;

                    if (l_IndexPosition >= 0)
                        l_SQLQueryStr += " ORDER BY " + p_SQL->m_IndexName + " DESC";
                    l_SQLQueryStr += ';';

                    l_SQLQueryResult = HotfixDatabase.Query(l_SQLQueryStr.c_str());

                    if (l_SQLQueryResult)
                        l_SQLFields = l_SQLQueryResult->Fetch();
                }
            }

            m_FieldCount    = l_DB2Reader.GetCols();
            m_TableHash     = l_DB2Reader.GetHash();

            m_DataTable = (T*)l_DB2Reader.AutoProduceData(m_Format, m_MaxID, (char**&)m_IndexTable, m_RelationshipTableMaxID, (char**&)m_RelationshipLookupTable, m_LocalizedString);         ///< Load raw non-string data

            if (m_DataTable)
            {
                l_DB2Reader.AutoProduceStrings(m_Format, (char*)m_DataTable, p_Locale);
                l_DB2Reader.AutoProduceCStrings(m_Format, (char*)m_DataTable, p_Locale);
            }

            std::size_t l_ArrayLen = 1;

            /// Insert SQL data into arrays
            if (l_SQLQueryResult)
            {
                if (m_IndexTable)
                {
                    do
                    {
                        l_SQLFields = l_SQLQueryResult->Fetch();

                        uint32 l_Entry = 0;

                        if (l_IndexSize == sizeof(uint32))
                            l_Entry = l_SQLFields[l_IndexPosition].GetUInt32();
                        else if (l_IndexSize == sizeof(uint16))
                            l_Entry = l_SQLFields[l_IndexPosition].GetUInt16();
                        else if (l_IndexSize == sizeof(uint8))
                            l_Entry = l_SQLFields[l_IndexPosition].GetUInt8();
                        else
                            assert(false);

                        T * l_NewEntry = (T*)LookupEntry(l_Entry);

                        if (!l_NewEntry)
                            l_NewEntry = new T();

                        char * l_WritePtr = (char*)l_NewEntry;

                        uint32 l_SQLColumnNumber    = 0;
                        uint32 l_WritePosition      = 0;

                        for (uint32 l_I = 0; l_I < l_FieldCount; ++l_I)
                        {
                            if (isdigit(m_Format[l_I]))
                            {
                                l_ArrayLen = atoi(m_Format + l_I);
                                l_I += (l_GetNumberOfDigits(l_ArrayLen) - 1);
                                continue;
                            }

                            for (uint32 l_Y = 0; l_Y < l_ArrayLen; ++l_Y)
                            {
                                switch (m_Format[l_I])
                                {
                                    case FT_FLOAT:
                                        *((float*)(&l_WritePtr[l_WritePosition])) = l_SQLFields[l_SQLColumnNumber].GetFloat();
                                        l_WritePosition += sizeof(float);
                                        break;
                                    case FT_INDEX:
                                    case FT_INT:
                                    case FT_INT_RELATIONSHIP:
                                        *((uint32*)(&l_WritePtr[l_WritePosition])) = l_SQLFields[l_SQLColumnNumber].GetUInt32();
                                        l_WritePosition += sizeof(uint32);
                                        break;
                                    case FT_QWORD:
                                        *((uint64*)(&l_WritePtr[l_WritePosition])) = l_SQLFields[l_SQLColumnNumber].GetUInt64();
                                        l_WritePosition += sizeof(uint64);
                                        break;
                                    case FT_WORD_INDEX:
                                    case FT_SHORT:
                                    case FT_WORD_RELATIONSHIP:
                                        *((uint16*)(&l_WritePtr[l_WritePosition])) = l_SQLFields[l_SQLColumnNumber].GetUInt16();
                                        l_WritePosition += sizeof(uint16);
                                        break;
                                    case FT_BYTE:
                                    case FT_BYTE_INDEX:
                                    case FT_BYTE_RELATIONSHIP:
                                        *((uint8*)(&l_WritePtr[l_WritePosition])) = l_SQLFields[l_SQLColumnNumber].GetUInt8();
                                        l_WritePosition += sizeof(uint8);
                                        break;
                                    case FT_STRING:
                                    case FT_NSTRING:
                                    {
                                        if (!LookupEntry(l_Entry))
                                        {
                                            *((LocalizedString**)(&l_WritePtr[l_WritePosition])) = new LocalizedString();
                                            m_LocalizedString.emplace(*((LocalizedString**)(&l_WritePtr[l_WritePosition])));
                                        }

                                        m_CustomStrings.push_back(l_SQLFields[l_SQLColumnNumber].GetString());

                                        char * l_NewString = const_cast<char*>(m_CustomStrings.back().c_str());

                                        LocalizedString * l_LocalizedString = *((LocalizedString**)(&l_WritePtr[l_WritePosition]));
                                        l_LocalizedString->Str[p_Locale]    = l_NewString;

                                        l_WritePosition += sizeof(LocalizedString*);
                                        break;
                                    }
                                }

                                l_SQLColumnNumber++;
                            }

                            l_ArrayLen = 1;
                        }

                        if (!LookupEntry(l_Entry))
                            AddEntry(l_Entry, l_NewEntry, true);

                    } while (l_SQLQueryResult->NextRow());
                }
            }

            uint32 l_StringCount = 0;
            for (uint32 l_I = 0; l_I < l_FieldCount; ++l_I)
            {
                if (isdigit(m_Format[l_I]))
                {
                    l_ArrayLen = atoi(m_Format + l_I);
                    l_I += (l_GetNumberOfDigits(l_ArrayLen) - 1);
                    continue;
                }

                for (uint32 l_Y = 0; l_Y < l_ArrayLen; ++l_Y)
                {
                    if (m_Format[l_I] == FT_STRING || m_Format[l_I] == FT_NSTRING)
                        l_StringCount++;
                }

                l_ArrayLen = 1;
            }

            if (m_SQL && l_StringCount)
            {
                l_SQLQueryResult = HotfixDatabase.PQuery("SHOW TABLES LIKE '%s'", (p_SQL->m_SQLTableName + "_locales").c_str());

                if (!l_SQLQueryResult || !l_SQLQueryResult->GetRowCount())
                {
                    l_SQLQueryResult = QueryResult(nullptr);
                    p_SQL = nullptr;
                }
                else
                {
                    std::string l_SQLQueryStr = "SELECT * FROM " + m_SQL->m_SQLTableName + "_locales";

                    if (l_IndexPosition >= 0)
                        l_SQLQueryStr += " ORDER BY " + m_SQL->m_IndexName + " DESC";
                    l_SQLQueryStr += ';';

                    l_SQLQueryResult = HotfixDatabase.Query(l_SQLQueryStr.c_str());
                    if (l_SQLQueryResult)
                    {
                        /// All locales (but not us), Index & BuildVerified
                        if (l_SQLQueryResult->GetFieldCount() != ((l_StringCount * (MAX_LOCALES - 1)) + 1))
                        {
                            sLog->outError(LOG_FILTER_GENERAL, "SQL format invalid for table : '%s_locales'", p_SQL->m_SQLTableName.c_str());
                            return false;
                        }

                        do
                        {
                            Field* l_SQLFields = l_SQLQueryResult->Fetch();
                            uint32 l_SQLColumnNumber = 0;
                            uint32 l_EntryID = 0;

                            if (l_IndexSize == sizeof(uint32))
                                l_EntryID = l_SQLFields[0].GetUInt32();
                            else if (l_IndexSize == sizeof(uint16))
                                l_EntryID = l_SQLFields[0].GetUInt16();
                            else if (l_IndexSize == sizeof(uint8))
                                l_EntryID = l_SQLFields[0].GetUInt8();
                            else
                                assert(false);

                            l_SQLColumnNumber++;
                            T * l_Entry = (T*)LookupEntry(l_EntryID);

                            if (!l_Entry)
                                continue;

                            char * l_WritePtr = (char*)l_Entry;

                            uint32 l_WritePosition = 0;
                            std::size_t l_ArrayLen = 1;

                            for (uint32 l_I = 0; l_I < l_FieldCount; ++l_I)
                            {
                                if (isdigit(m_Format[l_I]))
                                {
                                    l_ArrayLen = atoi(m_Format + l_I);
                                    l_I += (l_GetNumberOfDigits(l_ArrayLen) - 1);
                                    continue;
                                }

                                for (uint32 l_Y = 0; l_Y < l_ArrayLen; ++l_Y)
                                {
                                    switch (m_Format[l_I])
                                    {
                                        case FT_FLOAT:
                                            l_WritePosition += sizeof(float);
                                            break;
                                        case FT_INDEX:
                                        case FT_INT:
                                        case FT_INT_RELATIONSHIP:
                                            l_WritePosition += sizeof(uint32);
                                            break;
                                        case FT_QWORD:
                                            l_WritePosition += sizeof(uint64);
                                            break;
                                        case FT_SHORT:
                                        case FT_WORD_INDEX:
                                        case FT_WORD_RELATIONSHIP:
                                            l_WritePosition += sizeof(uint16);
                                            break;
                                        case FT_BYTE:
                                        case FT_BYTE_INDEX:
                                        case FT_BYTE_RELATIONSHIP:
                                            l_WritePosition += sizeof(uint8);
                                            break;
                                        case FT_STRING:
                                        case FT_NSTRING:
                                        {
                                            if (!LookupEntry(l_EntryID))
                                            {
                                                *((LocalizedString**)(&l_WritePtr[l_WritePosition])) = new LocalizedString();
                                                m_LocalizedString.emplace(*((LocalizedString**)(&l_WritePtr[l_WritePosition])));
                                            }

                                            LocalizedString * l_LocalizedString = *((LocalizedString**)(&l_WritePtr[l_WritePosition]));

                                            for (uint32 l_Z = 1; l_Z < MAX_LOCALES; l_Z++)
                                            {
                                                std::string l_Str = l_SQLFields[l_SQLColumnNumber++].GetString();

                                                if (l_Str.empty())
                                                    continue;

                                                m_CustomStrings.push_back(l_Str);
                                                char * l_NewString = const_cast<char*>(m_CustomStrings.back().c_str());

                                                l_LocalizedString->Str[l_Z] = l_NewString;
                                            }
                                            l_WritePosition += sizeof(LocalizedString*);
                                            break;
                                        }
                                    }
                                }

                                l_ArrayLen = 1;
                            }

                        } while (l_SQLQueryResult->NextRow());
                    }
                }
            }

            /// Error in DB2 file at loading if NULL
            return m_IndexTable != nullptr;
        }

        /// Reload the data store
        bool Reload(std::string& p_OutMessage) override
        {
            if (!m_SQL)
            {
                p_OutMessage = "No sql settings found";
                return false;
            }

            std::string l_SQLQueryStr = "SELECT * FROM " + m_SQL->m_SQLTableName;

            int l_IndexPosition;
            int l_IndexSize;

            l_IndexPosition = GetIndexPositionAndSize(l_IndexSize);

            if (l_IndexPosition >= 0)
                l_SQLQueryStr += " ORDER BY " + m_SQL->m_IndexName + " DESC";
            l_SQLQueryStr += ';';

            std::size_t l_FieldCount = strlen(m_Format);
            std::size_t l_ArrayLen = 1;

            auto l_GetNumberOfDigits = [](int p_Num) -> int
            {
                return p_Num > 0 ? (int)log10((double)p_Num) + 1 : 1;
            };

            QueryResult l_SQLQueryResult = HotfixDatabase.Query(l_SQLQueryStr.c_str());
            if (l_SQLQueryResult)
            {
                Field* l_SQLFields = nullptr;
                l_SQLFields = l_SQLQueryResult->Fetch();

                int l_IndexPosition = 0;
                int l_IndexSize = 0;
                l_IndexPosition = GetIndexPositionAndSize(l_IndexSize);

                do
                {
                    l_SQLFields = l_SQLQueryResult->Fetch();

                    uint32 l_Entry = 0;

                    if (l_IndexSize == sizeof(uint32))
                        l_Entry = l_SQLFields[l_IndexPosition].GetUInt32();
                    else if (l_IndexSize == sizeof(uint16))
                        l_Entry = l_SQLFields[l_IndexPosition].GetUInt16();
                    else if (l_IndexSize == sizeof(uint8))
                        l_Entry = l_SQLFields[l_IndexPosition].GetUInt8();
                    else
                        assert(false);

                    T * l_NewEntry = (T*)LookupEntry(l_Entry);

                    if (!l_NewEntry)
                        l_NewEntry = new T();

                    char * l_WritePtr = (char*)l_NewEntry;

                    uint32 l_SQLColumnNumber = 0;
                    uint32 l_WritePosition = 0;

                    for (uint32 l_I = 0; l_I < l_FieldCount; ++l_I)
                    {
                        if (isdigit(m_Format[l_I]))
                        {
                            l_ArrayLen = atoi(m_Format + l_I);
                            l_I += (l_GetNumberOfDigits(l_ArrayLen) - 1);
                            continue;
                        }

                        for (uint32 l_Y = 0; l_Y < l_ArrayLen; ++l_Y)
                        {
                            switch (m_Format[l_I])
                            {
                                case FT_FLOAT:
                                    *((float*)(&l_WritePtr[l_WritePosition])) = l_SQLFields[l_SQLColumnNumber].GetFloat();
                                    l_WritePosition += sizeof(float);
                                    break;
                                case FT_INDEX:
                                case FT_INT:
                                case FT_INT_RELATIONSHIP:
                                    *((uint32*)(&l_WritePtr[l_WritePosition])) = l_SQLFields[l_SQLColumnNumber].GetUInt32();
                                    l_WritePosition += sizeof(uint32);
                                    break;
                                case FT_QWORD:
                                    *((uint64*)(&l_WritePtr[l_WritePosition])) = l_SQLFields[l_SQLColumnNumber].GetUInt64();
                                    l_WritePosition += sizeof(uint64);
                                    break;
                                case FT_WORD_INDEX:
                                case FT_SHORT:
                                case FT_WORD_RELATIONSHIP:
                                    *((uint16*)(&l_WritePtr[l_WritePosition])) = l_SQLFields[l_SQLColumnNumber].GetUInt16();
                                    l_WritePosition += sizeof(uint16);
                                    break;
                                case FT_BYTE:
                                case FT_BYTE_INDEX:
                                case FT_BYTE_RELATIONSHIP:
                                    *((uint8*)(&l_WritePtr[l_WritePosition])) = l_SQLFields[l_SQLColumnNumber].GetUInt8();
                                    l_WritePosition += sizeof(uint8);
                                    break;
                                case FT_STRING:
                                case FT_NSTRING:
                                {
                                    if (!LookupEntry(l_Entry))
                                    {
                                        *((LocalizedString**)(&l_WritePtr[l_WritePosition])) = new LocalizedString();
                                        m_LocalizedString.emplace(*((LocalizedString**)(&l_WritePtr[l_WritePosition])));
                                    }

                                    m_CustomStrings.push_back(l_SQLFields[l_SQLColumnNumber].GetString());

                                    char * l_NewString = const_cast<char*>(m_CustomStrings.back().c_str());

                                    LocalizedString * l_LocalizedString = *((LocalizedString**)(&l_WritePtr[l_WritePosition]));
                                    l_LocalizedString->Str[LOCALE_enUS] = l_NewString;

                                    l_WritePosition += sizeof(LocalizedString*);
                                    break;
                                }
                            }

                            l_SQLColumnNumber++;
                        }

                        l_ArrayLen = 1;
                    }

                    if (!LookupEntry(l_Entry))
                        AddEntry(l_Entry, l_NewEntry, true);

                } while (l_SQLQueryResult->NextRow());
            }

            uint32 l_StringCount = 0;
            for (uint32 l_I = 0; l_I < l_FieldCount; ++l_I)
            {
                if (isdigit(m_Format[l_I]))
                {
                    l_ArrayLen = atoi(m_Format + l_I);
                    l_I += (l_GetNumberOfDigits(l_ArrayLen) - 1);
                    continue;
                }

                for (uint32 l_Y = 0; l_Y < l_ArrayLen; ++l_Y)
                {
                    if (m_Format[l_I] == FT_STRING)
                        l_StringCount++;
                }

                l_ArrayLen = 1;
            }

            if (l_StringCount)
            {
                std::string l_SQLQueryStr = "SELECT * FROM " + m_SQL->m_SQLTableName + "_locale";

                if (l_IndexPosition >= 0)
                    l_SQLQueryStr += " ORDER BY " + m_SQL->m_IndexName + " DESC";
                l_SQLQueryStr += ';';

                l_SQLQueryResult = HotfixDatabase.Query(l_SQLQueryStr.c_str());
                if (l_SQLQueryResult)
                {
                    /// All locales (but not us), Index & BuildVerified
                    if (l_SQLQueryResult->GetFieldCount() != ((l_StringCount * (MAX_LOCALES - 1)) + 2))
                    {
                        p_OutMessage = "SQL format invalid for table : '" + m_SQL->m_SQLTableName + "_locale'";
                        return false;
                    }

                    do
                    {
                        Field* l_SQLFields          = l_SQLQueryResult->Fetch();
                        uint32 l_SQLColumnNumber    = 0;

                        uint32 l_EntryID = 0;

                        if (l_IndexSize == sizeof(uint32))
                            l_EntryID = l_SQLFields[l_SQLColumnNumber++].GetUInt32();
                        else if (l_IndexSize == sizeof(uint16))
                            l_EntryID = l_SQLFields[l_SQLColumnNumber++].GetUInt16();
                        else if (l_IndexSize == sizeof(uint8))
                            l_EntryID = l_SQLFields[l_SQLColumnNumber++].GetUInt8();
                        else
                            assert(false);

                        T * l_Entry = (T*)LookupEntry(l_EntryID);

                        if (!l_Entry)
                            continue;

                        char * l_WritePtr = (char*)l_Entry;

                        uint32 l_WritePosition = 0;
                        std::size_t l_ArrayLen = 1;

                        for (uint32 l_I = 0; l_I < l_FieldCount; ++l_I)
                        {
                            if (isdigit(m_Format[l_I]))
                            {
                                l_ArrayLen = atoi(m_Format + l_I);
                                l_I += (l_GetNumberOfDigits(l_ArrayLen) - 1);
                                continue;
                            }

                            for (uint32 l_Y = 0; l_Y < l_ArrayLen; ++l_Y)
                            {
                                switch (m_Format[l_I])
                                {
                                    case FT_FLOAT:
                                        l_WritePosition += sizeof(float);
                                        break;
                                    case FT_INDEX:
                                    case FT_INT:
                                    case FT_INT_RELATIONSHIP:
                                        l_WritePosition += sizeof(uint32);
                                        break;
                                    case FT_QWORD:
                                        l_WritePosition += sizeof(uint64);
                                        break;
                                    case FT_SHORT:
                                    case FT_WORD_INDEX:
                                    case FT_WORD_RELATIONSHIP:
                                        l_WritePosition += sizeof(uint16);
                                        break;
                                    case FT_BYTE:
                                    case FT_BYTE_INDEX:
                                    case FT_BYTE_RELATIONSHIP:
                                        l_WritePosition += sizeof(uint8);
                                        break;
                                    case FT_STRING:
                                    {
                                        LocalizedString * l_LocalizedString = *((LocalizedString**)(&l_WritePtr[l_WritePosition]));

                                        for (uint32 l_Y = 1; l_Y < MAX_LOCALES; l_Y++)
                                        {
                                            std::string l_Str = l_SQLFields[l_SQLColumnNumber++].GetString();

                                            if (l_Str.empty())
                                                continue;

                                            m_CustomStrings.push_back(l_Str);
                                            char * l_NewString = const_cast<char*>(m_CustomStrings.back().c_str());

                                            l_LocalizedString->Str[l_Y] = l_NewString;
                                        }
                                        l_WritePosition += sizeof(LocalizedString*);
                                        break;
                                    }
                                }

                                l_SQLColumnNumber++;
                            }

                            l_ArrayLen = 1;
                        }
                    } while (l_SQLQueryResult->NextRow());
                }
            }

            return true;
        }

        /// Load string from an other DB2
        /// @p_FileName : DB2 file path
        bool LoadStringsFrom(char const* p_FileName, uint32 p_Locale)
        {
            /// DB2 must be already loaded using Load
            if (!m_IndexTable)
                return false;

            DB2FileLoader l_DB2Reader;

            /// Check if load was successful, only then continue
            if (!l_DB2Reader.Load(p_FileName, m_Format))
                return false;

            m_DB2FileName = p_FileName;

            /// load strings from another locale db2 data
            l_DB2Reader.AutoProduceStrings(m_Format, (char*)m_DataTable, p_Locale);
            l_DB2Reader.AutoProduceCStrings(m_Format, (char*)m_DataTable, p_Locale);

            return true;
        }

        void GenerateSQLOuput(char const* p_FileName)
        {
            /// DB2 must be already loaded using Load
            if (!m_IndexTable)
                return;

            DB2FileLoader l_DB2Reader;

            /// Check if load was successful, only then continue
            if (!l_DB2Reader.Load(p_FileName, m_Format))
                return;

            m_DB2FileName = p_FileName;

            l_DB2Reader.GenerateSQLOutput(m_Format, (char*)m_DataTable);
        }

        /// Clear the DB2
        void Clear()
        {
            if (!m_IndexTable)
                return;

            delete[]((char*)m_IndexTable);
            delete[]((char*)m_RelationshipLookupTable);
            delete[]((char*)m_DataTable);

            m_IndexTable = nullptr;
            m_DataTable  = nullptr;

            for (typename DataTableEx::const_iterator l_It = m_DataTableEx.begin(); l_It != m_DataTableEx.end(); ++l_It)
                delete *l_It;

            for (auto l_Elem : m_LocalizedString)
                delete l_Elem;

            m_LocalizedString.clear();
            m_DataTableEx.clear();

            m_MaxID = 0;

            if (m_SQL)
                delete m_SQL;
        }

        std::string const& GetDB2FileName() const
        {
            return m_DB2FileName;
        }

        iterator begin() { return iterator(m_IndexTable, m_MaxID); }
        iterator end() { return iterator(m_IndexTable, m_MaxID, m_MaxID); }

    private:
        T** m_IndexTable;
        T** m_RelationshipLookupTable;

        T* m_DataTable;
        DataTableEx m_DataTableEx;
        std::list<std::string> m_CustomStrings;
        std::set<LocalizedString*> m_LocalizedString;
        SqlDb2 * m_SQL;
        std::string m_DB2FileName;
        bool m_LoadNew;
};

#endif
