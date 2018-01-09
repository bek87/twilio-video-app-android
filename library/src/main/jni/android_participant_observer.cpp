/*
 * Copyright (C) 2017 Twilio, Inc.
 *
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

#include "android_participant_observer.h"
#include "com_twilio_video_RemoteParticipant.h"
#include "com_twilio_video_RemoteDataTrack.h"
#include "class_reference_holder.h"
#include "logging.h"
#include "webrtc/sdk/android/src/jni/classreferenceholder.h"
#include "com_twilio_video_TwilioException.h"

namespace twilio_video_jni {

jobject createJavaWebRtcVideoTrack(JNIEnv *env,
                                   std::shared_ptr<twilio::media::RemoteVideoTrack> remote_video_track) {
    jclass j_webrtc_video_track_class = webrtc_jni::FindClass(env, "org/webrtc/VideoTrack");
    jmethodID j_webrtc_video_track_ctor_id = webrtc_jni::GetMethodID(env,
                                                                     j_webrtc_video_track_class,
                                                                     "<init>",
                                                                     "(J)V");
    jobject j_webrtc_video_track = env->NewObject(j_webrtc_video_track_class,
                                                  j_webrtc_video_track_ctor_id,
                                                  webrtc_jni::jlongFromPointer(
                                                          remote_video_track->getWebRtcTrack()));
    CHECK_EXCEPTION(env) << "Failed to create org.webrtc.VideoTrack";

    return j_webrtc_video_track;
}

jobject createJavaWebRtcAudioTrack(JNIEnv *env,
                                   std::shared_ptr<twilio::media::RemoteAudioTrack> remote_audio_track) {
    jclass j_webrtc_audio_track_class = webrtc_jni::FindClass(env, "org/webrtc/AudioTrack");
    jmethodID j_webrtc_audio_track_ctor_id = webrtc_jni::GetMethodID(env,
                                                                     j_webrtc_audio_track_class,
                                                                     "<init>",
                                                                     "(J)V");
    jobject j_webrtc_audio_track = env->NewObject(j_webrtc_audio_track_class,
                                                  j_webrtc_audio_track_ctor_id,
                                                  webrtc_jni::jlongFromPointer(
                                                          remote_audio_track->getWebRtcTrack()));
    CHECK_EXCEPTION(env) << "Failed to create org.webrtc.AudioTrack";

    return j_webrtc_audio_track;
}

AndroidParticipantObserver::AndroidParticipantObserver(JNIEnv *env,
                                                       jobject j_remote_participant,
                                                       jobject j_remote_participant_observer,
                                                       std::map<std::shared_ptr<twilio::media::RemoteAudioTrackPublication>, jobject>& remote_audio_track_publication_map,
                                                       std::map<std::shared_ptr<twilio::media::RemoteAudioTrack>, jobject>& remote_audio_track_map,
                                                       std::map<std::shared_ptr<twilio::media::RemoteVideoTrackPublication>, jobject>& remote_video_track_publication_map,
                                                       std::map<std::shared_ptr<twilio::media::RemoteVideoTrack>, jobject>& remote_video_track_map,
                                                       std::map<std::shared_ptr<twilio::media::RemoteDataTrackPublication>, jobject>& remote_data_track_publication_map,
                                                       std::map<std::shared_ptr<twilio::media::RemoteDataTrack>, jobject>& remote_data_track_map) :
        j_remote_participant_(env, j_remote_participant),
        j_remote_participant_observer_(env, j_remote_participant_observer),
        j_remote_participant_observer_class_(env,
                                             webrtc_jni::GetObjectClass(env, *j_remote_participant_observer_)),
        remote_audio_track_publication_map_(remote_audio_track_publication_map),
        remote_audio_track_map_(remote_audio_track_map),
        remote_video_track_publication_map_(remote_video_track_publication_map),
        remote_video_track_map_(remote_video_track_map),
        remote_data_track_publication_map_(remote_data_track_publication_map),
        remote_data_track_map_(remote_data_track_map),
        j_remote_audio_track_class_(env, twilio_video_jni::FindClass(env,
                                                                     "com/twilio/video/RemoteAudioTrack")),
        j_remote_audio_track_publication_class_(env, twilio_video_jni::FindClass(env,
                                                                                 "com/twilio/video/RemoteAudioTrackPublication")),
        j_remote_video_track_class_(env, twilio_video_jni::FindClass(env,
                                                                     "com/twilio/video/RemoteVideoTrack")),
        j_remote_video_track_publication_class_(env, twilio_video_jni::FindClass(env,
                                                                                 "com/twilio/video/RemoteVideoTrackPublication")),
        j_remote_data_track_class_(env, twilio_video_jni::FindClass(env,
                                                                    "com/twilio/video/RemoteDataTrack")),
        j_remote_data_track_publication_class_(env, twilio_video_jni::FindClass(env,
                                                                                "com/twilio/video/RemoteDataTrackPublication")),
        j_twilio_exception_class_(env, twilio_video_jni::FindClass(env,
                                                                   "com/twilio/video/TwilioException")),
        j_on_audio_track_published_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_participant_observer_class_,
                                        "onAudioTrackPublished",
                                        "(Lcom/twilio/video/RemoteParticipant;Lcom/twilio/video/RemoteAudioTrackPublication;)V")),
        j_on_audio_track_unpublished_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_participant_observer_class_,
                                        "onAudioTrackUnpublished",
                                        "(Lcom/twilio/video/RemoteParticipant;Lcom/twilio/video/RemoteAudioTrackPublication;)V")),
        j_on_audio_track_subscribed_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_participant_observer_class_,
                                        "onAudioTrackSubscribed",
                                        "(Lcom/twilio/video/RemoteParticipant;Lcom/twilio/video/RemoteAudioTrackPublication;Lcom/twilio/video/RemoteAudioTrack;)V")),
        j_on_audio_track_subscription_failed_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_participant_observer_class_,
                                        "onAudioTrackSubscriptionFailed",
                                        "(Lcom/twilio/video/RemoteParticipant;Lcom/twilio/video/RemoteAudioTrackPublication;Lcom/twilio/video/TwilioException;)V")),
        j_on_audio_track_unsubscribed_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_participant_observer_class_,
                                        "onAudioTrackUnsubscribed",
                                        "(Lcom/twilio/video/RemoteParticipant;Lcom/twilio/video/RemoteAudioTrackPublication;Lcom/twilio/video/RemoteAudioTrack;)V")),
        j_on_video_track_published_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_participant_observer_class_,
                                        "onVideoTrackPublished",
                                        "(Lcom/twilio/video/RemoteParticipant;Lcom/twilio/video/RemoteVideoTrackPublication;)V")),
        j_on_video_track_unpublished_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_participant_observer_class_,
                                        "onVideoTrackUnpublished",
                                        "(Lcom/twilio/video/RemoteParticipant;Lcom/twilio/video/RemoteVideoTrackPublication;)V")),
        j_on_video_track_subscribed_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_participant_observer_class_,
                                        "onVideoTrackSubscribed",
                                        "(Lcom/twilio/video/RemoteParticipant;Lcom/twilio/video/RemoteVideoTrackPublication;Lcom/twilio/video/RemoteVideoTrack;)V")),
        j_on_video_track_subscription_failed_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_participant_observer_class_,
                                        "onVideoTrackSubscriptionFailed",
                                        "(Lcom/twilio/video/RemoteParticipant;Lcom/twilio/video/RemoteVideoTrackPublication;Lcom/twilio/video/TwilioException;)V")),
        j_on_video_track_unsubscribed_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_participant_observer_class_,
                                        "onVideoTrackUnsubscribed",
                                        "(Lcom/twilio/video/RemoteParticipant;Lcom/twilio/video/RemoteVideoTrackPublication;Lcom/twilio/video/RemoteVideoTrack;)V")),
        j_on_data_track_published_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_participant_observer_class_,
                                        "onDataTrackPublished",
                                        "(Lcom/twilio/video/RemoteParticipant;Lcom/twilio/video/RemoteDataTrackPublication;)V")),
        j_on_data_track_unpublished_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_participant_observer_class_,
                                        "onDataTrackUnpublished",
                                        "(Lcom/twilio/video/RemoteParticipant;Lcom/twilio/video/RemoteDataTrackPublication;)V")),
        j_on_data_track_subscribed_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_participant_observer_class_,
                                        "onDataTrackSubscribed",
                                        "(Lcom/twilio/video/RemoteParticipant;Lcom/twilio/video/RemoteDataTrackPublication;Lcom/twilio/video/RemoteDataTrack;)V")),
        j_on_data_track_subscription_failed_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_participant_observer_class_,
                                        "onDataTrackSubscriptionFailed",
                                        "(Lcom/twilio/video/RemoteParticipant;Lcom/twilio/video/RemoteDataTrackPublication;Lcom/twilio/video/TwilioException;)V")),
        j_on_data_track_unsubscribed_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_participant_observer_class_,
                                        "onDataTrackUnsubscribed",
                                        "(Lcom/twilio/video/RemoteParticipant;Lcom/twilio/video/RemoteDataTrackPublication;Lcom/twilio/video/RemoteDataTrack;)V")),
        j_on_audio_track_enabled_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_participant_observer_class_,
                                        "onAudioTrackEnabled",
                                        "(Lcom/twilio/video/RemoteParticipant;Lcom/twilio/video/RemoteAudioTrackPublication;)V")),
        j_on_audio_track_disabled_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_participant_observer_class_,
                                        "onAudioTrackDisabled",
                                        "(Lcom/twilio/video/RemoteParticipant;Lcom/twilio/video/RemoteAudioTrackPublication;)V")),
        j_on_video_track_enabled_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_participant_observer_class_,
                                        "onVideoTrackEnabled",
                                        "(Lcom/twilio/video/RemoteParticipant;Lcom/twilio/video/RemoteVideoTrackPublication;)V")),
        j_on_video_track_disabled_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_participant_observer_class_,
                                        "onVideoTrackDisabled",
                                        "(Lcom/twilio/video/RemoteParticipant;Lcom/twilio/video/RemoteVideoTrackPublication;)V")),
        j_audio_track_ctor_id_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_audio_track_class_,
                                        "<init>",
                                        "(Lorg/webrtc/AudioTrack;Ljava/lang/String;Z)V")),
        j_audio_track_publication_ctor_id_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_audio_track_publication_class_,
                                        "<init>",
                                        "(ZZLjava/lang/String;Ljava/lang/String;)V")),
        j_video_track_ctor_id_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_video_track_class_,
                                        "<init>",
                                        "(Lorg/webrtc/VideoTrack;Ljava/lang/String;Z)V")),
        j_video_track_publication_ctor_id_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_video_track_publication_class_,
                                        "<init>",
                                        "(ZZLjava/lang/String;Ljava/lang/String;)V")),
        j_data_track_ctor_id_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_data_track_class_,
                                        "<init>",
                                        "(ZZZIILjava/lang/String;J)V")),
        j_data_track_publication_ctor_id_(
                webrtc_jni::GetMethodID(env,
                                        *j_remote_data_track_publication_class_,
                                        "<init>",
                                        "(ZZLjava/lang/String;Ljava/lang/String;)V")),
        j_twilio_exception_ctor_id_(
                webrtc_jni::GetMethodID(env,
                                        *j_twilio_exception_class_,
                                        "<init>",
                                        kTwilioExceptionConstructorSignature)) {
    VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                      twilio::video::LogLevel::kDebug,
                      "AndroidMediaObserver");
}

AndroidParticipantObserver::~AndroidParticipantObserver() {
    VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                      twilio::video::LogLevel::kDebug,
                      "~AndroidMediaObserver");
}

void AndroidParticipantObserver::setObserverDeleted() {
    rtc::CritScope cs(&deletion_lock_);
    observer_deleted_ = true;
    VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                      twilio::video::LogLevel::kDebug,
                      "participant observer deleted");
}

void AndroidParticipantObserver::onAudioTrackPublished(twilio::video::RemoteParticipant *remote_participant,
                                                       std::shared_ptr<twilio::media::RemoteAudioTrackPublication> remote_audio_track_publication) {
    webrtc_jni::ScopedLocalRefFrame local_ref_frame(jni());
    std::string func_name = std::string(__FUNCTION__);
    VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                      twilio::video::LogLevel::kDebug,
                      "%s", func_name.c_str());

    {
        rtc::CritScope cs(&deletion_lock_);

        if (!isObserverValid(func_name)) {
            return;
        }

        jobject j_audio_track_publication = createJavaRemoteAudioTrackPublication(jni(),
                                                                                  remote_audio_track_publication,
                                                                                  *j_remote_audio_track_publication_class_,
                                                                                  j_audio_track_publication_ctor_id_);
        /*
         * We create a global reference to the java audio track so we can map audio track events
         * to the original java instance.
         */
        remote_audio_track_publication_map_.insert(std::make_pair(remote_audio_track_publication,
                                                                  webrtc_jni::NewGlobalRef(jni(),
                                                                                           j_audio_track_publication)));
        jni()->CallVoidMethod(*j_remote_participant_observer_,
                              j_on_audio_track_published_,
                              *j_remote_participant_,
                              j_audio_track_publication);
        CHECK_EXCEPTION(jni()) << "error during CallVoidMethod";
    }
}

void AndroidParticipantObserver::onAudioTrackUnpublished(twilio::video::RemoteParticipant *remote_participant,
                                                         std::shared_ptr<twilio::media::RemoteAudioTrackPublication> remote_audio_track_publication) {
    webrtc_jni::ScopedLocalRefFrame local_ref_frame(jni());
    std::string func_name = std::string(__FUNCTION__);
    VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                      twilio::video::LogLevel::kDebug,
                      "%s", func_name.c_str());
    {
        rtc::CritScope cs(&deletion_lock_);
        if (!isObserverValid(func_name)) {
            return;
        }

        // Notify developer
        auto publication_it = remote_audio_track_publication_map_.find(remote_audio_track_publication);
        jobject j_audio_track_publication = publication_it->second;
        jni()->CallVoidMethod(*j_remote_participant_observer_,
                              j_on_audio_track_unpublished_,
                              *j_remote_participant_,
                              j_audio_track_publication);
        CHECK_EXCEPTION(jni()) << "error during CallVoidMethod";

        // We can remove audio track and delete the global reference after notifying developer
        remote_audio_track_publication_map_.erase(publication_it);
        webrtc_jni::DeleteGlobalRef(jni(), j_audio_track_publication);
        CHECK_EXCEPTION(jni()) << "error deleting global RemoteAudioTrackPublication reference";
    }
}

void AndroidParticipantObserver::onVideoTrackPublished(twilio::video::RemoteParticipant *remote_participant,
                                                       std::shared_ptr<twilio::media::RemoteVideoTrackPublication> remote_video_track_publication) {
    webrtc_jni::ScopedLocalRefFrame local_ref_frame(jni());
    std::string func_name = std::string(__FUNCTION__);
    VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                      twilio::video::LogLevel::kDebug,
                      "%s", func_name.c_str());
    {
        rtc::CritScope cs(&deletion_lock_);

        if (!isObserverValid(func_name)) {
            return;
        }

        jobject j_video_track_publication = createJavaRemoteVideoTrackPublication(jni(),
                                                                                  remote_video_track_publication,
                                                                                  *j_remote_video_track_publication_class_,
                                                                                  j_video_track_publication_ctor_id_);
        /*
         * We create a global reference to the java video track so we can map video track events
         * to the original java instance.
         */
        remote_video_track_publication_map_.insert(std::make_pair(remote_video_track_publication,
                                                                  webrtc_jni::NewGlobalRef(jni(),
                                                                                           j_video_track_publication)));
        jni()->CallVoidMethod(*j_remote_participant_observer_,
                              j_on_video_track_published_,
                              *j_remote_participant_,
                              j_video_track_publication);
        CHECK_EXCEPTION(jni()) << "error during CallVoidMethod";
    }
}

void AndroidParticipantObserver::onVideoTrackUnpublished(twilio::video::RemoteParticipant *remote_participant,
                                                         std::shared_ptr<twilio::media::RemoteVideoTrackPublication> remote_video_track_publication) {
    webrtc_jni::ScopedLocalRefFrame local_ref_frame(jni());
    std::string func_name = std::string(__FUNCTION__);
    VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                      twilio::video::LogLevel::kDebug,
                      "%s", func_name.c_str());
    {
        rtc::CritScope cs(&deletion_lock_);

        if (!isObserverValid(func_name)) {
            return;
        }

        // Notify developer
        auto publication_it = remote_video_track_publication_map_.find(remote_video_track_publication);
        jobject j_video_track_publication = publication_it->second;
        jni()->CallVoidMethod(*j_remote_participant_observer_,
                              j_on_video_track_unpublished_,
                              *j_remote_participant_,
                              j_video_track_publication);
        CHECK_EXCEPTION(jni()) << "error during CallVoidMethod";

        // We can remove video track and delete the global reference after notifying developer
        remote_video_track_publication_map_.erase(publication_it);
        webrtc_jni::DeleteGlobalRef(jni(), j_video_track_publication);
        CHECK_EXCEPTION(jni()) << "error deleting global RemoteVideoTrackPublication reference";
    }
}

void AndroidParticipantObserver::onDataTrackPublished(twilio::video::RemoteParticipant *remote_participant,
                                                      std::shared_ptr<twilio::media::RemoteDataTrackPublication> remote_data_track_publication) {
    webrtc_jni::ScopedLocalRefFrame local_ref_frame(jni());
    std::string func_name = std::string(__FUNCTION__);
    VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                      twilio::video::LogLevel::kDebug,
                      "%s", func_name.c_str());
    {
        rtc::CritScope cs(&deletion_lock_);

        if (!isObserverValid(func_name)) {
            return;
        }

        jobject j_data_track_publication = createJavaRemoteDataTrackPublication(jni(),
                                                                                remote_data_track_publication,
                                                                                *j_remote_data_track_publication_class_,
                                                                                j_data_track_publication_ctor_id_);
        /*
         * Create a global reference to the java data track so we can map data track events
         * to the original java instance.
         */
        remote_data_track_publication_map_.insert(std::make_pair(remote_data_track_publication,
                                                                 webrtc_jni::NewGlobalRef(jni(),
                                                                                          j_data_track_publication)));
        jni()->CallVoidMethod(*j_remote_participant_observer_,
                              j_on_data_track_published_,
                              *j_remote_participant_,
                              j_data_track_publication);
        CHECK_EXCEPTION(jni()) << "error during CallVoidMethod";
    }
}

void AndroidParticipantObserver::onDataTrackUnpublished(twilio::video::RemoteParticipant *remote_participant,
                                                        std::shared_ptr<twilio::media::RemoteDataTrackPublication> remote_data_track_publication) {
    webrtc_jni::ScopedLocalRefFrame local_ref_frame(jni());
    std::string func_name = std::string(__FUNCTION__);
    VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                      twilio::video::LogLevel::kDebug,
                      "%s", func_name.c_str());
    {
        rtc::CritScope cs(&deletion_lock_);

        if (!isObserverValid(func_name)) {
            return;
        }

        // Notify developer
        auto publication_it = remote_data_track_publication_map_.find(remote_data_track_publication);
        jobject j_data_track_publication = publication_it->second;
        jni()->CallVoidMethod(*j_remote_participant_observer_,
                              j_on_data_track_unpublished_,
                              *j_remote_participant_,
                              j_data_track_publication);
        CHECK_EXCEPTION(jni()) << "error during CallVoidMethod";

        // Remove data track and delete the global reference after notifying developer
        remote_data_track_publication_map_.erase(publication_it);
        webrtc_jni::DeleteGlobalRef(jni(), j_data_track_publication);
        CHECK_EXCEPTION(jni()) << "error deleting global RemoteDataTrackPublication reference";
    }
}

void AndroidParticipantObserver::onAudioTrackEnabled(twilio::video::RemoteParticipant *remote_participant,
                                                     std::shared_ptr<twilio::media::RemoteAudioTrackPublication> remote_audio_track_publication) {
    webrtc_jni::ScopedLocalRefFrame local_ref_frame(jni());
    std::string func_name = std::string(__FUNCTION__);
    VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                      twilio::video::LogLevel::kDebug,
                      "%s", func_name.c_str());

    {
        rtc::CritScope cs(&deletion_lock_);

        if (!isObserverValid(func_name)) {
            return;
        }

        jobject j_remote_audio_track_publication = remote_audio_track_publication_map_[remote_audio_track_publication];
        jni()->CallVoidMethod(*j_remote_participant_observer_,
                              j_on_audio_track_enabled_,
                              *j_remote_participant_,
                              j_remote_audio_track_publication);
        CHECK_EXCEPTION(jni()) << "error during CallVoidMethod";
    }
}

void AndroidParticipantObserver::onAudioTrackDisabled(twilio::video::RemoteParticipant *remote_participant,
                                                      std::shared_ptr<twilio::media::RemoteAudioTrackPublication> remote_audio_track_publication) {
    webrtc_jni::ScopedLocalRefFrame local_ref_frame(jni());
    std::string func_name = std::string(__FUNCTION__);
    VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                      twilio::video::LogLevel::kDebug,
                      "%s", func_name.c_str());

    {
        rtc::CritScope cs(&deletion_lock_);

        if (!isObserverValid(func_name)) {
            return;
        }

        jobject j_remote_audio_track_publication = remote_audio_track_publication_map_[remote_audio_track_publication];
        jni()->CallVoidMethod(*j_remote_participant_observer_,
                              j_on_audio_track_disabled_,
                              *j_remote_participant_,
                              j_remote_audio_track_publication);
        CHECK_EXCEPTION(jni()) << "error during CallVoidMethod";
    }
}

void AndroidParticipantObserver::onVideoTrackEnabled(twilio::video::RemoteParticipant *remote_participant,
                                                     std::shared_ptr<twilio::media::RemoteVideoTrackPublication> remote_video_track_publication) {
    webrtc_jni::ScopedLocalRefFrame local_ref_frame(jni());
    std::string func_name = std::string(__FUNCTION__);
    VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                      twilio::video::LogLevel::kDebug,
                      "%s", func_name.c_str());

    {
        rtc::CritScope cs(&deletion_lock_);

        if (!isObserverValid(func_name)) {
            return;
        }

        jobject j_remote_video_track_publication = remote_video_track_publication_map_[remote_video_track_publication];
        jni()->CallVoidMethod(*j_remote_participant_observer_,
                              j_on_video_track_enabled_,
                              *j_remote_participant_,
                              j_remote_video_track_publication);
        CHECK_EXCEPTION(jni()) << "error during CallVoidMethod";
    }
}

void AndroidParticipantObserver::onVideoTrackDisabled(twilio::video::RemoteParticipant *remote_participant,
                                                      std::shared_ptr<twilio::media::RemoteVideoTrackPublication> remote_video_track_publication) {
    webrtc_jni::ScopedLocalRefFrame local_ref_frame(jni());
    std::string func_name = std::string(__FUNCTION__);
    VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                      twilio::video::LogLevel::kDebug,
                      "%s", func_name.c_str());

    {
        rtc::CritScope cs(&deletion_lock_);

        if (!isObserverValid(func_name)) {
            return;
        }

        jobject j_remote_video_track_publication = remote_video_track_publication_map_[remote_video_track_publication];
        jni()->CallVoidMethod(*j_remote_participant_observer_,
                              j_on_video_track_disabled_,
                              *j_remote_participant_,
                              j_remote_video_track_publication);
        CHECK_EXCEPTION(jni()) << "error during CallVoidMethod";
    }
}

void AndroidParticipantObserver::onAudioTrackSubscribed(twilio::video::RemoteParticipant *participant,
                                                        std::shared_ptr<twilio::media::RemoteAudioTrackPublication> remote_audio_track_publication,
                                                        std::shared_ptr<twilio::media::RemoteAudioTrack> remote_audio_track) {
    webrtc_jni::ScopedLocalRefFrame local_ref_frame(jni());
    std::string func_name = std::string(__FUNCTION__);
    VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                      twilio::video::LogLevel::kDebug,
                      "%s", func_name.c_str());

    {
        rtc::CritScope cs(&deletion_lock_);

        if (!isObserverValid(func_name)) {
            return;
        }

        jobject j_remote_audio_track_publication = remote_audio_track_publication_map_[remote_audio_track_publication];
        jobject j_webrtc_audio_track = createJavaWebRtcAudioTrack(jni(), remote_audio_track);
        jobject j_remote_audio_track = createJavaRemoteAudioTrack(jni(),
                                                                  remote_audio_track,
                                                                  j_webrtc_audio_track,
                                                                  *j_remote_audio_track_class_,
                                                                  j_audio_track_ctor_id_);
        remote_audio_track_map_.insert(std::make_pair(remote_audio_track_publication->getRemoteTrack(),
                                                      webrtc_jni::NewGlobalRef(jni(),
                                                                               j_remote_audio_track)));
        jni()->CallVoidMethod(*j_remote_participant_observer_,
                              j_on_audio_track_subscribed_,
                              *j_remote_participant_,
                              j_remote_audio_track_publication,
                              j_remote_audio_track);
        CHECK_EXCEPTION(jni()) << "error during CallVoidMethod";
    }
}

void AndroidParticipantObserver::onAudioTrackSubscriptionFailed(twilio::video::RemoteParticipant *participant,
                                                                std::shared_ptr<twilio::media::RemoteAudioTrackPublication> remote_audio_track_publication,
                                                                const twilio::video::TwilioError twilio_error) {
    webrtc_jni::ScopedLocalRefFrame local_ref_frame(jni());
    std::string func_name = std::string(__FUNCTION__);
    VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                      twilio::video::LogLevel::kDebug,
                      "%s", func_name.c_str());

    {
        rtc::CritScope cs(&deletion_lock_);

        if (!isObserverValid(func_name)) {
            return;
        }

        jobject j_remote_audio_track_publication = remote_audio_track_publication_map_[remote_audio_track_publication];
        jobject j_twilio_exception = createJavaTwilioException(jni(),
                                                               *j_twilio_exception_class_,
                                                               j_twilio_exception_ctor_id_,
                                                               twilio_error);
        jni()->CallVoidMethod(*j_remote_participant_observer_,
                              j_on_audio_track_subscription_failed_,
                              *j_remote_participant_,
                              j_remote_audio_track_publication,
                              j_twilio_exception);
        CHECK_EXCEPTION(jni()) << "error during CallVoidMethod";
    }
}

void AndroidParticipantObserver::onAudioTrackUnsubscribed(twilio::video::RemoteParticipant *participant,
                                                          std::shared_ptr<twilio::media::RemoteAudioTrackPublication> remote_audio_track_publication,
                                                          std::shared_ptr<twilio::media::RemoteAudioTrack> remote_audio_track) {
    webrtc_jni::ScopedLocalRefFrame local_ref_frame(jni());
    std::string func_name = std::string(__FUNCTION__);
    VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                      twilio::video::LogLevel::kDebug,
                      "%s", func_name.c_str());

    {
        rtc::CritScope cs(&deletion_lock_);
        if (!isObserverValid(func_name)) {
            return;
        }

        // Notify developer
        auto track_it = remote_audio_track_map_.find(remote_audio_track_publication->getRemoteTrack());
        jobject j_remote_audio_track = track_it->second;
        jobject j_remote_audio_track_publication = remote_audio_track_publication_map_[remote_audio_track_publication];
        jni()->CallVoidMethod(*j_remote_participant_observer_,
                              j_on_audio_track_unsubscribed_,
                              *j_remote_participant_,
                              j_remote_audio_track_publication,
                              j_remote_audio_track);
        CHECK_EXCEPTION(jni()) << "error during CallVoidMethod";
        remote_audio_track_map_.erase(track_it);
        webrtc_jni::DeleteGlobalRef(jni(), j_remote_audio_track);
        CHECK_EXCEPTION(jni()) << "error deleting global RemoteAudioTrack reference";
    }
}

void AndroidParticipantObserver::onVideoTrackSubscribed(twilio::video::RemoteParticipant *participant,
                                                        std::shared_ptr<twilio::media::RemoteVideoTrackPublication> remote_video_track_publication,
                                                        std::shared_ptr<twilio::media::RemoteVideoTrack> remote_video_track) {
    webrtc_jni::ScopedLocalRefFrame local_ref_frame(jni());
    std::string func_name = std::string(__FUNCTION__);
    VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                      twilio::video::LogLevel::kDebug,
                      "%s", func_name.c_str());

    {
        rtc::CritScope cs(&deletion_lock_);

        if (!isObserverValid(func_name)) {
            return;
        }

        jobject j_remote_video_track_publication = remote_video_track_publication_map_[remote_video_track_publication];
        jobject j_webrtc_video_track = createJavaWebRtcVideoTrack(jni(), remote_video_track);
        jobject j_remote_video_track = createJavaRemoteVideoTrack(jni(),
                                                                  remote_video_track,
                                                                  j_webrtc_video_track,
                                                                  *j_remote_video_track_class_,
                                                                  j_video_track_ctor_id_);
        remote_video_track_map_.insert(std::make_pair(remote_video_track_publication->getRemoteTrack(),
                                                      webrtc_jni::NewGlobalRef(jni(),
                                                                               j_remote_video_track)));
        jni()->CallVoidMethod(*j_remote_participant_observer_,
                              j_on_video_track_subscribed_,
                              *j_remote_participant_,
                              j_remote_video_track_publication,
                              j_remote_video_track);
        CHECK_EXCEPTION(jni()) << "error during CallVoidMethod";
    }
}

void AndroidParticipantObserver::onVideoTrackSubscriptionFailed(twilio::video::RemoteParticipant *participant,
                                                                std::shared_ptr<twilio::media::RemoteVideoTrackPublication> remote_video_track_publication,
                                                                const twilio::video::TwilioError twilio_error) {
    webrtc_jni::ScopedLocalRefFrame local_ref_frame(jni());
    std::string func_name = std::string(__FUNCTION__);
    VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                      twilio::video::LogLevel::kDebug,
                      "%s", func_name.c_str());

    {
        rtc::CritScope cs(&deletion_lock_);

        if (!isObserverValid(func_name)) {
            return;
        }

        jobject j_remote_video_track_publication = remote_video_track_publication_map_[remote_video_track_publication];
        jobject j_twilio_exception = createJavaTwilioException(jni(),
                                                               *j_twilio_exception_class_,
                                                               j_twilio_exception_ctor_id_,
                                                               twilio_error);
        jni()->CallVoidMethod(*j_remote_participant_observer_,
                              j_on_video_track_subscription_failed_,
                              *j_remote_participant_,
                              j_remote_video_track_publication,
                              j_twilio_exception);
        CHECK_EXCEPTION(jni()) << "error during CallVoidMethod";
    }
}

void AndroidParticipantObserver::onVideoTrackUnsubscribed(twilio::video::RemoteParticipant *participant,
                                                          std::shared_ptr<twilio::media::RemoteVideoTrackPublication> remote_video_track_publication,
                                                          std::shared_ptr<twilio::media::RemoteVideoTrack> remote_video_track) {
    webrtc_jni::ScopedLocalRefFrame local_ref_frame(jni());
    std::string func_name = std::string(__FUNCTION__);
    VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                      twilio::video::LogLevel::kDebug,
                      "%s", func_name.c_str());

    {
        rtc::CritScope cs(&deletion_lock_);

        if (!isObserverValid(func_name)) {
            return;
        }

        // Notify developer
        auto track_it = remote_video_track_map_.find(remote_video_track);
        jobject j_remote_video_track = track_it->second;
        jobject j_remote_video_track_publication = remote_video_track_publication_map_[remote_video_track_publication];
        jni()->CallVoidMethod(*j_remote_participant_observer_,
                              j_on_video_track_unsubscribed_,
                              *j_remote_participant_,
                              j_remote_video_track_publication,
                              j_remote_video_track);
        CHECK_EXCEPTION(jni()) << "error during CallVoidMethod";
        remote_video_track_map_.erase(track_it);
        webrtc_jni::DeleteGlobalRef(jni(), j_remote_video_track);
        CHECK_EXCEPTION(jni()) << "error deleting global RemoteVideoTrack reference";
    }
}

void AndroidParticipantObserver::onDataTrackSubscribed(twilio::video::RemoteParticipant *participant,
                                                       std::shared_ptr<twilio::media::RemoteDataTrackPublication> remote_data_track_publication,
                                                       std::shared_ptr<twilio::media::RemoteDataTrack> remote_data_track) {
    webrtc_jni::ScopedLocalRefFrame local_ref_frame(jni());
    std::string func_name = std::string(__FUNCTION__);
    VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                      twilio::video::LogLevel::kDebug,
                      "%s", func_name.c_str());

    {
        rtc::CritScope cs(&deletion_lock_);

        if (!isObserverValid(func_name)) {
            return;
        }

        jobject j_remote_data_track_publication = remote_data_track_publication_map_[remote_data_track_publication];
        jobject j_remote_data_track = createJavaRemoteDataTrack(jni(),
                                                                remote_data_track,
                                                                *j_remote_data_track_class_,
                                                                j_data_track_ctor_id_);
        remote_data_track_map_.insert(std::make_pair(remote_data_track_publication->getRemoteTrack(),
                                                      webrtc_jni::NewGlobalRef(jni(),
                                                                               j_remote_data_track)));
        jni()->CallVoidMethod(*j_remote_participant_observer_,
                              j_on_data_track_subscribed_,
                              *j_remote_participant_,
                              j_remote_data_track_publication,
                              j_remote_data_track);
        CHECK_EXCEPTION(jni()) << "error during CallVoidMethod";
    }
}

void AndroidParticipantObserver::onDataTrackSubscriptionFailed(twilio::video::RemoteParticipant *participant,
                                                               std::shared_ptr<twilio::media::RemoteDataTrackPublication> remote_data_track_publication,
                                                               const twilio::video::TwilioError twilio_error) {
    webrtc_jni::ScopedLocalRefFrame local_ref_frame(jni());
    std::string func_name = std::string(__FUNCTION__);
    VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                      twilio::video::LogLevel::kDebug,
                      "%s", func_name.c_str());

    {
        rtc::CritScope cs(&deletion_lock_);

        if (!isObserverValid(func_name)) {
            return;
        }

        jobject j_remote_data_track_publication = remote_data_track_publication_map_[remote_data_track_publication];
        jobject j_twilio_exception = createJavaTwilioException(jni(),
                                                               *j_twilio_exception_class_,
                                                               j_twilio_exception_ctor_id_,
                                                               twilio_error);
        jni()->CallVoidMethod(*j_remote_participant_observer_,
                              j_on_data_track_subscription_failed_,
                              *j_remote_participant_,
                              j_remote_data_track_publication,
                              j_twilio_exception);
        CHECK_EXCEPTION(jni()) << "error during CallVoidMethod";
    }
}

void AndroidParticipantObserver::onDataTrackUnsubscribed(twilio::video::RemoteParticipant *participant,
                                                         std::shared_ptr<twilio::media::RemoteDataTrackPublication> remote_data_track_publication,
                                                         std::shared_ptr<twilio::media::RemoteDataTrack> remote_data_track) {
    webrtc_jni::ScopedLocalRefFrame local_ref_frame(jni());
    std::string func_name = std::string(__FUNCTION__);
    VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                      twilio::video::LogLevel::kDebug,
                      "%s", func_name.c_str());

    {
        rtc::CritScope cs(&deletion_lock_);

        if (!isObserverValid(func_name)) {
            return;
        }

        // Notify developer
        auto track_it = remote_data_track_map_.find(remote_data_track);
        jobject j_remote_data_track = track_it->second;
        jobject j_remote_data_track_publication = remote_data_track_publication_map_[remote_data_track_publication];
        jni()->CallVoidMethod(*j_remote_participant_observer_,
                              j_on_data_track_unsubscribed_,
                              *j_remote_participant_,
                              j_remote_data_track_publication,
                              j_remote_data_track);
        CHECK_EXCEPTION(jni()) << "error during CallVoidMethod";
        remote_data_track_map_.erase(track_it);
        webrtc_jni::DeleteGlobalRef(jni(), j_remote_data_track);
        CHECK_EXCEPTION(jni()) << "error deleting global RemoteDataTrack reference";
    }
}

bool AndroidParticipantObserver::isObserverValid(const std::string &callbackName) {
    if (observer_deleted_) {
        VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                          twilio::video::LogLevel::kWarning,
                          "participant observer is marked for deletion, skipping %s callback",
                          callbackName.c_str());
        return false;
    };
    if (webrtc_jni::IsNull(jni(), *j_remote_participant_observer_)) {
        VIDEO_ANDROID_LOG(twilio::video::LogModule::kPlatform,
                          twilio::video::LogLevel::kWarning,
                          "participant observer reference has been destroyed, skipping %s callback",
                          callbackName.c_str());
        return false;
    }
    return true;
}

}
