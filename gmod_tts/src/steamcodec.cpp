#include "steamcodec.h"
#include "crc32.h"

VoiceEncoder_Opus* g_pCodec = new VoiceEncoder_Opus;

enum PayLoadType : uint8
{
	PLT_OPUS_PLC = 6,
	PLT_SamplingRate = 11
};

void InitVoiceCodec()
{
	g_pCodec->Init(0);
}

uint16 GetVoiceSampleRate()
{
	return g_pCodec->m_samplerate;
}

uint16 EncodeVoicePayload(const char* pUncompressedBytes, uint32 nSamples, char* pCompressed, uint32 maxCompressedBytes, bool bFinal)
{
	char* cursor = (char*)pCompressed;

	*(uint32*)cursor = 0x00000011;
	cursor += 4;

	*(uint32*)cursor = 0x01100001;
	cursor += 4;

	*cursor = PLT_SamplingRate;
	++cursor;

	*(uint16*)cursor = g_pCodec->m_samplerate;
	cursor += sizeof(uint16);

	*cursor = PLT_OPUS_PLC;
	++cursor;

	uint16 bytescompressed = g_pCodec->Compress(pUncompressedBytes, nSamples, cursor + 2, maxCompressedBytes - (1 + 2 + 1 + 2), bFinal);

	*(uint16*)cursor = bytescompressed;
	cursor += sizeof(uint16);

	cursor += bytescompressed;

	uint32 checksum = CRC32_ProcessSingleBuffer(pCompressed, cursor - pCompressed);

	*(uint32*)cursor = checksum;
	cursor += sizeof(uint32);

	return cursor - pCompressed;
}