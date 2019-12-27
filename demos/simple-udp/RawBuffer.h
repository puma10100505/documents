#pragma once

#include "Header.h"

#define MAX_RAW_BUFFER_SIZE 100 * 1024

namespace yinpsoft
{
class RawBuffer
{
public:
    RawBuffer()
    {
        capacity = MAX_RAW_BUFFER_SIZE;
    }

    ~RawBuffer() {}

    void Initialize(const std::string &str);
    void Initialize(const uint8_t *data, size_t len);
    void Initialize();

    inline size_t Capacity() const { return capacity; }
    inline size_t Size() const { return capacity; }
    inline size_t Position() const { return index; }
    inline void Rewind() { index = 0; }
    inline bool IsWriteFull() const { return index >= capacity; }
    inline bool IsReadDone() const { return index >= length; }
    inline bool IsEmpty() const { return length == 0; }
    inline size_t Length() const { return length; }

    inline const uint8_t *Buffer() const { return buffer; }
    inline uint8_t *MutableBuffer() { return buffer; }

public:
    void WriteByte(uint8_t value);
    void WriteBytes(const uint8_t *value, size_t len);

    uint8_t ReadByte();
    size_t ReadBytes(uint8_t *value, size_t len);

private:
    uint8_t buffer[MAX_RAW_BUFFER_SIZE];
    size_t capacity;
    size_t index = 0;
    size_t length = 0;
};
}; // namespace yinpsoft