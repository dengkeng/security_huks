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

#ifndef HKS_IPC_SERVICE_H
#define HKS_IPC_SERVICE_H

#include "hks_type.h"

#ifdef __cplusplus
extern "C" {
#endif

void HksIpcServiceProvision(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceProvisionVerify(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceGenerateKey(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceImportKey(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceExportPublicKey(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceDeleteKey(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceGetKeyParamSet(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceKeyExist(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceGenerateRandom(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceSign(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceSignInit(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceSignUpdate(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceSignFinal(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceVerify(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceVerifyInit(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceVerifyUpdate(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceVerifyFinal(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceEncrypt(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceEncryptInit(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceEncryptUpdate(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceEncryptFinal(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceDecrypt(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceDecryptInit(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceDecryptUpdate(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceDecryptFinal(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceAgreeKey(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceDeriveKey(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceMac(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceMacInit(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceMacUpdate(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceMacFinal(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceGetKeyInfoList(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceAttestKey(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceGetCertificateChain(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceWrapKey(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceUnwrapKey(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceSignWithDeviceKey(const struct HksBlob *msg, const uint8_t *context);

void HksIpcServiceExportTrustCerts(const struct HksBlob *srcData, const uint8_t *context);

void HksIpcServiceWrapKey(const struct HksBlob *srcData, const uint8_t *context);

void HksIpcServiceUnwrapKey(const struct HksBlob *srcData, const uint8_t *context);

#ifdef __cplusplus
}
#endif

#endif
