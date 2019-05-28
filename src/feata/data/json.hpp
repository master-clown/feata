#pragma once

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include "stl/string.hpp"


namespace data::json
{
    using JsonObject = QJsonObject;
    using JsonDocument = QJsonDocument;
    using JsonArray = QJsonArray;
    using JsonValue = QJsonValue;

    JsonDocument ParseFromString(const String& json_text);
}
