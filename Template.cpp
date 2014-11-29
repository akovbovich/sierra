// The top of every source code file must include this line
#include "sierrachart.h"


/*****************************************************************************

	For reference, please refer to the Advanced Custom Study Interface
	and Language documentation on the Sierra Chart website. 
	
*****************************************************************************/


// This line is required. Change the text within the quote
// marks to what you want to name your group of custom studies. 
SCDLLName("Custom Study DLL") 


//This is the basic framework of a study function.
SCSFExport scsf_SkeletonFunction(SCStudyGraphRef sc)
{
	// Section 1 - Set the configuration variables and defaults
	if (sc.SetDefaults)
	{
		sc.GraphName = "Skeleton Function";
		
		// During development set this flag to 1, so the DLL can be rebuilt without restarting Sierra Chart. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 1;

		sc.AutoLoop = 1;  //Auto looping is enabled. 
		
		sc.Subgraph[0].Name = "Name";
		sc.Subgraph[0].DrawStyle = DRAWSTYLE_LINE;
		sc.Subgraph[0].PrimaryColor = RGB (0, 255, 0);
		
		sc.Input[0].Name = "Float Input";
		sc.Input[0].SetFloat(0.0f);
		
		return;
	}
	
	
	// Section 2 - Do data processing here
	
	
}
