#pragma once


namespace plug { class PluginSettings; }

namespace core
{
    class CompPluggable
    {
    public:
        virtual ~CompPluggable() = default;

        virtual plug::PluginSettings* GetPluginSettings() const = 0;
    };
}
