#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include <tchar.h>

using namespace std;

void CloseAll(char *pzsBuffer, HANDLE hMap, HANDLE hSrc, HANDLE hDst);

int main(int argc, char **argv) {
    /** console 에 대한 한국어 설정 */
    ::_wsetlocale(LC_ALL, L"Korean");
    /** 복사할 file 에 대한 정보를 담기 위한 pointer HANDLE 객체 */
    HANDLE hFileSource = nullptr, hFileDest = nullptr;
    /** 파일에 대한 크기를 가져오기 위한 구조체 변수 */
    LARGE_INTEGER lFileSize = {0,};
    /** MapViewOfFile 로 만들어주고 가르킬 Pointer 객체 */
    HANDLE hMap = nullptr;
    /** 버퍼를 담을 변수 */
    char *pszBuffer = nullptr;
    /** 복사할 대상 파일 열기 */
    hFileSource = ::CreateFile(
            TEXT("C:\\Users\\fain9\\source\\repos\\inflearn-window-system-study\\chapter03\\05_memory_map_file_copy_zip_file\\sample_zip_file.zip"),
            GENERIC_READ, // 파일에 대해서 읽기 권한으로 열기
            FILE_SHARE_READ, // 읽기에 대해서만 공유
            NULL, // 보안 속성은 상속을 받아서 사용
            OPEN_EXISTING, // 존재하는 경우에만 열기
            FILE_ATTRIBUTE_NORMAL, // 파일에 대한 속성 정의
            NULL // 동기  설정하는 flag
    );
    if (hFileSource == INVALID_HANDLE_VALUE) {
        printf("failed open file - C:\\Users\\fain9\\source\\repos\\inflearn-window-system-study\\chapter03\\05_memory_map_file_copy_zip_file\\sample_zip_file.zip\r\n");
        return 0;
    }

    /** 파일에 대한 크기 가져오기 */
    if (!::GetFileSizeEx(hFileSource, &lFileSize)) {
        wprintf(L"Failed to get source file size : [ERROR CODE : %d]\r\n", ::GetLastError());
        CloseAll(nullptr, nullptr, hFileSource, nullptr);
        return 0;
    }
    cout << "get file size : " << lFileSize.QuadPart << endl;
    /** 원본 파일에 대한 MapFile 만들기 */
    hMap = ::CreateFileMapping(
            hFileSource, // file map 을 만들 HANDLE 객체
            NULL, // 매핑 객체에 대한 속성에 대해서 정의 하는 값을 인자로 받는다.
            PAGE_READONLY, // 매핑 객체의 용도에 대해서 설정하는 인자를 받는다. -> 현재는 읽기를 해당 Mapping 객체를 통해서 할 수 있도록 해준 옵션이다.
            0, // 대용량의 파일의 매핑 객체를 생성할 경우 사용한다. -> 64bit의 크기를 사용할 때 사용 32bit는 최대 4GB를 사용이 가능하다.
            (DWORD) lFileSize.QuadPart, // 파일 매핑 객체의 크기에 대해서 byte 값을 넣어준다.
            NULL // file map 에 대한 이름을 부여할 경우 사용 한다.
    );

    if (hMap == nullptr) {
        wprintf(L"Failed to make mapping file source file  : [ERROR CODE : %d]\r\n", ::GetLastError());
        CloseAll(nullptr, nullptr, hFileSource, nullptr);
        return 0;
    }

    /** File Mapping 객체에 대한 접근 포인터 가져오기 */
    pszBuffer = (char *) ::MapViewOfFile(
            hMap, // file mapping 을 시킨 map 파일에 대한 HANDLE 객체를 가져온다.
            FILE_MAP_READ, // 읽기 모드로 사용한다는 의미이다.
            0,
            0,
            (DWORD) lFileSize.QuadPart // 매핑을 시킬 byte 수에 대해서 인자로 받는다. -> 복사할 파일에 대한 전체 크기를 받는다.
    );
    /** 기존에 복사본이 있으면 삭제하기 위한 delete file */
    ::DeleteFile(
            TEXT("C:\\Users\\fain9\\source\\repos\\inflearn-window-system-study\\chapter03\\05_memory_map_file_copy_zip_file\\sample_zip_file_copy.zip")
    );
    /** 복사 파일을 만들기 위한 File 객체 생성 */
    hFileDest = ::CreateFile(
            TEXT("C:\\Users\\fain9\\source\\repos\\inflearn-window-system-study\\chapter03\\05_memory_map_file_copy_zip_file\\sample_zip_file_copy.zip"), // 파일에 대햇 쓸 위치 및 이름을 인자로 넣어준다.
            GENERIC_ALL, // 파일에 대한 접근에 대해서 모든 접근 허용 모드로 열기
            0, // 공유 모드를 사용하지 않는다.
            NULL, // 보안에 대한 속성을 정의 하는 인자이다
            CREATE_ALWAYS, // 항상 파일에 대한 생성 한다는 의미
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // 파일에 대한 속성 정의 및 중첩 사용을 위한 설정
            NULL
    );
    /** 쓸 파일에 대한 생성 실패 시 */
    if (hFileDest == nullptr) {
        wprintf(L"Failed to create dst file [ERROR CODE : %d]\r\n", ::GetLastError());
        CloseAll(pszBuffer, hMap, hFileSource, nullptr);
        return 0;
    }
    /** 중첩 처리를 위한 객체 생성 */
    LPOVERLAPPED pOverlapped = (LPOVERLAPPED) malloc(sizeof(OVERLAPPED));
    pOverlapped->OffsetHigh = 0; // 중첩된 처리를 위한 offset 설정
    pOverlapped->Offset = 0; // offset 에 대한 초기화
    pOverlapped->hEvent = ::CreateEvent(
            NULL, // 이벤트에 대한 속성 정의
            FALSE, // 자동으로 이벤트 초기화 진행
            FALSE, // 이벤트에 대한 초기 값 설정 -> FALSE
            NULL // 이벤트에 대한 이름 정의 시 인자를 넣어준다.
    );
    /** 실제로 쓴 데이터 바이트 수를 저장할 변수 */
    DWORD dwWritten = 0;
    /** 파일에 대한 쓰기 */
    ::WriteFile(
            hFileDest, // 쓸 파일에 대한 HANDLE 객체
            pszBuffer, // 쓸 데이터가 저장이된 버퍼
            (DWORD) lFileSize.QuadPart, // 최대로 쓸 바이트 수
            &dwWritten, // 실제로 쓴 바이트의 수를 저장하기 위한 변수
            pOverlapped // 중첩된 처리 및 이벤트 처리를 위한 Overlapped 객체
    );
    /** 처리 대기 중이 아닌 애러인 경우 */
    if (::GetLastError() != ERROR_IO_PENDING) {
        wprintf(L"write file [ERROR CODE : %d]\r\n", ::GetLastError());
        CloseAll(pszBuffer, hMap, hFileSource, hFileDest);
        ::CloseHandle(pOverlapped->hEvent);
        free(pOverlapped);
        return 0;
    }
    /** 완료가 될 때 까지 대기 하는 함수 */
    if (::WaitForSingleObject(pOverlapped->hEvent, INFINITE) == WAIT_OBJECT_0) {
        _putws(L"Completed!\r\n");
    }

    ::CloseAll(pszBuffer, hMap, hFileSource, hFileDest);
    ::CloseHandle(pOverlapped->hEvent);
    free(pOverlapped);
    return 0;
}


void CloseAll(char *pzsBuffer, HANDLE hMap, HANDLE hSrc, HANDLE hDst) {
    if (pzsBuffer != nullptr) {
        ::UnmapViewOfFile(pzsBuffer);
    }
    if (hMap != nullptr) {
        ::CloseHandle(hMap);
    }
    if (hSrc != nullptr) {
        ::CloseHandle(hSrc);
    }
    if (hDst != nullptr) {
        ::CloseHandle(hDst);
    }
}