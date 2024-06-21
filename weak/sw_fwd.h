#pragma once

#include <exception>

class BadWeakPtr : public std::exception {};

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;

struct ControlBlockBase {
    size_t GetRefCount() const {
        return ref_counter_;
    }

    size_t GetRefWCount() {
        return weak_ref_counter_;
    }

    void AddRef() {
        ++ref_counter_;
    }

    void RemRef() {
        --ref_counter_;
    }

    void AddRefW() {
        ++weak_ref_counter_;
    }

    void RemRefW() {
        --weak_ref_counter_;
    }

    virtual void DefDeleter() {
    }
    virtual ~ControlBlockBase() {
    }

protected:
    size_t ref_counter_ = 1;
    size_t weak_ref_counter_ = 0;
};

template <typename T>
struct ControlBlockUs : ControlBlockBase {
    ControlBlockUs(T* ptr) {
        ptr_ = ptr;
        ref_counter_ = 1;
    }

    ~ControlBlockUs() override {
    }

    void DefDeleter() override {
        delete ptr_;
    }

private:
    T* ptr_;
};

template <typename T>
struct ControlBlockMS : ControlBlockBase {
    template <typename... Arg>
    ControlBlockMS(Arg&&... args) {
        new (&storage_) T(std::forward<Arg>(args)...);
    }
    T* GetPtr() {
        return reinterpret_cast<T*>(&storage_);
    }
    ~ControlBlockMS() override {
    }
    void DefDeleter() override {
        GetPtr()->~T();
    }

private:
    std::aligned_storage_t<sizeof(T), alignof(T)> storage_;
};
