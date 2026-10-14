#pragma once
#include "ImageBuilder.h"
namespace soundstudio {
// File decoding is independent of the audio device and of the UI.
// Both methods return signed 16-bit, mono, 8 kHz PCM in little-endian order.
class PreviewDecoder {
public:
    static bool Wave(const std::vector<Byte>& file, std::vector<Byte>& pcm);
    static bool Image(const std::vector<Byte>& file, unsigned clip, std::vector<Byte>& pcm);
};
}
