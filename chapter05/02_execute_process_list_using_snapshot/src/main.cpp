#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <tlhelp32.h>


using namespace std;

int main(int argc, char **argv) {
    /** console setting */
    _wsetlocale(LC_ALL, L"korean");
    /** snapshot에 대한 HANDLE 객체를 담기 위한 변수 */
    HANDLE hSnapShot = INVALID_HANDLE_VALUE;
    /** snapshot 객체 생성 */
    hSnapShot = ::CreateToolhelp32Snapshot(
            TH32CS_SNAPPROCESS, // 스냅샷에 대한 설정 -> process에 대한 정보를 모두 포함 시키는 인자
            NULL // 모든 process에 대한 정볼ㄹ 가져오기 위한 인자 NULL -> 특정 값을 넣어주면 해당 process에 대한 검색을 한다.
    );
    /** snapshot에 대한 HANDLE 객체가 있을 경우 */
    if (hSnapShot != INVALID_HANDLE_VALUE) {
        /** process에 대한 정보를 담기 위한 구조체 */
        PROCESSENTRY32 processInfo = {sizeof(processInfo)};
        /** 처음에 process에 대한 정보를 조회 시 호출하는 함수 */
        BOOL isHave = ::Process32First(
                hSnapShot, // process에 대한 정보를 가져올 snapshot에 대한 HANDLE 값
                &processInfo // 담아줄 process에 대한 구조체
        );
        /** 다음 프로세스의 정보가 있을 경우 반복해서 가져오기 위한 반복 문 */
        for (; isHave; isHave = ::Process32Next(
                hSnapShot, // snapshot에 대한 HANDLE 객체
                &processInfo // process에 대한 정보를 담아줄 구조체
        )) {
            /** process에 대한 정보 출력 */
            _tprintf(TEXT("[PID : %d] %s\n"), processInfo.th32ProcessID, processInfo.szExeFile);
        }
        /** 사용한 snapshot에 대한 자원 반환 */
        ::CloseHandle(hSnapShot);
    }
    return 0;
}