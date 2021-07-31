#define GMMODULE
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ivengineclient.h"
IVEngineClient* g_pEngine = 0;

#include <GarrysMod/Lua/Interface.h>
using namespace GarrysMod::Lua;

#include "sapimanager.h"
SAPIManager* g_pSAPI = new SAPIManager;

#include "steamcodec.h"

typedef void*(*FnCreateInterface)(const char* version, int* code);
typedef void(*FnMsg)(const char* format, ...);
FnMsg Msg = 0;

typedef float(*FnPlat_FloatTime)();
FnPlat_FloatTime Plat_FloatTime = 0;

char sampleData[4096];
char compressedData[4096];
char voicePayload[4096];

class StrojarAudioSource
{
public:
	char* m_data = 0;
	float m_starttime = 0;
	uint32 m_curbyte = 0;
	uint32 m_size = 0;
	bool m_finished = true;
public:
	void LoadFromFile(const char* filename)
	{
		FILE* fin = fopen(filename, "rb");
		fseek(fin, 0, SEEK_END);
		m_size = ftell(fin);
		fseek(fin, 0, SEEK_SET);

		m_data = new char[m_size];
		fread(m_data, sizeof(char), m_size, fin);
		fclose(fin);

		m_finished = false;
		m_starttime = 0.f;
		m_curbyte = 0;
	}

	void LoadFromBuffer(char* buff, uint32 size)
	{
		m_size = size;
		m_data = buff;

		m_finished = false;
		m_starttime = 0.f;
		m_curbyte = 0;
	}
	
	uint32 SamplePlay(char* dest, uint32 destSize)
	{
		float curtime = Plat_FloatTime();
		int32 nShouldGet = (int32)((curtime - m_starttime) * GetVoiceSampleRate());

		if (m_finished)
		{
			if (m_data)
			{
				delete m_data;
				m_data = 0;
			}
		}
		else
		{
			if (nShouldGet > 0)
			{
				int32 gotten = min(destSize / BYTES_PER_SAMPLE, min(nShouldGet, (m_size - m_curbyte) / BYTES_PER_SAMPLE));
				
				if (gotten > 0)
				{
					memcpy(dest, &m_data[m_curbyte], gotten * BYTES_PER_SAMPLE);
					m_curbyte += gotten * BYTES_PER_SAMPLE;
					m_starttime = curtime;
					
					return gotten;
				}
				else
				{
					m_finished = true;
				}
			}
		}

		return 0;
	}
};

StrojarAudioSource* audioSrc = new StrojarAudioSource;

static void RunDECtalk(const wchar_t* output, wchar_t* txt)
{
	wchar_t dectalkCmd[1024];
	swprintf(dectalkCmd, L"say.exe -w %s %s", output, txt);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset(&si, 0, sizeof(si));
	si.cb = sizeof(STARTUPINFO);

	CreateProcess(NULL, dectalkCmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

static void RunSAM(const wchar_t* output, wchar_t* txt)
{
	wchar_t samCmd[4096];
	swprintf(samCmd, L"sam.exe -wav %s %s", output, txt);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset(&si, 0, sizeof(si));
	si.cb = sizeof(STARTUPINFO);

	CreateProcess(NULL, samCmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

static void RunFFMPEG(const wchar_t* input, const wchar_t* output, uint16 toRate)
{
	wchar_t ffmpegCmd[1024];
	swprintf(ffmpegCmd, L"ffmpeg.exe -i %s -f s16le -acodec pcm_s16le -ar %hi %s", input, toRate, output);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset(&si, 0, sizeof(si));
	si.cb = sizeof(STARTUPINFO);

	CreateProcess(NULL, ffmpegCmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

int SpeakSAPI(lua_State* state)
{
	if (LUA->IsType(1, Type::STRING))
	{
		const char* text = LUA->GetString();
		char* buf = 0;
		uint32 size = 0;

		uint32 textLen = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
		wchar_t* textW = new wchar_t[textLen];
		MultiByteToWideChar(CP_UTF8, 0, text, -1, textW, textLen);

		g_pSAPI->Speak(textW, &buf, &size);
		
		audioSrc->LoadFromBuffer(buf, size);
	}

	return 1;
}

int SpeakDEC(lua_State* state)
{
	if (LUA->IsType(1, Type::STRING))
	{
		const char* text = LUA->GetString();

		uint32 textLen = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
		wchar_t* textW = new wchar_t[textLen];
		MultiByteToWideChar(CP_UTF8, 0, text, -1, textW, textLen);
		
		RunDECtalk(L"dectalk.wav", textW);
		RunFFMPEG(L"dectalk.wav", L"dectalk.raw", GetVoiceSampleRate());
		
		audioSrc->LoadFromFile("dectalk.raw");
		remove("dectalk.raw");
		remove("dectalk.wav");
	}

	return 1;
}

int SpeakSAM(lua_State* state)
{
	if (LUA->IsType(1, Type::STRING))
	{
		const char* text = LUA->GetString();

		uint32 textLen = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
		wchar_t* textW = new wchar_t[textLen];
		MultiByteToWideChar(CP_UTF8, 0, text, -1, textW, textLen);

		RunSAM(L"sam.wav", textW);
		RunFFMPEG(L"sam.wav", L"sam.raw", GetVoiceSampleRate());

		audioSrc->LoadFromFile("sam.raw");
		remove("sam.raw");
		remove("sam.wav");
	}

	return 1;
}

int TickVoice(lua_State* state)
{
	uint32 nSamples = audioSrc->SamplePlay(sampleData, sizeof(sampleData));

	if (nSamples > 0)
	{
		uint16 nCompressed = EncodeVoicePayload(sampleData, nSamples, compressedData, sizeof(compressedData), false);
		
		StrojarBitbuffer pck(voicePayload, sizeof(voicePayload));
		pck.WriteUBitLong(10, 6);
		pck.WriteWord(nCompressed * 8);
		pck.WriteBits(compressedData, nCompressed * 8);

		INetChannel* netchan = (INetChannel*)g_pEngine->GetNetChannelInfo();
		netchan->SendData(pck, true);
	}

	return 1;
}

int ChangeVoiceSAPI(lua_State* state)
{
	if (LUA->IsType(1, Type::STRING))
	{
		const char* text = LUA->GetString();

		uint32 textLen = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
		wchar_t* textW = new wchar_t[textLen];
		MultiByteToWideChar(CP_UTF8, 0, text, -1, textW, textLen);

		g_pSAPI->ChangeVoice(textW);
	}

	return 1;
}

int FinishedPlayingSample(lua_State* state)
{
	LUA->PushBool(audioSrc->m_finished);

	return 1;
}

int ConvertFFMPEG(lua_State* state)
{
	if (LUA->IsType(1, Type::STRING) && LUA->IsType(2, Type::STRING))
	{
		const char* text1 = LUA->GetString(-1);
		const char* text2 = LUA->GetString(-2);
		uint32 textLen = 0;

		textLen = MultiByteToWideChar(CP_UTF8, 0, text1, -1, NULL, 0);
		wchar_t* outputFile = new wchar_t[textLen];
		MultiByteToWideChar(CP_UTF8, 0, text1, -1, outputFile, textLen);

		textLen = MultiByteToWideChar(CP_UTF8, 0, text2, -1, NULL, 0);
		wchar_t* inputFile = new wchar_t[textLen];
		MultiByteToWideChar(CP_UTF8, 0, text2, -1, inputFile, textLen);

		RunFFMPEG(inputFile, outputFile, GetVoiceSampleRate());
	}

	return 1;
}

int PlaySample(lua_State* state)
{
	if (LUA->IsType(1, Type::STRING))
	{
		audioSrc->LoadFromFile(LUA->GetString());
	}

	return 1;
}

GMOD_MODULE_OPEN()
{
	Msg = (FnMsg)GetProcAddress(GetModuleHandleA("tier0.dll"), "Msg");
	Plat_FloatTime = (FnPlat_FloatTime)GetProcAddress(GetModuleHandleA("tier0.dll"), "Plat_FloatTime");

	FnCreateInterface engineFactory = (FnCreateInterface)GetProcAddress(GetModuleHandleA("engine.dll"), "CreateInterface");
	g_pEngine = (IVEngineClient*)engineFactory(IVENGINECLIENT_INTERFACE, 0);

	if (g_pEngine)
	{
		Msg("[gmod_tts] Engine interface acquired.\n");
	}
	else
	{
		Msg("[gmod_tts] Failed to acquire engine interface.\n");
	}

	InitVoiceCodec();
	Msg("[gmod_tts] Opus voice codec ready.\n");

	LUA->PushSpecial(SPECIAL_GLOB);
	LUA->PushString("SpeakSAPI");
	LUA->PushCFunction(SpeakSAPI);
	LUA->SetTable(-3);

	LUA->PushSpecial(SPECIAL_GLOB);
	LUA->PushString("TickVoice");
	LUA->PushCFunction(TickVoice);
	LUA->SetTable(-3);

	LUA->PushSpecial(SPECIAL_GLOB);
	LUA->PushString("ChangeVoiceSAPI");
	LUA->PushCFunction(ChangeVoiceSAPI);
	LUA->SetTable(-3);

	LUA->PushSpecial(SPECIAL_GLOB);
	LUA->PushString("FinishedPlayingSample");
	LUA->PushCFunction(FinishedPlayingSample);
	LUA->SetTable(-3);

	LUA->PushSpecial(SPECIAL_GLOB);
	LUA->PushString("SpeakDEC");
	LUA->PushCFunction(SpeakDEC);
	LUA->SetTable(-3);

	LUA->PushSpecial(SPECIAL_GLOB);
	LUA->PushString("SpeakSAM");
	LUA->PushCFunction(SpeakSAM);
	LUA->SetTable(-3);

	LUA->PushSpecial(SPECIAL_GLOB);
	LUA->PushString("ConvertFFMPEG");
	LUA->PushCFunction(ConvertFFMPEG);
	LUA->SetTable(-3);

	LUA->PushSpecial(SPECIAL_GLOB);
	LUA->PushString("PlaySample");
	LUA->PushCFunction(PlaySample);
	LUA->SetTable(-3);
	
	return 0;
}

GMOD_MODULE_CLOSE()
{
	return 0;
}