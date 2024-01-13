/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * main.c - Basic audio sample.
 *
 * Copyright (c) 2005 Frank Buss <fb@frank-buss.de> (aka Shine)
 *
 * $Id: main.c 1147 2005-10-12 15:52:52Z mrbrown $
 */
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspaudiolib.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <resources\Scripts\AudioManagement.h>




void dump_threadstatus(void);

int  pspAudioInit();
void pspAudioEndPre();
void pspAudioEnd();

#define SAMPLE_COUNT 0x10000
float sample[SAMPLE_COUNT];

#define SAMPLE_RATE 44100

#define OCTAVE_COUNT 6

float octaves[OCTAVE_COUNT][12];

typedef struct {
	int note;
	int octave;
	int duration;
} Note_t;

typedef struct {
	Note_t currentNote;
	int noteIndex;
	int currentTime;
	float currentsampleIndex;
	float currentsampleIncrement;
} ChannelState_t;

ChannelState_t channelStates[3];

// "S" means "#"
#define NOTE_END -2
#define NOTE_PAUSE -1
#define NOTE_C 0
#define NOTE_CS 1
#define NOTE_D 2
#define NOTE_DS 3
#define NOTE_E 4
#define NOTE_F 5
#define NOTE_FS 6
#define NOTE_G 7
#define NOTE_GS 8
#define NOTE_A 9
#define NOTE_AS 10
#define NOTE_B 11

#define EIGHT_NOTE(note, octave, duration) { note, octave, SAMPLE_RATE * duration / 8}

Note_t JumpSfx[] = {


    //        (Nota,Octava,Duración)
//	EIGHT_NOTE(NOTE_B, 4, 1),
//	EIGHT_NOTE(NOTE_E, 5, 3),	
EIGHT_NOTE(NOTE_G, 4, 1),	
EIGHT_NOTE(NOTE_C, 5, 2),	

	EIGHT_NOTE(NOTE_PAUSE, 4, 8),
	
	{ NOTE_END, 0, 0 }
};

Note_t CoinSfx[] = {
	EIGHT_NOTE(NOTE_B, 4, 1),
	EIGHT_NOTE(NOTE_E, 5, 3),	

	
	EIGHT_NOTE(NOTE_PAUSE, 4, 8),
	{ NOTE_END, 0, 0 }
};
Note_t ClickSfx[] = {

	EIGHT_NOTE(NOTE_E, 5, 1),	

	
	{ NOTE_END, 0, 0 }
};

Note_t* channels[] = { JumpSfx, CoinSfx, ClickSfx };

void nextNote(int channel)
{
	ChannelState_t* state = &channelStates[channel];
	state->currentNote = channels[channel][state->noteIndex];
	state->currentTime = 0;
	state->currentsampleIndex = 0;
	int note = state->currentNote.note;
	if (note == NOTE_PAUSE) {
		state->currentsampleIncrement = 0;
	} else {
		state->currentsampleIncrement = octaves[state->currentNote.octave][note] * ((float) SAMPLE_COUNT) / ((float) SAMPLE_RATE);
	}

	state->noteIndex++;



	if (channels[channel][state->noteIndex].note == NOTE_END) state->noteIndex = 0;
}

// calculate current value of attack/delay/sustain/release envelope
float adsr(float time, float duration) {
	if (time < 0.0) return 0.0;
	const float attack = 0.004;
	const float decay = 0.02;
	const float sustain = 0.5;
	const float release = 0.08;
	duration -= attack + decay + release;
	if (time < attack) return time / attack;
	time -= attack;
	if (time < decay) return (decay - time) / decay * (1.0 - sustain) + sustain;
	time -= decay;
	if (time < duration) return sustain;
	time -= duration;
	if (time < release) return (release - time) / release * sustain;
	return 0.0;
}

void audioOutCallback(int channel, unsigned short* buf, unsigned int reqn)
{
	ChannelState_t* state = &channelStates[channel];
	unsigned int i;
	for (i = 0; i < reqn; i++) {
		float time = ((float) state->currentTime) / ((float) SAMPLE_RATE);
		if (state->currentTime++ == state->currentNote.duration) nextNote(channel);
		float value;
		if (state->currentsampleIncrement == 0.0) {
			value = 0.0;
		} else {
			value = sample[(int)state->currentsampleIndex] * adsr(time, ((float) state->currentNote.duration) / ((float) SAMPLE_RATE));
			value *= (float) 0x7000;
			state->currentsampleIndex += state->currentsampleIncrement;
			if (state->currentsampleIndex >= SAMPLE_COUNT) state->currentsampleIndex -= (float) SAMPLE_COUNT;
		}
		buf[0] = value;
		buf[1] = value;
		buf += 2;
	}
}

void audioOutCallback0(void *buf, unsigned int reqn, void *userdata) { audioOutCallback(0, buf, reqn); }
void audioOutCallback1(void *buf, unsigned int reqn, void *userdata) { audioOutCallback(1, buf, reqn); }
void audioOutCallback2(void *buf, unsigned int reqn, void *userdata) { audioOutCallback(2, buf, reqn); }

void createPitches(float base, float* target)
{
	int i;
	for (i = 0; i < 12; i++) {
		target[i] = base;
		base *= 1.0594630943592952645618252949463;  // 2^(1/12)
	}
}


int JumpEffectStop = 0;
int CoinEffectStop = 0;
int ClickEffectStop = 0;

int StartTheAduioSystem(){
	
        int i;
	int maxAt = SAMPLE_COUNT / 16;
	for (i = 0; i < SAMPLE_COUNT; i++) {
		float value;
		if (i < maxAt) {
			value = ((float) i) / ((float) maxAt) * 2.0 - 1.0;
		} else {
			value = 1.0 - ((float) (i - maxAt)) / ((float) (SAMPLE_COUNT - maxAt)) * 2.0;
		}
		sample[i] = value;
	}
	float base = 40.0;
	for (i = 0; i < OCTAVE_COUNT; i++) {
		createPitches(base, octaves[i]);
		base *= 2;
	}
	
	
	
	channelStates[0].noteIndex = 0; nextNote(0);
	channelStates[1].noteIndex = 0; nextNote(1);
	channelStates[2].noteIndex = 0; nextNote(2);
	pspAudioInit();
}

int UpdateAudioSystem(){
	if(JumpEffectStop != 0){
		
if(JumpEffectStop > 25){ 
		pspAudioSetVolume(0, 0, 0);
		JumpEffectStop = 0;
}
else{
	JumpEffectStop ++;
}
	}

	if(CoinEffectStop != 0){
	
if(CoinEffectStop > 25){ 
		pspAudioSetVolume(1, 0, 0);
		CoinEffectStop = 0;
}
else{
	CoinEffectStop ++;
}
	}

	if(ClickEffectStop != 0){
if(ClickEffectStop > 5){ 
		pspAudioSetVolume(2, 0, 0);
		ClickEffectStop = 0;
}
else{
			ClickEffectStop ++;

}
	}
}



int PlayJumpSound()
{
	channelStates[0].noteIndex = 0; nextNote(0);
	pspAudioSetVolume(0, 0x4000, 0x4000);
	pspAudioSetChannelCallback(0, audioOutCallback0, NULL);
	
	JumpEffectStop = 0;
		JumpEffectStop ++;

}



int PlayCoinSound()
{
	channelStates[1].noteIndex = 0; nextNote(1);
	pspAudioSetVolume(1, 0x4000, 0x4000);
	pspAudioSetChannelCallback(1, audioOutCallback1, NULL);
	CoinEffectStop = 0;
	
	CoinEffectStop++;
}



int PlayClickSound()
{
	channelStates[2].noteIndex = 0; nextNote(2);
	pspAudioSetVolume(2, 0x4000, 0x4000);
	pspAudioSetChannelCallback(2, audioOutCallback2, NULL);

ClickEffectStop = 0;
	ClickEffectStop ++;
}