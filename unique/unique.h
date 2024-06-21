#pragma once

#include "compressed_pair.h"

#include <cstddef>  // std::nullptr_t

template <class T>
struct Slug {
    Slug() = default;

    template <typename U>
    Slug(Slug<U>&&) noexcept {
    }

    ~Slug() = default;

    void operator()(T* p) const {
        static_assert(!std::is_void<T>::value, "void* type");
        delete p;
    }
};

template <class T>
struct Slug<T[]> {
    Slug() = default;

    template <typename U>
    Slug(Slug<U>&&) noexcept {
    }

    ~Slug() = default;

    void operator()(T* p) const {
        static_assert(!std::is_void<T>::value, "void* type");
        delete[] p;
    }
};

// Primary template
template <typename T, typename Deleter = Slug<T>>
class UniquePtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) {
        pair_.GetFirst() = ptr;
    }
    UniquePtr(T* ptr, Deleter deleter) {
        pair_.GetFirst() = ptr;
        pair_.GetSecond() = std::forward<Deleter>(deleter);
    }

    template <class U, class V>
    UniquePtr(UniquePtr<U, V>&& other) noexcept {
        pair_.GetFirst() = other.Release();
        pair_.GetSecond() = std::forward<V>(other.GetDeleter());
    }

    UniquePtr(const UniquePtr&) = delete;
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    template <class U, class V>
    UniquePtr& operator=(UniquePtr<U, V>&& other) noexcept {
        Reset(other.Release());
        pair_.GetSecond() = std::forward<V>(other.GetDeleter());
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) noexcept {
        Reset(std::nullptr_t());
        return *this;
    }

    UniquePtr& operator=(UniquePtr& other) = delete;
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        if (pair_.GetFirst()) {
            pair_.GetSecond()(pair_.GetFirst());
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        T* el = pair_.GetFirst();
        pair_.GetFirst() = nullptr;
        return el;
    }
    void Reset(T* ptr = nullptr) noexcept {
        T* tmp = pair_.GetFirst();
        pair_.GetFirst() = ptr;
        if (tmp) {
            GetDeleter()(tmp);
        }
    }
    void Swap(UniquePtr& other) noexcept {
        std::swap(pair_, other.pair_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return pair_.GetFirst();
    }
    Deleter& GetDeleter() {
        return pair_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return pair_.GetSecond();
    }
    explicit operator bool() const {
        return Get() != nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    std::add_lvalue_reference_t<T> operator*() const {
        return *pair_.GetFirst();
    }
    T* operator->() const {
        return pair_.GetFirst();
    }

private:
    CompressedPair<T*, Deleter> pair_;
};

// Specialization for arrays
template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) {
        pair_.GetFirst() = ptr;
    }
    UniquePtr(T* ptr, Deleter deleter) {
        pair_.GetFirst() = ptr;
        pair_.GetSecond() = std::forward<Deleter>(deleter);
    }

    template <class U, class V>
    UniquePtr(UniquePtr<U, V>&& other) noexcept {
        pair_.GetFirst() = other.Release();
        pair_.GetSecond() = std::forward<V>(other.GetDeleter());
    }

    UniquePtr(const UniquePtr&) = delete;
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    template <class U, class V>
    UniquePtr& operator=(UniquePtr<U, V>&& other) noexcept {
        Reset(other.Release());
        pair_.GetSecond() = std::forward<V>(other.pair_.GetSecond());
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) noexcept {
        Reset(std::nullptr_t());
        return *this;
    }

    UniquePtr& operator=(UniquePtr& other) = delete;
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        if (pair_.GetFirst()) {
            pair_.GetSecond()(pair_.GetFirst());
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        T* el = pair_.GetFirst();
        pair_.GetFirst() = nullptr;
        return el;
    }
    void Reset(T* ptr = nullptr) noexcept {
        T* tmp = pair_.GetFirst();
        pair_.GetFirst() = ptr;
        if (tmp) {
            GetDeleter()(tmp);
        }
    }
    void Swap(UniquePtr& other) noexcept {
        std::swap(pair_, other.pair_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return pair_.GetFirst();
    }
    Deleter& GetDeleter() {
        return pair_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return pair_.GetSecond();
    }
    explicit operator bool() const {
        return Get() != nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    std::add_lvalue_reference_t<T> operator*() const {
        return *pair_.GetFirst();
    }
    T* operator->() const {
        return pair_.GetFirst();
    }
    T& operator[](size_t ind) {
        return *(pair_.GetFirst() + ind);
    }

private:
    CompressedPair<T*, Deleter> pair_;
};
