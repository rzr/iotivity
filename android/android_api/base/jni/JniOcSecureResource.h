/*
* //******************************************************************
* //
* // Copyright 2015 Samsung Electronics All Rights Reserved.
* //
* //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
* //
* // Licensed under the Apache License, Version 2.0 (the "License");
* // you may not use this file except in compliance with the License.
* // You may obtain a copy of the License at
* //
* //      http://www.apache.org/licenses/LICENSE-2.0
* //
* // Unless required by applicable law or agreed to in writing, software
* // distributed under the License is distributed on an "AS IS" BASIS,
* // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* // See the License for the specific language governing permissions and
* // limitations under the License.
* //
* //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

#include "JniOcStack.h"
#include "JniProvisionResultListner.h"
#include "OCProvisioningManager.h"
#include <mutex>

#ifndef _Included_org_iotivity_base_OcSecureResource
#define _Included_org_iotivity_base_OcSecureResource

using namespace OC;

class JniOcSecureResource
{
    public:
        JniOcSecureResource(std::shared_ptr<OCSecureResource> secureresource);
        ~JniOcSecureResource();

        static JniOcSecureResource* getJniOcSecureResourcePtr(JNIEnv *env, jobject thiz);
        std::string getDeviceID(void);
        std::string getIpAddr(void);
        int getDeviceStatus(void);
        bool getOwnedStatus(void);
        OCSecureResource* getDevicePtr();

        JniProvisionResultListner* AddProvisionResultListener(JNIEnv* env, jobject jListener);
        void  RemoveProvisionResultListener(JNIEnv* env, jobject jListener);

        OCStackResult doOwnershipTransfer(JNIEnv* env, jobject jListener);
        OCStackResult getLinkedDevices(JNIEnv *env, UuidList_t &uuidList);
        OCStackResult provisionACL(JNIEnv* env, jobject acl, jobject jListener);
        OCStackResult provisionPairwiseDevices(JNIEnv* env, jint type, jint keySize,
                jobject acl1, jobject device2, jobject acl2, jobject jListener);
        OCStackResult provisionCredentials(JNIEnv* env, jint type, jint keySize,
                jobject device2, jobject jListener);
        OCStackResult unlinkDevices(JNIEnv* env, jobject device2, jobject jListener);
        OCStackResult removeDevice(JNIEnv* env, jint timeout, jobject jListener);

    private:

        std::map<jobject, std::pair<JniProvisionResultListner*, int> > resultMap;
        std::mutex resultMapLock;
        std::shared_ptr<OCSecureResource> m_sharedSecureResource;
};

/* DO NOT EDIT THIS FILE BEYOND THIS LINE - it is machine generated */

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     org_iotivity_base_OcSecureResource
 * Method:    doOwnershipTransfer
 * Signature: (Lorg/iotivity/base/OcSecureResource/DoOwnershipTransferListener;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcSecureResource_doOwnershipTransfer
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_iotivity_base_OcSecureResource
 * Method:    removeDevice
 * Signature: (ILorg/iotivity/base/OcSecureResource/RemoveDeviceListener;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcSecureResource_removeDevice
  (JNIEnv *, jobject, jint, jobject);

/*
 * Class:     org_iotivity_base_OcSecureResource
 * Method:    unlinkDevices
 * Signature: (Ljava/lang/Object;Lorg/iotivity/base/OcSecureResource/UnlinkDevicesListener;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcSecureResource_unlinkDevices
  (JNIEnv *, jobject, jobject, jobject);

/*
 * Class:     org_iotivity_base_OcSecureResource
 * Method:    provisionCredentials1
 * Signature: (IILjava/lang/Object;Lorg/iotivity/base/OcSecureResource/ProvisionCredentialsListener;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcSecureResource_provisionCredentials1
  (JNIEnv *, jobject, jint, jint, jobject, jobject);

/*
 * Class:     org_iotivity_base_OcSecureResource
 * Method:    provisionACL
 * Signature: (Ljava/lang/Object;Lorg/iotivity/base/OcSecureResource/ProvisionAclListener;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcSecureResource_provisionACL
  (JNIEnv *, jobject, jobject, jobject);

/*
 * Class:     org_iotivity_base_OcSecureResource
 * Method:    provisionPairwiseDevices1
 * Signature: (IILjava/lang/Object;Ljava/lang/Object;Ljava/lang/Object;Lorg/iotivity/base/OcSecureResource/ProvisionPairwiseDevicesListener;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcSecureResource_provisionPairwiseDevices1
  (JNIEnv *, jobject, jint, jint, jobject, jobject, jobject, jobject);

/*
 * Class:     org_iotivity_base_OcSecureResource
 * Method:    getLinkedDevices
 * Signature: ()Ljava/util/List;
 */
JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcSecureResource_getLinkedDevices
  (JNIEnv *, jobject);

/*
 * Class:     org_iotivity_base_OcSecureResource
 * Method:    getIpAddr
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_iotivity_base_OcSecureResource_getIpAddr
  (JNIEnv *, jobject);

/*
 * Class:     org_iotivity_base_OcSecureResource
 * Method:    getDeviceID
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_iotivity_base_OcSecureResource_getDeviceID
  (JNIEnv *, jobject);

/*
 * Class:     org_iotivity_base_OcSecureResource
 * Method:    deviceStatus
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_iotivity_base_OcSecureResource_deviceStatus
  (JNIEnv *, jobject);

/*
 * Class:     org_iotivity_base_OcSecureResource
 * Method:    ownedStatus
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_iotivity_base_OcSecureResource_ownedStatus
  (JNIEnv *, jobject);

/*
 * Class:     org_iotivity_base_OcSecureResource
 * Method:    dispose
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcSecureResource_dispose
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif

