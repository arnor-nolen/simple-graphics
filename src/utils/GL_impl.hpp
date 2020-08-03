#pragma once

#include "utils/GL.hpp"

namespace gl {

template <typename T>
Buffer<T>::Buffer(const GLenum &buffer_type, const std::vector<T> &data)
    : buffer_type_(buffer_type), data_(data) {
  glGenBuffers(1, &buf_);
}
template <typename T> Buffer<T>::~Buffer() { glDeleteBuffers(1, &buf_); }

template <typename T> Buffer<T>::Buffer(Buffer &&other) noexcept {
  swap(other);
}
template <typename T>
auto Buffer<T>::operator=(Buffer &&other) noexcept -> Buffer & {
  swap(other);
  return *this;
};

template <typename T> void Buffer<T>::swap(Buffer &other) {
  std::swap(this->buf_, other.buf_);
  std::swap(this->data_, other.data_);
  std::swap(this->buffer_type_, other.buffer_type_);
  this->bind();
  this->set_layout();
  other.bind();
  other.set_layout();
}

template <typename T>
auto Buffer<T>::get_data() const -> const std::vector<T> & {
  return data_;
}

template <typename T> void Buffer<T>::bind() {
  glBindBuffer(buffer_type_, buf_);
}

template <typename T> void Buffer<T>::set_layout() {
  if (data_.size() > 0) {
    glBufferData(buffer_type_, data_.size() * sizeof(T), data_.data(),
                 GL_STATIC_DRAW);
  }
}

} // namespace gl