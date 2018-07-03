static void
_catFunc3(given_, CFP_ARRAY_TYPE, _when_ctor_expect_paramsSet)(void **state)
{
  struct setupVars *bundle = *state;
  size_t csize = 300;
  CFP_ARRAY_TYPE* cfpArr = cfp_api.SUB_NAMESPACE.ctor(bundle->dataSideLen, bundle->dataSideLen, bundle->rate, bundle->dataArr, csize);
  assert_non_null(cfpArr);

  assert_int_equal(cfp_api.SUB_NAMESPACE.size(cfpArr), bundle->totalDataLen);

  assert_true(cfp_api.SUB_NAMESPACE.rate(cfpArr) >= bundle->rate);

  uchar* compressedPtr = cfp_api.SUB_NAMESPACE.compressed_data(cfpArr);
  size_t compressedSize = cfp_api.SUB_NAMESPACE.compressed_size(cfpArr);
  assert_int_not_equal(hashBitstream((uint64*)compressedPtr, compressedSize), 0);

  // sets a minimum cache size
  assert_true(cfp_api.SUB_NAMESPACE.cache_size(cfpArr) >= csize);

  cfp_api.SUB_NAMESPACE.dtor(cfpArr);
}

static void
_catFunc3(given_, CFP_ARRAY_TYPE, _when_resize_expect_sizeChanged)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE* cfpArr = bundle->cfpArr;

  size_t newSizeX = 81, newSizeY = 123;
  assert_int_not_equal(cfp_api.SUB_NAMESPACE.size(cfpArr), newSizeX * newSizeY);

  cfp_api.SUB_NAMESPACE.resize(cfpArr, newSizeX, newSizeY, 1);

  assert_int_equal(cfp_api.SUB_NAMESPACE.size_x(cfpArr), newSizeX);
  assert_int_equal(cfp_api.SUB_NAMESPACE.size_y(cfpArr), newSizeY);
  assert_int_equal(cfp_api.SUB_NAMESPACE.size(cfpArr), newSizeX * newSizeY);
}

static void
_catFunc3(given_, CFP_ARRAY_TYPE, _when_setIJ_expect_entryWrittenToCacheOnly)(void **state)
{
  struct setupVars *bundle = *state;

  CFP_ARRAY_TYPE* cfpArr = bundle->cfpArr;

  // getting the ptr automatically flushes cache, so do this before setting an entry
  uchar* compressedDataPtr = cfp_api.SUB_NAMESPACE.compressed_data(cfpArr);
  size_t compressedSize = cfp_api.SUB_NAMESPACE.compressed_size(cfpArr);

  uchar* oldMemory = malloc(compressedSize * sizeof(uchar));
  memcpy(oldMemory, compressedDataPtr, compressedSize);

  cfp_api.SUB_NAMESPACE.set_ij(cfpArr, 1, 1, VAL);

  assert_memory_equal(compressedDataPtr, oldMemory, compressedSize);
  free(oldMemory);
}

static void
_catFunc3(given_, CFP_ARRAY_TYPE, _when_getIJ_expect_entryReturned)(void **state)
{
  struct setupVars *bundle = *state;
  CFP_ARRAY_TYPE* cfpArr = bundle->cfpArr;
  uint i = 1, j = 2;
  cfp_api.SUB_NAMESPACE.set_ij(cfpArr, i, j, VAL);

  assert_true(fabs(cfp_api.SUB_NAMESPACE.get_ij(cfpArr, i, j) - VAL) <= COMPRESS_TOL);
}
