#include <iostream>
// Thread 함수를 사용하기 위해서는 window에서는 Window.h를 포함 시켜야 한다.
// Linux에서는 Thread 함수를 사용하기 위해서는 pthread.h를 포함 시켜야 한다.
#include <Windows.h>

using namespace std;

/**
* Thread에서 실행할 함수에 대해서 정의
*/
DWORD WINAPI ThreadFunction(LPVOID pParam) {
	cout << "ThreadFunction() - Begin" << endl;
	cout << (const char*)pParam << endl;
	cout << "ThreadFunction() - End" << endl;
	return 0;
}

int main() {

	cout << "main() - Begin" << endl;
	DWORD dwThreadId = 0;
	/** Thread에 대해서 생성 */
	HANDLE hThread = CreateThread(
		NULL, // 보안 속성에 대해서 설정 -> NULL 값은 부모의 Thread에 대한 속성을 상속을 받는다.
 		0, // Stack Size에 대한 설정 -> 0일 때는 compiler의 기본 값이 1MB를 설정하게 된다.
 		ThreadFunction, // Thread에서 실행할 함수에 대해서 함수 포인터의 값으로 넘겨준다. 
		(LPVOID)"PARAM", // Thread에 넘겨줄 매개변수의 값을 넣어준다.
		0, // Thread의 생성에 대한 속성을 정의해서 넘겨준다. -> 0일때는 기본적으로 생성이 되자마자 실행 된다.
		&dwThreadId // Thread에 대한 ID 값을 담아줄 주소 값을 넣어준다.
	);

	/** Thread가 생성이 안되어서 ID 값을 부여 받지 못했을 경우 */
	if (hThread == NULL) {

	}
	/** 해당 Thread가 변화가 있을 때 까지 대기하는 함수 */
	WaitForSingleObject(hThread, INFINITE);
	/**Thread에 대해서 사용후 반환을 하는 함수 */
	CloseHandle(hThread);
	cout << "main() - End" << endl;
	return 0;
}