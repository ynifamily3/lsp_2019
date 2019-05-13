#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <string.h>

struct backups_head {

};

struct backup_file_node {
    struct backup_file_node *next;
    char pathname[256];
    
    int period;
    int option_m; // 입력받은 PERIOD마다 파일의 mtime이 수정 되었을 경우 백업 실행

    int option_n; // -n NUMBER : NUMBER는 백업한 파일의 최대 개수. 가장 최근 NUMBER개의 백업파일 외 나머지 파일은 삭제
    int maxn;
    
    int option_t; // -t TIME : 백업해야할 파일에 대한 백업 디렉토리 내 보관 기간을 TIME만큼 설정
    int store_time; // 보관 기간
    // int option_d; // -d : 지정한 디렉토리 내의 모든 파일들을 백업 리스트에 추가
    pthread_t backup_thread;
};

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
    static char pathname[512] = "";
    struct dirent *dirp;
    struct stat statbuf;
    char *ptr;
    DIR *dp;
    if (absolute_dir != NULL)
        strncpy(pathname, absolute_dir, 512);
    
    if (strlen(pathname) > 255) {
        fprintf(stderr, "path length is too long !\n");
        return;
    }
    
    if (lstat(pathname, &statbuf) < 0) {
        fprintf(stderr, "lstat error for %s\n", pathname);
        return;
    }

    if (S_ISREG(statbuf.st_mode)) {
        // 일반 FILE
        printf("%s\n", pathname);
        return;
    }
    else if (!S_ISDIR(statbuf.st_mode))
        return;
    
    // 디렉토리
    ptr = pathname + strlen(pathname);
    *ptr++ = '/';
    *ptr = '\0';

    if ((dp = opendir(pathname)) == NULL) {
        fprintf(stderr, "opendir error for %s\n", pathname);
        return;
    }

    while ((dirp = readdir(dp)) != NULL)
        if (strcmp(dirp->d_name, ".") && strcmp(dirp->d_name, "..")) {
            strncpy(ptr, dirp->d_name, 512);
            twae(NULL);
        }
    ptr[-1] = 0;
    closedir(dp);
    return;
}