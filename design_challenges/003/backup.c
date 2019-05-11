#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
/*
백업해야할 파일(FILENAME)을 백업 리스트에 새롭게 추가
백업 리스트는 링크드 리스트로 구현
백업 리스트 구조체는 1. 파일의 절대경로, 2. 백업 주기, 3. 백업 옵션 등을 포

라. add 명령어 10
마. add –m 옵션 4
바. add –n 옵션 4
사. add –t 옵션 5
아. add –d 옵션 

스트럭트 *백업파일헤드 {
    백업파일개수
    
    스트럭트 * 백업파일첫노드
}


스트럭트 백업파일노드 {
    스트럭트* 백업파일노드 넥스트
    파일의 절대경로
    백업 주기
    백업 옵션 (mntd)
    담당쓰레드 정보
}

*/

/*
 * 절대경로로 디렉토리가 주어지면, 모든 파일 리스트를 뱉음. (재귀적으로)
 */

void twae(const char *absolute_dir)
{
    static char current_dir[512] = "";
    static char *ptr;
    if (strlen (current_dir) == 0) {
        // 경로 초기화 작업
        strncpy(current_dir, absolute_dir, 512);
        ptr = strlen(current_dir);
    }
    DIR *dirp;
    struct dirent *dirent;

    if ((dirp = opendir(current_dir) == NULL)) {
        fprintf(stderr, "디렉토리를 열 수 없습니다. -> %s\n",current_dir);
        return;
    }

    while ( (dirent = readdir(dirp) ) != NULL ) {
        printf("[%s]\n", dirent->d_name);
    }
    return;
}