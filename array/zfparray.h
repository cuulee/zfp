#ifndef ZFP_ARRAY_H
#define ZFP_ARRAY_H

#include <algorithm>
#include <climits>
#include "zfp.h"
#include "zfp/memory.h"

namespace zfp {

// abstract base class for compressed array of scalars
class array {
protected:
  // default constructor
  array() :
    dims(0), type(zfp_type_none),
    nx(0), ny(0), nz(0),
    bx(0), by(0), bz(0),
    blocks(0), blkvals(0), blkbits(0), blksize(0),
    bytes(0), data(0),
    stream(0),
    shape(0)
  {}

  // generic array with 'dims' dimensions and scalar type 'type'
  array(uint dims, zfp_type type) :
    dims(dims), type(type),
    nx(0), ny(0), nz(0),
    bx(0), by(0), bz(0),
    blocks(0), blkvals(1u << (2 * dims)), blkbits(0), blksize(0),
    bytes(0), data(0),
    stream(zfp_stream_open(0)),
    shape(0)
  {}

  // copy constructor--performs a deep copy
  array(const array& a) :
    data(0),
    stream(0),
    shape(0)
  {
    deep_copy(a);
  }

  // protected destructor (cannot delete array through base class pointer)
  ~array()
  {
    free();
    zfp_stream_close(stream);
  }

  // assignment operator--performs a deep copy
  array& operator=(const array& a)
  {
    deep_copy(a);
    return *this;
  }

public:
  // rate in bits per value
  double rate() const { return double(blkbits) / blkvals; }

  // set compression rate in bits per value
  double set_rate(double rate)
  {
    rate = zfp_stream_set_rate(stream, rate, type, dims, 1);
    blkbits = stream->maxbits;
    blksize = blkbits / CHAR_BIT;
    alloc();
    return rate;
  }

  // empty cache without compressing modified cached blocks
  virtual void clear_cache() const = 0;

  // flush cache by compressing all modified cached blocks
  virtual void flush_cache() const = 0;

  // number of bytes in header (includes padding)
  size_t header_size() const { return compressed_data_offset_bits() / CHAR_BIT; }

  // pointer to header
  uchar* header_data() const
  {
    flush_cache();
    return data;
  }

  // number of bytes of compressed data
  size_t compressed_size() const { return bytes; }

  // pointer to compressed data for read or write access
  uchar* compressed_data() const
  {
    // first write back any modified cached data
    flush_cache();
    return data + header_size();
  }

  // dimensionality
  uint dimensionality() const { return dims; }

  // underlying scalar type
  zfp_type scalar_type() const { return type; }

protected:
  // allocate memory for compressed data
  void alloc(bool clear = true)
  {
    bytes = blocks * blksize;
    size_t totalBytes = bytes + header_size();

    reallocate_aligned(data, totalBytes, 0x100u);
    if (clear)
      std::fill(data, data + totalBytes, 0);
    stream_close(stream->stream);
    zfp_stream_set_bit_stream(stream, stream_open(data, totalBytes));
    clear_cache();
  }

  // free memory associated with compressed data
  void free()
  {
    nx = ny = nz = 0;
    bx = by = bz = 0;
    blocks = 0;
    stream_close(stream->stream);
    zfp_stream_set_bit_stream(stream, 0);
    bytes = 0;
    deallocate_aligned(data);
    data = 0;
    deallocate(shape);
    shape = 0;
  }

  // perform a deep copy
  void deep_copy(const array& a)
  {
    // copy metadata
    dims = a.dims;
    type = a.type;
    nx = a.nx;
    ny = a.ny;
    nz = a.nz;
    bx = a.bx;
    by = a.by;
    bz = a.bz;
    blocks = a.blocks;
    blkvals = a.blkvals;
    blkbits = a.blkbits;
    blksize = a.blksize;
    bytes = a.bytes;

    // copy dynamically allocated data
    clone_aligned(data, a.data, bytes + header_size(), 0x100u);
    if (stream) {
      if (stream->stream)
        stream_close(stream->stream);
      zfp_stream_close(stream);
    }
    stream = zfp_stream_open(0);
    *stream = *a.stream;
    zfp_stream_set_bit_stream(stream, stream_open(data, bytes + header_size()));
    clone(shape, a.shape, blocks);
  }

  size_t header_size_bits() const
  {
    return ZFP_MAGIC_BITS + ZFP_META_BITS + ZFP_MODE_SHORT_BITS;
  }

  // returns offset (in bits) to start of header
  // (header is pre-padded such that header completes the word)
  size_t header_offset_bits() const
  {
    return stream_word_bits - (header_size_bits() % stream_word_bits);
  }

  // returns offset (in bits) to start of compressed stream
  size_t compressed_data_offset_bits() const
  {
    return (header_size_bits() + stream_word_bits - 1) & ~(stream_word_bits - 1);
  }

  uint dims;           // array dimensionality (1, 2, or 3)
  zfp_type type;       // scalar type
  uint nx, ny, nz;     // array dimensions
  uint bx, by, bz;     // array dimensions in number of blocks
  uint blocks;         // number of blocks
  uint blkvals;        // number of values per block
  size_t blkbits;      // number of bits per compressed block
  size_t blksize;      // byte size of single compressed block
  size_t bytes;        // total bytes of compressed data (excluding header)
  mutable uchar* data; // pointer to header (followed by compressed data)
  zfp_stream* stream;  // compressed stream
  uchar* shape;        // precomputed block dimensions (or null if uniform)
};

}

#endif
