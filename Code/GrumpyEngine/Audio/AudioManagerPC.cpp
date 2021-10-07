#include "pch.h"

#if GE_PLATFORM_PC
#include <soloud.h>
#include <soloud_wavstream.h>
#include <soloud_sfxr.h>
#include <soloud_speech.h>
#include <soloud_noise.h>

#include "AudioManager.h"

namespace AudioManager
{
    SoLoud::Soloud locInstance;
}

void AudioManager::Initialize()
{
    locInstance.init(SoLoud::Soloud::CLIP_ROUNDOFF | SoLoud::Soloud::ENABLE_VISUALIZATION, SoLoud::Soloud::BACKENDS::MINIAUDIO);
}

void AudioManager::Shutdown()
{
    locInstance.deinit();
}

bool AudioManager::GetSound(const std::string& filelocation, AudioHandle& handle)
{
    delete handle.myWav;
    handle.myWav = nullptr;

    handle.myWav = new SoLoud::Wav();
    handle.myWav->load(filelocation.c_str());
    return true;
}

void AudioManager::UpdateSound(AudioHandle& handle, const Vec3& velocity, const Vec3& position)
{
    locInstance.set3dSourcePosition(handle.myHandle, position.x, position.y, position.z);
    locInstance.set3dSourceVelocity(handle.myHandle, velocity.x, velocity.y, velocity.z);
}

void AudioManager::RemoveSound(AudioHandle& handle)
{
    delete handle.myWav;
    handle.myWav = nullptr;
    handle.myHandle = 0;
}

void AudioManager::Update()
{
    locInstance.update3dAudio();
}

void AudioManager::Play(AudioHandle& handle)
{
    handle.myHandle = locInstance.play(*handle.myWav);
}

void AudioManager::Play3D(AudioHandle& handle, const Vec3& position)
{
    handle.myHandle = locInstance.play3d(*handle.myWav, position.x, position.y, position.z);
}

void AudioManager::Stop(AudioHandle& handle)
{
    locInstance.stop(handle.myHandle);
}

void AudioManager::SetVolume(AudioHandle& handle, float volume)
{
    locInstance.setVolume(handle.myHandle, volume);
}

void AudioManager::SetPitch(AudioHandle& handle, float pitch)
{
    locInstance.setRelativePlaySpeed(handle.myHandle, pitch);
}

void AudioManager::SetMinMaxDistance(AudioHandle& handle, float min, float max)
{
    locInstance.set3dSourceMinMaxDistance(handle.myHandle, min, max);
    locInstance.set3dSourceAttenuation(handle.myHandle, SoLoud::AudioSource::EXPONENTIAL_DISTANCE, 0.5f);
}

void AudioManager::SetLooping(AudioHandle& handle, bool loops)
{
    locInstance.setLooping(handle.myHandle, loops);
}

void AudioManager::SetListenerData(const Vec3& position, const Vec3& velocity, const Vec3& direction, const Vec3& up)
{
    locInstance.set3dListenerPosition(position.x, position.y, position.z);
    locInstance.set3dListenerVelocity(velocity.x, velocity.y, velocity.z);
    locInstance.set3dListenerUp(up.x, up.y, up.z);
    locInstance.set3dListenerAt(direction.x, direction.y, direction.z);
}
#endif