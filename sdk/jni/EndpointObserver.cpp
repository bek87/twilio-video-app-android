#include "EndpointObserver.h"
#include "com_twilio_signal_impl_SignalCore.h"

using namespace twiliosdk;

EndpointObserver::EndpointObserver(JNIEnv* env, jobject config)
{
	this->m_config = env->NewGlobalRef(config);
};

EndpointObserver::~EndpointObserver(){
	assert(this->m_config == NULL);
};

void EndpointObserver::destroy(JNIEnv* env) {

	__android_log_print(ANDROID_LOG_VERBOSE, "JNI SIGNAL", "EndpointObserver::destroy() called", 1);

     env->DeleteGlobalRef(this->m_config);
     this->m_config = NULL;
   }

void EndpointObserver::onRegistrationDidComplete(TSCErrorObject* error)
{
	__android_log_print(ANDROID_LOG_VERBOSE, "JNI SIGNAL", "Received notification:onRegistrationDidComplete", 1);

	JNIEnvAttacher attacher;
	JNIEnv *g_env = attacher.get();

	jobject callbacks = tw_jni_fetch_object(g_env, m_config, "callbacks", "Lcom/twilio/signal/impl/SignalCoreConfig$Callbacks;");
	jmethodID meth = tw_jni_get_method(g_env, callbacks, "onRegistrationComplete","()V");
	g_env->CallVoidMethod(callbacks, meth);

}

void EndpointObserver::onUnregistrationDidComplete(TSCErrorObject* error)
{
	__android_log_print(ANDROID_LOG_VERBOSE, "JNI SIGNAL", "Received notification:onUnregistrationDidComplete", 1);
	JNIEnvAttacher attacher;
	JNIEnv *g_env = attacher.get();

	jobject callbacks = tw_jni_fetch_object(g_env, m_config, "callbacks", "Lcom/twilio/signal/impl/SignalCoreConfig$Callbacks;");
	jmethodID meth = tw_jni_get_method(g_env, callbacks, "onUnRegistrationComplete","()V");
	g_env->CallVoidMethod(callbacks, meth);

}

void EndpointObserver::onStateDidChange(TSCEndpointState state) {
	__android_log_print(ANDROID_LOG_VERBOSE, "JNI SIGNAL", "Received notification:onStateDidChange", 1);
}


void EndpointObserver::onIncomingCallDidReceive(TSCIncomingSession* session) {
	__android_log_print(ANDROID_LOG_VERBOSE, "JNI SIGNAL", "Received notification:onIncomingCallDidReceive", 1);

	JNIEnvAttacher attacher;
	JNIEnv *g_env = attacher.get();

	jobject callbacks = tw_jni_fetch_object(g_env, m_config, "callbacks", "Lcom/twilio/signal/impl/SignalCoreConfig$Callbacks;");

	__android_log_print(ANDROID_LOG_VERBOSE, "JNI SIGNAL", "callbacks is not null notification:onIncomingCallDidReceive", 1);
	jmethodID meth = tw_jni_get_method(g_env, callbacks, "onIncomingCall","()V");
	__android_log_print(ANDROID_LOG_VERBOSE, "JNI SIGNAL", "meth is not null notification:onIncomingCallDidReceive", 1);

	g_env->CallVoidMethod(callbacks, meth);
}




