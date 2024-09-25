#include "api_fsmUtility.h"

#define MAX_FSM_STATES  MAX_FSM_STAGES

typedef enum
{
  STAGE_1,
  STAGE_2,
  STAGE_3,
  STAGE_4,
  STAGE_5,
  STAGE_6,
  STAGE_7,

/* Add more Stages in the FSM, as per application, above this */
  MAX_FSM_STAGES
}e_fsmStates;

int main(void)
{

  printf("\nHello world!");
  return 0;
}


void myStateFunc1(void)
{
  
}