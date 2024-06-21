#pragma once

#include "sw_fwd.h"  // Forward declaration

// https://en.cppreference.com/w/cpp/memory/weak_ptr
template <typename T>
class WeakPtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    WeakPtr() : data_(nullptr), ptr_(nullptr) {
    }

    template<typename U>
    WeakPtr(const WeakPtr<U>& other) noexcept : data_(other.GetBlock()), ptr_(other.Get()) {
        CheckAdd();
    }
    WeakPtr(const WeakPtr& other) noexcept : data_(other.GetBlock()), ptr_(other.Get()) {
        CheckAdd();
    }
    template <typename P>
    WeakPtr(WeakPtr<P>&& other) noexcept : data_(other.GetBlock()), ptr_(other.Get()) {
        CheckAdd();
        other.Reset();
    }

    // Demote `SharedPtr`
    // #2 from https://en.cppreference.com/w/cpp/memory/weak_ptr/weak_ptr
    template <typename U>
    WeakPtr(const SharedPtr<U>& other) noexcept : data_(other.GetBlock()), ptr_(other.Get()) {
        CheckAdd();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    WeakPtr& operator=(const WeakPtr& other) noexcept {
        DelChecking();
        data_ = other.GetBlock();
        ptr_ = other.Get();
        CheckAdd();
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) noexcept {
        DelChecking();
        data_ = other.GetBlock();
        ptr_ = other.Get();
        CheckAdd();
        other.Reset();
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~WeakPtr() {
        if (data_ != nullptr) {
            data_->RemRefW();
            if (data_->GetRefCount() == 0 && data_->GetRefWCount() == 0) {
                delete data_;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        DelChecking();
        data_ = nullptr;
        ptr_ = nullptr;
    }
    void Swap(WeakPtr& other) {
        std::swap(data_, other.data_);
        std::swap(ptr_, other.ptr_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    void DelChecking() {
        if (data_ != nullptr) {
            data_->RemRefW();
            if (data_->GetRefCount() == 0 && data_->GetRefWCount() == 0) {
                delete data_;
            }
        }
    }

    void CheckAdd() {
        if (data_ != nullptr) {
            data_->AddRefW();
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
        return (data_ != nullptr) ? data_->GetRefCount() : 0;
    }
    bool Expired() const {
        return (data_ == nullptr) ? true : (data_->GetRefCount() == 0);
    }
    SharedPtr<T> Lock() const {
        return Expired() ? SharedPtr<T>() : SharedPtr<T>(*this);
    }

private:
    ControlBlockBase* data_ = nullptr;
    T* ptr_ = nullptr;
};
