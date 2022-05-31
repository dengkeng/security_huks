/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "hks_core_service.h"

#include "hks_ability.h"
#include "hks_attest.h"
#include "hks_auth.h"
#include "hks_check_paramset.h"
#include "hks_cmd_id.h"
#include "hks_crypto_adapter.h"
#include "hks_crypto_hal.h"
#include "hks_keyblob.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_param.h"
#include "hks_type_inner.h"

#ifndef _HARDWARE_ROOT_KEY_
#include "hks_rkc.h"
#endif

#ifdef HKS_SUPPORT_UPGRADE_STORAGE_DATA
#include "hks_upgrade_key_info.h"
#endif

#ifndef _CUT_AUTHENTICATE_
#define CURVE25519_KEY_BYTE_SIZE HKS_KEY_BYTES(HKS_CURVE25519_KEY_SIZE_256)

static HksMutex *g_huksMutex = NULL;  /* global mutex using in keynode */

static struct HksCoreInitHandler g_hksCoreInitHandler[] = {
    { HKS_KEY_PURPOSE_SIGN, HksCoreSignVerifyThreeStageInit },
    { HKS_KEY_PURPOSE_VERIFY, HksCoreSignVerifyThreeStageInit },
    { HKS_KEY_PURPOSE_ENCRYPT, HksCoreCryptoThreeStageInit },
    { HKS_KEY_PURPOSE_DECRYPT, HksCoreCryptoThreeStageInit },
    { HKS_KEY_PURPOSE_DERIVE, HksCoreDeriveThreeStageInit },
    { HKS_KEY_PURPOSE_AGREE, HksCoreAgreeThreeStageInit },
    { HKS_KEY_PURPOSE_MAC, HksCoreMacThreeStageInit }
};

static struct HksCoreUpdateHandler g_hksCoreUpdateHandler[] = {
    { HKS_KEY_PURPOSE_SIGN, HksCoreSignVerifyThreeStageUpdate },
    { HKS_KEY_PURPOSE_VERIFY, HksCoreSignVerifyThreeStageUpdate },
    { HKS_KEY_PURPOSE_ENCRYPT, HksCoreCryptoThreeStageUpdate },
    { HKS_KEY_PURPOSE_DECRYPT, HksCoreCryptoThreeStageUpdate },
    { HKS_KEY_PURPOSE_DERIVE, HksCoreDeriveThreeStageUpdate },
    { HKS_KEY_PURPOSE_AGREE, HksCoreAgreeThreeStageUpdate },
    { HKS_KEY_PURPOSE_MAC, HksCoreMacThreeStageUpdate }
};

static struct HksCoreFinishHandler g_hksCoreFinishHandler[] = {
    { HKS_KEY_PURPOSE_SIGN, HksCoreSignVerifyThreeStageFinish },
    { HKS_KEY_PURPOSE_VERIFY, HksCoreSignVerifyThreeStageFinish },
    { HKS_KEY_PURPOSE_ENCRYPT, HksCoreEncryptThreeStageFinish },
    { HKS_KEY_PURPOSE_DECRYPT, HksCoreDecryptThreeStageFinish },
    { HKS_KEY_PURPOSE_DERIVE, HksCoreDeriveThreeStageFinish },
    { HKS_KEY_PURPOSE_AGREE, HksCoreAgreeThreeStageFinish },
    { HKS_KEY_PURPOSE_MAC, HksCoreMacThreeStageFinish }
};

static struct HksCoreAbortHandler g_hksCoreAbortHandler[] = {
    { HKS_KEY_PURPOSE_SIGN, HksCoreSignVerifyThreeStageAbort },
    { HKS_KEY_PURPOSE_VERIFY, HksCoreSignVerifyThreeStageAbort },
    { HKS_KEY_PURPOSE_ENCRYPT, HksCoreCryptoThreeStageAbort },
    { HKS_KEY_PURPOSE_DECRYPT, HksCoreCryptoThreeStageAbort },
    { HKS_KEY_PURPOSE_DERIVE, HksCoreDeriveThreeStageAbort },
    { HKS_KEY_PURPOSE_AGREE, HksCoreAgreeThreeStageAbort },
    { HKS_KEY_PURPOSE_MAC, HksCoreMacThreeStageAbort }
};

static int32_t GetGenType(const struct HksParamSet *paramSet, uint32_t *genType)
{
    struct HksParam *keyGenTypeParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_KEY_GENERATE_TYPE, &keyGenTypeParam);
    if (ret != HKS_SUCCESS) {
        return HKS_SUCCESS; /* if not set KEY_GENERATE_TYPE, gen key by default type */
    }

    struct HksParam *keyAlgParam = NULL;
    ret = HksGetParam(paramSet, HKS_TAG_ALGORITHM, &keyAlgParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get alg tag fail");
        return HKS_ERROR_CHECK_GET_ALG_FAIL;
    }

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
int32_t CheckAgreeKeyIn(const struct HksBlob *key)
{
    if (CheckBlob(key) != HKS_SUCCESS) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

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

int32_t GetAgreeBaseKey(const bool isPubKey, const bool isPlainPubKey, const struct HksBlob *keyIn,
    struct HksBlob *keyOut)
{
    struct Hks25519KeyPair *keyPair = (struct Hks25519KeyPair *)(keyIn->data);
    uint32_t size = isPubKey ? keyPair->publicBufferSize : keyPair->privateBufferSize;
    uint8_t *buffer = (uint8_t *)HksMalloc(size);
    if (buffer == NULL) {
        HKS_LOG_E("malloc failed");
        return HKS_ERROR_MALLOC_FAIL;
    }

    uint8_t *tmp = isPubKey ? (keyIn->data + sizeof(*keyPair)) :
        (keyIn->data + sizeof(*keyPair) + keyPair->publicBufferSize);
    if (memcpy_s(buffer, size, tmp, size) != EOK) {
        HKS_LOG_E("memcpy failed");
        HKS_FREE_PTR(buffer);
        return HKS_ERROR_BAD_STATE;
    }

    if (isPlainPubKey) { /* public key is plain key, only copy */
        keyOut->data = buffer;
        keyOut->size = size;
        return HKS_SUCCESS;
    }

    struct HksBlob tempKey = { size, buffer };
    struct HksKeyNode *keyNode = HksGenerateKeyNode(&tempKey);
    HKS_FREE_PTR(buffer);
    if (keyNode == NULL) {
        HKS_LOG_E("generating keynode with agree key failed");
        return HKS_ERROR_BAD_STATE;
    }

    int32_t ret = HksGetRawKey(keyNode->paramSet, keyOut);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get raw key during key agreement failed!");
    }
    HksFreeKeyNode(&keyNode);
    return ret;
}

int32_t GetAgreePriKey(const struct HksBlob *keyIn, struct HksBlob *keyOut)
{
    return GetAgreeBaseKey(false, false, keyIn, keyOut);
}

int32_t GetAgreePubKey(const struct HksBlob *keyIn, const struct HksParamSet *paramSet, struct HksBlob *keyOut)
{
    struct HksParam *isKeyAliasParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_AGREE_PUBLIC_KEY_IS_KEY_ALIAS, &isKeyAliasParam);
    if ((ret == HKS_SUCCESS) && (!(isKeyAliasParam->boolParam))) {
        return GetAgreeBaseKey(true, true, keyIn, keyOut);
    }

    return GetAgreeBaseKey(true, false, keyIn, keyOut);
}

int32_t GenAgreeKey(const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, struct HksBlob *agreedKey)
{
    struct HksParam *agreeAlgParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_AGREE_ALG, &agreeAlgParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("generate key not set agree alg");
        return ret;
    }

    agreedKey->size = HKS_KEY_BYTES(HKS_AES_KEY_SIZE_256);
    agreedKey->data = (uint8_t *)HksMalloc(agreedKey->size);
    if (agreedKey->data == NULL) {
        HKS_LOG_E("malloc failed");
        return HKS_ERROR_MALLOC_FAIL;
    }

    struct HksKeySpec agreeSpec = { 0, HKS_CURVE25519_KEY_SIZE_256, NULL };
    if (agreeAlgParam->uint32Param == HKS_ALG_X25519) {
        agreeSpec.algType = HKS_ALG_X25519;
    } else if (agreeAlgParam->uint32Param == HKS_ALG_ED25519) {
        agreeSpec.algType = HKS_ALG_ED25519;
    } else {
        HKS_FREE_PTR(agreedKey->data);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    ret = HksCryptoHalAgreeKey(privateKey, peerPublicKey, &agreeSpec, agreedKey);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("agree key failed, ret = %d", ret);
        HKS_FREE_PTR(agreedKey->data); /* X25519AgreeKey will memset sharedKey if fail */
    }
    return ret;
}

static int32_t GenKeyByAgree(const struct HksBlob *keyIn, const struct HksParamSet *paramSet,
    struct HksBlob *sharedKey)
{
    int32_t ret = CheckAgreeKeyIn(keyIn);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    struct HksBlob priKey = { 0, NULL };
    struct HksBlob pubKey = { 0, NULL };
    do {
        ret = GetAgreePriKey(keyIn, &priKey);
        if (ret != HKS_SUCCESS) {
            break;
        }

        ret = GetAgreePubKey(keyIn, paramSet, &pubKey);
        if (ret != HKS_SUCCESS) {
            break;
        }

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
    int32_t ret = HksCoreCheckGenKeyParams(keyAlias, paramSet, keyIn, keyOut);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("hks core check generate key params failed, ret:%x!", ret);
        return ret;
    }

    uint32_t genType = HKS_KEY_GENERATE_TYPE_DEFAULT;
    ret = GetGenType(paramSet, &genType);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

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
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("GenerateKey failed, ret:%x!", ret);
        return ret;
    }

    ret = HksBuildKeyBlob(keyAlias, HKS_KEY_FLAG_GENERATE_KEY, &key, paramSet, keyOut);
    (void)memset_s(key.data, key.size, 0, key.size);
    HKS_FREE_BLOB(key);
    return ret;
}

static int32_t CipherAuth(const struct HksKeyNode *keyNode, const struct HksParamSet *paramSet)
{
    struct HksParam *algParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_ALGORITHM, &algParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("append cipher get alg param failed!");
        return ret;
    }

    if (algParam->uint32Param == HKS_ALG_AES) {
        return HksAuth(HKS_AUTH_ID_SYM_CIPHER, keyNode, paramSet);
    } else if (algParam->uint32Param == HKS_ALG_RSA) {
        return HksAuth(HKS_AUTH_ID_ASYM_CIPHER, keyNode, paramSet);
    } else {
        return HKS_ERROR_INVALID_ALGORITHM;
    }
}

static int32_t SignVerifyAuth(const struct HksKeyNode *keyNode, const struct HksParamSet *paramSet)
{
    struct HksParam *algParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_ALGORITHM, &algParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("append cipher get alg param failed!");
        return ret;
    }

    if (algParam->uint32Param == HKS_ALG_RSA) {
        return HksAuth(HKS_AUTH_ID_SIGN_VERIFY_RSA, keyNode, paramSet);
    } else if (algParam->uint32Param == HKS_ALG_ECC) {
        return HksAuth(HKS_AUTH_ID_SIGN_VERIFY_ECC, keyNode, paramSet);
    } else if (algParam->uint32Param == HKS_ALG_DSA) {
        return HKS_SUCCESS;
    } else if (algParam->uint32Param == HKS_ALG_ED25519) {
        return HKS_SUCCESS;
    } else {
        return HKS_ERROR_INVALID_ALGORITHM;
    }
}

static int32_t GetSignVerifyMessage(const struct HksParamSet *paramSet, const struct HksBlob *srcData,
    struct HksBlob *message, bool *needFree)
{
    struct HksParam *algParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_ALGORITHM, &algParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get param get 0x%x failed", HKS_TAG_ALGORITHM);
        return HKS_ERROR_CHECK_GET_ALG_FAIL;
    }

    if (algParam->uint32Param != HKS_ALG_ED25519) {
        struct HksParam *digestParam = NULL;
        ret = HksGetParam(paramSet, HKS_TAG_DIGEST, &digestParam);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("SignVerify get digestParam failed!");
            return HKS_ERROR_CHECK_GET_DIGEST_FAIL;
        }

        message->size = MAX_HASH_SIZE;
        message->data = (uint8_t *)HksMalloc(MAX_HASH_SIZE);
        if (message->data == NULL) {
            HKS_LOG_E("SignVerify malloc message data failed!");
            return HKS_ERROR_MALLOC_FAIL;
        }

        /* NONEwithECDSA/RSA default sha256 */
        uint32_t digest = (digestParam->uint32Param == HKS_DIGEST_NONE) ? HKS_DIGEST_SHA256 : digestParam->uint32Param;
        ret = HksCryptoHalHash(digest, srcData, message);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("SignVerify calc hash failed!");
            HKS_FREE_PTR(message->data);
            return ret;
        }

        *needFree = true;
    } else {
        message->size = srcData->size;
        message->data = srcData->data;
        *needFree = false;
    }

    return HKS_SUCCESS;
}

static int32_t SignVerify(uint32_t cmdId, const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *signature)
{
    int32_t ret = HksCoreCheckSignVerifyParams(cmdId, key, paramSet, srcData, signature);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("hks failed to check signature or verify params, cmdId:%x, ret:%x!\n", cmdId, ret);
        return ret;
    }

    struct HksKeyNode *keyNode = HksGenerateKeyNode(key);
    if (keyNode == NULL) {
        HKS_LOG_E("SignVerify generate keynode failed");
        return HKS_ERROR_BAD_STATE;
    }

    bool needFree = true;
    struct HksBlob message = { 0, NULL };
    do {
        ret = SignVerifyAuth(keyNode, paramSet);
        if (ret != HKS_SUCCESS) {
            break;
        }

        ret = GetSignVerifyMessage(keyNode->paramSet, srcData, &message, &needFree);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("SignVerify calc hash failed!");
            break;
        }

        struct HksBlob rawKey = { 0, NULL };
        ret = HksGetRawKey(keyNode->paramSet, &rawKey);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("SignVerify get raw key failed!");
            break;
        }

        struct HksUsageSpec usageSpec = {0};
        HksFillUsageSpec(paramSet, &usageSpec);
        /* NONEwithECDSA/RSA default sha256 */
        usageSpec.digest = (usageSpec.digest == HKS_DIGEST_NONE) ? HKS_DIGEST_SHA256 : usageSpec.digest;
        if (cmdId == HKS_CMD_ID_SIGN) {
            ret = HksCryptoHalSign(&rawKey, &usageSpec, &message, signature);
        } else {
            ret = HksCryptoHalVerify(&rawKey, &usageSpec, &message, signature);
        }
        (void)memset_s(rawKey.data, rawKey.size, 0, rawKey.size);
        HKS_FREE_PTR(rawKey.data);
    }while (0);

    HksFreeKeyNode(&keyNode);
    if (needFree) {
        HKS_FREE_PTR(message.data);
    }
    return ret;
}

static int32_t CipherEncrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksUsageSpec *usageSpec, const struct HksBlob *inData, struct HksBlob *outData)
{
    struct HksBlob tag = { 0, NULL };
    int32_t ret = HksGetEncryptAeTag(paramSet, inData, outData, &tag);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("cipher encrypt get ae tag failed!");
        return ret;
    }

    ret = HksCryptoHalEncrypt(key, usageSpec, inData, outData, &tag);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("cipher encrypt failed!");
        return ret;
    }

    outData->size += tag.size;
    return HKS_SUCCESS;
}

static int32_t Cipher(uint32_t cmdId, const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData)
{
    int32_t ret = HksCoreCheckCipherParams(cmdId, key, paramSet, inData, outData);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("hks core check cipher params failed, cmdId:%x, ret:%x!\n", cmdId, ret);
        return ret;
    }

    struct HksKeyNode *keyNode = HksGenerateKeyNode(key);
    if (keyNode == NULL) {
        HKS_LOG_E("Cipher generate keynode failed");
        return HKS_ERROR_BAD_STATE;
    }

    do {
        ret = CipherAuth(keyNode, paramSet);
        if (ret != HKS_SUCCESS) {
            break;
        }

        struct HksBlob rawKey = { 0, NULL };
        ret = HksGetRawKey(keyNode->paramSet, &rawKey);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("cipher get raw key failed!");
            break;
        }

        struct HksUsageSpec *usageSpec = NULL;
        bool isEncrypt = (cmdId == HKS_CMD_ID_ENCRYPT);
        struct HksBlob tmpInData = { inData->size, inData->data };
        ret = HksBuildCipherUsageSpec(paramSet, isEncrypt, &tmpInData, &usageSpec);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("build cipher usageSpec failed!");
            (void)memset_s(rawKey.data, rawKey.size, 0, rawKey.size);
            HKS_FREE_PTR(rawKey.data);
            break;
        }

        if (cmdId == HKS_CMD_ID_ENCRYPT) {
            ret = CipherEncrypt(&rawKey, paramSet, usageSpec, &tmpInData, outData);
        } else {
            ret = HksCryptoHalDecrypt(&rawKey, usageSpec, &tmpInData, outData);
        }
        (void)memset_s(rawKey.data, rawKey.size, 0, rawKey.size);
        HKS_FREE_PTR(rawKey.data);

        HksFreeUsageSpec(&usageSpec);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("cipher[%x] failed!", cmdId);
        }
    }while (0);

    HksFreeKeyNode(&keyNode);
    return ret;
}

int32_t HksCoreSign(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *signature)
{
    return SignVerify(HKS_CMD_ID_SIGN, key, paramSet, srcData, signature);
}

int32_t HksCoreVerify(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, const struct HksBlob *signature)
{
    return SignVerify(HKS_CMD_ID_VERIFY, key, paramSet, srcData, (struct HksBlob *)signature);
}

int32_t HksCoreEncrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *plainText, struct HksBlob *cipherText)
{
    return Cipher(HKS_CMD_ID_ENCRYPT, key, paramSet, plainText, cipherText);
}

int32_t HksCoreDecrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *cipherText, struct HksBlob *plainText)
{
    return Cipher(HKS_CMD_ID_DECRYPT, key, paramSet, cipherText, plainText);
}

int32_t HksCheckKeyValidity(const struct HksParamSet *paramSet, const struct HksBlob *key)
{
    (void)paramSet;

    struct HksKeyNode *keyNode = HksGenerateKeyNode(key);
    if (keyNode == NULL) {
        HKS_LOG_E("check key legality failed");
        return HKS_ERROR_BAD_STATE;
    }

    HksFreeKeyNode(&keyNode);
    return HKS_SUCCESS;
}

int32_t HksCoreImportKey(const struct HksBlob *keyAlias, const struct HksBlob *key,
    const struct HksParamSet *paramSet, struct HksBlob *keyOut)
{
    int32_t ret = HksCoreCheckImportKeyParams(keyAlias, key, paramSet, keyOut);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    return HksBuildKeyBlob(keyAlias, HKS_KEY_FLAG_IMPORT_KEY, key, paramSet, keyOut);
}

int32_t HksCoreExportPublicKey(const struct HksBlob *key, const struct HksParamSet *paramSet, struct HksBlob *keyOut)
{
    (void)paramSet;
    if (CheckBlob(key) != HKS_SUCCESS || CheckBlob(keyOut) != HKS_SUCCESS) {
        HKS_LOG_E("input param invalid");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    struct HksKeyNode *keyNode = HksGenerateKeyNode(key);
    if (keyNode == NULL) {
        HKS_LOG_E("SignVerify generate keynode failed");
        return HKS_ERROR_BAD_STATE;
    }

    struct HksBlob rawKey = { 0, NULL };
    int32_t ret = HksGetRawKey(keyNode->paramSet, &rawKey);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get raw key when exporting public key failed!");
        HksFreeKeyNode(&keyNode);
        return ret;
    }

    ret = HksCryptoHalGetPubKey(&rawKey, keyOut);
    (void)memset_s(rawKey.data, rawKey.size, 0, rawKey.size);
    HKS_FREE_PTR(rawKey.data);
    HksFreeKeyNode(&keyNode);
    return ret;
}

int32_t HksCoreAgreeKey(const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, struct HksBlob *agreedKey)
{
    int32_t ret = HksCoreCheckAgreeKeyParams(paramSet, privateKey, peerPublicKey, agreedKey, false);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check agreeKey params failed");
        return ret;
    }

    struct HksKeyNode *privateKeyNode = HksGenerateKeyNode(privateKey);
    if (privateKeyNode == NULL) {
        HKS_LOG_E("agree key generate keynode failed");
        return HKS_ERROR_BAD_STATE;
    }

    struct HksBlob key = { 0, NULL };
    ret = HksGetRawKey(privateKeyNode->paramSet, &key);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get raw key when agreeing key failed!");
        HksFreeKeyNode(&privateKeyNode);
        return ret;
    }

    struct HksKeySpec agreeSpec = {0};
    HksFillKeySpec(paramSet, &agreeSpec);
    ret = HksCryptoHalAgreeKey(&key, peerPublicKey, &agreeSpec, agreedKey);
    (void)memset_s(key.data, key.size, 0, key.size);
    HKS_FREE_PTR(key.data);
    HksFreeKeyNode(&privateKeyNode);
    return ret;
}

int32_t HksCoreDeriveKey(const struct HksParamSet *paramSet, const struct HksBlob *mainKey, struct HksBlob *derivedKey)
{
    int32_t ret = HksCoreCheckDeriveKeyParams(paramSet, mainKey, derivedKey, false);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check deriveKey params failed");
        return ret;
    }

    struct HksKeyNode *keyNode = HksGenerateKeyNode(mainKey);
    if (keyNode == NULL) {
        HKS_LOG_E("SignVerify generate keynode failed");
        return HKS_ERROR_BAD_STATE;
    }

    do {
        ret = HksAuth(HKS_AUTH_ID_DERIVE, keyNode, paramSet);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("derive auth failed!");
            break;
        }

        struct HksBlob key = { 0, NULL };
        ret = HksGetRawKey(keyNode->paramSet, &key);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("derive get raw key failed!");
            break;
        }

        struct HksKeyDerivationParam derParam = { { 0, NULL }, { 0, NULL }, 0, 0 };
        struct HksKeySpec derivationSpec = { 0, 0, &derParam };
        HksFillKeySpec(paramSet, &derivationSpec);
        HksFillKeyDerivationParam(paramSet, &derParam);
        ret = HksCryptoHalDeriveKey(&key, &derivationSpec, derivedKey);
        (void)memset_s(key.data, key.size, 0, key.size);
        HKS_FREE_PTR(key.data);
    } while (0);

    HksFreeKeyNode(&keyNode);
    return ret;
}

int32_t HksCoreMac(const struct HksBlob *key, const struct HksParamSet *paramSet, const struct HksBlob *srcData,
    struct HksBlob *mac)
{
    int32_t ret = HksCoreCheckMacParams(key, paramSet, srcData, mac, false);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check mac params failed");
        return ret;
    }

    struct HksKeyNode *keyNode = HksGenerateKeyNode(key);
    if (keyNode == NULL) {
        HKS_LOG_E("mac generate keynode failed");
        return HKS_ERROR_BAD_STATE;
    }

    do {
        ret = HksAuth(HKS_AUTH_ID_MAC_HMAC, keyNode, paramSet);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("mac auth failed!");
            break;
        }

        struct HksParam *digestParam = NULL;
        ret = HksGetParam(paramSet, HKS_TAG_DIGEST, &digestParam);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("mac get HKS_TAG_DIGEST param failed!");
            break;
        }

        struct HksBlob rawKey = { 0, NULL };
        ret = HksGetRawKey(keyNode->paramSet, &rawKey);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("mac get raw key failed!");
            break;
        }

        ret = HksCryptoHalHmac(&rawKey, digestParam->uint32Param, srcData, mac);
        (void)memset_s(rawKey.data, rawKey.size, 0, rawKey.size);
        HKS_FREE_PTR(rawKey.data);
    } while (0);

    HksFreeKeyNode(&keyNode);
    return ret;
}

int32_t HksCoreRefreshKeyInfo(void)
{
#ifndef _HARDWARE_ROOT_KEY_
    HksRkcDestroy();
    int32_t ret = HksRkcInit();
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Hks rkc refresh info failed! ret = 0x%X", ret);
    }

    return ret;
#else
    return HKS_SUCCESS;
#endif
}

#ifdef HKS_SUPPORT_UPGRADE_STORAGE_DATA
int32_t HksCoreUpgradeKeyInfo(const struct HksBlob *keyAlias, const struct HksBlob *keyInfo, struct HksBlob *keyOut)
{
    return HksUpgradeKeyInfo(keyAlias, keyInfo, keyOut);
}
#endif

#ifdef _STORAGE_LITE_
static int32_t GetMacKey(const struct HksBlob *salt, struct HksBlob *macKey)
{
    uint8_t keyBuf[HKS_KEY_BYTES(HKS_AES_KEY_SIZE_256)] = {0};
    struct HksBlob mk = { HKS_KEY_BYTES(HKS_AES_KEY_SIZE_256), keyBuf };

    int32_t ret = HksCryptoHalGetMainKey(NULL, &mk);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get kek failed, ret = %d", ret);
        return ret;
    }

    struct HksKeyDerivationParam derParam = {
        .salt = *salt,
        .iterations = HKS_KEY_BLOB_DERIVE_CNT,
        .digestAlg = HKS_DIGEST_SHA256,
    };
    struct HksKeySpec derivationSpec = { HKS_ALG_PBKDF2, HKS_KEY_BYTES(HKS_AES_KEY_SIZE_256), &derParam };
    ret = HksCryptoHalDeriveKey(&mk, &derivationSpec, macKey);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get keyblob derive key failed!");
    }

    (void)memset_s(mk.data, mk.size, 0, mk.size);
    return ret;
}

int32_t HksCoreCalcMacHeader(const struct HksParamSet *paramSet, const struct HksBlob *salt,
    const struct HksBlob *srcData, struct HksBlob *mac)
{
    /* 1. get mac key by derive from salt */
    uint8_t keyBuf[HKS_KEY_BYTES(HKS_AES_KEY_SIZE_256)] = {0};
    struct HksBlob macKey = { HKS_KEY_BYTES(HKS_AES_KEY_SIZE_256), keyBuf };
    int32_t ret = GetMacKey(salt, &macKey);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get mac key failed, ret = %d", ret);
        return ret;
    }

    struct HksParam *digestParam = NULL;
    ret = HksGetParam(paramSet, HKS_TAG_DIGEST, &digestParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("calc mac header get HKS_TAG_DIGEST param failed, ret = %d", ret);
        (void)memset_s(macKey.data, macKey.size, 0, macKey.size);
        return ret;
    }

    /* 2. do mac */
    ret = HksCryptoHalHmac(&macKey, digestParam->uint32Param, srcData, mac);
    (void)memset_s(macKey.data, macKey.size, 0, macKey.size);
    return ret;
}
#endif

int32_t HksCoreModuleInit(void)
{
    int32_t ret;
    g_huksMutex = HksMutexCreate();
    if (g_huksMutex == NULL) {
        HKS_LOG_E("Hks mutex init failed, null pointer!");
        ret = HKS_FAILURE;
        return ret;
    }

    ret = HksCryptoAbilityInit();
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Hks init crypto ability failed, ret = %d", ret);
        return ret;
    }
#ifndef _HARDWARE_ROOT_KEY_
    ret = HksRkcInit();
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Hks rkc init failed! ret = 0x%X", ret);
    }
#endif

    return ret;
}

int32_t HksCoreRefresh(void)
{
    return HksCoreRefreshKeyInfo();
}

int32_t HksCoreImportWrappedKey(const struct HksBlob *wrappingKeyAlias, const struct HksBlob *key,
    const struct HksBlob *wrappedKeyData, const struct HksParamSet *paramSet, struct HksBlob *keyOut)
{
    (void)(wrappingKeyAlias);
    (void)(key);
    (void)(wrappedKeyData);
    (void)(paramSet);
    (void)(keyOut);
    return 0;
}

int32_t GetPurposeAndAlgorithm(const struct HksParamSet *paramSet, uint32_t *pur, uint32_t *alg)
{
    uint32_t i;
    if (paramSet == NULL) {
        HKS_LOG_E("paramSet == NULL");
        return HKS_FAILURE;
    }

    HKS_LOG_D("Get paramSet->paramsCnt %u", paramSet->paramsCnt);
    for (i = 0; i < paramSet->paramsCnt; i++) {
        if (paramSet->params[i].tag ==  HKS_TAG_PURPOSE) {
            *pur = paramSet->params[i].uint32Param;
        }

        if (paramSet->params[i].tag ==  HKS_TAG_ALGORITHM) {
            *alg = paramSet->params[i].uint32Param;
        }

        if (*pur != 0 && *alg != 0) {
            HKS_LOG_E("found purpose : %u, algorithm : %u", *pur, *alg);
            break;
        }
    }

    if (i == paramSet->paramsCnt) {
        HKS_LOG_E("don't found purpose or algrithm");
        return HKS_FAILURE;
    }

    if (*alg == HKS_ALG_HMAC || *alg == HKS_ALG_SM3 || *pur == HKS_KEY_PURPOSE_SIGN || *pur == HKS_KEY_PURPOSE_VERIFY) {
        for (i = 0; i < paramSet->paramsCnt; i++) {
            if (paramSet->params[i].tag ==  HKS_TAG_DIGEST) {
                *alg = paramSet->params[i].uint32Param;
                break;
            }
        }

        if (i == paramSet->paramsCnt) {
            HKS_LOG_E("don't found digest");
            return HKS_FAILURE;
        }
    }

    return HKS_SUCCESS;
}

int32_t HksCoreInit(const struct  HksBlob *key, const struct HksParamSet *paramSet, struct HksBlob *handle)
{
    HKS_LOG_D("HksCoreInit in Core start");
    uint32_t pur = 0;
    uint32_t alg = 0;
    int32_t ret;

    if (key == NULL || paramSet == NULL || handle == NULL) {
        HKS_LOG_E("the pointer param entered is invalid");
        return HKS_FAILURE;
    }

    if (handle->size < sizeof(uint64_t)) {
        HKS_LOG_E("handle size is too small, size : %u", handle->size);
        return HKS_ERROR_INSUFFICIENT_MEMORY;
    }

    struct HuksKeyNode *keyNode = HksCreateKeyNode(key, paramSet);
    if (keyNode == NULL || handle == NULL) {
        HKS_LOG_E("the pointer param entered is invalid");
        return HKS_ERROR_BAD_STATE;
    }

    handle->size = sizeof(uint64_t);
    (void)memcpy_s(handle->data, handle->size, &(keyNode->handle), handle->size);

    ret = GetPurposeAndAlgorithm(paramSet, &pur, &alg);
    if (ret != HKS_SUCCESS) {
        HksDeleteKeyNode(keyNode->handle);
        return ret;
    }

    uint32_t i;
    uint32_t size = HKS_ARRAY_SIZE(g_hksCoreInitHandler);
    for (i = 0; i < size; i++) {
        if (g_hksCoreInitHandler[i].pur == pur) {
            HKS_LOG_E("Core HksCoreInit [pur] = %d, pur = %d", g_hksCoreInitHandler[i].pur, pur);
            ret = g_hksCoreInitHandler[i].handler(keyNode, paramSet, alg);
            break;
        }
    }

    if (ret != HKS_SUCCESS) {
        HksDeleteKeyNode(keyNode->handle);
        HKS_LOG_E("CoreInit failed, ret : %d", ret);
        return ret;
    }

    if (i == size) {
        HksDeleteKeyNode(keyNode->handle);
        HKS_LOG_E("don't found purpose, pur : %u", pur);
        return HKS_FAILURE;
    }

    HKS_LOG_D("HksCoreInit in Core end");
    return ret;
}

int32_t HksCoreUpdate(const struct HksBlob *handle, const struct HksParamSet *paramSet, const struct HksBlob *inData,
    struct HksBlob *outData)
{
    HKS_LOG_D("HksCoreUpdate in Core start");
    uint32_t pur = 0;
    uint32_t alg = 0;

    if (handle == NULL || paramSet == NULL || inData == NULL || inData->data == NULL) {
        HKS_LOG_E("the pointer param entered is invalid");
        return HKS_FAILURE;
    }

    uint64_t sessionId;
    if (memcpy_s(&sessionId, sizeof(sessionId), handle->data, handle->size) != EOK) {
        HKS_LOG_E("memcpy handle value fail");
        return HKS_FAILURE;
    }

    struct HuksKeyNode *keyNode = HksQueryKeyNode(sessionId);
    if (keyNode == NULL) {
        HKS_LOG_E("HksCoreUpdate query keynode failed");
        return HKS_ERROR_BAD_STATE;
    }

    int32_t ret = GetPurposeAndAlgorithm(keyNode->runtimeParamSet, &pur, &alg);
    if (ret != HKS_SUCCESS) {
        HksDeleteKeyNode(sessionId);
        return ret;
    }

    uint32_t i;
    uint32_t size = HKS_ARRAY_SIZE(g_hksCoreUpdateHandler);
    for (i = 0; i < size; i++) {
        if (g_hksCoreUpdateHandler[i].pur == pur) {
            ret = g_hksCoreUpdateHandler[i].handler(keyNode, paramSet, inData, outData, alg);
            break;
        }
    }

    if (ret != HKS_SUCCESS) {
        HksDeleteKeyNode(keyNode->handle);
        HKS_LOG_E("CoreUpdate failed, ret : %d", ret);
        return ret;
    }

    if (i == size) {
        HksDeleteKeyNode(sessionId);
        HKS_LOG_E("don't found purpose, pur : %u", pur);
        return HKS_FAILURE;
    }
    return ret;
}

int32_t HksCoreFinish(const struct HksBlob *handle, const struct HksParamSet *paramSet, const struct HksBlob *inData,
    struct HksBlob *outData)
{
    HKS_LOG_D("HksCoreFinish in Core start");
    uint32_t pur = 0;
    uint32_t alg = 0;
    int32_t ret;

    if (handle == NULL || paramSet == NULL || inData == NULL || inData->data == NULL) {
        HKS_LOG_E("the pointer param entered is invalid");
        return HKS_FAILURE;
    }

    uint64_t sessionId;
    if (memcpy_s(&sessionId, sizeof(sessionId), handle->data, handle->size)) {
        HKS_LOG_E("memcpy handle fail");
        return HKS_ERROR_INSUFFICIENT_MEMORY;
    }

    struct HuksKeyNode *keyNode = HksQueryKeyNode(sessionId);
    if (keyNode == NULL) {
        HKS_LOG_E("Cipher generate keynode failed");
        return HKS_ERROR_BAD_STATE;
    }

    ret = GetPurposeAndAlgorithm(keyNode->runtimeParamSet, &pur, &alg);
    if (ret != HKS_SUCCESS) {
        HksDeleteKeyNode(sessionId);
        return ret;
    }

    uint32_t i;
    uint32_t size = HKS_ARRAY_SIZE(g_hksCoreFinishHandler);
    for (i = 0; i < size; i++) {
        if (g_hksCoreFinishHandler[i].pur == pur) {
            ret = g_hksCoreFinishHandler[i].handler(keyNode, paramSet, inData, outData, alg);
            break;
        }
    }

    if (i == size) {
        HKS_LOG_E("don't found purpose, pur : %d", pur);
    }
    HksDeleteKeyNode(sessionId);
    HKS_LOG_D("HksCoreFinish in Core end");
    return ret;
}

int32_t HksCoreAbort(const struct HksBlob *handle, const struct HksParamSet *paramSet)
{
    HKS_LOG_D("HksCoreAbort in Core start");
    uint32_t pur = 0;
    uint32_t alg = 0;
    int32_t ret;

    if (handle == NULL || paramSet == NULL) {
        HKS_LOG_E("the pointer param entered is invalid");
        return HKS_FAILURE;
    }

    uint64_t sessionId;
    if (memcpy_s(&sessionId, sizeof(sessionId), handle->data, handle->size)) {
        HKS_LOG_E("memcpy handle fail");
        return HKS_ERROR_INSUFFICIENT_MEMORY;
    }

    struct HuksKeyNode *keyNode = HksQueryKeyNode(sessionId);
    if (keyNode == NULL) {
        HKS_LOG_E("abort get key node failed");
        return HKS_SUCCESS;
    }

    ret = GetPurposeAndAlgorithm(keyNode->runtimeParamSet, &pur, &alg);
    if (ret != HKS_SUCCESS) {
        HksDeleteKeyNode(sessionId);
        return ret;
    }

    uint32_t i;
    uint32_t size = HKS_ARRAY_SIZE(g_hksCoreAbortHandler);
    for (i = 0; i < size; i++) {
        if (g_hksCoreAbortHandler[i].pur == pur) {
            ret = g_hksCoreAbortHandler[i].handler(keyNode, paramSet, alg);
            break;
        }
    }

    if (i == size) {
        HksDeleteKeyNode(sessionId);
        HKS_LOG_E("don't found purpose, pur : %d", pur);
        return HKS_FAILURE;
    }

    HksDeleteKeyNode(sessionId);
    HKS_LOG_D("HksCoreAbort in Core end");

    return ret;
}

int32_t HksCoreGetKeyProperties(const struct HksParamSet *paramSet, const struct HksBlob *key)
{
    return HksCheckKeyValidity(paramSet, key);
}

int32_t HksCoreAttestKey(const struct HksBlob *key, const  struct HksParamSet *paramSet, struct HksBlob *certChain)
{
#ifdef HKS_SUPPORT_API_ATTEST_KEY
    return HksSoftAttestKey(key, paramSet, certChain);
#else
    (void)key;
    (void)paramSet;
    (void)certChain;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

int32_t HksCoreGetAbility(int32_t funcType)
{
    (void)(funcType);
    return 0;
}

int32_t HksCoreGetHardwareInfo(void)
{
    return 0;
}

HksMutex *HksCoreGetHuksMutex(void)
{
    if (g_huksMutex == NULL) {
        HKS_LOG_E("Hks mutex init failed, reinit!");
        g_huksMutex = HksMutexCreate();
        if (g_huksMutex == NULL) {
            HKS_LOG_E("Hks mutex reinit failed!");
            return NULL;
        }
    }

    return g_huksMutex;
}
#endif /* _CUT_AUTHENTICATE_ */

int32_t HksCoreGenerateRandom(const struct HksParamSet *paramSet, struct HksBlob *random)
{
    (void)paramSet;
    return HksCryptoHalFillRandom(random);
}

