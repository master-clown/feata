#include <QFile>
#include "textstream.hpp"


namespace data
{
    TextStream::TextStream(const String& file_name,
                           const TextStreamOpenMode mode)
    {
        Open(file_name, mode);
    }

    TextStream::TextStream(String* str,
                           const TextStreamOpenMode mode)
    {
        Open(str, mode);
    }

    TextStream::TextStream(ByteArray* ba,
                           const TextStreamOpenMode mode)
        : Base(ba, mode)
    {}

    TextStream::~TextStream()
    {
        Close();
    }

    bool TextStream::Open(const String& file_name,
              const TextStreamOpenMode mode)
    {
        Close();

        auto file_ = new QFile(file_name);
        file_->open(mode);
        if(!file_->isOpen())
            return false;

        setDevice(file_);

        return true;
    }

    bool TextStream::Open(String* str,
                          const TextStreamOpenMode mode)
    {
        Close();

        setString(str, mode);

        return true;
    }

    void TextStream::Close()
    {
        if(IsOpenFile())
        {
            device()->close();
            delete device();

            setDevice(nullptr);
        }
        else
        if(IsOpenStr())
        {
            setString(nullptr);
        }
    }

    bool TextStream::IsOpen() const
    {
        return IsOpenFile() || IsOpenStr();
    }

    bool TextStream::IsOpenFile() const
    {
        return device() && device()->isOpen();
    }

    bool TextStream::IsOpenStr() const
    {
        return string();
    }

    bool TextStream::IsEOF() const
    {
        return atEnd();
    }

    bool TextStream::SetPosition(const StreamOffsetOrigin from,
                                 const int64 off)
    {
        if(!IsOpen())
            return false;

        return seek((from == STREAM_OFFSET_ORIGIN_CUR ? pos() : 0) + off);
    }

#define READ_FUNC_IMPL(Type) { Type tmp; *this >> tmp; return tmp; }

    bool   TextStream::ReadBool()   READ_FUNC_IMPL(char)
    sbyte  TextStream::ReadByte()   READ_FUNC_IMPL(char)
    ubyte  TextStream::ReadUByte()  READ_FUNC_IMPL(char)
    int8   TextStream::ReadInt8()   READ_FUNC_IMPL(char)
    int16  TextStream::ReadInt16()  READ_FUNC_IMPL(int16)
    int32  TextStream::ReadInt32()  READ_FUNC_IMPL(int32)
    int64  TextStream::ReadInt64()  READ_FUNC_IMPL(int64)
    uint8  TextStream::ReadUInt8()  READ_FUNC_IMPL(char)
    uint16 TextStream::ReadUInt16() READ_FUNC_IMPL(uint16)
    uint32 TextStream::ReadUInt32() READ_FUNC_IMPL(uint32)
    uint64 TextStream::ReadUInt64() READ_FUNC_IMPL(uint64)
    float  TextStream::ReadFloat()  READ_FUNC_IMPL(float)
    double TextStream::ReadDouble() READ_FUNC_IMPL(double)
    char   TextStream::ReadChar()   READ_FUNC_IMPL(char)
    uchar  TextStream::ReadUChar()  READ_FUNC_IMPL(char)
    String TextStream::ReadWord()   READ_FUNC_IMPL(String)
    String TextStream::ReadLine()   { return readLine(); }

    TextStream& TextStream::WriteBool(const bool val)      { *this << val; return *this; }
    TextStream& TextStream::WriteByte(const sbyte val)     { *this << val; return *this; }
    TextStream& TextStream::WriteUByte(const ubyte val)    { *this << val; return *this; }
    TextStream& TextStream::WriteInt8(const int8 val)      { *this << val; return *this; }
    TextStream& TextStream::WriteInt16(const int16 val)    { *this << val; return *this; }
    TextStream& TextStream::WriteInt32(const int32 val)    { *this << val; return *this; }
    TextStream& TextStream::WriteInt64(const int64 val)    { *this << val; return *this; }
    TextStream& TextStream::WriteUInt8(const uint8 val)    { *this << val; return *this; }
    TextStream& TextStream::WriteUInt16(const uint16 val)  { *this << val; return *this; }
    TextStream& TextStream::WriteUInt32(const uint32 val)  { *this << val; return *this; }
    TextStream& TextStream::WriteUInt64(const uint64 val)  { *this << val; return *this; }
    TextStream& TextStream::WriteFloat(const float val)    { *this << val; return *this; }
    TextStream& TextStream::WriteDouble(const double val)  { *this << val; return *this; }
    TextStream& TextStream::WriteChar(const char val)      { *this << val; return *this; }
    TextStream& TextStream::WriteUChar(const uchar val)    { *this << val; return *this; }
    TextStream& TextStream::WriteString(const String& s)   { *this << s; return *this; }
}
