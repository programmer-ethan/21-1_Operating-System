#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#define NEXT(index) ((index+1)%1000)

struct Buffer
{
	int buf[1000];
	int front, rear;
};

void *producer(void *buffer);
void* producer2(void* buffer);
void *consumer(void *buffer);
void* consumer2(void* buffer);
void* consumer3(void* buffer);
void InitBuffer(struct Buffer* buffer);
int IsFull(struct Buffer* buffer);
int IsEmpty(struct Buffer* buffer);
void ToBuffer(struct Buffer* buffer, int data);
int FromBuffer(struct Buffer* buffer);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_has_space = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_has_data = PTHREAD_COND_INITIALIZER;

int total = 0;
int count[100] = {0};

int main(int argc, char *argv[])
{
	int i;
	pthread_t thread_p[2];
	pthread_t thread_c[3];
	struct Buffer *buffer;
	buffer = (struct Buffer*)malloc(sizeof(struct Buffer));

	InitBuffer(buffer);

	pthread_create(&thread_p[0],NULL,producer, (void*)buffer);
	pthread_create(&thread_p[1],NULL,producer2, (void*)buffer);
	pthread_create(&thread_c[0],NULL,consumer, (void*)buffer);
	pthread_create(&thread_c[1],NULL,consumer2, (void*)buffer);
	pthread_create(&thread_c[2],NULL,consumer3, (void*)buffer);

	for(i=0;i<2;i++) pthread_join(thread_p[i],NULL);
	for(i=0;i<3;i++) pthread_join(thread_c[i],NULL);
	printf("\ntotal: %d\n\n", total);
	for (i = 0; i < 100; i++) printf("count[%d] : %d\n", i, count[i]);

	return 0;
}

void *producer (void *buffer){
	struct Buffer* p_buffer = (struct Buffer*)buffer;
	int i, data, num;
	printf("P1 starts!\n");

	while(1){
		pthread_mutex_lock(&mutex);
		if (IsFull(p_buffer)) {
			pthread_cond_wait(&buffer_has_space, &mutex);
		}
		else {
			if (p_buffer->buf[p_buffer->front]==-999) {//for exit after -999
				pthread_mutex_unlock(&mutex);
				break;
			}
			scanf("%d", &data);
			if (data == -999) {
				ToBuffer(p_buffer, data);
				usleep(1000);
				pthread_cond_signal(&buffer_has_data);
				pthread_mutex_unlock(&mutex);
				break;
			}
			else {
				ToBuffer(p_buffer, data);
				usleep(1000);
				pthread_cond_signal(&buffer_has_data);
			}
		}
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(0);
}
void* producer2(void* buffer) {
	struct Buffer* p_buffer = (struct Buffer*)buffer;
	int i, data, num;
	printf("P2 starts!\n");

	while (1) {
		pthread_mutex_lock(&mutex);
		if (IsFull(p_buffer)) {
			pthread_cond_wait(&buffer_has_space, &mutex);
		}
		else {
			if (p_buffer->buf[p_buffer->front] == -999) {
				pthread_mutex_unlock(&mutex);
				break;
			}
			scanf("%d", &data);
			if (data == -999) {
				ToBuffer(p_buffer, data);
				usleep(2000);
				pthread_cond_signal(&buffer_has_data);
				pthread_mutex_unlock(&mutex);
				break;
			}
			else {
				ToBuffer(p_buffer, data);
				usleep(1000);
				pthread_cond_signal(&buffer_has_data);
			}
		}
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(0);
}
void *consumer (void *buffer){
	struct Buffer* c_buffer = (struct Buffer*)buffer;
	int i, data, countidx, num;
	printf("C1 starts!\n");

	while(1){
		pthread_mutex_lock(&mutex);
		if (IsEmpty(c_buffer)) {
			pthread_cond_wait(&buffer_has_data, &mutex);
		}
		else {
			data = FromBuffer(c_buffer);
			if (data == -999) {//for exit after -999
				c_buffer->front = (c_buffer->front + 999) % 1000;//index -1 for remaining consumer thread to exit
				pthread_mutex_unlock(&mutex);
				break;
			}
			countidx = data / 10000;
			count[countidx]++;
			total += data;
			usleep(3000);
			pthread_cond_signal(&buffer_has_space);
		}
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(0);
}
void* consumer2(void* buffer) {
	struct Buffer* c_buffer = (struct Buffer*)buffer;
	int i, data, countidx, num;
	printf("C2 starts!\n");

	while (1) {
		pthread_mutex_lock(&mutex);
		if (IsEmpty(c_buffer)) {
			pthread_cond_wait(&buffer_has_data, &mutex);
		}
		else {
			data = FromBuffer(c_buffer);
			if (data == -999) {
				c_buffer->front = (c_buffer->front + 999) % 1000;
				pthread_mutex_unlock(&mutex);
				break;
			}
			countidx = data / 10000;
			count[countidx]++;
			total += data;
			usleep(4000);
			pthread_cond_signal(&buffer_has_space);
		}
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(0);
}
void* consumer3(void* buffer) {
	struct Buffer* c_buffer = (struct Buffer*)buffer;
	int i, data, countidx, num;
	printf("C3 starts!\n");

	while (1) {
		pthread_mutex_lock(&mutex);
		if (IsEmpty(c_buffer)) {
			pthread_cond_wait(&buffer_has_data, &mutex);
		}
		else {
			data = FromBuffer(c_buffer);
			if (data == -999) {
				c_buffer->front = (c_buffer->front + 999) % 1000;
				pthread_mutex_unlock(&mutex);
				break;
			}
			countidx = data / 10000;
			count[countidx]++;
			total += data;
			usleep(5000);
			pthread_cond_signal(&buffer_has_space);
		}
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(0);
}

void InitBuffer(struct Buffer *buffer) {
	buffer->front = buffer->rear = 0;
}

int IsFull(struct Buffer *buffer) {
	return NEXT(buffer->rear) == buffer->front;
}

int IsEmpty(struct Buffer *buffer) {
	return buffer->front == buffer->rear;
}

void ToBuffer(struct Buffer *buffer, int data){

	buffer->buf[buffer->rear]= data;
	buffer->rear = NEXT(buffer->rear);
}

int FromBuffer(struct Buffer *buffer)
{
	int re = 0;

	re = buffer->buf[buffer->front];
	buffer->front = NEXT(buffer->front);
	return re;
}