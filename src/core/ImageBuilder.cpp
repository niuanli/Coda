#include "ImageBuilder.h"
#include "ImageTemplate.h"
#include <cstring>
namespace soundstudio {
static unsigned LE16(const Byte* p) { return (unsigned)p[0] | ((unsigned)p[1] << 8); }
static unsigned LE32(const Byte* p) { return LE16(p) | (LE16(p+2) << 16); }
static void Put16(std::vector<Byte>& image, unsigned offset, unsigned value) {
    image[offset]=(Byte)value; image[offset+1]=(Byte)(value>>8);
}
BuildError ImageBuilder::Decode(const std::vector<Byte>& wave, WaveClip& clip) {
    clip=WaveClip();
    if (wave.size()<12 || wave.size()>MaxWaveBytes || std::memcmp(&wave[0],"RIFF",4) || std::memcmp(&wave[8],"WAVE",4)) return InvalidWave;
    unsigned size=LE32(&wave[4]);
    if (size<4 || size>wave.size()-8) return InvalidWave;
    const unsigned end=size+8;
    bool format=false, dataFound=false; unsigned dataOffset=0,dataSize=0;
    for (unsigned offset=12; offset<end;) {
        if (end-offset<8) return InvalidWave;
        unsigned length=LE32(&wave[offset+4]);
        if (length>end-offset-8) return InvalidWave;
        if (!std::memcmp(&wave[offset],"fmt ",4)) {
            if (format || length<16) return InvalidWave;
            const Byte* f=&wave[offset+8];
            if (LE16(f)!=1 || LE16(f+2)!=1 || LE32(f+4)!=8000 || LE16(f+14)!=16 || LE16(f+12)!=2 || LE32(f+8)!=16000) return UnsupportedWave;
            format=true;
        } else if (!std::memcmp(&wave[offset],"data",4)) {
            if (dataFound) return InvalidWave;
            dataFound=true; dataOffset=offset+8; dataSize=length;
        }
        unsigned padded=length+(length&1);
        if (padded>end-offset-8) return InvalidWave;
        offset+=8+padded;
    }
    if (!format || !dataFound || (dataSize&1)) return InvalidWave;
    if (!dataSize) return EmptyAudio;
    clip.samples=dataSize/2;
    // Same framing as J-Runner: 0x78 at encoded offsets divisible by 0xFFB,
    // followed by the high byte of each signed little-endian 16-bit sample.
    if (clip.samples>MaxEncodedBytes) return ImageTooLarge;
    clip.encoded.reserve(clip.samples+clip.samples/4090+1);
    for (unsigned i=0; i<clip.samples; ++i) {
        if (clip.encoded.size()%0xFFB==0) clip.encoded.push_back(0x78);
        clip.encoded.push_back(wave[dataOffset+i*2+1]);
    }
    if (clip.encoded.size()>MaxEncodedBytes) { clip.encoded.clear(); return ImageTooLarge; }
    return BuildOk;
}
BuildError ImageBuilder::Assemble(const WaveClip& power, const WaveClip& eject, std::vector<Byte>& image) {
    image.clear();
    if (power.encoded.empty() || eject.encoded.empty()) return EmptyAudio;
    if (power.encoded.size()>MaxEncodedBytes || eject.encoded.size()>MaxEncodedBytes-power.encoded.size()) return ImageTooLarge;
    unsigned count=(unsigned)(power.encoded.size()+eject.encoded.size());
    if (sizeof(IsdHeader)+count+sizeof(IsdTable)>ImageSize) return ImageTooLarge;
    image.assign(ImageSize,0xFF);
    std::memcpy(&image[0],IsdHeader,sizeof(IsdHeader));
    const unsigned base=count+0x3E;
    const unsigned fields[]={5,8,0xB,0xE,0x11,0x14,0x17,0x1A,0x1D,0x20};
    const unsigned relative[]={0,0x24,0x25,0x43,0x44,0x62,0x63,0x6C,0x6D,0x76};
    for (unsigned i=0;i<10;++i) Put16(image,fields[i],base+relative[i]);
    Put16(image,0x23,0x3E);
    Put16(image,0x26,(unsigned)power.encoded.size()-1+0x3E);
    Put16(image,0x29,(unsigned)power.encoded.size()+0x3E);
    Put16(image,0x2C,count-1+0x3E);
    std::memcpy(&image[0x3E],&power.encoded[0],power.encoded.size());
    std::memcpy(&image[0x3E+power.encoded.size()],&eject.encoded[0],eject.encoded.size());
    std::memcpy(&image[base],IsdTable,sizeof(IsdTable));
    return BuildOk;
}
}
