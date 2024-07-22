#include <iostream>
#include <Windows.h>
#include <process.h>
#include <string.h>
#include <memory.h>
#include <tchar.h>

using namespace std;


void CALLBACK FileIoComplete(
        DWORD dwError, // 에러 코드에 대한 인자
        DWORD dwTransfered, // 입/출력이 완료된 데이터 크기
        LPOVERLAPPED pOl // Overlapped 구조체
) {
    printf("FileIoComplete() Callback - [%d byte] 쓰기 완료 - %s\r\n", dwTransfered, (char *) pOl->hEvent);
    /** 동적으로 할당한 객체에 대해서 메모리 해제를 구현 */
    delete[] pOl->hEvent;
    delete pOl;
    puts("FileIoComplete() - return \n");
}

DWORD WINAPI  IoThreadFunction(LPVOID pParam) {
    /** 메모리를 동적할당하고 값을 채운다. */
    /** 동적 할당이 된 공간이므로 사용이 완료 시에 메모리 해제를 해줘야 한다. */
    char *pszBuffer = new char[16];
    memset(pszBuffer, 0, sizeof(char) * 16);
    /** 문자열 복사 */
    strcpy_s(pszBuffer, sizeof(char) * 16, "Hello IOCP\n");

    /** 중첩 요청에 대한 구조체 정의를 위한 구조체 포인터 변수 선언 */
    LPOVERLAPPED pOverLapped = NULL;
    /** 구조체 동적 할당 */
    pOverLapped = new OVERLAPPED;
    /** 구조체 초기화 */
    memset(pOverLapped, 0, sizeof(OVERLAPPED));
    /** 구조체 값 정의 */
    pOverLapped->Offset = 1024 * 1024 * 512; // 512MB 공간 offset
    pOverLapped->hEvent = pszBuffer; // 쓰기 위한 값을 담아준다.
    /** 다음과 같이 완료 후 해당 호출을 위한 함수에 대해서 정의된 값을 넘겨줘도 된다.*/
//    LPOVERLAPPED_COMPLETION_ROUTINE completedRoutine = FileIoComplete;

    puts("IOThreadFunction() - 중첩된 쓰기 시도");
    /** 파일에 쓰기 */
    ::WriteFileEx(
            (HANDLE) pParam, // 쓸 파일에 대한 HANDLE 객체를 인자로 받는다
            pszBuffer, // 쓸 데이터가 담겨져있는 buffer에 대한 값을 넣어준다.
            sizeof(char) * 16, // 쓸 데이터의 수를 인자로 받는다.
            pOverLapped, // 중첩 처리 시에 중첩 처리에 대한 정의가 되어 있는 OVER_LAPPED 구조체를 인자로 받는다.
//            completedRoutine // 완료 시에 실행을 시켜줄 함수에 대한 함수 포인터를 받는다.
            FileIoComplete
    );

    /** 비동기 쓰기 시도에 대해 Alterable wait 상태가 된다. */
    for (; ::SleepEx(1, TRUE) != WAIT_IO_COMPLETION;);
    puts("IoThreadFunction() - return");
    return 0;
}

int main(int argc, char *argv[]) {
    /** 파일 쓰기 모드로 파일 HANDLE 객체 생성 */
    HANDLE hFile = ::CreateFile(
            _T("TextFile.txt"), // 파일에 대한 이름 및 경로 정의
            GENERIC_WRITE, // 일반적으로 사용을 하는 쓰기 모드로 파일 열기
            0, // 공유 모드에 대한 정보를 전달 -> 0의 값은 공유를 하지 않는다는 FLAG
            NULL, // 파일에 대한 보안 속성에 대해서 정의 -> NULL은 기본적으로 상속을 받아서 처리한다는 의미이다.
            CREATE_ALWAYS, // 파일을 항상 새로 만들 것이라고 명시를 해주는 FLAG
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // 파일에 대한 속성을 기본 속성과 비동기 요청을 위한 중첩 요청이라고 전달
            NULL // 임시 파일을 만들 것인지 아닌지에 대한 정의
    );
    /** 비동기 쓰기를 위한 Thread 생성 => Alterable Wait 상태여야만 비동기 쓰기가 가능하므로 Thread를 따로 할당해준다. */
    HANDLE hThread = NULL;
    /** Thread의 ID를 저장하기 위한 4byte 공간 변수 */
    DWORD dwThreadID = 0;
    /** Thread 생성 */
    hThread = ::CreateThread(
            NULL, // Thread에 대한 보안 속성에 대해서 정의 -> NULL은 상속을 받아서 사용한다는 의미
            0, // Thread에 대한 stack size 정의 기본 값은 -> 1MB
            IoThreadFunction, // Thread에서 실행할 함수에 대한 포인터 정보 넘겨주기
            hFile, // Thread에 넘겨줄 Parameter 값
            0, // Thread 생성에 대한 속성을 정의해서 넘겨주는 값 -> 0일 경우 기본적으로 생성이 되자마자 실행이 된다.
            &dwThreadID // Thread에 대한 ID 값을 저장하기 위한 변수의 주소 값을 넣어준다.
    );
    /** Thread에서 종료 혹은 신호가 올 때 까지 대기하는 함수 */
    ::WaitForSingleObject(hThread, INFINITE);
    ::CloseHandle(hThread);
    ::CloseHandle(hFile);
    
    return 1;
}
