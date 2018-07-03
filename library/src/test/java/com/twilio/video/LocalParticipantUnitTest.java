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

package com.twilio.video;

import static org.mockito.Mockito.when;

import android.os.Handler;
import com.twilio.video.util.Constants;
import java.util.Arrays;
import java.util.Random;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.runners.MockitoJUnitRunner;

@RunWith(MockitoJUnitRunner.class)
public class LocalParticipantUnitTest {
    private static final int INT_MAX = 25;

    private final Random random = new Random();
    private LocalParticipant localParticipant;
    @Mock Handler handler;
    @Mock LocalAudioTrackPublication mockLocalAudioTrackPublication;
    @Mock LocalVideoTrackPublication mockLocalVideoTrackPublicationOne;
    @Mock LocalDataTrackPublication mockLocalDataTrackPublication;
    @Mock LocalAudioTrack mockLocalAudioTrack;
    @Mock LocalVideoTrack mockLocalVideoTrack;
    @Mock LocalDataTrack mockLocalDataTrack;

    @Before
    public void setup() {
        localParticipant =
                new LocalParticipant(
                        random.nextLong(),
                        Constants.MOCK_PARTICIPANT_SID,
                        String.valueOf(random.nextInt(INT_MAX)),
                        Arrays.asList(mockLocalAudioTrackPublication),
                        Arrays.asList(mockLocalVideoTrackPublicationOne),
                        Arrays.asList(mockLocalDataTrackPublication),
                        handler);
    }

    @Test(expected = NullPointerException.class)
    public void shouldFailWithNullSid() {
        new LocalParticipant(
                random.nextLong(),
                null,
                String.valueOf(random.nextInt(INT_MAX)),
                Arrays.asList(mockLocalAudioTrackPublication),
                Arrays.asList(mockLocalVideoTrackPublicationOne),
                Arrays.asList(mockLocalDataTrackPublication),
                handler);
    }

    @Test(expected = IllegalArgumentException.class)
    public void shouldFailWithEmptySid() {
        new LocalParticipant(
                random.nextLong(),
                "",
                String.valueOf(random.nextInt(INT_MAX)),
                Arrays.asList(mockLocalAudioTrackPublication),
                Arrays.asList(mockLocalVideoTrackPublicationOne),
                Arrays.asList(mockLocalDataTrackPublication),
                handler);
    }

    @Test(expected = NullPointerException.class)
    public void shouldFailWithNullIdentity() {
        new LocalParticipant(
                random.nextLong(),
                Constants.MOCK_PARTICIPANT_SID,
                null,
                Arrays.asList(mockLocalAudioTrackPublication),
                Arrays.asList(mockLocalVideoTrackPublicationOne),
                Arrays.asList(mockLocalDataTrackPublication),
                handler);
    }

    @Test
    public void shouldSucceedWithValidTrackSid() {
        new LocalParticipant(
                random.nextLong(),
                Constants.MOCK_PARTICIPANT_SID,
                String.valueOf(random.nextInt(INT_MAX)),
                Arrays.asList(mockLocalAudioTrackPublication),
                Arrays.asList(mockLocalVideoTrackPublicationOne),
                Arrays.asList(mockLocalDataTrackPublication),
                handler);
    }

    @Test(expected = NullPointerException.class)
    public void setListener_shouldNotAllowNull() {
        localParticipant.setListener(null);
    }

    @Test(expected = UnsupportedOperationException.class)
    public void shouldNotAllowModifyingAudioTracks() {
        localParticipant.getAudioTracks().add(mockLocalAudioTrackPublication);
    }

    @Test(expected = UnsupportedOperationException.class)
    public void shouldNotAllowModifyingVideoTracks() {
        localParticipant.getVideoTracks().add(mockLocalVideoTrackPublicationOne);
    }

    @Test(expected = UnsupportedOperationException.class)
    public void shouldNotAllowModifyingPublishedAudioTracks() {
        localParticipant.getLocalAudioTracks().add(mockLocalAudioTrackPublication);
    }

    @Test(expected = UnsupportedOperationException.class)
    public void shouldNotAllowModifyingPublishedVideoTracks() {
        localParticipant.getLocalVideoTracks().add(mockLocalVideoTrackPublicationOne);
    }

    @Test(expected = IllegalArgumentException.class)
    public void publishTrack_shouldNotAllowReleasedAudioTrack() {
        when(mockLocalAudioTrack.isReleased()).thenReturn(true);
        localParticipant.publishTrack(mockLocalAudioTrack);
    }

    @Test(expected = IllegalArgumentException.class)
    public void publishTrack_shouldNotAllowReleasedVideoTrack() {
        when(mockLocalVideoTrack.isReleased()).thenReturn(true);
        localParticipant.publishTrack(mockLocalVideoTrack);
    }

    @Test(expected = IllegalArgumentException.class)
    public void publishTrack_shouldNotAllowReleasedDataTrack() {
        when(mockLocalDataTrack.isReleased()).thenReturn(true);
        localParticipant.publishTrack(mockLocalDataTrack);
    }

    @Test(expected = IllegalArgumentException.class)
    public void unpublishTrack_shouldNotAllowReleasedAudioTrack() {
        when(mockLocalAudioTrack.isReleased()).thenReturn(true);
        localParticipant.unpublishTrack(mockLocalAudioTrack);
    }

    @Test(expected = IllegalArgumentException.class)
    public void unpublishTrack_shouldNotAllowReleasedVideoTrack() {
        when(mockLocalVideoTrack.isReleased()).thenReturn(true);
        localParticipant.unpublishTrack(mockLocalVideoTrack);
    }

    @Test(expected = IllegalArgumentException.class)
    public void unpublishTrack_shouldNotAllowReleasedDataTrack() {
        when(mockLocalDataTrack.isReleased()).thenReturn(true);
        localParticipant.unpublishTrack(mockLocalDataTrack);
    }
}
