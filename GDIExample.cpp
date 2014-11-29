#include <windows.h>
#include "sierrachart.h"

//SCDLLName ("GDI Example")

// This file demonstrates the functionality to use the Windows graphics device interface with ACSIL to freely draw inside of the chart window

// Windows GDI documentation can be found here: http://msdn.microsoft.com/en-nz/library/windows/desktop/dd145203%28v=vs.85%29.aspx

/*==========================================================================*/
//Drawing function declaration
void DrawToChart(HWND WindowHandle, HDC DeviceContext, SCStudyInterfaceRef sc); 


/*==========================================================================*/
SCSFExport scsf_DrawToChartExample(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Draw To Chart Example";

		sc.AutoLoop = 0;

		//  When specifying a GDI function below, this must be false.  Otherwise, the GDI function will not be called.
		sc.FreeDLL = 0;

		// This is how we specify the drawing function. This function will be called when the study graph is drawn on the chart.
		sc.p_GDIFunction = DrawToChart;

		return;
	}


	// Do data processing


}
/*==========================================================================*/

//This is the actual drawing function. This function is specified by the "sc.p_GDIFunction" member in the main study function above. This drawing function is called when Sierra Chart draws the study on the chart.  This will only occur after there has been a call to the main "scsf_" study function which is defined above.

//This drawing function has access to the ACSIL "sc." structure. However, any changes to the variable members will have no effect.


void DrawToChart(HWND WindowHandle, HDC DeviceContext, SCStudyInterfaceRef sc )
{
	//Create a Yellow brush
	HBRUSH Brush = CreateSolidBrush(RGB(255,255,0));

	//Select the  brush into the device context
	HGDIOBJ PriorBrush = SelectObject(DeviceContext, Brush);

	//Draw a rectangle at the top left of the chart
	Rectangle(DeviceContext, 0,0 ,200, 200);

	//Remove the brush from the device context and put the prior brush back in. This is critical!
	SelectObject(DeviceContext,PriorBrush);

	//Delete the brush.  This is critical!  If you do not do this, you will end up with a GDI leak and crash Sierra Chart.
	DeleteObject(Brush);
	return;
}

/*==========================================================================*/

