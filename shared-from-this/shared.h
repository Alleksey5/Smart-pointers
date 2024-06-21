#pragma once

#include "sw_fwd.h"

#include <cstddef>
#include "bits/stdc++.h"

class ESFTBase {};
template <typename T>
class SharedPtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    SharedPtr() : data_(nullptr), ptr_(nullptr) {
    }

    SharedPtr(std::nullptr_t) : data_(nullptr), ptr_(nullptr) {
    }

    template <typename P>
    explicit SharedPtr(P* ptr) noexcept : data_(new ControlBlockUs(ptr)), ptr_(ptr) {
        if constexpr (std::is_convertible_v<P*, ESFTBase*>) {
            ptr_->weak_this = *this;
        }
    }

    SharedPtr(const SharedPtr& other) noexcept : data_(other.GetBlock()), ptr_(other.Get()) {
        CheckAdd();
    }
    template <typename P>
    SharedPtr(SharedPtr<P>& other) noexcept : data_(other.GetBlock()), ptr_(other.Get()) {
        CheckAdd();
    }

    template <typename P>
    SharedPtr(SharedPtr<P>&& other) noexcept : data_(other.GetBlock()), ptr_(other.Get()) {
        CheckAdd();
        other.Reset();
    }

    SharedPtr(ControlBlockMS<T>* el) noexcept : data_(el), ptr_(el->GetPtr()) {
        if constexpr (std::is_convertible_v<T*, ESFTBase*>) {
            ptr_->weak_this = *this;
        }
    }

    // Aliasing constructor
    // #8 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) noexcept : data_(other.GetBlock()), ptr_(ptr) {
        CheckAdd();
    }

    // Promote `WeakPtr`
    // #11 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    explicit SharedPtr(const WeakPtr<T>& other) {
        if (other.GetBlock()->GetRefCount() == 0) {
            throw BadWeakPtr();
        }
        data_ = other.GetBlock();
        ptr_ = other.Get();
        CheckAdd();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    SharedPtr& operator=(const SharedPtr& other) noexcept {
        if (data_) {
            data_->DefDeleter();
        }
        data_ = other.GetBlock();
        ptr_ = other.Get();
        CheckAdd();
        return *this;
    }
    template <typename P>
    SharedPtr& operator=(SharedPtr<P>&& other) noexcept {
        if (data_) {
            data_->DefDeleter();
        }
        data_ = other.GetBlock();
        ptr_ = other.Get();
        CheckAdd();
        other.Reset();
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~SharedPtr() {
        if constexpr (std::is_convertible_v<T*, ESFTBase*>) {
            if (data_->GetRefWCount() <= 1) {
                ptr_->weak_this.Reset();
            }
        }
        data_->DefDeleter();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        if (data_) {
            data_->DefDeleter();
        }
        data_ = nullptr;
        ptr_ = nullptr;
    }
    template <typename P>
    void Reset(P* ptr) {
        if (data_) {
            data_->DefDeleter();
        }
        data_ = new ControlBlockUs<P>(ptr);
        ptr_ = ptr;
    }
    void Swap(SharedPtr& other) {
        std::swap(data_, other.data_);
        std::swap(ptr_, other.ptr_);
    }
    ///////////////////////////////////////////////////////////
    // Observers

    void CheckAdd() {
        if (data_ != nullptr) {
            data_->AddRef();
        }
    }
    T* Get() const {
        return ptr_;
    }
    ControlBlockBase* GetBlock() const {
        return data_;
    }
    T& operator*() const {
        return *ptr_;
    }
    T* operator->() const {
        return ptr_;
    }
    size_t UseCount() const {
        if (data_ != nullptr) {
            return data_->GetRefCount();
        }
        return 0;
    }
    explicit operator bool() const {
        return data_ != nullptr;
    }

private:
    ControlBlockBase* data_ = nullptr;
    T* ptr_ = nullptr;
};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right) {
    return left.Get() == right.Get();
}

// Allocate memory only once
template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    ControlBlockMS<T>* block = new ControlBlockMS<T>(std::forward<Args>(args)...);
    return SharedPtr<T>(block);
}

// Look for usage examples in tests
template <typename T>
class EnableSharedFromThis : public ESFTBase {
public:
    SharedPtr<T> SharedFromThis() {
        return weak_this.Lock();
    }
    SharedPtr<const T> SharedFromThis() const {
        return weak_this.Lock();
    }

    WeakPtr<T> WeakFromThis() noexcept {
        return weak_this;
    }
    WeakPtr<const T> WeakFromThis() const noexcept {
        return weak_this;
    }

    WeakPtr<T> weak_this;
};
