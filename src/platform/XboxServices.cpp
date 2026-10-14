#include "XboxServices.h"
#include "../../vendor/ISDFlasher.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
extern "C" {
    typedef struct { USHORT Length, MaximumLength; char* Buffer; } NativeString;
    void __stdcall RtlInitAnsiString(NativeString* destination, const char* source);
    LONG __stdcall ObCreateSymbolicLink(NativeString* link, NativeString* target);
}
namespace soundstudio {
bool LanguageSettings::LoadRussian() {
    HANDLE file = CreateFile("GAME:\\coda-language.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return true;
    char data[3] = {0}; DWORD read = 0;
    bool english = ::ReadFile(file, data, 2, &read, NULL) && read == 2 && data[0] == 'e' && data[1] == 'n';
    CloseHandle(file); return !english;
}
bool LanguageSettings::SaveRussian(bool russian) {
    // A two-byte preference does not need a rename transaction. Some Xbox/VFS
    // environments cannot rename the temporary file even when writes work.
    HANDLE file = CreateFile("GAME:\\coda-language.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return false;
    DWORD written = 0;
    bool ok = ::WriteFile(file, russian ? "ru" : "en", 2, &written, NULL) && written == 2;
    if (ok) ok = FlushFileBuffers(file) != FALSE;
    if (!CloseHandle(file)) ok = false;
    if (!ok) return false;
    file = CreateFile("GAME:\\coda-language.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return false;
    char check[2]={0}; DWORD read=0;
    ok = ::ReadFile(file,check,2,&read,NULL) && read==2 &&
        check[0]==(russian?'r':'e') && check[1]==(russian?'u':'n');
    CloseHandle(file); return ok;
}
bool XboxSoundDevice::Connect() {
    for (int i = 0; i < 3; ++i) if ((isd_read_device_id() & 0xFFFFFF00) == 0x03EF2000) return true;
    return false;
}
bool XboxSoundDevice::PowerUp() { return isd_power_up() != FALSE; }
bool XboxSoundDevice::Read(unsigned page, Byte* data) { return isd_flash_read(page, data) != FALSE; }
bool XboxSoundDevice::Erase() { return isd_chip_erase() != FALSE; }
bool XboxSoundDevice::Write(unsigned page, const Byte* data) { return isd_flash_write(page, data) != FALSE; }
bool XboxSoundDevice::Play(unsigned index) { return isd1200_play_vp((UINT16)index) != FALSE; }
bool MountSystemFont() {
    NativeString link, target;
    RtlInitAnsiString(&link, "\\??\\Flash:"); RtlInitAnsiString(&target, "\\Device\\Flash");
    // Existing links are left intact.
    LONG result = ObCreateSymbolicLink(&link, &target);
    return result >= 0 || GetFileAttributes("Flash:\\XenonJKLatin.xtt") != (DWORD)-1;
}
bool XboxImageStore::Load(const std::string& path, std::vector<Byte>& data) {
    data.clear();
    HANDLE file = CreateFile(path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return false;
    DWORD high = 0, size = GetFileSize(file, &high), read = 0;
    bool ok = high == 0 && size == ImageSize;
    if (ok) { data.resize(ImageSize); ok = ::ReadFile(file, &data[0], ImageSize, &read, NULL) && read == ImageSize; }
    CloseHandle(file); if (!ok) data.clear(); return ok;
}
bool XboxImageStore::SaveVerifiedBackup(const std::vector<Byte>& data, std::string& path) {
    if (data.size() != ImageSize) return false;
    CreateDirectory("GAME:\\Dumps", NULL);
    HANDLE file = INVALID_HANDLE_VALUE;
    char candidate[MAX_PATH];
    // CREATE_NEW is the authority: never replace an existing backup.
    for (unsigned i = 1; i <= 100000; ++i) {
        sprintf_s(candidate, sizeof(candidate), "GAME:\\Dumps\\SoundBackup_%06u.bin", i);
        file = CreateFile(candidate, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file != INVALID_HANDLE_VALUE) break;
        if (GetLastError() != ERROR_FILE_EXISTS && GetLastError() != ERROR_ALREADY_EXISTS) return false;
    }
    if (file == INVALID_HANDLE_VALUE) return false;
    path = candidate; DWORD written = 0;
    bool ok = ::WriteFile(file, &data[0], ImageSize, &written, NULL) && written == ImageSize;
    if (ok) ok = FlushFileBuffers(file) != FALSE;
    if (!CloseHandle(file)) ok = false;
    std::vector<Byte> check;
    return ok && Load(path, check) && check == data;
}
static bool EntryLess(const ImageEntry& a, const ImageEntry& b) { return _stricmp(a.name.c_str(), b.name.c_str()) < 0; }
std::vector<ImageEntry> XboxImageStore::List(bool backups, DWORD* scanError) {
    const char* root = backups ? "GAME:\\Dumps" : "GAME:\\Sounds";
    if(scanError)*scanError=0;
    std::vector<ImageEntry> result;
    DWORD attributes=GetFileAttributes(root);
    if(attributes==(DWORD)-1) {
        DWORD error=GetLastError();if(scanError)*scanError=error?error:ERROR_PATH_NOT_FOUND;return result;
    }
    if(!(attributes&FILE_ATTRIBUTE_DIRECTORY)){if(scanError)*scanError=ERROR_DIRECTORY;return result;}
    // Scanning is read-only. A missing folder is not silently created.
    std::string pattern = std::string(root) + "\\*";
    WIN32_FIND_DATA data; HANDLE find = FindFirstFile(pattern.c_str(), &data);
    if (find == INVALID_HANDLE_VALUE) {
        DWORD error=GetLastError();
        if(scanError && error!=ERROR_FILE_NOT_FOUND && error!=ERROR_NO_MORE_FILES)*scanError=error;
        return result;
    }
    do {
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        const char* extension=strrchr(data.cFileName,'.');
        if(!extension || _stricmp(extension,".bin")!=0)continue;
        ImageEntry entry; entry.name = data.cFileName; entry.path = std::string(root) + "\\" + entry.name;
        entry.valid=false;
        // Library validation only needs size and protection byte. Do not read
        // every 44 KiB image, and do not take an exclusive lock on uploads.
        if(data.nFileSizeHigh==0 && data.nFileSizeLow==ImageSize) {
            HANDLE file=CreateFile(entry.path.c_str(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
            if(file!=INVALID_HANDLE_VALUE) {
                BYTE first=0;DWORD read=0;
                entry.valid=::ReadFile(file,&first,1,&read,NULL)&&read==1&&first==0xCF;
                CloseHandle(file);
            }
        }
        result.push_back(entry);
    } while (FindNextFile(find, &data));
    DWORD error=GetLastError();FindClose(find);
    if(scanError && error!=ERROR_NO_MORE_FILES)*scanError=error;
    std::sort(result.begin(), result.end(), EntryLess); return result;
}
LibraryScanner::LibraryScanner():thread_(NULL),backups_(false),generation_(0),completed_(0),error_(0) {}
LibraryScanner::~LibraryScanner() {
    if(thread_){WaitForSingleObject(thread_,INFINITE);CloseHandle(thread_);}
}
bool LibraryScanner::Start(bool backups,unsigned generation) {
    if(thread_)return false;
    backups_=backups;generation_=generation;result_.clear();error_=0;InterlockedExchange(&completed_,0);
    thread_=CreateThread(NULL,0,ThreadMain,this,0,NULL);return thread_!=NULL;
}
DWORD WINAPI LibraryScanner::ThreadMain(void* context) {
    LibraryScanner& scanner=*static_cast<LibraryScanner*>(context);
    XboxImageStore store;scanner.result_=store.List(scanner.backups_,&scanner.error_);
    InterlockedExchange(&scanner.completed_,1);return 0;
}
bool LibraryScanner::Take(std::vector<ImageEntry>& result,unsigned& generation,DWORD& error) {
    if(!thread_ || InterlockedCompareExchange(&completed_,0,0)==0)return false;
    CloseHandle(thread_);thread_=NULL;
    result.swap(result_);generation=generation_;error=error_;return true;
}
// Adapter keeps native file errors and Win32 calls out of the core workflow.
class XboxWorkshopStore : public IWorkshopStore {
public:
    bool ReadWave(const std::string& path,std::vector<Byte>& data,unsigned& error) {
        DWORD nativeError=0;
        bool ok=XboxWorkshopFiles::ReadWave(path,data,&nativeError);
        error=nativeError;return ok;
    }
    bool SaveImage(const std::vector<Byte>& data,std::string& path) {
        return XboxWorkshopFiles::SaveImage(data,path);
    }
};
OperationRunner::OperationRunner() : thread_(NULL), running_(0), cancel_(0), kind_(BackupOperation), index_(0) {
    InitializeCriticalSection(&lock_);
}
OperationRunner::~OperationRunner() {
    if (thread_) { WaitForSingleObject(thread_, INFINITE); CloseHandle(thread_); }
    DeleteCriticalSection(&lock_);
}
bool OperationRunner::Running() const { return running_ != 0; }
void OperationRunner::Publish(const OperationStatus& status) {
    EnterCriticalSection(&lock_); snapshot_ = status; LeaveCriticalSection(&lock_);
}
OperationStatus OperationRunner::Snapshot() {
    EnterCriticalSection(&lock_); OperationStatus value = snapshot_; LeaveCriticalSection(&lock_); return value;
}
bool OperationRunner::Launch() {
    if (thread_) { WaitForSingleObject(thread_, INFINITE); CloseHandle(thread_); thread_ = NULL; }
    InterlockedExchange(&cancel_, 0); InterlockedExchange(&running_, 1);
    OperationStatus initial; initial.stage = Connecting; initial.message = MsgStarting; Publish(initial);
    thread_ = CreateThread(NULL, 0, ThreadMain, this, 0, NULL);
    if (!thread_) {
        initial.stage = Failed; initial.message = MsgWorkerFailed;
        Publish(initial); InterlockedExchange(&running_, 0); return false;
    }
    return true;
}
bool OperationRunner::Start(bool install, const std::string& path) {
    if (Running()) return false; kind_ = install ? InstallOperation : BackupOperation; path_ = path; return Launch();
}
bool OperationRunner::StartPlayback(unsigned index) {
    if (Running()) return false; kind_=PlaybackOperation; index_ = index; return Launch();
}
bool OperationRunner::StartBuild(const std::string& power,const std::string& eject) {
    if (Running()) return false; kind_=BuildOperation; path_=power; ejectPath_=eject; return Launch();
}
void OperationRunner::RequestCancel() { InterlockedExchange(&cancel_, 1); }
DWORD WINAPI OperationRunner::ThreadMain(void* context) {
    OperationRunner& runner = *static_cast<OperationRunner*>(context);
    if (runner.kind_==BuildOperation) {
        XboxWorkshopStore store;
        ImageBuildWorkflow flow(store);
        flow.Begin(runner.path_,runner.ejectPath_);
        runner.Publish(flow.Status());
        while(flow.Busy()) {
            if(InterlockedExchange(&runner.cancel_,0))flow.Cancel();
            flow.Tick();runner.Publish(flow.Status());Sleep(1);
        }
    } else if (runner.kind_==PlaybackOperation) {
        OperationStatus status;
        bool ok = runner.device_.Connect() && runner.device_.PowerUp() && runner.device_.Play(runner.index_);
        status.stage = ok ? Complete : Failed;
        status.message = ok ? MsgPlayDone : MsgPlayFailed;
        runner.Publish(status);
    } else {
        SoundWorkflow flow(runner.device_, runner.store_);
        if (runner.kind_==InstallOperation) flow.BeginInstall(runner.path_); else flow.BeginBackup();
        runner.Publish(flow.Status());
        while (flow.Busy()) {
            if (InterlockedExchange(&runner.cancel_, 0)) flow.Cancel();
            flow.Tick(); runner.Publish(flow.Status()); Sleep(1);
        }
    }
    InterlockedExchange(&runner.running_, 0); return 0;
}
std::vector<std::string> XboxWorkshopFiles::ListImports() {
    CreateDirectory("GAME:\\Imports",NULL);
    std::vector<std::string> files; WIN32_FIND_DATA info;
    HANDLE find=FindFirstFile("GAME:\\Imports\\*.wav",&info);
    if (find==INVALID_HANDLE_VALUE) return files;
    do { if (!(info.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)) files.push_back(std::string("GAME:\\Imports\\")+info.cFileName); } while(FindNextFile(find,&info));
    FindClose(find);std::sort(files.begin(),files.end());return files;
}
bool XboxWorkshopFiles::ReadWave(const std::string& path,std::vector<Byte>& data,DWORD* error) {
    data.clear();if(error)*error=0;
    HANDLE file=CreateFile(path.c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if(file==INVALID_HANDLE_VALUE){if(error)*error=GetLastError();return false;}
    DWORD high=0,size=GetFileSize(file,&high),read=0;
    DWORD failure=0;
    if(size==INVALID_FILE_SIZE)failure=GetLastError();
    if(!failure && (high || size>ImageBuilder::MaxWaveBytes))failure=223; // Win32 ERROR_FILE_TOO_LARGE; absent from this XDK.
    if(!failure && size<12)failure=ERROR_INVALID_DATA;
    if(!failure){
        data.resize(size);
        if(!::ReadFile(file,&data[0],size,&read,NULL))failure=GetLastError();
        if(!failure && read!=size)failure=ERROR_HANDLE_EOF;
    }
    CloseHandle(file);
    if(failure){data.clear();if(error)*error=failure;return false;}
    return true;
}
bool XboxWorkshopFiles::SaveImage(const std::vector<Byte>& image,std::string& path) {
    if(!SoundWorkflow::ValidImage(image))return false;
    CreateDirectory("GAME:\\Sounds",NULL);HANDLE file=INVALID_HANDLE_VALUE;char name[MAX_PATH];
    for(unsigned i=1;i<=100000;++i){
        sprintf_s(name,sizeof(name),"GAME:\\Sounds\\CodaSound_%06u.bin",i);
        file=CreateFile(name,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
        if(file!=INVALID_HANDLE_VALUE)break;
        if(GetLastError()!=ERROR_FILE_EXISTS&&GetLastError()!=ERROR_ALREADY_EXISTS)return false;
    }
    if(file==INVALID_HANDLE_VALUE)return false;
    DWORD written=0;bool ok=::WriteFile(file,&image[0],ImageSize,&written,NULL)&&written==ImageSize;
    if(ok)ok=FlushFileBuffers(file)!=FALSE;
    if(!CloseHandle(file))ok=false;
    std::vector<Byte> check;XboxImageStore store;
    if(!ok || !store.Load(name,check) || check!=image){DeleteFile(name);return false;}
    path=name;return true;
}
}

