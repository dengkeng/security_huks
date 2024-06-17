/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hks_hkdf_derive_test_common.h"
#include "hks_test_adapt_for_de.h"

#include <gtest/gtest.h>

using namespace testing::ext;
namespace Unittest::HkdfDerive {
int32_t HksHkdfDeriveTestNormalCase(const struct HksBlob keyAlias, const struct HksParamSet *genParamSet,
    struct HksParamSet *deriveParamSet, struct HksParamSet *deriveFinalParamsSet, int32_t cmpRet)
{
    struct HksBlob inData = {
        (uint32_t)g_inData.length(),
        const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(g_inData.c_str()))
    };
    int32_t ret = HKS_FAILURE;

    /* 1. Generate Key */
    // Generate Key
    ret = HksGenerateKeyForDe(&keyAlias, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey failed.";

    /* 2. Derive Three Stage */
    // Init
    uint8_t handleDTest[sizeof(uint64_t)] = {0};
    struct HksBlob handleDerive = { sizeof(uint64_t), handleDTest };
    ret = HksInitForDe(&keyAlias, deriveParamSet, &handleDerive, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    // Update
    uint8_t tmpOut[COMMON_SIZE] = {0};
    struct HksBlob outData = { COMMON_SIZE, tmpOut };
    ret = HksUpdateForDe(&handleDerive, deriveParamSet, &inData, &outData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Update failed.";
    // Finish
    uint8_t outDataD[COMMON_SIZE] = {0};
    struct HksBlob outDataDerive = { COMMON_SIZE, outDataD };
    ret = HksFinishForDe(&handleDerive, deriveFinalParamsSet, &inData, &outDataDerive);
    EXPECT_EQ(ret, cmpRet) << "Finish failed.";

    return ret;
}
#ifdef L2_STANDARD
int32_t HksHkdfDeriveTestCmpCase(const struct HksBlob keyAlias,
    const struct HksParamSet *genParamSet, struct HksParamSet *deriveParamSet, struct HksParamSet *deriveFinalParamsSet)
{
    struct HksBlob inDataTest = {
        g_inData.length(),
        const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(g_inData.c_str()))
    };
    int32_t ret = HKS_FAILURE;

    /* 1. Generate Key */
    // Generate Key
    ret = HksGenerateKeyForDe(&keyAlias, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey failed.";

    /* 2. Derive Three Stage */
    // Init
    uint8_t handleD[sizeof(uint64_t)] = {0};
    struct HksBlob handleDeriveTest = { sizeof(uint64_t), handleD };
    ret = HksInitForDe(&keyAlias, deriveParamSet, &handleDeriveTest, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    // Update
    uint8_t tmpOut[COMMON_SIZE] = {0};
    struct HksBlob outData = { COMMON_SIZE, tmpOut };
    ret = HksUpdateForDe(&handleDeriveTest, deriveParamSet, &inDataTest, &outData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Update failed.";
    // Finish
    uint8_t outDataD[COMMON_SIZE] = {0};
    struct HksBlob outDataDerive = { COMMON_SIZE, outDataD };
    ret = HksFinishForDe(&handleDeriveTest, deriveFinalParamsSet, &inDataTest, &outDataDerive);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Finish failed.";

    uint8_t tmpDeriveTest[COMMON_SIZE] = {0};
    struct HksBlob derivedKey = { COMMON_SIZE, tmpDeriveTest };
    ret = HksDeriveKeyForDe(deriveParamSet, &keyAlias, &derivedKey);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksDeriveKey failed.";

    ret = HksMemCmp(derivedKey.data, outDataDerive.data, outDataDerive.size);
    EXPECT_EQ(ret, HKS_SUCCESS) << "outDataDerive not equals derivedKey.";

    return ret;
}
#endif
}
