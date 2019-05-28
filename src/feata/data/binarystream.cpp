#include <QFile>
#include "data/binarystream.hpp"


namespace data
{
    BinaryStream::BinaryStream(const String& file_name,
                           const BinaryStreamOpenMode mode)
    {
        Open(file_name, mode);
    }

    BinaryStream::BinaryStream(ByteArray* ba,
                           const BinaryStreamOpenMode mode)
        : Base(ba, mode)
    {}

    BinaryStream::~BinaryStream()
    {
        Close();
    }

    bool BinaryStream::Open(const String& file_name,
              const BinaryStreamOpenMode mode)
    {
        Close();

        auto file_ = new QFile(file_name);
        file_->open(mode);
        if(!file_->isOpen())
            return false;

        setDevice(file_);

        return true;
    }

    void BinaryStream::Close()
    {
        if(!IsOpen())
            return;

        device()->close();
        delete device();

        setDevice(nullptr);
    }

    bool BinaryStream::IsOpen() const
    {
        return device() && device()->isOpen();
    }

    bool BinaryStream::IsEOF() const
    {
        return atEnd();
    }

    bool BinaryStream::SetPosition(const StreamOffsetOrigin from,
                                   const int64 off)
    {
        if(!IsOpen())
            return false;

        return device()->seek((from == STREAM_OFFSET_ORIGIN_CUR ? device()->pos() : 0) + off);
    }

#define READ_FUNC_IMPL(Type) { Type tmp; *this >> tmp; return tmp; }

    bool   BinaryStream::ReadBool()     READ_FUNC_IMPL(bool)
    sbyte  BinaryStream::ReadByte()     READ_FUNC_IMPL(sbyte)
    ubyte  BinaryStream::ReadUByte()    READ_FUNC_IMPL(ubyte)
    int8   BinaryStream::ReadInt8()     READ_FUNC_IMPL(int8)
    int16  BinaryStream::ReadInt16()    READ_FUNC_IMPL(int16)
    int32  BinaryStream::ReadInt32()    READ_FUNC_IMPL(int32)
    int64  BinaryStream::ReadInt64()    READ_FUNC_IMPL(int64)
    uint8  BinaryStream::ReadUInt8()    READ_FUNC_IMPL(uint8)
    uint16 BinaryStream::ReadUInt16()   READ_FUNC_IMPL(uint16)
    uint32 BinaryStream::ReadUInt32()   READ_FUNC_IMPL(uint32)
    uint64 BinaryStream::ReadUInt64()   READ_FUNC_IMPL(uint64)
    float  BinaryStream::ReadFloat()    READ_FUNC_IMPL(float)
    double BinaryStream::ReadDouble()   READ_FUNC_IMPL(double)
    char   BinaryStream::ReadChar()     READ_FUNC_IMPL(sbyte)
    uchar  BinaryStream::ReadUChar()    READ_FUNC_IMPL(ubyte)
    String BinaryStream::ReadLine()     { return device()->readLine(); }

    String BinaryStream::ReadWord()
    {
        QByteArray data;
        for(uchar c; !isspace(c = ReadUChar()); data.push_back(c));

        SetPosition(STREAM_OFFSET_ORIGIN_CUR, -1);

        return data;
    }

    BinaryStream& BinaryStream::WriteBool(const bool val)      { *this << val; return *this; }
    BinaryStream& BinaryStream::WriteByte(const sbyte val)     { *this << val; return *this; }
    BinaryStream& BinaryStream::WriteUByte(const ubyte val)    { *this << val; return *this; }
    BinaryStream& BinaryStream::WriteInt8(const int8 val)      { *this << val; return *this; }
    BinaryStream& BinaryStream::WriteInt16(const int16 val)    { *this << val; return *this; }
    BinaryStream& BinaryStream::WriteInt32(const int32 val)    { *this << val; return *this; }
    BinaryStream& BinaryStream::WriteInt64(const int64 val)    { *this << val; return *this; }
    BinaryStream& BinaryStream::WriteUInt8(const uint8 val)    { *this << val; return *this; }
    BinaryStream& BinaryStream::WriteUInt16(const uint16 val)  { *this << val; return *this; }
    BinaryStream& BinaryStream::WriteUInt32(const uint32 val)  { *this << val; return *this; }
    BinaryStream& BinaryStream::WriteUInt64(const uint64 val)  { *this << val; return *this; }
    BinaryStream& BinaryStream::WriteFloat(const float val)    { *this << val; return *this; }
    BinaryStream& BinaryStream::WriteDouble(const double val)  { *this << val; return *this; }
    BinaryStream& BinaryStream::WriteChar(const char val)      { *this << val; return *this; }
    BinaryStream& BinaryStream::WriteUChar(const uchar val)    { *this << val; return *this; }
    BinaryStream& BinaryStream::WriteString(const String& s)   { *this << s; return *this; }
}
