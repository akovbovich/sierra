// The top of every source code file must include this line
#include "sierrachart.h"


/*****************************************************************************

	For reference, please refer to the Advanced Custom Study Interface
	and Language documentation on the Sierra Chart website. 
	
*****************************************************************************/


// This line is required. Change the text within the quote
// marks to what you want to name your group of custom studies. 
SCDLLName("AutoLoopExample") 


//This is the basic framework of a study function.
SCSFExport scsf_SkeletonFunction(SCStudyGraphRef sc)
{
	// Section 1 - Set the configuration variables and defaults
	if (sc.SetDefaults)
	{
		sc.GraphName = "Auto Loop Example";
		
		// During development set this flag to 1, so the DLL can be rebuilt without restarting Sierra Chart. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 1;

		sc.AutoLoop = 1;  //Auto looping is enabled. 
		
		sc.Subgraph[0].Name = "Average";
		sc.Subgraph[1].Name = "Back Reference Example";
		sc.Subgraph[3].Name = "Current Low Price";
		
		return;
	}
	
	
	// Section 2 - Do data processing here
	sc.SimpleMovAvg(sc.BaseData[SC_LAST], sc.Subgraph[0], sc.Index, 10);
	sc.Subgraph[1][sc.Index] = sc.BaseData[SC_LAST][sc.Index - 1];
	sc.Subgraph[3][sc.Index] = sc.BaseData[SC_LOW][sc.Index];
	
}
