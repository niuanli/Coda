#pragma once
#include "SoundWorkflow.h"
namespace soundstudio {
enum BuildError { BuildOk, InvalidWave, UnsupportedWave, EmptyAudio, ImageTooLarge, BuildReadFailed, BuildSaveFailed };
struct WaveClip {
    std::vector<Byte> encoded;
    unsigned samples;
    WaveClip() : samples(0) {}
};
// Information Expert: validates RIFF, encodes ISD audio and owns image layout.
// No file APIs, controls or device access. Input is PCM mono, 8000 Hz, 16 bit.
class ImageBuilder {
public:
    enum { MaxEncodedBytes = 44800, MaxWaveBytes = 1024 * 1024 };
    static BuildError Decode(const std::vector<Byte>& wave, WaveClip& clip);
    static BuildError Assemble(const WaveClip& power, const WaveClip& eject, std::vector<Byte>& image);
};
}
