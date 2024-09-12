// TestDLL.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>

/** Runtime 에 호출할 라이브러리에 대해서 정보 알려주기 -> 해당 라이브러리의 경로를 알려준다. */
#pragma comment(lib, "C:\\Users\\fain9\\source\\repos\\inflearn-window-system-study\\chapter06\\01_sample_dll\\x64\\Debug\\01_dll_library.lib")
extern "C" {
void WINAPI DLLTestFunction(int nParam);
}
int main()
{
    /** 가져와서 사용할 함수에 대해서 호출 */
    DLLTestFunction(5);
    std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started:
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
