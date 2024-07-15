#include <iostream>
#include <Windows.h>

#define UNICODE

using namespace std;

void LoadDiskInfo(const TCHAR *pszPath);

void LoadFileList(const TCHAR *pszPath);

int main(int argc, char **argv) {
    /** console 에 대한 언어에 대한 설정을 한국어로 설정 -> 한국어로 설정 안할 시 언어에 한글이 있을 경우 깨지는 현상이 발생한다.*/
    ::_wsetlocale(LC_ALL, TEXT("korean"));
    /** C:\\ 디스크에 있는 정보를 가져오는 함수 */
    LoadDiskInfo(TEXT("C:\\"));
    /** C:\\ 폴더 위치에 있는 파일에 대한 정보를 가져오는 함수 */
    LoadFileList(TEXT("C:\\"));
    /** 현재 폴더의 위치에 대한 정보를 저장하기 이한 변수 */
    TCHAR szCurDir[MAX_PATH] = {0};
    /** 현재 폴더에 대한 정보를 가져오기 위한 함수 */
    ::GetCurrentDirectory(MAX_PATH, szCurDir);
    /** 현재 폴더에 대한 경로 출력 */
    wprintf(TEXT("Current Directory: %s\n"), szCurDir);
//    printf(TEXT("Current Directory: %s\n"), szCurDir);
    return 0;
}

/** disk 에 대한 정보를 가져오는 함수 */
void LoadDiskInfo(const TCHAR *pszPath) {
    /** 디스크에 대한 정보를 담기 위한 배열 */
    TCHAR szVolume[MAX_PATH] = {0,};
    /** 시스템 파일에 대한 정보를 담기 위한 배열 */
    TCHAR szFileSys[MAX_PATH] = {0,};
    /** 최상위 경로를 저장하기 위한 배열 */
    TCHAR szRoot[MAX_PATH] = {0,};
    /** */
    DWORD dwSerialNumber = 0, dwMaxCompLen = 0, dwSysFlag = 0;
    /** C:\\을 넣어주므로 해당 값만큼 복사 */
    memcpy(szRoot, pszPath, sizeof(TCHAR) * 3);
    /** 해당 경로의 디스크 정보를 가져오기 위한 함수 */
    ::GetVolumeInformation(
            szRoot, // 찾을 경로의 위치를 넣어준다.
            szVolume, // 현재 디스크의 이름에 대한 정보를 담기 위한 변수
            MAX_PATH, // 이름의 최대로 받을 수 있는 사이즈
            &dwSerialNumber, // 볼륨에 대한 일련번호를 저장하기 위한 변수
            &dwMaxCompLen, // 최대의 파일에 대한 길이에 대한 값을 받을 변수
            &dwSysFlag, // 현재 디스크의 시스템 포맷 정보
            szFileSys, // 파일 시스템의 이름을 담기 위한 변수
            MAX_PATH // 파일 시스템 최대 받을 수 있는 사이즈
    );
    /** 디스크에 대한 이름과 파일 시스템에 대한 종류에 대해서 출력 */
    wprintf(TEXT(("Volume name : %s, File System: %s\n")), szVolume, szFileSys);
//    printf(TEXT("Volume name : %s, File System: %s\n"), szVolume, szFileSys);
    /** 하위 시스템에 지원을 하기 위해서 ULARGE_INTEGER 공용체를 사용한다. */
    /** 현재 사용할 수 있는 공간에 대한 정보를 담기 위한 공용체 */
    ULARGE_INTEGER llAvailableSpace = {0,};
    /** 최대 공간에 대한 정보를 담기 위한 공용체 */
    ULARGE_INTEGER llTotalSpace = {0,};
    /** 사용할 수 있는 공간에 대한 정보를 담기 위한 공용체 */
    ULARGE_INTEGER llFreeSpace = {0,};
    /** 디스크에 대한 용량에 대한 정보를 가져오는 함수 */
    if (::GetDiskFreeSpaceEx(szRoot, &llAvailableSpace, &llTotalSpace, &llFreeSpace)) {
        /** 사용 가능한 용량과 최대 용량 출력 */
        wprintf(
                TEXT("(Disk free space : %16u/%16u GB\n"),
                llFreeSpace.QuadPart / (1024 * 1024 * 1024),
                llTotalSpace.QuadPart / (1024 * 1024 * 1024)
        );

//        printf(TEXT("(Disk free space : %16u/%16u GB\n"), llFreeSpace.QuadPart / (1024 * 1024 * 1024), llTotalSpace.QuadPart / (1024 * 1024 * 1024));

    }
}

void LoadFileList(const TCHAR *pszPath) {
    TCHAR szPath[MAX_PATH] = {0,};
    /**
     * *.exe: 실행파일만 보기
     * a*.txt: a로 시작하는 텍스트 파일만 보기
     * 모든 파일 보기 *.*
     * */
    wsprintf(szPath, TEXT("%s\\*.*"), pszPath);
    /** 파일에 대한 정보를 담기 위한 구조체 */
    WIN32_FIND_DATA FindData;
    /** 파일이 있는지 없는지 확인하기 위한 FLAG */
    BOOL bResult = TRUE;
    /** 구조체에 대한 초기화 */
    ::ZeroMemory(
            &FindData,
            sizeof(FindData)
    );
    /** 파일 리스트로 해당 경로에 파일이 있을 경우 사용한다. -> 파일에 대해서 확인하기 위해서 처음에 사용하는 함수 이다. */
    HANDLE hFirstFile = ::FindFirstFile(
            (LPTSTR) (const TCHAR *) szPath, // 경로에 대한 값
            &FindData // 파일에 대한 정보가 있을 때 담아줄 구조체
    );
    /** 파일을 모두 읽어오기 위한 반복문 */
    while (bResult) {
        /** 해당 파일이 폴더일 경우 */
        if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            /** 폴더에 대한 출력 */
            wprintf(TEXT("[DIR] %s\n"), FindData.cFileName);
            //printf(TEXT("[DIR] %s\n"), FindData.cFileName);
        } else {
            /** 파일에 대한 이름 출력 */
            wprintf(TEXT("%s\n"), FindData.cFileName);
            //printf(TEXT("%s\n"), FindData.cFileName);
        }
        /** 파일이 더 있으면 파일에 대한 정보를 가져오기 위한 함수 */
        bResult = ::FindNextFile(
                hFirstFile, // 처음 가져온 파일에 대한 HANDLE 값
                &FindData // 파일에 대한 값을 저장하기 위한 구조체
        );
    }
}