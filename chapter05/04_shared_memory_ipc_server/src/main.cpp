#include <iostream>
#include <Windows.h>
#include <tchar.h>



int main(int argc, char **argv) {
    /** Debugging 콘솔에 출력하기 위한 함수 */
    OutputDebugString(_T("Server - Begin\r\n"));

    /** Shared Memory 형태로 값 가져오기 */
    HANDLE hMap = ::CreateFileMapping(
            INVALID_HANDLE_VALUE, // 메모리 공간에 대한 접근을 하기 위한 옵션
            NULL, // 파일에 대한 속성  정의 -> NULL 일 경우 부모 상속
            PAGE_READWRITE, // 동작할 권한에 대해서 정의
            0, // 큰 공간을 부여할 경우 설정
            128, // 작은 범위일 경우 공간에 대한 설정
            TEXT("IPC_TEST_SHARED_MEMORY") // processor 수준에서는 이름을 부여해서 사용
    );
    /** 이미 존재할 경우 */
    if (::GetLastError() == ERROR_ALREADY_EXISTS) {
        /** File view 열기 */
        hMap = ::OpenFileMapping(
                FILE_MAP_ALL_ACCESS, // 접근 권한에 대해서 설정
                FALSE, // 상속 여부 설정 -> FALSE를 이용해서 보통 사용한다.
                TEXT("IPC_TEST_SHARED_MEMORY") // 가져올 객체의 이름
        );
        /** mapping 객체가 없을 경우 에러 반환 후 종료 */
        if (hMap == NULL) {
            _tprintf(TEXT("Failed to open file mapping obj (%d)\n"), GetLastError());
            return FALSE;
        }
    }
    /** 가져온 Memory 주소를 User Mode 에서 사용하기 위한 MapView 객체 생성 */
    TCHAR *pSharedMemory = (TCHAR *) ::MapViewOfFile(
            hMap,
            FILE_MAP_ALL_ACCESS, // 접근 권한에 대해서 설정
            0, // 큰 크기일 경우 여기서 가져올 공간 설정
            0, // offset 설정
            128 // 작은 크기일 경우 여기서 가져올 공간 설정
    );
    /** 가져온 메모리 주소가 없을 경우 */
    if (pSharedMemory == NULL) {
        _tprintf(TEXT("Failed to get shared memory. (%d)\r\n"), ::GetLastError());
        /** FileMapping 객체 릴리즈 */
        ::CloseHandle(hMap);
        return FALSE;
    }

    /** 공유 메모리 접근 및 읽기(수신완료) 이벤트 생서 (혹은 열기) */
    HANDLE hEvent = ::CreateEvent(
            NULL, // event 속성에 대해서 정의
            TRUE, // os에서 자동으로 이벤트 발행 후 초기화 지원 여부 설정
            FALSE, // 초기 상태 정의
            TEXT("IPC_SHAREDMEM_ACCESS") // 이벤트에 대한 이름 부여
    );

    /** 이벤트 생성 실패 시 */
    if (::GetLastError() == ERROR_ALREADY_EXISTS) {
        /** 이벤트 가져오기 */
        hEvent = ::OpenEvent(
                EVENT_ALL_ACCESS, // 접근 권한에 대한 설정
                FALSE, // 상속 여부 설정
                TEXT("IPC_SHAREDMEM_ACCESS") // 이벤트의 이름으로 가져오기
        );

        /** event 가 존재하지 않는 경우 */
        if (hEvent == NULL) {
            _tprintf(TEXT("Failed to open event obj. (%d)\r\n"), ::GetLastError());
            ::UnmapViewOfFile(pSharedMemory);
            ::CloseHandle(hMap);
            return FALSE;
        }
    }

    /** 수신 완료 이벤트 생성 */
    HANDLE hEventReceiveComplete = ::CreateEvent(
            NULL, // 이벤트에 대한 속성 정의
            TRUE, // 시스템에 의해서 초기화 여부 설정
            FALSE, // 초기 상태 정의
            TEXT("IPC_SHAREDMEM_RECV_COMPLETE") // 생성할 이벤트에 대한 이름 부여
    );

    /** 이벤트 이미 존재하는 경우 */
    if (::GetLastError() == ERROR_ALREADY_EXISTS) {
        hEventReceiveComplete = ::OpenEvent(
                EVENT_ALL_ACCESS, // 접근 권한 설정
                FALSE, // 상속 여부 설정
                TEXT("IPC_SHAREDMEM_RECV_COMPLETE") // 가져올 이벤트의 이름 설정
        );

        /** 이벤트가 존재하지 않는 경우 */
        if (hEventReceiveComplete == NULL) {
            _tprintf(TEXT("Failed to open event obj. (%d)\r\n"), ::GetLastError());
            ::UnmapViewOfFile(pSharedMemory);
            ::CloseHandle(hMap);
            ::CloseHandle(hEvent);
            return FALSE;
        }
    }

    /** processor 동기화를 위한 뮤텍스 생성 */
    HANDLE hMutex = ::CreateMutex(
            NULL, // mutex 의 속성 정의
            FALSE, // 상속 여부 설정
            TEXT("IPC_SHAREDMEM_MUTEX") // 뮤텍스에 이름 부여
    );
    /** mutex 가 이미 존재하는 경우 */
    if (::GetLastError() == ERROR_ALREADY_EXISTS) {
        /** mutex 가져오기 */
        hMutex = ::OpenMutex(
                MUTEX_ALL_ACCESS, // 뮤텍스에 대한 접근 권한
                FALSE, // 상속 여부 설정
                TEXT("IPC_SHAREDMEM_MUTEX") // 가져올 mutex 이름
        );

        if (hMutex == NULL) {
            _tprintf(TEXT("Failed to open Mutex obj. (%d)\r\n"), ::GetLastError());
            ::UnmapViewOfFile(pSharedMemory);
            ::CloseHandle(hMap);
            ::CloseHandle(hEvent);
            ::CloseHandle(hEventReceiveComplete);
            return FALSE;
        }
    }
    /** mutex lock 에 대한 획득 까지 대기 */
    DWORD mutexFlag = ::WaitForSingleObject(hMutex, INFINITE);
    if (mutexFlag != WAIT_OBJECT_0) {
        OutputDebugString(TEXT("Mutex Error\r\n"));
    } else {
        OutputDebugString(TEXT("Mutex Lock.\r\n"));
    }
    wsprintfW(pSharedMemory, _T("%s"), _T("Hello, Shared memory!"));
    /** Lock 반환 */
    ::ReleaseMutex(hMutex);
    OutputDebugString(TEXT("MUTEX Unlock.\n"));

    //6. 공유 메모리 쓰기 접근 및 쓰기 이벤트 세트
    OutputDebugString(TEXT("Message event set.\n"));
    ::SetEvent(hEvent);

    //7. 클라이언트 수신 완료 이벤트 대기
    OutputDebugString(TEXT("Waiting recv event...\n"));
    if (::WaitForSingleObject(hEventReceiveComplete, INFINITE) == WAIT_OBJECT_0)
        OutputDebugString(_T("Completion event!\n"));

    ::UnmapViewOfFile(pSharedMemory);
    ::CloseHandle(hMap);
    ::CloseHandle(hEvent);
    ::CloseHandle(hEventReceiveComplete);
    ::CloseHandle(hMutex);

    OutputDebugString(_T("Server - end\n"));
    return 0;
}
