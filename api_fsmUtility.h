#include "stdio.h"
#include <stdint.h>
#include "stdlib.h"
#include "string.h"

#define NUM_STATES  ((uint8_t)7U)

/* Calculations required to define the Structure members based on User Input */
#define NUM_STATE_TRANSITION          ((uint8_t)(NUM_STATES*(NUM_STATES - 1U)))
#define NUM_STATE_TRANSITION_GRAPH    (((uint16_t)(NUM_STATE_TRANSITION)/64)+1)
#define NUM_BITS_PER_STATE            ((uint8_t)2U)

/* Designated function arguments for using the APIs */

/* < FUNCTION = fsmUtilSetStateEvaluatorReturnStatus > < ARGUMENT = aPermissionStatus_u8 > */
/* < FUNCTION = fsmUtilGetStateEvaluatorReturnStatus > < ARGUMENT = *aPermissionStatus_u8 > */
/* < INFERENCE = " The state transition evaluation returns Success "> */
#define FSM_STATE_EVALUATION_CRITERIA_PASS  ((uint8_t)1U)
/* < INFERENCE = " The state transition evaluation returns Failure "> */
#define FSM_STATE_EVALUATION_CRITERIA_FAIL  ((uint8_t)0U)
/* < \ ARGUMENT > < \ FUNCTION > */

/* < FUNCTION = fsmUtilSetStateTransitionPermission > < ARGUMENT = aPermissionStatus_u8 > */
/* < FUNCTION = fsmUtilGetStateTransitionPermission > < ARGUMENT = *aPermissionStatus_u8 > */
/* < INFERENCE = " The state transition is permitted "> */
#define FSM_STATE_TRANSITION_GRANTED  ((uint8_t)1U)
/* < INFERENCE = " The state transition is not permitted "> */
#define FSM_STATE_TRANSITION_DENIED   ((uint8_t)0U)
/* < \ ARGUMENT > < \ FUNCTION > */

/* Error Map for the APIs so that Developers that integrate this Feature can know the Status of execution of the API */
#define FSM_UTIL_RETURNTYPE_SUCCESS  ((uint8_t)0x00U)
#define FSM_UTIL_RETURNTYPE_FAILURE  ((uint8_t)0xFEU)

/*
The following is the main structure responsible for creating the main datatype governing the FSM.
properties of any FSM:
*/

typedef struct
{
/*
- number of states of the FSM
*/
  uint8_t numOfStates_u8; 

/*
- current state of the FSM.
*/
  uint8_t currentFsmState_u8;

/*
- permissible state transition diagram/graph representation (bitfield)
  - this graph can be altered during the execution to allow the developer to control the FSM.
  - 2 bits per state-pair (the state under consideration & other possible state)
    - 1st bit represents if transition from the given state to a certain state is permitted
    - 2nd bit represents if transition from a certain state to the given state is permitted
      - 0 = not permitted / 1 = permitted
*/
  uint64_t permissibleStateTransition_u64[NUM_STATE_TRANSITION_GRAPH];

/*
- the state evaluator function updates the bit respective to their from/to state pair (as per the permissible state transition graph)
*/
  uint64_t stateEvaluatorReturnStatus_u64[(NUM_STATE_TRANSITION_GRAPH/2)];

/*
- array of function pointers that point to functions (void arguments, void return-type) to be executed when present in any particular state.
*/
  void (*stateFunction_u8[NUM_STATES])(void);

/*
- array of function pointers that point to functions (void arguments, void return-type) to be executed when state transition needs to be evaluated.
*/
  void (*stateTransitionEvaluator_u8[NUM_STATE_TRANSITION])(void);

/*
- flag for initialization the members of the structure for the first time.
*/
  uint8_t isInitialized_u8;

}s_fsmType;

/*
@brief
creating and initializing a FSM.

@details
Get started with the FSM by initializing the utmost important structure members.

@input
- Number of states that are going to be present in the FSM.
- State transition graph that represents what valid transitions can be carried out while running the FSM.

@output
- Return type that indicated the success / failure of the API.
- FSM structure variable initialized with the given data.
*/
uint8_t fsmUtilCreateFsm(uint8_t aNumberOfStates_u8, uint64_t aPermissibleStateTransition_u64[]);

/*
@brief

@details

@input

@output

*/
uint8_t fsmUtilSetStateFunction(uint8_t aState_u8, void (* aFsmStateFunction)(void));

/*
@brief

@details

@input

@output

*/
uint8_t fsmUtilSetStateTransitionEvaluatorFunction(uint8_t aFromStateNum_u8, uint8_t aToStateNum_u8, void (* aFromToStateEvaluatorFn)(void));

/*
@brief

@details

@input

@output

*/
uint8_t fsmUtilSetStateTransitionPermission(uint8_t aFromStateNum_u8, uint8_t aToStateNum_u8, uint8_t aPermissionStatus_u8);

/*
@brief

@details

@input

@output

*/
uint8_t fsmUtilGetStateTransitionPermission(uint8_t aFromStateNum_u8, uint8_t aToStateNum_u8, uint8_t * aPermissionStatus_u8);

/*
@brief

@details

@input

@output

*/
uint8_t fsmUtilSetStateEvaluatorReturnStatus(uint8_t aFromStateNum_u8, uint8_t aToStateNum_u8, uint8_t aRequiredReturnStatus_u8);

/*
@brief

@details

@input

@output

*/
uint8_t fsmUtilGetStateEvaluatorReturnStatus(uint8_t aFromStateNum_u8, uint8_t aToStateNum_u8, uint8_t * aRequiredReturnStatus_u8);

/*
@brief

@details

@input

@output

*/
uint8_t fsmUtilGetNumberOfStates_u8(uint8_t * aNumberOfStates_u8);

/*
@brief

@details

@input

@output

*/
uint8_t fsmUtilGetCurrentFsmState_u8(uint8_t * aCurrentState_u8);

/*
@brief

@details

@input

@output

*/
void fsmUtilFullResetFsm(void);