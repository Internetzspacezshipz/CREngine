#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>

//Logging
#include "CrLogging.h"

#include "ThirdParty/ThirdPartyLibs.h"


template<typename ... args>
using Array = std::vector<args...>;

//char size binary array - aka string lol.
//This is clarified to be BinArray so it is clear that this is not a displayable string.
using BinArray = std::string;
using BinArrayV = std::string_view;

template<typename ... args>
using Map = std::unordered_map<args...>;

template<typename ... args>
using Set = std::unordered_set<args...>;

template<typename ... args>
using Func = fu2::function<args...>;

using Path = std::filesystem::path;

using String = std::string;
using StringV = std::string_view;


using WString = std::wstring;
using WStringV = std::wstring_view;

using Json = nlohmann::json;

//Serialiation function type trait decl
template<typename Type>
struct Has_Serializer_Function
{
	constexpr static bool Value = false;
};

//def for forceinline to make it easier to type and also rename later if needed.
#define forceinline __forceinline

//Improved weak pointer that is hashable
template<class T>
struct HashableWeakPointer : protected std::weak_ptr<T>
{
public:
    // Hash class
    class Hash
    {
    public:
        size_t operator()(HashableWeakPointer const& hashableWeakPointer) const
        {
            return hashableWeakPointer.getHash();
        }
    };


    // constructor
    HashableWeakPointer(std::shared_ptr<T> const& sp)
        : std::weak_ptr<T>(sp)
        , hash(0)
    {
        if (static_cast<bool>(sp))
        {
            hash = std::hash<T*>{}(sp.get());
        }
    }


    // weak_ptr-interface
    void reset() noexcept
    {
        std::weak_ptr<T>::reset();
        hash = 0;
    }

    void swap(HashableWeakPointer& r) noexcept
    {
        std::weak_ptr<T>::swap(r);
        std::swap(hash, r.hash);
    }

    using std::weak_ptr<T>::use_count;
    using std::weak_ptr<T>::expired;
    using std::weak_ptr<T>::lock;

    template< class Y >
    bool owner_before(const HashableWeakPointer<Y>& other) const noexcept
    {
        return std::weak_ptr<T>::owner_before(static_cast<std::weak_ptr<Y>>(other));
    }

    template< class Y >
    bool owner_before(const std::shared_ptr<Y>& other) const noexcept
    {
        return std::weak_ptr<T>::owner_before(other);
    }


    // hash-interface
    std::size_t getHash() const noexcept
    {
        return hash;
    }

    // helper methods

    // https://en.cppreference.com/w/cpp/memory/shared_ptr
    // "The destructor of shared_ptr decrements the number of shared owners of the control block. If that counter
    // reaches zero, the control block calls the destructor of the managed object. The control block does not
    // deallocate itself until the std::weak_ptr counter reaches zero as well."
    // So below comparisons should stay save even if all shared_ptrs to the managed instance were destroyed.

    friend bool operator<(HashableWeakPointer const& lhs, HashableWeakPointer const& rhs)
    {
        return lhs.owner_before(rhs);
    }

    friend bool operator!=(HashableWeakPointer const& lhs, HashableWeakPointer const& rhs)
    {
        return lhs < rhs || rhs < lhs;
    }

    friend bool operator==(HashableWeakPointer const& lhs, HashableWeakPointer const& rhs)
    {
        return !(lhs != rhs);
    }

    friend std::ostream& operator<<(std::ostream& os, const HashableWeakPointer& dt)
    {
        os << "<" << dt.lock().get() << "," << dt.hash << ">";
        return os;
    }

private:

    std::size_t hash;
};

namespace std
{
    template <typename PtrType>
    struct hash<HashableWeakPointer<PtrType>>
    {
        std::size_t operator()(const HashableWeakPointer<PtrType>& Item) const
        {
            return Item.getHash();
        }
    };
}


//template argument usable string
template<size_t N>
struct StringLiteral
{
    consteval StringLiteral(const char(&Str)[N])
    {
        std::copy_n(Str, N, Value);
    }

    std::size_t Size = N - 1;
    char Value[N];
};




//hide shared_ptr and such.
#define SP std::shared_ptr
#define SharedFromThis std::enable_shared_from_this
#define WP std::weak_ptr
#define HWP HashableWeakPointer
#define UP std::unique_ptr

#define Pair std::pair

//make shared helper
#define MkSP std::make_shared
#define MkUP std::make_unique

typedef glm::mat4 Mat4;
typedef glm::vec4 Vec4;

typedef glm::mat3 Mat3;
typedef glm::vec3 Vec3;

typedef glm::mat2 Mat2;
typedef glm::vec2 Vec2;


