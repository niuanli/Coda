#pragma once
#include "../core/SoundWorkflow.h"
namespace soundstudio {
// Decoders return WAV-compatible little-endian PCM. Xbox XAudio2 consumes
// native (big-endian) 16-bit samples; convert at the device boundary only.
inline bool PrepareXboxPcm16(std::vector<Byte>& pcm) {
    if(pcm.empty() || (pcm.size()&1))return false;
    for(size_t i=0;i<pcm.size();i+=2){Byte low=pcm[i];pcm[i]=pcm[i+1];pcm[i+1]=low;}
    return true;
}
}
