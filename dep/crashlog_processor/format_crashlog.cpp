#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <regex>
#include <unordered_map>
#include <stdio.h>
#include <stdlib.h>
using uint32 = unsigned long;

int main(int argc, char* argv[])
{
    if (argc < 3)
        return 4;

    std::string l_CrashLog = argv[1];
    std::string l_PointersLog = argv[2];

    std::string l_FreezeThread;
    std::string l_FreezeInfo;
    if (argc >= 4)
    {
        l_FreezeInfo = argv[3];
        l_FreezeThread = argv[4];
    }

    std::string l_Output = l_CrashLog.substr(0, l_CrashLog.length() - 3) + "html";

#ifdef WIN32
    FILE* f_Pointer;
    fopen_s(&f_Pointer, l_PointersLog.c_str(), "r+");
#else
    FILE* f_Pointer = fopen(l_PointersLog.c_str(), "r+");
#endif // WIN32
    if (!f_Pointer)
        return 1;

#ifdef WIN32
    FILE* f_CrashLog;
    fopen_s(&f_CrashLog, l_CrashLog.c_str(), "r+");
#else
    FILE* f_CrashLog = fopen(l_CrashLog.c_str(), "r+");
#endif // WIN32
    if (!f_CrashLog)
        return 2;

    struct PointerData
    {
        PointerData(){}
        PointerData(std::string p_Type, std::string p_Data) : Type(p_Type), Data(p_Data) { }

        std::string GetData() const
        {
            std::string l_Data = "<b><span style=\"color:";

            if (Type == "Spell")
                l_Data += "pink";
            else if (Type == "Creature")
                l_Data += "blue";
            else if (Type == "Player")
                l_Data += "red";
            else if (Type == "GameObject")
                l_Data += "green";
            else if (Type == "GameObjectModel")
                l_Data += "lightgreen";
            else if (Type == "AreaTrigger")
                l_Data += "orange";
            else if (Type == "WorldSession")
                l_Data += "salmon";
            else if (Type == "Map")
                l_Data += "purple";
            else
                l_Data += "black";

            l_Data += "\">" + Type + Data + "</span></b>";
            return l_Data;
        }

        std::string Type;
        std::string Data;
    };

    std::unordered_map<std::string, PointerData> l_PointersData;

    char l_Buffer[255];
    while (fgets(l_Buffer, 255, f_Pointer))
    {
        uint32 l_I = 0;
        std::string l_Pointer;
        std::string l_Type;
        std::string l_Data;
        while (l_Buffer[l_I++] != ' ');
        while (l_Buffer[l_I++] != ' ');
        while (l_Buffer[l_I++] != 'x');
        while (l_Buffer[l_I] != ' ')
            l_Pointer += l_Buffer[l_I++];
        l_I++;
        while (l_Buffer[l_I++] != ' ');
        while (l_Buffer[l_I] != ' ')
            l_Type += l_Buffer[l_I++];
        while (l_Buffer[l_I] != '\n')
            l_Data += l_Buffer[l_I++];

        l_PointersData[l_Pointer] = PointerData(l_Type, l_Data);
    }

    fclose(f_Pointer);

#ifdef WIN32
    FILE* f_Output;
    fopen_s(&f_Output, l_Output.c_str(), "w+");
#else
    FILE* f_Output = fopen(l_Output.c_str(), "w+");
#endif // WIN32
    if (!f_Output)
        return 3;

    std::string l_Format = "<html style='font-family:consolas;font-size:13'><br/>";
    fputs(l_Format.c_str(), f_Output);

    if (l_FreezeInfo != "")
        fputs(("<b><span style='color:blue'>" + l_FreezeInfo + "</span></b><br/>").c_str(), f_Output);


    if (l_FreezeThread != "")
        fputs(("Froze in thread ID : <b><span style='color:red'>" + l_FreezeThread + "</span></b><br/><br/>").c_str(), f_Output);

    std::string l_Line;
    std::string l_NewLine;
    memset(l_Buffer, 0, 255);
    while (fgets(l_Buffer, 255, f_CrashLog))
    {
        l_Line += l_Buffer;
        if (l_Buffer[254] != '\0')
        {
            memset(l_Buffer, 0, 255);
            continue;
        }

        for (uint32 l_I = 0; l_I < l_Line.size(); ++l_I)
        {
            if (l_Line[l_I] != '0')
            {
                switch (l_Line[l_I])
                {
                    case '>':
                        l_NewLine += "&gt;";
                        break;
                    case '<':
                        l_NewLine += "&lt;";
                        break;
                    default:
                        l_NewLine += l_Line[l_I];
                        break;
                }

                continue;
            }

            uint32 l_J = l_I + 1;
            if (l_Line[l_J++] != 'x')
            {
                l_NewLine += l_Line[l_I];
                continue;
            }

            std::string l_Pointer;
            while (l_J < l_Line.size()
                && ((l_Line[l_J] >= '0' && l_Line[l_J] <= '9')
                || (l_Line[l_J] >= 'a' && l_Line[l_J] <= 'f')))
                l_Pointer += l_Line[l_J++];

            l_I = l_J;

            if (l_PointersData.find(l_Pointer) != l_PointersData.end())
                l_NewLine += l_PointersData[l_Pointer].GetData();
            else
                l_NewLine += "0x" + l_Pointer;
        }

        fputs((l_NewLine + "<br/>").c_str(), f_Output);

        l_Line = "";
        l_NewLine = "";
        memset(l_Buffer, 0, 255);
    }

    l_Format = "</html>";
    fputs(l_Format.c_str(), f_Output);

    fclose(f_CrashLog);
    fclose(f_Output);

    return 0;
}
