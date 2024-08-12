/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifdef HKS_CONFIG_FILE
#include HKS_CONFIG_FILE
#else
#include "hks_config.h"
#endif

#include "hks_core_service_key_generate.h"

#include <stdbool.h>
#include <stddef.h>

#include "dcm_attest.h"
#include "hks_check_paramset.h"
#include "hks_client_service_adapter_common.h"
#include "hks_common_check.h"
#include "hks_crypto_adapter.h"
#include "hks_crypto_hal.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_param.h"
#include "hks_secure_access.h"
#include "hks_sm_import_wrap_key.h"
#include "hks_template.h"
#include "hks_type_inner.h"

#include "securec.h"

#ifndef _HARDWARE_ROOT_KEY_
#include "hks_rkc.h"
#endif

static const uint8_t g_defaultRsaPubExponent[] = { 0x01, 0x00, 0x01 }; /* default 65537 */

static int32_t GetGenType(const struct HksParamSet *paramSet, uint32_t *genType)
{
    struct HksParam *keyGenTypeParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_KEY_GENERATE_TYPE, &keyGenTypeParam);
    /* if not set KEY_GENERATE_TYPE, gen key by default type */
    HKS_IF_NOT_SUCC_RETURN(ret, HKS_SUCCESS)

    struct HksParam *keyAlgParam = NULL;
    ret = HksGetParam(paramSet, HKS_TAG_ALGORITHM, &keyAlgParam);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, HKS_ERROR_CHECK_GET_ALG_FAIL, "get alg tag fail")

    ret = HKS_ERROR_INVALID_ARGUMENT;
    switch (keyGenTypeParam->uint32Param) {
        case HKS_KEY_GENERATE_TYPE_DEFAULT:
            *genType = HKS_KEY_GENERATE_TYPE_DEFAULT;
            ret = HKS_SUCCESS;
            break;
        case HKS_KEY_GENERATE_TYPE_AGREE:
            if (keyAlgParam->uint32Param == HKS_ALG_AES) { /* only aes key can be generated by agree */
                *genType = HKS_KEY_GENERATE_TYPE_AGREE;
                ret = HKS_SUCCESS;
            } else {
                *genType = HKS_KEY_GENERATE_TYPE_DEFAULT;
                ret = HKS_SUCCESS;
            }
            break;
        default:
            HKS_LOG_E("invalid generated key type");
    }

    return ret;
}

#ifdef HKS_SUPPORT_ED25519_TO_X25519
static int32_t CheckAgreeKeyIn(const struct HksBlob *key)
{
    HKS_IF_NOT_SUCC_RETURN(CheckBlob(key), HKS_ERROR_INVALID_ARGUMENT)

    if (key->size < sizeof(struct Hks25519KeyPair)) {
        HKS_LOG_E("invlaid agree key size");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    struct Hks25519KeyPair *keyPair = (struct Hks25519KeyPair *)(key->data);
    if ((keyPair->privateBufferSize > (key->size - sizeof(*keyPair))) ||
        (keyPair->publicBufferSize > (key->size - sizeof(*keyPair) - keyPair->privateBufferSize))) {
        HKS_LOG_E("invlaid agree key size, small than keyPair");
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    return HKS_SUCCESS;
}

static int32_t GetAgreeBaseKey(const bool isPubKey, const bool isPlainPubKey, const struct HksBlob *keyIn,
    struct HksBlob *keyOut)
{
    struct Hks25519KeyPair *keyPair = (struct Hks25519KeyPair *)(keyIn->data);
    uint32_t size = isPubKey ? keyPair->publicBufferSize : keyPair->privateBufferSize;
    uint8_t *buffer = (uint8_t *)HksMalloc(size);
    HKS_IF_NULL_LOGE_RETURN(buffer, HKS_ERROR_MALLOC_FAIL, "malloc failed")

    uint8_t *tmp = isPubKey ? (keyIn->data + sizeof(*keyPair)) :
        (keyIn->data + sizeof(*keyPair) + keyPair->publicBufferSize);
    (void)memcpy_s(buffer, size, tmp, size);

    if (isPlainPubKey) { /* public key is plain key, only copy */
        keyOut->data = buffer;
        keyOut->size = size;
        return HKS_SUCCESS;
    }

    struct HksBlob tempKey = { size, buffer };
    struct HksKeyNode *keyNode = HksGenerateKeyNode(&tempKey);
    (void)memset_s(buffer, size, 0, size);
    HKS_FREE(buffer);
    HKS_IF_NULL_LOGE_RETURN(keyNode, HKS_ERROR_CORRUPT_FILE, "generating keynode with agree key failed")

    bool isSupportUserAuth = false;
    int32_t ret = HksCheckKeybBlobIsSupportUserAuth(keyNode->paramSet, &isSupportUserAuth);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("HksCheckKeybBlobIsSupportUserAuth failed");
        HksFreeKeyNode(&keyNode);
        return ret;
    }

    if (isSupportUserAuth) {
        HKS_LOG_E("key should do user auth, but one stage api do not support user auth operation");
        HksFreeKeyNode(&keyNode);
        return HKS_ERROR_NOT_SUPPORTED;
    }

    ret = HksGetRawKey(keyNode->paramSet, keyOut);
    HKS_IF_NOT_SUCC_LOGE(ret, "get raw key during key agreement failed!")

    HksFreeKeyNode(&keyNode);
    return ret;
}

static int32_t GetAgreePriKey(const struct HksBlob *keyIn, struct HksBlob *keyOut)
{
    return GetAgreeBaseKey(false, false, keyIn, keyOut);
}

static int32_t GetAgreePubKey(const struct HksBlob *keyIn, const struct HksParamSet *paramSet, struct HksBlob *keyOut)
{
    struct HksParam *isKeyAliasParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_AGREE_PUBLIC_KEY_IS_KEY_ALIAS, &isKeyAliasParam);
    if ((ret == HKS_SUCCESS) && (!(isKeyAliasParam->boolParam))) {
        return GetAgreeBaseKey(true, true, keyIn, keyOut);
    }

    return GetAgreeBaseKey(true, false, keyIn, keyOut);
}

static int32_t GenAgreeKey(const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, struct HksBlob *agreedKey)
{
    struct HksParam *agreeAlgParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_AGREE_ALG, &agreeAlgParam);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "generate key not set agree alg")

    agreedKey->size = HKS_KEY_BYTES(HKS_AES_KEY_SIZE_256);
    agreedKey->data = (uint8_t *)HksMalloc(agreedKey->size);
    HKS_IF_NULL_LOGE_RETURN(agreedKey->data, HKS_ERROR_MALLOC_FAIL, "malloc failed")

    struct HksKeySpec agreeSpec = { 0, HKS_CURVE25519_KEY_SIZE_256, NULL };
    if (agreeAlgParam->uint32Param == HKS_ALG_X25519) {
        agreeSpec.algType = HKS_ALG_X25519;
    } else if (agreeAlgParam->uint32Param == HKS_ALG_ED25519) {
        agreeSpec.algType = HKS_ALG_ED25519;
    } else {
        HKS_FREE(agreedKey->data);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    ret = HksCryptoHalAgreeKey(privateKey, peerPublicKey, &agreeSpec, agreedKey);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("agree key failed, ret = %" LOG_PUBLIC "d", ret);
        HKS_FREE(agreedKey->data); /* X25519AgreeKey will memset sharedKey if fail */
    }
    return ret;
}

static int32_t GenKeyByAgree(const struct HksBlob *keyIn, const struct HksParamSet *paramSet,
    struct HksBlob *sharedKey)
{
    int32_t ret = CheckAgreeKeyIn(keyIn);
    HKS_IF_NOT_SUCC_RETURN(ret, ret)

    struct HksBlob priKey = { 0, NULL };
    struct HksBlob pubKey = { 0, NULL };
    do {
        ret = GetAgreePriKey(keyIn, &priKey);
        HKS_IF_NOT_SUCC_BREAK(ret)

        ret = GetAgreePubKey(keyIn, paramSet, &pubKey);
        HKS_IF_NOT_SUCC_BREAK(ret)

        ret = GenAgreeKey(paramSet, &priKey, &pubKey, sharedKey);
    } while (0);

    if (priKey.data != NULL) {
        (void)memset_s(priKey.data, priKey.size, 0, priKey.size);
        HKS_FREE_BLOB(priKey);
    }
    if (pubKey.data != NULL) {
        (void)memset_s(pubKey.data, pubKey.size, 0, pubKey.size);
        HKS_FREE_BLOB(pubKey);
    }
    return ret;
}
#endif

int32_t HksCoreGenerateKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    const struct HksBlob *keyIn, struct HksBlob *keyOut)
{
    int32_t ret = HksCoreCheckGenKeyParams(keyAlias, paramSet, keyIn, keyOut, HKS_KEY_FLAG_GENERATE_KEY);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "hks core check generate key params failed, ret:%" LOG_PUBLIC "x!", ret)

    uint32_t genType = HKS_KEY_GENERATE_TYPE_DEFAULT;
    ret = GetGenType(paramSet, &genType);
    HKS_IF_NOT_SUCC_RETURN(ret, ret)

    HKS_IF_NOT_SUCC_LOGE_RETURN(CheckIfNeedIsDevicePasswordSet(paramSet), HKS_ERROR_DEVICE_PASSWORD_UNSET,
        "a device password is required but not set yet!")

    struct HksBlob key = { 0, NULL };
    switch (genType) {
        case HKS_KEY_GENERATE_TYPE_DEFAULT: {
            struct HksKeySpec spec = {0};
            HksFillKeySpec(paramSet, &spec);
            ret = HksCryptoHalGenerateKey(&spec, &key);
            break;
        }
        case HKS_KEY_GENERATE_TYPE_AGREE:
#ifdef HKS_SUPPORT_ED25519_TO_X25519
            ret = GenKeyByAgree(keyIn, paramSet, &key);
#else
            ret = HKS_ERROR_INVALID_ARGUMENT;
#endif
            break;
        default:
            ret = HKS_ERROR_INVALID_ARGUMENT;
    }
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "GenerateKey failed, ret:%" LOG_PUBLIC "x!", ret)

    ret = HksBuildKeyBlob(keyAlias, HKS_KEY_FLAG_GENERATE_KEY, &key, paramSet, keyOut);
    (void)memset_s(key.data, key.size, 0, key.size);
    HKS_FREE_BLOB(key);
    return ret;
}

static int32_t AddProcessIdentityInfoToParamSet(const struct HksParamSet *inParamSet, struct HksParamSet *paramSet)
{
    uint32_t transferTagList[] = { HKS_TAG_ACCESS_TOKEN_ID, HKS_TAG_USER_ID, HKS_TAG_PROCESS_NAME };
    int32_t ret;
    for (uint32_t i = 0; i < HKS_ARRAY_SIZE(transferTagList); ++i) {
        struct HksParam *tmpParam = NULL;
        ret = HksGetParam(inParamSet, transferTagList[i], &tmpParam);
        HKS_IF_NOT_SUCC_LOGE_RETURN(ret, HKS_ERROR_BAD_STATE, "get param %" LOG_PUBLIC "u failed.", i)

        ret = HksAddParams(paramSet, tmpParam, 1);
        HKS_IF_NOT_SUCC_LOGE_RETURN(ret, HKS_ERROR_BAD_STATE, "add param %" LOG_PUBLIC "u failed.", i)
    }
    return ret;
}

static int32_t AddAgreeKeyParamSetFromUnwrapSuite(uint32_t suite, const struct HksParamSet *inParamSet,
    struct HksParamSet *paramSet)
{
    uint32_t alg = HKS_ALG_X25519;
    uint32_t keySize = HKS_CURVE25519_KEY_SIZE_256;
    switch (suite) {
        case HKS_UNWRAP_SUITE_X25519_AES_256_GCM_NOPADDING:
            alg = HKS_ALG_X25519;
            keySize = HKS_CURVE25519_KEY_SIZE_256;
            break;
        case HKS_UNWRAP_SUITE_ECDH_AES_256_GCM_NOPADDING:
            alg = HKS_ALG_ECDH;
            keySize = HKS_ECC_KEY_SIZE_256;
            break;
        case HKS_UNWRAP_SUITE_SM2_SM4_128_CBC_PKCS7:
        case HKS_UNWRAP_SUITE_SM2_SM4_128_CBC_PKCS7_WITH_VERIFY_DIG_SM3:
            alg = HKS_ALG_SM2;
            keySize = HKS_SM2_KEY_SIZE_256;
            break;
        default:
            HKS_LOG_E("invalid suite type use x25519 default");
            break;
    }

    struct HksParam agreeParams[] = {
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = alg },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = keySize }
    };

    int32_t ret = HksAddParams(paramSet, agreeParams, sizeof(agreeParams) / sizeof(struct HksParam));
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "unwrap suite add params failed.")

    return AddProcessIdentityInfoToParamSet(inParamSet, paramSet);
}

static int32_t GenAgreeKeyParamSetFromUnwrapSuite(uint32_t suite, const struct HksParamSet *inParamSet,
    struct HksParamSet **outParamSet)
{
    struct HksParamSet *paramSet = NULL;
    int32_t ret = HksInitParamSet(&paramSet);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "init agree_key param set fail")

    ret = AddAgreeKeyParamSetFromUnwrapSuite(suite, inParamSet, paramSet);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("unwrap suite add params failed.");
        HksFreeParamSet(&paramSet);
        return ret;
    }

    ret = HksBuildParamSet(&paramSet);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("unwrap suite build params failed.");
        HksFreeParamSet(&paramSet);
        return ret;
    }

    *outParamSet = paramSet;
    return HKS_SUCCESS;
}

static int32_t BuildDecryptUsageSpecOfUnwrap(const struct HksBlob *aad, const struct HksBlob *nonce,
    const struct HksBlob *aeadTag, uint32_t payloadLen, struct HksUsageSpec *usageSpec)
{
    usageSpec->mode = HKS_MODE_GCM;
    usageSpec->padding = HKS_PADDING_NONE;
    usageSpec->digest = HKS_DIGEST_NONE;
    usageSpec->algType = HKS_ALG_AES;

    struct HksAeadParam *aeadParam = (struct HksAeadParam *)HksMalloc(sizeof(struct HksAeadParam));
    HKS_IF_NULL_LOGE_RETURN(aeadParam, HKS_ERROR_MALLOC_FAIL, "build dec wrapped usage: aeadParam malloc failed!")

    aeadParam->aad = *aad;
    aeadParam->nonce = *nonce;
    aeadParam->payloadLen = payloadLen;
    aeadParam->tagDec = *aeadTag;

    usageSpec->algParam = aeadParam;
    return HKS_SUCCESS;
}

static int32_t CheckWrappingKeyIsUsedForUnwrap(const struct HksBlob *wrappingKey)
{
    struct HksKeyNode *wrappingKeyNode = HksGenerateKeyNode(wrappingKey);
    HKS_IF_NULL_LOGE_RETURN(wrappingKeyNode, HKS_ERROR_CORRUPT_FILE,
        "check agree params: generate wrapping keynode failed!")

    struct HksParam *purposeParamWrappingKey = NULL;
    int32_t ret = HksGetParam(wrappingKeyNode->paramSet, HKS_TAG_PURPOSE, &purposeParamWrappingKey);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get wrapping key param 0x%" LOG_PUBLIC "x failed!", HKS_TAG_PURPOSE);
        HksFreeKeyNode(&wrappingKeyNode);
        return HKS_ERROR_CHECK_GET_PURPOSE_FAIL;
    }

    if (purposeParamWrappingKey->uint32Param != HKS_KEY_PURPOSE_UNWRAP) {
        HKS_LOG_E("wrapping key is not used for unwrap!");
        HksFreeKeyNode(&wrappingKeyNode);
        return HKS_ERROR_INVALID_USAGE_OF_KEY;
    }
    HksFreeKeyNode(&wrappingKeyNode);
    return HKS_SUCCESS;
}

static int32_t GetPublicKeyInnerFormat(const struct HksBlob *wrappingKey, const struct HksBlob *wrappedKeyData,
    struct HksBlob *outPublicKey, uint32_t *partOffset)
{
    struct HksBlob peerPubKeyPart = { 0, NULL };
    uint32_t offset = *partOffset;
    int32_t ret = HksGetBlobFromWrappedData(wrappedKeyData, offset++, HKS_IMPORT_WRAPPED_KEY_TOTAL_BLOBS,
                                            &peerPubKeyPart);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "get peer pub key failed!")

    /* peer public key format should be same as wrapping key */
    struct HksKeyNode *wrappingKeyNode = HksGenerateKeyNode(wrappingKey);
    HKS_IF_NULL_LOGE_RETURN(wrappingKeyNode, HKS_ERROR_CORRUPT_FILE, "generate wrapping key keynode failed")

    ret = GetHksPubKeyInnerFormat(wrappingKeyNode->paramSet, &peerPubKeyPart, outPublicKey);
    HksFreeKeyNode(&wrappingKeyNode);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "get peer pub key inner format failed!")

    *partOffset  = offset;
    return HKS_SUCCESS;
}

static int32_t AgreeSharedSecretWithPeerPublicKey(const struct HksBlob *wrappingKey, const struct HksBlob *publicKey,
    uint32_t unwrapSuite, struct HksBlob *agreeSharedSecret, const struct HksParamSet *inParamSet)
{
    int32_t ret = CheckWrappingKeyIsUsedForUnwrap(wrappingKey);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "check agree params failed!")

    struct HksParamSet *agreeKeyParamSet = NULL;
    ret = GenAgreeKeyParamSetFromUnwrapSuite(unwrapSuite, inParamSet, &agreeKeyParamSet);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "gen agree key paramSet failed!")

    struct HksBlob sharedSecret = { 0, NULL };
    sharedSecret.size = HKS_KEY_BYTES(HKS_AES_KEY_SIZE_256);
    uint8_t *shareSecretBuffer = (uint8_t *) HksMalloc(sharedSecret.size);
    if (shareSecretBuffer == NULL) {
        HksFreeParamSet(&agreeKeyParamSet);
        HKS_LOG_E("malloc shared key failed!");
        return HKS_ERROR_MALLOC_FAIL;
    }
    sharedSecret.data = shareSecretBuffer;

    ret = HksCoreAgreeKey(agreeKeyParamSet, wrappingKey, publicKey, &sharedSecret);
    HksFreeParamSet(&agreeKeyParamSet);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("agree with peer public key failed! ret = %" LOG_PUBLIC "d", ret);
        HKS_FREE(sharedSecret.data);
        return ret;
    }

    agreeSharedSecret->size = sharedSecret.size;
    agreeSharedSecret->data = sharedSecret.data;
    return HKS_SUCCESS;
}

static int32_t ParseKekDecryptParams(const struct HksBlob *wrappedKeyData, uint32_t *partOffset,
    uint32_t totalBlobs, struct HksBlob **blobArray)
{
    uint32_t offset = *partOffset;
    uint32_t blobIndex = 0;
    int32_t ret = HksGetBlobFromWrappedData(wrappedKeyData, offset++, totalBlobs, blobArray[blobIndex++]);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "get agree-key aad data failed!")

    ret = HksGetBlobFromWrappedData(wrappedKeyData, offset++, totalBlobs, blobArray[blobIndex++]);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "get agree-key nonce data failed!")

    ret = HksGetBlobFromWrappedData(wrappedKeyData, offset++, totalBlobs, blobArray[blobIndex++]);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "get agree-key aead tag data failed!")

    ret = HksGetBlobFromWrappedData(wrappedKeyData, offset++, totalBlobs, blobArray[blobIndex++]);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "get kek enc data failed!")

    *partOffset = offset;
    return HKS_SUCCESS;
}

static int32_t DecryptKekWithAgreeSharedSecret(const struct HksBlob *wrappedKeyData,
    const struct HksBlob *agreeSharedSecret, uint32_t *partOffset, struct HksBlob *outKekBlob)
{
    struct HksBlob agreeKeyAadPart = { 0, NULL };
    struct HksBlob agreeKeyNoncePart = { 0, NULL };
    struct HksBlob agreeKeyTagPart = { 0, NULL };
    struct HksBlob kekEncDataPart = { 0, NULL };
    struct HksBlob *blobArray[] = { &agreeKeyAadPart, &agreeKeyNoncePart, &agreeKeyTagPart, &kekEncDataPart };

    uint32_t offset = *partOffset;
    int32_t ret = ParseKekDecryptParams(wrappedKeyData, &offset, HKS_IMPORT_WRAPPED_KEY_TOTAL_BLOBS, blobArray);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "parse agree-key decrypt kek params failed!")

    /* build decrypt kek usagespec */
    struct HksUsageSpec *decKekUsageSpec = (struct HksUsageSpec *)HksMalloc(sizeof(struct HksUsageSpec));
    HKS_IF_NULL_LOGE_RETURN(decKekUsageSpec, HKS_ERROR_MALLOC_FAIL, "malloc decrypt kek usage spec failed!")

    (void)memset_s(decKekUsageSpec, sizeof(struct HksUsageSpec), 0, sizeof(struct HksUsageSpec));
    ret = BuildDecryptUsageSpecOfUnwrap(&agreeKeyAadPart, &agreeKeyNoncePart, &agreeKeyTagPart, kekEncDataPart.size,
                                        decKekUsageSpec);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("build decrypt wrapped data kek usageSpec failed!");
        HksFreeUsageSpec(&decKekUsageSpec);
        return ret;
    }
    struct HksBlob kek = { 0, NULL };
    kek.size = HKS_KEY_BYTES(HKS_AES_KEY_SIZE_256);
    uint8_t *kekBuffer = (uint8_t *) HksMalloc(kek.size);
    if (kekBuffer == NULL) {
        HKS_LOG_E("malloc kek memory failed!");
        HksFreeUsageSpec(&decKekUsageSpec);
        return HKS_ERROR_MALLOC_FAIL;
    }
    kek.data = kekBuffer;
    ret = HksCryptoHalDecrypt(agreeSharedSecret, decKekUsageSpec, &kekEncDataPart, &kek);
    HksFreeUsageSpec(&decKekUsageSpec);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("decrypt kek data failed!");
        HKS_FREE(kek.data);
        return ret;
    }

    outKekBlob->size = kek.size;
    outKekBlob->data = kek.data;
    *partOffset = offset;
    return HKS_SUCCESS;
}

static int32_t ParseImportedKeyDecryptParams(const struct HksBlob *wrappedKeyData, uint32_t *partOffset,
    uint32_t totalBlobs, uint32_t *outKeyMaterialSize, struct HksBlob **blobArray)
{
    uint32_t offset = *partOffset;
    uint32_t blobIndex = 0;
    int32_t ret = HksGetBlobFromWrappedData(wrappedKeyData, offset++, totalBlobs, blobArray[blobIndex++]);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "get kek aad data failed!")

    ret = HksGetBlobFromWrappedData(wrappedKeyData, offset++, totalBlobs, blobArray[blobIndex++]);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "get kek nonce data failed!")

    ret = HksGetBlobFromWrappedData(wrappedKeyData, offset++, totalBlobs, blobArray[blobIndex++]);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "get kek aead tag data failed!")

    struct HksBlob keyMatLenBlobPart = { 0, NULL };
    ret = HksGetBlobFromWrappedData(wrappedKeyData, offset++, totalBlobs, &keyMatLenBlobPart);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "get key material len failed!")

    if (keyMatLenBlobPart.size != sizeof(uint32_t)) {
        HKS_LOG_E("key material len part is invalid!");
        return HKS_ERROR_INVALID_WRAPPED_FORMAT;
    }

    uint32_t keyMaterialSize = 0;
    (void)memcpy_s((uint8_t *)&keyMaterialSize, sizeof(uint32_t), keyMatLenBlobPart.data, keyMatLenBlobPart.size);
    if ((keyMaterialSize == 0) || (keyMaterialSize > MAX_KEY_SIZE)) {
        HKS_LOG_E("key material size is invalid!");
        return HKS_ERROR_INVALID_WRAPPED_FORMAT;
    }

    ret = HksGetBlobFromWrappedData(wrappedKeyData, offset++, totalBlobs, blobArray[blobIndex++]);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "get imported key encryption data failed!")

    *partOffset = offset;
    *outKeyMaterialSize = keyMaterialSize;
    return HKS_SUCCESS;
}

static int32_t DecryptImportedKeyWithKek(const struct HksBlob *wrappedKeyData, const struct HksBlob *kek,
    uint32_t *partOffset, struct HksBlob *outImportedKey)
{
    struct HksBlob kekAadPart = { 0, NULL };
    struct HksBlob kekNoncePart = { 0, NULL };
    struct HksBlob kekTagPart = { 0, NULL };
    struct HksBlob originKeyEncDataPart = { 0, NULL };
    struct HksBlob *blobArray[] = { &kekAadPart, &kekNoncePart, &kekTagPart, &originKeyEncDataPart };

    uint32_t offset = *partOffset;
    uint32_t keyMaterialSize = 0;
    int32_t ret = ParseImportedKeyDecryptParams(wrappedKeyData, &offset, HKS_IMPORT_WRAPPED_KEY_TOTAL_BLOBS,
                                                &keyMaterialSize, blobArray);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "parse kek decrypt imported-key params failed!")

    struct HksBlob originKey = { 0, NULL };
    originKey.size = keyMaterialSize;
    uint8_t *originKeyBuffer = (uint8_t *) HksMalloc(originKey.size);
    HKS_IF_NULL_LOGE_RETURN(originKeyBuffer, HKS_ERROR_MALLOC_FAIL, "malloc originKeyBuffer memory failed!")

    originKey.data = originKeyBuffer;

    struct HksUsageSpec *decOriginKeyUsageSpec = (struct HksUsageSpec *)HksMalloc(sizeof(struct HksUsageSpec));
    if (decOriginKeyUsageSpec == NULL) {
        HKS_LOG_E("malloc originKeyBuffer memory failed!");
        HKS_FREE(originKey.data);
        return HKS_ERROR_MALLOC_FAIL;
    }
    (void)memset_s(decOriginKeyUsageSpec, sizeof(struct HksUsageSpec), 0, sizeof(struct HksUsageSpec));
    uint32_t payloadSize = originKeyEncDataPart.size;
    ret = BuildDecryptUsageSpecOfUnwrap(&kekAadPart, &kekNoncePart, &kekTagPart, payloadSize, decOriginKeyUsageSpec);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("build decrypt wrapped data origin key usageSpec failed!");
        HKS_FREE(originKey.data);
        HksFreeUsageSpec(&decOriginKeyUsageSpec);
        return ret;
    }

    ret = HksCryptoHalDecrypt(kek, decOriginKeyUsageSpec, &originKeyEncDataPart, &originKey);
    HksFreeUsageSpec(&decOriginKeyUsageSpec);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("decrypt importKey failed!");
        HKS_FREE(originKey.data);
        return ret;
    }

    outImportedKey->size = originKey.size;
    outImportedKey->data = originKey.data;
    *partOffset = offset;
    return HKS_SUCCESS;
}

static void ClearAndFreeKeyBlobsIfNeed(struct HksBlob *peerPublicKey, struct HksBlob *agreeSharedSecret,
    struct HksBlob *originKey, struct HksBlob *kek)
{
    if (originKey->data != NULL) {
        (void)memset_s(originKey->data, originKey->size, 0, originKey->size);
        HKS_FREE(originKey->data);
    }

    if (kek->data != NULL) {
        (void)memset_s(kek->data, kek->size, 0, kek->size);
        HKS_FREE(kek->data);
    }

    if (agreeSharedSecret->data != NULL) {
        (void)memset_s(agreeSharedSecret->data, agreeSharedSecret->size, 0, agreeSharedSecret->size);
        HKS_FREE(agreeSharedSecret->data);
    }

    if (peerPublicKey->data != NULL) {
        (void)memset_s(peerPublicKey->data, peerPublicKey->size, 0, peerPublicKey->size);
        HKS_FREE(peerPublicKey->data);
    }
}

static int32_t CheckRsaKeyMaterialLen(uint32_t keyType, const struct HksBlob *key)
{
    if (key->size < sizeof(struct HksKeyMaterialRsa)) {
        HKS_LOG_E("invalid import key size: %" LOG_PUBLIC "u", key->size);
        return HKS_ERROR_INVALID_KEY_INFO;
    }

    struct HksKeyMaterialRsa *keyMaterial = (struct HksKeyMaterialRsa *)(key->data);

    if ((keyMaterial->nSize > HKS_RSA_KEY_SIZE_4096) || (keyMaterial->nSize == 0) ||
        (keyMaterial->dSize > HKS_RSA_KEY_SIZE_4096) || (keyMaterial->dSize == 0) ||
        (keyMaterial->eSize > HKS_RSA_KEY_SIZE_4096)) {
        HKS_LOG_E("invalid import key material n/d/e size");
        return HKS_ERROR_INVALID_KEY_INFO;
    }

    if ((keyType == HKS_KEY_TYPE_KEY_PAIR) && (keyMaterial->eSize == 0)) {
        HKS_LOG_E("invalid import key material e size while import key pair");
        return HKS_ERROR_INVALID_KEY_INFO;
    }

    uint32_t keySize = sizeof(struct HksKeyMaterialRsa) + keyMaterial->nSize + keyMaterial->dSize + keyMaterial->eSize;
    if (key->size < keySize) {
        HKS_LOG_E("import key size[%" LOG_PUBLIC "u] smaller than keySize[%" LOG_PUBLIC "u]", key->size, keySize);
        return HKS_ERROR_INVALID_KEY_INFO;
    }

    return HKS_SUCCESS;
}

static int32_t AppendRsaPublicExponent(const struct HksBlob *key, struct HksBlob *outKey)
{
    /* key len has been checked by caller */
    struct HksKeyMaterialRsa *keyMaterial = (struct HksKeyMaterialRsa *)(key->data);
    uint32_t size = sizeof(struct HksKeyMaterialRsa) + keyMaterial->nSize + keyMaterial->dSize +
        sizeof(g_defaultRsaPubExponent);

    uint8_t *out = (uint8_t *)HksMalloc(size);
    HKS_IF_NULL_LOGE_RETURN(out, HKS_ERROR_MALLOC_FAIL, "malloc failed")

    uint32_t offset = 0;
    do {
        (void)memcpy_s(out + offset, size - offset, key->data, sizeof(struct HksKeyMaterialRsa));
        offset += sizeof(struct HksKeyMaterialRsa);

        struct HksKeyMaterialRsa *newkeyMaterial = (struct HksKeyMaterialRsa *)out;
        newkeyMaterial->eSize = sizeof(g_defaultRsaPubExponent);

        (void)memcpy_s(out + offset, size - offset, key->data + offset, keyMaterial->nSize);
        offset += keyMaterial->nSize;

        (void)memcpy_s(out + offset, size - offset, g_defaultRsaPubExponent, sizeof(g_defaultRsaPubExponent));

        (void)memcpy_s(out + offset + sizeof(g_defaultRsaPubExponent), size - offset - sizeof(g_defaultRsaPubExponent),
            key->data + offset, keyMaterial->dSize);
    } while (0);

    outKey->data = out;
    outKey->size = size;
    return HKS_SUCCESS;
}

static int32_t GetRsaPrivateOrPairInnerFormat(uint32_t keyType, const struct HksBlob *key, struct HksBlob *outKey)
{
    int32_t ret = CheckRsaKeyMaterialLen(keyType, key);
    HKS_IF_NOT_SUCC_RETURN(ret, ret)

    struct HksKeyMaterialRsa *keyMaterial = (struct HksKeyMaterialRsa *)(key->data);
    if ((keyType == HKS_KEY_TYPE_PRIVATE_KEY) && (keyMaterial->eSize == 0)) {
        return AppendRsaPublicExponent(key, outKey);
    }

    return CopyToInnerKey(key, outKey);
}

static int32_t GetCurve25519PrivateOrPairInnerFormat(uint8_t alg, uint32_t keyType,
    const struct HksBlob *key, struct HksBlob *outKey)
{
    if (keyType == HKS_KEY_TYPE_KEY_PAIR) {
        return CopyToInnerKey(key, outKey);
    }

    if (key->size != HKS_KEY_BYTES(HKS_CURVE25519_KEY_SIZE_256)) {
        HKS_LOG_E("Invalid curve25519 private key size! key size = 0x%" LOG_PUBLIC "X", key->size);
        return HKS_ERROR_INVALID_KEY_INFO;
    }

    uint32_t totalSize = sizeof(struct HksKeyMaterial25519) + key->size;
    uint8_t *buffer = (uint8_t *)HksMalloc(totalSize);
    HKS_IF_NULL_LOGE_RETURN(buffer, HKS_ERROR_MALLOC_FAIL, "malloc failed! %" LOG_PUBLIC "u", totalSize)

    (void)memset_s(buffer, totalSize, 0, totalSize);

    struct HksKeyMaterial25519 *curve25519Key = (struct HksKeyMaterial25519 *)buffer;
    curve25519Key->keyAlg = (enum HksKeyAlg)alg;
    curve25519Key->keySize = HKS_CURVE25519_KEY_SIZE_256;
    curve25519Key->priKeySize = key->size; /* curve25519 private key */

    uint32_t offset = sizeof(struct HksKeyMaterial25519);
    (void)memcpy_s(buffer + offset, totalSize - offset, key->data, key->size);
    outKey->data = buffer;
    outKey->size = totalSize;
    return HKS_SUCCESS;
}

static int32_t GetPrivateOrPairInnerFormat(uint32_t keyType, const struct HksBlob *key,
    const struct HksParamSet *paramSet, struct HksBlob *outKey)
{
    HKS_IF_NOT_SUCC_LOGE_RETURN(CheckBlob(key), HKS_ERROR_INVALID_ARGUMENT, "invalid key or outKey")

    struct HksParam *algParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_ALGORITHM, &algParam);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, HKS_ERROR_CHECK_GET_ALG_FAIL, "get alg param failed")

    switch (algParam->uint32Param) {
        case HKS_ALG_RSA:
            return GetRsaPrivateOrPairInnerFormat(keyType, key, outKey);
        case HKS_ALG_ECC:
        case HKS_ALG_DSA:
        case HKS_ALG_DH:
        case HKS_ALG_SM2:
        case HKS_ALG_HMAC:
        case HKS_ALG_SM3:
        case HKS_ALG_SM4:
        case HKS_ALG_AES:
            return CopyToInnerKey(key, outKey);
        case HKS_ALG_ED25519:
        case HKS_ALG_X25519:
            return GetCurve25519PrivateOrPairInnerFormat(algParam->uint32Param, keyType, key, outKey);
        default:
            return HKS_ERROR_INVALID_ALGORITHM;
    }
}

int32_t HksCoreImportKey(const struct HksBlob *keyAlias, const struct HksBlob *key,
    const struct HksParamSet *paramSet, struct HksBlob *keyOut)
{
    struct HksBlob innerKey = { 0, NULL };
    struct HksParam *importKeyTypeParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_IMPORT_KEY_TYPE, &importKeyTypeParam);
    if ((ret == HKS_SUCCESS) &&
        ((importKeyTypeParam->uint32Param == HKS_KEY_TYPE_PRIVATE_KEY) ||
        (importKeyTypeParam->uint32Param == HKS_KEY_TYPE_KEY_PAIR))) {
        ret = GetPrivateOrPairInnerFormat(importKeyTypeParam->uint32Param, key, paramSet, &innerKey);
    } else {
        ret = CopyToInnerKey(key, &innerKey);
    }
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "translate key to inner format failed, ret = %" LOG_PUBLIC "d", ret)

    do {
        ret = HksCoreCheckImportKeyParams(keyAlias, &innerKey, paramSet, keyOut);
        HKS_IF_NOT_SUCC_BREAK(ret)

        ret = HksBuildKeyBlob(keyAlias, HKS_KEY_FLAG_IMPORT_KEY, &innerKey, paramSet, keyOut);
    } while (0);

    (void)memset_s(innerKey.data, innerKey.size, 0, innerKey.size);
    HKS_FREE_BLOB(innerKey);
    return ret;
}

int32_t HksCoreImportWrappedKey(const struct HksBlob *keyAlias, const struct HksBlob *wrappingKey,
    const struct HksBlob *wrappedKeyData, const struct HksParamSet *paramSet, struct HksBlob *keyOut)
{
    uint32_t unwrapSuite = 0;
    int32_t ret = HksCoreCheckImportWrappedKeyParams(wrappingKey, wrappedKeyData, paramSet, keyOut, &unwrapSuite);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "check import wrapped key params failed!")

    if ((unwrapSuite == HKS_UNWRAP_SUITE_SM2_SM4_128_CBC_PKCS7_WITH_VERIFY_DIG_SM3) ||
        (unwrapSuite == HKS_UNWRAP_SUITE_SM2_SM4_128_CBC_PKCS7)) {
        return HksSmImportWrappedKey(keyAlias, paramSet, wrappingKey, wrappedKeyData, keyOut);
    }

    struct HksBlob peerPublicKey = { 0, NULL };
    struct HksBlob agreeSharedSecret = { 0, NULL };
    struct HksBlob originKey = { 0, NULL };
    struct HksBlob kek = { 0, NULL };
    uint32_t partOffset = 0;

    do {
        /* 1. get peer public key and translate to inner format */
        ret = GetPublicKeyInnerFormat(wrappingKey, wrappedKeyData, &peerPublicKey, &partOffset);
        HKS_IF_NOT_SUCC_LOGE_BREAK(ret, "get peer public key of inner format failed!")

        /* 2. agree shared key with wrappingAlias's private key and peer public key */
        ret = AgreeSharedSecretWithPeerPublicKey(wrappingKey, &peerPublicKey, unwrapSuite, &agreeSharedSecret,
            paramSet);
        HKS_IF_NOT_SUCC_LOGE_BREAK(ret, "agree share secret failed!")

        /* 4. decrypt kek data with agreed secret */
        ret = DecryptKekWithAgreeSharedSecret(wrappedKeyData, &agreeSharedSecret, &partOffset, &kek);
        HKS_IF_NOT_SUCC_LOGE_BREAK(ret, "decrypt kek with agreed secret failed!")

        /* 5. decrypt imported key data with kek */
        ret = DecryptImportedKeyWithKek(wrappedKeyData, &kek, &partOffset, &originKey);
        HKS_IF_NOT_SUCC_LOGE_BREAK(ret, "decrypt origin key failed!")

        /* 6. call HksCoreImportKey to build key blob */
        ret = HksCoreImportKey(keyAlias, &originKey, paramSet, keyOut);
        HKS_IF_NOT_SUCC_LOGE_BREAK(ret, "import origin key failed!")
    } while (0);

    ClearAndFreeKeyBlobsIfNeed(&peerPublicKey, &agreeSharedSecret, &originKey, &kek);
    return ret;
}

int32_t HksCoreGenerateRandom(const struct HksParamSet *paramSet, struct HksBlob *random)
{
    (void)paramSet;
    return HksCryptoHalFillRandom(random);
}