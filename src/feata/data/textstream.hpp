#pragma once

#include <QTextStream>
#include "data/basestream.hpp"
#include "stl/bytearray.hpp"
#include "stl/string.hpp"


namespace data
{
    using TextStreamOpenMode = QIODevice::OpenModeFlag;

    class TextStream
            : protected QTextStream
            , public BaseStream
    {
    public:
        TextStream() = default;
        TextStream(const String& file_name,
                   const TextStreamOpenMode mode);
        TextStream(String* str,
                   const TextStreamOpenMode mode);
        TextStream(ByteArray* str,
                   const TextStreamOpenMode mode);
        virtual ~TextStream() override;

        bool Open(const String& file_name,
                  const TextStreamOpenMode mode);
        bool Open(String* str,
                  const TextStreamOpenMode mode);
        void Close() override;

        bool IsOpen() const override;
        bool IsOpenFile() const;
        bool IsOpenStr() const;
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

        TextStream& WriteBool(const bool val) override;
        TextStream& WriteByte(const sbyte val) override;
        TextStream& WriteUByte(const ubyte val) override;
        TextStream& WriteInt8(const int8 val) override;
        TextStream& WriteInt16(const int16 val) override;
        TextStream& WriteInt32(const int32 val) override;
        TextStream& WriteInt64(const int64 val) override;
        TextStream& WriteUInt8(const uint8 val) override;
        TextStream& WriteUInt16(const uint16 val) override;
        TextStream& WriteUInt32(const uint32 val) override;
        TextStream& WriteUInt64(const uint64 val) override;
        TextStream& WriteFloat(const float val) override;
        TextStream& WriteDouble(const double val) override;
        TextStream& WriteChar(const char val) override;
        TextStream& WriteUChar(const uchar val) override;
        TextStream& WriteString(const String& s) override;

    private:
        using Base = QTextStream;
    };
}
