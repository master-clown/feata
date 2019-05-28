#pragma once

#include "defs/common.hpp"
#include "stl/string.hpp"


namespace data
{
    enum StreamOffsetOrigin
    {
        STREAM_OFFSET_ORIGIN_BEG,
        STREAM_OFFSET_ORIGIN_CUR
    };

    class BaseStream
    {
    public:
        virtual ~BaseStream() = default;

        virtual void Close() = 0;
        virtual bool IsOpen() const = 0;
        virtual bool IsEOF() const = 0;

        virtual bool SetPosition(const StreamOffsetOrigin from,
                                 const int64 off) = 0;

        virtual bool    ReadBool() = 0;
        virtual sbyte   ReadByte() = 0;
        virtual ubyte   ReadUByte() = 0;
        virtual int8    ReadInt8() = 0;
        virtual int16   ReadInt16() = 0;
        virtual int32   ReadInt32() = 0;
        virtual int64   ReadInt64() = 0;
        virtual uint8   ReadUInt8() = 0;
        virtual uint16  ReadUInt16() = 0;
        virtual uint32  ReadUInt32() = 0;
        virtual uint64  ReadUInt64() = 0;
        virtual float   ReadFloat() = 0;
        virtual double  ReadDouble() = 0;
        virtual char    ReadChar() = 0;
        virtual uchar   ReadUChar() = 0;
        virtual String  ReadWord() = 0;
        virtual String  ReadLine() = 0;
        virtual void    SkipWords(const uint count) { for(uint i = 0; i < count; ++i) ReadWord(); }
        virtual void    SkipLines(const uint count) { for(uint i = 0; i < count; ++i) ReadLine(); }

        virtual BaseStream& WriteBool(const bool val) = 0;
        virtual BaseStream& WriteByte(const sbyte val) = 0;
        virtual BaseStream& WriteUByte(const ubyte val) = 0;
        virtual BaseStream& WriteInt8(const int8 val) = 0;
        virtual BaseStream& WriteInt16(const int16 val) = 0;
        virtual BaseStream& WriteInt32(const int32 val) = 0;
        virtual BaseStream& WriteInt64(const int64 val) = 0;
        virtual BaseStream& WriteUInt8(const uint8 val) = 0;
        virtual BaseStream& WriteUInt16(const uint16 val) = 0;
        virtual BaseStream& WriteUInt32(const uint32 val) = 0;
        virtual BaseStream& WriteUInt64(const uint64 val) = 0;
        virtual BaseStream& WriteFloat(const float val) = 0;
        virtual BaseStream& WriteDouble(const double val) = 0;
        virtual BaseStream& WriteChar(const char val) = 0;
        virtual BaseStream& WriteUChar(const uchar val) = 0;
        virtual BaseStream& WriteString(const String& s) = 0;
    };
}
