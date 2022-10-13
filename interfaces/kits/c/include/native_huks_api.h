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

#ifndef NATIVE_HUKS_API_H
#define NATIVE_HUKS_API_H

/**
 * @addtogroup HuksKeyApi
 * @{
 *
 * @brief Describes the OpenHarmony Universal KeyStore (HUKS) capabilities, including key management and cryptography operations, provided for applications.
 * The keys managed by HUKS can be imported by applications or generated by calling the HUKS APIs.
 *
 * @syscap SystemCapability.Security.Huks
 * @since 9
 * @version 1.0
 */

/**
 * @file native_huks_api.h
 *
 * @brief Declares the APIs used to access the HUKS.
 *
 * @since 9
 * @version 1.0
 */

#include "native_huks_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Obtains the current HUKS SDK version.
 *
 * @param sdkVersion Indicates the pointer to the SDK version (in string format) obtained.
 * @return Returns {@link OH_Huks_ErrCode#OH_HUKS_SUCCESS} if the operation is successful; returns an error code otherwise.
 * @since 9
 * @version 1.0
 */
struct OH_Huks_Result OH_Huks_GetSdkVersion(struct OH_Huks_Blob *sdkVersion);

/**
 * @brief Generates a key.
 *
 * @param keyAlias Indicates the pointer to the alias of the key to generate. The alias must be unique in the process of the service. Otherwise, the key will be overwritten.
 * @param paramSetIn Indicates the pointer to the parameter set for generating the key.
 * @param paramSetOut Indicates the pointer to a temporary key generated. If the generated key is not of a temporary type, this parameter is a null pointer.
 * @return Returns {@link OH_Huks_ErrCode#OH_HUKS_SUCCESS} if the operation is successful; returns an error code otherwise.
 * @since 9
 * @version 1.0
 */
struct OH_Huks_Result OH_Huks_GenerateKeyItem(const struct OH_Huks_Blob *keyAlias,
    const struct OH_Huks_ParamSet *paramSetIn, struct OH_Huks_ParamSet *paramSetOut);

/**
 * @brief Imports a key in plaintext.
 *
 * @param keyAlias Indicates the pointer to the alias of the key to import. The alias must be unique in the process of the service. Otherwise, the key will be overwritten.
 * @param paramSet Indicates the pointer to the parameters of the key to import.
 * @param key Indicates the pointer to the key to import. The key must be in the format required by the HUKS. For details, see {@link HuksTypeApi}.
 * @return Returns {@link OH_Huks_ErrCode#OH_HUKS_SUCCESS} if the operation is successful; returns an error code otherwise.
 * @since 9
 * @version 1.0
 */
struct OH_Huks_Result OH_Huks_ImportKeyItem(const struct OH_Huks_Blob *keyAlias,
    const struct OH_Huks_ParamSet *paramSet, const struct OH_Huks_Blob *key);

/**
 * @brief Imports a wrapped key.
 *
 * @param keyAlias Indicates the pointer to the alias of the key to import. The alias must be unique in the process of the service. Otherwise, the key will be overwritten.
 * @param wrappingKeyAlias Indicates the pointer to the alias of the wrapping key, which is obtained through key agreement and used to decrypt the key to import.
 * @param paramSet Indicates the pointer to the parameters of the wrapped key to import.
 * @param wrappedKeyData Indicates the pointer to the wrapped key to import. The key must be in the format required by the HUKS. For details, see {@link OH_Huks_AlgSuite}.
 * @return Returns {@link OH_Huks_ErrCode#OH_HUKS_SUCCESS} if the operation is successful; returns an error code otherwise.
 * @since 9
 * @version 1.0
 */
struct OH_Huks_Result OH_Huks_ImportWrappedKeyItem(const struct OH_Huks_Blob *keyAlias,
    const struct OH_Huks_Blob *wrappingKeyAlias, const struct OH_Huks_ParamSet *paramSet,
    const struct OH_Huks_Blob *wrappedKeyData);

/**
 * @brief Exports a public key.
 *
 * @param keyAlias Indicates the pointer to the alias of the public key to export. The alias must be the same as the alias for the key generated.
 * @param paramSet Indicates the pointer to the parameters required for exporting the public key.
 * @param key Indicates the pointer to the public key exported.
 * @return Returns {@link OH_Huks_ErrCode#OH_HUKS_SUCCESS} if the operation is successful; returns an error code otherwise.
 * @since 9
 * @version 1.0
 */
struct OH_Huks_Result OH_Huks_ExportPublicKeyItem(const struct OH_Huks_Blob *keyAlias,
    const struct OH_Huks_ParamSet *paramSet, struct OH_Huks_Blob *key);

/**
 * @brief Deletes a key.
 *
 * @param keyAlias Indicates the pointer to the alias of the key to delete. The alias must be the same as the alias for the key generated.
 * @param paramSet Indicates the pointer to the parameters required for deleting the key. By default, this parameter is a null pointer.
 * @return Returns {@link OH_Huks_ErrCode#OH_HUKS_SUCCESS} if the operation is successful; returns an error code otherwise.
 * @since 9
 * @version 1.0
 */
struct OH_Huks_Result OH_Huks_DeleteKeyItem(const struct OH_Huks_Blob *keyAlias,
    const struct OH_Huks_ParamSet *paramSet);

/**
 * @brief Obtains the attributes of a key.
 *
 * @param keyAlias Indicates the pointer to the alias of the target key.
 * @param paramSetIn Indicates the pointer to the attribute tag required for obtaining the attributes. By default, this parameter is a null pointer.
 * @param paramSetOut Indicates the pointer to the attributes obtained.
 * @return Returns {@link OH_Huks_ErrCode#OH_HUKS_SUCCESS} if the operation is successful; returns an error code otherwise.
 * @since 9
 * @version 1.0
 */
struct OH_Huks_Result OH_Huks_GetKeyItemParamSet(const struct OH_Huks_Blob *keyAlias,
    const struct OH_Huks_ParamSet *paramSetIn, struct OH_Huks_ParamSet *paramSetOut);

/**
 * @brief Checks whether a key exists.
 *
 * @param keyAlias Indicates the pointer to the alias of the target key.
 * @param paramSet Indicates the pointer to the attribute tag required for checking the key. By default, this parameter is a null pointer.
 * @return Returns {@link OH_Huks_ErrCode#OH_HUKS_SUCCESS} if the key exists.
 * @return Returns {@link OH_Huks_ErrCode#OH_HUKS_ERR_CODE_ITEM_NOT_EXIST} if the key does not exist. 
 * @return Returns any other error code for other cases.
 * @since 9
 * @version 1.0
 */
struct OH_Huks_Result OH_Huks_IsKeyItemExist(const struct OH_Huks_Blob *keyAlias,
    const struct OH_Huks_ParamSet *paramSet);

/**
 * @brief Obtain the key certificate chain.
 *
 * @param keyAlias Indicates the pointer to the alias of the target key.
 * @param paramSet Indicates the pointer to the parameters required for obtaining the key certificate.
 * @param certChain Indicates the pointer to the key certificate chain obtained.
 * @return Returns {@link OH_Huks_ErrCode#OH_HUKS_SUCCESS} if the operation is successful; returns an error code otherwise.
 * @since 9
 * @version 1.0
 */
struct OH_Huks_Result OH_Huks_AttestKeyItem(const struct OH_Huks_Blob *keyAlias,
    const struct OH_Huks_ParamSet *paramSet, struct OH_Huks_CertChain *certChain);

/**
 * @brief Initializes the key session interface and obtains a handle (mandatory) and challenge value (optional).
 *
 * @param keyAlias Indicates the pointer to the alias of the target key.
 * @param paramSet Indicates the pointer to the parameters for the initialization operation.
 * @param handle Indicates the pointer to the handle of the key session obtained. This handle is required for subsequent operations, including {@link OH_Huks_UpdateSession}, 
 * {@link OH_Huks_FinishSession}, and {@link OH_Huks_AbortSession}.
 * @param challenge Indicates the pointer to the challenge value obtained.
 * @return Returns {@link OH_Huks_ErrCode#OH_HUKS_SUCCESS} if the operation is successful; returns an error code otherwise.
 * @since 9
 * @version 1.0
 * @see OH_Huks_UpdateSession
 * @see OH_Huks_FinishSession
 * @see OH_Huks_AbortSession
 */
struct OH_Huks_Result OH_Huks_InitSession(const struct OH_Huks_Blob *keyAlias,
    const struct OH_Huks_ParamSet *paramSet, struct OH_Huks_Blob *handle, struct OH_Huks_Blob *challenge);

/**
 * @brief Adds data by segment for the key operation, performs the related key operation, and outputs the processed data.
 *
 * @param handle Indicates the pointer to the key session handle, which is generated by {@link OH_Huks_InitSession}.
 * @param paramSet Indicates the pointer to the parameters required for the key operation.
 * @param inData Indicates the pointer to the data to be processed. This API can be called multiples time to process large data by segment.
 * @param outData Indicates the pointer to the output data.
 * @return Returns {@link OH_Huks_ErrCode#OH_HUKS_SUCCESS} if the operation is successful; returns an error code otherwise.
 * @since 9
 * @version 1.0
 * @see OH_Huks_InitSession
 * @see OH_Huks_FinishSession
 * @see OH_Huks_AbortSession
 */
struct OH_Huks_Result OH_Huks_UpdateSession(const struct OH_Huks_Blob *handle,
    const struct OH_Huks_ParamSet *paramSet, const struct OH_Huks_Blob *inData, struct OH_Huks_Blob *outData);

/**
 * @brief Ends the key session.
 *
 * @param handle Indicates the pointer to the key session handle, which is generated by {@link OH_Huks_InitSession}.
 * @param paramSet Indicates the pointer to the parameters required for the key operation.
 * @param inData Indicates the pointer to the data to be processed.
 * @param outData Indicates the pointer to the output data.
 * @return Returns {@link OH_Huks_ErrCode#OH_HUKS_SUCCESS} if the operation is successful; returns an error code otherwise.
 * @since 9
 * @version 1.0
 * @see OH_Huks_InitSession
 * @see OH_Huks_UpdateSession
 * @see OH_Huks_AbortSession
 */
struct OH_Huks_Result OH_Huks_FinishSession(const struct OH_Huks_Blob *handle,
    const struct OH_Huks_ParamSet *paramSet, const struct OH_Huks_Blob *inData, struct OH_Huks_Blob *outData);

/**
 * @brief Aborts a key session.
 *
 * @param handle Indicates the pointer to the key session handle, which is generated by {@link OH_Huks_InitSession}.
 * @param paramSet Indicates the pointer to the parameters required for aborting the key session. By default, this parameter is a null pointer.
 * @return Returns {@link OH_Huks_ErrCode#OH_HUKS_SUCCESS} if the operation is successful; returns an error code otherwise.
 * @since 9
 * @version 1.0
 * @see OH_Huks_InitSession
 * @see OH_Huks_UpdateSession
 * @see OH_Huks_FinishSession
 */
struct OH_Huks_Result OH_Huks_AbortSession(const struct OH_Huks_Blob *handle,
    const struct OH_Huks_ParamSet *paramSet);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NATIVE_HUKS_API_H */
