#include "data/json.hpp"
#include "util/log.hpp"


namespace data::json
{
    JsonDocument ParseFromString(const String& json_text)
    {
        QJsonParseError err;
        const auto doc = QJsonDocument::fromJson(json_text.toUtf8(), &err);

        if(err.error != QJsonParseError::NoError)
        {
            util::logger::PrintE("Error during parse of JSON document: %1;\n"
                                 "Offset of the error in the input string: %2"_qs
                                 .arg(err.errorString()).arg(err.offset));
        }

        return doc;
    }
}
