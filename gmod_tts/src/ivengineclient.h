#ifndef IVENGINECLIENT_H
#define IVENGINECLIENT_H
#pragma once

#include "inetchannel.h"

class IVEngineClient
{
public:
	virtual void GetIntersectingSurfaces() = 0;
	virtual void GetLightForPoint() = 0;
	virtual void TraceLineMaterialAndLighting() = 0;
	virtual void ParseFile() = 0;
	virtual void CopyLocalFile() = 0;
	virtual void GetScreenSize() = 0;
	virtual void ServerCmd() = 0;
	virtual void ClientCmd() = 0;
	virtual void GetPlayerInfo() = 0;
	virtual void GetPlayerForUserID() = 0;
	virtual void TextMessageGet() = 0;
	virtual void Con_IsVisible() = 0;
	virtual void GetLocalPlayer() = 0;
	virtual void LoadModel() = 0;
	virtual void Time() = 0;
	virtual void GetLastTimeStamp() = 0;
	virtual void GetSentence() = 0;
	virtual void GetSentenceLength() = 0;
	virtual void IsStreaming() = 0;
	virtual void GetViewAngles() = 0;
	virtual void SetViewAngles() = 0;
	virtual void GetMaxClients() = 0;
	virtual	void Key_LookupBinding() = 0;
	virtual void Key_BindingForKey() = 0;
	virtual void StartKeyTrapMode() = 0;
	virtual void CheckDoneKeyTrapping() = 0;
	virtual void IsInGame() = 0;
	virtual void IsConnected() = 0;
	virtual void IsDrawingLoadingImage() = 0;
	virtual void Con_NPrintf() = 0;
	virtual void Con_NXPrintf() = 0;
	virtual void IsBoxVisible() = 0;
	virtual void IsBoxInViewCluster() = 0;
	virtual void CullBox() = 0;
	virtual void Sound_ExtraUpdate() = 0;
	virtual void GetGameDirectory() = 0;
	virtual void WorldToScreenMatrix() = 0;
	virtual void WorldToViewMatrix() = 0;
	virtual void GameLumpVersion() = 0;
	virtual void GameLumpSize() = 0;
	virtual void LoadGameLump() = 0;
	virtual void LevelLeafCount() = 0;
	virtual void GetBSPTreeQuery() = 0;
	virtual void LinearToGamma() = 0;
	virtual void LightStyleValue() = 0;
	virtual void ComputeDynamicLighting() = 0;
	virtual void GetAmbientLightColor() = 0;
	virtual void GetDXSupportLevel() = 0;
	virtual void SupportsHDR() = 0;
	virtual void Mat_Stub() = 0;
	virtual void GetChapterName() = 0;;
	virtual void GetLevelName() = 0;
	virtual void GetLevelVersion() = 0;
	virtual void GetVoiceTweakAPI() = 0;
	virtual void EngineStats_BeginFrame() = 0;
	virtual void EngineStats_EndFrame() = 0;
	virtual void FireEvents() = 0;
	virtual void GetLeavesArea() = 0;
	virtual void DoesBoxTouchAreaFrustum() = 0;
	virtual void SetAudioState() = 0;
	virtual void SentenceGroupPick() = 0;
	virtual void SentenceGroupPickSequential() = 0;
	virtual void SentenceIndexFromName() = 0;
	virtual void SentenceNameFromIndex() = 0;
	virtual void SentenceGroupIndexFromName() = 0;
	virtual void SentenceGroupNameFromIndex() = 0;
	virtual void SentenceLength() = 0;
	virtual void ComputeLighting() = 0;
	virtual void ActivateOccluder() = 0;
	virtual void IsOccluded() = 0;
	virtual void SaveAllocMemory() = 0;
	virtual void SaveFreeMemory() = 0;

	virtual INetChannelInfo* GetNetChannelInfo() = 0;
};

#define IVENGINECLIENT_INTERFACE "VEngineClient013"

#endif