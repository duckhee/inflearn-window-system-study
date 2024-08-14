#include <iostream>
#include <Windows.h>
#include <tchar.h>

using namespace std;

int main(int argc, char **argv) {
    /** Get Process Heap -> 기본 힙을 사용을 할 때 사용 */
//    HANDLE hProcessHeap = ::GetProcessHeap();
    /** Create custom Heap */
    HANDLE hProcessHeap = ::HeapCreate(
            0, // 생성할 heap 에 대한 옵션 값
            0, // 생성 초기에 할당할 힙의 크기 값
            0 // 최대로 가질 수 있는 힙의 크기 값
    );
    /** Custom Heap Using Memory */
    void *pMemory_4b = ::HeapAlloc(
            hProcessHeap, // 메모리 할당할 Heap 객체 HANDLE
            0,
            4 // 4byte의 크기의 메모리 할당
    );
    /** Custom Heap Using memory */
    LPVOID pMemory_1k = ::HeapAlloc(
            hProcessHeap, // 메모리 할당할 Heap 객체 HANDLE
            0,
            1024 * 1024 * 4 // 24MB의 크기의 메모리 할당
    );
    /** Heap에 대한 정보를 저장하기 위한 Heap Entry 구조체 */
    PROCESS_HEAP_ENTRY Entry = {0,};

    /** Heap 에 대한 정보를 가져오기 */
    while (::HeapWalk(hProcessHeap, &Entry) != FALSE) {
        /** */
        if ((Entry.wFlags & PROCESS_HEAP_ENTRY_BUSY) != 0) {
            _tprintf(TEXT("Allocated block"));
            /** */
            if ((Entry.wFlags & PROCESS_HEAP_ENTRY_MOVEABLE) != 0) {
                _tprintf(TEXT(", movable with HANDLE %#p"), Entry.Block.hMem);
            }
        }
            /** */
        else if ((Entry.wFlags & PROCESS_HEAP_REGION) != 0) {
            _tprintf(TEXT("Region\n  %d bytes committed\n") \
                TEXT("  %d bytes uncommitted\n  First block address: %#p\n") \
                TEXT("  Last block address: %#p\n"),
                     Entry.Region.dwCommittedSize,
                     Entry.Region.dwUnCommittedSize,
                     Entry.Region.lpFirstBlock,
                     Entry.Region.lpLastBlock);
        } else if ((Entry.wFlags & PROCESS_HEAP_UNCOMMITTED_RANGE) != 0) {
            _tprintf(TEXT("Uncommitted range\n"));
        } else {
            _tprintf(TEXT("Block\n"));
        }

        _tprintf(TEXT("  Data portion begins at: %#p\n  Size: %d bytes\n") \
            TEXT("  Overhead: %d bytes\n  Region index: %d\n\n"),
                 Entry.lpData,
                 Entry.cbData,
                 Entry.cbOverhead,
                 Entry.iRegionIndex);
    }
    /** Heap에 생성한 메모리 해제 */
    ::HeapFree(hProcessHeap, 0, pMemory_4b);
    ::HeapFree(hProcessHeap, 0, pMemory_1k);
    /** 생성한 힙에 대해서 자원 반환 */
    ::HeapDestroy(hProcessHeap);
    return 0;
}