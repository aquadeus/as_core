////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Common.h"
#include <fstream>
#include <iostream>

template <class T>
class GameTable
{
public:

    explicit GameTable(std::string p_Key = "") : m_Storage(nullptr), m_Key(p_Key), m_FieldsCount(0), m_RowsCount(0), m_MaxID(0)
    {
    }

    ~GameTable()
    {
        if (m_Storage)
        {
            delete[] m_Storage;
        }
    }

    bool Load(std::string p_FileName)
    {
        std::ifstream l_File(p_FileName.c_str());

        if (!l_File.is_open())
            return false;

        std::string l_Line;

        if (!std::getline(l_File, l_Line))
            return false;

        Tokenizer l_Header(l_Line, 9); ///< The token is a TAB(9) not a space
        m_Names.resize(l_Header.size());
        uint32 l_KeyIndex = -1;

        for (uint32 l_I = 0; l_I < l_Header.size(); ++l_I)
        {
            m_Names[l_I] = l_Header[l_I];

            if (m_Names[l_I] == m_Key)
                l_KeyIndex = m_FieldsCount;

            ++m_FieldsCount;
        }

        if (!m_FieldsCount || l_KeyIndex == -1 || m_FieldsCount != sizeof(T) / sizeof(float))
            return false;

        long long l_DataPos = l_File.tellg();
        while (std::getline(l_File, l_Line))
            ++m_RowsCount;

        l_File.clear();
        l_File.seekg(l_DataPos, std::fstream::beg);

        if (!m_RowsCount)
            return false;

        m_Storage = new T[m_RowsCount];
        memset(m_Storage, 0, m_RowsCount * sizeof(T));

        uint32 l_ID = 0;

        while (std::getline(l_File, l_Line))
        {
            Tokenizer l_Values(l_Line, 9);

            for (uint32 l_I = 0; l_I < l_Values.size(); ++l_I)
            {
                char const* l_Value = l_Values[l_I];

                if (l_I == l_KeyIndex)
                {
                    uint32 l_RowID = atoi(l_Value);
                    *(((uint32*)(&m_Storage[l_ID])) + l_I) = l_RowID;
                    m_MaxID = std::max(l_RowID, m_MaxID);

                }
                else
                    *(((float*)(&m_Storage[l_ID])) + l_I) = (float)atof(l_Value);
            }
            ++l_ID;
        }

        m_ReferenceTable.resize(m_MaxID + 1);
        std::fill(m_ReferenceTable.begin(), m_ReferenceTable.end(), nullptr);

        for (uint32 l_I = 0; l_I < m_RowsCount; ++l_I)
        {
            m_ReferenceTable[*(((uint32*)(&m_Storage[l_I])) + l_KeyIndex)] = &m_Storage[l_I];
        }

        return true;
    }

    uint32 GetTableRowCount() const { return uint32(m_ReferenceTable.size()); }
    std::vector<T*>& GetData() { return m_ReferenceTable; }

    T const* LookupEntry(uint32 p_ID)
    {
        return p_ID > m_MaxID  || !m_Storage ? nullptr : m_ReferenceTable[p_ID];
    }

private:
    T* m_Storage;
    std::string m_Key;
    uint32 m_FieldsCount;
    uint32 m_RowsCount;
    uint32 m_MaxID;
    std::vector<T*> m_IndexTable;
    std::vector<std::string> m_Names;
    std::vector<T*> m_ReferenceTable;
};
