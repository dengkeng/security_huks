/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef HKS_UPGRADE_OPERATION_H
#define HKS_UPGRADE_OPERATION_H

#ifdef HKS_CONFIG_FILE
#include HKS_CONFIG_FILE
#else
#include "hks_config.h"
#endif

#include "hks_param.h"
#include "hks_type_inner.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HKS_ENABLE_SMALL_TO_SERVICE
int32_t HksChangeKeyOwnerForSmallToService(const struct HksProcessInfo *processInfo, const struct HksBlob *keyAlias,
    int32_t mode);

int32_t HksDeleteOldKeyForSmallToService(const struct HksBlob *keyAlias);

int32_t HksGetOldKeyInfoListForSmallToService(const struct HksProcessInfo *processInfo, struct HksKeyInfo *keyInfoList,
    uint32_t listMaxCnt, uint32_t *listCount);

int32_t HksIsNeedUpgradeForSmallToService(const struct HksProcessInfo *processInfo);

#endif

#ifdef __cplusplus
}
#endif

#endif /* HKS_UPGRADE_OPERATION_H */
