#include <cstdio>
#include <memory>
#include <malloc.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <unordered_map>

class GameObject
{
public:
    GameObject() {}
    GameObject(float InSpeed, float InHeight, float InBulletNum)
        : Speed(InSpeed), Height(InHeight), BulletNum(InBulletNum) {}
    ~GameObject(){}

private:
    float Speed;
    float Height;
    float BulletNum;
};

class OutputMemoryStream 
{
public:
    OutputMemoryStream()
        : Capacity(32), Head(0), Length(0), RawBuff(nullptr)
    {
        ReAlloc(Capacity);
    }

    ~OutputMemoryStream() 
    {
        std::free(RawBuff);
    }

    template<typename T>
    uint32_t Write(const T& InData);

    template<typename T>
    uint32_t Write(const std::vector<T>& InData);

    template<typename TKey, typename TValue>
    uint32_t Write(const std::unordered_map<TKey, TValue>& InData);

    const uint32_t GetCapacity() const { return Capacity; }
    const unsigned char* GetBufferPtr() const { return RawBuff; }
    unsigned char* GetBufferPtr() { return RawBuff; }
    const uint32_t GetLength() const { return Length; }
    inline void Reset() { Head = 0; Length = 0;}

protected:
    uint32_t ReAlloc(uint32_t Count);
    uint32_t Write(const void* InData, uint32_t Count);
    
private:
    uint32_t Capacity;
    uint32_t Head;
    uint32_t Length;
    unsigned char* RawBuff;
};

uint32_t OutputMemoryStream::ReAlloc(uint32_t Count) 
{
    RawBuff = static_cast<unsigned char*>(std::realloc(RawBuff, Count));
    if (!RawBuff)
    {
        return 0;
    }

    return sizeof(RawBuff);
}

uint32_t OutputMemoryStream::Write(const void* InData, uint32_t Count)
{
    uint32_t ResultHead = Head + static_cast<uint32_t>(Count);
    if (ResultHead > Capacity)
    {
        ReAlloc(std::max(ResultHead, Capacity * 2));
    }

    std::memcpy(RawBuff + Head, InData, Count);

    Head = ResultHead;

    return sizeof(RawBuff);
}

template<typename T>
uint32_t OutputMemoryStream::Write(const T& InData)
{
    static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, 
        "Generic write only accept primitive type");

    uint32_t Len = Write(&InData, sizeof(InData));

    Length += Len;

    return Length; 
}

template<typename T>
uint32_t OutputMemoryStream::Write(const std::vector<T>& InData) 
{
    uint32_t TotalLen = 0;
    size_t InDataLen = InData.size();
    TotalLen = Write<uint32_t>(InDataLen);

    for (const T& Item: InData)
    {
        TotalLen += Write<T>(Item);
    }

    return TotalLen;
}

template<typename TKey, typename TValue>
uint32_t OutputMemoryStream::Write(const std::unordered_map<TKey, TValue>& InData)
{
    uint32_t TotalLen = 0;
    size_t InDataLen = InData.size();
    TotalLen = Write<uint32_t>(InDataLen);    

    for (const auto& Item: InData)
    {
        TotalLen += Write<TKey>(Item.first);
        TotalLen += Write<TValue>(Item.second);
    }

    return TotalLen;
}

/* ------------------------------------------------------------------------------ */

class InputMemoryStream 
{
public:
    InputMemoryStream(unsigned char* InData, uint32_t MaxCapacity):
        RawBuff(InData), Head(0), Capacity(MaxCapacity)
    {
        //PrepareBuff(InData, sizeof(InData));
    }

    ~InputMemoryStream() { free(RawBuff); }

    uint32_t GetRemainingSize() { return Capacity - Head; }

    void Reset() { Head = 0; }

    uint32_t Read(void* OutData, uint32_t Length);

    template<typename T>
    uint32_t Read(T& OutData);

    template<typename T>
    uint32_t Read(std::vector<T>& OutData);

    template<typename TKey, typename TValue>
    uint32_t Read(std::unordered_map<TKey, TValue>& OutData);

protected:
    void PrepareBuff(const unsigned char* InData, uint32_t Length);

private:
    unsigned char* RawBuff;
    uint32_t Capacity;
    uint32_t Head;
};

void InputMemoryStream::PrepareBuff(const unsigned char* InData, uint32_t Length) 
{
    RawBuff = static_cast<unsigned char*>(std::realloc(RawBuff, Capacity));
    if (!RawBuff)
    {
        // Error
    }

    std::memcpy(RawBuff, InData, Length);
}

uint32_t InputMemoryStream::Read(void* OutData, uint32_t InBytesCount)
{
    if (GetRemainingSize() < InBytesCount)
    {
        // Error
        return 0;
    }

    std::memcpy(OutData, RawBuff + Head, InBytesCount);

    Head += InBytesCount;

    return InBytesCount;
}

template<typename T>
uint32_t InputMemoryStream::Read(T& OutData)
{
    return Read(&OutData, sizeof(T));
}

template<typename T>
uint32_t InputMemoryStream::Read(std::vector<T>& OutData)
{
    uint32_t DataSize = 0;
    Read<uint32_t>(DataSize);

    std::cout << "Data Size: " << DataSize << std::endl;

    OutData.resize(DataSize);

    for (T& Elem: OutData) 
    {
        Read<T>(Elem);
    }

    return 0;
}

template<typename TKey, typename TValue>
uint32_t InputMemoryStream::Read(std::unordered_map<TKey, TValue>& OutData)
{
    uint32_t DataSize = 0;
    Read<uint32_t>(DataSize);

    // OutData.reserve(DataSize);
    
    for (int32_t i = 0; i < DataSize; i++)
    {
        TKey Key; 
        TValue Value;
        Read<TKey>(Key);
        Read<TValue>(Value);
        OutData.emplace(Key, Value);
    }

    return 0;
}

/* ------------------------------------------------------------------------------ */

class OutputMemoryBitStream 
{
public:
    OutputMemoryBitStream(uint32_t InCapacity = 1024)
        : BitHead(0), RawBuff(nullptr), BitCapacity(InCapacity) 
    {
        ReallocBuffer(InCapacity * 8);
    }

    ~OutputMemoryBitStream() { free(RawBuff); }

    const unsigned char* GetBuffPtr() const { return RawBuff; }
    uint32_t GetBitCapacity() const { return BitCapacity; }
    uint32_t GetBitLength() const { return BitHead; }
    uint32_t ByteLength() const { return (BitHead + 7) >> 3; }

    void WriteBits(uint8_t InData, uint32_t InBitCount);
    void WriteBits(const void* InData, uint32_t InBitCount);
    inline void WriteBytes(const void* InData, uint32_t InByteCount) { WriteBits(InData, InByteCount << 3); }

protected:
    void ReallocBuffer(uint32_t MaxCapacity);

private:
    uint32_t BitHead;
    unsigned char* RawBuff;
    uint32_t BitCapacity;
};

void OutputMemoryBitStream::WriteBits(uint8_t InData, uint32_t InBitCount) 
{
    uint32_t NextBitHead = BitHead + static_cast<uint32_t>(InBitCount);

    if (NextBitHead > BitCapacity) 
    {
        ReallocBuffer(std::max(BitCapacity * 2, NextBitHead));
    }

    // find the byte position (just like find the row in matrix)
    uint32_t ByteOffset = BitHead >> 3;
    // find the bit position (just like find the col in matrix)
    uint32_t BitOffset = BitHead & 0x07;

    uint8_t CurrentMask = ~(0xff << BitOffset);
    RawBuff[ByteOffset] = (RawBuff[ByteOffset] & CurrentMask) | (InData << BitOffset);

    uint32_t BitsRemaining = 8 - BitOffset;     // Calc the remaining bits when writing into buff

    if (BitsRemaining < InBitCount) 
    {
        // Use another byte to store the left bits
        RawBuff[ByteOffset + 1] = InData >> BitsRemaining;
    } 

    BitHead = NextBitHead;
} 

void OutputMemoryBitStream::WriteBits(const void* InData, uint32_t InBitCount)
{
    const char* SrcByte = static_cast<const char*>(InData);
    while (InBitCount > 8)
    {
        WriteBits(*SrcByte, 8);
        ++SrcByte;
        InBitCount -= 8;
    }

    if (InBitCount > 0)
    {
        WriteBits(*SrcByte, InBitCount);
    }
}

void OutputMemoryBitStream::ReallocBuffer(uint32_t MaxCapacity)
{
    if (RawBuff == nullptr) 
    {
        RawBuff = static_cast<unsigned char*>(std::malloc(MaxCapacity >> 3));
        memset(RawBuff, 0, MaxCapacity >> 3);
    }
    else 
    {
        unsigned char* TempBuff = static_cast<unsigned char*>(std::malloc(MaxCapacity >> 3));
        memset(TempBuff, 0, MaxCapacity >> 3);
        memcpy(RawBuff, TempBuff, BitCapacity >> 3);
        std::free(RawBuff);
        RawBuff = TempBuff;
    }

    BitCapacity = MaxCapacity;
}

class InputMemoryBitStream
{
public:
    InputMemoryBitStream(unsigned char* InData, uint32_t Capacity): BitHead(0), BitCapacity(Capacity), RawBuff(InData)
    {

    }

    ~InputMemoryBitStream() 
    {
        free(RawBuff);
    }

    void ReadBits(void* OutData, uint32_t RequiredBitCount);
    void ReadBits(uint8_t OutData, uint32_t RequiredBitCount);
    void ReadBytes(void* OutData, uint32_t RequiredByteCount) { ReadBits(OutData, RequiredByteCount << 3); }

private:
    uint32_t BitHead;
    uint32_t BitCapacity;
    unsigned char* RawBuff;
};

void InputMemoryBitStream::ReadBits(void* OutData, uint32_t RequiredBitCount) 
{

}

void InputMemoryBitStream::ReadBits(uint8_t OutData, uint32_t RequiredBitCount)
{
    uint32_t ByteOffset = BitHead >> 3;
    uint32_t BitOffset = BitHead & 0x07;

    // find the position of starting to read
    OutData = static_cast<uint8_t>(RawBuff[ByteOffset]) >> BitOffset;

    uint32_t LeftFreeBitsThisByte = 8 - BitOffset;
    if (LeftFreeBitsThisByte < RequiredBitCount)
    {
        OutData |= static_cast<uint8_t>(RawBuff[ByteOffset + 1]) << LeftFreeBitsThisByte;
    }

    OutData &= (~(0xff << RequiredBitCount));

    BitHead += RequiredBitCount;
}

/* ---------------------------------------------------------------------------------------- */
int main() {
    OutputMemoryStream stream;
    std::cout << stream.GetCapacity() << " " << stream.GetLength() << std::endl;
    stream.Write<int32_t>(111345330);
    stream.Write<float>(110.0f);
    stream.Write<bool>(false);
    std::cout << stream.GetCapacity() << " " << stream.GetLength() << std::endl;

    InputMemoryStream instream(stream.GetBufferPtr(), stream.GetCapacity());

    int32_t retInt = 0;
    float retFloat = 0.0f;
    bool retBool = true;

    instream.Read<int32_t>(retInt);
    instream.Read<float>(retFloat);
    instream.Read<bool>(retBool);

    std::cout << "output: " << retInt << " " << retFloat << " " << retBool << std::endl;

    stream.Reset();

    // OutputMemoryStream stream2;

    std::vector<int32_t> Data;
    Data.push_back(34);
    Data.push_back(11);
    Data.push_back(56);
    stream.Write<int32_t>(Data);
    std::cout << "after write vector data" << std::endl;

    Data.clear();
    InputMemoryStream instream2(stream.GetBufferPtr(), stream.GetCapacity());
    instream2.Read<int32_t>(Data);

    std::cout << "Vector Size:" << Data.size() << std::endl;

    for (int32_t Item: Data)
    {
        std::cout << Item << std::endl;
    }

    return 0;
}