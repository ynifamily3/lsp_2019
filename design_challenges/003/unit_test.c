#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct test_struct {
    int front, rear;
    int limit; // n옵션의 인자가 된다. (최대 100의 값이 들어온다고 가정)
    int number_of_nodes;
    char *queue[101];

};

void testinit(struct test_struct *test, int arg)
{
    test->number_of_nodes = 0;
    test->front = test->rear = 0;
    test->limit = arg + 1;
}

void testfunc(struct test_struct *test, const char *data)
{
    if ((test->rear+1) % test->limit == test->front) {
        fprintf(stderr, "backuped_file fully added!!\n");
        return;
    }
    ++test->number_of_nodes;
    ++test->rear;
    test->rear %= test->limit;
    printf("%d\n", test->rear);
    test->queue[test->rear] = (char *)calloc(1, strlen(data) + 1);
}

void *exec_thr(void *args)
{
   struct test_struct *test;
    test = (struct test_struct *) calloc(1, sizeof(struct test_struct));
    testinit(test, 5);
    testfunc(test, "backback");
    sleep(1);
    testfunc(test, "backback");
    testfunc(test, "backback");
    testfunc(test, "backback");
    testfunc(test, "backback");
    testfunc(test, "backback");
}

int main(void)
{
    pthread_t a;
    pthread_create(&a, NULL, exec_thr, NULL);
    pthread_join(a, NULL);
    return 0;
}