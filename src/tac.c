#include "tac.h"

// 全局变量
Tac *head = NULL; // 链表头部
Tac *tail = NULL; // 链表尾部

// 插入新的Tac结构体到链表中
Tac *insertTac(char* target, char* op, char* arg1, char* arg2) {
    // 创建新的Tac结构体
    Tac *newTac = (Tac *)malloc(sizeof(Tac));
    if (newTac == NULL) {
        // 内存分配失败
        return NULL;
    }

    // 复制字符串
    newTac->target = target ? strdup(target) : NULL;
    newTac->op = op ? strdup(op) : NULL;
    newTac->arg1 = arg1 ? strdup(arg1) : NULL;
    newTac->arg2 = arg2 ? strdup(arg2) : NULL;

    // 初始化链表指针
    newTac->before = NULL;
    newTac->next = NULL;

    // 插入到链表中
    if (tail == NULL) {
        // 链表为空
        head = tail = newTac;
    } else {
        // 链表不为空
        tail->next = newTac;
        newTac->before = tail;
        tail = newTac;
    }

    return newTac;
}

// 打印整个Tac链表
void printTacList(FILE *file) {
    Tac *current = head; // 假设head是您的链表头部全局变量

    while (current != NULL) {
        // 判断是打印到文件还是标准输出
        if (file != NULL) {
            fprintf(file, "%s %s %s %s\n",
                    current->target, current->op, current->arg1, current->arg2);
        } else {
            printf("%s %s %s %s\n",
                   current->target, current->op, current->arg1, current->arg2);
        }
        current = current->next;
    }
}

// 释放整个Tac链表
void freeTacList() {
    Tac *current = head; // 假设head是您的链表头部全局变量

    while (current != NULL) {
        Tac *temp = current;
        current = current->next;
    }
}
