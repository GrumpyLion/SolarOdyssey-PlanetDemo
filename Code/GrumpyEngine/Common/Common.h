#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>
#include <array>

//////////////////////////////////////////////////////////////////////////
// DataTypes

//using u64 = unsigned long;
//using s64 = signed long;
//using u32 = unsigned int;
//using s32 = signed int;
//using u16 = unsigned short;
//using s16 = signed short;
//using u8 = unsigned char;
//using s8 = signed char;
//using float= float;
//using f64 = double;

// alternate long data types
using uint64 = unsigned long;
using int64 = long;
using uint = unsigned int;
using uint16 = unsigned short;
using int16 = short;
using uint8 = unsigned char;
using int8 = char;
using float32 = float;
using float64 = double;

template<typename T>
using UPtr = std::unique_ptr<T>;

template<typename T>
using SPtr = std::shared_ptr<T>;

template<typename T, class... Args>
SPtr<T> MakeShared(Args... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T, class... Args>
UPtr<T> MakeUnique(Args... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

#define PRAGMA_DEOPTIMIZE __pragma(optimize( "", off ))
#define PRAGMA_REOPTIMIZE __pragma(optimize( "", on ))

//
//////////////////////////////////////////////////////////////////////////
// Logging

#if !GE_FINAL && GE_PLATFORM_PC
#include <windows.h>
#define GE_PRINT(...) { char cad[2048]; sprintf(cad, __VA_ARGS__); OutputDebugStringA(cad); OutputDebugStringA("\n"); }
#else
#define GE_PRINT(...)
#endif

//
//////////////////////////////////////////////////////////////////////////
// Assert

#if !GE_FINAL
#define GE_EXPAND_VARGS(x) x

#define GE_ASSERT_NO_MESSAGE(condition) { if(!(condition)) { GE_PRINT("Assertion Failed"); __debugbreak(); } }
#define GE_ASSERT_MESSAGE(condition, ...) { if(!(condition)) { GE_PRINT("Assertion Failed: %s", __VA_ARGS__); __debugbreak(); } }

#define GE_ASSERT_RESOLVE(arg1, arg2, macro, ...) macro
#define GE_GET_ASSERT_MACRO(...) GE_EXPAND_VARGS(GE_ASSERT_RESOLVE(__VA_ARGS__, GE_ASSERT_MESSAGE, GE_ASSERT_NO_MESSAGE))

#define GE_ASSERT(...) GE_EXPAND_VARGS( GE_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#define GE_CORE_ASSERT(...) GE_EXPAND_VARGS( GE_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#else
#define GE_ASSERT(...)
#define GE_CORE_ASSERT(...)
#endif

//
//////////////////////////////////////////////////////////////////////////
// Events

template<class RetType, class... Args>
class TEvent
{
public:
    inline void Subscribe(void* key, std::function<RetType(Args...)> function)
    {
        myListeners.insert({ key, function });
    }

    inline void Unsubscribe(void* key)
    {
        myListeners.erase(key);
    }

    inline void operator() (Args... args)
    {
        Fire(std::forward<Args>(args)...);
    }

    inline void Fire(Args... args)
    {
        for (const auto listener : myListeners)
        {
            listener.second(std::forward<Args>(args)...);
        }
    }

private:
    std::unordered_map<void*, std::function<RetType(Args...)>> myListeners;
};

template <typename Signature>
class Event;

template <class RetType, class... Args>
class Event<RetType(Args...) > : public TEvent <RetType, Args...>
{ };

//
//////////////////////////////////////////////////////////////////////////
// Static Array

template<typename T, uint Size>
class StaticArray
{
public:
    void Add(const T& object)
    {
        GE_ASSERT(myCurrentCount < myMaxCount);

        myArray[myCurrentCount++] = object;
    }

    void Remove(const uint index)
    {
        GE_ASSERT(index < Size);

        std::swap(myArray[index], myArray[myCurrentCount - 1]);
        myCurrentCount--;
    }

    T& operator[] (uint index)
    {
        return myArray[index];
    }

    const T& operator[] (uint index) const
    {
        return myArray[index];
    }

    T* GetArray()
    {
        return myArray;
    }

    const uint myMaxCount = Size;
    uint myCurrentCount = 0;

private:
    T myArray[Size];
};