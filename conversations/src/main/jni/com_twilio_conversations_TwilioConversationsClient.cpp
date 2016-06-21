#include <jni.h>
#include <string.h>

#include "TSCMediaCodecRegistry.h"
#include "TSCoreSDKTypes.h"
#include "TSCoreConstants.h"
#include "TSCoreSDK.h"
#include "TSCEndpoint.h"
#include "TSCEndpointObserver.h"
#include "TSCConfiguration.h"
#include "TSCLogger.h"
#include "android_platform_info_provider.h"
#include "AccessManager/AccessManager.h"
#include "webrtc/voice_engine/include/voe_base.h"
#include "webrtc/modules/video_capture/video_capture_internal.h"
#include "webrtc/modules/video_render/video_render_internal.h"
#include "webrtc/api/java/jni/androidvideocapturer_jni.h"
#include "webrtc/modules/audio_device/android/audio_manager.h"
#include "webrtc/modules/audio_device/android/opensles_player.h"

#include "com_twilio_conversations_TwilioConversationsClient.h"

#include "webrtc/api/java/jni/jni_helpers.h"
#include "webrtc/api/java/jni/classreferenceholder.h"
#include "webrtc/api/java/jni/androidnetworkmonitor_jni.h"
#include "webrtc/media/engine/webrtcvideodecoderfactory.h"
#include "webrtc/media/engine/webrtcvideoencoderfactory.h"
#include "android_video_codec_manager.h"


#define TAG  "TwilioSDK(native)"

using cricket::WebRtcVideoDecoderFactory;
using cricket::WebRtcVideoEncoderFactory;
using namespace webrtc_jni;
using namespace twiliosdk;


static bool media_jvm_set = false;

extern "C" jint JNIEXPORT JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) {
    TS_CORE_LOG_MODULE(kTSCoreLogModulePlatform, kTSCoreLogLevelDebug, "JNI_OnLoad");
    jint ret = InitGlobalJniVariables(jvm);
    if (ret < 0) {
        TS_CORE_LOG_MODULE(kTSCoreLogModulePlatform,
                           kTSCoreLogLevelError,
                           "InitGlobalJniVariables() failed");
        return -1;
    }
    LoadGlobalClassReferenceHolder();

    return ret;
}

extern "C" void JNIEXPORT JNICALL JNI_OnUnLoad(JavaVM *jvm, void *reserved) {
    TS_CORE_LOG_MODULE(kTSCoreLogModulePlatform, kTSCoreLogLevelDebug, "JNI_OnUnload");
    FreeGlobalClassReferenceHolder();
}

/*
 * Class:     com_twilio_conversations_TwilioConversationsClient
 * Method:    initCore
 * Signature: (Landroid/content/Context;)Z
 */
JNIEXPORT jlong JNICALL Java_com_twilio_conversations_TwilioConversationsClient_nativeInitCore(
        JNIEnv *env, jobject obj, jobject context) {
    TS_CORE_LOG_MODULE(kTSCoreLogModulePlatform, kTSCoreLogLevelDebug, "initCore");
    bool failure = false;
    TSCSDK* tscSdk = new TSCSDK();

    // TODO investigate relocating some of these calls to more timely locations
    if (!media_jvm_set) {
        failure |= webrtc::OpenSLESPlayer::SetAndroidAudioDeviceObjects(GetJVM(), context);
        failure |= webrtc::VoiceEngine::SetAndroidObjects(GetJVM(), context);
        failure |= webrtc::SetRenderAndroidVM(GetJVM());
        failure |= webrtc_jni::AndroidVideoCapturerJni::SetAndroidObjects(env, context);
        media_jvm_set = true;
    }

    if (tscSdk != NULL && !failure) {
        TSCPlatformDataProviderRef provider =
                new rtc::RefCountedObject<AndroidPlatformInfoProvider>(env, context);
        TSCMediaCodecRegistry& codecManager = tscSdk->getMediaCodecRegistry();
        TSCVideoCodecRef androidVideoCodecManager =
                new rtc::RefCountedObject<AndroidVideoCodecManager>();

        tscSdk->setPlatformDataProvider(provider);
        codecManager.registerVideoCodec(androidVideoCodecManager);

        return jlongFromPointer(tscSdk);
    }

    return 0;
}


/*
 * Class:     com_twilio_conversations_TwilioConversationsClient
 * Method:    destroyCore
 * Signature: ()J
 */
JNIEXPORT void JNICALL Java_com_twilio_conversations_TwilioConversationsClient_nativeDestroyCore(JNIEnv *env, jobject obj, jlong nativeCore) {
    TS_CORE_LOG_MODULE(kTSCoreLogModulePlatform, kTSCoreLogLevelDebug, "destroyCore");
    TSCSDK* tscSdk = reinterpret_cast<TSCSDK*>(nativeCore);
    TSCMediaCodecRegistry& codecManager = tscSdk->getMediaCodecRegistry();

    codecManager.unregisterVideoCodecsForName(AndroidVideoCodecManager::videoCodecManagerName);
    delete tscSdk;
}


JNIEXPORT void JNICALL Java_com_twilio_conversations_TwilioConversationsClient_nativeSetCoreLogLevel
        (JNIEnv *env, jobject obj, jint level) {
    TS_CORE_LOG_MODULE(kTSCoreLogModulePlatform, kTSCoreLogLevelDebug, "setCoreLogLevel");
    TSCoreLogLevel coreLogLevel = static_cast<TSCoreLogLevel>(level);
    TSCLogger::instance()->setLogLevel(coreLogLevel);
}

JNIEXPORT void JNICALL Java_com_twilio_conversations_TwilioConversationsClient_nativeSetModuleLevel
        (JNIEnv *env, jobject obj, jint module, jint level) {
    TS_CORE_LOG_MODULE(kTSCoreLogModulePlatform, kTSCoreLogLevelDebug, "setModuleLevel");
    TSCoreLogModule coreLogModule = static_cast<TSCoreLogModule>(module);
    TSCoreLogLevel coreLogLevel = static_cast<TSCoreLogLevel>(level);
    TSCLogger::instance()->setModuleLogLevel(coreLogModule, coreLogLevel);
}

JNIEXPORT jint JNICALL Java_com_twilio_conversations_TwilioConversationsClient_nativeGetCoreLogLevel
        (JNIEnv *env, jobject obj) {
    TS_CORE_LOG_MODULE(kTSCoreLogModulePlatform, kTSCoreLogLevelDebug, "getCoreLogLevel");
    return TSCLogger::instance()->getLogLevel();
}

JNIEXPORT void JNICALL Java_com_twilio_conversations_TwilioConversationsClient_nativeRefreshRegistrations
        (JNIEnv *, jobject, jlong nativeCore) {
    TS_CORE_LOG_MODULE(kTSCoreLogModulePlatform, kTSCoreLogLevelDebug, "refreshRegistrations");
    TSCSDK* tscSdk = reinterpret_cast<TSCSDK*>(nativeCore);
    tscSdk->refreshRegistrations();
}


