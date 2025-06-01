#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

int numProcess, numResource;
int resource[MAX] = { 0 };
int remainResource[MAX] = { 0 };
int maxClaim[MAX][MAX] = { 0 };
int allocation[MAX][MAX] = { 0 };
int need[MAX][MAX] = { 0 };

int safeSequence[MAX] = { 0 };

void loadFile();
void printData();
int getError();
void getOptimalProcess();
int circulate(int[], int, int[]);

void main() {
	loadFile();
	printData();
	if (!getError()) getOptimalProcess();
}

void loadFile() {
	FILE* file = fopen("input.txt", "r");
	fscanf(file, "%d %d", &numProcess, &numResource);

	for (int i = 0; i < numResource; i++) {
		fscanf(file, "%d", &resource[i]);
		remainResource[i] = resource[i];
	}

	for (int i = 0; i < numProcess; i++) {
		for (int j = 0; j < numResource; j++) {
			fscanf(file, "%d", &maxClaim[i][j]);
		}
	}

	for (int i = 0; i < numProcess; i++) {
		for (int j = 0; j < numResource; j++) {
			fscanf(file, "%d", &allocation[i][j]);
			need[i][j] = maxClaim[i][j] - allocation[i][j];
			remainResource[j] -= allocation[i][j];
		}
	}

	fclose(file);
}

void printData() {
	printf("[초기 상태]\n\n");

	printf("총 프로세스 수 : %d\n총 자원 수 : %d\n\n", numProcess, numResource);

	printf("각 자원별 유닛 수 : \n");
	for (int i = 0; i < numResource; i++) {
		printf("\t%d번 자원 : %d개\n", i + 1, resource[i]);
	}

	printf("\n각 프로세스별 최대 자원 할당 요구량 : \n");
	for (int i = 0; i < numProcess; i++) {
		printf("\t%d번 프로세스 :", i + 1);
		for (int j = 0; j < numResource; j++) {
			printf(" %d개 |", maxClaim[i][j]);
		}
		printf("\n");
	}

	printf("\n각 프로세스별 할당된 자원 개수 : \n");
	for (int i = 0; i < numProcess; i++) {
		printf("\t%d번 프로세스 :", i + 1);
		for (int j = 0; j < numResource; j++) {
			printf(" %d개 |", allocation[i][j]);
		}
		printf("\n");
	}

	printf("\n각 프로세스별 필요한 자원 개수 : \n");
	for (int i = 0; i < numProcess; i++) {
		printf("\t%d번 프로세스 :", i + 1);
		for (int j = 0; j < numResource; j++) {
			printf(" %d개 |", need[i][j]);
		}
		printf("\n");
	}

	printf("\n각 자원별 남은 유닛 수 : \n");
	for (int i = 0; i < numResource; i++) {
		printf("\t%d번 자원 : %d개\n", i + 1, remainResource[i]);
	}
}

int getError() {
	int err = 0;

	for (int i = 0; i < numProcess; i++) {
		for (int j = 0; j < numResource; j++) {
			// 1. 프로세스가 선언한 최대 자원 요구량보다 더 많은 자원을 할당받은 경우
			if (need[i][j] < 0) {
				err = 1;
				printf("\n오류!\n%d번 프로세스의 %d번 자원에 대한 현재 할당량이 요구량을 초과함\n", i + 1, j + 1);
			}

			// 2. 프로세스가 요구하는 자원량이 시스템이 보유한 최대 자원량보다 많은 경우
			if (maxClaim[i][j] > resource[j]) {
				err = 1;
				printf("\n오류!\n%d번 프로세스의 %d번 자원에 대한 최대 요구량이 시스템 자원 총량을 초과함\n", i + 1,  j + 1);
			}
		}
	}

	// 3. 특정 자원의 현재 할당량이 실제 보유량보다 많은 경우
	for (int j = 0; j < numResource; j++) {
		if (remainResource[j] < 0) {
			err = 1;
			printf("\n오류!\n%d번 자원의 할당량이 실제 보유량을 초과함\n", j + 1);
		}
	}

	return err;
}

void getOptimalProcess() {
	int isSafe = 0;

	for (int i = 0; i < numProcess; i++) {
		int isPossible = 1;
		for (int j = 0; j < numResource; j++) {
			if (need[i][j] > remainResource[j]) isPossible = 0;
		}

		if (isPossible) {
			int progressP[MAX] = { 0 };
			progressP[0] = i;
			
			int tempRemain[MAX];
			memcpy(tempRemain, remainResource, sizeof(int) * numResource);

			safeSequence[0] = i;
			isSafe = circulate(progressP, 1, tempRemain);
		}

		if (isSafe) break;
	}

	printf("\nRESULT : ");
	if (isSafe) {
		printf("SAFE\n시퀀스 : ");
		for (int i = 0; i < numProcess; i++) {
			printf("%d", safeSequence[i] + 1);
			if (i != numProcess - 1) {
				printf(" -> ");
			}
		}
	}
	else printf("UNSAFE");

	printf("\n");
}

int circulate(int progressP[MAX], int level, int remain[MAX]) {
	if (level == numProcess) return 1;

	int finish[MAX] = { 0 };
	for (int i = 0; i < level; i++) finish[progressP[i]] = 1;

	for (int i = 0; i < numResource; i++) remain[i] += allocation[progressP[level-1]][i];

	for (int i = 0; i < numProcess; i++) {
		if (!finish[i]) {
			int isPossible = 1;
			for (int j = 0; j < numResource; j++) {
				if (need[i][j] > remain[j]) isPossible = 0;
			}

			if (isPossible) {
				progressP[level] = i;

				int tempRemain[MAX];
				memcpy(tempRemain, remain, sizeof(int) * numResource);

				safeSequence[level] = i;
				if (circulate(progressP, level + 1, tempRemain)) return 1;
			}
		}
	}

	return 0;
}
