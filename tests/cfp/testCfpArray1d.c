#include "src/traitsd.h"
#include "src/block1.h"

#include "constants/1dDouble.h"
#include "utils/hash64.h"

#define CFP_ARRAY_TYPE cfp_array1d
#define SUB_NAMESPACE array1d

#include "testCfpArray_source.c"
#include "testCfpArray1_source.c"

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(when_seededRandomSmoothDataGenerated_expect_ChecksumMatches),

    cmocka_unit_test(given_cfp_array1d_when_minimalCtor_expect_returnsNonNullPtr),
    cmocka_unit_test_setup_teardown(given_cfp_array1d_when_ctor_expect_paramsSet, setupCfpArrLargeComplete, teardownCfpArr),

    cmocka_unit_test_setup_teardown(given_cfp_array1d_when_setRate_expect_rateSet, setupCfpArrMinimal, teardownCfpArr),
    cmocka_unit_test_setup_teardown(given_cfp_array1d_when_setCacheSize_expect_cacheSizeSet, setupCfpArrSmall, teardownCfpArr),

    cmocka_unit_test_setup_teardown(given_cfp_array1d_with_dirtyCache_when_flushCache_expect_cacheEntriesPersistedToMemory, setupCfpArrSmall, teardownCfpArr),
    cmocka_unit_test_setup_teardown(given_cfp_array1d_when_clearCache_expect_cacheCleared, setupCfpArrSmall, teardownCfpArr),
    cmocka_unit_test_setup_teardown(given_cfp_array1d_when_resize_expect_sizeChanged, setupCfpArrSmall, teardownCfpArr),

    cmocka_unit_test_setup_teardown(given_cfp_array1d_when_setEntry_expect_entryWrittenToCacheOnly, setupCfpArrSmall, teardownCfpArr),
    cmocka_unit_test_setup_teardown(given_cfp_array1d_when_getEntry_expect_entryReturned, setupCfpArrSmall, teardownCfpArr),

    // fixed rate rounds up to multiples of 16 (omit fixed rate 8)
    cmocka_unit_test_setup_teardown(given_cfp_array1d_when_setArray_expect_compressedStreamChecksumMatches, setupFixedRate1, teardownCfpArr),
    cmocka_unit_test_setup_teardown(given_cfp_array1d_when_setArray_expect_compressedStreamChecksumMatches, setupFixedRate2, teardownCfpArr),
    cmocka_unit_test_setup_teardown(given_cfp_array1d_when_getArray_expect_decompressedArrChecksumMatches, setupFixedRate1, teardownCfpArr),
    cmocka_unit_test_setup_teardown(given_cfp_array1d_when_getArray_expect_decompressedArrChecksumMatches, setupFixedRate2, teardownCfpArr),
  };

  return cmocka_run_group_tests(tests, prepCommonSetupVars, teardownCommonSetupVars);
}
