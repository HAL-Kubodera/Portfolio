/*============================================================
	[audio.h]
	Å•ëËñº : âπÇÃèoóÕ
	Å•DATE : 2026/03/18
	Å•AUTHOR : ãvï€éõ èrâÓ
============================================================*/
#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <xaudio2.h>
#include "component.h"

class Audio : public Component
{
private:
	static IXAudio2*				m_Xaudio;
	static IXAudio2MasteringVoice*	m_MasteringVoice;

	IXAudio2SourceVoice*	m_SourceVoice{};
	BYTE*					m_SoundData{};

	int						m_Length{};
	int						m_PlayLength{};


public:
	static void InitMaster();
	static void UninitMaster();

	using Component::Component;

	void Uninit();

	void Load(const char *FileName);
	void Play(bool Loop = false,float Volume = 1.0f);
};

#endif // _AUDIO_H_