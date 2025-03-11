#pragma once
#include <array>
#include <algorithm>
#include <string>

template <size_t N, typename T>
struct static_string {
  std::array<T, N> data;
  size_t size = 0;

  constexpr static_string() = default;
  constexpr explicit static_string(const T (&str)[N]) {
    std::copy_n(str, N, data.begin());
    size = N;
  }

  constexpr static_string(const T *str, size_t len) {
    std::copy_n(str, len, data.begin());
    size = len;
  }

  constexpr static_string(const T *str) {
    size = 0;
    // eek - that's a scary while loop
    while (*str) {
      data[size++] = *str++;
    }
  }

  constexpr static_string(const static_string &other) {
    std::copy_n(other.data.begin(), other.size, data.begin());
    size = other.size;
  }

  constexpr static_string &operator=(const static_string &other) {
    std::copy_n(other.data.begin(), other.size, data.begin());
    size = other.size;
    return *this;
  }

  constexpr static_string &operator+=(const T &c) {
    data[size++] = c;
    return *this;
  }

  constexpr static_string &operator+=(const T (&str)[N]) {
    std::copy_n(str, N, data.begin() + size);
    size += N;
    return *this;
  }

  constexpr static_string &operator+=(const static_string &other) {
    std::copy_n(other.data.begin(), other.size, data.begin() + size);
    size += other.size;
    return *this;
  }

  constexpr size_t length() const { return size; }
  constexpr size_t max_size() const { return N; }
  constexpr bool empty() const { return size == 0; }
  constexpr const T *c_str() const { return data.data(); }

  constexpr T &operator[](size_t i) { return data[i]; }

  constexpr const T &operator[](size_t i) const { return data[i]; }

  constexpr bool operator==(const static_string &other) const {
    return size == other.size &&
           std::equal(data.begin(), data.begin() + size, other.data.begin());
  }

  template <size_t M>
  constexpr bool operator==(const char (&str)[N]) {
    return size == M && std::equal(data.begin(), data.begin() + size, str);
  }

  // castable to std::string
  operator std::string() const { return std::string(data.begin(), data.begin() + size); }

  constexpr bool operator!=(const static_string &other) const {
    return *this != other;
  }

};

