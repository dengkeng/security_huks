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

#ifndef HKS_THREE_STAGE_TEST_COMMON_H
#define HKS_THREE_STAGE_TEST_COMMON_H

#include <securec.h>

#include "hks_api.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_param.h"
#include "hks_type.h"

#ifdef __cplusplus
extern "C" {
#endif

static const uint32_t TIMES = 4;
static const uint32_t MAX_UPDATE_SIZE = 64;
static const uint32_t MAX_OUTDATA_SIZE = MAX_UPDATE_SIZE * TIMES;

static const uint8_t g_nData2048[] = {
    0xc5, 0x35, 0x62, 0x48, 0xc4, 0x92, 0x87, 0x73, 0x0d, 0x42, 0x96, 0xfc, 0x7b, 0x11, 0x05, 0x06,
    0x0f, 0x8d, 0x66, 0xc1, 0x0e, 0xad, 0x37, 0x44, 0x92, 0x95, 0x2f, 0x6a, 0x55, 0xba, 0xec, 0x1d,
    0x54, 0x62, 0x0a, 0x4b, 0xd3, 0xc7, 0x05, 0xe4, 0x07, 0x40, 0xd9, 0xb7, 0xc2, 0x12, 0xcb, 0x9a,
    0x90, 0xad, 0xe3, 0x24, 0xe8, 0x5e, 0xa6, 0xf8, 0xd0, 0x6e, 0xbc, 0xd1, 0x69, 0x7f, 0x6b, 0xe4,
    0x2b, 0x4e, 0x1a, 0x65, 0xbb, 0x73, 0x88, 0x6b, 0x7c, 0xaf, 0x7e, 0xd0, 0x47, 0x26, 0xeb, 0xa5,
    0xbe, 0xd6, 0xe8, 0xee, 0x9c, 0xa5, 0x66, 0xa5, 0xc9, 0xd3, 0x25, 0x13, 0xc4, 0x0e, 0x6c, 0xab,
    0x50, 0xb6, 0x50, 0xc9, 0xce, 0x8f, 0x0a, 0x0b, 0xc6, 0x28, 0x69, 0xe9, 0x83, 0x69, 0xde, 0x42,
    0x56, 0x79, 0x7f, 0xde, 0x86, 0x24, 0xca, 0xfc, 0xaa, 0xc0, 0xf3, 0xf3, 0x7f, 0x92, 0x8e, 0x8a,
    0x12, 0x52, 0xfe, 0x50, 0xb1, 0x5e, 0x8c, 0x01, 0xce, 0xfc, 0x7e, 0xf2, 0x4f, 0x5f, 0x03, 0xfe,
    0xa7, 0xcd, 0xa1, 0xfc, 0x94, 0x52, 0x00, 0x8b, 0x9b, 0x7f, 0x09, 0xab, 0xa8, 0xa4, 0xf5, 0xb4,
    0xa5, 0xaa, 0xfc, 0x72, 0xeb, 0x17, 0x40, 0xa9, 0xee, 0xbe, 0x8f, 0xc2, 0xd1, 0x80, 0xc2, 0x0d,
    0x44, 0xa9, 0x59, 0x44, 0x59, 0x81, 0x3b, 0x5d, 0x4a, 0xde, 0xfb, 0xae, 0x24, 0xfc, 0xa3, 0xd9,
    0xbc, 0x57, 0x55, 0xc2, 0x26, 0xbc, 0x19, 0xa7, 0x9a, 0xc5, 0x59, 0xa3, 0xee, 0x5a, 0xef, 0x41,
    0x80, 0x7d, 0xf8, 0x5e, 0xc1, 0x1d, 0x32, 0x38, 0x41, 0x5b, 0xb6, 0x92, 0xb8, 0xb7, 0x03, 0x0d,
    0x3e, 0x59, 0x0f, 0x1c, 0xb3, 0xe1, 0x2a, 0x95, 0x1a, 0x3b, 0x50, 0x4f, 0xc4, 0x1d, 0xcf, 0x73,
    0x7c, 0x14, 0xca, 0xe3, 0x0b, 0xa7, 0xc7, 0x1a, 0x41, 0x4a, 0xee, 0xbe, 0x1f, 0x43, 0xdd, 0xf9,
};

static const uint8_t g_dData2048[] = {
    0x88, 0x4b, 0x82, 0xe7, 0xe3, 0xe3, 0x99, 0x75, 0x6c, 0x9e, 0xaf, 0x17, 0x44, 0x3e, 0xd9, 0x07,
    0xfd, 0x4b, 0xae, 0xce, 0x92, 0xc4, 0x28, 0x44, 0x5e, 0x42, 0x79, 0x08, 0xb6, 0xc3, 0x7f, 0x58,
    0x2d, 0xef, 0xac, 0x4a, 0x07, 0xcd, 0xaf, 0x46, 0x8f, 0xb4, 0xc4, 0x43, 0xf9, 0xff, 0x5f, 0x74,
    0x2d, 0xb5, 0xe0, 0x1c, 0xab, 0xf4, 0x6e, 0xd5, 0xdb, 0xc8, 0x0c, 0xfb, 0x76, 0x3c, 0x38, 0x66,
    0xf3, 0x7f, 0x01, 0x43, 0x7a, 0x30, 0x39, 0x02, 0x80, 0xa4, 0x11, 0xb3, 0x04, 0xd9, 0xe3, 0x57,
    0x23, 0xf4, 0x07, 0xfc, 0x91, 0x8a, 0xc6, 0xcc, 0xa2, 0x16, 0x29, 0xb3, 0xe5, 0x76, 0x4a, 0xa8,
    0x84, 0x19, 0xdc, 0xef, 0xfc, 0xb0, 0x63, 0x33, 0x0b, 0xfa, 0xf6, 0x68, 0x0b, 0x08, 0xea, 0x31,
    0x52, 0xee, 0x99, 0xef, 0x43, 0x2a, 0xbe, 0x97, 0xad, 0xb3, 0xb9, 0x66, 0x7a, 0xae, 0xe1, 0x8f,
    0x57, 0x86, 0xe5, 0xfe, 0x14, 0x3c, 0x81, 0xd0, 0x64, 0xf8, 0x86, 0x1a, 0x0b, 0x40, 0x58, 0xc9,
    0x33, 0x49, 0xb8, 0x99, 0xc6, 0x2e, 0x94, 0x70, 0xee, 0x09, 0x88, 0xe1, 0x5c, 0x4e, 0x6c, 0x22,
    0x72, 0xa7, 0x2a, 0x21, 0xdd, 0xd7, 0x1d, 0xfc, 0x63, 0x15, 0x0b, 0xde, 0x06, 0x9c, 0xf3, 0x28,
    0xf3, 0xac, 0x4a, 0xa8, 0xb5, 0x50, 0xca, 0x9b, 0xcc, 0x0a, 0x04, 0xfe, 0x3f, 0x98, 0x68, 0x81,
    0xac, 0x24, 0x53, 0xea, 0x1f, 0x1c, 0x6e, 0x5e, 0xca, 0xe8, 0x31, 0x0d, 0x08, 0x12, 0xf3, 0x26,
    0xf8, 0x5e, 0xeb, 0x10, 0x27, 0xae, 0xaa, 0xc3, 0xad, 0x6c, 0xc1, 0x89, 0xdb, 0x7d, 0x5a, 0x12,
    0x55, 0xad, 0x11, 0x19, 0xa1, 0xa9, 0x8f, 0x0b, 0x6d, 0x78, 0x8d, 0x1c, 0xdf, 0xe5, 0x63, 0x82,
    0x0b, 0x7d, 0x23, 0x04, 0xb4, 0x75, 0x8c, 0xed, 0x77, 0xfc, 0x1a, 0x85, 0x29, 0x11, 0xe0, 0x61,
};

static const uint8_t g_nData4096[] = {
    0xcd, 0x1f, 0x40, 0xcd, 0x37, 0x56, 0x2f, 0x78, 0xbb, 0x27, 0x1e, 0xba, 0x66, 0xba, 0x84, 0xc6,
    0xdb, 0xa0, 0x72, 0xf3, 0x1b, 0x53, 0x8c, 0x46, 0x99, 0x65, 0x0d, 0xf7, 0xb4, 0xae, 0x19, 0x46,
    0x1e, 0x05, 0xb0, 0xbc, 0xe1, 0x84, 0x9b, 0x44, 0xc5, 0x06, 0x18, 0xef, 0x68, 0x4b, 0x80, 0xf4,
    0xfc, 0x00, 0x79, 0x8e, 0x21, 0x5e, 0x8f, 0x26, 0x65, 0x69, 0x09, 0x31, 0x4b, 0xa0, 0x95, 0x37,
    0x53, 0xa2, 0xf5, 0x52, 0x78, 0xb5, 0x3c, 0xec, 0x58, 0x25, 0x1f, 0x23, 0x99, 0x89, 0xc1, 0x46,
    0x29, 0x65, 0xbb, 0xbe, 0x59, 0x4c, 0xe4, 0x6f, 0xfc, 0xeb, 0x53, 0xe5, 0x33, 0x6e, 0x9a, 0x14,
    0x57, 0x2a, 0x39, 0xc5, 0xec, 0x4e, 0xc3, 0x3e, 0xcf, 0x76, 0xa3, 0xe9, 0xf8, 0xe3, 0x8a, 0x40,
    0x1b, 0x3d, 0x07, 0x0e, 0xe4, 0x8e, 0xc7, 0x03, 0xbe, 0xf0, 0x3b, 0xb1, 0x25, 0x4d, 0x20, 0x07,
    0x9d, 0x2a, 0x18, 0x92, 0x4b, 0x62, 0x34, 0x73, 0xbc, 0x60, 0x3c, 0x18, 0xa2, 0xf8, 0x23, 0x3f,
    0x43, 0xa5, 0xd8, 0x0c, 0x3f, 0x36, 0xd3, 0x4a, 0x83, 0xe8, 0x93, 0xce, 0x55, 0x6b, 0x31, 0xeb,
    0x8e, 0x8d, 0xc0, 0x01, 0x96, 0xee, 0x4c, 0x15, 0xf1, 0x21, 0x5b, 0x34, 0xe9, 0x42, 0x1a, 0x4a,
    0x7b, 0x6f, 0x58, 0x0b, 0x44, 0x4b, 0xc6, 0x9e, 0x15, 0x1c, 0xb9, 0x49, 0x97, 0x99, 0xfc, 0x4b,
    0xac, 0x9e, 0xc8, 0xfe, 0x97, 0xf7, 0x56, 0x62, 0x7e, 0x8f, 0x0e, 0xd2, 0x8a, 0xc3, 0x65, 0x72,
    0xe8, 0xcd, 0xb0, 0xbb, 0x21, 0x95, 0xb5, 0x4a, 0x6e, 0x57, 0x13, 0xf9, 0x68, 0x9f, 0xab, 0x53,
    0xe8, 0xab, 0x10, 0x36, 0x6f, 0x0f, 0x53, 0x53, 0x0b, 0xcd, 0x9b, 0x16, 0x7f, 0x33, 0xfa, 0x45,
    0x2d, 0x52, 0x4d, 0x67, 0x5d, 0x4e, 0xcd, 0x67, 0xef, 0x32, 0x09, 0x09, 0xe6, 0x26, 0x37, 0x63,
    0x4e, 0x43, 0x4e, 0x70, 0xc1, 0xc8, 0xc7, 0xf1, 0x98, 0x81, 0xa3, 0x94, 0xa3, 0x3f, 0xe5, 0x40,
    0x00, 0x26, 0xf9, 0xef, 0x37, 0x57, 0xdc, 0xf4, 0x14, 0x52, 0x8e, 0x24, 0xf2, 0x80, 0xf7, 0x09,
    0x94, 0x29, 0x04, 0xc0, 0x93, 0xdc, 0xc4, 0xfb, 0x67, 0xa3, 0x93, 0x48, 0xfe, 0x3e, 0x50, 0x3e,
    0x40, 0xbf, 0xa5, 0xfb, 0x73, 0xf7, 0x72, 0xad, 0x1e, 0x79, 0x95, 0x3d, 0x52, 0x06, 0x6c, 0xc1,
    0xfa, 0x63, 0x92, 0xcd, 0xfc, 0xc8, 0x61, 0x9f, 0x8d, 0xc1, 0xc3, 0xce, 0x5d, 0x88, 0xcd, 0xf0,
    0xce, 0x82, 0xb4, 0x2d, 0x17, 0x01, 0x03, 0x74, 0xb1, 0x4c, 0x7c, 0xeb, 0x53, 0xd6, 0x2a, 0x4c,
    0x99, 0xb1, 0x17, 0x89, 0xb6, 0x50, 0x37, 0x51, 0xb4, 0xea, 0x54, 0x4f, 0x34, 0x5c, 0xc2, 0xa6,
    0xe7, 0x15, 0x6c, 0x35, 0xa4, 0x18, 0x76, 0x44, 0x52, 0xda, 0x66, 0x0c, 0x18, 0x49, 0x2f, 0x7e,
    0x54, 0x59, 0xfa, 0x9c, 0xbb, 0xc3, 0xf4, 0x57, 0x59, 0x53, 0x90, 0xca, 0x73, 0x62, 0xc6, 0xbd,
    0xd4, 0x37, 0x0f, 0x6b, 0xb8, 0x6f, 0xbf, 0x24, 0xd4, 0xef, 0xde, 0x98, 0x9c, 0x05, 0x30, 0xc5,
    0xa6, 0xb9, 0x39, 0xe8, 0x5b, 0xc5, 0x30, 0xb8, 0x18, 0xb0, 0x86, 0x33, 0x1f, 0x36, 0x64, 0xe7,
    0x18, 0xd5, 0x05, 0xf5, 0x7d, 0x23, 0xe7, 0xdd, 0x96, 0x1a, 0x82, 0xf4, 0xfd, 0x90, 0x86, 0x69,
    0xb5, 0x4b, 0x7b, 0xec, 0x4d, 0x1a, 0x78, 0xad, 0x6f, 0x55, 0x69, 0xc7, 0x8d, 0x69, 0x00, 0x8b,
    0x56, 0x8e, 0x4c, 0xef, 0x1d, 0xe3, 0xa8, 0xbd, 0xa8, 0x93, 0xfc, 0xc8, 0xc9, 0x1f, 0x4c, 0x0c,
    0x64, 0xc4, 0x42, 0x15, 0xdc, 0xfd, 0x57, 0x44, 0xe6, 0xf8, 0x83, 0xc5, 0xb5, 0x97, 0x2a, 0xd1,
    0x0d, 0x30, 0xb4, 0xef, 0xac, 0x7f, 0xc8, 0xcf, 0x90, 0x83, 0x89, 0xb3, 0x2c, 0xc7, 0xb0, 0x5b,
};

static const uint8_t g_dData4096[] = {
    0x80, 0x03, 0xc2, 0x48, 0x91, 0x01, 0x8b, 0xcc, 0xf8, 0x58, 0x58, 0xd8, 0x12, 0x66, 0xd3, 0x98,
    0xdb, 0xd6, 0xce, 0x06, 0xa5, 0x06, 0x46, 0x64, 0x85, 0x35, 0x49, 0x3b, 0x4f, 0x9a, 0xdc, 0x11,
    0x23, 0x89, 0x7f, 0x9c, 0xd1, 0xce, 0x15, 0xad, 0x1c, 0x9d, 0x4a, 0x90, 0x97, 0x71, 0x8f, 0xc0,
    0xd5, 0x49, 0x62, 0x93, 0x6e, 0x85, 0xfa, 0x27, 0x07, 0x61, 0x3f, 0x28, 0x71, 0xbf, 0x7d, 0x80,
    0xb3, 0x04, 0xdf, 0xc0, 0x21, 0xaf, 0x52, 0x9f, 0x59, 0xff, 0x88, 0xe8, 0x89, 0xd9, 0x2c, 0x33,
    0x22, 0x80, 0xe8, 0x63, 0xca, 0x8d, 0xaf, 0x7f, 0xa8, 0xc4, 0x21, 0x6e, 0xc8, 0xe3, 0x7c, 0xeb,
    0xb6, 0xde, 0x96, 0x81, 0xcc, 0x91, 0xf7, 0x4e, 0x4a, 0xe2, 0x0b, 0x38, 0x69, 0x69, 0x70, 0x8d,
    0xc3, 0xb2, 0x19, 0xa6, 0x94, 0xc3, 0xfa, 0xb6, 0x7e, 0xc2, 0xa5, 0x2e, 0x50, 0x4e, 0x4a, 0xf0,
    0x15, 0x89, 0x01, 0x1e, 0xc4, 0x47, 0x3c, 0xdf, 0x02, 0x30, 0xb9, 0x30, 0xd2, 0xfc, 0xb3, 0xda,
    0xb6, 0xd8, 0x58, 0xf6, 0x83, 0xb9, 0x2e, 0xb2, 0xb1, 0x92, 0x44, 0x34, 0xa0, 0xa0, 0xae, 0xde,
    0x08, 0xf0, 0xb4, 0x65, 0x09, 0x92, 0x1d, 0x10, 0x10, 0x96, 0x35, 0xba, 0xd6, 0xdc, 0xb7, 0x55,
    0xc6, 0xa5, 0x5d, 0xb8, 0x62, 0x5e, 0x3c, 0x4d, 0xca, 0x75, 0x4b, 0xb9, 0xff, 0x10, 0x4e, 0x21,
    0xd8, 0x50, 0x9c, 0xb6, 0x2f, 0x97, 0x60, 0x65, 0x46, 0x00, 0xf8, 0x4a, 0x33, 0xe9, 0xf0, 0x75,
    0x7f, 0x8f, 0x67, 0x7d, 0xd2, 0x5f, 0x01, 0x6e, 0xbb, 0x60, 0x43, 0x0a, 0x03, 0xde, 0xaa, 0x69,
    0x4a, 0xab, 0x00, 0x0a, 0x48, 0xd3, 0xb2, 0xb8, 0x00, 0x84, 0xc8, 0x06, 0x89, 0x4a, 0xa2, 0x25,
    0xec, 0x3c, 0x12, 0x2c, 0xb1, 0x52, 0x0d, 0xe5, 0x90, 0xa5, 0x94, 0x78, 0x86, 0x17, 0x51, 0xb6,
    0xdc, 0x1a, 0xd4, 0xc2, 0xf3, 0x4e, 0xa9, 0xf8, 0x90, 0x86, 0xd0, 0x34, 0xa6, 0x80, 0x8a, 0x40,
    0x13, 0xd2, 0xa1, 0x68, 0x27, 0xca, 0xfe, 0x66, 0xc5, 0x2c, 0x7f, 0x79, 0x60, 0x8d, 0x27, 0xac,
    0xbb, 0xa8, 0x61, 0xe0, 0xa0, 0x10, 0x64, 0x97, 0xb0, 0x0f, 0xa7, 0xef, 0xe8, 0x88, 0x09, 0xac,
    0x46, 0x80, 0xbe, 0x90, 0xa5, 0xe6, 0xc9, 0xba, 0x4b, 0x10, 0x91, 0x24, 0xa7, 0x02, 0x00, 0x5c,
    0x49, 0x16, 0x1d, 0xa1, 0x1f, 0xbf, 0x71, 0x36, 0x9e, 0xfd, 0x4b, 0xc7, 0x7e, 0x98, 0xee, 0x7b,
    0x97, 0xf3, 0xa5, 0x3f, 0x52, 0xc4, 0x87, 0x20, 0xdc, 0x14, 0xfa, 0xae, 0x34, 0xd2, 0xf5, 0x7a,
    0x26, 0x3a, 0x01, 0xba, 0x15, 0x52, 0xd0, 0x3d, 0xe4, 0x42, 0x2b, 0x5b, 0x96, 0x54, 0x41, 0xd6,
    0x29, 0x3f, 0x06, 0xf0, 0xb2, 0x6a, 0x59, 0x9a, 0x99, 0xa3, 0xa8, 0x40, 0x67, 0xba, 0x41, 0x42,
    0xce, 0x02, 0x1d, 0x48, 0xda, 0x91, 0x85, 0x5c, 0x87, 0x5a, 0xe7, 0xa4, 0x35, 0x26, 0xaa, 0x4f,
    0x04, 0xd4, 0x30, 0x18, 0x8c, 0xb1, 0x2f, 0x89, 0x5f, 0x3e, 0x49, 0x0c, 0x4d, 0x21, 0xe0, 0xd9,
    0x10, 0xb2, 0x5e, 0x66, 0x8f, 0x33, 0x96, 0xaf, 0x61, 0xff, 0xc6, 0x95, 0xd3, 0xb1, 0x8c, 0x71,
    0x1f, 0xc4, 0x95, 0x8f, 0xb8, 0x32, 0x0a, 0x71, 0x3f, 0xc5, 0xe8, 0xca, 0x88, 0xc7, 0xd3, 0xa1,
    0x25, 0xcb, 0xf8, 0x04, 0x04, 0x53, 0x28, 0xf2, 0xf0, 0xb5, 0xf5, 0x8f, 0xf8, 0xdc, 0x57, 0xb4,
    0xc8, 0x13, 0x80, 0x55, 0x33, 0xc1, 0xdd, 0x88, 0xcc, 0x37, 0xc9, 0xcd, 0xa3, 0x2b, 0x2b, 0x2c,
    0xe2, 0xdb, 0xd7, 0xca, 0x34, 0xe0, 0x0c, 0xb5, 0x3c, 0xa3, 0x12, 0xa6, 0x94, 0x68, 0xaa, 0x66,
    0xc7, 0x92, 0xe2, 0xde, 0x06, 0xbb, 0x48, 0xee, 0x27, 0x03, 0x7c, 0x0e, 0xf5, 0x51, 0xca, 0xd9,
};

static const uint8_t g_xData[] = {
    0x99, 0x97, 0x76, 0x67, 0x4a, 0xd1, 0x21, 0xe2, 0xbd, 0x75, 0xf3, 0x05, 0x34, 0xe6, 0xc2, 0x27,
    0xf9, 0x4b, 0xb0, 0x47, 0xb4, 0x7c, 0xc0, 0x16, 0x05, 0x2c, 0x93, 0xd2, 0xdf, 0xe6, 0x13, 0xa8,
};

static const uint8_t g_yData[] = {
    0x9b, 0xf4, 0x99, 0x33, 0x05, 0xf0, 0x0c, 0xa9, 0x88, 0xcc, 0xb0, 0x3e, 0x17, 0x6b, 0x67, 0xc7,
    0x8e, 0x31, 0x6d, 0xf7, 0x34, 0xfe, 0xdd, 0x76, 0x8e, 0x3d, 0xfa, 0x8c, 0x39, 0xdd, 0xce, 0xfa,
    0xa9, 0xe2, 0x7b, 0xc0, 0x01, 0x23, 0xf6, 0x62, 0x93, 0x77, 0x69, 0x91, 0x6e, 0xa5, 0x32, 0x46,
    0xbb, 0x95, 0x67, 0x81, 0xbd, 0xe8, 0xb1, 0xe4, 0xdf, 0xc8, 0x75, 0x99, 0x23, 0xc0, 0x29, 0x1b,
    0xc7, 0x23, 0x0b, 0xf2, 0x11, 0xb8, 0x0e, 0x59, 0x25, 0xe3, 0xa2, 0x3f, 0x22, 0xe4, 0xf6, 0x20,
    0xd2, 0xdf, 0x58, 0xd2, 0x99, 0xd8, 0x62, 0x9a, 0x24, 0x2e, 0x13, 0x4b, 0xea, 0xc9, 0x8c, 0xf9,
    0x90, 0x6d, 0xa4, 0x34, 0x27, 0x67, 0xee, 0x5c, 0x10, 0x63, 0x58, 0xc0, 0x96, 0x7b, 0x2d, 0x5b,
    0x6c, 0xcd, 0x0c, 0x7b, 0x18, 0xcc, 0x0d, 0x7d, 0x3a, 0xa7, 0x57, 0x9c, 0x12, 0x10, 0x8a, 0x3a,
    0xf4, 0x6b, 0xe4, 0x38, 0xba, 0x9c, 0xf4, 0x7c, 0x27, 0xeb, 0x06, 0x88, 0x35, 0x6b, 0xd7, 0x01,
    0xe3, 0x71, 0x48, 0x41, 0xa4, 0x1d, 0x9b, 0x1d, 0x06, 0x8a, 0x1f, 0x2c, 0x9d, 0xac, 0x81, 0x50,
    0x4d, 0x9b, 0x05, 0xcf, 0xa2, 0x3f, 0x5c, 0x84, 0x97, 0x82, 0xa6, 0x27, 0x77, 0x88, 0xfa, 0x8a,
    0x34, 0x8f, 0x23, 0xcb, 0x79, 0x51, 0x07, 0xfb, 0x43, 0xab, 0x75, 0xee, 0xd4, 0x0c, 0x6a, 0x4c,
    0x5e, 0x1a, 0xc1, 0xa1, 0xd7, 0x32, 0xf8, 0xbe, 0x2b, 0x82, 0xa1, 0x69, 0x4f, 0x24, 0x25, 0x24,
    0x35, 0x5d, 0xf1, 0x4e, 0x07, 0x0b, 0x0b, 0xb5, 0x79, 0x44, 0xcf, 0xea, 0xb6, 0xfa, 0x99, 0x60,
    0x34, 0x5d, 0xad, 0xca, 0x3a, 0x6c, 0x9e, 0xa1, 0x81, 0xe0, 0x51, 0xc2, 0x10, 0x0e, 0x6c, 0x61,
    0x78, 0x07, 0x94, 0x2f, 0xca, 0x0e, 0x04, 0x48, 0x01, 0xaf, 0x29, 0x17, 0x3e, 0x95, 0x99, 0x2d,
};

static const uint8_t g_pData[] = {
    0xa3, 0x45, 0xc9, 0x21, 0x8a, 0xce, 0x76, 0xf9, 0x52, 0x6e, 0x49, 0x2f, 0x25, 0x05, 0xa9, 0xde,
    0xdb, 0xd7, 0x6b, 0x39, 0x59, 0x9c, 0x73, 0xe4, 0xf0, 0xe9, 0x39, 0xd0, 0xb5, 0x12, 0x3d, 0xc0,
    0xb5, 0x2a, 0x08, 0xf3, 0x0a, 0xa4, 0xfc, 0x4a, 0xdb, 0x00, 0x83, 0x85, 0x05, 0xd2, 0xea, 0x9e,
    0x0b, 0xa9, 0x1f, 0x68, 0x77, 0xd6, 0x13, 0x82, 0xdb, 0xf9, 0x52, 0xe5, 0xae, 0x26, 0x37, 0x2d,
    0x8b, 0xbd, 0x2e, 0x1d, 0x9e, 0x0c, 0xf2, 0xab, 0x1c, 0xe9, 0x78, 0xad, 0x76, 0x16, 0x55, 0x12,
    0x81, 0xe0, 0xc9, 0x99, 0x43, 0x9e, 0xf9, 0x78, 0x31, 0x27, 0x7d, 0x13, 0xd7, 0x05, 0x8c, 0xd7,
    0xef, 0x47, 0xc4, 0x9a, 0xc5, 0xa2, 0x99, 0x2a, 0x6a, 0xa0, 0xd8, 0xc7, 0x26, 0xde, 0xf6, 0xa2,
    0x24, 0x12, 0x66, 0x4e, 0x90, 0xa1, 0xe2, 0x9e, 0xaa, 0xb3, 0x40, 0x93, 0x2a, 0x9d, 0x23, 0x85,
    0xec, 0x64, 0xa6, 0x20, 0xdb, 0xe4, 0xbe, 0x8e, 0xe5, 0x4e, 0xec, 0xf6, 0xa8, 0xca, 0x9b, 0x0d,
    0x3e, 0x34, 0x5c, 0x7a, 0xb7, 0x38, 0x2d, 0x41, 0x48, 0x0c, 0x03, 0xc3, 0x55, 0xfa, 0x10, 0x81,
    0x62, 0xc4, 0x08, 0x30, 0xf5, 0x39, 0x4b, 0x5f, 0x32, 0x22, 0x50, 0x6f, 0x9f, 0xcb, 0xb2, 0x8a,
    0x9e, 0x45, 0x71, 0xe2, 0xec, 0xc5, 0x67, 0xeb, 0x3c, 0xe5, 0x8f, 0x16, 0x44, 0x19, 0xe3, 0x2d,
    0xa5, 0x8f, 0xd7, 0xdb, 0x40, 0x3a, 0x17, 0xa9, 0x0c, 0x19, 0x8b, 0x00, 0x69, 0x22, 0x8d, 0x3f,
    0x52, 0x9f, 0x43, 0x06, 0xd5, 0x5d, 0x79, 0x60, 0xa9, 0xc2, 0xd4, 0x3c, 0x1e, 0x81, 0x05, 0x6e,
    0x37, 0x77, 0x82, 0x51, 0x25, 0x74, 0x6a, 0x99, 0xaa, 0xb0, 0xf2, 0x4f, 0x40, 0x2b, 0x29, 0x3f,
    0x34, 0x9c, 0x97, 0x42, 0x63, 0x9c, 0x49, 0xe8, 0x09, 0x3a, 0xbf, 0x26, 0x8c, 0xcb, 0x7e, 0x11,
};

static const uint8_t g_qData[] = {
    0xc4, 0x59, 0x2b, 0xa5, 0xe1, 0x19, 0x89, 0xf2, 0x2c, 0xde, 0x54, 0x08, 0xd3, 0xfa, 0xd3, 0x37,
    0x40, 0xd3, 0xc0, 0x88, 0xe1, 0x08, 0xf0, 0x06, 0xd4, 0x65, 0x82, 0x46, 0xbe, 0xa1, 0x82, 0xdd,
};

static const uint8_t g_gData[] = {
    0x1c, 0x2d, 0x79, 0x2c, 0x1e, 0x9d, 0x9a, 0x69, 0xe3, 0x59, 0xa6, 0xc9, 0xbe, 0xb3, 0x55, 0x2e,
    0x9d, 0xc6, 0xfd, 0x77, 0x33, 0xfe, 0x61, 0x31, 0x6a, 0x2d, 0xcf, 0x12, 0xb9, 0x1f, 0x58, 0x4a,
    0xac, 0xd4, 0xbc, 0xd0, 0xf7, 0xb7, 0x16, 0x3a, 0x6b, 0x80, 0x3e, 0x22, 0x74, 0x81, 0xde, 0x51,
    0x29, 0x95, 0xe6, 0x28, 0x0c, 0xaf, 0x64, 0xba, 0x11, 0x0e, 0x1a, 0xae, 0xb5, 0xb8, 0x40, 0x59,
    0xed, 0x75, 0x5a, 0x5e, 0x54, 0x0b, 0xba, 0x8a, 0xb2, 0x14, 0x23, 0xb0, 0xec, 0x68, 0x18, 0xa1,
    0xd2, 0xfa, 0x2d, 0x16, 0x15, 0xab, 0x1f, 0x0b, 0x18, 0x32, 0x93, 0xc2, 0xd0, 0x54, 0xe0, 0x37,
    0x6f, 0xff, 0x0d, 0x67, 0x4a, 0x90, 0x41, 0x06, 0x33, 0xfc, 0xab, 0xf8, 0xdc, 0x1e, 0x16, 0xf2,
    0x06, 0x93, 0xe3, 0x52, 0x18, 0x46, 0x5e, 0xe3, 0x7f, 0xba, 0x98, 0x56, 0x89, 0x0b, 0xce, 0x0b,
    0xba, 0x01, 0xe2, 0x66, 0x71, 0x85, 0x2a, 0x32, 0x43, 0x9d, 0x48, 0xaf, 0xb9, 0xe4, 0xd3, 0xc8,
    0xdc, 0x3a, 0x8e, 0xb4, 0xf0, 0xa5, 0x11, 0xd4, 0x5f, 0xbf, 0x65, 0x62, 0x76, 0x4c, 0x30, 0xfb,
    0x29, 0x1c, 0x15, 0xa6, 0x16, 0x8a, 0x7f, 0x17, 0x56, 0x40, 0x79, 0x33, 0xd4, 0x91, 0x29, 0xf1,
    0x39, 0x8d, 0xfd, 0x48, 0x97, 0x84, 0xc6, 0x42, 0x1e, 0x83, 0xd2, 0xe5, 0xf9, 0xa4, 0x26, 0x3c,
    0xb8, 0x6a, 0xce, 0x8b, 0xb5, 0x0d, 0xd8, 0x72, 0x38, 0x3b, 0x65, 0xc8, 0x7c, 0x01, 0xf7, 0x6d,
    0x8d, 0x50, 0x87, 0xc2, 0xce, 0x55, 0xfb, 0xe4, 0xf9, 0xe2, 0x98, 0x28, 0x9e, 0x05, 0xdf, 0x28,
    0xcc, 0x0f, 0xe3, 0x54, 0x64, 0x36, 0x2d, 0xa4, 0x7c, 0x5a, 0x0c, 0xcd, 0xe0, 0x51, 0x8f, 0x38,
    0x3a, 0xe7, 0x82, 0x3a, 0x62, 0x69, 0xce, 0xee, 0x53, 0x2a, 0x7c, 0xec, 0x46, 0x5c, 0x51, 0x33,
};

int32_t InitParamSet(struct HksParamSet **paramSet, const struct HksParam *params, uint32_t paramcount);
int32_t HksTestUpdate(const struct HksBlob *handle, const struct HksParamSet *paramSet, const struct HksBlob *inData);
int32_t TestUpdateLoopFinish(const struct HksBlob *handle, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData);
int32_t TestUpdateFinish(const struct HksBlob *handle, const struct HksParamSet *paramSet,
    uint32_t purpose, const struct HksBlob *inData, struct HksBlob *outData);
int32_t MallocAndCheckBlobData(struct HksBlob *blob, const uint32_t blobSize);
int32_t TestCmpKeyAliasHash(const struct HksBlob *srcData1, const struct HksBlob *srcData2);

#ifdef __cplusplus
}
#endif

#endif // HKS_THREE_STAGE_TEST_COMMON_H