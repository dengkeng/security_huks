/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "openssl_aes_helper.h"

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "hks_api.h"
#include "hks_mem.h"

using namespace testing::ext;
namespace OHOS {
namespace Security {
namespace Huks {
namespace MT {
namespace {
struct TestCaseParams {
    std::vector<HksParam> params;
    std::string hexData;
    uint32_t inLenAppend = 0;

    HksErrorCode generateKeyResult;
    HksErrorCode encryptResult;
    HksErrorCode decryptResult;
};

const uint8_t IV[IV_SIZE] = {0};
const uint8_t AAD_DATA[AAD_SIZE] = {0};

const TestCaseParams HUKS_AES_DECRYPT_MT_00100_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_128 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS7 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = COMPLEMENT_LEN,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_DECRYPT_MT_00200_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_128 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_DECRYPT_MT_00300_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_128 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CTR },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_DECRYPT_MT_00400_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_128 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_DECRYPT_MT_00500_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_128 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS7 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const TestCaseParams HUKS_AES_DECRYPT_MT_00600_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_128 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_GCM },
        { .tag = HKS_TAG_NONCE, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
        { .tag = HKS_TAG_ASSOCIATED_DATA, .blob = { .size = sizeof(AAD_DATA), .data = (uint8_t*)AAD_DATA } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_DECRYPT_MT_00700_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_192 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS7 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = COMPLEMENT_LEN,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_DECRYPT_MT_00800_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_192 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_DECRYPT_MT_00900_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_192 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CTR },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_DECRYPT_MT_01000_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_192 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_DECRYPT_MT_01100_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_192 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS7 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const TestCaseParams HUKS_AES_DECRYPT_MT_01200_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_192 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_GCM },
        { .tag = HKS_TAG_NONCE, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
        { .tag = HKS_TAG_ASSOCIATED_DATA, .blob = { .size = sizeof(AAD_DATA), .data = (uint8_t*)AAD_DATA } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_DECRYPT_MT_01300_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS7 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = COMPLEMENT_LEN,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_DECRYPT_MT_01400_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_DECRYPT_MT_01500_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CTR },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_DECRYPT_MT_01600_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_DECRYPT_MT_01700_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS7 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const TestCaseParams HUKS_AES_DECRYPT_MT_01800_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_GCM },
        { .tag = HKS_TAG_NONCE, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
        { .tag = HKS_TAG_ASSOCIATED_DATA, .blob = { .size = sizeof(AAD_DATA), .data = (uint8_t*)AAD_DATA } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};
}  // namespace

class HksAesDecryptMt : public testing::Test {
protected:
    uint32_t ReadValueByTag(std::vector<HksParam> params, uint32_t needTag)
    {
        for (auto testParams : params) {
            if (testParams.tag == needTag) {
                return testParams.uint32Param;
            }
        }
        return 0;
    }

    void RunTestCase(const TestCaseParams &testCaseParams)
    {
        struct HksBlob authId = { .size = AUTHID_KEY_SIZE, .data = (uint8_t *)HksMalloc(AUTHID_KEY_SIZE) };

        struct HksParamSet *paramInSet = nullptr;
        HksInitParamSet(&paramInSet);

        HksAddParams(paramInSet, testCaseParams.params.data(), testCaseParams.params.size());
        HksBuildParamSet(&paramInSet);

        uint32_t keySize = ReadValueByTag(testCaseParams.params, HKS_TAG_KEY_SIZE);
        uint32_t blockMode = ReadValueByTag(testCaseParams.params, HKS_TAG_BLOCK_MODE);

        EXPECT_EQ(GenerateAesKey(keySize, &authId), testCaseParams.generateKeyResult);

        HksBlob plainText = {
            .size = (uint32_t)testCaseParams.hexData.length(),
            .data = (uint8_t *)&testCaseParams.hexData[0],
        };

        uint32_t inLen = testCaseParams.hexData.length() + COMPLEMENT_LEN;
        HksBlob cipherText = { .size = inLen, .data = (uint8_t *)HksMalloc(inLen) };
        ASSERT_NE(cipherText.data, nullptr);
        HksBlob plainTextDecrypt = { .size = inLen, .data = (uint8_t *)HksMalloc(inLen) };
        ASSERT_NE(plainTextDecrypt.data, nullptr);
        if (blockMode == HKS_MODE_GCM) {
            HksBlob tagAead = { .size = AAD_SIZE, .data = (uint8_t *)HksMalloc(AAD_SIZE) };
            EXPECT_EQ(
                AesGCMEncrypt(paramInSet, &plainText, &cipherText, &authId, &tagAead), testCaseParams.encryptResult);
            cipherText.size += AAD_SIZE;
            (void)memcpy_s(cipherText.data + AAD_SIZE, AAD_SIZE, tagAead.data, AAD_SIZE);
            HksFree(tagAead.data);
        } else {
            EXPECT_EQ(AesEncrypt(paramInSet, &plainText, &cipherText, &authId), testCaseParams.encryptResult);
        }
        EXPECT_EQ(HksDecrypt(&authId, paramInSet, &cipherText, &plainTextDecrypt), testCaseParams.decryptResult);
        if (testCaseParams.decryptResult != HKS_ERROR_NOT_SUPPORTED) {
            EXPECT_EQ(plainTextDecrypt.size, testCaseParams.hexData.length());
            EXPECT_EQ(HksMemCmp(plainText.data, plainTextDecrypt.data, testCaseParams.hexData.length()), 0);
        }

        HksFree(authId.data);
        HksFree(cipherText.data);
        HksFree(plainTextDecrypt.data);
        HksFreeParamSet(&paramInSet);
    }
};

/**
 * @tc.number    : HksAesDecryptMt.HksAesDecryptMt00100
 * @tc.name      : HksAesDecryptMt00100
 * @tc.desc      : OpenSSL generates an aes128 bit key, which can be successfully used for OpenSSL encryption using
 * AES/CBC/pkcs7padding algorithm and huks decryption using AES/CBC/nopadding algorithm
 */
HWTEST_F(HksAesDecryptMt, HksAesDecryptMt00100, TestSize.Level0)
{
    RunTestCase(HUKS_AES_DECRYPT_MT_00100_PARAMS);
}

/**
 * @tc.number    : HksAesDecryptMt.HksAesDecryptMt00200
 * @tc.name      : HksAesDecryptMt00200
 * @tc.desc      : OpenSSL generates an aes128 bit key, which can be successfully used for OpenSSL encryption using
 * AES/CBC/nopadding algorithm and huks decryption using AES/CBC/nopadding algorithm
 */
HWTEST_F(HksAesDecryptMt, HksAesDecryptMt00200, TestSize.Level0)
{
    RunTestCase(HUKS_AES_DECRYPT_MT_00200_PARAMS);
}

/**
 * @tc.number    : HksAesDecryptMt.HksAesDecryptMt00300
 * @tc.name      : HksAesDecryptMt00300
 * @tc.desc      : OpenSSL generates an aes128 bit key, which can be successfully used for OpenSSL encryption using
 * AES/CTR/nopadding algorithm and huks decryption using AES/CTR/nopadding algorithm
 */
HWTEST_F(HksAesDecryptMt, HksAesDecryptMt00300, TestSize.Level0)
{
    RunTestCase(HUKS_AES_DECRYPT_MT_00300_PARAMS);
}

/**
 * @tc.number    : HksAesDecryptMt.HksAesDecryptMt00400
 * @tc.name      : HksAesDecryptMt00400
 * @tc.desc      : OpenSSL generates an aes128 bit key, which can be successfully used for OpenSSL encryption using
 * AES/ECB/nopadding algorithm and huks decryption using AES/ECB/nopadding algorithm
 */
HWTEST_F(HksAesDecryptMt, HksAesDecryptMt00400, TestSize.Level0)
{
    RunTestCase(HUKS_AES_DECRYPT_MT_00400_PARAMS);
}

/**
 * @tc.number    : HksAesDecryptMt.HksAesDecryptMt00500
 * @tc.name      : HksAesDecryptMt00500
 * @tc.desc      : OpenSSL generates an aes128 bit key, which can be successfully used for OpenSSL encryption using
 * AES/ECB/pkcs7padding algorithm and huks decryption using AES/ECB/pkcs7padding algorithm
 */
HWTEST_F(HksAesDecryptMt, HksAesDecryptMt00500, TestSize.Level0)
{
    RunTestCase(HUKS_AES_DECRYPT_MT_00500_PARAMS);
}

/**
 * @tc.number    : HksAesDecryptMt.HksAesDecryptMt00600
 * @tc.name      : HksAesDecryptMt00600
 * @tc.desc      : OpenSSL generates an aes128 bit key, which can be successfully used for OpenSSL encryption using
 * AES/GCM/nopadding algorithm and huks decryption using AES/GCM/nopadding algorithm
 */
HWTEST_F(HksAesDecryptMt, HksAesDecryptMt00600, TestSize.Level0)
{
    RunTestCase(HUKS_AES_DECRYPT_MT_00600_PARAMS);
}

/**
 * @tc.number    : HksAesDecryptMt.HksAesDecryptMt00700
 * @tc.name      : HksAesDecryptMt00700
 * @tc.desc      : OpenSSL generates an aes192 bit key, which can be successfully used for OpenSSL encryption using
 * AES/CBC/pkcs7padding algorithm and huks decryption using AES/CBC/nopadding algorithm
 */
HWTEST_F(HksAesDecryptMt, HksAesDecryptMt00700, TestSize.Level0)
{
    RunTestCase(HUKS_AES_DECRYPT_MT_00700_PARAMS);
}

/**
 * @tc.number    : HksAesDecryptMt.HksAesDecryptMt00800
 * @tc.name      : HksAesDecryptMt00800
 * @tc.desc      : OpenSSL generates an aes192 bit key, which can be successfully used for OpenSSL encryption using
 * AES/CBC/nopadding algorithm and huks decryption using AES/CBC/nopadding algorithm
 */
HWTEST_F(HksAesDecryptMt, HksAesDecryptMt00800, TestSize.Level0)
{
    RunTestCase(HUKS_AES_DECRYPT_MT_00800_PARAMS);
}

/**
 * @tc.number    : HksAesDecryptMt.HksAesDecryptMt00900
 * @tc.name      : HksAesDecryptMt00900
 * @tc.desc      : OpenSSL generates an aes192 bit key, which can be successfully used for OpenSSL encryption using
 * AES/CTR/nopadding algorithm and huks decryption using AES/CTR/nopadding algorithm
 */
HWTEST_F(HksAesDecryptMt, HksAesDecryptMt00900, TestSize.Level0)
{
    RunTestCase(HUKS_AES_DECRYPT_MT_00900_PARAMS);
}

/**
 * @tc.number    : HksAesDecryptMt.HksAesDecryptMt01000
 * @tc.name      : HksAesDecryptMt01000
 * @tc.desc      : OpenSSL generates an aes192 bit key, which can be successfully used for OpenSSL encryption using
 * AES/ECB/nopadding algorithm and huks decryption using AES/ECB/nopadding algorithm
 */
HWTEST_F(HksAesDecryptMt, HksAesDecryptMt01000, TestSize.Level0)
{
    RunTestCase(HUKS_AES_DECRYPT_MT_01000_PARAMS);
}

/**
 * @tc.number    : HksAesDecryptMt.HksAesDecryptMt01100
 * @tc.name      : HksAesDecryptMt01100
 * @tc.desc      : OpenSSL generates an aes192 bit key, which can be successfully used for OpenSSL encryption using
 * AES/ECB/pkcs7padding algorithm and huks decryption using AES/ECB/pkcs7padding algorithm
 */
HWTEST_F(HksAesDecryptMt, HksAesDecryptMt01100, TestSize.Level0)
{
    RunTestCase(HUKS_AES_DECRYPT_MT_01100_PARAMS);
}

/**
 * @tc.number    : HksAesDecryptMt.HksAesDecryptMt01200
 * @tc.name      : HksAesDecryptMt01200
 * @tc.desc      : OpenSSL generates an aes192 bit key, which can be successfully used for OpenSSL encryption using
 * AES/GCM/nopadding algorithm and huks decryption using AES/GCM/nopadding algorithm
 */
HWTEST_F(HksAesDecryptMt, HksAesDecryptMt01200, TestSize.Level0)
{
    RunTestCase(HUKS_AES_DECRYPT_MT_01200_PARAMS);
}

/**
 * @tc.number    : HksAesDecryptMt.HksAesDecryptMt01300
 * @tc.name      : HksAesDecryptMt01300
 * @tc.desc      : OpenSSL generates an aes256 bit key, which can be successfully used for OpenSSL encryption using
 * AES/CBC/pkcs7padding algorithm and huks decryption using AES/CBC/nopadding algorithm
 */
HWTEST_F(HksAesDecryptMt, HksAesDecryptMt01300, TestSize.Level0)
{
    RunTestCase(HUKS_AES_DECRYPT_MT_01300_PARAMS);
}

/**
 * @tc.number    : HksAesDecryptMt.HksAesDecryptMt01400
 * @tc.name      : HksAesDecryptMt01400
 * @tc.desc      : OpenSSL generates an aes256 bit key, which can be successfully used for OpenSSL encryption using
 * AES/CBC/nopadding algorithm and huks decryption using AES/CBC/nopadding algorithm
 */
HWTEST_F(HksAesDecryptMt, HksAesDecryptMt01400, TestSize.Level0)
{
    RunTestCase(HUKS_AES_DECRYPT_MT_01400_PARAMS);
}

/**
 * @tc.number    : HksAesDecryptMt.HksAesDecryptMt01500
 * @tc.name      : HksAesDecryptMt01500
 * @tc.desc      : OpenSSL generates an aes256 bit key, which can be successfully used for OpenSSL encryption using
 * AES/CTR/nopadding algorithm and huks decryption using AES/CTR/nopadding algorithm
 */
HWTEST_F(HksAesDecryptMt, HksAesDecryptMt01500, TestSize.Level0)
{
    RunTestCase(HUKS_AES_DECRYPT_MT_01500_PARAMS);
}
/**
 * @tc.number    : HksAesDecryptMt.HksAesDecryptMt01600
 * @tc.name      : HksAesDecryptMt01600
 * @tc.desc      : OpenSSL generates an aes256 bit key, which can be successfully used for OpenSSL encryption using
 * AES/ECB/nopadding algorithm and huks decryption using AES/ECB/nopadding algorithm
 */
HWTEST_F(HksAesDecryptMt, HksAesDecryptMt01600, TestSize.Level0)
{
    RunTestCase(HUKS_AES_DECRYPT_MT_01600_PARAMS);
}

/**
 * @tc.number    : HksAesDecryptMt.HksAesDecryptMt01700
 * @tc.name      : HksAesDecryptMt01700
 * @tc.desc      : OpenSSL generates an aes256 bit key, which can be successfully used for OpenSSL encryption using
 * AES/ECB/pkcs7padding algorithm and huks decryption using AES/ECB/pkcs7padding algorithm
 */
HWTEST_F(HksAesDecryptMt, HksAesDecryptMt01700, TestSize.Level0)
{
    RunTestCase(HUKS_AES_DECRYPT_MT_01700_PARAMS);
}

/**
 * @tc.number    : HksAesDecryptMt.HksAesDecryptMt01800
 * @tc.name      : HksAesDecryptMt01800
 * @tc.desc      : OpenSSL generates an aes256 bit key, which can be successfully used for OpenSSL encryption using
 * AES/GCM/nopadding algorithm and huks decryption using AES/GCM/nopadding algorithm
 */
HWTEST_F(HksAesDecryptMt, HksAesDecryptMt01800, TestSize.Level0)
{
    RunTestCase(HUKS_AES_DECRYPT_MT_01800_PARAMS);
}
}  // namespace MT
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS