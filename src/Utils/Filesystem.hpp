#pragma once

#include <exception>
#include <vector>
#include <string>

#include <QString>

namespace nemus::utils
{
  class FilesystemException : std::exception
  {
  public:
    static FilesystemException UnableToOpenFile(const std::string &filename)
    {
      return FilesystemException("Unable to open file: " + filename);
    }

    static FilesystemException NoRomInArchive(const std::string &filename)
    {
      return FilesystemException("No rom found in archive: \"" + filename + "\"");
    }

    static FilesystemException CorruptArchive(const std::string &filename)
    {
      return FilesystemException("Unable to load rom. Potentially corrupted archive: " + filename);
    }

    // TODO: Detect file without file extension.
    static FilesystemException InvalidFileExtension(const std::string &filename)
    {
      return FilesystemException("Invalid file extension: " + filename);
    }

    const char *what() const noexcept
    {
      return message.c_str();
    }

  private:
    FilesystemException(const std::string &message) : message(message) {}

    std::string message;
  };

  std::vector<char> loadFile(const QString &filename);
} // namespace nemus::utils
