/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <iostream>

#include "hks_ability.h"
#include "hks_config.h"
#include "hks_crypto_hal.h"
#include "hks_crypto_hal_common.h"
#include "hks_mem.h"

using namespace testing::ext;
namespace OHOS {
namespace Security {
namespace Huks {
namespace UnitTest {
namespace {
struct TestCaseParams {
    HksKeySpec spec;
    HksKeyDigest digest;

    HksErrorCode generateKeyResult;
    HksErrorCode hmacResult;
};

const TestCaseParams HKS_CRYPTO_HAL_HMAC_HMAC_001_PARAMS = {
    .spec = {
        .algType = HKS_ALG_HMAC,
        .keyLen = 256,
        .algParam = nullptr,
    },
    .digest = HKS_DIGEST_SHA1,

    .generateKeyResult = HKS_SUCCESS,
    .hmacResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_HMAC_HMAC_002_PARAMS = {
    .spec = {
        .algType = HKS_ALG_HMAC,
        .keyLen = 256,
        .algParam = nullptr,
    },
    .digest = HKS_DIGEST_SHA224,

    .generateKeyResult = HKS_SUCCESS,
    .hmacResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_HMAC_HMAC_003_PARAMS = {
    .spec = {
        .algType = HKS_ALG_HMAC,
        .keyLen = 256,
        .algParam = nullptr,
    },
    .digest = HKS_DIGEST_SHA256,

    .generateKeyResult = HKS_SUCCESS,
    .hmacResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_HMAC_HMAC_004_PARAMS = {
    .spec = {
        .algType = HKS_ALG_HMAC,
        .keyLen = 256,
        .algParam = nullptr,
    },
    .digest = HKS_DIGEST_SHA384,

    .generateKeyResult = HKS_SUCCESS,
    .hmacResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_HMAC_HMAC_005_PARAMS = {
    .spec = {
        .algType = HKS_ALG_HMAC,
        .keyLen = 256,
        .algParam = nullptr,
    },
    .digest = HKS_DIGEST_SHA512,

    .generateKeyResult = HKS_SUCCESS,
    .hmacResult = HKS_SUCCESS,
};
}  // namespace

class HksCryptoHalHmacHmac : public HksCryptoHalCommon, public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
protected:
    void RunTestCase(const TestCaseParams &testCaseParams)
    {
        HksBlob key = { .size = 0, .data = nullptr };

        EXPECT_EQ(HksCryptoHalGenerateKey(&testCaseParams.spec, &key), testCaseParams.generateKeyResult);

        const char *hexData = "00112233445566778899aabbccddeeff";
        uint32_t dataLen = strlen(hexData) / HKS_COUNT_OF_HALF;

        HksBlob message = { .size = dataLen, .data = (uint8_t *)HksMalloc(dataLen) };
        for (uint32_t ii = 0; ii < dataLen; ii++) {
            message.data[ii] = ReadHex((const uint8_t *)&hexData[2 * ii]);
        }
        struct HksBlob signature = { .size = 1024, .data = (uint8_t *)HksMalloc(1024) };

        EXPECT_EQ(HksCryptoHalHmac(&key, testCaseParams.digest, &message, &signature), testCaseParams.hmacResult);

        HksFree(message.data);
        HksFree(signature.data);
        HksFree(key.data);
    }
};

void HksCryptoHalHmacHmac::SetUpTestCase(void)
{
}

void HksCryptoHalHmacHmac::TearDownTestCase(void)
{
}

void HksCryptoHalHmacHmac::SetUp()
{
    EXPECT_EQ(HksCryptoAbilityInit(), 0);
}

void HksCryptoHalHmacHmac::TearDown()
{
}

/**
 * @tc.number    : HksCryptoHalHmacHmac_001
 * @tc.name      : HksCryptoHalHmacHmac_001
 * @tc.desc      : Using HksCryptoHalHmac HMAC HMAC-256-SHA1 key.
 */
HWTEST_F(HksCryptoHalHmacHmac, HksCryptoHalHmacHmac_001, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_HMAC_HMAC_001_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalHmacHmac_002
 * @tc.name      : HksCryptoHalHmacHmac_002
 * @tc.desc      : Using HksCryptoHalHmac HMAC HMAC-256-SHA224 key.
 */
HWTEST_F(HksCryptoHalHmacHmac, HksCryptoHalHmacHmac_002, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_HMAC_HMAC_002_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalHmacHmac_003
 * @tc.name      : HksCryptoHalHmacHmac_003
 * @tc.desc      : Using HksCryptoHalHmac HMAC HMAC-256-SHA256 key.
 */
HWTEST_F(HksCryptoHalHmacHmac, HksCryptoHalHmacHmac_003, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_HMAC_HMAC_003_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalHmacHmac_004
 * @tc.name      : HksCryptoHalHmacHmac_004
 * @tc.desc      : Using HksCryptoHalHmac HMAC HMAC-256-SHA384 key.
 */
HWTEST_F(HksCryptoHalHmacHmac, HksCryptoHalHmacHmac_004, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_HMAC_HMAC_004_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalHmacHmac_005
 * @tc.name      : HksCryptoHalHmacHmac_005
 * @tc.desc      : Using HksCryptoHalHmac HMAC HMAC-256-SHA512 key.
 */
HWTEST_F(HksCryptoHalHmacHmac, HksCryptoHalHmacHmac_005, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_HMAC_HMAC_005_PARAMS);
}
}  // namespace UnitTest
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS