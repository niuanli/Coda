#include "PreviewDecoder.h"
#include <cstring>
namespace soundstudio {
static unsigned U16(const Byte* p){return p[0]|(unsigned(p[1])<<8);}
static unsigned U32(const Byte* p){return U16(p)|(U16(p+2)<<16);}
bool PreviewDecoder::Wave(const std::vector<Byte>& file,std::vector<Byte>& pcm){
    pcm.clear();WaveClip validated;
    if(ImageBuilder::Decode(file,validated)!=BuildOk)return false;
    unsigned end=U32(&file[4])+8;
    for(unsigned at=12;at<end;){
        unsigned size=U32(&file[at+4]);
        if(!std::memcmp(&file[at],"data",4)){pcm.assign(file.begin()+at+8,file.begin()+at+8+size);return true;}
        at+=8+size+(size&1);
    }
    return false;
}
bool PreviewDecoder::Image(const std::vector<Byte>& file,unsigned clip,std::vector<Byte>& pcm){
    pcm.clear();if(!SoundWorkflow::ValidImage(file)||clip>1)return false;
    unsigned field=clip?0x29:0x23;
    unsigned start=U16(&file[field]),last=U16(&file[field+3]);
    unsigned table=U16(&file[5]);
    if(start<0x3E||last<start||last>=table||table>=file.size())return false;
    for(unsigned at=start;at<=last;++at){
        if((at-start)%0xFFB==0){
            // Only the PCM framing used by the supported ISD images is previewable.
            if(file[at]!=0x78){pcm.clear();return false;}
        }else{pcm.push_back(0);pcm.push_back(file[at]);}
    }
    return !pcm.empty();
}
}
