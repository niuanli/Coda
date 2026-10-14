"""Compile real scanner and refresh methods; hold I/O to expose UI waits."""
from pathlib import Path
root=Path(__file__).resolve().parent
ui=(root/'../src/ui/StudioScene.cpp').read_text()
platform=(root/'../src/platform/XboxServices.cpp').read_text()
def method(source,signature):
    start=source.index(signature);brace=source.index('{',start);depth=1;end=brace+1
    while depth:
        if source[end]=='{':depth+=1
        if source[end]=='}':depth-=1
        end+=1
    return source[start:end]
cpp=r'''#include <windows.h>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cstring>
#define CHECK(x) do{if(!(x)){std::printf("FAIL line %d: %s\n",__LINE__,#x);std::exit(1);}}while(0)
struct ImageEntry{std::string path,name;bool valid;};
static HANDLE gate;static std::vector<ImageEntry> files;static DWORD fakeScanError=0;
struct XboxImageStore{std::vector<ImageEntry> List(bool,DWORD* error){WaitForSingleObject(gate,5000);*error=fakeScanError;return files;}};
class LibraryScanner {
public:LibraryScanner();~LibraryScanner();bool Start(bool,unsigned);bool Take(std::vector<ImageEntry>&,unsigned&,DWORD&);bool Active()const{return thread_!=NULL;}
private:HANDLE thread_;bool backups_;unsigned generation_;volatile LONG completed_;DWORD error_;std::vector<ImageEntry> result_;static DWORD WINAPI ThreadMain(void*);
};
'''
for signature in ['LibraryScanner::LibraryScanner()','LibraryScanner::~LibraryScanner()','bool LibraryScanner::Start(','DWORD WINAPI LibraryScanner::ThreadMain(','bool LibraryScanner::Take(']:cpp+=method(platform,signature)+'\n'
cpp+='enum {ImageSize=45056};\nstruct FolderStore { std::vector<ImageEntry> List(bool,DWORD*); };\n'
cpp+='static bool EntryLess(const ImageEntry& a,const ImageEntry& b){return _stricmp(a.name.c_str(),b.name.c_str())<0;}\n'
cpp+=method(platform,'std::vector<ImageEntry> XboxImageStore::List(').replace('XboxImageStore::List','FolderStore::List').replace(r'GAME:\\',r'build\\folder-probe\\')+'\n'
cpp+=r'''
static DWORD fakeTime=1000;
#define GetTickCount() fakeTime
enum{TxtRefreshScanning,TxtRefreshResult,TxtRefreshFailed,TxtFolderMissing,TxtFolderError,TxtScanPath,TxtScanTimeout};
struct Locale{const wchar_t* Text(int id){return id==TxtRefreshResult?L"Files: %u":L"Scanning";}};
struct Control{int selected;bool enabled,shown;std::wstring text;Control():selected(0),enabled(true),shown(false){}int GetCurSel(){return selected;}void SetCurSel(unsigned i){selected=(int)i;}void SetFocus(){}void SetEnable(bool e){enabled=e;}void SetText(const wchar_t* t){text=t;}void SetShow(bool s){shown=s;}};
struct StudioScene {
 bool scanWaiting_,scanQueued_,backups_;unsigned scanGeneration_,renders;DWORD lastRefreshAt_,scanStartedAt_;LibraryScanner libraryScanner_;
 std::vector<ImageEntry> entries_,homeEntries_;std::string refreshSelection_;Control list_,x_,a_,notice_,body_;Locale locale_;
 StudioScene():scanWaiting_(false),scanQueued_(false),backups_(false),scanGeneration_(0),renders(0),lastRefreshAt_(0),scanStartedAt_(0){}
 void RefreshLibrary();void PollLibraryScan();
 void RenderLibrary(){++renders;list_.selected=0;x_.enabled=true;a_.enabled=true;list_.enabled=true;}
};
'''
cpp+=method(ui,'void StudioScene::RefreshLibrary()')+'\n'+method(ui,'void StudioScene::PollLibraryScan()')
cpp+=r'''
int main(){
 CreateDirectoryA("build\\folder-probe",NULL);
 DeleteFileA("build\\folder-probe\\Sounds\\fixture.BIN");
 RemoveDirectoryA("build\\folder-probe\\Sounds");
 FolderStore folder;DWORD folderError=0;
 CHECK(folder.List(false,&folderError).empty());
 CHECK(folderError==ERROR_FILE_NOT_FOUND||folderError==ERROR_PATH_NOT_FOUND);
 CHECK(GetFileAttributesA("build\\folder-probe\\Sounds")==INVALID_FILE_ATTRIBUTES);
 CHECK(CreateDirectoryA("build\\folder-probe\\Sounds",NULL));
 CHECK(folder.List(false,&folderError).empty());CHECK(folderError==0);
 FILE* fixture=NULL;CHECK(fopen_s(&fixture,"build\\folder-probe\\Sounds\\fixture.BIN","wb")==0);
 for(unsigned i=0;i<45056;++i)fputc(i==0?0xCF:0,fixture);fclose(fixture);
 std::vector<ImageEntry> found=folder.List(false,&folderError);CHECK(found.size()==1);CHECK(found[0].valid);CHECK(folderError==0);
 gate=CreateEvent(NULL,TRUE,FALSE,NULL);CHECK(gate!=NULL);
 ImageEntry a;a.path="Sounds/A.BIN";a.name="A.BIN";a.valid=true;
 ImageEntry b;b.path="Sounds/B.bin";b.name="B.bin";b.valid=true;
 files.push_back(a);files.push_back(b);StudioScene view;view.entries_.push_back(b);
 view.RefreshLibrary();CHECK(view.scanQueued_&&view.scanWaiting_);CHECK(!view.libraryScanner_.Active());
 view.PollLibraryScan();CHECK(view.libraryScanner_.Active());CHECK(view.x_.enabled);
 // Delayed enumeration must never make the UI wait or reset its scene/timer.
 for(unsigned frame=0;frame<1000;++frame){view.PollLibraryScan();CHECK(view.renders==0);}
 unsigned generation=view.scanGeneration_;fakeTime+=500;view.RefreshLibrary();CHECK(view.scanGeneration_==generation);
 CHECK(!view.libraryScanner_.Start(false,999));
 SetEvent(gate);
 for(unsigned i=0;i<500&&view.scanWaiting_;++i){view.PollLibraryScan();Sleep(1);}
 CHECK(!view.scanWaiting_);CHECK(view.entries_.size()==2);CHECK(view.entries_[view.list_.selected].name=="B.bin");CHECK(view.notice_.text==L"Files: 2");
 // Leaving/changing screens while a scan runs invalidates its generation.
 ResetEvent(gate);fakeTime+=500;view.RefreshLibrary();view.PollLibraryScan();
 ++view.scanGeneration_;view.scanWaiting_=false;view.scanQueued_=false;
 SetEvent(gate);for(unsigned i=0;i<500&&view.libraryScanner_.Active();++i){view.PollLibraryScan();Sleep(1);}
 CHECK(!view.libraryScanner_.Active());CHECK(view.renders==1);
 files.clear();fakeTime+=500;view.RefreshLibrary();
 for(unsigned i=0;i<500&&view.scanWaiting_;++i){view.PollLibraryScan();Sleep(1);}
 CHECK(view.entries_.empty());CHECK(view.notice_.text==L"Files: 0");
 fakeScanError=ERROR_PATH_NOT_FOUND;fakeTime+=500;view.RefreshLibrary();
 for(unsigned i=0;i<500&&view.scanWaiting_;++i){view.PollLibraryScan();Sleep(1);}
 CHECK(!view.scanWaiting_);CHECK(view.body_.shown);CHECK(view.notice_.text.find(L"GAME:\\Sounds")!=std::wstring::npos);
 // A permanently delayed request must leave the waiting UI state after 5 s.
 ResetEvent(gate);fakeScanError=0;fakeTime+=500;view.RefreshLibrary();view.PollLibraryScan();fakeTime+=5001;view.PollLibraryScan();
 CHECK(!view.scanWaiting_);CHECK(view.x_.enabled);SetEvent(gate);
 for(unsigned i=0;i<500&&view.libraryScanner_.Active();++i){view.PollLibraryScan();Sleep(1);}
 CloseHandle(gate);std::puts("PASS: real missing/empty folder and BIN, asynchronous refresh, missing-folder UI with path, timeout, duplicate request and obsolete result");
}
'''
(root/'build/refresh_tests.cpp').write_text(cpp)

