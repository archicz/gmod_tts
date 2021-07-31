#ifndef STEAMCODEC_H
#define STEAMCODEC_H
#pragma once

#include "common.h"

#include "voicecodec.h"
extern VoiceEncoder_Opus* g_pCodec;

void InitVoiceCodec();
uint16 GetVoiceSampleRate();
uint16 EncodeVoicePayload(const char* pUncompressedBytes, uint32 nSamples, char* pCompressed, uint32 maxCompressedBytes, bool bFinal);

#endif