#include "BufferReader.h"

#include "Header.h"
#include "google/protobuf/message.h"

using namespace std;
using namespace yinpsoft;

BufferReader::BufferReader(const uint8_t *data, size_t len)
{
    raw_buffer.Initialize(data, len);
}

BufferReader::BufferReader(const std::string &data)
{
    raw_buffer.Initialize(data);
}

size_t BufferReader::ReadString(uint8_t *value, size_t len)
{
    return raw_buffer.ReadBytes(value, len);
}

bool BufferReader::ReadProto(google::protobuf::Message &msg)
{
    return msg.ParseFromArray((const void *)(raw_buffer.Buffer() + raw_buffer.Position()),
                              raw_buffer.Length() - raw_buffer.Position());
}