#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>


sem_t a_r;

sem_t smoker_semaphor[3];

char* s_t[3] = { "matches & tobacco", "matches & paper", "tobacco & paper" };


bool items_on_table[3] = { false, false, false };

sem_t p_s[3];


void* smoker(void* arg)
{
	int s_i = *(int*) arg;
	int type_id   = s_i % 3;

	
	for (int i = 0; i < 3; ++i)
	{
		printf("\033[0;37mSmoker %d \033[0;31m>>\033[0m Waiting for %s\n",
			s_i, s_t[type_id]);

		
		sem_wait(&smoker_semaphor[type_id]);

		
		printf("\033[0;37mSmoker %d \033[0;32m<<\033[0m Now making the a cigarette\n", s_i);
		usleep(rand() % 50000);
		sem_post(&a_r);

		
		printf("\033[0;37mSmoker %d \033[0;37m--\033[0m Now smoking\n", s_i);
		usleep(rand() % 50000);
	}

	return NULL;
}


sem_t pusher_lock;


void* pusher(void* arg)
{
	int pusher_id = *(int*) arg;

	for (int i = 0; i < 12; ++i)
	{
		
		sem_wait(&p_s[pusher_id]);
		sem_wait(&pusher_lock);

		
		if (items_on_table[(pusher_id + 1) % 3])
		{
			items_on_table[(pusher_id + 1) % 3] = false;
			sem_post(&smoker_semaphor[(pusher_id + 2) % 3]);
		}
		else if (items_on_table[(pusher_id + 2) % 3])
		{
			items_on_table[(pusher_id + 2) % 3] = false;
			sem_post(&smoker_semaphor[(pusher_id + 1) % 3]);
		}
		else
		{
			
			items_on_table[pusher_id] = true;
		}

		sem_post(&pusher_lock);
	}

	return NULL;
}

void* agent(void* arg)
{
	int agent_id = *(int*) arg;

	for (int i = 0; i < 6; ++i)
	{
		usleep(rand() % 200000);

		
		sem_wait(&a_r);

		
		sem_post(&p_s[agent_id]);
		sem_post(&p_s[(agent_id + 1) % 3]);

		
		printf("\033[0;35m==> \033[0;33mAgent %d giving out %s\033[0;0m\n",
			agent_id, s_t[(agent_id + 2) % 3]);
	}

	return NULL;
}


int main(int argc, char* arvg[])
{
	
	srand(time(NULL));

	sem_init(&a_r, 0, 1);

	sem_init(&pusher_lock, 0, 1);

	for (int i = 0; i < 3; ++i)
	{
		sem_init(&smoker_semaphor[i], 0, 0);
		sem_init(&p_s[i], 0, 0);
	}



	int s_is[6];

	pthread_t s_r[6];

	for (int i = 0; i < 6; ++i)
	{
		s_is[i] = i;

		if (pthread_create(&s_r[i], NULL, smoker, &s_is[i]) == EAGAIN)
		{
			perror("Insufficient resources to create thread");
			return 0;
		}
	}

	int pusher_ids[6];

	pthread_t p_r[6];

	for (int i = 0; i < 3; ++i)
	{
		pusher_ids[i] = i;

		if (pthread_create(&p_r[i], NULL, pusher, &pusher_ids[i]) == EAGAIN)
		{
			perror("Insufficient resources to create thread");
			return 0;
		}
	}

	int a_i[6];

	pthread_t a_t[6];

	for (int i = 0; i < 3; ++i)
	{
		a_i[i] =i;

		if (pthread_create(&a_t[i], NULL, agent, &a_i[i]) == EAGAIN)
		{
			perror("Insufficient resources to create thread");
			return 0;
		}
	}

	for (int i = 0; i < 6; ++i)
	{
		pthread_join(s_r[i], NULL);
	}

	return 0;
}