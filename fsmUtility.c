/*************************************************************************
  DEPENDENT FILE DECLARATION
*************************************************************************/

#include "fsmUtility.h"

/*************************************************************************
  GLOBAL VARIABLE DECLARATION / DEFINITION
*************************************************************************/

s_fsmType Fsm;

/*************************************************************************
  MACRO DEFINITION
*************************************************************************/

/*************************************************************************
  STATIC FUNCTION DECLARATION
*************************************************************************/

/*
@brief
Sets the values of all the strcuture members to 0 or NULL.
Sets the value of 'isInitialized' flag to 1 after completing the above operation.

@details
This function is expected to be called before executing any operations on the main structure.
So that the further operations can proceed with determinate initial value of the structure members instead of undeterminate assumed value.

@input
No arguments (till date).
Modifies the main structure.

@output
No return values (till date).
*/
static void FsmUtilInitFsmStruct(void);

/*
@brief
Get the bit number, corresponding to the graph, for referring / accessing the Structure.

@details
Implement logic =
For any state 'm', the relation with the 'j'th state will be given by 'i', where 'i' is,
i = [ (j < m) ? (j) : (j - 1) ]

@input
With reference to the above mentioned logic, input paramaters are,
- m = the state under consideration,
- j = the state in question,
also, for storing the result,
- address of the variable that needs to contain the result of the function.

@output
- success / failure of the Function.
*/
static uint8_t FsmUtilGetBitNumber(uint8_t aFromStateNum_u8, uint8_t aToStateNum_u8, uint8_t * aReturnBitNumber_u8);

/*************************************************************************
  API FUNCTION DEFINITION OF FSM UTILITY FUNCTIONS
*************************************************************************/

uint8_t fsmUtilCreateFsm(uint8_t aNumberOfStates_u8, uint64_t aPermissibleStateTransition_u64[])
{
  uint8_t lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE;
  uint64_t lZero_u8 = 0U;
  Fsm.isInitialized_u8 = 0U;
  
  FsmUtilInitFsmStruct(); /* Initialize all the variables present in the Structure to avoid uninitialized unitended values */
  
  Fsm.currentFsmState_u8 = 0U; /* Always begins in the 1st State */
  
  if(aNumberOfStates_u8 != 0U) /* FSM with non-zero states is valid */
  {
    Fsm.numOfStates_u8 = aNumberOfStates_u8;
    if(aPermissibleStateTransition_u64 != NULL) /* Intialize with the given Permissible State transition diagram */
    {
      memcpy(&Fsm.permissibleStateTransition_u64, aPermissibleStateTransition_u64, NUM_STATE_TRANSITION_GRAPH);
    }
    else /* Intialize with Permissible State transition diagram with 0 i.e. no state transitions are allowed */
    {
      memcpy(&Fsm.permissibleStateTransition_u64, &lZero_u8, NUM_STATE_TRANSITION_GRAPH);
    }
    lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_SUCCESS;
  }
  else /* FSM with 0 states is invalid */
  {
    Fsm.numOfStates_u8 = 0U;
    /* Do nothing and let lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE */
  }

  return lReturnStatus_u8;
}

uint8_t fsmUtilSetStateFunction(uint8_t aStateNum_u8, void (* aFsmStateFunction)(void))
{
  uint8_t lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE;
  if(aStateNum_u8 < Fsm.numOfStates_u8) /* a valid state will always be within the limit in which the FSM was created. */
  {
    Fsm.stateFunction_u8[aStateNum_u8] = aFsmStateFunction; /* set the state function */
    lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_SUCCESS;         /* indicate successful operation */
  }
  else /* if the state is invalid then nothing needs to be done and error needs to be returned */
  {
    /* do nothing and let lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE */
  }
  return lReturnStatus_u8;
}

uint8_t fsmUtilRunStateFunction(uint8_t aStateNum_u8)
{
  uint8_t lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE;
  if(aStateNum_u8 < Fsm.numOfStates_u8) /* a Valid state always lies within the limit with which the FSM was initialized. */
  {
    (*Fsm.stateFunction_u8[aStateNum_u8])(); /* Execute the State function of a valid State of the FSM */
    lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_SUCCESS;
  }
  else /* invalid State indicates Failure of the operation */
  {
    /* do nothing and let lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE */
  }
  return lReturnStatus_u8;
}

uint8_t fsmUtilSetStateTransitionEvaluatorFunction(uint8_t aFromStateNum_u8, uint8_t aToStateNum_u8, void (* aFromToStateEvaluatorFn)(void))
{
  uint8_t lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE;
  uint8_t lBitNumber_u8 = 0U;
  uint8_t lPosition_u8 = 0U;
  
  if(
    /* valid state pairs are those that are within the initialized limits ;
    functionality can be provided only if function address is provided */
    (aFromStateNum_u8 < Fsm.numOfStates_u8) && (aToStateNum_u8 < Fsm.numOfStates_u8) && (aFromToStateEvaluatorFn != NULL) &&
    /* get the position, in the stateTransitionEvaluator_u8 array, for setting the required Function */
    (FsmUtilGetBitNumber(aFromStateNum_u8, aToStateNum_u8, &lBitNumber_u8) == FSM_UTIL_RETURNTYPE_SUCCESS)
    )
  {
    lPosition_u8 = (MAX_NUM_STATES - 1)*aFromStateNum_u8 + lBitNumber_u8;
    Fsm.stateTransitionEvaluator_u8[lPosition_u8] = aFromToStateEvaluatorFn;
    lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_SUCCESS;
  }
  else
  {
    /* Function will not work when invalid state pair and NULL pointer address is provided */
    /* do nothing and let lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE */
  }
  return lReturnStatus_u8;
}

uint8_t fsmUtilRunStateTransitionEvaluatorFunction(uint8_t aFromStateNum_u8, uint8_t aToStateNum_u8)
{
  uint8_t lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE;
  uint8_t lBitNumber_u8 = 0U;
  uint8_t lPosition_u8 = 0U;
  if(
    (aFromStateNum_u8 < Fsm.numOfStates_u8) && (aToStateNum_u8 < Fsm.numOfStates_u8) &&
    (FsmUtilGetBitNumber(aFromStateNum_u8, aToStateNum_u8, &lBitNumber_u8) == FSM_UTIL_RETURNTYPE_SUCCESS)
    ) /* a Valid state always lies within the limit with which the FSM was initialized. */
  {
    lPosition_u8 = (MAX_NUM_STATES - 1)*aFromStateNum_u8 + lBitNumber_u8;
    (*Fsm.stateTransitionEvaluator_u8[lPosition_u8])(); /* Execute the State function of a valid State of the FSM */
    lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_SUCCESS;
  }
  else /* invalid State indicates Failure of the operation */
  {
    /* do nothing and let lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE */
  }
  return lReturnStatus_u8;
}

uint8_t fsmUtilSetStateTransitionPermission(uint8_t aFromStateNum_u8, uint8_t aToStateNum_u8, uint8_t aPermissionStatus_u8)
{
  uint8_t lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE;
  uint8_t lBitNumber_u8 = 0U;     /* get the unscaled bit number which needs to be addressed */
  uint8_t lPosition_u8 = 0U;      /* get the position in the bitfield which needs to be addressed */
  uint8_t lArrPosition_u8 = 0U;   /* translate the bitfield position into the Array index */
  uint8_t lBitPosition_u8 = 0U;   /* translate the bitfield position into the Bit position  */

  if(
    (aFromStateNum_u8 < Fsm.numOfStates_u8) && (aToStateNum_u8 < Fsm.numOfStates_u8) &&                     /* valid state pairs are those that are within the initialized limits ; functionality can be provided only if variable address is provided */
    (FsmUtilGetBitNumber(aFromStateNum_u8, aToStateNum_u8, &lBitNumber_u8) == FSM_UTIL_RETURNTYPE_SUCCESS)  /* get the position, in the stateTransitionEvaluator_u8 array, for setting the required Function */
    )
  {
    lPosition_u8 = (MAX_NUM_STATES - 1)*aFromStateNum_u8 + lBitNumber_u8;
    lArrPosition_u8 = ((lPosition_u8/64U) - 1U);                      /* 64 because the structure array is of type uint64_t; (-1) for scaling correctly */
    lBitPosition_u8 = lPosition_u8%64U;
    
    if(aPermissionStatus_u8 == FSM_STATE_TRANSITION_GRANTED)
    {
      Fsm.permissibleStateTransition_u64[lArrPosition_u8] |= (1U << lBitPosition_u8);
      lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_SUCCESS;
    }
    else if(aPermissionStatus_u8 == FSM_STATE_TRANSITION_DENIED)
    {
      Fsm.permissibleStateTransition_u64[lArrPosition_u8] &= (~(1U << lBitPosition_u8));
      lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_SUCCESS;
    }
    else
    {
      /* any permission status, as function argument, other than the defined Macro values will be erroneous */
      /* do nothing and let lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE */
    }
  }
  else
  {
    /* Function will not work when invalid state pair and NULL pointer address is provided */
    /* do nothing and let lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE */
  }
  return lReturnStatus_u8;
}

uint8_t fsmUtilGetStateTransitionPermission(uint8_t aFromStateNum_u8, uint8_t aToStateNum_u8, uint8_t * aPermissionStatus_u8)
{
  uint8_t lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE;
  uint8_t lBitNumber_u8 = 0U;     /* get the unscaled bit number which needs to be addressed */
  uint8_t lPosition_u8 = 0U;      /* get the position in the bitfield which needs to be addressed */
  uint8_t lArrPosition_u8 = 0U;   /* translate the bitfield position into the Array index */
  uint8_t lBitPosition_u8 = 0U;   /* translate the bitfield position into the Bit position  */

  if(
    /* valid state pairs are those that are within the initialized limits ;
    functionality can be provided only if variable address is provided */
    (aFromStateNum_u8 < Fsm.numOfStates_u8) && (aToStateNum_u8 < Fsm.numOfStates_u8) && (aPermissionStatus_u8 != NULL) &&
    /* get the position, in the stateTransitionEvaluator_u8 array, for setting the required Function */
    (FsmUtilGetBitNumber(aFromStateNum_u8, aToStateNum_u8, &lBitNumber_u8) == FSM_UTIL_RETURNTYPE_SUCCESS)
    )
  {
    lPosition_u8 = (MAX_NUM_STATES - 1)*aFromStateNum_u8 + lBitNumber_u8;
    /* 64 because the structure array is of type uint64_t; (-1) for scaling correctly */
    lArrPosition_u8 = ((lPosition_u8/64U) - 1U);
    lBitPosition_u8 = lPosition_u8%64U;

    (*aPermissionStatus_u8) = ((Fsm.permissibleStateTransition_u64[lArrPosition_u8] & (1U << lBitPosition_u8)) >> lBitPosition_u8);
    lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_SUCCESS;
  }
  else
  {
    /* Function will not work when invalid state pair and NULL pointer address is provided */
    /* do nothing and let lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE */
  }
  return lReturnStatus_u8;
}

uint8_t fsmUtilSetStateEvaluatorReturnStatus(uint8_t aFromStateNum_u8, uint8_t aToStateNum_u8, uint8_t aRequiredReturnStatus_u8)
{
  uint8_t lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE;
  uint8_t lBitNumber_u8 = 0U;     /* get the unscaled bit number which needs to be addressed */
  uint8_t lPosition_u8 = 0U;      /* get the position in the bitfield which needs to be addressed */
  uint8_t lArrPosition_u8 = 0U;   /* translate the bitfield position into the Array index */
  uint8_t lBitPosition_u8 = 0U;   /* translate the bitfield position into the Bit position  */

  if(
    /* valid state pairs are those that are within the initialized limits ;
    functionality can be provided only if variable address is provided */
    (aFromStateNum_u8 < Fsm.numOfStates_u8) && (aToStateNum_u8 < Fsm.numOfStates_u8) &&
    /* get the position, in the stateTransitionEvaluator_u8 array, for setting the required Function */
    (FsmUtilGetBitNumber(aFromStateNum_u8, aToStateNum_u8, &lBitNumber_u8) == FSM_UTIL_RETURNTYPE_SUCCESS)
    )
  {
    lPosition_u8 = (MAX_NUM_STATES - 1)*aFromStateNum_u8 + lBitNumber_u8;
    /* 64 because the structure array is of type uint64_t; (-1) for scaling correctly */
    lArrPosition_u8 = ((lPosition_u8/64U) - 1U);
    lBitPosition_u8 = lPosition_u8%64U;

    if(aRequiredReturnStatus_u8 == FSM_STATE_EVALUATION_CRITERIA_PASS)
    {
      Fsm.stateEvaluatorReturnStatus_u64[lArrPosition_u8] |= (1U << lBitPosition_u8);
      lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_SUCCESS;
    }
    else if(aRequiredReturnStatus_u8 == FSM_STATE_EVALUATION_CRITERIA_FAIL)
    {
      Fsm.stateEvaluatorReturnStatus_u64[lArrPosition_u8] &= (~(1U << lBitPosition_u8));
      lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_SUCCESS;
    }
    else
    {
      /* any permission status, as function argument, other than the defined Macro values will be erroneous */
      /* do nothing and let lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE */
    }
  }
  else
  {
    /* Function will not work when invalid state pair and NULL pointer address is provided */
    /* do nothing and let lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE */
  }
  return lReturnStatus_u8;
}

uint8_t fsmUtilGetStateEvaluatorReturnStatus(uint8_t aFromStateNum_u8, uint8_t aToStateNum_u8, uint8_t * aRequiredReturnStatus_u8)
{
  uint8_t lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE;
  uint8_t lBitNumber_u8 = 0U;     /* get the unscaled bit number which needs to be addressed */
  uint8_t lPosition_u8 = 0U;      /* get the position in the bitfield which needs to be addressed */
  uint8_t lArrPosition_u8 = 0U;   /* translate the bitfield position into the Array index */
  uint8_t lBitPosition_u8 = 0U;   /* translate the bitfield position into the Bit position  */

  if(
    /* valid state pairs are those that are within the initialized limits ;
    functionality can be provided only if variable address is provided */
    (aFromStateNum_u8 < Fsm.numOfStates_u8) && (aToStateNum_u8 < Fsm.numOfStates_u8) && (aRequiredReturnStatus_u8 != NULL) &&
    /* get the position, in the stateTransitionEvaluator_u8 array, for setting the required Function */
    (FsmUtilGetBitNumber(aFromStateNum_u8, aToStateNum_u8, &lBitNumber_u8) == FSM_UTIL_RETURNTYPE_SUCCESS)
    )
  {
    lPosition_u8 = ((MAX_NUM_STATES - 1)*aFromStateNum_u8 + lBitNumber_u8)*NUM_BITS_PER_STATE;
    /* 64 because the structure array is of type uint64_t; (-1) for scaling correctly */
    lArrPosition_u8 = ((lPosition_u8/64U) - 1U);
    lBitPosition_u8 = lPosition_u8%64U;

    (*aRequiredReturnStatus_u8) = ((Fsm.stateEvaluatorReturnStatus_u64[lArrPosition_u8] & (1U << lBitPosition_u8)) >> lBitPosition_u8);
    lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_SUCCESS;
  }
  else
  {
    /* Function will not work when invalid state pair and NULL pointer address is provided */
    /* do nothing and let lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE */
  }
  return lReturnStatus_u8;
}

uint8_t fsmUtilityGetNumberOfStates_u8(uint8_t * aNumberOfStates_u8)
{
  uint8_t lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE;
  if(aNumberOfStates_u8 != NULL)
  {
    (*aNumberOfStates_u8) = Fsm.numOfStates_u8;
    lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_SUCCESS;
  }
  else
  {
    /* Do nothing and let lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE */
  }
  return lReturnStatus_u8;
}

uint8_t fsmUtilityGetCurrentFsmState_u8(uint8_t * aCurrentState_u8)
{
  uint8_t lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE;
  if(aCurrentState_u8 != NULL)
  {
    (*aCurrentState_u8) = Fsm.currentFsmState_u8;
    lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_SUCCESS;
  }
  else
  {
    /* Do nothing and let lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE */
  }
  return lReturnStatus_u8;
}

void fsmUtilFullResetFsm(void)
{
  void FsmUtilInitFsmStruct(void); /* Full reset is equivalent to reintialization */
}

/*************************************************************************
  API FUNCTION DEFINITIONS FOR FSM EXECUTION FUNCTIONS
*************************************************************************/

uint8_t fsmUtilUpdateFsm(void)
{
  uint8_t lCurrentState_u8 = 0U;
  uint8_t lFromStateIterator_u8 = 0U;
  uint8_t lToStateIterator_u8 = 0U;
  uint8_t lMaxNumberOfStates_u8 = 0U;
  uint8_t lStatePermission_u8 = 0U;
  uint8_t lEvaluatorReturnStatus_u8 = 0U;
  if(
    (fsmUtilityGetCurrentFsmState_u8(&lCurrentState_u8) == FSM_UTIL_RETURNTYPE_SUCCESS) &&    /* get the current state */
    (fsmUtilityGetNumberOfStates_u8(&lMaxNumberOfStates_u8) ==  FSM_UTIL_RETURNTYPE_SUCCESS) /* get max number of states available in the FSM */
    )
  {
    for(lFromStateIterator_u8 = 0; ((lFromStateIterator_u8 < lMaxNumberOfStates_u8) && (lFromStateIterator_u8 != lCurrentState_u8)); lFromStateIterator_u8++)
    {

    }
  }

}
/*************************************************************************
  STATIC FUNCTION DEFINITION
*************************************************************************/
static void FsmUtilInitFsmStruct(void)
{
  uint8_t lZero_u8 = 0U;

  Fsm.currentFsmState_u8 = 0U;
  Fsm.numOfStates_u8 = 0U;
  memcpy(&Fsm.permissibleStateTransition_u64, &lZero_u8, NUM_STATE_TRANSITION_GRAPH);
  memcpy(&Fsm.stateFunction_u8, &lZero_u8, MAX_NUM_STATE_TRANSITION);
  memcpy(&Fsm.stateTransitionEvaluator_u8, &lZero_u8, MAX_NUM_STATE_TRANSITION);
  Fsm.isInitialized_u8 = 1U;
}

static uint8_t FsmUtilGetBitNumber(uint8_t aFromStateNum_u8, uint8_t aToStateNum_u8, uint8_t * aReturnBitNumber_u8)
{
  uint8_t lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE;
  /* valid state pairs are those that are within the initialized limits ;
  functionality can be provided only if variable address is provided */
  if((aFromStateNum_u8 < Fsm.numOfStates_u8) && (aToStateNum_u8 < Fsm.numOfStates_u8) && (aReturnBitNumber_u8 != NULL)) 
  {
    (*aReturnBitNumber_u8) = ((aFromStateNum_u8 > aToStateNum_u8)?(aToStateNum_u8):(aToStateNum_u8 - 1));
    lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_SUCCESS;
  }
  else
  {
    /* Function will not work when invalid state pair and NULL pointer address is provided */
    /* do nothing and let lReturnStatus_u8 = FSM_UTIL_RETURNTYPE_FAILURE */
  }
  return lReturnStatus_u8;
}