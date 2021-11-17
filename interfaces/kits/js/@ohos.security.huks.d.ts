
/*
* Copyright (c) 2021 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

import {AsyncCallback} from './basic';

declare namespace huks {
    function generateKey(keyAlias: string, options: HksOptions, callback: AsyncCallback<HksResult>) : void;
    function generateKey(keyAlias: string, options: HksOptions) : Promise<HksResult>;

    function deleteKey(keyAlias: string, options: HksOptions, callback: AsyncCallback<HksResult>) : void;
    function deleteKey(keyAlias: string, options: HksOptions) : Promise<HksResult>;

    function encrypt(keyAlias: string, options: HksOptions, callback: AsyncCallback<HksResult>) : void;
    function encrypt(keyAlias: string, options: HksOptions) : Promise<HksResult>;

    function decrypt(keyAlias: string, options: HksOptions, callback: AsyncCallback<HksResult>) : void;
    function decrypt(keyAlias: string, options: HksOptions) : Promise<HksResult>;

    function getSdkVersion(options: HksOptions) : string;

    function importKey(keyAlias: string, options: HksOptions, callback: AsyncCallback<HksResult>) : void;
    function importKey(keyAlias: string, options: HksOptions) : Promise<HksResult>;

    function exportKey(keyAlias: string, options: HksOptions, callback: AsyncCallback<HksResult>) : void;
    function exportKey(keyAlias: string, options: HksOptions) : Promise<HksResult>;

    function getKeyProperties(keyAlias: string, options: HksOptions, callback: AsyncCallback<HksResult>) : void;
    function getKeyProperties(keyAlias: string, options: HksOptions) : Promise<HksResult>;

    function isKeyExist(keyAlias: string, options: HksOptions, callback: AsyncCallback<boolean>) : void;
    function isKeyExist(keyAlias: string, options: HksOptions) : Promise<boolean>;

    function sign(keyAlias: string, options: HksOptions, callback: AsyncCallback<HksResult>) : void;
    function sign(keyAlias: string, options: HksOptions) : Promise<HksResult>;

    function verify(
        keyAlias: string, options: HksOptions, signature: Uint8Array, callback: AsyncCallback<HksResult>) : void;
    function verify(keyAlias: string, options: HksOptions, signature: Uint8Array) : Promise<HksResult>;

    function agreeKey(keyAlias: string, options: HksOptions, callback: AsyncCallback<HksResult>) : void;
    function agreeKey(keyAlias: string, options: HksOptions) : Promise<HksResult>;

    function deriveKey(keyAlias: string, options: HksOptions, callback: AsyncCallback<HksResult>) : void;
    function deriveKey(keyAlias: string, options: HksOptions) : Promise<HksResult>;

    function mac(keyAlias: string, options: HksOptions, callback: AsyncCallback<HksResult>) : void;
    function mac(keyAlias: string, options: HksOptions) : Promise<HksResult>;

    function attestKey(keyAlias: string, options: HksOptions, callback: AsyncCallback<HksResult>) : void;
    function attestKey(keyAlias: string, options: HksOptions) : Promise<HksResult>;

    function getCertificateChain(keyAlias: string, options: HksOptions, callback: AsyncCallback<HksResult>) : void;
    function getCertificateChain(keyAlias: string, options: HksOptions) : Promise<HksResult>;

    function wrapKey(keyAlias: string, targetKeyAlias: string, options: HksOptions, callback: AsyncCallback<HksResult>) : void;
    function wrapKey(keyAlias: string, targetKeyAlias: string, options: HksOptions) : Promise<HksResult>;

    function unwrapKey(keyAlias: string, targetKeyAlias: string, options: HksOptions, callback: AsyncCallback<HksResult>) : void;
    function unwrapKey(keyAlias: string, targetKeyAlias: string, options: HksOptions) : Promise<HksResult>;

    export interface HksParam {
        tag: HksTag;
        value: boolean | number | bigint | Uint8Array;
    }

    export interface HksOptions {
        properties?: Array<HksParam>;
        inData?: Uint8Array;
    }

    export interface HksResult {
        errorCode: number;
        outData?: Uint8Array;
        properties?: Array<HksParam>;
        certChains?: Array<string>;
    }

    export enum HksErrorCode {
        HKS_SUCCESS = 0,
        HKS_FAILURE = -1,
        HKS_ERROR_BAD_STATE = -2,
        HKS_ERROR_INVALID_ARGUMENT = -3,
        HKS_ERROR_NOT_SUPPORTED = -4,
        HKS_ERROR_NO_PERMISSION = -5,
        HKS_ERROR_INSUFFICIENT_DATA = -6,
        HKS_ERROR_BUFFER_TOO_SMALL = -7,
        HKS_ERROR_INSUFFICIENT_MEMORY = -8,
        HKS_ERROR_COMMUNICATION_FAILURE = -9,
        HKS_ERROR_STORAGE_FAILURE = -10,
        HKS_ERROR_HARDWARE_FAILURE = -11,
        HKS_ERROR_ALREADY_EXISTS = -12,
        HKS_ERROR_NOT_EXIST = -13,
        HKS_ERROR_NULL_POINTER = -14,
        HKS_ERROR_FILE_SIZE_FAIL = -15,
        HKS_ERROR_READ_FILE_FAIL = -16,
        HKS_ERROR_INVALID_PUBLIC_KEY = -17,
        HKS_ERROR_INVALID_PRIVATE_KEY = -18,
        HKS_ERROR_INVALID_KEY_INFO = -19,
        HKS_ERROR_HASH_NOT_EQUAL = -20,
        HKS_ERROR_MALLOC_FAIL = -21,
        HKS_ERROR_WRITE_FILE_FAIL = -22,
        HKS_ERROR_REMOVE_FILE_FAIL = -23,
        HKS_ERROR_OPEN_FILE_FAIL = -24,
        HKS_ERROR_CLOSE_FILE_FAIL = -25,
        HKS_ERROR_MAKE_DIR_FAIL = -26,
        HKS_ERROR_INVALID_KEY_FILE = -27,
        HKS_ERROR_IPC_MSG_FAIL = -28,
        HKS_ERROR_REQUEST_OVERFLOWS = -29,
        HKS_ERROR_PARAM_NOT_EXIST = -30,
        HKS_ERROR_CRYPTO_ENGINE_ERROR = -31,
        HKS_ERROR_COMMUNICATION_TIMEOUT = -32,
        HKS_ERROR_IPC_INIT_FAIL = -33,
        HKS_ERROR_IPC_DLOPEN_FAIL = -34,
        HKS_ERROR_EFUSE_READ_FAIL = -35,
        HKS_ERROR_NEW_ROOT_KEY_MATERIAL_EXIST = -36,
        HKS_ERROR_UPDATE_ROOT_KEY_MATERIAL_FAIL = -37,

        HKS_ERROR_CHECK_GET_ALG_FAIL = -100,
        HKS_ERROR_CHECK_GET_KEY_SIZE_FAIL = -101,
        HKS_ERROR_CHECK_GET_PADDING_FAIL = -102,
        HKS_ERROR_CHECK_GET_PURPOSE_FAIL = -103,
        HKS_ERROR_CHECK_GET_DIGEST_FAIL =  -104,
        HKS_ERROR_CHECK_GET_MODE_FAIL = -105,
        HKS_ERROR_CHECK_GET_NONCE_FAIL = -106,
        HKS_ERROR_CHECK_GET_AAD_FAIL = -107,
        HKS_ERROR_CHECK_GET_IV_FAIL = -108,
        HKS_ERROR_CHECK_GET_AE_TAG_FAIL = -109,
        HKS_ERROR_CHECK_GET_SALT_FAIL = -110,
        HKS_ERROR_CHECK_GET_ITERATION_FAIL = -111,
        HKS_ERROR_INVALID_ALGORITHM = -112,
        HKS_ERROR_INVALID_KEY_SIZE = -113,
        HKS_ERROR_INVALID_PADDING = -114,
        HKS_ERROR_INVALID_PURPOSE = -115,
        HKS_ERROR_INVALID_MODE = -116,
        HKS_ERROR_INVALID_DIGEST =  -117,
        HKS_ERROR_INVALID_SIGNATURE_SIZE = -118,
        HKS_ERROR_INVALID_IV = -119,
        HKS_ERROR_INVALID_AAD = -120,
        HKS_ERROR_INVALID_NONCE = -121,
        HKS_ERROR_INVALID_AE_TAG = -122,
        HKS_ERROR_INVALID_SALT = -123,
        HKS_ERROR_INVALID_ITERATION = -124,
        HKS_ERROR_INVALID_OPERATION = -125,

        HKS_ERROR_INTERNAL_ERROR = -999,
        HKS_ERROR_UNKNOWN_ERROR = -1000,
    }

    export enum HksKeyType {
        HKS_KEY_TYPE_RSA_PUBLIC_KEY = 0x01001000,
        HKS_KEY_TYPE_RSA_KEYPAIR = 0x01002000,

        HKS_KEY_TYPE_ECC_P256_PUBLIC_KEY = 0x02021000,
        HKS_KEY_TYPE_ECC_P256_KEYPAIR = 0x02022000,
        HKS_KEY_TYPE_ECC_P384_PUBLIC_KEY = 0x02031000,
        HKS_KEY_TYPE_ECC_P384_KEYPAIR = 0x02032000,
        HKS_KEY_TYPE_ECC_P521_PUBLIC_KEY = 0x02051000,
        HKS_KEY_TYPE_ECC_P521_KEYPAIR = 0x02052000,

        HKS_KEY_TYPE_ED25519_PUBLIC_KEY = 0x02101000,
        HKS_KEY_TYPE_ED25519_KEYPAIR = 0x02102000,
        HKS_KEY_TYPE_X25519_PUBLIC_KEY = 0x02111000,
        HKS_KEY_TYPE_X25519_KEYPAIR = 0x02112000,

        HKS_KEY_TYPE_AES = 0x03000000,
        HKS_KEY_TYPE_CHACHA20 = 0x04010000,
        HKS_KEY_TYPE_CHACHA20_POLY1305 = 0x04020000,

        HKS_KEY_TYPE_HMAC = 0x05000000,
        HKS_KEY_TYPE_HKDF = 0x06000000,
        HKS_KEY_TYPE_PBKDF2 = 0x07000000,
    }

    export enum HksKeyPurpose {
        HKS_KEY_PURPOSE_ENCRYPT = 1,                   /* Usable with RSA, EC and AES keys. */
        HKS_KEY_PURPOSE_DECRYPT = 2,                   /* Usable with RSA, EC and AES keys. */
        HKS_KEY_PURPOSE_SIGN = 4,                      /* Usable with RSA, EC keys. */
        HKS_KEY_PURPOSE_VERIFY = 8,                    /* Usable with RSA, EC keys. */
        HKS_KEY_PURPOSE_DERIVE = 16,                   /* Usable with EC keys. */
        HKS_KEY_PURPOSE_WRAP = 32,                     /* Usable with wrap key. */
        HKS_KEY_PURPOSE_UNWRAP = 64,                   /* Usable with unwrap key. */
        HKS_KEY_PURPOSE_MAC = 128,                     /* Usable with mac. */
        HKS_KEY_PURPOSE_AGREE = 256,                   /* Usable with agree. */
    }

    export enum HksKeyDigest {
        HKS_DIGEST_NONE = 0,
        HKS_DIGEST_MD5 = 1,
        HKS_DIGEST_SHA1 = 10,
        HKS_DIGEST_SHA224 = 11,
        HKS_DIGEST_SHA256 = 12,
        HKS_DIGEST_SHA384 = 13,
        HKS_DIGEST_SHA512 = 14,
    }

    export enum HksKeyPadding {
        HKS_PADDING_NONE = 0,
        HKS_PADDING_OAEP = 1,
        HKS_PADDING_PSS = 2,
        HKS_PADDING_PKCS1_V1_5 = 3,
        HKS_PADDING_PKCS5 = 4,
        HKS_PADDING_PKCS7 = 5,
    }

    export enum HksCipherMode {
        HKS_MODE_ECB = 1,
        HKS_MODE_CBC = 2,
        HKS_MODE_CTR = 3,
        HKS_MODE_OFB = 4,
        HKS_MODE_CCM = 31,
        HKS_MODE_GCM = 32,
    }

    export enum HksKeySize {
        HKS_RSA_KEY_SIZE_512 = 512,
        HKS_RSA_KEY_SIZE_768 = 768,
        HKS_RSA_KEY_SIZE_1024 = 1024,
        HKS_RSA_KEY_SIZE_2048 = 2048,
        HKS_RSA_KEY_SIZE_3072 = 3072,
        HKS_RSA_KEY_SIZE_4096 = 4096,

        HKS_ECC_KEY_SIZE_224 = 224,
        HKS_ECC_KEY_SIZE_256 = 256,
        HKS_ECC_KEY_SIZE_384 = 384,
        HKS_ECC_KEY_SIZE_521 = 521,

        HKS_AES_KEY_SIZE_128 = 128,
        HKS_AES_KEY_SIZE_192 = 192,
        HKS_AES_KEY_SIZE_256 = 256,
        HKS_AES_KEY_SIZE_512 = 512,

        HKS_CURVE25519_KEY_SIZE_256 = 256,

        HKS_DH_KEY_SIZE_2048 = 2048,
        HKS_DH_KEY_SIZE_3072 = 3072,
        HKS_DH_KEY_SIZE_4096 = 4096,
    }

    export enum HksKeyAlg {
        HKS_ALG_RSA = 1,
        HKS_ALG_ECC = 2,
        HKS_ALG_DSA = 3,

        HKS_ALG_AES = 20,
        HKS_ALG_HMAC = 50,
        HKS_ALG_HKDF = 51,
        HKS_ALG_PBKDF2 = 52,

        HKS_ALG_ECDH = 100,
        HKS_ALG_X25519 = 101,
        HKS_ALG_ED25519 = 102,
        HKS_ALG_DH = 103,
    }

    export enum HksKeyGenerateType {
        HKS_KEY_GENERATE_TYPE_DEFAULT = 0,
        HKS_KEY_GENERATE_TYPE_DERIVE = 1,
        HKS_KEY_GENERATE_TYPE_AGREE = 2,
    }

    export enum HksKeyFlag {
        HKS_KEY_FLAG_IMPORT_KEY = 1,
        HKS_KEY_FLAG_GENERATE_KEY = 2,
    }

    export enum HksKeyStorageType {
        HKS_STORAGE_TEMP = 0,
        HKS_STORAGE_PERSISTENT = 1,
    }

    export enum HksSendType {
        HKS_SEND_TYPE_ASYNC = 0,
        HKS_SEND_TYPE_SYNC = 1,
    }

    declare enum HksTagType {
        HKS_TAG_TYPE_INVALID = 0 << 28,
        HKS_TAG_TYPE_INT = 1 << 28,
        HKS_TAG_TYPE_UINT = 2 << 28,
        HKS_TAG_TYPE_ULONG = 3 << 28,
        HKS_TAG_TYPE_BOOL = 4 << 28,
        HKS_TAG_TYPE_BYTES = 5 << 28,
    }

    export enum HksTag {
        /* Invalid TAG */
        HKS_TAG_INVALID = HksTagType.HKS_TAG_TYPE_INVALID | 0,

        /* Base algrithom TAG: 1 - 200 */
        HKS_TAG_ALGORITHM = HksTagType.HKS_TAG_TYPE_UINT | 1,
        HKS_TAG_PURPOSE = HksTagType.HKS_TAG_TYPE_UINT | 2,
        HKS_TAG_KEY_SIZE = HksTagType.HKS_TAG_TYPE_UINT | 3,
        HKS_TAG_DIGEST = HksTagType.HKS_TAG_TYPE_UINT | 4,
        HKS_TAG_PADDING = HksTagType.HKS_TAG_TYPE_UINT | 5,
        HKS_TAG_BLOCK_MODE = HksTagType.HKS_TAG_TYPE_UINT | 6,
        HKS_TAG_KEY_TYPE = HksTagType.HKS_TAG_TYPE_UINT | 7,
        HKS_TAG_ASSOCIATED_DATA = HksTagType.HKS_TAG_TYPE_BYTES | 8,
        HKS_TAG_NONCE = HksTagType.HKS_TAG_TYPE_BYTES | 9,
        HKS_TAG_IV = HksTagType.HKS_TAG_TYPE_BYTES | 10,

        /* Key derivation TAG */
        HKS_TAG_INFO = HksTagType.HKS_TAG_TYPE_BYTES | 11,
        HKS_TAG_SALT = HksTagType.HKS_TAG_TYPE_BYTES | 12,
        HKS_TAG_PWD = HksTagType.HKS_TAG_TYPE_BYTES | 13,
        HKS_TAG_ITERATION = HksTagType.HKS_TAG_TYPE_UINT | 14,

        HKS_TAG_KEY_GENERATE_TYPE = HksTagType.HKS_TAG_TYPE_UINT | 15, /* choose from enum HksKeyGenerateType */
        HKS_TAG_DERIVE_MAIN_KEY = HksTagType.HKS_TAG_TYPE_BYTES | 16,
        HKS_TAG_DERIVE_FACTOR = HksTagType.HKS_TAG_TYPE_BYTES | 17,
        HKS_TAG_DERIVE_ALG = HksTagType.HKS_TAG_TYPE_UINT | 18,
        HKS_TAG_AGREE_ALG = HksTagType.HKS_TAG_TYPE_UINT | 19,
        HKS_TAG_AGREE_PUBLIC_KEY_IS_KEY_ALIAS = HksTagType.HKS_TAG_TYPE_BOOL | 20,
        HKS_TAG_AGREE_PRIVATE_KEY_ALIAS = HksTagType.HKS_TAG_TYPE_BYTES | 21,
        HKS_TAG_AGREE_PUBLIC_KEY = HksTagType.HKS_TAG_TYPE_BYTES | 22,

        /*
         * Key authentication related TAG: 201 - 300
         *
         * Start of validity
         */
        HKS_TAG_ACTIVE_DATETIME = HksTagType.HKS_TAG_TYPE_ULONG | 201,

        /* Date when new "messages" should not be created. */
        HKS_TAG_ORIGINATION_EXPIRE_DATETIME = HksTagType.HKS_TAG_TYPE_ULONG | 202,

        /* Date when existing "messages" should not be used. */
        HKS_TAG_USAGE_EXPIRE_DATETIME = HksTagType.HKS_TAG_TYPE_ULONG | 203,

        /* Key creation time */
        HKS_TAG_CREATION_DATETIME = HksTagType.HKS_TAG_TYPE_ULONG | 204,

        /* Other authentication related TAG: 301 - 500 */
        HKS_TAG_ALL_USERS = HksTagType.HKS_TAG_TYPE_BOOL | 301,
        HKS_TAG_USER_ID = HksTagType.HKS_TAG_TYPE_UINT | 302,
        HKS_TAG_NO_AUTH_REQUIRED = HksTagType.HKS_TAG_TYPE_BOOL | 303,
        HKS_TAG_USER_AUTH_TYPE = HksTagType.HKS_TAG_TYPE_UINT | 304,
        HKS_TAG_AUTH_TIMEOUT = HksTagType.HKS_TAG_TYPE_UINT | 305,
        HKS_TAG_AUTH_TOKEN = HksTagType.HKS_TAG_TYPE_BYTES | 306,

        /* Attestation related TAG: 501 - 600 */
        HKS_TAG_ATTESTATION_CHALLENGE = HksTagType.HKS_TAG_TYPE_BYTES | 501,
        HKS_TAG_ATTESTATION_APPLICATION_ID = HksTagType.HKS_TAG_TYPE_BYTES | 502,
        HKS_TAG_ATTESTATION_ID_BRAND = HksTagType.HKS_TAG_TYPE_BYTES | 503,
        HKS_TAG_ATTESTATION_ID_DEVICE = HksTagType.HKS_TAG_TYPE_BYTES | 504,
        HKS_TAG_ATTESTATION_ID_PRODUCT = HksTagType.HKS_TAG_TYPE_BYTES | 505,
        HKS_TAG_ATTESTATION_ID_SERIAL = HksTagType.HKS_TAG_TYPE_BYTES | 506,
        HKS_TAG_ATTESTATION_ID_IMEI = HksTagType.HKS_TAG_TYPE_BYTES | 507,
        HKS_TAG_ATTESTATION_ID_MEID = HksTagType.HKS_TAG_TYPE_BYTES | 508,
        HKS_TAG_ATTESTATION_ID_MANUFACTURER = HksTagType.HKS_TAG_TYPE_BYTES | 509,
        HKS_TAG_ATTESTATION_ID_MODEL = HksTagType.HKS_TAG_TYPE_BYTES | 510,
        HKS_TAG_ATTESTATION_ID_ALIAS = HksTagType.HKS_TAG_TYPE_BYTES | 511,

        /*
         * Other reserved TAG: 601 - 1000
         *
         * Extention TAG: 1001 - 9999
         */
        HKS_TAG_IS_KEY_ALIAS = HksTagType.HKS_TAG_TYPE_BOOL | 1001,
        HKS_TAG_KEY_STORAGE_FLAG = HksTagType.HKS_TAG_TYPE_UINT | 1002, /* choose from enum HksKeyStorageType */
        HKS_TAG_IS_ALLOWED_WRAP = HksTagType.HKS_TAG_TYPE_BOOL | 1003,
        HKS_TAG_KEY_WRAP_TYPE = HksTagType.HKS_TAG_TYPE_UINT | 1004,
        HKS_TAG_KEY_AUTH_ID = HksTagType.HKS_TAG_TYPE_BYTES | 1005,
        HKS_TAG_KEY_ROLE = HksTagType.HKS_TAG_TYPE_UINT | 1006,
        HKS_TAG_KEY_FLAG = HksTagType.HKS_TAG_TYPE_UINT | 1007, /* choose from enum HksKeyFlag */
        HKS_TAG_IS_ASYNCHRONIZED = HksTagType.HKS_TAG_TYPE_UINT | 1008,
        HKS_TAG_SECURE_KEY_ALIAS = HksTagType.HKS_TAG_TYPE_BOOL | 1009,
        HKS_TAG_SECURE_KEY_UUID = HksTagType.HKS_TAG_TYPE_BYTES | 1010,
        HKS_TAG_KEY_DOMAIN = HksTagType.HKS_TAG_TYPE_UINT | 1011,

        /* Inner-use TAG: 10001 - 10999 */
        HKS_TAG_PROCESS_NAME = HksTagType.HKS_TAG_TYPE_BYTES | 10001,
        HKS_TAG_PACKAGE_NAME = HksTagType.HKS_TAG_TYPE_BYTES | 10002,
        HKS_TAG_ACCESS_TIME = HksTagType.HKS_TAG_TYPE_UINT | 10003,
        HKS_TAG_USES_TIME = HksTagType.HKS_TAG_TYPE_UINT | 10004,
        HKS_TAG_CRYPTO_CTX = HksTagType.HKS_TAG_TYPE_ULONG | 10005,
        HKS_TAG_KEY = HksTagType.HKS_TAG_TYPE_BYTES | 10006,
        HKS_TAG_KEY_VERSION = HksTagType.HKS_TAG_TYPE_UINT | 10007,
        HKS_TAG_PAYLOAD_LEN = HksTagType.HKS_TAG_TYPE_UINT | 10008,
        HKS_TAG_AE_TAG = HksTagType.HKS_TAG_TYPE_BYTES | 10009,
        HKS_TAG_IS_KEY_HANDLE = HksTagType.HKS_TAG_TYPE_ULONG | 10010,

        /* Os version related TAG */
        HKS_TAG_OS_VERSION = HksTagType.HKS_TAG_TYPE_UINT | 10101,
        HKS_TAG_OS_PATCHLEVEL = HksTagType.HKS_TAG_TYPE_UINT | 10102,

        /*
         * Reversed TAGs for SOTER: 11000 - 12000
         *
         * Other TAGs: 20001 - N
         * TAGs used for paramSetOut
         */
        HKS_TAG_SYMMETRIC_KEY_DATA = HksTagType.HKS_TAG_TYPE_BYTES | 20001,
        HKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA = HksTagType.HKS_TAG_TYPE_BYTES | 20002,
        HKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA = HksTagType.HKS_TAG_TYPE_BYTES | 20003,
    }
}

export default huks;
