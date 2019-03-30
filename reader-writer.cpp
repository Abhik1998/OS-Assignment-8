#include<Semaphores.h>

#include<pthread.h>

#include<stdio.h>

    int rc = 0, wc = 0, val;

    pthread_mutex_t mutex1, mwrite, mread, rallow;

    pthread_t tr1, tr2, tl1, tl2;

    pthread_attr_t tr1attr, tr2attr, tw1attr, tw2attr;

    void *writer();

    void *reader();

    int main()

    {

        pthread_mutex_init(&mwrite, NULL);

        pthread_mutex_init(&mread, NULL);

        pthread_mutex_init(&rallow, NULL);

        pthread_mutex_init(&mutex1, NULL);

        pthread_attr_init(&tl1attr);

        pthread_attr_init(&tr1attr);

        pthread_attr_init(&tr2attr);

        pthread_attr_init(&tl2attr);

        printf("\n Writer 1 created");

        pthread_create(&tl1, &tl1attr, writer, NULL);

        printf("\n Reader 1 created");

        pthread_create(&tr1, &tr1attr, reader, NULL);

        printf("\n Reader 2 created");

        pthread_create(&tr2, &tr2attr, reader, NULL);

        printf("\n WRITER 2 created");

        pthread_create(&tl2, &tl2attr, writer, NULL);

        pthread_join(tl1, NULL);

        pthread_join(tr1, NULL);

        pthread_join(tr2, NULL);

        pthread_join(tl2, NULL);

        return 0;
    }

    void *writer()

    {

        pthread_mutex_lock(&mwrite);

        wc++;

        if (wc == 1)

            pthread_mutex_lock(&rallow);

        pthread_mutex_unlock(&mwrite);

        pthread_mutex_lock(&mutex1);

        printf("\n Enter data in writer %d", wc);

        scanf("%d", &val);

        pthread_mutex_unlock(&mutex1);

        pthread_mutex_lock(&mwrite);

        wc--;

        if (wc == 0)

            pthread_mutex_unlock(&rallow);

        pthread_mutex_unlock(&mwrite);

        pthread_exit(0);
    }

    void *reader()

    {

        pthread_mutex_lock(&rallow);

        pthread_mutex_lock(&mread);

        rc++;

        if (rc == 1)

            pthread_mutex_lock(&mutex1);

        pthread_mutex_unlock(&mread);

        pthread_mutex_unlock(&rallow);

        printf("\n reader %d read data: %d", rc, val);

        pthread_mutex_lock(&mread);

        rc--;

        if (rc == 0)

            pthread_mutex_unlock(&mutex1);

        pthread_mutex_unlock(&mread);

        pthread_exit(0);
    }
