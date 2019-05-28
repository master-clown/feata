#include "plug/version.hpp"


namespace plug
{
    Version::Version(uint64 encoded_ver)
        : Version(encoded_ver & 0xFFFF000000000000,
                  encoded_ver & 0x0000FFFF00000000,
                  encoded_ver & 0x00000000FFFF0000,
                  encoded_ver & 0x000000000000FFFF)
    {}

    Version::Version(uint16 major,
                     uint16 minor,
                     uint16 build,
                     uint16 revision)
        : Major(major)
        , Minor(minor)
        , Build(build)
        , Revis(revision)
    {}

    Version::Version(const String& str)
    {
        Parse(str);
    }

    uint64 Version::AsEncoded() const
    {
        return ((uint64)Major << 48) |
               ((uint64)Minor << 32) |
               ((uint64)Build << 16) |
               ((uint64)Revis <<  0);
    }

    bool Version::Parse(const String& str)
    {
        bool ok;
        const auto lst = str.split('.');
        uint16* field_lst = reinterpret_cast<uint16*>(this);   // to interpret the object as array of uint16's

        for(uint8 i = 0; i < 4; ++i)
            if(uint16 tmp = lst[i].toUInt(&ok); ok)
                field_lst[i] = tmp;
            else
                return false;

        return true;
    }
}

template<>
String TO_STRING(const plug::Version& ver)
{
    return String("%1.%2.%3.%4").arg(ver.Major).arg(ver.Minor)
                                .arg(ver.Build).arg(ver.Revis);
}
