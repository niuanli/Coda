"""Compile the actual preference methods with Windows file APIs on the host."""
from pathlib import Path
root=Path(__file__).resolve().parent
source=(root/'../src/platform/XboxServices.cpp').read_text()
def method(signature):
    start=source.index(signature);brace=source.index('{',start);depth=1;end=brace+1
    while depth:
        if source[end]=='{':depth+=1
        if source[end]=='}':depth-=1
        end+=1
    return source[start:end].replace(r'GAME:\\',r'build\\language-test\\')
cpp='''#include <windows.h>
#include <cstdio>
#include <cstdlib>
struct LanguageSettings { static bool LoadRussian(); static bool SaveRussian(bool); };
#define CHECK(x) do {if(!(x)){std::printf("FAIL line %d\\n",__LINE__);std::exit(1);}}while(0)
'''+method('bool LanguageSettings::LoadRussian()')+'\n'+method('bool LanguageSettings::SaveRussian(bool russian)')+'''
int main(){
 CreateDirectoryA("build\\\\language-test",NULL);
 const char* path="build\\\\language-test\\\\coda-language.txt";
 SetFileAttributesA(path,FILE_ATTRIBUTE_NORMAL);DeleteFileA(path);
 CHECK(LanguageSettings::LoadRussian());
 CHECK(LanguageSettings::SaveRussian(false));CHECK(!LanguageSettings::LoadRussian());
 CHECK(LanguageSettings::SaveRussian(true));CHECK(LanguageSettings::LoadRussian());
 CHECK(SetFileAttributesA(path,FILE_ATTRIBUTE_READONLY));
 CHECK(!LanguageSettings::SaveRussian(false));CHECK(LanguageSettings::LoadRussian());
 CHECK(SetFileAttributesA(path,FILE_ATTRIBUTE_NORMAL));
 CHECK(LanguageSettings::SaveRussian(false));CHECK(!LanguageSettings::LoadRussian());
 std::puts("PASS: default language, RU/EN persistence and read-only failure");
}
'''
(root/'build/settings_tests.cpp').write_text(cpp)
