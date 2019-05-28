#pragma once

#include "stl/bytearray.hpp"
#include "stl/string.hpp"


namespace util::filesys
{
	bool IsFileExists(const String& file);
	String GetFileName(const String& path);
    String GetFileExt(const String& path);
    String GetFileNameWithoutExt(String file);

    StringList GetFilesInDir(const String& dir,
                             const StringList& filters = StringList());

	void ChangeCurrentDir(const String& new_cwd);

	bool ReadFile(const String& file_name,
				  String& content);
	String ReadFile(const String& file_name);
	bool WriteToFile(const String& file_name,
					 const String& content);

	bool WriteToFile(const String& file_name,
					 const ByteArray& data);
	bool AppendToFile(const String& file_name,
					  const String& content);
}
