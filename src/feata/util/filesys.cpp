#include <QDataStream>
#include <QTextStream>
#include <QDir>
#include "util/filesys.hpp"


namespace util::filesys
{
	bool IsFileExists(const String& file)
	{
		return QFile(file).exists();
	}

	String GetFileName(const String& path)
	{
		return QFileInfo(path).fileName();
	}

    String GetFileExt(const String& path)
    {
        return QFileInfo(path).completeSuffix();
    }

    String GetFileNameWithoutExt(String file)
    {
        file.chop(QFileInfo(file).completeSuffix().size() + 1);
        return file;
    }

    StringList GetFilesInDir(const String& dir,
                             const StringList& filters)
	{
        StringList res;
		const auto lst = QDir(dir).entryInfoList(filters);

		for(const auto& file: lst)
			res << file.absoluteFilePath();

		return res;
	}

	void ChangeCurrentDir(const String& new_cwd)
	{
		QDir::setCurrent(new_cwd);
	}


	bool ReadFile(const String& file_name,
				  String& content)
	{
		if(file_name.trimmed().isEmpty())
			return false;

		QFile file(file_name);

		if(!file.open(QFile::ReadOnly))
			return false;

		QTextStream stream(&file);
		stream.setCodec("UTF-8");

		content = stream.readAll();

		file.close();

		return true;
	}

	String ReadFile(const String& file_name)
	{
		String content;

		ReadFile(file_name, content);

		return content;
	}

	bool WriteToFile(const String& file_name,
					 const String& content)
	{
		if(file_name.trimmed().isEmpty())
			return false;

		QFile file(file_name);

		if(!file.open(QFile::WriteOnly | QFile::Truncate))
			return false;

		QTextStream stream(&file);
		stream.setCodec("UTF-8");

		stream << content;

		file.close();

		return true;
	}

	bool WriteToFile(const String& file_name,
					 const ByteArray& data)
	{
		if(file_name.trimmed().isEmpty())
			return false;

		QFile file(file_name);

		if(!file.open(QFile::WriteOnly | QFile::Truncate))
			return false;

		QDataStream stream(&file);

		stream << data;

		file.close();

		return true;
	}

	bool AppendToFile(const String& file_name,
					  const String& content)
	{
		if(file_name.trimmed().isEmpty())
			return false;

		QFile file(file_name);

		if(!file.open(QFile::WriteOnly | QFile::Append))
			return false;

		QTextStream stream(&file);
		stream.setCodec("UTF-8");

		stream << content;

		file.close();

		return true;
	}
}
