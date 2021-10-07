#pragma once

struct SimplexNoise
{
    // 1D Perlin simplex noise
    static float Noise(float x);
    // 2D Perlin simplex noise
    static float Noise(float x, float y);
    // 3D Perlin simplex noise
    static float Noise(float x, float y, float z);

    // Fractal/Fractional Brownian Motion (fBm) noise summation
    float Fractal(size_t octaves, float x) const;
    float Fractal(size_t octaves, float x, float y) const;
    float Fractal(size_t octaves, float x, float y, float z) const;

    /**
     * Constructor of to initialize a fractal noise summation
     *
     * @param[in] frequency    Frequency ("width") of the first octave of noise (default to 1.0)
     * @param[in] amplitude    Amplitude ("height") of the first octave of noise (default to 1.0)
     * @param[in] lacunarity   Lacunarity specifies the frequency multiplier between successive octaves (default to 2.0).
     * @param[in] persistence  Persistence is the loss of amplitude between successive octaves (usually 1/lacunarity)
     */
    explicit SimplexNoise(float frequency = 1.0f,
        float amplitude = 1.0f,
        float lacunarity = 2.0f,
        float persistence = 0.5f) :
        myFrequency(frequency),
        myAmplitude(amplitude),
        myLacunarity(lacunarity),
        myPersistence(persistence)
    {
    }

    // Parameters of Fractional Brownian Motion (fBm) : sum of N "octaves" of noise
    float myFrequency;   ///< Frequency ("width") of the first octave of noise (default to 1.0)
    float myAmplitude;   ///< Amplitude ("height") of the first octave of noise (default to 1.0)
    float myLacunarity;  ///< Lacunarity specifies the frequency multiplier between successive octaves (default to 2.0).
    float myPersistence; ///< Persistence is the loss of amplitude between successive octaves (usually 1/lacunarity)
};