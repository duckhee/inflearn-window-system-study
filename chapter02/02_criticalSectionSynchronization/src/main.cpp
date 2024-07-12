#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <process.h>

char *g_pszBuffer = NULL;
/** 임계 구간이라고 말할 수 있는 부분에 산정해서 막아주면 해결이 된다. */
CRITICAL_SECTION g_cs;

void setString(const char *pszData) {
    /** 임계 구간에 대한 Lock 을 걸어주는 것 */
    ::EnterCriticalSection(&g_cs);
    if (g_pszBuffer != NULL) {
        free(g_pszBuffer);
        g_pszBuffer = (char *) malloc(64);
        sprintf_s(g_pszBuffer, 64, "%s", pszData);
    } else {
        g_pszBuffer = (char *) malloc(64);
        sprintf_s(g_pszBuffer, 64, "%s", pszData);
    }
    /** 임계 구간에 대한 사용 종료 후 Lock 을 해제 하는 것 */
    ::LeaveCriticalSection(&g_cs);
}

BOOL getString(char *pszData) {
    /** 임계 구간에 대한 Lock 을 걸어주는 것 */
    ::EnterCriticalSection(&g_cs);
    if (pszData != NULL) {
        sprintf_s(pszData, 64, "%s", g_pszBuffer);
        free(g_pszBuffer);
        g_pszBuffer = NULL;
        /** 임계 구간에 대한 사용 종료 후 Lock 을 해제 하는 것 */
        ::LeaveCriticalSection(&g_cs);
        return TRUE;
    }
    /** 임계 구간에 대한 사용 종료 후 Lock 을 해제 하는 것 */
    ::LeaveCriticalSection(&g_cs);
    return FALSE;
}

UINT threadFunction1(LPVOID pParam) {
    while (TRUE) {
        ::Sleep(1);
        setString("ThreadFunction1");
    }
    return 0;
}

UINT threadFunction2(LPVOID pParam) {
    while (TRUE) {
        ::Sleep(1);
        setString("ThreadFunction2");
    }
    return 0;
}

int main(int argc, char **argv) {
    /** 임계영역에 대한 Lock 접근에 대해서 사용하기 위한 CRITICAL_SECTION 에 대한 초기화 */
    ::InitializeCriticalSection(&g_cs);
    UINT nThreadId = 0;
    HANDLE hThread = (HANDLE) ::_beginthreadex(
            NULL, // 보안에 대한 속성 정의
            0, // thread stack size 에 대한 설정 -> 기본 값은 1MB
            threadFunction1, // 동작할 흐름에 대한 함수
            NULL, // 흐름에 넣어줄 매개변수 값
            0, // Thread의 생성에 대한 속성을 정의해서 넘겨준다. -> 0일때는 기본적으로 생성이 되자마자 실행 된다.
            &nThreadId // 쓰레드에 대한 ID 값을 저장할 변수의 주소 값
    );
    if (hThread == NULL) {
        printf("failed create thread\r\n");
    }
    /** 첫번째 Thread 에 대한 handle 반환*/
    ::CloseHandle(hThread);
    hThread = (HANDLE) ::_beginthreadex(
            NULL, // 보안에 대한 속성 정의
            0, // thread stack size 에 대한 설정 -> 기본 값은 1MB
            threadFunction2, // 동작할 흐름에 대한 함수
            NULL, // 흐름에 넣어줄 매개변수 값
            0, // Thread의 생성에 대한 속성을 정의해서 넘겨준다. -> 0일때는 기본적으로 생성이 되자마자 실행 된다.
            &nThreadId // 쓰레드에 대한 ID 값을 저장할 변수의 주소 값
    );
    if (hThread == NULL) {
        printf("failed create thread\r\n");
    }


    char szBuffer[64];
    for (int i = 0; i < 5; ++i) {
        ::Sleep(500);

        getString(szBuffer);
        puts(szBuffer);
    }

    ::WaitForSingleObject(hThread, INFINITE);
    ::CloseHandle(hThread);
    /** 사용한 임계영역 접근을 위한 CRITICAL_SECTION 에 대한 자원 반환 */
    ::DeleteCriticalSection(&g_cs);
    return 0;
}