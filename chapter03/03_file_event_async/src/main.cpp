#include <iostream>
#include <Windows.h>
#include <tchar.h>

using namespace std;

int main(int argc, char **argv) {
    /** 파일에 대해서 생성을 위한 여래기 */
    HANDLE hFile = ::CreateFile(
            _T("TestFile.txt"), // 생성할 파일에 대한 이름 정의
            GENERIC_WRITE, // 파일 쓰기 모드로 열기
            0, // 공유 모드에 대해서 비 공유 모드로 설정
            NULL, // 보안 설정에 대해서 상속을 받도록 설정
            CREATE_ALWAYS, // 항상 새로운 파일을 만들기 위한 설정
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // 파일에 대한 기본 속성과 비동기 요청에 대해서 사용하기 위한 속성 추가
            NULL
    );
    /** */
    DWORD dwRead;
    /** */
    OVERLAPPED aOverLapped[3] = {0,};
    /** 파일에 대한 이벤트를 저장하기 위한 배열 */
    HANDLE aFileEvent[3] = {0,};
    for (int i = 0; i < 3; i++) {
        /** 파일에 대한 쓰기 이벤트를 저장하기 위한 생성 */
        aFileEvent[i] = ::CreateEvent(
                NULL, // 이벤트에 속성을 기본값으로 설정
                FALSE, // 프로그램이 자동적으로 reset을 하도록 설정
                FALSE, // 초기 모드에 대한 설정
                NULL // 이벤트 객체에 대한 이름 설정
        );
        /** 중첩된 처리를 위한 객체애 해당 이벤트 핸들러 추가 */
        aOverLapped[i].hEvent = aFileEvent[i];
    }

    aOverLapped[0].Offset = 0; // 처음 부터 시작하도록 offset 설정
    aOverLapped[1].Offset = 1024 * 1024 * 5; // 5MB 부터 시작하도록 offset 조정
    aOverLapped[2].Offset = 16; // 16byte 다음 부터 시작하도록 offset 조정
    /** 파일에 대한 쓰기 요청 만들기 */
    for (int i = 0; i < 3; i++) {
        printf("%d번쨰 중첩된 쓰기 시도 \n", i);
        /** 파일에 쓰기 요청을 만들기 */
        ::WriteFile(
                hFile, // 쓸 파일에 대한 HANDLE 객체 등록
                "0123456789", // 파일에 쓸 데이터가 담겨져 있는 Buffer 또는 값 넣기
                10, // 파일에 쓸 데이터에 대한 갯수
                &dwRead, // 실제로 쓴 파일에 대한 값을 담기 위한 변수 주소 등록
                &aOverLapped[i] // 중첩된 요청일 경우 해당 값에 중첩에 대한 정보를 가져오기 위한 구조체 주소 등록
        );
        /**
         * ERROR_IO_PENDING는 현재 진행 중이지만, 아직 끝나지 않았다고 알려주는 이벤트이다.
         * */
        /** 현재 요청이 진행 중일 경우를 제외한 에러에 대한 처리를 위한 조건문 */
        if (::GetLastError() != ERROR_IO_PENDING) {
            ::exit(0);
        }
    }

    /** 몇번째 요청인지를 가져오기 위한 변수 */
    DWORD dwResult = 0;
    for (int i = 0; i < 3; i++) {
        /** 요청에 대해서 전체다 끝나기를 기다리는 함수 */
        dwResult = ::WaitForMultipleObjects(
                3,  // 몇개의 event를 대기할 것인지 숫자 등록
                aFileEvent, // 기다릴 이벤트에 대한 정보들이 담겨 있는 배열을 등록
                FALSE, // 이벤트에 대해서 하나씩 받기 위한 FALSE 설정 -> TRUE 설정 시 등록된 HANDLE이 모두 완료가 되면 반환한다.
                INFINITE // 대기할 시간에 대한 정의
        );
        /** 결과 값에 WAIT_OBJECT_O를 빼주면 몇 번째 배열에 있는 값인지 확인이 가능하므로 몇번째 이벤트가 끝났는지 출력 */
        printf("-> %d번쨰 쓰기 완료\r\n", (dwResult - WAIT_OBJECT_0));
    }
    /** 이벤트에 대해서 HANDLE 객체 반환 */
    for (int i = 0; i < 3; i++) {
        ::CloseHandle(aFileEvent[i]);
    }
    /** 파일에 대해서 사용 후 파일 포인터 반환 */
    ::CloseHandle(hFile);
    return 0;
}