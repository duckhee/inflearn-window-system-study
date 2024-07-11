#include <iostream>
#include <Windows.h>
#include <process.h>
#include <conio.h>

using namespace std;

UINT WINAPI ThreadFunction(LPVOID pParam) {
    /** 인자로 받은 값을 Event 객체로 casting 해서 받는다. */
    HANDLE hEvent = pParam;
    cout << "hThreadFunction() - Begin" << endl;
    /** 임시 대기하기 위한 함수 호출 -> _getch 는 문자 값이 있을 때까지 대기 하는 함수 */
    _getch();
    /** 이벤트에 대한 발행을 하는 함수 */
    SetEvent(hEvent);
    cout << "ThreadFunction() - SetEvent() published" << endl;
    cout << "ThreadFunction() - End" << endl;
    return 0;
}

int main(int argc, char **argv) {
    cout << "main() - Begin" << endl;
    /** Event 객체에 대해서 생성 */
    HANDLE hEvent = CreateEvent(
            NULL, // Event 객체에 대한 보안 속성을 설정하는 값 -> NULL 입력 시 부모 Thread의 속성을 상속 받는다.
            FALSE, // 초기화를 사용자가 직접 구현을 할지 자동으로 할지에 대해서 설정하는 값 -> FALSE일 경우 자동으로 해준다.
            FALSE, // 이벤트에 대한 초기 값에 대한 설정
            NULL // EVENT 객체에 대한 이름을 설정할 경우 문자열로 넣어주면 된다. -> 내부에서 사용되는 Event는 보통 이름을 지정하지 않는다.
    );
    UINT hThreadId = 0;
    HANDLE hThread = (HANDLE) ::_beginthreadex(
            NULL, // Thread에 대한 보안 속성에 대해서 설정 값을 넣어준다. -> NULL을 넣어줄 시 부모 Thread의 속성을 상속 받는다.
            0, // Thread에 부여할 Stack의 크기를 인자로 넣어준다. -> 기본 값은 1MB이다.
            ThreadFunction, // Thread 내부에서 실행할 함수에 대해서 넣어준다.
            (LPVOID )hEvent, // Thread 함수에 전달할 파라미터에 대해서 넣어준다.
            0, //
            &hThreadId // Thread에 대한 ID 값을 저장하기 위한 변수의 주소 값을 넣어준다.
    );

    /** 이벤트가 발행이 될 때까지 대기하는 조건문 -> 기다리고 있는 Thread에서 신호를 보냈을 경우  */
    if (::WaitForSingleObject(hEvent, INFINITE) == WAIT_OBJECT_0) {
        cout << "main() - WaitForSingleObject() return" << endl;
    }
    /** Thread 가 종료가 될때까지 기다리기 */
    ::WaitForSingleObject(hThread, INFINITE);
    cout << "main() - Thread End" << endl;
    /** Thread에 대한 Handle 값 해제 */
    ::CloseHandle(hThread);
//    ::CloseHandle(hEvent);
    cout << "main() - End" << endl;
    return 0;
}