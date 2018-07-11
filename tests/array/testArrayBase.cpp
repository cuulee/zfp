void VerifyProperHeaderWritten(uchar* headerPtr, size_t headerSizeBytes, uint chosenSize, double chosenRate)
{
  // verify non empty header
  uint64 checksum = hashBitstream((uint64*)headerPtr, headerSizeBytes);
  EXPECT_NE(0, checksum);

  // verify valid header (manually through C API)
  size_t headerSizeBits = ZFP_MAGIC_BITS + ZFP_META_BITS + ZFP_MODE_SHORT_BITS;
  size_t offsetBits = stream_word_bits - (headerSizeBits % stream_word_bits);
  bitstream* stream = stream_open(headerPtr, headerSizeBytes);
  stream_rseek(stream, offsetBits);

  zfp_field* field = zfp_field_alloc();
  zfp_stream* zfp = zfp_stream_open(stream);
  EXPECT_EQ(headerSizeBits, zfp_read_header(zfp, field, ZFP_HEADER_FULL));

  // verify header contents
  EXPECT_EQ(chosenSize, field->nx);
  EXPECT_EQ(0, field->ny);
  EXPECT_EQ(0, field->nz);

  EXPECT_EQ(ZFP_TYPE, field->type);

  // to verify rate, we can only compare the 4 compression-param basis
  zfp_stream* expectedZfpStream = zfp_stream_open(0);
  zfp_stream_set_rate(expectedZfpStream, chosenRate, ZFP_TYPE, testEnv->getDims(), 1);
  EXPECT_EQ(expectedZfpStream->minbits, zfp->minbits);
  EXPECT_EQ(expectedZfpStream->maxbits, zfp->maxbits);
  EXPECT_EQ(expectedZfpStream->maxprec, zfp->maxprec);
  EXPECT_EQ(expectedZfpStream->minexp, zfp->minexp);

  zfp_stream_close(expectedZfpStream);
  zfp_stream_close(zfp);
  zfp_field_free(field);
  stream_close(stream);
}
