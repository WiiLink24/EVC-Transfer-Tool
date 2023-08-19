#pragma once

#include <ogcsys.h>
#include <string_view>

class EVCSaveFile final
{
public:
    explicit EVCSaveFile();
    void RemoveLarsen();
    bool FlushData();
    std::string_view Error() const;

private:
    static constexpr u32 FILE_SIZE_BEFORE_CRC = 125216;

    static std::string_view GetSavePath();
    u32 CalculateCRC32() const;

    using RegionName = u8[128];
    using Result = u8[256];

#pragma pack(push, 1)
    struct Mii final
    {
        u32 mii_id;
        u32 mac_address;
        u8 name[28];
        u8 unknown[124];
    };
    static_assert(sizeof(Mii) == 160, "Mii size invalid");

    struct NationalQuestion final
    {
        u32 question_id;
        u32 start_timestamp;
        u32 end_timestamp;
        u64 unknown;
        u8 question[200][4];
        u8 response_1[80][4];
        u8 response_2[80][4];
        u8 padding[44];
    };
    static_assert(sizeof(NationalQuestion) == 1504, "NationalQuestion size invalid");

    struct WorldWideQuestion final
    {
        u32 question_id;
        u32 start_timestamp;
        u32 end_timestamp;
        u64 unknown;
        u8 question[200][4];
        u8 response_1[80][4];
        u8 response_2[80][4];
        u8 padding[60];
    };
    static_assert(sizeof(WorldWideQuestion) == 1520, "WorldWideQuestion size invalid");

    struct SaveFile final
    {
        u16 magic_number;
        u8 language_index;
        u8 unknown[29];
        RegionName country_names[16];
        RegionName province_names[16];
        Mii miis[6];
        NationalQuestion national_questions[6];
        WorldWideQuestion worldwide_questions[2];
        Result results[12];
        Result recent_results[2];
        u8 unknown2[40992];
        NationalQuestion results_text[12];
        NationalQuestion recent_national_question;
        WorldWideQuestion recent_worldwide_question;
        u8 unknown3[42416];
        u32 crc32;
        u8 unknown4[5852];
    };
    static_assert(sizeof(SaveFile) == 131072, "SaveFile size invalid");
#pragma pack(pop)

    SaveFile m_data;
    std::string_view m_filepath;
    std::string_view m_error;
};