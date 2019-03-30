#include<stdio.h>

#include<unistd.h>

#include<pthread.h>

#define buffSize 1000

struct

{

pthread_mutex_t mutex;

int buff[buffSize];

int pC;

int cC;

}shared={PTHREAD_MUTEX_INITIALIZER};

void *produce(void *arg);

void *consume(void *arg);

void main()

{

int pT,cT,i;

printf(“\nEnter the no of Producers: “);

scanf(“%d”,&pT);

printf(“\nEnter the no of Consumers: “);

scanf(“%d”,&cT);

int pdArr[pT],csArr[cT];

pthread_t pd[pT],cs[cT];

pthread_setconcurrency(pT+cT);

for(i=0;i<pT;i++)

{

pdArr[i]=0;

pthread_create(&pd[i],NULL,produce,&pdArr[i]);

}

for(i=0;i<cT;i++)

{

csArr[i]=0;

pthread_create(&cs[i],NULL,consume,&csArr[i]);

}

for(i=0;i<pT;i++)

{

pthread_join(pd[i],NULL);

printf(“\nThe Producer (%d) produced: [%d] Items”,i,pdArr[i]);

sleep(1);

}

printf(“\n”);

for(i=0;i<cT;i++)

{

pthread_join(cs[i],NULL);

printf(“\nThe Consumer (%d) Consumed: [%d] Items”,i,csArr[i]);

sleep(1);

}

}

void *produce(void *arg)

{

while(1)

{

pthread_mutex_lock(&shared.mutex);

if(shared.pC<buffSize)

{

shared.pC++;

*((int *)arg) += 1;

pthread_mutex_unlock(&shared.mutex);

}

else

{

pthread_mutex_unlock(&shared.mutex);

return NULL;

}

}

}

void *consume(void *arg)

{

while(1)

{

pthread_mutex_lock(&shared.mutex);

if(shared.cC<shared.pC)

{

shared.cC++;

*((int *)arg) += 1;

pthread_mutex_unlock(&shared.mutex);

}

else

{

pthread_mutex_unlock(&shared.mutex);

if (shared.cC>=buffSize)

return NULL;

}

}

}