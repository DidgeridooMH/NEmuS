#include <fstream>

#include <quazip.h>
#include <quazipfile.h>

#include <fmt/printf.h>

#include <Utils/Filesystem.hpp>

namespace nemus::utils
{
  static constexpr const char *NesFileExtension = ".nes";
  static constexpr const char *ZipFileExtension = ".zip";

  static std::vector<char> loadRomFile(const QString &filename)
  {
    std::vector<char> contents;
    std::ifstream file(filename.toStdString(), std::ios::ate | std::ios::binary);
    if (file.is_open())
    {
      auto size = file.tellg();
      file.seekg(file.beg);

      contents.resize(size);
      file.read(contents.data(), size);
    }
    else
    {
      throw FilesystemException::UnableToOpenFile(filename.toStdString());
    }
    return contents;
  }

  static std::vector<char> loadArchive(const QString &filename)
  {
    QuaZip file(filename);
    if (!file.open(QuaZip::Mode::mdUnzip))
    {
      throw FilesystemException::UnableToOpenFile(filename.toStdString());
    }

    auto fileList = file.getFileNameList();
    auto romFilename = std::find_if(fileList.begin(), fileList.end(), [](const QString &f)
                                    { return f.endsWith(NesFileExtension); });
    if (romFilename != fileList.end())
    {
      file.setCurrentFile(*romFilename);
      QuaZipFile romFile(&file);
      if (!romFile.open(QIODeviceBase::ReadOnly))
      {
        throw FilesystemException::CorruptArchive(filename.toStdString());
      }
      auto romContents = romFile.readAll();
      return std::vector(romContents.begin(), romContents.end());
    }

    throw FilesystemException::NoRomInArchive(filename.toStdString());
  }

  std::vector<char> loadFile(const QString &filename)
  {
    std::vector<char> fileContents;
    if (filename.endsWith(NesFileExtension))
    {
      return loadRomFile(filename);
    }
    else if (filename.endsWith(ZipFileExtension))
    {
      return loadArchive(filename);
    }

    throw FilesystemException::InvalidFileExtension(filename.toStdString());
  }
} // namespace nemus::utils
