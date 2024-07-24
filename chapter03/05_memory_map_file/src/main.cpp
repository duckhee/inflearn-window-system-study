#include <iostream>
#include <Windows.h>
#include <WinBase.h>
#include <tchar.h>

using namespace std;

int main(int argc, char **argv) {
    HANDLE hFile = ::CreateFile(
            TEXT("C:\\Users\\fain9\\source\\repos\\inflearn-window-system-study\\chapter03\\05_memory_map_file\\test.txt"), // 생성할 파일에 대한 이름 및 경로 인자로 받기
            GENERIC_READ | GENERIC_WRITE, // 파일에 대한 쓰기 및 읽기 권한으로 생성
            FILE_SHARE_READ, // 읽기에 대한 공유 허용
            NULL, // 보안 속성에 대해서 상속을 받아서 사용
            CREATE_ALWAYS, // 항상 해당 파일을 생성
            FILE_ATTRIBUTE_NORMAL, // 파일에 대한 기본적인 속성으로 만들어주기
            NULL
    );
    /** 생성된 파일에 대한 크기를 설정 해주는 함수 -> fseek와 동일한 기능을 한다. */
    ::SetFilePointer(
            hFile, // 설정을 할 파일에 대한 HANDLE 객체
            1024, // 변경할 크기에 대한 값
            NULL, // 최대로 이동할 위치 값
            FILE_BEGIN //  이동을 시작할 위치에 대한 값
    );

    /** file 에 대한 Mapping 객체를 생성한다. */
    HANDLE hMap = ::CreateFileMapping(
            hFile, // 매핑 객체를 만들 파일에 대한 HANDLE 객체를 인자로 받는다.
            NULL, // 매핑 객체에 대한 속성에 대해서 정의 하는 값을 인자로 받는다.
            PAGE_READWRITE, // 매핑 객체의 용도에 대해서 설정하는 인자를 받는다. -> 현재는 쓰기 및 읽기를 해당 Mapping 객체를 통해서 할 수 있도록 해준 옵션이다.
            0, // 대용량의 파일의 매핑 객체를 생성할 경우 사용한다. -> 64bit의 크기를 사용할 때 사용
            1024, // 파일 매핑 객체의 크기에 대해서 byte 값을 넣어준다.
            NULL // 파일 매핑에 대한 객체에 이름을 부여할 때 사용이 되는 인자이다. -> NULL 시 따로 이름을 생성하지 않는다.
    );

    /** Mapping 객체 생성이 안되었을 경우 */
    if (hMap == NULL) {
        wprintf(L"Failed to create file mapping obj [ERROR CODE : %d]\r\n", ::GetLastError());
        ::CloseHandle(hFile);
        return 0;
    }
    /** Mapping 객체에 접근 가능한 Pointer 가져오기 */
    char *pszMemory = (char *) ::MapViewOfFile(
            hMap, // 사용할 map 객체에 대한 HANDLE 객체를 인자로 받는다.
            FILE_MAP_ALL_ACCESS, // file mapping 객체에 대한 접근 권한에 대해서 인자로 받는다.
            0, // 대용량의 파일에 대한 mapping 을 할 때 값을 넣어준다.
            0, // 메모리에 연결할 하위 4byte 를 지정을 해주는 값이다.
            1024 // 매핑을 시킬 byte 수에 대해서 인자로 넣어준다.
    );
    if (pszMemory == NULL) {
        ::CloseHandle(hMap);
        ::CloseHandle(hFile);
        wprintf(L"Failed to mapping view obj [ERROR CODE : %d]\r\n", ::GetLastError());
        return 0;
    }
    /** 가져온 Memory 주소에 데이터 쓰기 */
    strcpy_s(pszMemory, 1024, "Hello, Memory Mapped File!");
    /** view file 객체에 IO 에 대한 flush 를 해주는 함수 */
    //::FlushViewOfFile(pszMemory, 1024);
    /** 가져온 메모리 해제 -> MapViewOfFile로 가져온 메모리 해제 */
    ::UnmapViewOfFile(pszMemory);
    /** map view of file 객체 메모리 해제 */
    ::CloseHandle(hMap);
    /** File 에 대한 Flush */
    //::FlushFileBuffers(hFile);
    /** 파일 객체에 대한 반환 */
    ::CloseHandle(hFile);
    return 0;
}
