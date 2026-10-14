#include "../src/core/ImageBuildWorkflow.h"
#include <cstdio>
#include <cstdlib>
using namespace soundstudio;
#define CHECK(x) do{if(!(x)){printf("FAIL %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static void Put(std::vector<Byte>& data,unsigned pos,unsigned value,unsigned bytes) { for(unsigned i=0;i<bytes;++i)data[pos+i]=(Byte)(value>>(i*8)); }
static std::vector<Byte> Wave(unsigned samples) {
    std::vector<Byte> w(44+samples*2,0); const char* ids[]={"RIFF","WAVE","fmt ","data"};unsigned offsets[]={0,8,12,36};
    for(unsigned j=0;j<4;++j)for(unsigned i=0;i<4;++i)w[offsets[j]+i]=(Byte)ids[j][i];
    Put(w,4,(unsigned)w.size()-8,4);Put(w,16,16,4);Put(w,20,1,2);Put(w,22,1,2);Put(w,24,8000,4);Put(w,28,16000,4);Put(w,32,2,2);Put(w,34,16,2);Put(w,40,samples*2,4);
    for(unsigned i=0;i<samples;++i){w[44+i*2]=(Byte)i;w[45+i*2]=(Byte)(i%256);}
    return w;
}
struct Store: IWorkshopStore {
    bool failPower,failEject,failSave;unsigned saves,reads;
    std::vector<Byte> first,second,saved;
    Store():failPower(false),failEject(false),failSave(false),saves(0),reads(0),first(Wave(8000)),second(Wave(16000)){}
    bool ReadWave(const std::string& path,std::vector<Byte>& data,unsigned& error){++reads;error=0;bool power=path=="power.wav";if(power?failPower:failEject){error=5;return false;}data=power?first:second;return true;}
    bool SaveImage(const std::vector<Byte>& image,std::string& path){++saves;if(failSave)return false;saved=image;path="output.bin";return true;}
};
static void Finish(ImageBuildWorkflow& flow){for(unsigned i=0;i<10&&flow.Busy();++i)flow.Tick();CHECK(!flow.Busy());}
int main(){
{Store store;ImageBuildWorkflow flow(store);flow.Begin("power.wav","eject.wav");Finish(flow);CHECK(flow.Status().stage==Complete);CHECK(flow.Status().percent==100);CHECK(store.reads==2&&store.saves==1);CHECK(store.saved.size()==ImageSize);CHECK(flow.Status().outputPath=="output.bin");}
{Store store;store.failPower=true;ImageBuildWorkflow flow(store);flow.Begin("power.wav","eject.wav");Finish(flow);CHECK(flow.Status().message==MsgBuildReadFailed);CHECK(flow.Status().inputPath=="power.wav");CHECK(flow.Status().fileError==5);CHECK(store.reads==1&&store.saves==0);}
{Store store;store.failEject=true;ImageBuildWorkflow flow(store);flow.Begin("power.wav","eject.wav");Finish(flow);CHECK(flow.Status().inputPath=="eject.wav");CHECK(store.saves==0);}
{Store store;store.second[0]=0;ImageBuildWorkflow flow(store);flow.Begin("power.wav","eject.wav");Finish(flow);CHECK(flow.Status().message==MsgBuildInvalid);CHECK(flow.Status().inputPath=="eject.wav");CHECK(store.saves==0);}
{Store store;store.failSave=true;ImageBuildWorkflow flow(store);flow.Begin("power.wav","eject.wav");Finish(flow);CHECK(flow.Status().message==MsgBuildSaveFailed);}
{Store store;ImageBuildWorkflow flow(store);flow.Begin("power.wav","eject.wav");for(int i=0;i<5;++i)flow.Tick();flow.Cancel();Finish(flow);CHECK(flow.Status().stage==Cancelled);CHECK(store.saves==0);}
puts("PASS: image build success, source diagnostics, invalid input, save failure and cancel before save");}
