#include "sapimanager.h"

SAPIManager::SAPIManager()
{
	CoInitialize(NULL);

	voice.CoCreateInstance(CLSID_SpVoice);
}

SAPIManager::~SAPIManager()
{
	CoUninitialize();
}

void SAPIManager::ChangeVoice(const wchar_t* attrib)
{
	ISpObjectToken* token = 0;
	SpFindBestToken(SPCAT_VOICES, attrib, L"", &token);

	voice->SetVoice(token);
}

void SAPIManager::Speak(const wchar_t* text, char** buffer, uint32* size)
{
	HRESULT hr = S_OK;
	CComPtr<ISpStream> stream;
	CComPtr<IStream> baseStream;

	GUID guidFormat;
	WAVEFORMATEX* wavFormat = 0;

	stream.CoCreateInstance(CLSID_SpStream);
	CreateStreamOnHGlobal(NULL, TRUE, &baseStream);

	SpConvertStreamFormatEnum(SPSF_24kHz16BitMono, &guidFormat, &wavFormat);
	
	stream->SetBaseStream(baseStream, guidFormat, wavFormat);
	baseStream.Release();

	voice->SetOutput(stream, TRUE);
	voice->Speak(text, SpeechVoiceSpeakFlags::SVSFlagsAsync, NULL);
	voice->WaitUntilDone(-1);
	
	_LARGE_INTEGER a = { 0 };
	stream->Seek(a, STREAM_SEEK_SET, NULL);

	IStream* streamBase;
	stream->GetBaseStream(&streamBase);

	STATSTG stats;
	streamBase->Stat(&stats, STATFLAG_NONAME);

	ULONG byteSize = stats.cbSize.QuadPart;
	ULONG bytesRead = 0;

	*buffer = new char[byteSize];
	streamBase->Read(*buffer, byteSize, &bytesRead);
	*size = bytesRead;

	stream.Release();
}