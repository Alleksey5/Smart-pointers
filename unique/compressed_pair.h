#pragma once

#include <type_traits>
#include <utility>

template <typename F, typename S, bool = std::is_empty_v<F> && !std::is_final_v<F>,
          bool = std::is_empty_v<S> && !std::is_final_v<S>>
class CompressedPair;

template <typename F>
class CompressedPair<F, F, true, true> : public F {
public:
    CompressedPair() = default;
    template <typename T, typename M>
    CompressedPair(T&& first, M&& second) : T(std::forward<T>(first)), M(std::forward<M>(second)) {
    }

    F& GetFirst() {
        return static_cast<F&>(*this);
    }

    const F& GetFirst() const {
        return static_cast<const F&>(*this);
    }

    F& GetSecond() {
        return static_cast<F&>(*this);
    }

    const F& GetSecond() const {
        return static_cast<const F&>(*this);
    }

private:
    F first_;
};

template <typename F, typename S>
class CompressedPair<F, S, true, true> : F, S {
public:
    template <typename T, typename M>
    CompressedPair(T&& first, M&& second) : F(std::forward<T>(first)), S(std::forward<M>(second)) {
    }

    F& GetFirst() {
        return *this;
    }

    const F& GetFirst() const {
        return *this;
    }

    S& GetSecond() {
        return *this;
    }

    const S& GetSecond() const {
        return *this;
    }
};

template <typename F, typename S>
class CompressedPair<F, S, true, false> : F {
public:
    CompressedPair() : second_() {
    }
    template <typename T, typename M>
    CompressedPair(T&& first, M&& second)
        : F(std::forward<T>(first)), second_(std::forward<M>(second)) {
    }

    F& GetFirst() {
        return static_cast<F&>(*this);
    }

    const F& GetFirst() const {
        return static_cast<const F&>(*this);
    }

    S& GetSecond() {
        return second_;
    }

    const S& GetSecond() const {
        return second_;
    }

private:
    S second_;
};

template <typename F, typename S>
class CompressedPair<F, S, false, true> : S {
public:
    CompressedPair() : first_() {
    }
    template <typename T, typename M>
    CompressedPair(T&& first, M&& second)
        : S(std::forward<M>(second)), first_(std::forward<T>(first)) {
    }

    F& GetFirst() {
        return first_;
    }

    const F& GetFirst() const {
        return first_;
    }

    S& GetSecond() {
        return static_cast<S&>(*this);
    }

    const S& GetSecond() const {
        return static_cast<const S&>(*this);
    }

private:
    F first_;
};

template <typename F, typename S>
class CompressedPair<F, S, false, false> {
public:
    CompressedPair() : first_(), second_() {
    }

    template <typename T, typename M>
    CompressedPair(T&& first, M&& second)
        : first_(std::forward<T>(first)), second_(std::forward<M>(second)) {
    }

    F& GetFirst() {
        return first_;
    }

    const F& GetFirst() const {
        return first_;
    }

    S& GetSecond() {
        return second_;
    }

    const S& GetSecond() const {
        return second_;
    }

private:
    F first_;
    S second_;
};