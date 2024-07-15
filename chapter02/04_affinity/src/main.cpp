#include <iostream>
#include <Windows.h>
#include <process.h>

using namespace std;

UINT WINAPI threadFunction(LPVOID pParam) {
    /** 점유율을 보기 위한 의미 없는 연산 */
    int nTmp = 0;
    /** sleep 함수가 없으므로 해당 CPU에 대한 점유율은 100%가 된다. */
    while (1) {
        ++nTmp;
    }
    return 0;
}

int main(int argc, char **argv) {
    UINT nThreadId = 0;
    HANDLE hThread = (HANDLE) ::_beginthreadex(
            NULL,
            0,
            threadFunction,
            NULL,
            0,
            &nThreadId
    );
    if (hThread == NULL) {
        cout << "ERROR: Failed to begin thread." << endl;
        return 0;
    }

    /** core를 순회를 하면서 점유 하는 것을 확인하기 위한 반복문 8 core 까지 확인 */
    for (int i = 0; i < 8; i++) {
        /**
         * Thread에 대한 특정 core에 대한 점유를 하기 위해서는 점유할 Thread에 대한 handle 값과 core의 위치 값을 넣어줘야 한다.
         * shift 연산을 통해 왼쪽으로 이동하면서 core의 값을 변경을 해준다.
         * */
        ::SetThreadAffinityMask(hThread, 0x00000001 << i);
        /** 확인하기 위한 sleep */
        ::Sleep(5000);
    }
    ::WaitForSingleObject(hThread, INFINITE);
    ::CloseHandle(hThread);
    return 0;
}