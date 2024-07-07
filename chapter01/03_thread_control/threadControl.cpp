#include <iostream>
#include <process.h>
#include <windows.h>

using namespace std;

/** worker thread에서 탈출학 위한 조건 */
int g_flag = FALSE;

/** Thread에서 동작할 함수 */
UINT WINAPI ThreadFunction(LPVOID pParam) {
	cout << "ThreadFunction() - Start" << endl;
	g_flag = TRUE;
	/** Thread 에 대한 정지 및 동작에 대해서 알기 위한 반복문 */
	while (g_flag) {
		cout << "worker thread - while loop" << endl;
		/*1초동안 대기하기 위한 대기 함수*/
		::Sleep(1000);
	}
	cout << "ThreadFunction() - End" << endl;
	return NULL;
}

int main(int argc, char** argv) {
	cout << "main() - Start" << endl;
	/*쓰레드에 대한 아이디 값을 저장하기 위한 변수*/
	UINT nThreadId = 0;
	/* 쓰레드에 대한 생성 */
	HANDLE hThread = (HANDLE)::_beginthreadex(
		NULL, // Thread에 대한 접근 권한 및 보안에 대해서 부모 쓰레드의 값을 상속
		0, // Thread에 대한 stack size 설정 -> 0 = 1MB
		ThreadFunction, // Thread에서 동작하기 위한 함수
		(LPVOID)"PARAM", // Thread에서 실행이 되는 함수에 넘겨줄 매개변수
		0, // Thread의 초기 상태에 대해서 설정
		&nThreadId // Thread의 아이디 값을 저장할 변수 포인터
	);
	/* 쓰레드 정상 생성 실패 시 */
	if (hThread == NULL) {
		cout << "ERROR: Failed to begin thread" << endl;
		return 0;
	}
	/* 메인 스레드가 대기하기 위한 설정*/
	::Sleep(2000);
	cout << "main() - SuspendThread()" << endl;
	/* 쓰레드에 대해서 대기 모드로 들어가기 위한 함수 */
	::SuspendThread(hThread);
	/* 쓰레드에 대한 상태를 확인하기 위한 반복문 */
	for (int i = 0; i < 3; i++) {
		/* 해당 쓰레드에 대한 상태를 확인하기 위한 함수 -> 1초동안만 대기*/
		::WaitForSingleObject(hThread, 1000);
		cout << "main() - WaitForSingleObject(1000)" << endl;
	}
	/*대기 상태에 있는 쓰레드 재시작*/
	::ResumeThread(hThread);
	/*2초동안 대기*/
	::Sleep(2000);
	/* 쓰레드에서 보고 있는 전역 변수의 값 변경*/
	g_flag = FALSE;
	/*쓰레드의 함수가 종료가 될 때까지 무한 대기*/
	::WaitForSingleObject(hThread, INFINITE);
	/* 쓰레드에 대하 사용 종료 후 쓰레드 닫기*/
	::CloseHandle(hThread);
	cout << "main() - End" << endl;
	return 0;
}