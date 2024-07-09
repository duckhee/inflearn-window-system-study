// SleepRandom.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <windows.h>

int main()
{
	LARGE_INTEGER freq;
	LARGE_INTEGER begin;
	LARGE_INTEGER end;
	__int64 elapsed;

	//CPU 타이머 주파수 확인
	::QueryPerformanceFrequency(&freq);
	std::cout << "초당 주파수: " << freq.QuadPart << std::endl;


	for (int i = 0; i < 5; ++i)
	{
		::QueryPerformanceCounter(&begin);
		////////////////////////////////////////////////////////
		//우연에 맡기기 위한 코드 한 줄!
		::Sleep(10);
		////////////////////////////////////////////////////////
		::QueryPerformanceCounter(&end);

		elapsed = end.QuadPart - begin.QuadPart;

		std::cout << "실제로 흘러간 시간:" <<
			elapsed << std::endl;
		std::cout << "실제로 흘러간 시간(ms):" <<
			(double)elapsed / freq.QuadPart * 100 << std::endl;
		std::cout << "실제로 흘러간 시간(micro):" <<
			(double)elapsed / freq.QuadPart * 100 * 1000 << std::endl;
		std::cout << "랜덤 값(0~100):" <<
			elapsed % 100 << std::endl;
	}

	return 0;
}
