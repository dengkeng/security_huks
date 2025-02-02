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

#include "hks_response.h"

#include <cinttypes>
#include <dlfcn.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "hks_at_api_wrap.h"
#ifdef HKS_CONFIG_FILE
#include HKS_CONFIG_FILE
#else
#include "hks_config.h"
#endif

#ifdef HKS_SUPPORT_ACCESS_TOKEN
#include "accesstoken_kit.h"
#endif
#include "ipc_skeleton.h"

#include "hks_base_check.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_template.h"
#include "hks_type_inner.h"
#include "hks_util.h"

#include "hap_token_info.h"
#ifdef HKS_SUPPORT_GET_BUNDLE_INFO
#include "hks_bms_api_wrap.h"
#endif

#ifdef HAS_OS_ACCOUNT_PART
#include "os_account_manager.h"
#endif // HAS_OS_ACCOUNT_PART

using namespace OHOS;

#ifndef HAS_OS_ACCOUNT_PART
constexpr static int UID_TRANSFORM_DIVISOR = 200000;
static void GetOsAccountIdFromUid(int uid, int &osAccountId)
{
    osAccountId = uid / UID_TRANSFORM_DIVISOR;
}
#endif // HAS_OS_ACCOUNT_PART

void HksSendResponse(const uint8_t *context, int32_t result, const struct HksBlob *response)
{
    if (context == nullptr) {
        HKS_LOG_E("SendResponse NULL Pointer");
        return;
    }

    MessageParcel *reply = const_cast<MessageParcel *>(reinterpret_cast<const MessageParcel *>(context));
    HKS_IF_NOT_TRUE_LOGE_RETURN_VOID(reply->WriteInt32(result));

    if (response == nullptr) {
        HKS_IF_NOT_TRUE_LOGE_RETURN_VOID(reply->WriteUint32(0));
    } else {
        HKS_IF_NOT_TRUE_LOGE_RETURN_VOID(reply->WriteUint32(response->size));
        HKS_IF_NOT_TRUE_LOGE_RETURN_VOID(reply->WriteBuffer(response->data, static_cast<size_t>(response->size)));
    }
}

int32_t HksGetProcessInfoForIPC(const uint8_t *context, struct HksProcessInfo *processInfo)
{
    if ((context == nullptr) || (processInfo == nullptr)) {
        HKS_LOG_D("Don't need get process name in hosp.");
        return HKS_SUCCESS;
    }

    auto callingUid = IPCSkeleton::GetCallingUid();
    uint8_t *name = static_cast<uint8_t *>(HksMalloc(sizeof(callingUid)));
    HKS_IF_NULL_LOGE_RETURN(name, HKS_ERROR_MALLOC_FAIL, "GetProcessName malloc failed.")

    (void)memcpy_s(name, sizeof(callingUid), &callingUid, sizeof(callingUid));
    processInfo->processName.size = sizeof(callingUid);
    processInfo->processName.data = name;
    processInfo->uidInt = callingUid;
    int userId = 0;
#ifdef HAS_OS_ACCOUNT_PART
    OHOS::AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(callingUid, userId);
#else // HAS_OS_ACCOUNT_PART
    GetOsAccountIdFromUid(callingUid, userId);
#endif // HAS_OS_ACCOUNT_PART

    HKS_LOG_I("Get callingUid = %" LOG_PUBLIC "d, userId = %" LOG_PUBLIC "d, sessionId = %" LOG_PUBLIC "u",
        callingUid, userId, g_sessionId);

    uint32_t size;
    if (userId == 0) {
        size = strlen("0");
    } else {
        size = sizeof(userId);
    }

    uint8_t *name1 = static_cast<uint8_t *>(HksMalloc(size));
    if (name1 == nullptr) {
        HKS_LOG_E("user id malloc failed.");
        HKS_FREE(name);
        processInfo->processName.data = nullptr;
        return HKS_ERROR_MALLOC_FAIL;
    }

    if (userId == 0) {
        (void)memcpy_s(name1, size, "0", size); /* ignore \0 at the end */
    } else {
        (void)memcpy_s(name1, size, &userId, size);
    }

    processInfo->userId.size = size;
    processInfo->userId.data = name1;
    processInfo->userIdInt = userId;

#ifdef HKS_SUPPORT_ACCESS_TOKEN
    processInfo->accessTokenId = static_cast<uint64_t>(IPCSkeleton::GetCallingTokenID());
#endif

    return HKS_SUCCESS;
}

int32_t HksGetFrontUserId(int32_t *outId)
{
#ifdef HAS_OS_ACCOUNT_PART
    std::vector<int> ids;
    int ret = OHOS::AccountSA::OsAccountManager::QueryActiveOsAccountIds(ids);
    if (ret != ERR_OK || ids.empty()) {
        HKS_LOG_E("QueryActiveOsAccountIds Failed!! ret = %" LOG_PUBLIC "d", ret);
        return HKS_FAILURE;
    }
    HKS_LOG_I("QueryActiveOsAccountIds success: FrontUserId= %" LOG_PUBLIC "d", ids[0]);
    *outId = ids[0];
#else // HAS_OS_ACCOUNT_PART
    *outId = -1;
    HKS_LOG_I("QueryActiveOsAccountIds, no os account part, set FrontUserId= -1");
#endif // HAS_OS_ACCOUNT_PART

    return HKS_SUCCESS;
}