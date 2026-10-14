#include "../src/core/ImageBuilder.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iterator>
using namespace soundstudio;
#define CHECK(x) do { if(!(x)) { std::printf("FAIL line %d: %s\n",__LINE__,#x); std::exit(1); } } while(0)
static void Put(std::vector<Byte>& data,unsigned pos,unsigned value,unsigned bytes) { for(unsigned i=0;i<bytes;++i)data[pos+i]=(Byte)(value>>(i*8)); }
static std::vector<Byte> Wave(unsigned samples) {
    std::vector<Byte> w(44+samples*2,0); const char* ids[]={"RIFF","WAVE","fmt ","data"};unsigned offsets[]={0,8,12,36};
    for(unsigned j=0;j<4;++j)for(unsigned i=0;i<4;++i)w[offsets[j]+i]=(Byte)ids[j][i];
    Put(w,4,(unsigned)w.size()-8,4);Put(w,16,16,4);Put(w,20,1,2);Put(w,22,1,2);Put(w,24,8000,4);Put(w,28,16000,4);Put(w,32,2,2);Put(w,34,16,2);Put(w,40,samples*2,4);
    for(unsigned i=0;i<samples;++i){w[44+i*2]=(Byte)i;w[45+i*2]=(Byte)(i%256);}
    return w;
}
static std::vector<Byte> Load(const char* path){std::ifstream f(path,std::ios::binary);return std::vector<Byte>(std::istreambuf_iterator<char>(f),std::istreambuf_iterator<char>());}
int main(int argc,char** argv) {
    if(argc==4){WaveClip a,b;CHECK(ImageBuilder::Decode(Load(argv[1]),a)==BuildOk);CHECK(ImageBuilder::Decode(Load(argv[2]),b)==BuildOk);std::vector<Byte> image;CHECK(ImageBuilder::Assemble(a,b,image)==BuildOk);std::ofstream f(argv[3],std::ios::binary);f.write((const char*)&image[0],image.size());return f.good()?0:1;}
    WaveClip a,b; std::vector<Byte> image;
    CHECK(ImageBuilder::Decode(Wave(4091),a)==BuildOk); CHECK(a.encoded[0]==0x78);CHECK(a.encoded[4091]==0x78);CHECK(a.encoded.size()==4093);CHECK(a.encoded[1]==0);CHECK(a.encoded[2]==1);
    CHECK(ImageBuilder::Decode(Wave(8000),b)==BuildOk);CHECK(ImageBuilder::Assemble(a,b,image)==BuildOk);CHECK(image.size()==45056);CHECK(image[0]==0xCF);CHECK(image.back()==0xFF);
    std::vector<Byte> invalid=Wave(1);invalid[0]=0;CHECK(ImageBuilder::Decode(invalid,a)==InvalidWave);
    invalid=Wave(1);Put(invalid,40,99999,4);CHECK(ImageBuilder::Decode(invalid,a)==InvalidWave);
    invalid=Wave(1);Put(invalid,22,2,2);CHECK(ImageBuilder::Decode(invalid,a)==UnsupportedWave);
    invalid=Wave(1);Put(invalid,24,44100,4);CHECK(ImageBuilder::Decode(invalid,a)==UnsupportedWave);
    invalid=Wave(1);Put(invalid,34,8,2);CHECK(ImageBuilder::Decode(invalid,a)==UnsupportedWave);
    CHECK(ImageBuilder::Decode(Wave(0),a)==EmptyAudio);CHECK(ImageBuilder::Decode(Wave(50000),a)==ImageTooLarge);
    CHECK(ImageBuilder::Decode(Wave(30000),a)==BuildOk);CHECK(ImageBuilder::Decode(Wave(30000),b)==BuildOk);CHECK(ImageBuilder::Assemble(a,b,image)==ImageTooLarge);CHECK(image.empty());
    a.encoded.assign(22400,0x78);b.encoded.assign(22400,0x78);CHECK(ImageBuilder::Assemble(a,b,image)==BuildOk);b.encoded.push_back(1);CHECK(ImageBuilder::Assemble(a,b,image)==ImageTooLarge);
    invalid=Wave(1); invalid.insert(invalid.begin()+36,{'J','U','N','K',1,0,0,0,0x12,0});Put(invalid,4,(unsigned)invalid.size()-8,4);CHECK(ImageBuilder::Decode(invalid,a)==BuildOk);
    std::puts("PASS: WAV validation, framing boundaries, image capacity and RIFF padding");
}
