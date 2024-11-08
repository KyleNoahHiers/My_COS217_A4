/*--------------------------------------------------------------------*/
/* checkerDT.c                                                        */
/* Author:                                                            */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "checkerDT.h"
#include "dynarray.h"
#include "path.h"

/* see checkerDT.h for specification */
boolean CheckerDT_Node_isValid(Node_T oNNode)
{
   Node_T oNParent;
   Path_T oPNPath;
   Path_T oPPPath;

   /* Sample check: a NULL pointer is not a valid node */
   if (oNNode == NULL)
   {
      fprintf(stderr, "A node is a NULL pointer\n");
      return FALSE;
   }

   /* Sample check: parent's path must be the longest possible
      proper prefix of the node's path */
   oNParent = Node_getParent(oNNode);
   if (oNParent != NULL)
   {
      oPNPath = Node_getPath(oNNode);
      oPPPath = Node_getPath(oNParent);

      if (Path_getSharedPrefixDepth(oPNPath, oPPPath) !=
          Path_getDepth(oPNPath) - 1)
      {
         fprintf(stderr, "P-C nodes don't have P-C paths: (%s) (%s)\n",
                 Path_getPathname(oPPPath), Path_getPathname(oPNPath));
         return FALSE;
      }
   }

   return TRUE;
}

/*
   Performs a pre-order traversal of the tree rooted at oNNode.
   Calculates the size of the tree and stores it in the location pointed
   to by total.
   Returns FALSE if:
   a. a duplicate node is found, then updates duplicateNodeFound to 1
   b. Node_getNumChildren claims more children than Node_getChild 
   returns
   c. wrong lexicographic ordering is found, then updates 
   orderWrongOccured to 1
   returns TRUE otherwise.

*/
static boolean CheckerDT_treeCheck(Node_T oNNode, 
int* orderWrongOccured, int* duplicateNodeFound, size_t* total)
{
   size_t ulIndex;
   Node_T prevNode = NULL;

   assert(total!= NULL);
   assert(orderWrongOccured!= NULL);
   assert(duplicateNodeFound!= NULL);

   if (oNNode != NULL)
   {

      /* Sample check on each node: node must be valid */
      /* If not, pass that failure back up immediately */
      if (!CheckerDT_Node_isValid(oNNode))
         return FALSE;

      /* for first node set total to one*/
      if (*total == 0) *total = 1;

      /* Recur on every child of oNNode */
      for (ulIndex = 0; ulIndex < Node_getNumChildren(oNNode); ulIndex++)
      {
         Node_T oNChild = NULL;
         int iStatus = Node_getChild(oNNode, ulIndex, &oNChild);
        
         if (iStatus != SUCCESS)
         {
            fprintf(stderr, "getNumChildren claims more children than getChild returns\n");
            return FALSE;
         }

          if (prevNode!= NULL && Path_comparePath(Node_getPath(oNChild),
           Node_getPath(prevNode)) < 0 && !*orderWrongOccured)
         {
            *orderWrongOccured = 1; 
            return FALSE;
         }
          if (prevNode!= NULL && Path_comparePath(Node_getPath(oNChild),
           Node_getPath(prevNode)) == 0 && !*duplicateNodeFound)
         {
            *duplicateNodeFound = 1; 
            return FALSE;
         }

         /* if recurring down one subtree results in a failed check
            farther down, passes the failure back up immediately */
         
         if(!CheckerDT_treeCheck(oNChild, orderWrongOccured, duplicateNodeFound, total))
            return FALSE;

         prevNode = oNChild;
         *total += 1;

        
      }
   }
   return TRUE;
}



/* see checkerDT.h for specification */
boolean CheckerDT_isValid(boolean bIsInitialized, Node_T oNRoot,
                          size_t ulCount) {
boolean result;
int orderWrong = 0;
int duplicateNodeFound = 0;
size_t total = 0;
   /* Sample check on a top-level data structure invariant:
      if the DT is not initialized, its count should be 0. */
   if(!bIsInitialized)
      if(ulCount != 0) {
         fprintf(stderr, "Not initialized, but count is not 0\n");
         return FALSE;
      }


   /* Now checks invariants recursively at each node from the root. */
   result = CheckerDT_treeCheck(oNRoot, &orderWrong, &duplicateNodeFound, 
   &total);
   if (orderWrong) {
      fprintf(stderr, "Nodes stored in the wrong order\n");
   }
   if (duplicateNodeFound) {
      fprintf(stderr, "Two nodes with the same path exist in the DT\n");
   }
   if (total != ulCount){ 
      fprintf(stderr, "The total number of nodes (in ulCount) is not being updated properly \n");
      /* set result to false since tree check helper function 
      does not internally check for this condition */
      result = FALSE;
   }
   return result;
}