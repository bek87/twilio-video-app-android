/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_twilio_conversations_impl_TwilioConversationsImpl */

#ifndef _Included_com_twilio_conversations_impl_TwilioConversationsImpl
#define _Included_com_twilio_conversations_impl_TwilioConversationsImpl
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_twilio_conversations_impl_TwilioConversationsImpl
 * Method:    initCore
 * Signature: (Landroid/content/Context;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_twilio_conversations_impl_TwilioConversationsImpl_initCore
  (JNIEnv *, jobject, jobject);

/*
 * Class:     com_twilio_conversations_impl_TwilioConversationsImpl
 * Method:    onApplicationForeground
 * Signature: ()J
 */
JNIEXPORT void JNICALL Java_com_twilio_conversations_impl_TwilioConversationsImpl_onApplicationForeground
        (JNIEnv *, jobject);

/*
 * Class:     com_twilio_conversations_impl_TwilioConversationsImpl
 * Method:    onApplicationWakeUp
 * Signature: ()J
 */
JNIEXPORT void JNICALL Java_com_twilio_conversations_impl_TwilioConversationsImpl_onApplicationWakeUp
        (JNIEnv *, jobject);

/*
 * Class:     com_twilio_conversations_impl_TwilioConversationsImpl
 * Method:    onApplicationBackground
 * Signature: ()J
 */
JNIEXPORT void JNICALL Java_com_twilio_conversations_impl_TwilioConversationsImpl_onApplicationBackground
        (JNIEnv *, jobject);

/*
 * Class:     com_twilio_conversations_impl_TwilioConversationsImpl
 * Method:    destroyCore
 * Signature: ()J
 */
JNIEXPORT void JNICALL Java_com_twilio_conversations_impl_TwilioConversationsImpl_destroyCore
  (JNIEnv *, jobject);

/*
 * Class:     com_twilio_conversations_impl_TwilioConversationsImpl
 * Method:    createEndpoint
 * Signature: (Ljava/lang/String;J)J
 */
JNIEXPORT jlong JNICALL Java_com_twilio_conversations_impl_TwilioConversationsImpl_createEndpoint
  (JNIEnv *, jobject, jobject, jobjectArray, jlong);

/*
 * Class:     com_twilio_conversations_impl_TwilioConversationsImpl
 * Method:    setCoreLogLevel 
 * Signature: (I)J
 */
JNIEXPORT void JNICALL Java_com_twilio_conversations_impl_TwilioConversationsImpl_setCoreLogLevel
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_twilio_conversations_impl_TwilioConversationsImpl
 * Method:    setModuleLevel
 * Signature: (II)J
 */
JNIEXPORT void JNICALL Java_com_twilio_conversations_impl_TwilioConversationsImpl_setModuleLevel
        (JNIEnv *, jobject, jint, jint);

/*
 * Class:     com_twilio_conversations_impl_TwilioConversationsImpl
 * Method:    getCoreLogLevel 
 * Signature: ()J
 */
JNIEXPORT jint JNICALL Java_com_twilio_conversations_impl_TwilioConversationsImpl_getCoreLogLevel
  (JNIEnv *, jobject);

/*
 * Class:     com_twilio_conversations_impl_TwilioConversationsImpl
 * Method:    refreshRegistrations
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_twilio_conversations_impl_TwilioConversationsImpl_refreshRegistrations
  (JNIEnv *, jobject);


#ifdef __cplusplus
}
#endif
#endif
