/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

struct RenderResourceHandle {
  uint index;
};

struct BindingsOffset {
  RenderResourceHandle bindings_offset;
  uint unused0;
  uint unused1;
  uint unused2;
};

[[vk::push_constant]] ConstantBuffer<BindingsOffset> g_bindings_offset;
[[vk::binding(0, 0)]] ByteAddressBuffer g_byte_address_buffers[];
[[vk::binding(0, 0)]] RWByteAddressBuffer g_rw_byte_address_buffers[];

#define BYTE_BUFFER(handle) g_byte_address_buffers[handle.index]
#define BYTE_BUFFER_UNIFORM(handle)                                            \
  g_byte_address_buffers[NonUniformResourceIndex(handle.index)]
#define RW_BYTE_BUFFER(handle) g_rw_byte_address_buffers[handle.index]
#define RW_BYTE_BUFFER_UNIFORM(handle)                                         \
  g_rw_byte_address_buffers[NonUniformResourceIndex(handle.index)]

template <typename T> T load_bindings() {
  T result = BYTE_BUFFER(g_bindings_offset.bindings_offset).Load<T>(0);
  return result;
}

struct SimpleBuffer {
  RenderResourceHandle handle;

  template <typename T> T load() {
    ByteAddressBuffer buffer = BYTE_BUFFER(this.handle);
    T result = buffer.Load<T>(0);
    return result;
  }

  template <typename T> T load_uniform() {
    ByteAddressBuffer buffer = BYTE_BUFFER_UNIFORM(this.handle);
    T result = buffer.Load<T>(0);
    return result;
  }
};

struct RwSimpleBuffer {
  RenderResourceHandle handle;

  template <typename T> T load() {
    RWByteAddressBuffer buffer = RW_BYTE_BUFFER(this.handle);
    T result = buffer.Load<T>(0);
    return result;
  }

  template <typename T> T load_uniform() {
    RWByteAddressBuffer buffer = RW_BYTE_BUFFER_UNIFORM(this.handle);
    T result = buffer.Load<T>(0);
    return result;
  }

  template <typename T> void store(T value) {
    RWByteAddressBuffer buffer = RW_BYTE_BUFFER(this.handle);
    buffer.Store<T>(0, value);
  }
};

struct ArrayBuffer {
  RenderResourceHandle handle;

  template <typename T> T load(uint index) {
    ByteAddressBuffer buffer = BYTE_BUFFER(this.handle);
    T result = buffer.Load<T>(sizeof(T) * index);
    return result;
  }

  template <typename T> T load_uniform(uint index) {
    ByteAddressBuffer buffer = BYTE_BUFFER_UNIFORM(this.handle);
    T result = buffer.Load<T>(sizeof(T) * index);
    return result;
  }
};

struct RwArrayBuffer {
  RenderResourceHandle handle;

  template <typename T> T load(uint index) {
    RWByteAddressBuffer buffer = RW_BYTE_BUFFER(this.handle);
    T result = buffer.Load<T>(sizeof(T) * index);
    return result;
  }

  template <typename T> T load_uniform(uint index) {
    RWByteAddressBuffer buffer = RW_BYTE_BUFFER_UNIFORM(this.handle);
    T result = buffer.Load<T>(sizeof(T) * index);
    return result;
  }

  template <typename T> void store(uint index, T value) {
    RWByteAddressBuffer buffer = RW_BYTE_BUFFER(this.handle);
    buffer.Store<T>(sizeof(T) * index, value);
  }
};
