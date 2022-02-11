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

#include "hks_api.h"

#include "hks_ability.h"
#include "hks_client_ipc.h"
#include "hks_local_engine.h"
#include "hks_log.h"
#include "hks_param.h"
#include "hks_type.h"

#ifdef _CUT_AUTHENTICATE_
#undef HKS_SUPPORT_API_GENERATE_KEY
#undef HKS_SUPPORT_API_IMPORT
#undef HKS_SUPPORT_API_EXPORT
#undef HKS_SUPPORT_API_DELETE_KEY
#undef HKS_SUPPORT_API_GET_KEY_PARAM_SET
#undef HKS_SUPPORT_API_KEY_EXIST
#undef HKS_SUPPORT_API_SIGN_VERIFY
#undef HKS_SUPPORT_API_SIGN_VERIFY
#undef HKS_SUPPORT_API_AGREE_KEY
#undef HKS_SUPPORT_API_HASH
#undef HKS_SUPPORT_API_GET_KEY_INFO_LIST
#undef HKS_SUPPORT_API_ATTEST_KEY
#undef HKS_SUPPORT_API_GET_CERTIFICATE_CHAIN
#undef HKS_SUPPORT_API_WRAP_KEY
#undef HKS_SUPPORT_API_UNWRAP_KEY
#endif

#ifdef HKS_SUPPORT_SEC_LEVEL
uint8_t g_secInfo[HKS_MAX_RANDOM_LEN] = {0};
uint8_t g_challengeInfo[HKS_MAX_RANDOM_LEN] = {0};
const char g_certInfo[] = "hello_cert_chain_hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh";
struct HksBlob g_secLevelInfo = { HKS_MAX_RANDOM_LEN, g_secInfo };
struct HksBlob g_challenge = { HKS_MAX_RANDOM_LEN, g_challengeInfo };
const struct HksBlob g_cert = { sizeof(g_certInfo), (uint8_t *)g_certInfo };
#endif

HKS_API_EXPORT int32_t HksGetSdkVersion(struct HksBlob *sdkVersion)
{
    if ((sdkVersion == NULL) || (sdkVersion->data == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }

    uint32_t versionLen = strlen(HKS_SDK_VERSION);
    if (sdkVersion->size < (versionLen + 1)) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if (memcpy_s(sdkVersion->data, sdkVersion->size, HKS_SDK_VERSION, versionLen) != EOK) {
        return HKS_ERROR_BAD_STATE;
    }

    sdkVersion->data[versionLen] = '\0';
    sdkVersion->size = versionLen;
    return HKS_SUCCESS;
}

HKS_API_EXPORT int32_t HksInitialize(void)
{
#ifndef _CUT_AUTHENTICATE_
    HKS_LOG_I("enter initialize");
    int32_t ret = HksClientInitialize();
    HKS_LOG_I("leave initialize, result = %d", ret);
    return ret;
#else
    (void)HksCryptoAbilityInit();
    return HKS_SUCCESS;
#endif
}

HKS_API_EXPORT int32_t HksRefreshKeyInfo(void)
{
#ifndef _CUT_AUTHENTICATE_
    HKS_LOG_I("enter refresh key info");
    int32_t ret = HksClientRefreshKeyInfo();
    HKS_LOG_I("leave refresh key info, result = %d", ret);
    return ret;
#else
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksGenerateKey(const struct HksBlob *keyAlias,
    const struct HksParamSet *paramSetIn, struct HksParamSet *paramSetOut)
{
#ifdef HKS_SUPPORT_API_GENERATE_KEY
    HKS_LOG_I("enter generate key");
    struct HksParam *storageFlag = NULL;
    int32_t ret = HksGetParam(paramSetIn, HKS_TAG_KEY_STORAGE_FLAG, &storageFlag);
    if ((ret == HKS_SUCCESS) && (storageFlag->uint32Param == HKS_STORAGE_TEMP)) {
        if ((paramSetIn == NULL) || (paramSetOut == NULL)) {
            return HKS_ERROR_NULL_POINTER;
        }
        ret = HksLocalGenerateKey(paramSetIn, paramSetOut);
        HKS_LOG_I("leave generate temp key, result = %d", ret);
        return ret;
    }

    /* generate persistent keys */
    if ((paramSetIn == NULL) || (keyAlias == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }
    ret = HksClientGenerateKey(keyAlias, paramSetIn, paramSetOut);
    HKS_LOG_I("leave generate persistent key, result = %d", ret);
    return ret;
#else
    (void)keyAlias;
    (void)paramSetIn;
    (void)paramSetOut;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksImportKey(const struct HksBlob *keyAlias,
    const struct HksParamSet *paramSet, const struct HksBlob *key)
{
#ifdef HKS_SUPPORT_API_IMPORT
    HKS_LOG_I("enter import key");
    if ((keyAlias == NULL) || (paramSet == NULL) || (key == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }
    int32_t ret = HksClientImportKey(keyAlias, paramSet, key);
    HKS_LOG_I("leave import key, result = %d", ret);
    return ret;
#else
    (void)keyAlias;
    (void)paramSet;
    (void)key;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksExportPublicKey(const struct HksBlob *keyAlias,
    const struct HksParamSet *paramSet, struct HksBlob *key)
{
#ifdef HKS_SUPPORT_API_EXPORT
    HKS_LOG_I("enter export public key");
    if ((keyAlias == NULL) || (key == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }
    int32_t ret = HksClientExportPublicKey(keyAlias, paramSet, key);
    HKS_LOG_I("leave export public key, result = %d", ret);
    return ret;
#else
    (void)keyAlias;
    (void)paramSet;
    (void)key;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksDeleteKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet)
{
#ifdef HKS_SUPPORT_API_DELETE_KEY
    HKS_LOG_I("enter delete key");
    if (keyAlias == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }
    int32_t ret = HksClientDeleteKey(keyAlias, paramSet);
    HKS_LOG_I("leave delete key, result = %d", ret);
    return ret;
#else
    (void)keyAlias;
    (void)paramSet;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksGetKeyParamSet(const struct HksBlob *keyAlias,
    const struct HksParamSet *paramSetIn, struct HksParamSet *paramSetOut)
{
#ifdef HKS_SUPPORT_API_GET_KEY_PARAM_SET
    HKS_LOG_I("enter get key paramset");
    (void)paramSetIn;
    if ((keyAlias == NULL) || (paramSetOut == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }
    int32_t ret = HksClientGetKeyParamSet(keyAlias, paramSetOut);
    HKS_LOG_I("leave get key paramset, result = %d", ret);
    return ret;
#else
    (void)keyAlias;
    (void)paramSetIn;
    (void)paramSetOut;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksKeyExist(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet)
{
#ifdef HKS_SUPPORT_API_KEY_EXIST
    HKS_LOG_I("enter check key exist");
    if (keyAlias == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }
    int32_t ret = HksClientKeyExist(keyAlias, paramSet);
    HKS_LOG_I("leave check key exist, result = %d", ret);
    return ret;
#else
    (void)keyAlias;
    (void)paramSet;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksGenerateRandom(const struct HksParamSet *paramSet, struct HksBlob *random)
{
#ifdef HKS_SUPPORT_API_GENERATE_RANDOM
    HKS_LOG_I("enter generate random");
    if (random == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }
    int32_t ret = HksClientGenerateRandom(random, paramSet);
    HKS_LOG_I("leave generate random, result = %d", ret);
    return ret;
#else
    (void)paramSet;
    (void)random;
#endif
}

HKS_API_EXPORT int32_t HksSign(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *signature)
{
#ifdef HKS_SUPPORT_API_SIGN_VERIFY
    HKS_LOG_I("enter sign");
    if ((key == NULL) || (paramSet == NULL) || (srcData == NULL) || (signature == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }

    struct HksParam *isKeyAlias = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_IS_KEY_ALIAS, &isKeyAlias);
    if ((ret == HKS_SUCCESS) && (!isKeyAlias->boolParam)) {
        return HksLocalSign(key, paramSet, srcData, signature);
    }

    return HksClientSign(key, paramSet, srcData, signature);
#else
    (void)key;
    (void)paramSet;
    (void)srcData;
    (void)signature;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksVerify(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, const struct HksBlob *signature)
{
#ifdef HKS_SUPPORT_API_SIGN_VERIFY
    HKS_LOG_I("enter verify");
    if ((key == NULL) || (paramSet == NULL) || (srcData == NULL) || (signature == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }

    struct HksParam *isKeyAlias = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_IS_KEY_ALIAS, &isKeyAlias);
    if ((ret == HKS_SUCCESS) && (!isKeyAlias->boolParam)) {
        ret = HksLocalVerify(key, paramSet, srcData, signature);
        HKS_LOG_I("leave verify with plain key, result = %d", ret);
        return ret;
    }
    ret = HksClientVerify(key, paramSet, srcData, signature);
    HKS_LOG_I("leave verify with persistent key, result = %d", ret);
    return ret;
#else
    (void)key;
    (void)paramSet;
    (void)srcData;
    (void)signature;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksEncrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *plainText, struct HksBlob *cipherText)
{
#ifdef HKS_SUPPORT_API_CIPHER
    HKS_LOG_I("enter encrypt");
    if ((key == NULL) || (paramSet == NULL) || (plainText == NULL) || (cipherText == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }

    struct HksParam *isKeyAlias = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_IS_KEY_ALIAS, &isKeyAlias);
    if ((ret == HKS_SUCCESS) && (!isKeyAlias->boolParam)) {
        ret = HksLocalEncrypt(key, paramSet, plainText, cipherText);
        HKS_LOG_I("leave encrypt with plain key, result = %d", ret);
        return ret;
    }
#ifndef _CUT_AUTHENTICATE_
    ret = HksClientEncrypt(key, paramSet, plainText, cipherText);
    HKS_LOG_I("leave encrypt with persistent key, result = %d", ret);
    return ret;
#else
    return HKS_ERROR_NOT_SUPPORTED;
#endif
#else
    (void)key;
    (void)paramSet;
    (void)plainText;
    (void)cipherText;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksDecrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *cipherText, struct HksBlob *plainText)
{
#ifdef HKS_SUPPORT_API_CIPHER
    HKS_LOG_I("enter decrypt");
    if ((key == NULL) || (paramSet == NULL) || (cipherText == NULL) || (plainText == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }

    struct HksParam *isKeyAlias = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_IS_KEY_ALIAS, &isKeyAlias);
    if ((ret == HKS_SUCCESS) && (!isKeyAlias->boolParam)) {
        ret = HksLocalDecrypt(key, paramSet, cipherText, plainText);
        HKS_LOG_I("leave decrypt with plain key, result = %d", ret);
        return ret;
    }
#ifndef _CUT_AUTHENTICATE_
    ret = HksClientDecrypt(key, paramSet, cipherText, plainText);
    HKS_LOG_I("leave decrypt with persistent key, result = %d", ret);
    return ret;
#else
    return HKS_ERROR_NOT_SUPPORTED;
#endif
#else
    (void)key;
    (void)paramSet;
    (void)plainText;
    (void)cipherText;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksAgreeKey(const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, struct HksBlob *agreedKey)
{
#ifdef HKS_SUPPORT_API_AGREE_KEY
    HKS_LOG_I("enter agree key");
    if ((paramSet == NULL) || (privateKey == NULL) || (peerPublicKey == NULL) || (agreedKey == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }

    struct HksParam *isKeyAlias = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_IS_KEY_ALIAS, &isKeyAlias);
    if ((ret == HKS_SUCCESS) && (!isKeyAlias->boolParam)) {
        ret = HksLocalAgreeKey(paramSet, privateKey, peerPublicKey, agreedKey);
        HKS_LOG_I("leave agree key with plain key, result = %d", ret);
        return ret;
    }

    ret = HksClientAgreeKey(paramSet, privateKey, peerPublicKey, agreedKey);
    HKS_LOG_I("leave agree key with persistent key, result = %d", ret);
    return ret;
#else
    (void)paramSet;
    (void)privateKey;
    (void)peerPublicKey;
    (void)agreedKey;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksDeriveKey(const struct HksParamSet *paramSet, const struct HksBlob *mainKey,
    struct HksBlob *derivedKey)
{
#ifdef HKS_SUPPORT_API_DERIVE_KEY
    HKS_LOG_I("enter derive key");
    if ((paramSet == NULL) || (mainKey == NULL) || (derivedKey == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }

    struct HksParam *isKeyAlias = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_IS_KEY_ALIAS, &isKeyAlias);
    if ((ret == HKS_SUCCESS) && (!isKeyAlias->boolParam)) {
        ret = HksLocalDeriveKey(paramSet, mainKey, derivedKey);
        HKS_LOG_I("leave derive key with plain key, result = %d", ret);
        return ret;
    }
#ifndef _CUT_AUTHENTICATE_
    ret = HksClientDeriveKey(paramSet, mainKey, derivedKey);
    HKS_LOG_I("leave derive key with persistent key, result = %d", ret);
    return ret;
#else
    return HKS_ERROR_NOT_SUPPORTED;
#endif
#else
    (void)paramSet;
    (void)mainKey;
    (void)derivedKey;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksMac(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *mac)
{
#ifdef HKS_SUPPORT_API_MAC
    HKS_LOG_I("enter mac");
    if ((key == NULL) || (paramSet == NULL) || (srcData == NULL) || (mac == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }

    struct HksParam *isKeyAlias = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_IS_KEY_ALIAS, &isKeyAlias);
    if ((ret == HKS_SUCCESS) && (!isKeyAlias->boolParam)) {
        ret = HksLocalMac(key, paramSet, srcData, mac);
        HKS_LOG_I("leave mac with plain key, result = %d", ret);
        return ret;
    }
#ifndef _CUT_AUTHENTICATE_
    ret = HksClientMac(key, paramSet, srcData, mac);
    HKS_LOG_I("leave mac with persistent key, result = %d", ret);
    return ret;
#else
    return HKS_ERROR_NOT_SUPPORTED;
#endif
#else
    (void)key;
    (void)paramSet;
    (void)srcData;
    (void)mac;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksHash(const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *hash)
{
#ifdef HKS_SUPPORT_API_HASH
    HKS_LOG_I("enter hash");
    if ((paramSet == NULL) || (srcData == NULL) || (hash == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }
    int32_t ret = HksLocalHash(paramSet, srcData, hash);
    HKS_LOG_I("leave hash, result = %d", ret);
    return ret;
#else
    (void)paramSet;
    (void)srcData;
    (void)hash;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksGetKeyInfoList(const struct HksParamSet *paramSet,
    struct HksKeyInfo *keyInfoList, uint32_t *listCount)
{
#ifdef HKS_SUPPORT_API_GET_KEY_INFO_LIST
    HKS_LOG_I("enter get key info list");
    (void)paramSet;
    if ((keyInfoList == NULL) || (listCount == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }
    int32_t ret = HksClientGetKeyInfoList(keyInfoList, listCount);
    HKS_LOG_I("leave get key info list, result = %d", ret);
    return ret;
#else
    (void)paramSet;
    (void)keyInfoList;
    (void)listCount;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksAttestKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    struct HksCertChain *certChain)
{
#ifdef HKS_SUPPORT_SEC_LEVEL
    if ((keyAlias == NULL) || (paramSet == NULL) || (certChain == NULL)) {
        return HKS_ERROR_NOT_SUPPORTED;
    }
    if (certChain->certs == NULL || certChain->certs->data == NULL ||
        HksCheckParamSet(paramSet, paramSet->paramSetSize) != HKS_SUCCESS) {
        return HKS_ERROR_NOT_SUPPORTED;
    }
    for (uint32_t i = 0; i < paramSet->paramsCnt; i++) {
        if (paramSet->params[i].tag == HKS_TAG_ATTESTATION_CHALLENGE) {
            if (memcpy_s(g_challenge.data, HKS_MAX_RANDOM_LEN, paramSet->params[i].blob.data,
                paramSet->params[i].blob.size) != EOK) {
                return HKS_ERROR_NOT_SUPPORTED;
                }
            g_challenge.size = paramSet->params[i].blob.size;
        }
    }
    if (memcpy_s(certChain->certs->data, certChain->certs->size, g_cert.data, g_cert.size) != EOK) {
        return HKS_ERROR_NOT_SUPPORTED;
    }
    certChain->certs->size = g_cert.size;
    return HKS_SUCCESS;
#endif

#ifdef HKS_SUPPORT_API_ATTEST_KEY
    HKS_LOG_I("enter attest key");
    if ((keyAlias == NULL) || (paramSet == NULL) || (certChain == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }
    int32_t ret = HksClientAttestKey(keyAlias, paramSet, certChain);
    HKS_LOG_I("leave attest key, result = %d", ret);
    return ret;
#else
    (void)keyAlias;
    (void)paramSet;
    (void)certChain;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksGetCertificateChain(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    struct HksCertChain *certChain)
{
#ifdef HKS_SUPPORT_API_GET_CERTIFICATE_CHAIN
    HKS_LOG_I("enter get certificate chain");
    if ((keyAlias == NULL) || (paramSet == NULL) || (certChain == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }
    int32_t ret = HksClientGetCertificateChain(keyAlias, paramSet, certChain);
    HKS_LOG_I("leave get certificate chain, result = %d", ret);
    return ret;
#else
    (void)keyAlias;
    (void)paramSet;
    (void)certChain;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksWrapKey(const struct HksBlob *keyAlias, const struct HksBlob *targetKeyAlias,
    const struct HksParamSet *paramSet, struct HksBlob *wrappedData)
{
#ifdef HKS_SUPPORT_API_WRAP_KEY
    HKS_LOG_I("enter wrap key");
    if ((keyAlias == NULL) || (targetKeyAlias == NULL) || (paramSet == NULL) || (wrappedData == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }
    int32_t ret = HksClientWrapKey(keyAlias, targetKeyAlias, paramSet, wrappedData);
    HKS_LOG_I("leave wrap key, result = %d", ret);
    return ret;
#else
    (void)keyAlias;
    (void)targetKeyAlias;
    (void)paramSet;
    (void)wrappedData;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksUnwrapKey(const struct HksBlob *keyAlias, const struct HksBlob *targetKeyAlias,
    const struct HksBlob *wrappedData, const struct HksParamSet *paramSet)
{
#ifdef HKS_SUPPORT_API_UNWRAP_KEY
    HKS_LOG_I("enter unwrap key");
    if ((keyAlias == NULL) || (targetKeyAlias == NULL) || (wrappedData == NULL) || (paramSet == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }
    int32_t ret = HksClientUnwrapKey(keyAlias, targetKeyAlias, wrappedData, paramSet);
    HKS_LOG_I("leave unwrap key, result = %d", ret);
    return ret;
#else
    (void)keyAlias;
    (void)targetKeyAlias;
    (void)paramSet;
    (void)wrappedData;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksBnExpMod(struct HksBlob *x, const struct HksBlob *a,
    const struct HksBlob *e, const struct HksBlob *n)
{
#ifdef HKS_SUPPORT_API_BN_EXP_MOD
    HKS_LOG_I("enter bn exp mod");
    if ((x == NULL) || (a == NULL) || (e == NULL) || (n == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }

    int32_t ret = HksLocalBnExpMod(x, a, e, n);
    HKS_LOG_I("leave bn exp mod key, result = %d", ret);
    return ret;
#else
    (void)x;
    (void)a;
    (void)e;
    (void)n;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HcmIsDeviceKeyExist(const struct HksParamSet *paramSet)
{
    (void)paramSet;
    return HKS_ERROR_NOT_SUPPORTED;
}

HKS_API_EXPORT int32_t HksImportId(const uint32_t idType, const struct HksBlob *id)
{
#ifdef HKS_SUPPORT_SEC_LEVEL
    if (id == NULL || id->data == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (idType != HKS_TAG_ATTESTATION_ID_SEC_LEVEL_INFO) {
        HKS_LOG_E("invalid idType %x", idType);
        return HKS_ERROR_NOT_SUPPORTED;
    }

    if (id->size >= HKS_MAX_RANDOM_LEN) {
        return HKS_ERROR_BAD_STATE;
    }

    if (memcmp(g_secLevelInfo.data, id->data, id->size) != 0) {
        if (memcpy_s(g_secLevelInfo.data, HKS_MAX_RANDOM_LEN, id->data, id->size) != EOK) {
            return HKS_ERROR_BAD_STATE;
        }
        g_secLevelInfo.size = id->size;
    }

    HKS_LOG_I("importId success, idType is %x", idType);
    return HKS_SUCCESS;
#else
    (void)idType;
    (void)id;
    return HKS_SUCCESS;
#endif
}

HKS_API_EXPORT int32_t HksIsIdExist(const uint32_t idType)
{
    if (idType == HKS_TAG_ATTESTATION_ID_SEC_LEVEL_INFO) {
        return HKS_SUCCESS;
    } else {
        return HKS_ERROR_NOT_SUPPORTED;
    }
}

HKS_API_EXPORT int32_t HksIsAttestReady(void)
{
    return HKS_SUCCESS;
}

HKS_API_EXPORT int32_t HksValidateCertChain(const struct HksBlob *certChain, struct HksParamSet *paramSetOut)
{
    (void)certChain;
#ifdef HKS_SUPPORT_SEC_LEVEL
    for (uint32_t i = 0; i < paramSetOut->paramsCnt; i++) {
        switch (paramSetOut->params[i].tag) {
            case HKS_TAG_ATTESTATION_CHALLENGE:
                HKS_LOG_I("getting challenge");
                if (memcpy_s(paramSetOut->params[i].blob.data, paramSetOut->params[i].blob.size,
                    g_challenge.data, g_challenge.size) != EOK) {
                    return HKS_ERROR_BAD_STATE;
                }
                paramSetOut->params[i].blob.size = g_challenge.size;
                continue;
            case HKS_TAG_ATTESTATION_ID_SEC_LEVEL_INFO:
                HKS_LOG_I("getting sec level");
                if (memcpy_s(paramSetOut->params[i].blob.data, paramSetOut->params[i].blob.size,
                    g_secLevelInfo.data, g_secLevelInfo.size) != EOK) {
                    return HKS_ERROR_BAD_STATE;
                }
                paramSetOut->params[i].blob.size = g_secLevelInfo.size;
                continue;
            default:
                HKS_LOG_I("invalid tag %x", paramSetOut->params[i].tag);
                return HKS_ERROR_NOT_SUPPORTED;
        }
    }
    return HKS_SUCCESS;
#else
    (void)paramSetOut;
    return HKS_SUCCESS;
#endif
}

HKS_API_EXPORT int32_t HksDeleteUserIDKeyAliasFile(const char *userID)
{
#ifdef HKS_SUPPORT_API_IMPORT
    HKS_LOG_I("enter DeleteUserIDKeyAliasFile");
    if (userID == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }
    int32_t ret = HksClientDeleteUserIDKeyAliasFile(userID);
    HKS_LOG_I("leave DeleteUserIDKeyAliasFile, result = %d", ret);
    return ret;
#else
    (void)userID;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksDeleteUIDKeyAliasFile(const char *userID, const char *uid)
{
#ifdef HKS_SUPPORT_API_IMPORT
    HKS_LOG_I("enter DeleteUIDKeyAliasFile");
    if ((userID == NULL) || (uid == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }
    int32_t ret = HksClientDeleteUIDKeyAliasFile(userID, uid);
    HKS_LOG_I("leave DeleteUIDKeyAliasFile, result = %d", ret);
    return ret;
#else
    (void)userID;
    (void)uid;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksInit(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    struct HksBlob *handle)
{
    if ((keyAlias == NULL) || (paramSet == NULL) || (handle == NULL)) {
        HKS_LOG_E("keyAlias : %p, paramSet : %p, handle : %p", keyAlias, paramSet, handle);
        return HKS_ERROR_NULL_POINTER;
    }

    int32_t ret = HksClientInit(keyAlias, paramSet, handle);
    return ret;
}

HKS_API_EXPORT int32_t HksUpdate(const struct HksBlob *handle, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData)
{
    if ((handle == NULL) || (paramSet == NULL) || (inData == NULL) || (outData == NULL)) {
        HKS_LOG_E("handle : %p, paramSet : %p, inData: %p, outData : %p", handle, paramSet, inData, outData);
        return HKS_ERROR_NULL_POINTER;
    }

    int32_t ret = HksClientUpdate(handle, paramSet, inData, outData);
    return ret;
}

HKS_API_EXPORT int32_t HksFinish(const struct HksBlob *handle, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData)
{
    if ((handle == NULL) || (paramSet == NULL) || (inData == NULL) || (outData == NULL)) {
        HKS_LOG_E("handle: %p, paramSet: %p, inData: %p, outData: %p", handle, paramSet, inData, outData);
        return HKS_ERROR_NULL_POINTER;
    }

    int32_t ret = HksClientFinish(handle, paramSet, inData, outData);
    return ret;
}

HKS_API_EXPORT int32_t HksAbort(const struct HksBlob *handle, const struct HksParamSet *paramSet)
{
    if ((handle == NULL) || (paramSet == NULL)) {
        HKS_LOG_E("paramSet: %p, handle: %p", paramSet, handle);
        return HKS_ERROR_NULL_POINTER;
    }

    int32_t ret = HksClientAbort(handle, paramSet);
    return ret;
}
