#ifndef VOICECODEC_H
#define VOICECODEC_H
#pragma once

#include <opus.h>

#include "common.h"
#include "buffer.h"

const uint32 BYTES_PER_SAMPLE = 2;
const uint32 MAX_CHANNELS = 1;
const uint32 FRAME_SIZE = 480;
const uint32 MAX_FRAME_SIZE = 3 * FRAME_SIZE;
const uint32 MAX_PACKET_LOSS = 10;

class VoiceEncoder_Opus
{
public:
	OpusEncoder* m_pEncoder;
	OpusDecoder* m_pDecoder;
	StrojarBuffer m_bufOverflowBytes;

	int32 m_samplerate;
	int32 m_bitrate;

	uint16 m_nCurFrame;
	uint16 m_nLastFrame;

	bool m_PacketLossConcealment;
public:
	VoiceEncoder_Opus();
	virtual ~VoiceEncoder_Opus();

	bool Init(int quality);
	void Release();
	bool ResetState();
	int Compress(const char* pUncompressedBytes, int nSamples, char* pCompressed, int maxCompressedBytes, bool bFinal);
	int Decompress(const char* pCompressed, int compressedBytes, char* pUncompressed, int maxUncompressedBytes);

	int GetNumQueuedEncodingSamples() const { return m_bufOverflowBytes.TellPut() / BYTES_PER_SAMPLE; }
};

#endif