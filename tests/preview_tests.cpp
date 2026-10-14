#include "../src/core/PreviewDecoder.h"
#include "../src/platform/PcmSampleOrder.h"
#include <fstream>
#include <iterator>
#include <cstdio>
#include <cstdlib>
using namespace soundstudio;
#define CHECK(x) do{if(!(x)){printf("FAIL %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static void Put(std::vector<Byte>& w,unsigned at,unsigned n,unsigned bytes){for(unsigned i=0;i<bytes;++i)w[at+i]=(Byte)(n>>(8*i));}
static std::vector<Byte> Wave(unsigned samples){
std::vector<Byte> w(44+samples*2,0);const char* names[]={"RIFF","WAVE","fmt ","data"};unsigned offsets[]={0,8,12,36};
for(unsigned j=0;j<4;++j)for(unsigned i=0;i<4;++i)w[offsets[j]+i]=(Byte)names[j][i];
Put(w,4,(unsigned)w.size()-8,4);Put(w,16,16,4);Put(w,20,1,2);Put(w,22,1,2);Put(w,24,8000,4);Put(w,28,16000,4);Put(w,32,2,2);Put(w,34,16,2);Put(w,40,samples*2,4);
for(unsigned i=0;i<samples;++i)Put(w,44+2*i,i*7919,2);return w;}
int main(){
std::vector<Byte> wave=Wave(21200),pcm,image;WaveClip a,b;CHECK(ImageBuilder::Decode(wave,a)==BuildOk);CHECK(ImageBuilder::Decode(Wave(18400),b)==BuildOk);CHECK(ImageBuilder::Assemble(a,b,image)==BuildOk);std::vector<Byte> valid=image;
CHECK(PreviewDecoder::Wave(wave,pcm));CHECK(pcm.size()==42400);
std::vector<Byte> decoded;CHECK(PreviewDecoder::Image(image,0,decoded));CHECK(decoded.size()==pcm.size());
for(unsigned i=0;i<pcm.size();i+=2){CHECK(decoded[i]==0);CHECK(decoded[i+1]==pcm[i+1]);}
CHECK(PreviewDecoder::Image(image,1,decoded));CHECK(decoded.size()==36800);
// Check the second clip's samples too, rather than just its length.
std::vector<Byte> ejectPcm;CHECK(PreviewDecoder::Wave(Wave(18400),ejectPcm));
for(unsigned i=0;i<decoded.size();i+=2){CHECK(decoded[i]==0);CHECK(decoded[i+1]==ejectPcm[i+1]);}
// Asymmetric bytes expose endianness errors which zero/8-bit samples hide.
const Byte le[]={0x34,0x12,0xCC,0xED,0x00,0x80,0xFF,0x7F};
const Byte be[]={0x12,0x34,0xED,0xCC,0x80,0x00,0x7F,0xFF};
std::vector<Byte> native(le,le+sizeof(le));CHECK(PrepareXboxPcm16(native));
CHECK(native==std::vector<Byte>(be,be+sizeof(be)));
native.assign(1,0);CHECK(!PrepareXboxPcm16(native));native.clear();CHECK(!PrepareXboxPcm16(native));
CHECK(!PreviewDecoder::Image(image,2,decoded));CHECK(decoded.empty());
image[0x3E]=0x77;CHECK(!PreviewDecoder::Image(image,0,decoded));CHECK(decoded.empty());
wave[0]=0;CHECK(!PreviewDecoder::Wave(wave,pcm));CHECK(pcm.empty());
image=valid;image[0x26]=0xff;image[0x27]=0xff;CHECK(!PreviewDecoder::Image(image,0,decoded));
puts("PASS: WAV PCM, BIN sample round-trip across framing blocks, both clips and malformed data");}
