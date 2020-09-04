
#include <cstdio>
#include <iostream>

template<typename TFrom, typename TTo>
class TypeAliaser 
{
public:
    TypeAliaser(TFrom InFromData): FromData(InFromData){}
    TTo& Get() { return ToData; }

    union 
    {
        /* data */
        TFrom FromData;
        TTo ToData;
    };
    
};

template<typename T, size_t TSize> class ByteSwapper;

template<typename T>
class ByteSwapper<T, 2>
{
public:
    T Swap(T InData) const 
    {
        uint16_t Result = __builtin_bswap16(TypeAliaser<T, uint16_t>(InData).Get());
        return TypeAliaser<uint16_t, T>(Result).Get();
    }
};

template<typename T>
class ByteSwapper<T, 4>
{
public:
    T Swap(T InData) const 
    {
        uint32_t Result = __builtin_bswap32(TypeAliaser<T, uint32_t>(InData).Get());
        return TypeAliaser<uint32_t, T>(Result).Get();
    }
};

template<typename T>
class ByteSwapper<T, 8>
{
public:
    T Swap(T InData) const 
    {
        uint64_t Result = __builtin_bswap64(TypeAliaser<T, uint64_t>(InData).Get());
        return TypeAliaser<uint64_t, T>(Result).Get();
    }
};

template<typename T>
T ByteSwap(T InData)
{
    ByteSwapper<T, sizeof(T)>().Swap(InData);
}

int main() 
{
    uint32_t ret = ByteSwap<uint32_t>(342);
    std::cout << ret << std::endl;
    uint32_t origin = ByteSwap<uint32_t>(ret);
    std::cout << origin << std::endl;

    float ret1 = ByteSwap<float>(34.3423f);
    std::cout << ret1 << std::endl;
    float origin1 = ByteSwap<float>(ret1);
    std::cout << origin1 << std::endl;
    return 0;
}