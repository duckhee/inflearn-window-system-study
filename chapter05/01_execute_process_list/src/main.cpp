#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <Psapi.h>

using namespace std;

int main(int argc, char **argv) {
    /** console setting language */
    _wsetlocale(LC_ALL, L"korean");

    /** process id array */
    DWORD pidArray[512] = {0,};
    DWORD cbNeeded = 0;
    DWORD nProcesses = 0;
    TCHAR szBuffer[MAX_PATH + _MAX_FNAME] = {0,};
    UINT uCounter = 0;

    /** 프로세스 목록 조회 -> EnumProcess는 process의 pid만 가져온다. */
    BOOL isProcessList = ::EnumProcesses(
            pidArray, // 실행 중인 프로세스에 대한 pid를 저장할 배열을 인자로 받는다.
            sizeof(pidArray), // 프로세스의 pid를 저장할 배열의 크기를 인자로 받는다.
            &cbNeeded // 실제로 조회된 process의 갯수에 대한 값을 담을 인자로 받는다.
    );
    /** process의 조회에 대한 성공 시 */
    if (isProcessList) {
        /** 조회된 process 의 갯수 구하기 */
        nProcesses = cbNeeded / sizeof(DWORD);
        /** 프로세스의 정보 가져오기 위한 반복문 */
        for (DWORD i = 0; i <= nProcesses; i++) {
            /** process 핸들 객체 담을 변수 */
            HANDLE hProcess;
            /** 저장된 process의 pid 정보 가져오기 */
            DWORD pid = pidArray[i];
            /** pid로 process 열기 */
            hProcess = ::OpenProcess(
                    PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, // process에 대한 접근 권한에 대한 설정 -> 프로세스 상세 정보를 가져오기 위한 조건
                    FALSE, // 접근 권한에 대한 상속 하지 않기 위한 인자 값
                    pid // 읽어올 프로세스에 대한 pid 값을 인자로 받는다.
            );
            /** PROCESS HANDLE 값을 가져온 경우 */
            if (hProcess != NULL) {
                /** buffer 초기화 */
                ::ZeroMemory(szBuffer, MAX_PATH + _MAX_FNAME);
                /** 해당 프로세스의 경로 및 파일 이름 가져오기 */
                DWORD getModuleFileName = ::GetModuleFileNameEx(
                        hProcess, // 가져올 프로세스의 HANDLE 객체
                        NULL, // Module에 대한 경로 및 이름을 가져올 것이 아니기 때문에 NULL 값
                        szBuffer, // 경로 및 파일 이름을 저장할 버퍼
                        MAX_PATH + _MAX_FNAME // 버퍼의 최대 길이에 대한 범위 값
                );
                /** 파일의 이름 및 경로 가져 왔을 경우 */
                if (getModuleFileName != NULL) {
                    _tprintf(TEXT("[PID : %d] %s\r\n"), pid, szBuffer);
                }
                /** 상세 정보를 가져온 process의 HANDLE 객체 반환 */
                ::CloseHandle(hProcess);
            } else {
                _tprintf(TEXT("[PID: %d, Error Code : %d]\r\n"), pid, ::GetLastError());
                continue;
            }
        }
    }

    return 0;
}