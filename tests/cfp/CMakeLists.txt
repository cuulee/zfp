include_directories(${ZFP_SOURCE_DIR}/cfp/include)

add_executable(testCfpArray1f testCfpArray1f.c)
target_link_libraries(testCfpArray1f cmocka cfp hash32Lib genSmoothRandNumsLib)
add_test(NAME testCfpArray1f COMMAND testCfpArray1f)

add_executable(testCfpArray1d testCfpArray1d.c)
target_link_libraries(testCfpArray1d cmocka cfp hash64Lib genSmoothRandNumsLib)
add_test(NAME testCfpArray1d COMMAND testCfpArray1d)

add_executable(testCfpArray2f testCfpArray2f.c)
target_link_libraries(testCfpArray2f cmocka cfp hash32Lib genSmoothRandNumsLib)
add_test(NAME testCfpArray2f COMMAND testCfpArray2f)

add_executable(testCfpArray2d testCfpArray2d.c)
target_link_libraries(testCfpArray2d cmocka cfp hash64Lib genSmoothRandNumsLib)
add_test(NAME testCfpArray2d COMMAND testCfpArray2d)

add_executable(testCfpArray3f testCfpArray3f.c)
target_link_libraries(testCfpArray3f cmocka cfp hash32Lib genSmoothRandNumsLib)
add_test(NAME testCfpArray3f COMMAND testCfpArray3f)

add_executable(testCfpArray3d testCfpArray3d.c)
target_link_libraries(testCfpArray3d cmocka cfp hash64Lib genSmoothRandNumsLib)
add_test(NAME testCfpArray3d COMMAND testCfpArray3d)
