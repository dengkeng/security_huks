/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef HKS_RKC_RW_H
#define HKS_RKC_RW_H

#include "hks_type_inner.h"

#define HKS_RKC_VER 2                                   /* the version of root key component */
#define HKS_MK_VER 2                                    /* the version of main key */
#define HKS_RKC_RMK_ITER 1                              /* the iterator number of times which derive Root Main Key */
#define HKS_RKC_RMK_HMAC_SHA256 1                       /* the hash algorithm which derive Root Main Key */
#define HKS_RKC_MK_CRYPT_ALG_AES256_GCM 1               /* the encrypt algorithm of main key */
#define HKS_RKC_CFG_RSV_LEN 32                          /* the reserve length of the system configuration */
#define HKS_RKC_KSF_FLAG_LEN 8                          /* the length of keystore file flag */
#define HKS_RKC_MATERIAL_LEN 32                         /* the material length of root key component */
#define HKS_RKC_SALT_LEN 32                             /* the salt length of root key component */
#define HKS_RKC_KSF_DATA_RSV_LEN 8                      /* the reserve length of the keystore file data */
#define HKS_RKC_MK_IV_LEN 16                            /* the tag length of main key */
#define HKS_RKC_MK_CIPHER_TEXT_LEN 48                   /* the cipher length of main key */
#define HKS_RKC_MK_LEN 32                               /* the length of main key */
#define HKS_KSF_NUM 2                                   /* the keystore file number of rkc or mk */

/* the storage type of root key component */
enum HksRkcStorageType {
    HKS_RKC_STORAGE_FILE_SYS = 0,                       /* file system */
    HKS_RKC_STORAGE_FLASH = 1,                          /* flash */
};

/* the state of root key component */
enum HksRkcState {
    HKS_RKC_STATE_INVALID = 0,
    HKS_RKC_STATE_VALID = 1,
};

/* the type of keystore file */
enum HksKsfType {
    HKS_KSF_TYPE_RKC = 0,
    HKS_KSF_TYPE_MK = 1,
}

/* time */
struct HksTime {
    uint16_t hksYear;
    uint8_t hksMon;
    uint8_t hksDay;
    uint8_t hksHour;
    uint8_t hksMin;
    uint8_t hksSec;
};

/* the attribute of rkc or mk keystore file */
// struct HksKsfAttr {
//     uint8_t num;                                        /* the number of files */
//     char *name[HKS_KSF_NUM];
// };

struct HksKsfAttr {
    char *name[HKS_KSF_NUM];
};

/* the fields of root key component */
struct HksKsfDataRkc {
    struct HksTime rkCreatedTime;                       /* the created time of root key */
    struct HksTime rkExpiredTime;                       /* the expired time of root key */
    uint8_t rkMaterial1[HKS_RKC_MATERIAL_LEN];          /* the first material of root key */
    uint8_t rkMaterial2[HKS_RKC_MATERIAL_LEN];          /* the second material of root key */
    uint32_t rmkIter;                                   /* the iterator number of times which derive root main key */
    uint8_t rmkSalt[HKS_RKC_SALT_LEN];                  /* the salt which derive root main key */
    uint32_t rmkHashAlg;                                /* the hash algorithm which derive root main key */
    uint8_t rkRsv[HKS_RKC_KSF_DATA_RSV_LEN];            /* mk_rsv data for root key, 32 byte */
}

/* the fields of main key */
struct HksKsfDataMk {
    struct HksTime mkCreatedTime;                       /* the created time of main key */
    struct HksTime mkExpiredTime;                       /* the expired time of main key */
    uint32_t mkEncryptAlg;                              /* the encrption algorithm of main key */
    uint8_t mkIv[HKS_RKC_MK_IV_LEN];                    /* the IV of main key */
    uint8_t mkCiphertext[HKS_RKC_MK_CIPHER_TEXT_LEN];   /* the ciphertext of main key */
    uint8_t mkRsv[HKS_RKC_KSF_DATA_RSV_LEN];            /* mk_rsv data for main key, 32 byte */
}

/* todo: separate code of old version using macro */
/* the keystore file data of root key component (old struct) */
struct HksRkcKsfData {
    uint16_t version;                                   /* version */
    struct HksKsfDataRkc ksfDataRkc;                    /* fields of root key */
    struct HksKsfDataMk ksfDataMk;                      /* fields of main key */
};

/* the keystore file data of root key (new struct) */
struct HksKsfDataRkcWithVer {
    uint16_t rkVersion;                                 /* the version of root key */
    struct HksKsfDataRkc ksfDataRkc;                    /* fields of root key */
}

/* the keystore file data of main key (new struct) */
struct HksKsfDataMkWithVer {
    uint16_t mkVersion;                                 /* the version of main key */
    struct HksKsfDataMk ksfDataMk;                      /* fields of main key */
}

/* the configuration of root key component */
// struct HksRkcCfg {
//     uint8_t state;                                      /* system state */
//     uint16_t rkVersion;                                 /* the version of root key component */
//     uint16_t mkVersion;                                 /* the version of main key */
//     uint8_t storageType;                                /* the storage type of root key component */
//     struct HksTime rkCreatedTime;                       /* the created time of root key */
//     struct HksTime rkExpiredTime;                       /* the expired time of root key */
//     struct HksKsfAttr ksfAttrRkc;                       /* the attribute of rkc keystore file */
//     struct HksKsfAttr ksfAttrMk;                        /* the attribute of mk keystore file */
//     uint32_t rmkIter;                                   /* the iterator number of times which derive Root Main Key */
//     uint32_t rmkHashAlg;                                /* the hash algorithm which derive Root Main Key */
//     uint8_t mkMask[HKS_RKC_MK_LEN];                     /* the mask of main key */
//     uint32_t mkEncryptAlg;                              /* the encrption algorithm of main key */
//     uint8_t reserve[HKS_RKC_CFG_RSV_LEN];               /* reserve data, 32 byte */
// };

// extern struct HksRkcCfg g_hksRkcCfg;

#ifdef __cplusplus
extern "C" {
#endif

/* todo: separate code of old version using macro */
int32_t HksRkcReadKsf(const char *ksfName, struct HksRkcKsfData *ksfData);

int32_t HksReadKsfRkc(const char *ksfName, struct HksKsfDataRkcWithVer *ksfDataRkc);

int32_t HksWriteKsfRkc(const char *ksfName, const struct HksKsfDataRkcWithVer *ksfDataRkc);

int32_t HksReadKsfMk(const char *ksfName, struct HksKsfDataMkWithVer *ksfDataMk);

int32_t HksWriteKsfMk(const char *ksfName, const struct HksKsfDataMkWithVer *ksfDataMk);

bool KsfExist(uint8_t ksfType);

#ifdef __cplusplus
}
#endif

#endif /* HKS_RKC_RW_H */
