#include <iostream>
#include <Windows.h>

#define CACHE_SIZE        65536 // 64kb

using namespace std;

int main(int argc, char **argv) {
    /** console에 대한 한국어 설정 */
    ::_wsetlocale(LC_ALL, L"korean");
    /** 현재 폴더의 위치에 대한 정보를 저장하기 이한 변수 */
    TCHAR szCurDir[MAX_PATH] = {0};
    /** 현재 폴더에 대한 정보를 가져오기 위한 함수 */
    ::GetCurrentDirectory(MAX_PATH, szCurDir);
    ::wprintf(L"Current Directory: %s\n", szCurDir);
    /** 읽어오고 쓰기를 진행할 파일 HANDLE 변수 선언 */
    HANDLE hFileSource = NULL, hFileTarget = NULL;
    /** 원본 파일에 대한 읽어오기 */
    hFileSource = ::CreateFile(
            TEXT("C:\\Users\\fain9\\source\\repos\\inflearn-window-system-study\\chapter03\\02_file_copy\\test.txt"), // 읽어올 파일에 대한 경로
//            TEXT("c:\\test.txt"),
            GENERIC_READ, // 읽기 모드로 파일 열기
            FILE_SHARE_READ, // 읽기 모드로 열고 있을 때 다른 객체가 읽기 모드로 접근을 허용 하는 FLAG
            NULL, // 보안 속성에 대해서 설정 하는 값
            OPEN_EXISTING, // 파일이 존재할 경우에만 읽기
            FILE_ATTRIBUTE_NORMAL, // 기본 파일에 대한 속성으로 읽기
            NULL // 동기 비동기에 대한 설정 -> NULL 값을 사용시 동기 모드
    );
    /** 파일에 대한 HANDLE 객체가 존재하지 않을 경우 */
    if (hFileSource == INVALID_HANDLE_VALUE) {
        /** 에러에 대한 정보를 가져오는 윈도우 함수 */
//        ::GetLastError();
        ::wprintf(L"Failed to open source file [ERROR CODE : %d]\r\n", ::GetLastError());
        return 0;
    }
    /** 파일을 복사 하기 위한 쓰기 HANDLE 생성 -> 파일에 대한 생성 */
    hFileTarget = ::CreateFile(
            TEXT("C:\\Users\\fain9\\source\\repos\\inflearn-window-system-study\\chapter03\\02_file_copy\\text.copy.txt"), // 쓸 파일에 대한 경로
//        TEXT("c:\\test.copy.txt"),
            GENERIC_WRITE, // 쓰기모드로 파일을 열기 위한 FLAG
            0, // 쓰기를 진행 중에 해당 파일에 대한 접근을 공유할 것인지 아닌지에 대해서 설정하는 FLAG
            NULL, // 보안 속성에 대해서 정의하는 매개변수 -> NULL 이용 시 상속을 받아서 사용을 한다.
            OPEN_ALWAYS, // 파일이 있거나 없거나 항상 열기 위한 FLAG
            FILE_ATTRIBUTE_NORMAL, // 파일에 대한 속성을 정의 하기 위한 FLAG -> 기본 값으로 설정 FILE_ATTRIBUTE_NORMAL
            NULL // 동기 모드 혹은 비동기로 진행을 할지에 대해서 나타내는 FLAG -> NULL 값은 동기 모드로 동작한다.
    );

    /** 파일에 대해서 열기 실패 시 */
    if (hFileTarget == INVALID_HANDLE_VALUE) {
        ::wprintf(L"Failed to open target file [ERROR CODE : %d\r\n]", ::GetLastError());
//        ::wprintf(TEXT((const wchar_t *)(*"Failed to open target file [ERROR CODE : %d\r\n]")), ::GetLastError());
        return 0;
    }
    /** 파일에 대한 사이즈를 가져오기 위한 변수 */
    LARGE_INTEGER lFileSize = {0,};
    /** 파일에 대한 전체 사이즈를 가져오기 위한 변수 */
    LONGLONG lFileTotalSize = {0,};
    /** 읽은 파일의 바이트 수와 쓰기 위한 파일의 바이트 수를 저장하기 위한 변수 */
    DWORD dwReadSize = 0, dwWriteSize = 0;
    /** 반복문 탈출을 위한 FLAG */
    BOOL bResult = FALSE;
    /** 임시 저장하기 위한 버퍼 */
    BYTE byBuffer[CACHE_SIZE];
    /** 읽어온 파일에 대한 전체 파일의 크기에 대해서 가져오는 함수  */
    if (!::GetFileSizeEx(hFileSource, &lFileSize)) {
        ::wprintf(L"원본 파일에 대한 사이즈를 가져올 수 없습니다.\n");

        ::CloseHandle(hFileSource);
        ::CloseHandle(hFileTarget);
        return 0;
    }
    ::wprintf(L"쓸 파일에 대한 바이트 수는 %lld 입니다.\n", lFileSize.QuadPart);
    
    /** 끊어서 파일을 쓰기 위한 반복문 */
    /** 읽어온 파일의 전체 사이즈만큼 반복 -> 읽어온 바이트 수를 더해서 전체 사이즈에 도달할 때 까지 반복 */
    for (LONGLONG i = 0; i < lFileSize.QuadPart; i += dwReadSize) {
        /** Buffer 에 대한 초기화 */
        ::ZeroMemory(byBuffer, CACHE_SIZE);
        /** 파일에 대해서 읽기 */
        bResult = ::ReadFile(
                hFileSource, // 읽어올 파일에 대한 HANDLE 객체
                byBuffer, // 읽어온 데이터를 저장할 Buffer
                CACHE_SIZE, // 최대로 읽어올 바이트 수
                &dwReadSize, // 실제로 읽어온 바이트 수를 저장할 변수
                NULL // 비동기 동작에 대한 설정
        );
        ::wprintf(L"read bytes : %dbyte : result %d \n", dwReadSize, bResult);
        /** 읽어오기에 실패 시 출력 */
        if (!bResult) {
            ::wprintf(L"Failed to read source file [ERROR CODE : %d]\n", ::GetLastError());
            break;
        }
            /** 읽어온 값이 없을 경우 -> 파일의 끝까지 읽은 경우 */
        else if (bResult && dwReadSize == 0) {
            break;
        }
        /** 쓴 파일의 전체 크기 업데이트 */
        lFileTotalSize += dwReadSize;
        /** 현재 진행된 %로 화면 출력 */
        ::wprintf(L"%I64d%%\n", lFileTotalSize * 100 / (lFileSize.QuadPart));
        /** 파일에 대한 쓰기 */
        BOOL writeFlag = ::WriteFile(
                hFileTarget, // 쓸 파일에 대한 HANDLE 객체
                byBuffer, // 쓸 데이터가 저장이 되어 있는 Buffer
                dwReadSize, // 쓸 데이터의 바이트 수
                &dwWriteSize, // 실제로 쓴 바이트 수를 저장할 변수
                NULL // 동시 작업 시 어떻게 진행을 할 것인지 정의가 되어 있는 구조체
        );
        /** 쓰기 실패 시 */
        if (!writeFlag || dwReadSize != dwWriteSize) {
            wprintf(L"Failed to write target file [ERROR CODE : %d]\n", ::GetLastError());
            break;
        }
    }
    /** 콘솔에 UNICODE 형태의 문자열을 출력을 해준다. */
    ::_putws(L"Complete Copy!\n");
    ::CloseHandle(hFileSource);
    ::CloseHandle(hFileTarget);
    return 0;
}