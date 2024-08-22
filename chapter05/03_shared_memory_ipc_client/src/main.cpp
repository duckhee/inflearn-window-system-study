#include <iostream>
#include <tchar.h>
#include <Windows.h>

int main(int argc, char **argv) {
    /** Debugging Console에 출력하기 위한 함수 */
    OutputDebugString(_T("Shared Memory Client - Begin\n"));
    /** 파일 매핑 객체 생성 -> 파일을 여는 것이 아니라 커널 영역의 메모리에 접근해서 메모리 할당을 한다. */
    HANDLE hMap = ::CreateFileMapping(
            INVALID_HANDLE_VALUE, // 파일을 가지고 매핑을 하는 것이 아니라 메뢰를 할당 해서 공유하기 위해서 INVALID_HANDLE_VALUE를 사용을 한다.
            NULL, // 파일 매핑 객체의 보안 수준에 대해서 정의한다. -> NULL 값일 경우에는 부모의 보안 수준을 상속을 받는다.
            PAGE_READWRITE, // 파일 매핑 객체를 가지고 할 행위에 대해서 인자로 받는다.
            0, // 파일 매핑으로 가져올 수 있는 메모리 크기 설정 -> 매우 큰 크기로 가져와야할 때 사용이 된다.
            128, // 최대로 가질 수 있는 크기에 대해서 정의 한다.
            TEXT("IPC_TEST_SHARED_MEMORY") // FileMapping에 대한 객체 이름 부여 -> process 수준에서 사용을 할 때에는 일반적으로 이름을 부여해서 가져온다.
    );
    /** 생성 시 이미 존재할 경우 존재하는 파일 매핑 객체를 열어서 가져온다. */
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        /** 이미 존재하는 FileMapping 객체를 열기 */
        hMap = ::OpenFileMapping(
                FILE_MAP_ALL_ACCESS, // 모든 접근 권한에 대해서 허용을 하기 위해서 FILE_MAP_AlL_ACCESS 값을 넣어준다.
                FALSE, // 상속을 사용하지 않기 위해서 FALSE 값을 넣어준다.
                TEXT("IPC_TEST_SHARED_MEMORY") // 가져올 파일 매핑 객체의 이름을 인자로 받는다.
        );
        /** FileMapping 객체를 열기에 실패 했을 경우 */
        if (hMap == NULL) {
            _tprintf(TEXT("Failed to open File Mapping Obj. (%d)\n"), ::GetLastError());
            return -1;
        }
    }

    /** 매핑 객체에 대해 접근을 하기 위해서 User Mode에서 접근이 가능한 형태로 만들어준다. (메모리 추상화) */
    TCHAR *pSharedMemory = (TCHAR *) ::MapViewOfFile(
            hMap, // 가져올 FileMapping 객체의 HANDLE 값을 인자로 받는다.
            FILE_MAP_ALL_ACCESS, // MapViewOfFile에 대한 접근 권한에 대해서 값을 넣어준다.
            0, // MapViewOfFile 객체가 FileMapping 객체의 offset에 대해서 설정하는 인자이다.
            0, // MapViewOfFile 객체가 FileMapping 객체의 offset에 대해서 설정하는 인자이다.
            128 // MapViewOfFile 객체를 통해서 접근할 총 메모리 공간의 크기를 인자로 받는다.
    );

    /** MapView 객체 확인 */
    if (pSharedMemory == NULL) {
        _tprintf(TEXT("Failed to get shared memory (%d)\n"), ::GetLastError());
        /** FileMapping 객체 반환 */
        ::CloseHandle(hMap);
        return -1;
    }

    /** 공유 메모리 접근 및 일기(수신 완료) 이벤트 생성 (혹은 개방) */
    HANDLE hEvent = ::CreateEvent(
            NULL,  // 이벤트에 대한 속성 정의 값을 인자로 받는다.
            TRUE, // 운영체제가 이벤트에 대한 RESET을 관리를 해주기 위해서 TRUE 값을 넣어준다. -> FALSE 값일 시 사용자가 ResetEvent를 이용해서 Event에 대한 상태 관리를 해줘야 한다.
            FALSE, //이벤트의 초기 상태 값을 정의하기 위한 값을 인자로 받는다. -> TRUE일 경우 이벤트가 생성이 되었을 때 발행이 된다.
            TEXT("IPC_SHAREDMEM_ACCESS") // 이벤트에 대한 이름을 인자로 받는다. -> Process 끼리의 사용을 할 때에는 일반적으로 이름을 부여해서 사용을 한다.
    );
    /** 이벤트가 이미 존재하는 경우 */
    if (::GetLastError() == ERROR_ALREADY_EXISTS) {
        /** 존재하는 이벤트를 가져오는 함수 */
        hEvent = ::OpenEvent(
                EVENT_ALL_ACCESS, // 접근 형태에 대한 정의 -> 모든 접근에 대해서 허용
                FALSE, //핸들러에 대한 상속 여부에 대한 설정 FALSE는 상속을 하지 않는다. -> TRUE 일 경우 Processor에서 생성한 이벤트를 상속을 받아서 이벤트를 재 정의한다.
                TEXT("IPC_SHAREDMEM_ACCESS") // 이벤트에 대한 이름을 인자로 받는다. -> Process 끼리의 사용을 할 때에는 일반적으로 이름을 부여해서 사용을 한다.
        );
        /** event 객체를 가져오기 실패 시 */
        if (hEvent == NULL) {
            _tprintf(TEXT("Failed to open event (%d)\n"), ::GetLastError());
            /** MapView 객체 메모리 해제 */
            ::UnmapViewOfFile(pSharedMemory);
            /** File Mapping 객체 해제 */
            ::CloseHandle(hMap);
            return -1;
        }
    }

    /** 공유 메모리를 이용해서 데이터 수신이 완료가 되었을 때 발행할 이벤트 생성 */
    HANDLE hEventComplete = ::CreateEvent(
            NULL, // 이벤트에 대한 속성 정의 값을 인자로 받는다.
            TRUE, // 이벤트 상태 변경에 대해서는 OS에게 위임 하기 위한 TRUE 값
            FALSE, //이벤트의 초기 상태 값을 정의하기 위한 값을 인자로 받는다. -> TRUE일 경우 이벤트가 생성이 되었을 때 발행이 된다.
            TEXT("IPC_SHAREDMEM_RECV_COMPLETE") // 생성한 이벤트에 대해서 이름 부여 -> processor 수준에서 사용을 할 때에는 이름을 부여해서 사용을 한다.
    );

    /** 이미 수신 완료 이벤트가 존재하는 경우 */
    if (::GetLastError() == ERROR_ALREADY_EXISTS) {
        /** 존재하는 이벤트 객체 열기 */
        hEventComplete = ::OpenEvent(
                EVENT_ALL_ACCESS, // 접근 권한에 대해서 설정
                FALSE, // 이벤트 객체를 상속해서 사용을 할 것인지에 대해서 정의하는 값
                TEXT("IPC_SHAREDMEM_RECV_COMPLETE") // 생성한 이벤트에 대해서 이름 부여 -> processor 수준에서 사용을 할 때에는 이름을 부여해서 사용을 한다.
        );
        /** 이벤트 객체 가져오기 실패 시 */
        if (hEventComplete == NULL) {
            _tprintf(TEXT("Failed to open event (%d)\n"), ::GetLastError());
            /** MapView 객체 메모리 해제 */
            ::UnmapViewOfFile(pSharedMemory);
            /** Mapping 객체 메모리 해제 */
            ::CloseHandle(hMap);
            /** 수신 확인 이벤트 메모리 해제 */
            ::CloseHandle(hEvent);
            return -1;
        }
    }

    /** Process 동기화를 위한 Mutex 생성 */
    HANDLE hMutex = ::CreateMutex(
            NULL, // mutex에 대한 보안 속성 정의하기 위한 값을 인자로 받는다. -> NULL 의 경우 보안 속성을 상속을 받아서 사용한다.
            FALSE, // 해당 Mutex에 대한 소유권을 획드할 때에는 TRUE 값이 들어가고 FALSE의 경우 소유권을 가져오지 않는다.
            TEXT("IPC_SHAREDMEM_MUTEX") // Mutex 이름 부여 -> Process 수준에서 사용할 때 보통 이름을 부여한다.
    );
    /** 해당 Mutex 가 존재할 경우 */
    if (::GetLastError() == ERROR_ALREADY_EXISTS) {
        /** 존재하는 Mutex 열기 */
        hMutex = ::OpenMutex(
                MUTEX_ALL_ACCESS, // Mutex에 대한 사용할 권한에 대해서 설정
                FALSE, // 상속을 해서 사용할지에 대해서 정의 값
                TEXT("IPC_SHAREDMEM_MUTEX") // Mutex 이름 부여 -> Process 수준에서 사용할 때 보통 이름을 부여한다.
        );

        /** Mutex 가져오기 실패 시 */
        if (hMutex == NULL) {
            _tprintf(TEXT("Failed to open mutex (%d)\n"), ::GetLastError());
            /** MapView 객체 메모리 해제 */
            ::UnmapViewOfFile(pSharedMemory);
            /** Mapping 객체 메모리 해제 */
            ::CloseHandle(hMap);
            /** 수신 완료 이벤트 메모리 해제 */
            ::CloseHandle(hEventComplete);
            /** 수신 확인 이벤트 메모리 해제 */
            ::CloseHandle(hEvent);
            return -1;
        }
    }

    /** 공유 메모리에 쓰기 이벤트 대기 */
    OutputDebugString(TEXT("Waiting message from server ...\n"));
    /** 서버에서 공유 메모리에 쓰기 이벤트 발행할 때 까지 대기 하기 위한 함수 */
    DWORD isHaveEvent = ::WaitForSingleObject(
            hEvent, // 기다릴 HANDLE 객체 -> Event 에 대한 값 설정 대기
            INFINITE // 대기할 기간에 대한 설정 -> 무한 대기
    );
    /** 이벤트 발행 확인 */
    if (isHaveEvent == WAIT_OBJECT_0) {
        /** Mutex 를 이용해서 Lock 획득을 위한 대기 -> 뮤텍스를 이용한 프로세스 동기화 (메모리 접근) */
        if (::WaitForSingleObject(hMutex, INFINITE) != WAIT_OBJECT_0) {
            OutputDebugString(TEXT("MUTEX Error.\r\n"));
        } else {
            OutputDebugString(TEXT("MUTEX Lock.\n"));
        }
        /** 공유 메모리에 적혀 있는 값을 읽어서 debug console 에 출력 */
        OutputDebugString(pSharedMemory);
        /** Mutex Lock 반환 */
        ::ReleaseMutex(hMutex);
        OutputDebugString(TEXT("MUTEX UnLock\n"));

        /** 메모리에 읽기 완료 이벤트 발행 */
        ::SetEvent(hEventComplete);
        OutputDebugString(_T("Completion event for server!\n"));
    }

    ::UnmapViewOfFile(pSharedMemory);
    ::CloseHandle(hMap);
    ::CloseHandle(hEvent);
    ::CloseHandle(hEventComplete);
    ::CloseHandle(hMutex);

    _tprintf(_T("Client - end\n"));
    return 0;

}