#include <Windows.h>
#include <process.h>
#include <iostream>

using namespace std;

UINT WINAPI ThreadFunction(LPVOID pParam) {
	cout << "ThreadFunction() - Begin" << endl;
	cout << (const char*)pParam << endl;
	cout << "ThreadFunction() - End" << endl;
	return 0;
}

int main(int argc, char** argv) {
	cout << "main() - Begin" << endl;
	/** type 에 대한 재정의가 되어 있다. -> unsigned int 형의 재정의 */
	UINT nThreadId = 0;
	/** Thread에 대한  생성 및 실행 -> 반환 값으로 Thread에 대한 handle 정보를 가져온다.*/
	HANDLE hThread = (HANDLE)::_beginthreadex(
		NULL, // 보안에 대한 설정 -> NULL은 부모의 보안 속성을 상속을 받아서 사용을 한다는 의미이다.
		0, // Thread에 부여할 stack 의 크기를 설정 해주는 것 -> 0은 기본 값인 1MB를 부여한다는 것이다.
		ThreadFunction, // Thread에서 실행할 함수에 대한 함수 포인터 넣어주기
		(LPVOID)"PARAM", // Thread에 함수에 넣어줄 파라미터에 대해서 넣어준다.
		0, // Thread에 대한 초기 상태에 대해서 설정하는 매개변수이다. 
		&nThreadId // 쓰레드에 부여가 되는 ID 값을 담을 변수에 대한 주소 값을 넣어준다.
	);

	// 쓰레드가 정상적으로 생성이 되지 않았을 때 동작하기 위한 if 문
	if (hThread == NULL) {
		cout << "ERROR: Failed to begin Thread." << endl;
		return 0;
	}
	// 자식 프로세서가 종료가 될때까지 대기하기 위한 함수 -> 대기할 Thread에 대한 handle 정보와 대기할 시간에 대해서 매개변수로 넣어준다.
	::WaitForSingleObject(hThread, INFINITE);
	// 사용을 다한 Thread에 대한 handle에 대해서 닫아주는 함수 -> 생성 시에는 항상 닫아줘야한다.
	::CloseHandle(hThread);
	cout << "main() - End" << endl;
	return 0;
}