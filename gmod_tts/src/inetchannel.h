#ifndef INETCHANNEL_H
#define INETCHANNEL_H
#pragma once

#include "bitbuffer.h"

class INetChannelInfo
{
public:
	virtual const char* GetName(void) const = 0;	// get channel name
	virtual const char* GetAddress(void) const = 0; // get channel IP address as string
	virtual float GetTime(void) const = 0;	// current net time
	virtual float GetTimeConnected(void) const = 0;	// get connection time in seconds
	virtual int GetBufferSize(void) const = 0;	// netchannel packet history size
	virtual int GetDataRate(void) const = 0; // send data rate in byte/sec

	virtual bool IsLoopback(void) const = 0;	// true if loopback channel
	virtual bool IsTimingOut(void) const = 0;	// true if timing out
	virtual bool IsPlayback(void) const = 0;	// true if demo playback

	virtual float GetLatency(int flow) const = 0;	 // current latency (RTT), more accurate but jittering
	virtual float GetAvgLatency(int flow) const = 0; // average packet latency in seconds
	virtual float GetAvgLoss(int flow) const = 0;	 // avg packet loss[0..1]
	virtual float GetAvgChoke(int flow) const = 0;	 // avg packet choke[0..1]
	virtual float GetAvgData(int flow) const = 0;	 // data flow in bytes/sec
	virtual float GetAvgPackets(int flow) const = 0; // avg packets/sec
	virtual int GetTotalData(int flow) const = 0;	 // total flow in/out in bytes
	virtual int GetSequenceNr(int flow) const = 0;	// last send seq number
	virtual bool IsValidPacket(int flow, int frame_number) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float GetPacketTime(int flow, int frame_number) const = 0; // time when packet was send
	virtual int GetPacketBytes(int flow, int frame_number, int group) const = 0; // group size of this packet
	virtual bool GetStreamProgress(int flow, int* received, int* total) const = 0;  // TCP progress if transmitting
	virtual float GetTimeSinceLastReceived(void) const = 0;	// get time since last recieved packet in seconds
	virtual	float GetCommandInterpolationAmount(int flow, int frame_number) const = 0;
	virtual void GetPacketResponseLatency(int flow, int frame_number, int* pnLatencyMsecs, int* pnChoke) const = 0;
	virtual void GetRemoteFramerate(float* pflFrameTime, float* pflFrameTimeStdDeviation) const = 0;

	virtual float GetTimeoutSeconds() const = 0;
};

class INetChannel : public INetChannelInfo
{
public:
	virtual	~INetChannel(void) {};

	virtual void SetDataRate(float rate) = 0;
	virtual bool RegisterMessage(void* msg) = 0;
	virtual bool StartStreaming(unsigned int challengeNr) = 0;
	virtual void ResetStreaming(void) = 0;
	virtual void SetTimeout(float seconds) = 0;
	virtual void SetDemoRecorder(void* recorder) = 0;
	virtual void SetChallengeNr(unsigned int chnr) = 0;

	virtual void Reset(void) = 0;
	virtual void Clear(void) = 0;
	virtual void Shutdown(const char* reason) = 0;

	virtual void ProcessPlayback(void) = 0;
	virtual bool ProcessStream(void) = 0;
	virtual void ProcessPacket(struct netpacket_s* packet, bool bHasHeader) = 0;

	virtual bool SendNetMsg(/*INetMessage& msg, bool bForceReliable = false, bool bVoice = false*/) = 0;

	virtual bool SendData(StrojarBitbuffer& msg, bool bReliable = true) = 0;
	virtual bool SendFile(const char* filename, unsigned int transferID) = 0;
	virtual void DenyFile(const char* filename, unsigned int transferID) = 0;
	virtual void RequestFile_OLD(const char* filename, unsigned int transferID) = 0;
	virtual void SetChoked(void) = 0;
	virtual int SendDatagram(StrojarBitbuffer* data) = 0;
	virtual bool Transmit(bool onlyReliable = false) = 0;
};

#endif