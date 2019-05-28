#pragma once

#include <QDataStream>
#include "data/basestream.hpp"
#include "stl/bytearray.hpp"
#include "stl/string.hpp"


namespace data
{
    using BinaryStreamOpenMode = QIODevice::OpenModeFlag;

    class BinaryStream
            : protected QDataStream
            , public BaseStream
    {
    public:
        BinaryStream() = default;
        BinaryStream(const String& file_name,
                     const BinaryStreamOpenMode mode);
        BinaryStream(ByteArray* ba,
                     const BinaryStreamOpenMode mode);
        virtual ~BinaryStream() override;

        bool Open(const String& file_name,
                  const BinaryStreamOpenMode mode);
        void Close() override;

        bool IsOpen() const override;
        bool IsEOF() const override;

        bool SetPosition(const StreamOffsetOrigin from,
                         const int64 off) override;

        bool   ReadBool() override;
        sbyte  ReadByte() override;
        ubyte  ReadUByte() override;
        int8   ReadInt8() override;
        int16  ReadInt16() override;
        int32  ReadInt32() override;
        int64  ReadInt64() override;
        uint8  ReadUInt8() override;
        uint16 ReadUInt16() override;
        uint32 ReadUInt32() override;
        uint64 ReadUInt64() override;
        float  ReadFloat() override;
        double ReadDouble() override;
        char   ReadChar() override;
        uchar  ReadUChar() override;
        String ReadWord() override;
        String ReadLine() override;

        BinaryStream& WriteBool(const bool val) override;
        BinaryStream& WriteByte(const sbyte val) override;
        BinaryStream& WriteUByte(const ubyte val) override;
        BinaryStream& WriteInt8(const int8 val) override;
        BinaryStream& WriteInt16(const int16 val) override;
        BinaryStream& WriteInt32(const int32 val) override;
        BinaryStream& WriteInt64(const int64 val) override;
        BinaryStream& WriteUInt8(const uint8 val) override;
        BinaryStream& WriteUInt16(const uint16 val) override;
        BinaryStream& WriteUInt32(const uint32 val) override;
        BinaryStream& WriteUInt64(const uint64 val) override;
        BinaryStream& WriteFloat(const float val) override;
        BinaryStream& WriteDouble(const double val) override;
        BinaryStream& WriteChar(const char val) override;
        BinaryStream& WriteUChar(const uchar val) override;
        BinaryStream& WriteString(const String& s) override;

    private:
        using Base = QDataStream;
    };
}
