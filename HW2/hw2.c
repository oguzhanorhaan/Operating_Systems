#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

// Thread A definitions
pthread_t tid[4];
int counter;

//Thread B &  C definitions
pthread_t threadB;
pthread_t threadC;

//Mutex controls for sync between threadA, threadB and threadC
pthread_mutex_t Lock_Type1A;
pthread_mutex_t Lock_Type1B;
pthread_mutex_t Lock_Type2;


char ch;
char filename1[]="numbers1.txt";
char filename2[]="numbers2.txt";
char filename3[]="resourceType2.txt";
int count=0;

int CheckIfEmptyR1=1;
int CheckIfEmptyR2=1;
int CheckIfEmptyR3=1;

void CutPaste(int arg);
void* ThreadA_Function();
int rand_int(int a,int b);
void* ThreadB_Function();
void RemoveNegatives(int arg);
void RemovePrimes(int arg);
void* ThreadC_Function();


void* ThreadA_Function()
{

    while(CheckIfEmptyR1 || CheckIfEmptyR2)
   { 
    if(pthread_mutex_trylock(&Lock_Type1A)==0)
    {
        if(pthread_mutex_trylock(&Lock_Type2)==0)
        {
          printf("Thread in critical section with 1A AND 2");
                 CutPaste(1);
        pthread_mutex_unlock(&Lock_Type2);
        pthread_mutex_unlock(&Lock_Type1A);
          printf("Thread exit from critical section with 1A AND 2");

                 }
        else{
         pthread_mutex_unlock(&Lock_Type1A);
            }
    }


    if(pthread_mutex_trylock(&Lock_Type1B)==0)
    {  
        if(pthread_mutex_trylock(&Lock_Type2)==0){
            printf("Thread in critical section with 1B AND 2\n");
            CutPaste(2);        
            pthread_mutex_unlock(&Lock_Type2);
            pthread_mutex_unlock(&Lock_Type1B);
               printf("Thread exit from critical section with 1B AND 2\n");

        }
        else
        {
        pthread_mutex_unlock(&Lock_Type1B);    
        }
    }

    if(pthread_mutex_trylock(&Lock_Type2)==0)
    {
      if(pthread_mutex_trylock(&Lock_Type1A)==0)
       {
          printf("Thread in critical section with 1A AND 2");
         CutPaste(1);
        pthread_mutex_unlock(&Lock_Type1A);
        pthread_mutex_unlock(&Lock_Type2);    
                  printf("Thread exit from critical section with 1A AND 2");

       }
      else if(pthread_mutex_trylock(&Lock_Type1B)==0)
       {
          printf("Thread in critical section with 1B AND 2");
         CutPaste(2);
        pthread_mutex_unlock(&Lock_Type1B);
        pthread_mutex_unlock(&Lock_Type2);   
                  printf("Thread exit from critical section with 1B AND 2");
 
       }
      else 
       {
        pthread_mutex_unlock(&Lock_Type2);             
       }
        
    }    
   }

    return NULL;
}


void CutPaste(int arg)
{
  FILE *pFileA;
  FILE *pFile2A;
  FILE *pReplicaA;

  time_t t;
  srand(time(NULL));
  int CutAmount;

  char buffer[256];

  int ChooseFile = arg;

  if(ChooseFile==1)
  {
    pFileA=fopen("numbers1.txt", "r");
  }
  else if(ChooseFile==2)
  {
    pFileA=fopen("numbers2.txt", "r");
  }
  else
  {
    printf("Error occured while reaching file.");
  }

  CutAmount = rand() % 10 + 1;

  pFile2A=fopen("resourceType2.txt", "a");
  //int CutAmount= rand_int(1,10);
  printf("Random: %d\n", CutAmount);

  if(pFileA==NULL) {
    perror("Error opening file.");
  }
  else {
    int i=0;

    for(i=0;i<CutAmount;i++)
      {
        if(fgets(buffer, 100, pFileA) != NULL) {
          fprintf(pFile2A, "%s",buffer);
        }
      }

    rewind(pFileA);
    pReplicaA = fopen("replica.txt", "w");

    int delete_line=0;
    int temp = 0;
    while(fgets(buffer, 100, pFileA) != NULL) {
      
      if(delete_line<CutAmount) {
          delete_line++;
      }
      else
      {
        fprintf(pReplicaA,"%s", buffer);
        temp++;
      }
    }
    if(temp == 0 ) {
      if(ChooseFile == 1) {
         CheckIfEmptyR1 = 0;
      }
      else{
        CheckIfEmptyR2 = 0;
      }
    }

  }


 fclose(pReplicaA);
fclose(pFileA);
fclose(pFile2A);

  if(ChooseFile==1) 
  {
    remove(filename1);
    rename("replica.txt", filename1);
    remove("replica.txt");
  }
  else if(ChooseFile==2)
  {
    remove(filename2);
    rename("replica.txt", filename2);
    remove("replica.txt");
  }  
  

}

int rand_int(int a,int b)
{
    //The basic function here is like (rand() % range)+ Lower_bound

    srand(time(0));
    if (a > b)
        return((rand() % (a-b+1)) + b);
    else if (b > a)
        return((rand() % (b-a+1)) + a);
    else
        return a;
}


void* ThreadB_Function()
{

 while(CheckIfEmptyR1 || CheckIfEmptyR2)
 {
  if(CheckIfEmptyR1==1 && pthread_mutex_trylock(&Lock_Type1A)==0)
  {
    printf("Threaed B in critical section!\n");
    RemoveNegatives(1);

        pthread_mutex_unlock(&Lock_Type1A);
                printf("Thread B exitting FROM 1A critical section!\n");

    
  }
   if(CheckIfEmptyR2==1 && pthread_mutex_trylock(&Lock_Type1B)==0)
  {
        printf("Threaed B in critical section!\n");
     RemoveNegatives(2);
        pthread_mutex_unlock(&Lock_Type1B);  
                printf("Thread B exitting  FROM 1B critical section!\n");

  }
   if( pthread_mutex_trylock(&Lock_Type2)==0)
  {
        printf("Threaed B in critical section!\n");

     RemoveNegatives(3);  
        pthread_mutex_unlock(&Lock_Type2); 
        printf("Thread B exitting FROM 2 critical section!\n");
  }
  

 }

}





// Removing Negative Numbers Function
void RemoveNegatives(int arg)
{
FILE *pFileC;
FILE *pFile2C;
FILE *pReplicaC;
int ChooseFile = arg;

if(ChooseFile==1)
{
pFileC=fopen("numbers1.txt", "r");
}
else if(ChooseFile==2)
{
pFileC=fopen("numbers2.txt", "r");
}
else if(ChooseFile==3)
{
pFileC=fopen("resourceType2.txt", "r");
}



if(pFileC==NULL) {
    perror("Error opening file.");
}
else {
  
  pReplicaC = fopen("replica1.txt", "w");

  int count =0;
  int i=0;

    char buffer[256];


  fscanf (pFileC, "%d", &i);    
  while (!feof (pFileC))
    {  
      fscanf (pFileC, "%d", &i);  
      if(i<=0)
        {
          count++;
        }
      else
      {
        fprintf(pReplicaC,"%d\n",i);
      }    
    }
}
  fclose(pReplicaC);
  fclose(pFileC);

//rename the file replica1.c to original name

if(ChooseFile==1)
{
remove(filename1);
rename("replica1.txt", filename1);
remove("replica1.txt");
}
if(ChooseFile==2)
{
remove(filename2);
rename("replica1.txt", filename2);
remove("replica1.txt");
}
if(ChooseFile==3)
{
remove(filename3);
rename("replica1.txt", filename3);
remove("replica1.txt");
}
    
}



void* ThreadC_Function()
{
   while(CheckIfEmptyR1 || CheckIfEmptyR2)
   {
   if( CheckIfEmptyR1==1 &&  pthread_mutex_trylock(&Lock_Type1A)==0)
   {
    printf("Threaed C in critical section!");
     RemovePrimes(1);
    pthread_mutex_unlock(&Lock_Type1A); 
            printf("Thread C exitting from 1A critical section!\n");

   }
    if(CheckIfEmptyR2==1 &&pthread_mutex_trylock(&Lock_Type1B)==0)
   {
    printf("Threaed C in critical section!");
      RemovePrimes(2);   
        pthread_mutex_unlock(&Lock_Type1B); 
                printf("Thread C exitting FROM 1B critical section!\n");

   }
    if(pthread_mutex_trylock(&Lock_Type2)==0)
   {
    printf("Threaed C in critical section!");
      RemovePrimes(3);   
    pthread_mutex_unlock(&Lock_Type2); 
            printf("Thread C exitting FROM 2critical section!\n");

   }
  

  }

}

// Removing Prime Numbers Function
void RemovePrimes(int arg)
{
FILE *pFileB;
FILE *pFile2B;
FILE *pReplicaB;

int ChooseFile = arg;

if(ChooseFile==1)
{
pFileB=fopen("numbers1.txt", "r");
}
if(ChooseFile==2)
{
pFileB=fopen("numbers2.txt", "r");
}
if(ChooseFile==3)
{
pFileB=fopen("resourceType2.txt", "r");
}

if(pFileB==NULL) {
    perror("Error opening file.");
}
else {
 pReplicaB = fopen("replica2.c", "w");

int count =0;
int i=0;
int  z, flag = 0;
 fscanf (pFileB, "%d", &i);    
  while (!feof (pFileB))
    {  
      fscanf (pFileB, "%d", &i);  
flag=0;
for(z=2; z<=i/2; ++z)
    {
        // condition for nonprime number
    
    if(i<0)
    {i=-1*i;}

        if(i%z==0)
        {
            flag=1;
            break;
        }
    }

    if (flag==0)
       {
//prime ise hiçbir şey yapma
}
    else
       {
fprintf(pReplicaB,"%d\n",i);
}
       
    }

}
 fclose(pReplicaB);
fclose(pFileB);

//rename the file replica2.c to original name
if(ChooseFile==1)
{
remove(filename1);
rename("replica2.txt", filename1);
remove("replica2.txt");
}
if(ChooseFile==2)
{
remove(filename2);
rename("replica2.txt", filename2);
remove("replica2.txt");
}
if(ChooseFile==3)
{
    remove(filename3);
    rename("replica2.txt", filename3);
    remove("replica2.txt");
  }
}



int main(void)
{
    int i = 0;
    int err;
    int err1;
    int err2;

    if (pthread_mutex_init(&Lock_Type1A, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
    if (pthread_mutex_init(&Lock_Type1B, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
    if (pthread_mutex_init(&Lock_Type2, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

  while(i < 4)
    {
        err = pthread_create(&(tid[i]), NULL, &ThreadA_Function, NULL);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        i++;
    }  

        err1 = pthread_create(&threadB, NULL, &ThreadB_Function, NULL);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        
        err2 = pthread_create(&threadC, NULL, &ThreadC_Function, NULL);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);
    pthread_join(tid[3], NULL);   
    pthread_join(threadB, NULL);
    pthread_join(threadC, NULL); 
    printf("The End.");
    pthread_mutex_destroy(&Lock_Type1A);
    pthread_mutex_destroy(&Lock_Type1B);
    pthread_mutex_destroy(&Lock_Type2);

    return 0;
}