#include "../src/core/SoundWorkflow.h"
#include "../src/ui/Locale.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace soundstudio;
#define CHECK(value) do { if (!(value)) { std::printf("FAIL line %d: %s\n", __LINE__, #value); std::exit(1); } } while(0)
struct Device : ISoundDevice {
    std::vector<Byte> data;
    bool connected, unstable, eraseFail, writeFail, verifyFail;
    unsigned reads, erases, writes;
    Device() : data(ImageSize, 0x11), connected(true), unstable(false), eraseFail(false), writeFail(false), verifyFail(false), reads(0), erases(0), writes(0) { data[0] = 0xCF; }
    bool Connect() { return connected; } bool PowerUp() { return true; }
    bool Read(unsigned page, Byte* bytes) {
        std::memcpy(bytes, &data[page * ReadSize], ReadSize); ++reads;
        if ((unstable && reads > ImageSize / ReadSize) || (verifyFail && writes)) bytes[1] ^= 1;
        return true;
    }
    bool Erase() { ++erases; if (eraseFail) return false; data.assign(ImageSize, 0xFF); return true; }
    bool Write(unsigned page, const Byte* bytes) { ++writes; if (writeFail) return false; std::memcpy(&data[page * WriteSize], bytes, WriteSize); return true; }
    bool Play(unsigned) { return true; }
};
struct Store : IImageStore {
    std::vector<Byte> image, backup;
    bool saveFail, loadFail;
    unsigned saves;
    Store() : image(ImageSize, 0x22), saveFail(false), loadFail(false), saves(0) { image[0] = 0xCF; }
    bool Load(const std::string&, std::vector<Byte>& result) { result = image; return !loadFail; }
    bool SaveVerifiedBackup(const std::vector<Byte>& value, std::string& path) { ++saves; if (saveFail) return false; backup = value; path = "Dumps/backup.bin"; return true; }
};
static void Finish(SoundWorkflow& flow) { unsigned ticks = 0; while (flow.Busy() && ticks++ < 1000) flow.Tick(); CHECK(!flow.Busy()); }
int main() {
    { Locale language;
      CHECK(language.Text(TxtHomeTitle)[0] == 0x0414);
      for (unsigned i=0; i<TxtCount; ++i) CHECK(language.Text((TextId)i)[0] != 0);
      for (unsigned i=1; i<MsgCount; ++i) CHECK(language.Message((OperationMessage)i)[0] != 0);
      language.SetRussian(false); CHECK(language.Text(TxtHomeTitle)[0] == L'G');
      CHECK(language.Text((TextId)999)[0] == 0); CHECK(language.Message((OperationMessage)999)[0] == 0);
      for (unsigned i=0; i<TxtCount; ++i) CHECK(language.Text((TextId)i)[0] != 0);
      for (unsigned i=1; i<MsgCount; ++i) CHECK(language.Message((OperationMessage)i)[0] != 0);
      language.SetRussian(true); CHECK(language.Text(TxtHomeTitle)[0] == 0x0414); }
    { Device d; Store s; SoundWorkflow f(d,s); std::vector<Byte> original=d.data;
      CHECK(f.BeginInstall("sound.bin")); Finish(f); CHECK(f.Status().stage==Complete);
      CHECK(d.data==s.image); CHECK(s.backup==original); CHECK(d.erases==1); CHECK(d.writes==ImageSize/WriteSize); }
    { Device d; Store s; s.saveFail=true; SoundWorkflow f(d,s); f.BeginInstall("x"); Finish(f); CHECK(f.Status().stage==Failed); CHECK(d.erases==0); }
    { Device d; Store s; d.unstable=true; SoundWorkflow f(d,s); f.BeginInstall("x"); Finish(f); CHECK(f.Status().stage==Failed); CHECK(s.saves==0); CHECK(d.erases==0); }
    { Device d; Store s; d.connected=false; SoundWorkflow f(d,s); f.BeginInstall("x"); Finish(f); CHECK(f.Status().stage==Failed); CHECK(d.erases==0); }
    { Device d; Store s; s.image[0]=0; SoundWorkflow f(d,s); CHECK(!f.BeginInstall("x")); CHECK(d.erases==0); CHECK(d.reads==0); }
    { Device d; Store s; s.image.pop_back(); SoundWorkflow f(d,s); CHECK(!f.BeginInstall("x")); CHECK(d.erases==0); }
    { Device d; Store s; SoundWorkflow f(d,s); f.BeginBackup(); Finish(f); CHECK(f.Status().stage==Complete); CHECK(s.backup==d.data); CHECK(d.erases==0); }
    { Device d; Store s; SoundWorkflow f(d,s); f.BeginInstall("x"); f.Tick(); CHECK(f.Cancel()); Finish(f); CHECK(f.Status().stage==Cancelled); CHECK(d.erases==0); }
    { Device d; Store s; d.eraseFail=true; SoundWorkflow f(d,s); f.BeginInstall("x"); Finish(f); CHECK(f.Status().stage==Failed); CHECK(f.Status().chipMayBeModified); CHECK(!f.Status().backupPath.empty()); }
    { Device d; Store s; d.writeFail=true; SoundWorkflow f(d,s); f.BeginInstall("x"); Finish(f); CHECK(f.Status().stage==Failed); CHECK(!f.Status().backupPath.empty()); }
    { Device d; Store s; d.verifyFail=true; SoundWorkflow f(d,s); f.BeginInstall("x"); Finish(f); CHECK(f.Status().stage==Failed); CHECK(!f.Status().backupPath.empty()); }
    { Device d; Store s; SoundWorkflow f(d,s); f.BeginInstall("x"); while(f.Status().stage!=Writing) f.Tick(); CHECK(!f.Cancel()); CHECK(!f.BeginBackup()); Finish(f); }
    std::puts("PASS: 12 workflow scenarios"); return 0;
}
