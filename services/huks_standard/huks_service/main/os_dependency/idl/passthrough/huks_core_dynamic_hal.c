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

#ifdef HKS_CONFIG_FILE
#include HKS_CONFIG_FILE
#else
#include "hks_config.h"
#endif

#include "huks_core_hal.h"

#include <dlfcn.h>

#include "hks_cfi.h"
#include "hks_log.h"
#include "hks_template.h"

typedef struct HuksHdi *(*HalCreateHandle)(void);
typedef void (*HalDestroyHandle)(struct HuksHdi *);

void *g_halDeviceHandle = NULL;

ENABLE_CFI(int32_t HksCreateHuksHdiDevice(struct HuksHdi **halDevice))
{
    HKS_IF_NULL_LOGE_RETURN(halDevice, HKS_ERROR_NULL_POINTER, "invalid input halDevice")

    if (*halDevice != NULL) {
        return HKS_SUCCESS;
    }

#ifdef HKS_L1_SMALL
    g_halDeviceHandle = dlopen("libhuks_engine_core_standard.so", RTLD_NOW);
#else
    g_halDeviceHandle = dlopen("libhuks_engine_core_standard.z.so", RTLD_NOW);
#endif

    HKS_IF_NULL_LOGE_RETURN(g_halDeviceHandle, HKS_FAILURE, "dlopen failed, %" LOG_PUBLIC "s!", dlerror())

    HalCreateHandle devicePtr = (HalCreateHandle)dlsym(g_halDeviceHandle, "HuksCreateHdiDevicePtr");
    if (devicePtr == NULL) {
        HKS_LOG_E("dlsym failed, %" LOG_PUBLIC "s!", dlerror());
        dlclose(g_halDeviceHandle);
        return HKS_ERROR_NULL_POINTER;
    }

    *halDevice = (*devicePtr)();
    if (*halDevice == NULL) {
        HKS_LOG_E("g_hksHalDevicePtr is NULL!");
        dlclose(g_halDeviceHandle);
        return HKS_ERROR_NULL_POINTER;
    }

    return HKS_SUCCESS;
}

ENABLE_CFI(int32_t HksDestroyHuksHdiDevice(struct HuksHdi **halDevice))
{
    if ((halDevice == NULL) || (*halDevice == NULL)) {
        return HKS_SUCCESS;
    }

    HKS_IF_NULL_LOGE_RETURN(g_halDeviceHandle, HKS_ERROR_NULL_POINTER, "g_halDeviceHandle is NULL!")

    HalDestroyHandle halDestroyHandle = (HalDestroyHandle)dlsym(g_halDeviceHandle, "HuksDestoryHdiDevicePtr");
    (*halDestroyHandle)(*halDevice);
    *halDevice = NULL;

    dlclose(g_halDeviceHandle);
    return HKS_SUCCESS;
}
