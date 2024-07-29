#include <iostream>
#include <Windows.h>

typedef struct _COPY_DATA {
    LPVOID hMapView;
    HANDLE hMap;
    HANDLE hFileSrc;
    HANDLE hFileDst;
} COPY_DATA;

using namespace std;

/** 비동기 처리 완료 시 호출이 될 함수 */
void CALLBACK WriteFileIOCompileRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);

void CloseAll(char *pszBuffer, HANDLE hMap, HANDLE hSrc, HANDLE hDst);

int main(int argc, char **argv) {
    /** console set locale*/
    ::_wsetlocale(LC_ALL, L"korean");
    /** HANDLE Read File and write File */
    HANDLE hSrcFile = nullptr, hDstFile = nullptr;
    /** get file size variable */
    LARGE_INTEGER lFileSize = {0,};
    /** Map of view File HANDEL */
    HANDLE hMap = nullptr;
    /** MapViewFile Access Pointer */
    char *pszBuffer = nullptr;
    /** 파일 열기 */
    hSrcFile = ::CreateFile(
            TEXT("C:\\Users\\fain9\\source\\repos\\inflearn-window-system-study\\chapter03\\06_memory_map_async_file_copy\\sample_zip_file.zip"), // 열 파일에 대한 경로와 리므
            GENERIC_READ, // 읽기 모드로 파일 열기
            FILE_SHARE_READ, // 공유에 대한 속성은 읽기만 공유하도록 설정
            NULL, // 보안 속성에 대한 상속 받기
            OPEN_EXISTING, // 파일이 존재할 때만 열기
            FILE_ATTRIBUTE_NORMAL, // 파일에 대한 기본 속성을 적용
            NULL //
    );
    /** FILE 에 대한 Handle 이 없을 경우 */
    if (hSrcFile == INVALID_HANDLE_VALUE) {
        printf("Failed Open file - C:\\Users\\fain9\\source\\repos\\inflearn-window-system-study\\chapter03\\06_memory_map_async_file_copy\\sample_zip_file.zip");
        return 0;
    }
    /** File 에 대한 크기를 가져 오기 */
    if (!::GetFileSizeEx(hSrcFile, &lFileSize)) {
        wprintf(L"Failed to get source file size : [ERROR CODE : %d]\r\n", ::GetLastError());
        CloseHandle(hSrcFile);
        return 0;
    }

    /** File Map Handle Create */
    hMap = ::CreateFileMapping(
            hSrcFile, // Mapping 시킬 파일에 대한 HANDLE
            NULL, // 매핑 객체에 대한 속성을 정의 -> NULL 은 상속 받는 것이 기본 값이다.
            PAGE_READONLY, // 매핑 객체에 대한 접근에 대한 설정 인자
            0, // 대용량의 파일의 매핑 객체르 생성할 때 사용하는 인자
            (DWORD) lFileSize.QuadPart, // mapping file 의 크기를 설정하는 인자.
            NULL // mapping file handle 의 이름 부여할 때 사용하는 인자.
    );
    /** FileMapping 에 대한 객체 생성 확인 */
    if (hMap == nullptr) {
        wprintf(L"Failed to make mapping file [ERROR CODE : %d]\r\n", ::GetLastError());
        CloseHandle(hSrcFile);
        return 0;
    }
    /** 파일 매핑 객체에 접근 pointer */
    pszBuffer = (char *) ::MapViewOfFile(
            hMap, // 접근할 Mapping File 객체
            FILE_MAP_READ, // 읽기 용도로 사용
            0, // offset 에 대한 설정
            0, // offset 에 대한 설정
            lFileSize.QuadPart // 접근할 크기 설정
    );
    /** 접근할 객체의 Pointer 획득 확인 */
    if (pszBuffer == nullptr) {
        printf("get mapping file pointer failed ...\r\n");
        CloseHandle(hMap);
        CloseHandle(hSrcFile);
        return 0;
    }
    /** 복사를 하기 전에 동일한 이름의 파일이 있으면 삭세 */
    ::DeleteFile(
            TEXT("C:\\Users\\fain9\\source\\repos\\inflearn-window-system-study\\chapter03\\06_memory_map_async_file_copy\\sample_zip_file_copy.zip"));
    hDstFile = ::CreateFile(
            TEXT("C:\\Users\\fain9\\source\\repos\\inflearn-window-system-study\\chapter03\\06_memory_map_async_file_copy\\sample_zip_file_copy.zip"), // 생성할 파일에 대한 경로 및 이름
            GENERIC_ALL, // 모든 접근에 대해서 허용한 상태로 파일 열기
            0, // 공유 하지 않음
            NULL, // 보안 속성에 대한 정의
            CREATE_ALWAYS, // 항상 새로 생성
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // 파일에 대한 기본 속성 정의 및 비동기 처리 설정
            NULL // 임시 저장을 할 것인지에 대한 설정
    );
    /** 생성된 파일에 대한 확인 */
    if (hDstFile == nullptr) {
        wprintf(L"Failed to create dst file [ERROR CODE : %d]\r\n", ::GetLastError());
        free(pszBuffer);
        CloseHandle(hMap);
        CloseHandle(hSrcFile);
        return 0;
    }
    /** 중첩 처리를 위한 객체 생성 */
    LPOVERLAPPED pOverlapped = (LPOVERLAPPED) malloc(sizeof(OVERLAPPED));
    pOverlapped->OffsetHigh = 0; // 중첩된 처리를 위한 offset 설정
    pOverlapped->Offset = 0; // offset 에 대한 초기화

    /** 사용자 정의한 객체를 넘겨줄 pointer */
    COPY_DATA *pCopyData = (COPY_DATA *) malloc(sizeof(COPY_DATA));
    pCopyData->hMapView = pszBuffer;
    pCopyData->hMap = hMap;
    pCopyData->hFileSrc = hSrcFile;
    pCopyData->hFileDst = hDstFile;
    /** event 객체 대신에 사용자가 정의한 pointer 를 넘겨 준다. */
    pOverlapped->hEvent = pCopyData;
    /** 파일에 대한 비동기 쓰기 */
    BOOL writeFileStatus = ::WriteFileEx(
            hDstFile, // 쓸 파일에 대한 HANDLE 객체
            pszBuffer, // 쓸 데이터를 가르키고 있는 Pointer
            (DWORD) lFileSize.QuadPart, // 쓰 데이터의 크기
            pOverlapped, // 비동기 요청에 해한 정보를 담고 있는 객체
            WriteFileIOCompileRoutine // 비동기 쓰기 후 호출이 될 함수
    );
    /** 비동기 쓰기 실패 시 */
    if (!writeFileStatus) {
        wprintf(L"Failed Write file Async [ERROR CODE : %d]\r\n", ::GetLastError());
        CloseAll(pszBuffer, hMap, hSrcFile, hDstFile);
        return 0;
    }
    /** 비동기 쓰기 시도에 대해 Alterable wait 상태가 된다. */
    for (; ::SleepEx(1, TRUE) != WAIT_IO_COMPLETION;);
    _putws(L"main() - end\r\n");
    return 0;
}

/** Close Handle Function */
void CloseAll(char *pszBuffer, HANDLE hMap, HANDLE hSrc, HANDLE hDst) {
    if (pszBuffer != NULL) {
        free(pszBuffer);
    }
    if (hMap != nullptr) {
        CloseHandle(hMap);
    }
    if (hSrc != nullptr) {
        CloseHandle(hSrc);
    }
    if (hDst != nullptr) {
        CloseHandle(hDst);
    }
}

void CALLBACK WriteFileIOCompileRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
    /** 화면에 출력을 위한 함수 */
    _putws(L"WriteFileIOCompileRoutine() - start");
    /** 인자로 받은 pointer 에 대한 확인 -> event로 사용한 것이 아니라 사용자가 사용할 pointer 객체로 넣어 줬다. */
    if (lpOverlapped->hEvent != nullptr) {
        /** Pointer 객체 사용자 데이터로 변환 -> casting */
        COPY_DATA *pCopyData = (COPY_DATA *) lpOverlapped->hEvent;
        /** mapview 객체 해제 */
        ::UnmapViewOfFile(pCopyData->hMapView);
        /** handle 객체 자원 해제 */
        CloseHandle(pCopyData->hMap);
        CloseHandle(pCopyData->hFileSrc);
        CloseHandle(pCopyData->hFileDst);
        /** 자원 해제 후 pointer 객체 해제 */
        free(pCopyData);
        /** 자원 해제 후 화면에 출력 */
        _putws(L"WriteFileIOCompileRoutine() - release memory & handles");
    }
    /** overlapped 객체 해제 */
    free(lpOverlapped);
    _putws(L"WriteIOCompileRoutine() - end");
}