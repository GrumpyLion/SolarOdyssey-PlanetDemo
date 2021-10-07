#pragma once

#include <soloud_wav.h>

struct AudioHandle
{
    int myHandle = 0;
    SoLoud::Wav* myWav = nullptr;
};

namespace AudioManager
{
    void Initialize();
    void Shutdown();

    bool GetSound(const std::string& fileLocation, AudioHandle& handle);
    void UpdateSound(AudioHandle& handle, const Vec3& velocity, const Vec3& position);
    void RemoveSound(AudioHandle& handle);

    void Update();

    void Play(AudioHandle& handle);
    void Play3D(AudioHandle& handle, const Vec3& position);
    void Stop(AudioHandle& handle);

    void SetVolume(AudioHandle& handle, float volume);
    void SetPitch(AudioHandle& handle, float pitch);
    void SetMinMaxDistance(AudioHandle& handle, float min, float max);
    void SetLooping(AudioHandle& handle, bool loops);

    void SetListenerData(const Vec3& position, const Vec3& velocity, const Vec3& direction, const Vec3& up);
};