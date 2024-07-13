
#include <iostream>
#include <stdio.h>
#include <process.h>
#include <Windows.h>

using namespace std;

/** semaphore 에 대한 제어하기 위한 global 변수 */
HANDLE g_hSema;
TCHAR g_StringList[10][64] = {0};

/** Thread에서 동작할 함수 정의 */
UINT WINAPI threadSemaphore(LPVOID pParam) {
    /** parameter 로 받은 값을 int 형으로 형 변환을 한다.*/
    int nIndex = *(int *) &pParam;

    while (TRUE) {
        /** 유니코드 값을 출력하기 위해서 사용하는 sprintf */
        ::wsprintf(
                g_StringList[nIndex],
                TEXT("%d thread is waiting!"),
                nIndex
        );
        /** semaphore 를 이용해서 접근이 가능한지를 대기하는 것 */
        DWORD dwResult = ::WaitForSingleObject(g_hSema, INFINITE);
        /** 유니코드 값을 출력하기 위해서 사용하는 sprintf */
        ::wsprintf(
                g_StringList[nIndex],
                TEXT("##%d thread is selected!##"),
                nIndex
        );
        ::Sleep(500);
        /** semaphore 의 자원을 가지고 하나의 session을 가져온다. */
        ::ReleaseSemaphore(
                g_hSema, // session에 대해서 관리를 하고 있는 Kernel Object 인 Semaphore 객체
                1, // 해당 객체에서 하나의 session을 사용하기 위해 1을 넣어준다.
                NULL // 현재까지 할당이 된 session에 대한 값을 가져오고 싶을 때에는 값을 저장할 변수의 주소를 넣어준다.
        );
    }
    return 0;
}

int main(int argc, char **argv) {
    /** semaphore 객체에 대한 생성 */
    g_hSema = ::CreateSemaphore(
            NULL, // 보안에 대한 속성에 대해서 정의 -> NULL은 생성하는 곳의 보안 속성을 상속을 받아서 사용한다는 의미이다.
            3, // 초기에 생성할 갯수 설정
            3, // 최대로 동시에 접속 가능한 갯수 설정
            NULL // semaphore에 대한 이름에 대한 정의
    );
    /** thread 에 대한 아이디를 저장하기 위한 변수 */
    UINT nThreadId = 0;
    /** Thread 에 대한 handler 저장하기 위한 변수 */
    HANDLE hThread = NULL;

    for (int i = 0; i < 10; i++) {
        /** Thread 생성 */
        hThread = (HANDLE) ::_beginthreadex(
                NULL, // Thread에 대한 보안 속성 정의
                0, // Thread에 대한 스텍 사이즈 설정
                threadSemaphore, // Thread에서 실행할 함수 정의
                (LPVOID) i, // Thread에 넘겨줄 파라미터
                0, // 초기 상태에 대한 전달
                &nThreadId // Thread 의 아이디 값을 저장할 변수 주소
        );
        /** Thread에 대한 handler 닫기 */
        ::CloseHandle(hThread);
    }

    while (1) {
        /** 화면을 초기화를 해주는 함수이다. */
        system("cls");
        for (int i = 0; i < 10; ++i) {
            /** puts 의 와일드 버전의 출력 -> 유니코드 값을 출력하기 위해서 사용 된다. */
//            _putws(g_StringList[i]);
            puts(g_StringList[i]);
        }
        ::Sleep(1000);
    }
    /** semaphore에 대한 객체 반환 */
    ::CloseHandle(g_hSema);
}