// 성균관대학교 인공지능융합전공 (프랑스어문학과) 조준범

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int id;
	int arriveTime;
	int burstTime;
	int remainTime;
	int finishTime;
} Process;

typedef struct {
	Process* processes[200];
	int start, fin;
} ReadyQueue;

void loadFile();
void enqueue(ReadyQueue*, Process*);
Process* dequeue(ReadyQueue*);
int getQueueSize(ReadyQueue*);
void showQueue(ReadyQueue*);
void schedule();
void schedule_RR(ReadyQueue*, Process*, int);
void getResult();
void ganttChart();

ReadyQueue q0, q1, q2;
Process process[100];
int time;
int processCount;

int ganttCount = 0;
int gantt[200] = { -1 };

void main() {
	q0.start = q1.start = q2.start = 0;
	q0.fin = q1.fin = q2.fin = 0;

	loadFile();
	printf("\n\n-----도착시간을 기준으로 재설정한 Q0 큐-----\n");
	showQueue(&q0);
	schedule();
	getResult();
}

void enqueue(ReadyQueue* q, Process* p) {
	q->processes[q->fin] = p;
	q->fin += 1;

	if (q == &q2) {
		Process* temp;
		for (int i = q->fin - 1; i > q->start; i--) {
			if (q->processes[i]->remainTime < q->processes[i - 1]->remainTime) {
				temp = q->processes[i - 1];
				q->processes[i - 1] = q->processes[i];
				q->processes[i] = temp;
			}
		}
	}
}

Process* dequeue(ReadyQueue* q) {
	q->start++;
	return q->processes[q->start - 1];
}

int getQueueSize(ReadyQueue* q) {
	int count = q->fin - q->start;
	return count;
}

void showQueue(ReadyQueue* q) {
	int count = q->fin - q->start;
	printf("할당된 프로세스 개수 : %d\n", count);

	for (int i = q->start; i < q->fin; i++) {
		printf("%d번 프로세스 -> ", q->processes[i]->id);
		printf("도착시간 : %d / 버스트시간 : %d / 남은시간 : %d\n", q->processes[i]->arriveTime, q->processes[i]->burstTime, q->processes[i]->remainTime);
	}
}

void loadFile() {
	FILE* file = fopen("input.txt", "r");
	fscanf(file, "%d", &processCount);

	for (int i = 0; i < processCount; i++) {
		process[i].id = i;
		fscanf(file, "%d %d", &process[i].arriveTime, &process[i].burstTime);
		process[i].remainTime = process[i].burstTime;
		process[i].finishTime = 0;
	}

	printf("-----input.txt-----\n");
	for (int i = 0; i < processCount; i++) {
		printf("%d번 프로세스 -> ", process[i].id);
		printf("도착시간 : %d / 버스트시간 : %d\n", process[i].arriveTime, process[i].burstTime);
	}

	Process temp;
	for (int i = processCount - 1; i >= 1; i--) {
		for (int k = i - 1; k >= 0; k--) {
			if (process[i].arriveTime < process[k].arriveTime) {
				temp = process[k];
				process[k] = process[i];
				process[i] = temp;
			}
		}
	}

	for (int i = 0; i < processCount; i++) {
		enqueue(&q0, &process[i]);
	}
}

void schedule() {
	Process* current;
	time = 0;

	printf("\n\n-----스케쥴링 시작-----\n");
	while (1) {
		if ((getQueueSize(&q0) != 0) && (q0.processes[q0.start]->arriveTime <= time)) {
			current = dequeue(&q0);
			schedule_RR(&q1, current, 2);
		}
		else if (getQueueSize(&q1) != 0) {
			current = dequeue(&q1);
			schedule_RR(&q2, current, 4);
		}
		else if (getQueueSize(&q2) != 0) {
			current = dequeue(&q2);
			printf("%d | 프로세스 %d Q2에서 스케쥴되어 실행 중..\n", time, current->id);
			time += current->remainTime;
			printf("%d | 프로세스 %d 완료!\n", time, current->id);
			current->finishTime = time;

			for (int i = ganttCount; i <= ganttCount + current->remainTime - 1; i++) {
				gantt[i] = current->id;
			}
			ganttCount += current->remainTime;
		}
		else if ((getQueueSize(&q0) != 0) && (q0.processes[q0.start]->arriveTime > time)) {
			printf("%d | CPU 대기 중..\n", time);
			gantt[ganttCount] = -1;
			ganttCount++;
			time++;
		}
		else {
			printf("프로세스가 존재하지 않습니다.\n");
			break;
		}
	}
}

void schedule_RR(ReadyQueue* nextQueue, Process* current, int quantum) {
	if (nextQueue == &q1) printf("%d | 프로세스 %d Q0에서 스케쥴되어 실행 중..\n", time, current->id);
	else printf("%d | 프로세스 %d Q1에서 스케쥴되어 실행 중..\n", time, current->id);

	if (current->remainTime > quantum) {
		time += quantum;
		current->remainTime -= quantum;
		enqueue(nextQueue, current);

		for (int i = ganttCount; i <= ganttCount + quantum - 1; i++) {
			gantt[i] = current->id;
		}
		ganttCount += quantum;
	}
	else {
		time += current->remainTime;
		printf("%d | 프로세스 %d 완료!\n", time, current->id);
		current->finishTime = time;

		for (int i = ganttCount; i <= ganttCount + current->remainTime - 1; i++) {
			gantt[i] = current->id;
		}
		ganttCount += current->remainTime;
	}
}

void getResult() {
	Process temp;
	for (int i = 1; i < processCount; i++) {
		for (int k = 0; k < i; k++) {
			if (process[i].id < process[k].id) {
				temp = process[k];
				process[k] = process[i];
				process[i] = temp;
			}
		}
	}

	printf("\n-----결과 출력-----\n");
	int wholeTurnTime = 0, wholeWaitTime = 0;
	int turnTime, waitTime;

	for (int i = 0; i < processCount; i++) {
		turnTime = process[i].finishTime - process[i].arriveTime;
		waitTime = turnTime - process[i].burstTime;

		wholeTurnTime += turnTime;
		wholeWaitTime += waitTime;

		printf("프로세스 %d | TurnAroundTime : %d, WaitTime : %d\n", process[i].id, turnTime, waitTime);
	}

	printf("\n전체 TurnAroundTime : %d, 전체 WaitTime : %d", wholeTurnTime, wholeWaitTime);
	printf("\n평균 TurnAroundTime : %.2f, 평균 WaitTime : %.2f", ((double)wholeTurnTime / processCount), ((double)wholeWaitTime / processCount));

	ganttChart();
}

void ganttChart() {
	printf("\n\n-----Gantt Chart-----\n");
	printf("FinishTime : %d\n", ganttCount);

	for (int i = 0; i < processCount; i++) {
		printf("P%-2d | ", i);
		for (int k = 0; k < ganttCount; k++) {
			if (i == gantt[k]) printf("■");
			else printf(" ");
		}
		printf("\n");
	}

	printf("Time| 0");
	for (int i = 1; i < ganttCount; i++) {
		if (i % 5 == 0) printf("%5d", i);
	}
	printf("\n");
}