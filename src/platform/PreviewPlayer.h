#pragma once
#include <xtl.h>
#include <xaudio2.h>
#include "../core/PreviewDecoder.h"
namespace soundstudio {
class PreviewPlayer {
public:
    PreviewPlayer();~PreviewPlayer();
    bool PlayWave(const std::string& path);
    bool PlayImage(const std::string& path,unsigned clip);
    void Stop();bool Playing();
private:
    IXAudio2* engine_;IXAudio2MasteringVoice* output_;IXAudio2SourceVoice* voice_;
    std::vector<Byte> pcm_;
    bool Play();
    PreviewPlayer(const PreviewPlayer&);PreviewPlayer& operator=(const PreviewPlayer&);
};
}
