#include "array/zfparray1.h"
using namespace zfp;

extern "C" {
  #include "constants/1dFloat.h"
  #include "utils/hash32.h"
};

#include "gtest/gtest.h"
#include "utils/gtestFloatEnv.h"
#include "utils/gtestBaseFixture.h"
#include "utils/predicates.h"

class Array1fTestEnv : public ArrayFloatTestEnv {
public:
  virtual int getDims() { return 1; }
};

Array1fTestEnv* const testEnv = new Array1fTestEnv;

#include "testArrayBase.cpp"

class Array1fTest : public ArrayNdTestFixture {};

TEST_F(Array1fTest, when_constructorCalled_then_rateSetWithWriteRandomAccess)
{
  double rate = ZFP_RATE_PARAM_BITS;
  array1f arr(inputDataTotalLen, rate);
  EXPECT_LT(rate, arr.rate());
}

TEST_F(Array1fTest, when_setRate_then_compressionRateChanged)
{
  double oldRate = ZFP_RATE_PARAM_BITS;
  array1f arr(inputDataTotalLen, oldRate, inputDataArr);

  double actualOldRate = arr.rate();
  size_t oldCompressedSize = arr.compressed_size();
  uint64 oldChecksum = hashBitstream((uint64*)arr.compressed_data(), oldCompressedSize);

  double newRate = oldRate - 10;
  EXPECT_LT(1, newRate);
  arr.set_rate(newRate);
  EXPECT_GT(actualOldRate, arr.rate());

  arr.set(inputDataArr);
  size_t newCompressedSize = arr.compressed_size();
  uint64 checksum = hashBitstream((uint64*)arr.compressed_data(), newCompressedSize);

  EXPECT_PRED_FORMAT2(ExpectNeqPrintHexPred, oldChecksum, checksum);

  EXPECT_GT(oldCompressedSize, newCompressedSize);
}

TEST_F(Array1fTest, given_defaultConstructor_when_sizeAndRateSet_then_headerWritten)
{
  array1f arr;

  // set size and rate
  uint chosenSize = 55;
  arr.resize(chosenSize);
  double chosenRate = ZFP_RATE_PARAM_BITS;
  arr.set_rate(chosenRate);

  VerifyProperHeaderWritten(arr.header_data(), arr.header_size(), chosenSize, chosenRate);
}

TEST_F(Array1fTest, when_constructorWithSizeAndRate_then_headerWritten)
{
  uint chosenSize = 55;
  double chosenRate = ZFP_RATE_PARAM_BITS;
  array1f arr(chosenSize, chosenRate);

  VerifyProperHeaderWritten(arr.header_data(), arr.header_size(), chosenSize, chosenRate);
}

TEST_F(Array1fTest, when_resize_then_headerUpdated)
{
  uint oldSize = 55;
  double chosenRate = ZFP_RATE_PARAM_BITS;
  array1f arr(oldSize, chosenRate);

  VerifyProperHeaderWritten(arr.header_data(), arr.header_size(), oldSize, chosenRate);

  uint newSize = oldSize + 1;
  arr.resize(newSize);

  VerifyProperHeaderWritten(arr.header_data(), arr.header_size(), newSize, chosenRate);
}

TEST_F(Array1fTest, when_setRate_then_headerUpdated)
{
  uint chosenSize = 55;
  double oldRate = ZFP_RATE_PARAM_BITS;
  array1f arr(chosenSize, oldRate);

  VerifyProperHeaderWritten(arr.header_data(), arr.header_size(), chosenSize, oldRate);

  // ensure new rate is different, so header will change
  oldRate = arr.rate();
  double newRate = oldRate + 0.5;
  EXPECT_LT(oldRate, arr.set_rate(newRate));

  VerifyProperHeaderWritten(arr.header_data(), arr.header_size(), chosenSize, newRate);
}

TEST_F(Array1fTest, when_generateRandomData_then_checksumMatches)
{
  EXPECT_PRED_FORMAT2(ExpectEqPrintHexPred, CHECKSUM_ORIGINAL_DATA_ARRAY, hashArray((uint32*)inputDataArr, inputDataTotalLen, 1));
}

// with write random access in 1d, fixed-rate params rounded up to multiples of 16
INSTANTIATE_TEST_CASE_P(TestManyCompressionRates, Array1fTest, ::testing::Values(1, 2));

TEST_P(Array1fTest, given_dataset_when_set_then_underlyingBitstreamChecksumMatches)
{
  array1f arr(inputDataTotalLen, getRate());

  uint64 expectedChecksum = getExpectedBitstreamChecksum();
  uint64 checksum = hashBitstream((uint64*)arr.compressed_data(), arr.compressed_size());
  EXPECT_PRED_FORMAT2(ExpectNeqPrintHexPred, expectedChecksum, checksum);

  arr.set(inputDataArr);

  checksum = hashBitstream((uint64*)arr.compressed_data(), arr.compressed_size());
  EXPECT_PRED_FORMAT2(ExpectEqPrintHexPred, expectedChecksum, checksum);
}

TEST_P(Array1fTest, given_setArray1f_when_get_then_decompressedValsReturned)
{
  array1f arr(inputDataTotalLen, getRate());
  arr.set(inputDataArr);

  float* decompressedArr = new float[inputDataTotalLen];
  arr.get(decompressedArr);

  uint64 expectedChecksum = getExpectedDecompressedChecksum();
  uint32 checksum = hashArray((uint32*)decompressedArr, inputDataTotalLen, 1);
  EXPECT_PRED_FORMAT2(ExpectEqPrintHexPred, expectedChecksum, checksum);

  delete[] decompressedArr;
}

TEST_P(Array1fTest, given_populatedCompressedArray_when_resizeWithClear_then_bitstreamZeroed)
{
  array1f arr(inputDataTotalLen, getRate());
  arr.set(inputDataArr);
  EXPECT_NE(0u, hashBitstream((uint64*)arr.compressed_data(), arr.compressed_size()));

  arr.resize(inputDataTotalLen + 1, true);

  EXPECT_EQ(0u, hashBitstream((uint64*)arr.compressed_data(), arr.compressed_size()));
}

TEST_P(Array1fTest, when_configureCompressedArrayFromDefaultConstructor_then_bitstreamChecksumMatches)
{
  array1f arr;
  arr.resize(inputDataTotalLen, false);
  arr.set_rate(getRate());
  arr.set(inputDataArr);

  uint64 expectedChecksum = getExpectedBitstreamChecksum();
  uint64 checksum = hashBitstream((uint64*)arr.compressed_data(), arr.compressed_size());
  EXPECT_PRED_FORMAT2(ExpectEqPrintHexPred, expectedChecksum, checksum);
}

void CheckMemberVarsCopied(array1f arr1, array1f arr2)
{
  double oldRate = arr1.rate();
  size_t oldCompressedSize = arr1.compressed_size();
  size_t oldSize = arr1.size();
  size_t oldCacheSize = arr1.cache_size();

  arr1.set_rate(oldRate + 10);
  arr1.resize(oldSize - 10);
  arr1.set(inputDataArr);
  arr1.set_cache_size(oldCacheSize + 10);

  EXPECT_EQ(oldRate, arr2.rate());
  EXPECT_EQ(oldCompressedSize, arr2.compressed_size());
  EXPECT_EQ(oldSize, arr2.size());
  EXPECT_EQ(oldCacheSize, arr2.cache_size());
}

void CheckDeepCopyPerformed(array1f arr1, array1f arr2, uchar* arr1UnflushedBitstreamPtr)
{
  // flush arr2 first, to ensure arr1 remains unflushed
  uint64 checksum = hashBitstream((uint64*)arr2.compressed_data(), arr2.compressed_size());
  uint64 arr1UnflushedChecksum = hashBitstream((uint64*)arr1UnflushedBitstreamPtr, arr1.compressed_size());
  EXPECT_PRED_FORMAT2(ExpectNeqPrintHexPred, arr1UnflushedChecksum, checksum);

  // flush arr1, compute its checksum, clear its bitstream, re-compute arr2's checksum
  uint64 expectedChecksum = hashBitstream((uint64*)arr1.compressed_data(), arr1.compressed_size());
  arr1.resize(arr1.size());
  checksum = hashBitstream((uint64*)arr2.compressed_data(), arr2.compressed_size());
  EXPECT_PRED_FORMAT2(ExpectEqPrintHexPred, expectedChecksum, checksum);

  // verify headers are the same
  // (calling header_data() flushes cache, so do this after verifying compressed streams)
  uint64 header1Checksum = hashBitstream((uint64*)arr1.header_data(), arr1.header_size());
  uint64 header2Checksum = hashBitstream((uint64*)arr2.header_data(), arr2.header_size());
  EXPECT_PRED_FORMAT2(ExpectEqPrintHexPred, header1Checksum, header2Checksum);
}

TEST_P(Array1fTest, given_compressedArray_when_copyConstructor_then_memberVariablesCopied)
{
  array1f arr(inputDataTotalLen, getRate(), inputDataArr, 128);

  array1f arr2(arr);

  CheckMemberVarsCopied(arr, arr2);
}

TEST_P(Array1fTest, given_compressedArray_when_copyConstructor_then_deepCopyPerformed)
{
  // create arr with dirty cache
  array1f arr(inputDataTotalLen, getRate(), inputDataArr);
  uchar* arrUnflushedBitstreamPtr = arr.compressed_data();
  arr[0] = 999;

  array1f arr2(arr);

  CheckDeepCopyPerformed(arr, arr2, arrUnflushedBitstreamPtr);
}

TEST_P(Array1fTest, given_compressedArray_when_setSecondArrayEqualToFirst_then_memberVariablesCopied)
{
  array1f arr(inputDataTotalLen, getRate(), inputDataArr, 128);

  array1f arr2 = arr;

  CheckMemberVarsCopied(arr, arr2);
}

TEST_P(Array1fTest, given_compressedArray_when_setSecondArrayEqualToFirst_then_deepCopyPerformed)
{
  // create arr with dirty cache
  array1f arr(inputDataTotalLen, getRate(), inputDataArr);
  uchar* arrUnflushedBitstreamPtr = arr.compressed_data();
  arr[0] = 999;

  array1f arr2 = arr;

  CheckDeepCopyPerformed(arr, arr2, arrUnflushedBitstreamPtr);
}

TEST_P(Array1fTest, when_fullConstructor_then_headerWritten)
{
  array1f arr(inputDataTotalLen, getRate(), inputDataArr);

  VerifyProperHeaderWritten(arr.header_data(), arr.header_size(), inputDataTotalLen, getRate());
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  static_cast<void>(::testing::AddGlobalTestEnvironment(testEnv));
  return RUN_ALL_TESTS();
}
