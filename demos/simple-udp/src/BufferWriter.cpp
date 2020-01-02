#include "BufferWriter.h"

#include "Header.h"
#include "google/protobuf/message.h"

using namespace std;
using namespace yinpsoft;

void BufferWriter::WriteString(const uint8_t *str, size_t len)
{
    raw_buffer.WriteBytes(str, len);
}

void BufferWriter::WriteBool(bool value)
{
    if (value)
    {
        raw_buffer.WriteByte(1);
    }
    else
    {
        raw_buffer.WriteByte(0);
    }
}

bool BufferWriter::WriteProto(const google::protobuf::Message &msg)
{
    return msg.SerializeToArray((void *)raw_buffer.MutableBuffer(), msg.ByteSizeLong());
}