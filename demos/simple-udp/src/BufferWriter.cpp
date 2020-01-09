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
    char buff[500];    
    msg.SerializeToArray(buff, msg.ByteSizeLong());
    printf("WriteProto msg: len: %lu, pos: %lu, buff.len: %lu\n", msg.ByteSizeLong(), raw_buffer.Position(), sizeof(buff));
    raw_buffer.WriteBytes((uint8_t*)buff, msg.ByteSizeLong());
    return true;
}
