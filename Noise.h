#ifndef NOISE_H
#define NOISE_H

class Noise
{
public:
    Noise (
        int seed = 0,
        double frequency = 1.0,
        double amplitude = 1.0,
        double lacunarity = 2.0,
        double persistence = 0.5);

    int seed () const;

    double generate (double x, unsigned int layers = 1,
        double cycle = 0.0) const;

    double generate (double x, double y, unsigned int layers = 1) const;

private:
    int mSeed;
    double mFrequency;
    double mAmplitude;
    double mLacunarity;
    double mPersistence;
};

#endif // NOISE_H
