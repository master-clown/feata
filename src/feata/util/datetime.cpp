#include <QDateTime>
#include "util/datetime.hpp"


namespace util::time
{
	String GetCurrentDateTime(const String& format)
	{
        return QDateTime::currentDateTime().toString(format);
	}
}
