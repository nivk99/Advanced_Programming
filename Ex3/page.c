//  @file makefile
// AUTHORS: Niv Kotek - 208236315


//https://github.com/yxu1183/Page-Faults
//https://medium.com/%E6%88%91%E7%9A%84learning-note/os-hw9-34-51c876e6a8e9

//Header Files
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include<time.h> 

#define MAX_LINE 1024

int array[MAX_LINE / 2];
int page_faults = 0;


void printfunction(int a[], int frame_number)
{
	int t;
	for(t=0;t<frame_number;t++)
	{
		printf("%d ", a[t]);
	}
	printf("\n");
}

//Initializes an int array with 9999
//Parameters - working ize frame
//Returns - Nothing
void initialize_check(int working_set_size)
{
  page_faults = 0;
  int j = 0;
  for (j = 0; j < working_set_size; j++)
  {
    array[j] = 9999;
  }
}

//Checks if data is in the array for FIFO,Optimal,LRU
//Parameters: data, working size frame
//Returns: 1 if data is present
int check_present(int check, int working_set_size)
{
  int present = 0;
  int k = 0;
  while (k < working_set_size)
  {
    if (array[k] != check)
    {
    }
    else
    {
      present = 1;
      break;
    }
    k++;
  }
  return present;
}



//Implements First In First Out page fault algorithm
//Paramters: array of page requests, working size frame, length of the array
//Returns: Page faults in FIFO
int FIFO_pagefault(int pages[], int working_set_size, int length)
{
  //Initialize the array
  initialize_check(working_set_size);
  int i, j = 0;
  for (i = 0; i < length; i++)
  {
    //Finds and executes only if there is a page fault
    if (check_present(pages[i], working_set_size) == 0)
    {
      for (j = 0; j < working_set_size - 1; j++)
      {
        array[j] = array[j + 1];
      }
      array[j] = pages[i];
      page_faults = page_faults + 1;
    }
  }
  return page_faults;
}

//Implements Optimal page fault algorithm
//Parameters: array of page requests, working size frame, length of the array
//Returns: Page Faults in Optimal
int optimal_pagefault(int pages[], int working_set_size, int length)
{
  int i, j, k = 0;
  int close[MAX_LINE / 2];
  //Initialize the array
  initialize_check(working_set_size);
  while (k < length)
  {
    //Finds and executes only if there is a page fault
    if (check_present(pages[k], working_set_size) == 0)
    {
      for (i = 0; i < working_set_size; i++)
      {
        int find = 0;
        int page = array[i];
        j = k;
        while (j < length)
        {
          if (page == pages[j])
          {
            find = 1;
            close[i] = j;
            break;
          }
          else
          {
            find = 0;
          }
          j++;
        }
        if (!find)
        {
          close[i] = 9999;
        }
      }
      int maximum = -9999;
      int repeated;
      i = 0;
      while (i < working_set_size)
      {
        if (maximum < close[i])
        {
          repeated = i;
          maximum = close[i];
        }
        i++;
      }
      array[repeated] = pages[k];
      page_faults = page_faults + 1;
    }
    k++;
  }
  return page_faults;
}

//Implements Least Recently Found page fault algorithm
//Parameters: array of page requests, working size frame, length of the array
//Returns: Page Faults in LRU
int LRU_pagefault(int pages[], int working_set_size, int length)
{
  int i, j, k = 0;
  int close[MAX_LINE / 2];
  //Initialize the array
  initialize_check(working_set_size);
  while (k < length)
  {
    //Finds and executes only if there is a page fault
    if (check_present(pages[k], working_set_size) == 0)
    {
      for (i = 0; i < working_set_size; i++)
      {
        int find = 0;
        int page = array[i];
        j = k - 1;
        while (j >= 0)
        {
          if (page == pages[j])
          {
            find = 1;
            close[i] = j;
            break;
          }
          else
          {
            find = 0;
          }
          j--;
        }
        if (!find)
        {
          close[i] = -9999;
        }
      }
      int least = 9999;
      int repeated;
      i = 0;
      while (i < working_set_size)
      {
        if (close[i] < least)
        {
          repeated = i;
          least = close[i];
        }
        i++;
      }
      array[repeated] = pages[k];
      page_faults = page_faults + 1;
    }
    k++;
  }
  return page_faults;
}


int main(int argc, char *argv[])
{
      int page_string[12];
      int length=12;
      srand(time(NULL));
      int i;
      for(i=0;i<12;i++)
      {
        page_string[i]=rand()%10;	//int random;
      }
      printf("page reference string is :\n");
      printfunction(page_string, 12);
      int page_frame;
      printf("please enter page frame : ");
      scanf("%d", &page_frame);
      printf("\n");

      //Prints the coressponding page faults in correspnding algorithm
      printf("Page faults of FIFO: %5d\n", FIFO_pagefault(page_string, page_frame, length));
      printf("Page faults of LRU: %6d\n", LRU_pagefault(page_string, page_frame, length));
      printf("Page faults of Optimal: %2d\n", optimal_pagefault(page_string, page_frame, length));
      printf("\n");
  return 0;
}
