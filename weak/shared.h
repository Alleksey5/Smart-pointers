#pragma once

#include "sw_fwd.h"

#include <cstddef>
#include "bits/stdc++.h"

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
        DelChecking();
        data_ = other.GetBlock();
        ptr_ = other.Get();
        CheckAdd();
        return *this;
    }
    template <typename P>
    SharedPtr& operator=(SharedPtr<P>&& other) noexcept {
        DelChecking();
        data_ = other.GetBlock();
        ptr_ = other.Get();
        CheckAdd();
        other.Reset();
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~SharedPtr() {
        DelChecking();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        DelChecking();
        data_ = nullptr;
        ptr_ = nullptr;
    }
    template <typename P>
    void Reset(P* ptr) {
        DelChecking();
        data_ = new ControlBlockUs<P>(ptr);
        ptr_ = ptr;
    }
    void Swap(SharedPtr& other) {
        std::swap(data_, other.data_);
        std::swap(ptr_, other.ptr_);
    }
    ///////////////////////////////////////////////////////////
    // Observers

    void DelChecking() {
        if (data_ != nullptr) {
            data_->RemRef();
            if (data_->GetRefCount() == 0) {
                data_->DefDeleter();
            }
            if (data_->GetRefCount() == 0 && data_->GetRefWCount() == 0) {
                delete data_;
            }
        }
    }

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
class EnableSharedFromThis {
public:
    SharedPtr<T> SharedFromThis();
    SharedPtr<const T> SharedFromThis() const;

    WeakPtr<T> WeakFromThis() noexcept;
    WeakPtr<const T> WeakFromThis() const noexcept;
};
