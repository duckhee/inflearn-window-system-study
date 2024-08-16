#include <iostream>
#include <stdio.h>
#include <Windows.h>


int main(int argc, char **argv) {
    /** 데이터가 보호가 되는 영역인 static에 읽기 전용 영역에 있다. */
    const char *pszHello = "Hello, world!";
    printf("%s\r\n", pszHello);
    /** 메모리 모드에 대한 설정 값을 저장하기 위한 변수 */
    DWORD dwOldProtected = 0;
    /** 메모리 보호 모드 변경 */
    ::VirtualProtect(
            (LPVOID) pszHello,
            16,
            PAGE_READWRITE,
            &dwOldProtected
    );
    printf("get old protection : %d\r\n", dwOldProtected);
    /** 보호 모드를 변경을 했기 때문에 해당 값에 대한 쓰기가 가능 */
    strcpy_s((char *) pszHello, 16, "nullnull!=null");
    printf("%s\r\n", pszHello);
    return 0;
}