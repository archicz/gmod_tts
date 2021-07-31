#ifndef SAPIMANAGER_H
#define SAPIMANAGER_H
#pragma once

#include <sphelper.h>
#include <sapi.h>

#include "common.h"

class SAPIManager
{
public:
	CComPtr<ISpVoice> voice;
public:
	SAPIManager();
	~SAPIManager();
public:
	void ChangeVoice(const wchar_t* attrib);
	void Speak(const wchar_t* text, char** buffer, uint32* size);
};

#endif