/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "hks_client_ipc.h"
#include "hks_client_service.h"
#include "hks_get_process_info.h"
#include "hks_log.h"
#include "hks_param.h"
#include "hks_template.h"

#define HKS_PROCESS_INFO_LEN    128
#define HKS_MAX_DIRENT_FILE_LEN 128

static int32_t GetProcessInfo(const struct HksParamSet *paramSet, char **processName, char **userId)
{
    (void)paramSet;
#ifdef HKS_ENABLE_LITE_HAP
    struct HksParam *bundleNameParam = NULL;
    if (paramSet != NULL && HksGetParam(paramSet, HKS_TAG_BUNDLE_NAME, &bundleNameParam) == HKS_SUCCESS) {
        // the end of bundleNameParam->blob.data is \0 and it's considered in blob.size
        *processName = (char *)bundleNameParam->blob.data;
    } else {
#endif
        HKS_IF_NOT_SUCC_LOGE_RETURN(HksGetProcessName(processName), HKS_ERROR_INTERNAL_ERROR, "get process name failed")
#ifdef HKS_ENABLE_LITE_HAP
    }
#endif
    HKS_IF_NOT_SUCC_LOGE_RETURN(HksGetUserId(userId), HKS_ERROR_INTERNAL_ERROR, "get user id failed")
    return HKS_SUCCESS;
}

#ifndef _CUT_AUTHENTICATE_
int32_t HksClientInitialize(void)
{
    return HksServiceInitialize();
}

int32_t HksClientRefreshKeyInfo(void)
{
    char *processName = NULL;
    HKS_IF_NOT_SUCC_LOGE_RETURN(HksGetProcessName(&processName), HKS_ERROR_INTERNAL_ERROR, "get process name failed")
    struct HksBlob processNameBlob = { strlen(processName), (uint8_t *)processName };
    return HksServiceRefreshKeyInfo(&processNameBlob);
}

int32_t HksClientGenerateKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSetIn,
    struct HksParamSet *paramSetOut)
{
    (void)paramSetOut;
    char *processName = NULL;
    char *userId = NULL;
    HKS_IF_NOT_SUCC_LOGE_RETURN(GetProcessInfo(paramSetIn, &processName, &userId), HKS_ERROR_INTERNAL_ERROR,
        "get process info failed")

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName },
        0,
        0,
        0,
        HKS_AUTH_STORAGE_LEVEL_DE
    };
    return HksServiceGenerateKey(&processInfo, keyAlias, paramSetIn, NULL);
}

int32_t HksClientImportKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    const struct HksBlob *key)
{
    char *processName = NULL;
    char *userId = NULL;
    HKS_IF_NOT_SUCC_LOGE_RETURN(GetProcessInfo(paramSet, &processName, &userId), HKS_ERROR_INTERNAL_ERROR,
        "get process info failed")

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName },
        0,
        0,
        0,
        HKS_AUTH_STORAGE_LEVEL_DE
    };
    return HksServiceImportKey(&processInfo, keyAlias, paramSet, key);
}

int32_t HksClientImportWrappedKey(const struct HksBlob *keyAlias, const struct HksBlob *wrappingKeyAlias,
    const struct HksParamSet *paramSet, const struct HksBlob *wrappedKeyData)
{
    char *processName = NULL;
    char *userId = NULL;
    HKS_IF_NOT_SUCC_LOGE_RETURN(GetProcessInfo(paramSet, &processName, &userId), HKS_ERROR_INTERNAL_ERROR,
        "get process info failed")

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName },
        0,
        0,
        0,
        HKS_AUTH_STORAGE_LEVEL_DE
    };

    return HksServiceImportWrappedKey(&processInfo, keyAlias, wrappingKeyAlias, paramSet, wrappedKeyData);
}

int32_t HksClientExportPublicKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    struct HksBlob *key)
{
    (void)paramSet;
    char *processName = NULL;
    char *userId = NULL;
    HKS_IF_NOT_SUCC_LOGE_RETURN(GetProcessInfo(paramSet, &processName, &userId), HKS_ERROR_INTERNAL_ERROR,
        "get process info failed")

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName },
        0,
        0,
        0,
        HKS_AUTH_STORAGE_LEVEL_DE
    };
    return HksServiceExportPublicKey(&processInfo, keyAlias, key);
}

int32_t HksClientDeleteKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet)
{
    (void)paramSet;
    char *processName = NULL;
    char *userId = NULL;
    HKS_IF_NOT_SUCC_LOGE_RETURN(GetProcessInfo(paramSet, &processName, &userId), HKS_ERROR_INTERNAL_ERROR,
        "get process info failed")

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName },
        0,
        0,
        0,
        HKS_AUTH_STORAGE_LEVEL_DE
    };
    return HksServiceDeleteKey(&processInfo, keyAlias);
}

int32_t HksClientGetKeyParamSet(const struct HksBlob *keyAlias, const struct HksParamSet *paramSetIn,
    struct HksParamSet *paramSetOut)
{
    (void)paramSetIn;
    char *processName = NULL;
    char *userId = NULL;
    HKS_IF_NOT_SUCC_LOGE_RETURN(GetProcessInfo(NULL, &processName, &userId), HKS_ERROR_INTERNAL_ERROR,
        "get process info failed")

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName },
        0,
        0,
        0,
        HKS_AUTH_STORAGE_LEVEL_DE
    };
    return HksServiceGetKeyParamSet(&processInfo, keyAlias, paramSetOut);
}

int32_t HksClientKeyExist(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet)
{
    (void)paramSet;
    char *processName = NULL;
    char *userId = NULL;
    HKS_IF_NOT_SUCC_LOGE_RETURN(GetProcessInfo(paramSet, &processName, &userId), HKS_ERROR_INTERNAL_ERROR,
        "get process info failed")

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName },
        0,
        0,
        0,
        HKS_AUTH_STORAGE_LEVEL_DE
    };
    return HksServiceKeyExist(&processInfo, keyAlias);
}

int32_t HksClientSign(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *signature)
{
    char *processName = NULL;
    char *userId = NULL;
    HKS_IF_NOT_SUCC_LOGE_RETURN(GetProcessInfo(paramSet, &processName, &userId), HKS_ERROR_INTERNAL_ERROR,
        "get process info failed")

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName },
        0,
        0,
        0,
        HKS_AUTH_STORAGE_LEVEL_DE
    };
    return HksServiceSign(&processInfo, key, paramSet, srcData, signature);
}

int32_t HksClientVerify(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, const struct HksBlob *signature)
{
    char *processName = NULL;
    char *userId = NULL;
    HKS_IF_NOT_SUCC_LOGE_RETURN(GetProcessInfo(paramSet, &processName, &userId), HKS_ERROR_INTERNAL_ERROR,
        "get process info failed")

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName },
        0,
        0,
        0,
        HKS_AUTH_STORAGE_LEVEL_DE
    };
    return HksServiceVerify(&processInfo, key, paramSet, srcData, signature);
}

int32_t HksClientEncrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *plainText, struct HksBlob *cipherText)
{
    char *processName = NULL;
    char *userId = NULL;
    HKS_IF_NOT_SUCC_LOGE_RETURN(GetProcessInfo(paramSet, &processName, &userId), HKS_ERROR_INTERNAL_ERROR,
        "get process info failed")

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName },
        0,
        0,
        0,
        HKS_AUTH_STORAGE_LEVEL_DE
    };
    return HksServiceEncrypt(&processInfo, key, paramSet, plainText, cipherText);
}

int32_t HksClientDecrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *cipherText, struct HksBlob *plainText)
{
    char *processName = NULL;
    char *userId = NULL;
    HKS_IF_NOT_SUCC_LOGE_RETURN(GetProcessInfo(paramSet, &processName, &userId), HKS_ERROR_INTERNAL_ERROR,
        "get process info failed")

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName },
        0,
        0,
        0,
        HKS_AUTH_STORAGE_LEVEL_DE
    };
    return HksServiceDecrypt(&processInfo, key, paramSet, cipherText, plainText);
}

int32_t HksClientAgreeKey(const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, struct HksBlob *agreedKey)
{
    char *processName = NULL;
    char *userId = NULL;
    HKS_IF_NOT_SUCC_LOGE_RETURN(GetProcessInfo(paramSet, &processName, &userId), HKS_ERROR_INTERNAL_ERROR,
        "get process info failed")

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName },
        0,
        0,
        0,
        HKS_AUTH_STORAGE_LEVEL_DE
    };
    return HksServiceAgreeKey(&processInfo, paramSet, privateKey, peerPublicKey, agreedKey);
}

int32_t HksClientDeriveKey(const struct HksParamSet *paramSet, const struct HksBlob *mainKey,
    struct HksBlob *derivedKey)
{
    char *processName = NULL;
    char *userId = NULL;
    HKS_IF_NOT_SUCC_LOGE_RETURN(GetProcessInfo(paramSet, &processName, &userId), HKS_ERROR_INTERNAL_ERROR,
        "get process info failed")

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName },
        0,
        0,
        0,
        HKS_AUTH_STORAGE_LEVEL_DE
    };
    return HksServiceDeriveKey(&processInfo, paramSet, mainKey, derivedKey);
}

int32_t HksClientMac(const struct HksBlob *key, const struct HksParamSet *paramSet, const struct HksBlob *srcData,
    struct HksBlob *mac)
{
    char *processName = NULL;
    char *userId = NULL;
    HKS_IF_NOT_SUCC_LOGE_RETURN(GetProcessInfo(paramSet, &processName, &userId), HKS_ERROR_INTERNAL_ERROR,
        "get process info failed")

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName },
        0,
        0,
        0,
        HKS_AUTH_STORAGE_LEVEL_DE
    };
    return HksServiceMac(&processInfo, key, paramSet, srcData, mac);
}

int32_t HksClientGetKeyInfoList(const struct HksParamSet *paramSet, struct HksKeyInfo *keyInfoList,
    uint32_t *listCount)
{
    (void)paramSet;
    char *processName = NULL;
    char *userId = NULL;
    HKS_IF_NOT_SUCC_LOGE_RETURN(GetProcessInfo(NULL, &processName, &userId), HKS_ERROR_INTERNAL_ERROR,
        "get process info failed")

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName },
        0,
        0,
        0,
        HKS_AUTH_STORAGE_LEVEL_DE
    };
    return HksServiceGetKeyInfoList(&processInfo, keyInfoList, listCount);
}

int32_t HksClientAttestKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    struct HksCertChain *certChain, bool needAnonCertChain)
{
    (void)keyAlias;
    (void)paramSet;
    (void)certChain;
    (void)needAnonCertChain;
    return HKS_ERROR_NOT_SUPPORTED;
}

int32_t HksClientInit(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    struct HksBlob *handle, struct HksBlob *token)
{
    char *processName = NULL;
    char *userId = NULL;
    HKS_IF_NOT_SUCC_LOGE_RETURN(GetProcessInfo(paramSet, &processName, &userId), HKS_ERROR_INTERNAL_ERROR,
        "get process info failed")

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName },
        0,
        0,
        0,
        HKS_AUTH_STORAGE_LEVEL_DE
    };

    struct HksBlob tokenTmp = { 0, NULL };
    if ((token != NULL) && (token->size != 0)) {
        tokenTmp.size = token->size;
        tokenTmp.data = token->data;
    }
    int32_t ret = HksServiceInit(&processInfo, keyAlias, paramSet, handle, &tokenTmp);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "service init failed, ret = %" LOG_PUBLIC "d", ret)

    if ((token != NULL) && (token->size != 0)) {
        token->size = tokenTmp.size;
    }
    return ret;
}

int32_t HksClientUpdate(const struct HksBlob *handle, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData)
{
    char *processName = NULL;
    char *userId = NULL;
    HKS_IF_NOT_SUCC_LOGE_RETURN(GetProcessInfo(paramSet, &processName, &userId), HKS_ERROR_INTERNAL_ERROR,
        "get process info failed")

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName },
        0,
        0,
        0,
        HKS_AUTH_STORAGE_LEVEL_DE
    };
    return HksServiceUpdate(handle, &processInfo, paramSet, inData, outData);
}

int32_t HksClientFinish(const struct HksBlob *handle, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData)
{
    char *processName = NULL;
    char *userId = NULL;
    HKS_IF_NOT_SUCC_LOGE_RETURN(GetProcessInfo(paramSet, &processName, &userId), HKS_ERROR_INTERNAL_ERROR,
        "get process info failed")

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName },
        0,
        0,
        0,
        HKS_AUTH_STORAGE_LEVEL_DE
    };
    return HksServiceFinish(handle, &processInfo, paramSet, inData, outData);
}

int32_t HksClientAbort(const struct HksBlob *handle, const struct HksParamSet *paramSet)
{
    char *processName = NULL;
    char *userId = NULL;
    HKS_IF_NOT_SUCC_LOGE_RETURN(GetProcessInfo(paramSet, &processName, &userId), HKS_ERROR_INTERNAL_ERROR,
        "get process info failed")

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName },
        0,
        0,
        0,
        HKS_AUTH_STORAGE_LEVEL_DE
    };
    return HksServiceAbort(handle, &processInfo, paramSet);
}
#endif

int32_t HksClientGenerateRandom(struct HksBlob *random, const struct HksParamSet *paramSet)
{
    (void)paramSet;
    char *processName = NULL;
    char *userId = NULL;
    HKS_IF_NOT_SUCC_LOGE_RETURN(GetProcessInfo(paramSet, &processName, &userId), HKS_ERROR_INTERNAL_ERROR,
        "get process info failed")

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName },
        0,
        0,
        0,
        HKS_AUTH_STORAGE_LEVEL_DE
    };
    return HksServiceGenerateRandom(&processInfo, random);
}

#ifdef HKS_SUPPORT_CHIPSET_PLATFORM_DECRYPT
int32_t HksClientExportChipsetPlatformPublicKey(const struct HksBlob *salt,
    enum HksChipsetPlatformDecryptScene scene, struct HksBlob *publicKey)
{
    return HksServiceExportChipsetPlatformPublicKey(salt, scene, publicKey);
}
#endif