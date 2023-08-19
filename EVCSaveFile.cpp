#include "EVCSaveFile.h"
#include "crc32.h"
#include "ISFS.h"

#include <stdexcept>
#include <cstring>

constexpr const char SAVE_FILE_PATH_US[] = "/title/00010001/48414a45/data/VoteCh.dat";
constexpr const char SAVE_FILE_PATH_JPN[] = "/title/00010001/48414a4a/data/VoteCh.dat";
constexpr const char SAVE_FILE_PATH_PAL[] = "/title/00010001/48414a50/data/VoteCh.dat";

EVCSaveFile::EVCSaveFile()
{
  u32 readSize;
  m_filepath = EVCSaveFile::GetSavePath();
  if (m_filepath.empty())
  {
    m_error = "Unsupported region detected";
    return;
  }

  void *fileBuffer = ISFS_GetFile(m_filepath.data(), &readSize);
  if (fileBuffer == nullptr)
  {
    m_error = "Error reading EVC save file";
    return;
  }

  m_data = *(reinterpret_cast<SaveFile*>(fileBuffer));
}

std::string_view EVCSaveFile::GetSavePath()
{
  switch (CONF_GetRegion()) {
    case CONF_REGION_JP:
      return SAVE_FILE_PATH_JPN;
    case CONF_REGION_US:
      return SAVE_FILE_PATH_US;
    case CONF_REGION_EU:
      return SAVE_FILE_PATH_PAL;
    default:
      return {};
  }
}

void EVCSaveFile::RemoveLarsen()
{
  // Remove current polls and recent results.
  std::memset(&m_data.national_questions, 0, sizeof(m_data.national_questions));
  std::memset(&m_data.worldwide_questions, 0, sizeof(m_data.worldwide_questions));
  std::memset(&m_data.recent_results, 0, sizeof(m_data.recent_results));
  std::memset(&m_data.recent_national_question, 0, sizeof(m_data.recent_national_question));
  std::memset(&m_data.recent_worldwide_question, 0, sizeof(m_data.recent_worldwide_question));
}

bool EVCSaveFile::FlushData()
{
  // Recalculate checksum
  m_data.crc32 = CalculateCRC32();

  s32 fd = ISFS_Open(m_filepath.data(), ISFS_OPEN_WRITE);
  if (fd < 0) {
    printf("Error opening file at %s\n", m_filepath.data());
    return false;
  }

  s32 ret = ISFS_Write(fd, reinterpret_cast<const u8 *>(&m_data), sizeof(m_data));
  if (ret < 0) {
    printf("Error writing file at %s\n", m_filepath.data());
    return false;
  }

  ret = ISFS_Close(fd);
  if (ret < 0) {
    printf("Error closing file at %s\n", m_filepath.data());
    return false;
  }

  return true;
}

std::string_view EVCSaveFile::Error() const
{
  return m_error;
}

u32 EVCSaveFile::CalculateCRC32() const
{
  return crc32(reinterpret_cast<const u8*>(&m_data), FILE_SIZE_BEFORE_CRC);
}