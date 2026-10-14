#include "../src/ui/CatalogueModel.h"
#include <cassert>
#include <cstdio>
using namespace soundstudio;
int main() {
    CatalogueModel list;
    assert(list.Selected()==-1 && !list.Move(1));
    list.Insert(0,8);
    for(int i=0;i<8;++i)list.SetLabel(i,L"image.bin");
    list.Select(5);assert(list.FirstVisible()==0);
    assert(list.Move(1));assert(list.Selected()==6 && list.FirstVisible()==6);
    assert(list.Move(1));assert(!list.Move(1));assert(list.Label(7)==L"image.bin");
    list.Delete(6,2);assert(list.Selected()==5 && list.FirstVisible()==0);
    list.Delete(0,list.Count());assert(list.Selected()==-1 && list.Count()==0);
    list.Insert(0,2);list.Select(1);list.Insert(0,1);assert(list.Selected()==2);
    list.Delete(0,1);assert(list.Selected()==1);list.Select(99);assert(list.Selected()==1);
    puts("PASS: catalogue paging, selection bounds, deletion and rebuilding");
}
