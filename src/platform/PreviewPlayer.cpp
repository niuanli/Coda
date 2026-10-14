#include "PreviewPlayer.h"
#include "XboxServices.h"
#include "PcmSampleOrder.h"
namespace soundstudio {
PreviewPlayer::PreviewPlayer():engine_(NULL),output_(NULL),voice_(NULL){}
PreviewPlayer::~PreviewPlayer(){Stop();if(output_)output_->DestroyVoice();if(engine_)engine_->Release();}
void PreviewPlayer::Stop(){if(voice_){voice_->Stop();voice_->DestroyVoice();voice_=NULL;}pcm_.clear();}
bool PreviewPlayer::Playing(){if(!voice_)return false;XAUDIO2_VOICE_STATE state;voice_->GetState(&state);return state.BuffersQueued!=0;}
bool PreviewPlayer::PlayWave(const std::string& path){
    Stop();std::vector<Byte> file;
    return XboxWorkshopFiles::ReadWave(path,file)&&PreviewDecoder::Wave(file,pcm_)&&Play();
}
bool PreviewPlayer::PlayImage(const std::string& path,unsigned clip){
    Stop();std::vector<Byte> file;XboxImageStore store;
    return store.Load(path,file)&&PreviewDecoder::Image(file,clip,pcm_)&&Play();
}
bool PreviewPlayer::Play(){
    if(!PrepareXboxPcm16(pcm_))return false;
    if(!engine_ && FAILED(XAudio2Create(&engine_)))return false;
    if(!output_ && FAILED(engine_->CreateMasteringVoice(&output_)))return false;
    WAVEFORMATEX format;ZeroMemory(&format,sizeof(format));
    format.wFormatTag=WAVE_FORMAT_PCM;format.nChannels=1;format.nSamplesPerSec=8000;
    format.wBitsPerSample=16;format.nBlockAlign=2;format.nAvgBytesPerSec=16000;
    if(FAILED(engine_->CreateSourceVoice(&voice_,&format)))return false;
    XAUDIO2_BUFFER buffer;ZeroMemory(&buffer,sizeof(buffer));
    buffer.Flags=XAUDIO2_END_OF_STREAM;buffer.AudioBytes=(UINT32)pcm_.size();buffer.pAudioData=&pcm_[0];
    if(FAILED(voice_->SubmitSourceBuffer(&buffer))||FAILED(voice_->Start())){Stop();return false;}
    return true;
}
}
