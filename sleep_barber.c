#include "synch.h"
#include "minithread.h"
#include <stdio.h>
#include "mtio.h"

#define P(s) semaphore_P(s)
#define V(s) semaphore_V(s)


#define MAX_CHAIRS 2

#define MAX_CUSTOMERS 20

#define TRUE 1
#define FALSE 0
#define NULL 0

long random();

semaphore_t mutex;

semaphore_t empty;
semaphore_t full;

semaphore_t done;

int customers_waiting;
int customer[MAX_CHAIRS];
int barber_index, customer_index;

void god_func(), barber_func(), customer_func();

main()
{
  srandom(1693);

  god_func();
}

void god_func()
{
  int i, num_customers;
  minithread_t mt;

  customers_waiting = -1;

  barber_index = customer_index = 0;

  mutex = semaphore_create();
  empty = semaphore_create();
  full = semaphore_create();
  done  = semaphore_create();

  semaphore_initialize(mutex, 1);     

  semaphore_initialize(empty, MAX_CHAIRS);  

  semaphore_initialize(full, 0);
  semaphore_initialize(done, 0);

  mt = minithread_fork((proc_t) barber_func, (arg_t) NULL);
  printf("barber = %p\n",mt);

  num_customers = random() % MAX_CUSTOMERS;

  printf("And god created %d customers.\n", num_customers);
  printf("There are/is %d chair(s) in the customer waiting room.\n", MAX_CHAIRS-1);

  for(i = 0; i < num_customers; i++) {
    mt = minithread_fork((proc_t) customer_func, (arg_t) i);
    printf("customer %d = %p\n",i,mt);
    printf("Forking customer %d..\n",i);
    fflush(stdout);
    
    random_stall(6);

   }

  printf("leaving GOD FUNC\n");
  fflush(stdout);
  
  _exit(0);
}

void barber_func()
{
    printf("BARBER STARTED\n");
    fflush(stdout);
    
  while(TRUE) {
    P(full);
    P(mutex);
    printf("Barbering customer %d.\n", customer[barber_index % MAX_CHAIRS]);
    fflush(stdout);
    V(mutex);
    barber_index++;
    V(empty);
    
    random_stall(7);

    
    V(done);
  }
}

void customer_func(i)
     int i;
{
  printf("Customer %d begin. TID = %p\n", i, minithread_self());
  fflush(stdout);
  

  if (customers_waiting == MAX_CHAIRS - 1)
    printf("** Customer %d leaving.\n", i);
  else {
    customers_waiting++;
    printf("Customer %d waiting. Total waiting: %d\n", i, customers_waiting);
    P(empty);
    P(mutex);
    customer[customer_index % MAX_CHAIRS] = i;
    printf("In barber seat...%i \n",i);
    V(mutex);
    V(full);
    customer_index++;
    
    P(done);
    printf("A satisfied Customer %i leaving...\n",i);
    
    customers_waiting--;  
  }
}



random_stall(time)
     int time;
{
  int i;
  int length = random() % time;

  for(i = 0; i <= length; i++)
    minithread_yield();
}