#include "sierrachart.h"

#include "scstudyfunctions.h"

/*==========================================================================*/
SCSFExport scsf_Accumulate(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Summ = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];

	if(sc.SetDefaults)	
	{
		sc.GraphName = "Summation";
		sc.GraphRegion = 1;
		sc.ValueFormat = 2;

		Summ.Name = "Summ";
		Summ.DrawStyle = DRAWSTYLE_LINE;
		Summ.PrimaryColor = RGB(0,255,0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;
		return;
	}

	sc.CumulativeSummation(sc.BaseData[InputData.GetInputDataIndex()], Summ, sc.Index);
}

/*==========================================================================*/
SCSFExport scsf_ARMSEaseOfMovement(SCStudyInterfaceRef sc)
{
	SCSubgraphRef EMV = sc.Subgraph[0];

	SCInputRef VolumeDivisor = sc.Input[3];

	if(sc.SetDefaults)
	{
		sc.GraphName = "ARMS Ease of Movement";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;

		EMV.Name = "EMV";
		EMV.DrawStyle = DRAWSTYLE_LINE;
		EMV.PrimaryColor = RGB(0,255,0);
		EMV.DrawZeros = false;

		VolumeDivisor.Name = "Volume divisor";
		VolumeDivisor.SetInt(100);
		VolumeDivisor.SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}
	if(sc.Index < 1)
		return;

	sc.ArmsEMV(sc.BaseDataIn, EMV, VolumeDivisor.GetInt(), sc.Index);
}

/*==========================================================================*/
SCSFExport scsf_ChaikinMoneyFlow(SCStudyInterfaceRef sc)
{
	SCSubgraphRef CMF = sc.Subgraph[0];


	SCInputRef Length = sc.Input[3];

	if(sc.SetDefaults)	
	{
		sc.GraphName = "Chaikin Money Flow";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;

		CMF.Name = "CMF";
		CMF.DrawStyle = DRAWSTYLE_LINE;
		CMF.PrimaryColor = RGB(0,255,0);
		CMF.DrawZeros = false;

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	sc.ChaikinMoneyFlow(sc.BaseDataIn, CMF, Length.GetInt()); 
}

/*==========================================================================*/
SCSFExport scsf_Dispersion(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Dispersion = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Dispersion";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;

		Dispersion.Name = "Dispersion";
		Dispersion.DrawStyle = DRAWSTYLE_LINE;
		Dispersion.PrimaryColor = RGB(0,255,0);
		Dispersion.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	sc.Dispersion(sc.BaseDataIn[(int)InputData.GetInputDataIndex()], Dispersion, sc.Index, Length.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_Envelope(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TopBand    = sc.Subgraph[0];
	SCSubgraphRef BottomBand = sc.Subgraph[1];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Value     = sc.Input[3];
	SCInputRef BandsType = sc.Input[4];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Bands/Envelope";

		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;

		TopBand.Name = "TopBand";
		TopBand.DrawStyle = DRAWSTYLE_LINE;
		TopBand.PrimaryColor = RGB(0,255,0);
		TopBand.DrawZeros = false;

		BottomBand.Name = "BottomBand";
		BottomBand.DrawStyle = DRAWSTYLE_LINE;
		BottomBand.PrimaryColor = RGB(255,0,255);
		BottomBand.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Value.Name = "Percentage/Fixed Value/Ticks";
		Value.SetFloat(0.05f);
		Value.SetFloatLimits(0.00001f,(float)MAX_STUDY_LENGTH);

		BandsType.Name = "Bands Type";
		BandsType.SetCustomInputStrings("Percentage;Fixed Value;Number of Ticks");
		BandsType.SetCustomInputIndex(0);

		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}


	SCFloatArrayRef In = sc.BaseDataIn[InputData.GetInputDataIndex()];

	if(BandsType.GetIndex() == 0)
	{
		sc.EnvelopePct(In, TopBand, Value.GetFloat());
		BottomBand[sc.Index] = TopBand.Arrays[0][sc.Index];
	}
	else if(BandsType.GetIndex() == 1)
	{
		sc.EnvelopeFixed(In, TopBand, Value.GetFloat());
		BottomBand[sc.Index] = TopBand.Arrays[0][sc.Index];
	}
	else
	{
		sc.EnvelopeFixed(In, TopBand, Value.GetFloat() * sc.TickSize);
		BottomBand[sc.Index] = TopBand.Arrays[0][sc.Index];
	}
}

/*==========================================================================*/
SCSFExport scsf_VerticalHorizontalFilter(SCStudyInterfaceRef sc)
{
	SCSubgraphRef VHF = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if(sc.SetDefaults)	
	{
		sc.GraphName = "Vertical Horizontal Filter";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;

		VHF.Name = "VHF";
		VHF.DrawStyle = DRAWSTYLE_LINE;
		VHF.PrimaryColor = RGB(0,255,0);
		VHF.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(28);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}
	if(sc.Index < Length.GetInt())
		return;

	SCFloatArrayRef In = sc.BaseDataIn[(int)InputData.GetInputDataIndex()];
	sc.VHF(In, VHF, sc.Index, Length.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_RWI(SCStudyInterfaceRef sc)
{
	SCSubgraphRef RWIHigh = sc.Subgraph[0];
	SCSubgraphRef RWILow = sc.Subgraph[1];
	SCSubgraphRef Temp2 = sc.Subgraph[2];
	SCSubgraphRef Temp3 = sc.Subgraph[3];
	SCSubgraphRef Temp4 = sc.Subgraph[4];

	SCInputRef Length = sc.Input[3];

	if(sc.SetDefaults)	
	{
		sc.GraphName = "Random Walk Indicator";

		sc.GraphRegion = 1;			

		RWIHigh.Name = "RWI High";
		RWIHigh.PrimaryColor = RGB(0,255,0);
		RWIHigh.DrawStyle = DRAWSTYLE_LINE;

		RWILow.Name = "RWI Low";
		RWILow.PrimaryColor = RGB(255,0,255);
		RWILow.DrawStyle = DRAWSTYLE_LINE;

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	sc.DataStartIndex = Length.GetInt() - 1;
	sc.InternalRWI(sc.BaseDataIn, RWIHigh, RWILow, Temp2, Temp3, Temp4, sc.Index, Length.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_UltimateOscillator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef UltimateOscillatorOut1 = sc.Subgraph[0];
	SCSubgraphRef UltimateOscillatorOut2 = sc.Subgraph[1];


	SCInputRef Length1 = sc.Input[3];
	SCInputRef Length2 = sc.Input[4];
	SCInputRef Length3 = sc.Input[5];

	if(sc.SetDefaults)	
	{
		sc.GraphName = "Ultimate Oscillator";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;

		UltimateOscillatorOut1.Name = "UO";
		UltimateOscillatorOut1.DrawZeros = false;
		UltimateOscillatorOut1.DrawStyle = DRAWSTYLE_LINE;
		UltimateOscillatorOut1.PrimaryColor = RGB(0,255,0);

		Length1.Name = "Length 1";
		Length1.SetInt(7);
		Length1.SetIntLimits(1,MAX_STUDY_LENGTH);

		Length2.Name = "Length 2";
		Length2.SetInt(14);
		Length2.SetIntLimits(1,MAX_STUDY_LENGTH);

		Length3.Name = " Length 3";
		Length3.SetInt(28);
		Length3.SetIntLimits(1,MAX_STUDY_LENGTH);


		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	sc.DataStartIndex = Length3.GetInt();

	sc.UltimateOscillator(sc.BaseData, UltimateOscillatorOut1,  UltimateOscillatorOut2, Length1.GetInt(), Length2.GetInt(), Length3.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_WilliamsAD(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AccumulationDistribution = sc.Subgraph[0];

	if(sc.SetDefaults)	
	{
		sc.GraphName="Accumulation/Distribution (Williams)";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;

		AccumulationDistribution.Name = "AD";
		AccumulationDistribution.DrawStyle = DRAWSTYLE_LINE;
		AccumulationDistribution.PrimaryColor = RGB(0,255,0);


		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	sc.DataStartIndex = 1;

	sc.WilliamsAD(sc.BaseDataIn, AccumulationDistribution, sc.Index);

}

/*==========================================================================*/
SCSFExport scsf_WilliamsR(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PercentR = sc.Subgraph[0];

	SCInputRef Length = sc.Input[3];
	SCInputRef Invert = sc.Input[4];
	SCInputRef InputDataHigh = sc.Input[5];
	SCInputRef InputDataLow = sc.Input[6];
	SCInputRef InputDataLast = sc.Input[7];
	SCInputRef UpdateFlag = sc.Input[8];

	if(sc.SetDefaults)	
	{
		sc.GraphName = "Williams %R";

		sc.GraphRegion= 1;
		sc.ValueFormat = 2;

		PercentR.Name = "%R";
		PercentR.DrawZeros = true;
		PercentR.DrawStyle = DRAWSTYLE_LINE;
		PercentR.PrimaryColor = RGB(0,255,0);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		Invert.Name = "Invert Output";
		Invert.SetYesNo(1);

		InputDataHigh.Name = "Input Data for High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);

		InputDataLow.Name = "Input Data for Low";
		InputDataLow.SetInputDataIndex(SC_LOW);

		InputDataLast.Name = "Input Data for Last";
		InputDataLast.SetInputDataIndex(SC_LAST);

		UpdateFlag.SetInt(1); //update flag

		sc.AutoLoop = 1;


		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;


		return;
	}

	if (UpdateFlag.GetInt() == 0)
	{
		InputDataHigh.SetInputDataIndex(SC_HIGH);
		InputDataLow.SetInputDataIndex(SC_LOW);
		InputDataLast.SetInputDataIndex(SC_LAST);
		UpdateFlag.SetInt(1);
	}

	sc.DataStartIndex = Length.GetInt();

	sc.WilliamsR(
		sc.BaseData[InputDataHigh.GetInputDataIndex()],
		sc.BaseData[InputDataLow.GetInputDataIndex()],
		sc.BaseData[InputDataLast.GetInputDataIndex()],
		PercentR,
		sc.Index,
		Length.GetInt()
		);

	if (Invert.GetYesNo())
		PercentR[sc.Index] *= -1;
}

/*==========================================================================*/
SCSFExport scsf_IslandReversal(SCStudyInterfaceRef sc)
{
	SCSubgraphRef IR = sc.Subgraph[0];

	if (sc.SetDefaults)	
	{
		sc.GraphName = "Island Reversal";

		sc.GraphRegion= 0;
		sc.ValueFormat = 0;

		IR.Name = "IR";
		IR.DrawStyle = DRAWSTYLE_POINT;
		IR.PrimaryColor = RGB(0,255,0);
		IR.LineWidth = 8;
		IR.DrawZeros = false;

		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	const int fmZERO = 0;
	const int fmUP = 1;
	const int fmDOWN = -1;

	int r = sc.GetIslandReversal(sc.BaseDataIn, sc.Index);

	if (r == fmZERO)
		IR[sc.Index]= 0;
	else if (r == fmUP)
		IR[sc.Index] = sc.High[sc.Index];
	else if (r == fmDOWN)
		IR[sc.Index] = sc.Low[sc.Index];
}
/*==========================================================================*/
SCSFExport scsf_WAOscillator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Oscillator = sc.Subgraph[0];
	SCSubgraphRef Temp1 = sc.Subgraph[1];
	SCSubgraphRef Temp2 = sc.Subgraph[2];

	SCInputRef InputData = sc.Input[0];
	SCInputRef SlowAvgLength = sc.Input[3];
	SCInputRef FastAvgLength = sc.Input[4];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Weighted Average Oscillator";

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		Oscillator.Name = "Oscillator";
		Oscillator.DrawStyle = DRAWSTYLE_LINE;
		Oscillator.PrimaryColor = RGB(0,255,0);
		Oscillator.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		SlowAvgLength.Name = "Slow Average Length";
		SlowAvgLength.SetInt(10);
		SlowAvgLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		FastAvgLength.Name = "Fast Average Length";
		FastAvgLength.SetInt(3);
		FastAvgLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;


		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}
	sc.DataStartIndex = max( SlowAvgLength.GetInt(),FastAvgLength.GetInt());


	sc.WeightedMovingAverage(sc.BaseDataIn[(int)InputData.GetInputDataIndex()], Temp1, SlowAvgLength.GetInt());
	sc.WeightedMovingAverage(sc.BaseDataIn[(int)InputData.GetInputDataIndex()], Temp2, FastAvgLength.GetInt());

	sc.Oscillator(Temp2, Temp1, Oscillator);
}

/*==========================================================================*/
SCSFExport scsf_PreferredStochasticDiNapoli(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PercentK = sc.Subgraph[0];
	SCSubgraphRef PercentD = sc.Subgraph[1];
	SCSubgraphRef Line1 = sc.Subgraph[2];
	SCSubgraphRef Line2 = sc.Subgraph[3];

	SCInputRef FastKLength = sc.Input[3];
	SCInputRef FastDMovAvgLength = sc.Input[4];
	SCInputRef ModifiedMovAvgLength = sc.Input[5];
	SCInputRef Line1Value = sc.Input[6];
	SCInputRef Line2Value = sc.Input[7];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Preferred Stochastic - DiNapoli";

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		PercentK.Name = "%K";
		PercentK.PrimaryColor = RGB(0,255,0);
		PercentK.DrawStyle = DRAWSTYLE_LINE;
		PercentK.DrawZeros = false;

		PercentD.Name = "%D";
		PercentD.PrimaryColor = RGB(255,0,255);
		PercentD.DrawStyle = DRAWSTYLE_LINE;
		PercentD.DrawZeros = false;

		Line1.Name = "Line1";
		Line1.PrimaryColor = RGB(255,255,0);
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.DrawZeros = false;

		Line2.Name = "Line2";
		Line2.PrimaryColor = RGB(255,127,0);
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.DrawZeros = false;

		FastKLength.Name = "Fast %K Length";
		FastKLength.SetInt(10);
		FastKLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		FastDMovAvgLength.Name = "Fast %D Mov Avg Length (Slow %K)";
		FastDMovAvgLength.SetInt(3);
		FastDMovAvgLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		ModifiedMovAvgLength.Name = "Modified Mov Avg Length (Slow %D)"; 
		ModifiedMovAvgLength.SetInt(3);
		ModifiedMovAvgLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		Line1Value.Name = "Line1 Value";
		Line1Value.SetFloat(70);

		Line2Value.Name = "Line2 Value";
		Line2Value.SetFloat(30);

		sc.AutoLoop = 1;

		// During development set this flag to 1, so the DLL can be modified. When development is done, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	int Index = sc.Index;
	sc.DataStartIndex = FastKLength.GetInt() + FastDMovAvgLength.GetInt() + 1;

	sc.Stochastic(sc.BaseDataIn, sc.Subgraph[4], FastKLength.GetInt(), 3, 3, MOVAVGTYPE_SIMPLE);

	sc.ExponentialMovAvg(sc.Subgraph[4], PercentK, Index, FastDMovAvgLength.GetInt());

	PercentD[Index] = PercentD[Index - 1] + 
		((PercentK[Index] - PercentD[Index - 1]) / ModifiedMovAvgLength.GetInt());

	Line1[Index] = Line1Value.GetFloat();
	Line2[Index] = Line2Value.GetFloat();
}

/*==========================================================================*/
SCSFExport scsf_ElderRay(SCStudyInterfaceRef sc)
{
	SCSubgraphRef BullPower = sc.Subgraph[0];
	SCSubgraphRef BearPower = sc.Subgraph[1];
	SCSubgraphRef Zero = sc.Subgraph[2];
	SCSubgraphRef MovAvgTemp = sc.Subgraph[3];

	SCInputRef MovAvgLength = sc.Input[3];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Elder Ray";

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		BullPower.Name = "Bull Power";
		BullPower.DrawStyle = DRAWSTYLE_LINE;
		BullPower.PrimaryColor = RGB(0,255,0);
		BullPower.DrawZeros = true;

		BearPower.Name = "Bear Power";
		BearPower.DrawStyle = DRAWSTYLE_LINE;
		BearPower.PrimaryColor = RGB(255,0,255);
		BearPower.DrawZeros = true;

		Zero.Name = "0";
		Zero.DrawStyle = DRAWSTYLE_LINE;
		Zero.PrimaryColor = RGB(255,255,0);
		Zero.DrawZeros = true;




		MovAvgLength.SetInt(10);
		MovAvgLength.SetIntLimits(1,MAX_STUDY_LENGTH);
		MovAvgLength.Name = "Moving Average Length";


		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	sc.DataStartIndex = MovAvgLength.GetInt() - 1;


	int i = sc.Index;
	sc.ExponentialMovAvg(sc.Close, MovAvgTemp, i, MovAvgLength.GetInt());


	BullPower[i] = sc.High[i] - MovAvgTemp[i];
	BearPower[i] = sc.Low[i]	- MovAvgTemp[i];
}

/*==========================================================================*/
SCSFExport scsf_MomentumWithMovingAverage(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Momentum = sc.Subgraph[0];
	SCSubgraphRef MovingAverage = sc.Subgraph[1];

	SCInputRef InputData = sc.Input[0];
	SCInputRef MomentumLength = sc.Input[3];
	SCInputRef MovAvgLength = sc.Input[4];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Momentum with Moving Average";

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		Momentum.Name = "Momentum";
		Momentum.DrawStyle = DRAWSTYLE_LINE;
		Momentum.PrimaryColor = RGB(0,255,0);
		Momentum.DrawZeros = false;

		MovingAverage.Name = "Moving Average";
		MovingAverage.DrawStyle = DRAWSTYLE_LINE;
		MovingAverage.PrimaryColor = RGB(128,128,128);
		MovingAverage.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		MomentumLength.SetInt(10);
		MomentumLength.SetIntLimits(1,MAX_STUDY_LENGTH);
		MomentumLength.Name = "Momentum Length";

		MovAvgLength.SetInt(5);
		MovAvgLength.SetIntLimits(1,MAX_STUDY_LENGTH);
		MovAvgLength.Name = "Moving Average Length";

		sc.AutoLoop = 1;


		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	sc.DataStartIndex = MomentumLength.GetInt() + MovAvgLength.GetInt();
	int i = sc.Index;
	if(i < MomentumLength.GetInt())
		return;

	Momentum[i]= ( sc.BaseDataIn[(int)InputData.GetInputDataIndex()][i] / 
		sc.BaseDataIn[(int)InputData.GetInputDataIndex()][i - MomentumLength.GetInt()] ) * 100;

	sc.SimpleMovAvg(Momentum, MovingAverage, i, MovAvgLength.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_VolatilityChaikin(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Volatility = sc.Subgraph[0];

	SCSubgraphRef Temp1 = sc.Subgraph[1];
	SCSubgraphRef Temp2 = sc.Subgraph[2];

	SCInputRef Length = sc.Input[3];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Volatility - Chaikin's";

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		Volatility.Name = "Volatility";
		Volatility.DrawStyle = DRAWSTYLE_LINE;
		Volatility.PrimaryColor = RGB(0,255,0);

		Length.Name= "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	sc.DataStartIndex = Length.GetInt() + 4;
	int i = sc.Index;

	Temp2[i] = sc.High[i] - sc.Low[i];

	sc.ExponentialMovAvg(Temp2, Temp1, i, Length.GetInt());

	Volatility[i] = ((Temp1[i] - Temp1[i - Length.GetInt()])
		/ Temp1[i-Length.GetInt()]) * 100;
}


/*==========================================================================*/
SCSFExport scsf_RateOfChangePoints(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ROC = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Rate Of Change - Points";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;

		ROC.Name = "ROC";
		ROC.DrawStyle = DRAWSTYLE_LINE;
		ROC.PrimaryColor = RGB(0,255,0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop=1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}
	sc.DataStartIndex = Length.GetInt();

	ROC[sc.Index] = (sc.BaseDataIn[(int)InputData.GetInputDataIndex()][sc.Index] - sc.BaseDataIn[(int)InputData.GetInputDataIndex()][sc.Index - Length.GetInt()]);
}

/*==========================================================================*/
SCSFExport scsf_SquareOf9(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Anchor = sc.Subgraph[0];
	SCSubgraphRef L1Plus = sc.Subgraph[1];
	SCSubgraphRef L1Minus = sc.Subgraph[2];
	SCSubgraphRef L1 = sc.Subgraph[3];
	SCSubgraphRef BaseUpperGraph = sc.Subgraph[4];
	SCSubgraphRef BaseLowerGraph = sc.Subgraph[5];
	SCSubgraphRef Offset = sc.Subgraph[6];

	SCInputRef AnchorValue = sc.Input[2];
	SCInputRef Degree = sc.Input[3];
	SCInputRef Level = sc.Input[4];
	SCInputRef AutoAdjust = sc.Input[5];
	SCInputRef Multiplier = sc.Input[6];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Square of 9";

		sc.GraphRegion = 0;

		// Set line styles to Dash

		Anchor.Name = "Anchor";
		Anchor.DrawStyle = DRAWSTYLE_DASH;
		Anchor.PrimaryColor = RGB(0,255,255);
		Anchor.DrawZeros = false;
		Anchor.LineLabel = (LL_DISPLAY_VALUE
			| LL_DISPLAY_NAME
			| LL_NAME_ALIGN_LEFT_EDGE
			| LL_VALUE_ALIGN_LEFT_EDGE
			| LL_NAME_ALIGN_ABOVE
			);

		L1Plus.Name = "L1+";
		L1Plus.DrawStyle = DRAWSTYLE_DASH;	
		L1Plus.PrimaryColor = RGB(0,255,0);
		L1Plus.DrawZeros = false;
		L1Plus.LineLabel = (LL_DISPLAY_VALUE
			| LL_DISPLAY_NAME
			| LL_NAME_ALIGN_ABOVE
			);

		L1Minus.Name = "L1-";
		L1Minus.DrawStyle = DRAWSTYLE_DASH;
		L1Minus.PrimaryColor = RGB(255,0,0);
		L1Minus.DrawZeros = false;
		L1Minus.LineLabel = (LL_DISPLAY_VALUE
			| LL_DISPLAY_NAME			
			| LL_NAME_ALIGN_ABOVE
			);

		L1.Name = "L1";
		L1.DrawStyle = DRAWSTYLE_DASH;
		L1.PrimaryColor = RGB(255,127,0);
		L1.DrawZeros = false;
		L1.LineLabel = (LL_DISPLAY_VALUE
			| LL_DISPLAY_NAME
			| LL_NAME_ALIGN_ABOVE
			);


		BaseUpperGraph.Name = "BaseUpper";
		BaseUpperGraph.DrawStyle = DRAWSTYLE_IGNORE;
		BaseUpperGraph.PrimaryColor = RGB(255,0,0);
		BaseUpperGraph.DrawZeros = false;

		BaseLowerGraph.Name = "BaseLower";
		BaseLowerGraph.DrawStyle = DRAWSTYLE_IGNORE;
		BaseLowerGraph.PrimaryColor = RGB(0,0,255);
		BaseLowerGraph.DrawZeros = false;

		Offset.Name = "Offset";
		Offset.DrawStyle = DRAWSTYLE_IGNORE;
		Offset.PrimaryColor = RGB(127,0,255);
		Offset.DrawZeros = false;


		AnchorValue.SetFloat(0);
		AnchorValue.Name = "Anchor Value";
		AnchorValue.SetFloatLimits(0,(float)MAX_STUDY_LENGTH);

		Degree.SetFloat(22.5);    
		Degree.Name = "Degrees";
		Degree.SetFloatLimits(0,360);

		Level.SetFloat(1);    
		Level.Name = "Level";
		Level.SetFloatLimits(1,10);

		AutoAdjust.SetYesNo(false);
		AutoAdjust.Name = "Auto Adjust";

		sc.AutoLoop=1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	if (AnchorValue.GetFloat() == 0)
	{
		AnchorValue.SetFloat(sc.Close[sc.Open.GetArraySize() - 1]);
	}

	int level = (int)(Level.GetFloat() + 0.5f);
	float multiplier = Multiplier.GetFloat();
	if (multiplier == 0)
		multiplier = 1;

	int index = sc.Index;

	if (AutoAdjust.GetYesNo())
	{
		float SquareRoot = sqrt(AnchorValue.GetFloat());

		float offset;
		float mid;
		float BaseUpper;
		float BaseLower;
		float upper;
		float lower;

		if(sc.Index == 0)
		{
			offset = 0;
			BaseUpper = (SquareRoot + Degree.GetFloat()/180) * (SquareRoot + Degree.GetFloat()/180);
			BaseLower = (SquareRoot - Degree.GetFloat()/180) * (SquareRoot - Degree.GetFloat()/180);
			mid = AnchorValue.GetFloat();
			upper = (SquareRoot + level * Degree.GetFloat()/180) * (SquareRoot + level * Degree.GetFloat()/180);
			lower = (SquareRoot - level * Degree.GetFloat()/180) * (SquareRoot - level * Degree.GetFloat()/180);
		}
		else
		{
			upper = L1Plus[sc.Index-1];
			lower = L1Minus[sc.Index-1];
			mid = L1[sc.Index-1];
			BaseUpper = BaseUpperGraph[sc.Index-1];
			BaseLower = BaseLowerGraph[sc.Index-1];
			offset = Offset[sc.Index-1];
		}


		SCString TempStr;
		TempStr.Format("+%g deg", level * Degree.GetFloat());

		L1Plus.Name = TempStr;

		TempStr.Format("-%g deg", level * Degree.GetFloat());
		L1Minus.Name = TempStr;

		L1.Name = "Mid";

		float input = sc.Close[sc.Index];


		while (input > BaseUpper)
		{
			offset += Degree.GetFloat();
			BaseUpper = (SquareRoot + (offset + Degree.GetFloat())/180) * (SquareRoot + (offset + Degree.GetFloat())/180);
			BaseLower = (SquareRoot + (offset - Degree.GetFloat())/180) * (SquareRoot + (offset - Degree.GetFloat())/180);
			upper = (SquareRoot + (offset + level * Degree.GetFloat())/180) * (SquareRoot + (offset + level * Degree.GetFloat())/180);
			lower = (SquareRoot + (offset - level * Degree.GetFloat())/180) * (SquareRoot + (offset - level * Degree.GetFloat())/180);
			mid = (SquareRoot + offset/180) * (SquareRoot + offset/180);
		}
		while (input < BaseLower)
		{
			offset -= Degree.GetFloat();
			BaseUpper = (SquareRoot + (offset + Degree.GetFloat())/180) * (SquareRoot + (offset + Degree.GetFloat())/180);
			BaseLower = (SquareRoot + (offset - Degree.GetFloat())/180) * (SquareRoot + (offset - Degree.GetFloat())/180);
			upper = (SquareRoot + (offset + level * Degree.GetFloat())/180) * (SquareRoot + (offset + level * Degree.GetFloat())/180);
			lower = (SquareRoot + (offset - level * Degree.GetFloat())/180) * (SquareRoot + (offset - level * Degree.GetFloat())/180);
			mid = (SquareRoot + offset/180) * (SquareRoot + offset/180);
		}

		Anchor[sc.Index] = AnchorValue.GetFloat();
		L1Plus[sc.Index] = upper;
		L1Minus[sc.Index] = lower;
		L1[sc.Index] = mid;
		BaseUpperGraph[sc.Index] = BaseUpper;
		BaseLowerGraph[sc.Index] = BaseLower;
		Offset[sc.Index] = offset;

	}
	else
	{
		float L1, L2;
		SCString posdeg, negdeg;

		float squareRoot = sqrt(AnchorValue.GetFloat());
		float localDegree = Degree.GetFloat() * level;

		posdeg.Format("+%g deg", localDegree);
		negdeg.Format("-%g deg", localDegree);

		L1 = (squareRoot + localDegree/180) * (squareRoot + localDegree/180);
		L2 = (squareRoot - localDegree/180) * (squareRoot - localDegree/180);

		Anchor[sc.Index] = AnchorValue.GetFloat();
		L1Plus[sc.Index] = L1;
		L1Minus[sc.Index] = L2;
	}
}


/*==========================================================================*/
SCSFExport scsf_LinearRegressionEndChannel(SCStudyInterfaceRef sc)
{
	SCSubgraphRef LinearRegression = sc.Subgraph[0];
	SCSubgraphRef StdDevHigh = sc.Subgraph[1];
	SCSubgraphRef StdDevLow = sc.Subgraph[2];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Input_NumberOfBarsToCalculate = sc.Input[2];
	SCInputRef NumberDeviations = sc.Input[3];
	SCInputRef UseStartDateTime = sc.Input[4];
	SCInputRef StartDateTime = sc.Input[5];
	SCInputRef CalculateOnBarClose = sc.Input[6];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Linear Regression End Channel";

		sc.GraphRegion = 0;
		sc.ValueFormat =VALUEFORMAT_INHERITED;
		sc.AutoLoop = false;

		LinearRegression.Name = "Linear Regression";
		LinearRegression.DrawStyle = DRAWSTYLE_LINE;
		LinearRegression.PrimaryColor = RGB(0,255,0);
		LinearRegression.DrawZeros = false;

		StdDevHigh.Name = "StdDev High";
		LinearRegression.DrawStyle = DRAWSTYLE_LINE;
		LinearRegression.PrimaryColor = RGB(255,0,255);
		LinearRegression.DrawZeros = false;

		StdDevLow.Name = "StdDev Low";
		LinearRegression.DrawStyle = DRAWSTYLE_LINE;
		LinearRegression.PrimaryColor = RGB(255,255,0);
		LinearRegression.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Input_NumberOfBarsToCalculate.Name = "Number of Bars";
		Input_NumberOfBarsToCalculate.SetInt(10);
		Input_NumberOfBarsToCalculate.SetIntLimits(1,10000);

		NumberDeviations.Name = "Number of Deviations";
		NumberDeviations.SetFloat(2.0f);
		NumberDeviations.SetFloatLimits(0.001f, 2000);

		UseStartDateTime.Name = "Use Start Date-Time instead of Number of Bars";
		UseStartDateTime.SetYesNo(false);

		StartDateTime.Name = "Start Date and Time";
		StartDateTime.SetDateTime(0);

		CalculateOnBarClose.Name = "Calculate On Bar Close";
		CalculateOnBarClose.SetYesNo(false);

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	int NumberOfBarsToCalculate = Input_NumberOfBarsToCalculate.GetInt();
	int InputDataOffset = 0;

	if(CalculateOnBarClose.GetYesNo())
		InputDataOffset = 1;

	int ArraySize = sc.ArraySize;
	// Formula is  y= a+bx
	// a= (sumY-(b)sumX)/n
	// b= ((n)sumXY-sumX*sumY)/((n)sumXX-(sumY)(sumY))
	float sumY = 0;
	float sumX = 0;
	float sumXY = 0;
	float sumXX = 0;
	float StandardDev = 0;
	float tempY, a, b;
	int n = 0;	

	int StartIndex = 0;
	if (UseStartDateTime.GetYesNo()) //StartDateTime.GetDateTime()) // if start Date Time is specified, use that
	{
		NumberOfBarsToCalculate = 5000;

		StartIndex = sc.GetContainingIndexForSCDateTime(sc.ChartNumber, StartDateTime.GetDateTime());

		if (ArraySize - StartIndex > 5000)
			StartIndex = ArraySize - 5000;
	}
	else // otherwise, use last bars on chart
	{
		StartIndex = max(0, ArraySize - NumberOfBarsToCalculate - InputDataOffset);
	}

	for (int Index = StartIndex; Index < StartIndex + NumberOfBarsToCalculate && Index < ArraySize; Index++)
	{
		tempY = sc.BaseDataIn[InputData.GetInputDataIndex()][Index];
		n++;
		sumY += tempY;
		sumX += n;
		sumXY += (tempY*n);
		sumXX += n*n;
	}

	b = (n*sumXY - (sumX*sumY))/((n*sumXX) - (sumX*sumX));
	a = (sumY - (b*sumX))/n;

	// Solve StandardDev sqrt( sum((Xi-Xbar)^2)/n )
	float tempX = 0;
	for (int Index = StartIndex; Index < StartIndex + NumberOfBarsToCalculate && Index < ArraySize; Index++)
	{
		float diff = sc.BaseDataIn[InputData.GetInputDataIndex()][Index] - (a + b * (Index-StartIndex+1));
		tempX += (diff * diff);
	}

	StandardDev = sqrt(tempX/n);

	// Fill in the Linear Regression data
	for(int Index = 0; Index < n; Index++)
	{
		LinearRegression[StartIndex+Index] = a + b * (Index+1);
		StdDevHigh[StartIndex+Index] = LinearRegression[StartIndex+Index] + (StandardDev * NumberDeviations.GetFloat());
		StdDevLow[StartIndex+Index] = LinearRegression[StartIndex+Index] - (StandardDev * NumberDeviations.GetFloat());
	}

	if (!UseStartDateTime.GetYesNo())
	{
		// Zero out points that are not within the scope
		int nNewElements = sc.Open.GetArraySize() - 1 - sc.Index;
		for (int i = ArraySize - 1 - NumberOfBarsToCalculate - InputDataOffset; i >= 0 && nNewElements > 0; i--)
		{
			LinearRegression[i] = 0;
			StdDevHigh[i] = 0;
			StdDevLow[i] = 0;
			nNewElements--;
		}
	}
}

/*==========================================================================*/
SCSFExport scsf_PriceVolumeTrend(SCStudyInterfaceRef sc)
{	
	SCSubgraphRef PVT = sc.Subgraph[0];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Price Volume Trend";
		sc.GraphRegion = 1;

		PVT.Name = "PVT";
		PVT.DrawStyle = DRAWSTYLE_LINE;
		PVT.PrimaryColor = RGB(0,255,0);

		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;
		return;
	}
	sc.DataStartIndex = 1;
	sc.PriceVolumeTrend(sc.BaseDataIn, PVT);
}

/*==========================================================================*/
SCSFExport scsf_StandardDeviationBands(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TopBand = sc.Subgraph[0];
	SCSubgraphRef BottomBand = sc.Subgraph[1];
	SCSubgraphRef TopMovAvg = sc.Subgraph[2];
	SCSubgraphRef BottomMovAvg = sc.Subgraph[3];
	SCSubgraphRef Temp4 = sc.Subgraph[4];
	SCSubgraphRef Temp5 = sc.Subgraph[5];

	SCInputRef TopBandInputData = sc.Input[0];
	SCInputRef BottomBandInputData = sc.Input[1];
	SCInputRef Length = sc.Input[3];
	SCInputRef MultFactor = sc.Input[5];
	SCInputRef MovAvgType = sc.Input[6];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Standard Deviation Bands";

		sc.GraphRegion = 0;


		TopBand.Name = "Top";
		TopBand.DrawStyle = DRAWSTYLE_LINE;
		TopBand.PrimaryColor = RGB(0,255,0);


		BottomBand.Name = "Bottom";
		BottomBand.DrawStyle = DRAWSTYLE_LINE;
		BottomBand.PrimaryColor = RGB(0,255,0);

		TopMovAvg.Name = "Top MovAvg";
		TopMovAvg.DrawStyle = DRAWSTYLE_LINE;
		TopMovAvg.PrimaryColor = RGB(0,0,255);

		BottomMovAvg.Name = "Bottom MovAvg";
		BottomMovAvg.DrawStyle = DRAWSTYLE_LINE;
		BottomMovAvg.PrimaryColor = RGB(0,0,255);

		TopBandInputData.Name = "Top Band Input Data";
		TopBandInputData.SetInputDataIndex(SC_HIGH);

		BottomBandInputData.Name = "Bottom Band Input Data";
		BottomBandInputData.SetInputDataIndex(SC_LOW);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		MultFactor.Name = "Multiplication Factor";
		MultFactor.SetFloat(1.8f);
		MultFactor.SetFloatLimits(0.1f, 20.0f);

		MovAvgType.Name = "Moving Average Type";
		MovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		sc.AutoLoop = true;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	sc.DataStartIndex = Length.GetInt() - 1;
	int Index = sc.Index;
	SCFloatArrayRef HighArray = sc.BaseDataIn[TopBandInputData.GetInputDataIndex()];
	SCFloatArrayRef LowArray = sc.BaseDataIn[BottomBandInputData.GetInputDataIndex()];

	sc.MovingAverage(HighArray, TopMovAvg, MovAvgType.GetMovAvgType(), Index, Length.GetInt());
	sc.MovingAverage(LowArray, BottomMovAvg, MovAvgType.GetMovAvgType(), Index, Length.GetInt());

	sc.StdDeviation(HighArray, Temp4, Index, Length.GetInt());
	sc.StdDeviation(LowArray, Temp5, Index, Length.GetInt());

	TopBand[Index] = (Temp4[Index] * MultFactor.GetFloat()) + TopMovAvg[Index];
	BottomBand[Index] = (Temp5[Index] * -MultFactor.GetFloat()) +  BottomMovAvg[Index];
}

/*==========================================================================*/
SCSFExport scsf_HighestLowestBars(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Highest = sc.Subgraph[0];
	SCSubgraphRef Lowest = sc.Subgraph[1];

	SCInputRef HighestLength = sc.Input[0];
	SCInputRef LowestLength = sc.Input[1];
	SCInputRef InputData = sc.Input[2];

	if(sc.SetDefaults)
	{
		sc.GraphName = "NumberOfBarsSinceHighestValue / NumberOfBarsSinceLowestValue";
		sc.GraphRegion = 2;

		Highest.Name = "Highest";
		Highest.DrawStyle = DRAWSTYLE_LINE;
		Highest.PrimaryColor = RGB(0,255,0);

		Lowest.Name = "Lowest";
		Lowest.DrawStyle = DRAWSTYLE_LINE;
		Lowest.PrimaryColor = RGB(255,0,255);

		HighestLength.Name = "HighestLength";
		HighestLength.SetInt(10);
		HighestLength.SetIntLimits(2,MAX_STUDY_LENGTH);

		LowestLength.Name = "LowestLength";
		LowestLength.SetInt(10);
		LowestLength.SetIntLimits(2,MAX_STUDY_LENGTH);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	SCFloatArrayRef In = sc.BaseDataIn[InputData.GetInputDataIndex()];

	sc.DataStartIndex = max(HighestLength.GetInt(), LowestLength.GetInt()) - 1;

	Highest[sc.Index] = (float)sc.NumberOfBarsSinceHighestValue(In, sc.Index, HighestLength.GetInt());
	Lowest[sc.Index] = (float)sc.NumberOfBarsSinceLowestValue(In, sc.Index, LowestLength.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_VolumeUp(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Volume = sc.Subgraph[0];

	SCInputRef CompareType = sc.Input[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Volume-Up";

		sc.StudyDescription = "The volume for up bars only.";

		sc.AutoLoop = 1;

		sc.ValueFormat = 0;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_BAR;
		Volume.PrimaryColor = RGB(0,255,0);
		Volume.DrawZeros = false;

		CompareType.Name = "Compare Close with: 0=PrevClose, 1=Open";
		CompareType.SetInt(0);
		CompareType.SetIntLimits(0,1);

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	// Do data processing

	bool IsUpBar = false;
	switch (CompareType.GetInt())
	{
	case 0:
		if (sc.Index > 0)
			IsUpBar = sc.Close[sc.Index] > sc.Close[sc.Index - 1];
		break;

	case 1:
		IsUpBar = sc.Close[sc.Index] > sc.Open[sc.Index];
		break;
	}

	if (IsUpBar)
		Volume[sc.Index] = sc.Volume[sc.Index];
	else
		Volume[sc.Index] = 0;
}

/*==========================================================================*/
SCSFExport scsf_VolumeDown(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Volume = sc.Subgraph[0];

	SCInputRef CompareType = sc.Input[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Volume-Down";

		sc.StudyDescription = "The volume for down bars only.";

		sc.AutoLoop = 1;

		sc.ValueFormat = 0;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_BAR;
		Volume.PrimaryColor = RGB(0,255,0);
		Volume.DrawZeros = false;

		CompareType.Name = "Compare Close with: 0=PrevClose, 1=Open";
		CompareType.SetInt(0);
		CompareType.SetIntLimits(0,1);

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	// Do data processing

	bool IsDownBar = false;
	switch (CompareType.GetInt())
	{
	case 0:
		if (sc.Index > 0)
			IsDownBar = sc.Close[sc.Index] < sc.Close[sc.Index - 1];
		break;

	case 1:
		IsDownBar = sc.Close[sc.Index] < sc.Open[sc.Index];
		break;
	}

	if (IsDownBar)
		Volume[sc.Index] = sc.Volume[sc.Index];
	else
		Volume[sc.Index] = 0;
}

/*==========================================================================*/
SCSFExport scsf_BarEndTime(SCStudyInterfaceRef sc)
{
	SCSubgraphRef BET = sc.Subgraph[0];

	if(sc.SetDefaults)	
	{
		sc.GraphName = "Bar End Time";

		sc.GraphRegion = 0;
		sc.ValueFormat = 20;

		BET.Name = "BET";
		BET.DrawStyle = DRAWSTYLE_IGNORE;
		BET.PrimaryColor = RGB(0,255,0);
		BET.DrawZeros = false;

		sc.AutoLoop = 1;


		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	SCDateTime EndTime;
	EndTime.SetTime(sc.BaseDateTimeIn[sc.Index].GetTime()+sc.SecondsPerBar);
	BET[sc.Index] = (float)EndTime;
}

/*==========================================================================*/
SCSFExport scsf_CorrelationCoefficient(SCStudyInterfaceRef sc)
{
	SCSubgraphRef CC = sc.Subgraph[0];

	SCInputRef InputArray1 = sc.Input[0];
	SCInputRef InputArray2 = sc.Input[1];
	SCInputRef Length = sc.Input[2];

	if(sc.SetDefaults)	
	{
		sc.GraphName = "Correlation Coefficient";

		sc.GraphRegion = 1;

		CC.Name = "CC";
		CC.DrawStyle = DRAWSTYLE_LINE;
		CC.PrimaryColor = RGB(0,255,0);
		CC.DrawZeros = true;

		InputArray1.Name = "Input Array 1";
		InputArray1.SetStudySubgraphValues(0,0);

		InputArray2.Name = "Input Array 2";
		InputArray2.SetStudySubgraphValues(0,0);

		Length.Name = "Length";
		Length.SetInt(200);

		sc.AutoLoop = 1;
		sc.CalculationPrecedence = LOW_PREC_LEVEL;


		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}
	sc.DataStartIndex = Length.GetInt();
	SCFloatArray Array1;
	SCFloatArray Array2;

	sc.GetStudyArrayUsingID(InputArray1.GetStudyID(),InputArray1.GetSubgraphIndex(),Array1);
	if(Array1.GetArraySize() < sc.ArraySize)
		return;

	sc.GetStudyArrayUsingID(InputArray2.GetStudyID(),InputArray2.GetSubgraphIndex(),Array2);
	if(Array2.GetArraySize() < sc.ArraySize)
		return;

	CC[sc.Index] = sc.GetCorrelationCoefficient(Array1, Array2, Length.GetInt());

}


/*==========================================================================*/
SCSFExport scsf_SierraSqueeze(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MomentumHist = sc.Subgraph[0];
	SCSubgraphRef SqueezeDots = sc.Subgraph[1];
	SCSubgraphRef MomentumHistUpColors = sc.Subgraph[2];
	SCSubgraphRef MomentumHistDownColors = sc.Subgraph[3];
	SCSubgraphRef Temp4 = sc.Subgraph[4];
	//SCSubgraphRef Temp5 = sc.Subgraph[5];
	SCSubgraphRef SignalValues = sc.Subgraph[6];
	SCSubgraphRef Temp8 = sc.Subgraph[8];

	SCInputRef InputData = sc.Input[0];
	SCInputRef HistogramLenFirstData = sc.Input[1];
	SCInputRef SqueezeLength = sc.Input[2];
	SCInputRef NK = sc.Input[3];
	SCInputRef NB = sc.Input[4];
	SCInputRef FirstMovAvgType = sc.Input[5];
	SCInputRef HistogramLenSecondData = sc.Input[6];
	SCInputRef SecondMovAvgType = sc.Input[7];
	SCInputRef VersionUpdate = sc.Input[8];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Squeeze Indicator 2";
		sc.StudyDescription	= "Developed by the user Tony C.";

		MomentumHist.Name = "Momentum HISTOGRAM";
		MomentumHist.DrawStyle = DRAWSTYLE_BAR;
		MomentumHist.PrimaryColor = RGB(0,255,0);
		MomentumHist.LineWidth = 5;
		MomentumHist.DrawZeros = true;

		SqueezeDots.Name = "Squeeze Dots";
		SqueezeDots.DrawStyle = DRAWSTYLE_POINT;
		SqueezeDots.PrimaryColor = RGB(255,0,255);
		SqueezeDots.LineWidth = 4;
		SqueezeDots.DrawZeros = true;

		MomentumHistUpColors.Name = "Momentum HISTOGRAM Up Colors";
		MomentumHistUpColors.DrawStyle = DRAWSTYLE_IGNORE;
		MomentumHistUpColors.SecondaryColorUsed = 1;
		MomentumHistUpColors.PrimaryColor = RGB(0, 0, 255);
		MomentumHistUpColors.SecondaryColor = RGB(0, 0, 130);
		MomentumHistUpColors.DrawZeros = true;

		MomentumHistDownColors.Name	= "Momentum HISTOGRAM Down Colors";
		MomentumHistDownColors.DrawStyle = DRAWSTYLE_IGNORE;
		MomentumHistDownColors.SecondaryColorUsed = 1;
		MomentumHistDownColors.PrimaryColor = RGB(255, 0, 0);
		MomentumHistDownColors.SecondaryColor = RGB(130, 0, 0);
		MomentumHistDownColors.DrawZeros = true;

		SignalValues.Name = "Signal Values";
		SignalValues.DrawStyle = DRAWSTYLE_IGNORE;
		SignalValues.PrimaryColor = RGB(127,0,255);
		SignalValues.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST); // default data field		

		HistogramLenFirstData.Name = "Histogram Length First Data";
		HistogramLenFirstData.SetInt(20);
		HistogramLenFirstData.SetIntLimits(1,MAX_STUDY_LENGTH);

		SqueezeLength.Name = "Squeeze Length";
		SqueezeLength.SetFloat(20);

		NK.Name = "NK.GetFloat()";
		NK.SetFloat(1.5);

		NB.Name = "NB.GetFloat()";
		NB.SetFloat(2);

		FirstMovAvgType.Name = "First MA Type";
		FirstMovAvgType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);

		HistogramLenSecondData.Name = "Histogram Length Second Data";
		HistogramLenSecondData.SetInt(20);
		HistogramLenSecondData.SetIntLimits(1,MAX_STUDY_LENGTH);

		SecondMovAvgType.Name = "Second MA Type";
		SecondMovAvgType.SetMovAvgType(MOVAVGTYPE_LINEARREGRESSION);

		// hidden input for old versions support
		VersionUpdate.SetInt(1);

		sc.AutoLoop = 1;
		sc.FreeDLL = 0;

		return;
	}

	// upgrading the default settings
	if (VersionUpdate.GetInt() != 1)
	{
		MomentumHistUpColors.Name = "Momentum Histogram Up Colors";
		MomentumHistUpColors.DrawStyle = DRAWSTYLE_IGNORE;
		MomentumHistUpColors.SecondaryColorUsed	= 1;
		MomentumHistUpColors.PrimaryColor = RGB(0, 0, 255);
		MomentumHistUpColors.SecondaryColor = RGB(0, 0, 130);

		MomentumHistDownColors.Name = "Momentum Histogram Down Colors";
		MomentumHistDownColors.DrawStyle = DRAWSTYLE_IGNORE;
		MomentumHistDownColors.SecondaryColorUsed = 1;
		MomentumHistDownColors.PrimaryColor = RGB(255, 0, 0);
		MomentumHistDownColors.SecondaryColor = RGB(130, 0, 0);

		SignalValues.Name = "Signal Values";
		SignalValues.DrawStyle = DRAWSTYLE_IGNORE;

		VersionUpdate.SetInt(1);
	}

	// Inputs
	int i = sc.Index;
	const DWORD inside		= RGB(255, 0, 0);	
	const DWORD outside		= RGB(0, 255, 0);	

	MomentumHistUpColors[i] = 0;
	MomentumHistDownColors[i] = 0;

	// First output elements are not valid
	sc.DataStartIndex = HistogramLenSecondData.GetInt();

	SCFloatArrayRef close =  sc.Close;
	sc.ExponentialMovAvg(close, MomentumHistUpColors, HistogramLenSecondData.GetInt());  // Note: EMA returns close when index is < HistogramLenSecondData.GetInt()
	sc.MovingAverage(close,  MomentumHistUpColors, FirstMovAvgType.GetMovAvgType(), HistogramLenFirstData.GetInt());

	float hlh = sc.GetHighest(sc.High, HistogramLenSecondData.GetInt());
	float lll = sc.GetLowest(sc.Low, HistogramLenSecondData.GetInt());

	SCFloatArrayRef price = sc.Open;

	MomentumHistDownColors[sc.Index] = price[sc.Index] - ((hlh + lll)/2.0f + MomentumHistUpColors[sc.Index])/2.0f;
	sc.LinearRegressionIndicator(MomentumHistDownColors, MomentumHist, HistogramLenSecondData.GetInt());
	sc.MovingAverage(close,  MomentumHistUpColors, SecondMovAvgType.GetMovAvgType(), HistogramLenSecondData.GetInt());


	if(
		(MomentumHist[i]<0) 
		&&(MomentumHist[i] < MomentumHist[i-1])
		)

	{
		MomentumHist.DataColor[sc.Index] = MomentumHistDownColors.PrimaryColor;		
	}
	else if(
		(MomentumHist[i]<=0) 
		&&(MomentumHist[i] > MomentumHist[i-1])
		)
	{
		MomentumHist.DataColor[sc.Index] = MomentumHistDownColors.SecondaryColor;		
	}
	else if(
		(MomentumHist[i]>0) 
		&&(MomentumHist[i] > MomentumHist[i-1])
		)
	{
		MomentumHist.DataColor[sc.Index] = MomentumHistUpColors.PrimaryColor;		
	}
	else if(
		(MomentumHist[i]>=0) 
		&&(MomentumHist[i] < MomentumHist[i-1])
		)
	{
		MomentumHist.DataColor[sc.Index] = MomentumHistUpColors.SecondaryColor;		
	}


	//Squeeze
	sc.Keltner(
		sc.BaseDataIn,
		sc.Close,
		Temp8,
		SqueezeLength.GetInt(),
		MOVAVGTYPE_SMOOTHED,
		SqueezeLength.GetInt(),
		MOVAVGTYPE_SMOOTHED,
		NK.GetFloat(),
		NK.GetFloat()
		);

	float TopBandOut = Temp8.Arrays[0][sc.Index];
	float BottomBandOut = Temp8.Arrays[1][sc.Index];

	sc.BollingerBands(sc.Close, Temp4, SqueezeLength.GetInt(), NB.GetFloat(), MOVAVGTYPE_SMOOTHED);

	float BU =Temp4.Arrays[0][sc.Index];
	float BL =Temp4.Arrays[1][sc.Index];

	if (
		(BU < TopBandOut)
		|| (BL > BottomBandOut)
		)
	{
		SqueezeDots[sc.Index] = 0.0;
		SqueezeDots.DataColor[sc.Index] = inside;		
		SignalValues[sc.Index] = 0.0;
		SignalValues.DataColor[sc.Index] = inside;
	}
	else
	{
		SqueezeDots[sc.Index] = 0.0;
		SqueezeDots.DataColor[sc.Index] = outside;		
		SignalValues[sc.Index] = 1.0;
		SignalValues.DataColor[sc.Index] = outside;
	}
}

/**************************************************************/
#define GREEN RGB(0,255,0)
#define DKGREEN RGB(0,128,0)
#define YELLOW RGB(255,255,0)
#define LTYELLOW RGB(255,255,128)
#define RED RGB(255,0,0)
#define DKRED RGB(198,0,0)
#define BLACK RGB(0,0,1)
#define WHITE RGB(255,255,255)
#define CYAN RGB(0,255,255)
#define PURPLE RGB(255,0,255)
#define GREY RGB(192,192,192)
#define BLUE RGB(0,128,255)
/**************************************************************/
SCSFExport scsf_R_Squared(SCStudyInterfaceRef sc)
{
	SCSubgraphRef RSquared = sc.Subgraph[0];

	SCInputRef Length = sc.Input[0];
	SCInputRef InputData = sc.Input[1];

	if(sc.SetDefaults)
	{
		sc.GraphName="R Squared";
		sc.StudyDescription="R Squared Indicator by ertrader.";


		sc.AutoLoop = 1;
		sc.GraphRegion = 1;
		sc.FreeDLL=0;

		RSquared.Name="R Squared";
		RSquared.DrawStyle = DRAWSTYLE_LINE;
		RSquared.LineWidth = 2;
		RSquared.PrimaryColor = GREEN;
		RSquared.SecondaryColor = RED;
		RSquared.DrawZeros = false;

		Length.Name="Length";
		Length.SetInt(14);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);        
		return;
	}

	// Start plotting after good output elements are valid
	sc.DataStartIndex = Length.GetInt();

	// R^2 indicator:  Pwr(Corr(Cum( 1 ),C,14,0),2)  or Correlation_Coefficient^2.  This method calculates R^ with minimal lag.
	SCFloatArrayRef PriceData = sc.BaseData[InputData.GetInputDataIndex()];

	//The following is a cumulative calculation.  It sums up a count of 1 for each bar.  It is then used to correlate data against    
	RSquared.Arrays[0][sc.Index] = (float)(sc.Index + 1);

	//Get the correlation coefficient. Correlate Price Data against the cumulation
	float coef = sc.GetCorrelationCoefficient(RSquared.Arrays[0], PriceData, sc.Index, Length.GetInt());

	// Calculate and Draw R squared
	//Pwr(Corr(Cum( 1 ),C,14,0),2) or Correlation_Coefficient^2
	RSquared[sc.Index]=coef*coef; 
}  


/*==========================================================================*/
//http://en.wikipedia.org/wiki/Exponential_function
SCSFExport scsf_RSIInverseFisherTransform(SCStudyInterfaceRef sc)
{
	SCSubgraphRef RSI_IFT = sc.Subgraph[0];
	SCSubgraphRef Line1 = sc.Subgraph[1];
	SCSubgraphRef Line2 = sc.Subgraph[2];
	SCSubgraphRef RSI = sc.Subgraph[3];

	SCInputRef RSILength = sc.Input[0];
	SCInputRef RSIInternalMovAvgType = sc.Input[1];
	SCInputRef RSIMovAvgType = sc.Input[2];
	SCInputRef RSIMovAvgLength = sc.Input[3];
	SCInputRef LineValue = sc.Input[4];

	if(sc.SetDefaults)
	{
		sc.GraphName="Inverse Fisher Transform RSI";
		sc.StudyDescription="Inverse Fisher Transform RSI by John Ehlers.";


		sc.AutoLoop = 1;
		sc.GraphRegion = 1;
		sc.FreeDLL=0;

		RSI_IFT.Name="RSI IFT";
		RSI_IFT.DrawStyle = DRAWSTYLE_LINE;
		RSI_IFT.LineWidth = 2;
		RSI_IFT.PrimaryColor = GREEN;
		RSI_IFT.DrawZeros = true;

		Line1.Name="Line 1";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.LineWidth = 1;
		Line1.PrimaryColor = RED;
		Line1.DrawZeros = true;

		Line2.Name="Line 2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.LineWidth = 1;
		Line2.PrimaryColor = BLUE;
		Line2.DrawZeros = true;

		RSILength.Name="RSI Length";
		RSILength.SetInt(5);
		RSILength.SetIntLimits(1,MAX_STUDY_LENGTH);

		RSIInternalMovAvgType.Name="RSI Internal MovAvg Type";
		RSIInternalMovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		RSIMovAvgType.Name="RSI MovAvg Type";
		RSIMovAvgType.SetMovAvgType(MOVAVGTYPE_WEIGHTED);

		RSIMovAvgLength.Name="RSI MovAvg Length";
		RSIMovAvgLength.SetInt(9);
		RSIMovAvgLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		LineValue.Name="Line Value";
		LineValue.SetFloat(0.5);

		return;
	}


	sc.DataStartIndex = RSILength.GetInt()+RSIMovAvgLength.GetInt();

	sc.RSI(sc.Close,RSI,RSIInternalMovAvgType.GetMovAvgType(),RSILength.GetInt());

	//put RSI calculation into an Array to be used with the moving average, since an array is required.
	RSI_IFT.Arrays[0][sc.Index] = (float)((RSI[sc.Index]-50)*0.1);

	sc.MovingAverage(RSI_IFT.Arrays[0],RSI_IFT.Arrays[1],RSIMovAvgType.GetMovAvgType(),RSIMovAvgLength.GetInt());

	float WMA =  RSI_IFT.Arrays[1][sc.Index];

	RSI_IFT[sc.Index] = (exp(2*WMA)-1)/(exp(2*WMA)+1);

	Line1[sc.Index] = LineValue.GetFloat();
	Line2[sc.Index] = -LineValue.GetFloat();
}

/*==========================================================================*/

SCSFExport scsf_InverseFisherTransform(SCStudyInterfaceRef sc)
{
	SCSubgraphRef IFT = sc.Subgraph[0];
	SCSubgraphRef Line1 = sc.Subgraph[1];
	SCSubgraphRef Line2 = sc.Subgraph[2];
	SCSubgraphRef IFTAvg = sc.Subgraph[3];

	SCInputRef InputData = sc.Input[0];
	SCInputRef LineValue = sc.Input[1];
	SCInputRef OutputMovAvgLength = sc.Input[2];
	SCInputRef OutputMovAvgType = sc.Input[3];
	SCInputRef InputMovAvgLength = sc.Input[4];
	SCInputRef InputMovAvgType = sc.Input[5];
	SCInputRef HighestLowestLength = sc.Input[6];

	if(sc.SetDefaults)
	{
		sc.GraphName="Inverse Fisher Transform";
		sc.StudyDescription="Inverse Fisher Transform.";


		sc.AutoLoop = 1;
		sc.GraphRegion = 1;
		sc.FreeDLL=0;

		IFT.Name="IFT";
		IFT.DrawStyle = DRAWSTYLE_LINE;
		IFT.LineWidth = 2;
		IFT.PrimaryColor = GREEN;
		IFT.DrawZeros = true;

		Line1.Name="Line 1";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.LineWidth = 1;
		Line1.PrimaryColor = RED;
		Line1.DrawZeros = true;

		Line2.Name="Line 2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.LineWidth = 1;
		Line2.PrimaryColor = BLUE;
		Line2.DrawZeros = true;

		IFTAvg.Name="IFT Avg";
		IFTAvg.DrawStyle = DRAWSTYLE_LINE;
		IFTAvg.LineWidth = 1;
		IFTAvg.PrimaryColor = RGB(255,255,0);
		IFTAvg.DrawZeros = true;

		InputData.Name="Input Data";
		InputData.SetInputDataIndex(0);

		LineValue.Name="Line Value";
		LineValue.SetFloat(0.5);

		OutputMovAvgLength.Name="Output Mov Avg Length";
		OutputMovAvgLength.SetInt(3);

		OutputMovAvgType.Name="Output Mov Avg Type";
		OutputMovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		InputMovAvgLength.Name="Input Data Mov Avg Length";
		InputMovAvgLength.SetInt(9);

		InputMovAvgType.Name="Input Data Mov Avg Type";
		InputMovAvgType.SetMovAvgType(MOVAVGTYPE_WEIGHTED);

		HighestLowestLength.Name = "Highest Value/Lowest Value Length";
		HighestLowestLength.SetInt(10);

		return;
	}

	sc.DataStartIndex = InputMovAvgLength.GetInt()+OutputMovAvgLength.GetInt();

	float Highest = sc. GetHighest(sc.BaseData[ InputData.GetInputDataIndex() ], HighestLowestLength.GetInt() );
	float Lowest = sc. GetLowest(sc.BaseData[ InputData.GetInputDataIndex() ], HighestLowestLength.GetInt() );

	float Ratio = 10.0f / (Highest - Lowest);
	IFT.Arrays[1][sc.Index] = ((sc.BaseData[InputData.GetInputDataIndex()][sc.Index] - Lowest) * Ratio) - 5.0f;

	sc.MovingAverage(IFT.Arrays[1],IFT.Arrays[0],InputMovAvgType.GetMovAvgType(),InputMovAvgLength.GetInt());

	float RefData = IFT.Arrays[0][sc.Index];
	IFT[sc.Index] = (exp(2*RefData)-1)/(exp(2*RefData)+1);//http://en.wikipedia.org/wiki/Exponential_function

	sc.MovingAverage(IFT,IFTAvg,OutputMovAvgType.GetMovAvgType(),OutputMovAvgLength.GetInt());

	Line1[sc.Index] = LineValue.GetFloat();
	Line2[sc.Index] = -LineValue.GetFloat();
}

/*============================================================================
Sine-Wave Weighted Moving Average
----------------------------------------------------------------------------*/
SCSFExport scsf_SineWaveWMA(SCStudyInterfaceRef sc)
{
	SCSubgraphRef WMA = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];

	if (sc.SetDefaults)
	{
		sc.GraphName="Sine Wave Weighted Mov Avg";


		sc.AutoLoop = 1;
		sc.GraphRegion = 0;
		sc.FreeDLL=0;

		WMA.Name="SWA";
		WMA.DrawStyle = DRAWSTYLE_LINE;
		WMA.LineWidth = 1;
		WMA.PrimaryColor = RGB(0,255,0);
		WMA.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		return;
	}

	//PI:=3.1415926;
	//SD:=180/6;
	//S1:=Sin(1*180/6)*C;
	//S2:=Sin(2*180/6)*Ref(C,-1);
	//S3:=Sin(3*180/6)*Ref(C,-2);
	//S4:=Sin(4*180/6)*Ref(C,-3);
	//S5:=Sin(5*180/6)*Ref(C,-4);
	//Num:=S1+S2+S3+S4+S5;
	//Den:=Sin(SD)+Sin(2*SD)+Sin(3*SD)+Sin(4*SD)+Sin(5*SD);
	//Num/Den

	sc.DataStartIndex = 1;

	if(InputData.GetInputDataIndex() == SC_OPEN)
		InputData.SetInputDataIndex(SC_LAST);

	int InputDataIndex = InputData.GetInputDataIndex();

	const float SD = (float)(M_PI/6);  // Note: the sin function takes radians, not degrees
	float S1 = sin(1*SD) * sc.BaseData[InputDataIndex][sc.Index - 0];
	float S2 = sin(2*SD) * sc.BaseData[InputDataIndex][sc.Index - 1];
	float S3 = sin(3*SD) * sc.BaseData[InputDataIndex][sc.Index - 2];
	float S4 = sin(4*SD) * sc.BaseData[InputDataIndex][sc.Index - 3];
	float S5 = sin(5*SD) * sc.BaseData[InputDataIndex][sc.Index - 4];

	float Num = S1 + S2 + S3 + S4 + S5;
	float Den = sin(1*SD) + sin(2*SD) + sin(3*SD) + sin(4*SD) + sin(5*SD);

	WMA[sc.Index] = Num / Den;
}

/*==========================================================================*/
/*
Bollinger bands squeeze.

Proportion = (kUpper - kLower) / (bbUpper - bbLower);

*/
SCSFExport scsf_BollingerSqueeze(SCStudyInterfaceRef sc)
{
	SCSubgraphRef BandsRatio = sc.Subgraph[0];
	SCSubgraphRef SqueezeIndicator = sc.Subgraph[1];
	SCSubgraphRef Keltner = sc.Subgraph[2];
	SCSubgraphRef BollingerBands = sc.Subgraph[3];

	SCInputRef InputData = sc.Input[0];
	SCInputRef InternalMovAvgType = sc.Input[1];
	SCInputRef KeltnerBandsLength = sc.Input[2];
	SCInputRef KeltnerTrueRangeMALength = sc.Input[3];
	SCInputRef KeltnerBandsMultiplier = sc.Input[4];
	SCInputRef BollingerBandsLength = sc.Input[5];
	SCInputRef BollingerBandsMultiplier = sc.Input[6];

	if (sc.SetDefaults)
	{
		sc.GraphName="Bollinger Squeeze";
		sc.StudyDescription="Bollinger Squeeze";

		sc.AutoLoop = 1;
		sc.GraphRegion = 1;


		BandsRatio.Name = "Bands Ratio";
		BandsRatio.DrawStyle = DRAWSTYLE_BAR;
		BandsRatio.PrimaryColor = RGB(0,255,0);
		BandsRatio.SecondaryColor = RGB(255,0,0);
		BandsRatio.SecondaryColorUsed = true;
		BandsRatio.DrawZeros = true;

		SqueezeIndicator.Name = "Squeeze Indicator";
		SqueezeIndicator.DrawStyle = DRAWSTYLE_POINT;
		SqueezeIndicator.LineWidth = 3;
		SqueezeIndicator.PrimaryColor = RGB(0,255,0);
		SqueezeIndicator.SecondaryColor = RGB(255,0,0);
		SqueezeIndicator.SecondaryColorUsed = true;
		SqueezeIndicator.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		InternalMovAvgType.Name="Moving Average Type for Internal Calculations";
		InternalMovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		KeltnerBandsLength.Name="Keltner Bands Length";
		KeltnerBandsLength.SetInt(20);
		KeltnerBandsLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		KeltnerTrueRangeMALength.Name = "Keltner True Range MovAvg Length";
		KeltnerTrueRangeMALength.SetInt(20);
		KeltnerTrueRangeMALength.SetIntLimits(1,MAX_STUDY_LENGTH);

		KeltnerBandsMultiplier.Name="Keltner Bands Multiplier";
		KeltnerBandsMultiplier.SetFloat(2.0f);

		BollingerBandsLength.Name="Bollinger Bands Length";
		BollingerBandsLength.SetInt(20);
		BollingerBandsLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		BollingerBandsMultiplier.Name="Bollinger Bands Multiplier";
		BollingerBandsMultiplier.SetFloat(2.0f);

		return;
	}

	if ((sc.CurrentIndex < KeltnerBandsLength.GetInt()) || (sc.CurrentIndex < BollingerBandsLength.GetInt()))
		return;

	// calculate Bollinger Bands
	sc.BollingerBands(sc.BaseData[InputData.GetInputDataIndex()],BollingerBands,BollingerBandsLength.GetInt(),BollingerBandsMultiplier.GetFloat(),InternalMovAvgType.GetMovAvgType());

	// calculate Keltner
	sc.Keltner( sc.BaseData,sc.BaseData[InputData.GetInputDataIndex()],Keltner,KeltnerBandsLength.GetInt(), InternalMovAvgType.GetMovAvgType(),BollingerBandsLength.GetInt(),InternalMovAvgType.GetMovAvgType(), KeltnerBandsMultiplier.GetFloat(),KeltnerBandsMultiplier.GetFloat());


	float KUp = Keltner.Arrays[0][sc.CurrentIndex];
	float KDown = Keltner.Arrays[1][sc.CurrentIndex];

	float UBB = BollingerBands.Arrays[0][sc.CurrentIndex];
	float LBB = BollingerBands.Arrays[1][sc.CurrentIndex];

	if ((UBB > KUp) && (LBB < KDown)) 
		SqueezeIndicator.DataColor[sc.CurrentIndex] = SqueezeIndicator.PrimaryColor;    
	else
		SqueezeIndicator.DataColor[sc.CurrentIndex] = SqueezeIndicator.SecondaryColor;

	BandsRatio[sc.CurrentIndex] = (KUp-KDown)/(UBB - LBB) - 1.0f;

	if (BandsRatio[sc.CurrentIndex] >= 0)
		BandsRatio.DataColor[sc.CurrentIndex] = BandsRatio.PrimaryColor;
	else
		BandsRatio.DataColor[sc.CurrentIndex] = BandsRatio.SecondaryColor;
}

/*==========================================================================*/
SCSFExport scsf_StudySubgraphsDifference(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Diff = sc.Subgraph[0];

	SCInputRef OldStudy1 = sc.Input[0];
	SCInputRef OldStudy1Subgraph = sc.Input[1];
	SCInputRef OldStudy2 = sc.Input[2];
	SCInputRef OldStudy2Subgraph = sc.Input[3];
	SCInputRef OldStudy2SubgraphOffset = sc.Input[4];

	SCInputRef StudySugbraph1 = sc.Input[5];
	SCInputRef StudySugbraph2 = sc.Input[6];
	SCInputRef Study2SubgraphOffset = sc.Input[7];
	SCInputRef VersionUpdate = sc.Input[8];


	SCInputRef DrawZeros = sc.Input[9];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraphs Difference";
		sc.StudyDescription = "Calculates the difference between the two selected study subgraphs.";

		sc.AutoLoop = 1;
		sc.CalculationPrecedence = LOW_PREC_LEVEL;
		//sc.CalculationPrecedence = VERY_LOW_PREC_LEVEL;

		sc.FreeDLL=0; 

		Diff.Name = "Diff";
		Diff.DrawStyle = DRAWSTYLE_LINE;
		Diff.PrimaryColor = RGB(0,255,0);
		Diff.LineWidth = 1;

		StudySugbraph1.Name = "Input Study 1";
		StudySugbraph1.SetStudySubgraphValues(0, 0);

		StudySugbraph2.Name = "Input Study 2";
		StudySugbraph2.SetStudySubgraphValues(0, 0);

		Study2SubgraphOffset.Name = "Study 2 Subgraph Offset";// This is always converted to a positive value and is the number of bars back
		Study2SubgraphOffset.SetInt(0);


		DrawZeros.Name = "Draw Zeros";
		DrawZeros.SetYesNo(false);


		VersionUpdate.SetYesNo(1);

		return;
	}

	if (VersionUpdate.GetYesNo() != 1)
	{
		StudySugbraph1.SetStudySubgraphValues(OldStudy1.GetStudyID(), OldStudy1Subgraph.GetSubgraphIndex());
		StudySugbraph2.SetStudySubgraphValues(OldStudy2.GetStudyID(), OldStudy2Subgraph.GetSubgraphIndex());
		Study2SubgraphOffset.SetInt(OldStudy2SubgraphOffset.GetInt());
		VersionUpdate.SetYesNo(1);
	}

	Diff.DrawZeros= DrawZeros. GetYesNo();

	SCFloatArray Study1Array;
	sc.GetStudyArrayUsingID(StudySugbraph1.GetStudyID(),StudySugbraph1.GetSubgraphIndex(),Study1Array);

	SCFloatArray Study2Array;
	sc.GetStudyArrayUsingID(StudySugbraph2.GetStudyID(),StudySugbraph2.GetSubgraphIndex(),Study2Array);


	Diff[sc.Index] = Study1Array[sc.Index] - Study2Array[sc.Index - labs (Study2SubgraphOffset.GetInt() ) ];
}

/*==========================================================================*/
SCSFExport scsf_LRS(SCStudyInterfaceRef sc)
{
	SCSubgraphRef LRS = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Linear Regressive Slope";
		
		sc.AutoLoop = 1;
		sc.FreeDLL=0; 

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		LRS.Name = "LRS";
		LRS.DrawStyle = DRAWSTYLE_LINE;
		LRS.PrimaryColor = RGB(0,255,0);
		LRS.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = Length.GetInt();

	int Len=Length.GetInt();
	double SumBars = Len*(Len-1)/2.0;
	double SumSqrBars = (Len-1)*Len*(2*Len-1)/6.0;

	double Sum1       = 0;
	double SumY=0;
	for(int Index=sc.Index; Index > (sc.Index - Len); Index--)
	{
		int x =sc.Index-Index;
		Sum1= Sum1 + x*sc.BaseData[InputData.GetInputDataIndex()][Index];
		SumY += sc.BaseData[InputData.GetInputDataIndex()][Index];
	}

	double Sum2 = SumBars * SumY;
	double Num1 = Len*Sum1 - Sum2;
	double Num2 = SumBars*SumBars - Len*SumSqrBars;
	LRS[sc.Index] = (float)(Num1 / Num2);


	return;
}

/*==========================================================================*/
SCSFExport scsf_SummationPeriodic(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Sum = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Summation - Periodic";
		sc.StudyDescription = "The cumulative sum over the number of elements specified by the Length input.";


		sc.AutoLoop = 1;
		sc.FreeDLL = 0; 

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		Sum.Name = "Sum";
		Sum.DrawStyle = DRAWSTYLE_LINE;
		Sum.PrimaryColor = RGB(0,255,0);
		Sum.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.Summation(sc.BaseData[InputData.GetInputDataIndex()], Sum, Length.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_SummationOfStudySubgraphPeriodic(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Sum = sc.Subgraph[0];

	SCInputRef Length = sc.Input[0];
	SCInputRef StudySubgraphReference = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Summation of Study Subgraph - Periodic";

		sc.AutoLoop = 1;
		sc.FreeDLL = 0; 

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		Sum.Name = "Sum";
		Sum.DrawStyle = DRAWSTYLE_LINE;
		Sum.PrimaryColor = RGB(0, 255, 0);
		Sum.DrawZeros = false;

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		StudySubgraphReference.Name = "Study and Subgraph To Reference";
		StudySubgraphReference.SetStudySubgraphValues(1,0);

		return;
	}

	SCFloatArray StudyArray;
	sc.GetStudyArrayUsingID(StudySubgraphReference.GetStudyID(), StudySubgraphReference.GetSubgraphIndex(), StudyArray);
	if(StudyArray.GetArraySize() == 0)
	{
		SCString Message;
		Message = sc.GraphName;
		Message += " Study being referenced does not exist.";
		sc.AddMessageToLog(Message, 1);
		return;
	}

	sc.DataStartIndex = sc.GetStudyDataStartIndexUsingID(StudySubgraphReference.GetStudyID());

	sc.Summation(StudyArray, Sum, Length.GetInt());
}
/***********************************************************************************/
SCSFExport scsf_PeriodOHLCVariablePeriod(SCStudyInterfaceRef sc)
{
	SCInputRef TimePeriodType = sc.Input[0];
	SCInputRef TimePeriodLength = sc.Input[1];
	SCInputRef UseCurrentPeriod = sc.Input[2];
	SCInputRef MinimumRequiredTP = sc.Input[3];
	SCInputRef DisplayDebuggingOutput = sc.Input[5];
	SCInputRef ForwardProjectLines = sc.Input[6];
	SCInputRef NumberOfDaysToCalculate = sc.Input[7];

	SCInputRef AutoSkipPeriodOfNoTrading = sc.Input[8];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Period OHLC-Variable Period";
		sc.AutoLoop = 0;

		sc.ScaleRangeType = SCALE_SAMEASREGION;
		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;

		sc.Subgraph[SC_OPEN].Name = "Open";
		sc.Subgraph[SC_OPEN].DrawStyle = DRAWSTYLE_DASH;
		sc.Subgraph[SC_OPEN].PrimaryColor = RGB(0,255,0);
		sc.Subgraph[SC_OPEN].LineWidth = 2;
		sc.Subgraph[SC_OPEN].DrawZeros = false;

		sc.Subgraph[SC_HIGH].Name = "High";
		sc.Subgraph[SC_HIGH].DrawStyle = DRAWSTYLE_DASH;
		sc.Subgraph[SC_HIGH].PrimaryColor = RGB(255,0,255);
		sc.Subgraph[SC_HIGH].LineWidth = 2;
		sc.Subgraph[SC_HIGH].DrawZeros = false;

		sc.Subgraph[SC_LOW].Name = "Low";
		sc.Subgraph[SC_LOW].DrawStyle = DRAWSTYLE_DASH;
		sc.Subgraph[SC_LOW].PrimaryColor = RGB(255,255,0);
		sc.Subgraph[SC_LOW].LineWidth = 2;
		sc.Subgraph[SC_LOW].DrawZeros = false;

		sc.Subgraph[SC_LAST].Name = "Close";
		sc.Subgraph[SC_LAST].DrawStyle = DRAWSTYLE_DASH;
		sc.Subgraph[SC_LAST].PrimaryColor = RGB(255,127,0);
		sc.Subgraph[SC_LAST].LineWidth = 2;
		sc.Subgraph[SC_LAST].DrawZeros = false;

		sc.Subgraph[SC_OHLC].Name = "OHLC Avg";
		sc.Subgraph[SC_OHLC].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[SC_OHLC].PrimaryColor = RGB(128,128,128);
		sc.Subgraph[SC_OHLC].DrawZeros = false;

		sc.Subgraph[SC_HLC] .Name= "HLC Avg";
		sc.Subgraph[SC_HLC].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[SC_HLC].PrimaryColor = RGB(128,128,128);
		sc.Subgraph[SC_HLC].DrawZeros = false;

		sc.Subgraph[SC_HL] .Name= "HL Avg";
		sc.Subgraph[SC_HL].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[SC_HL].PrimaryColor = RGB(128,128,128);
		sc.Subgraph[SC_HL].DrawZeros = false;

		for (int i = SC_OPEN; i <= SC_HL; i++)
		{
			sc.Subgraph[i].LineLabel = 
				LL_DISPLAY_NAME  | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_FAR_RIGHT | 
				LL_DISPLAY_VALUE | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
		}

		TimePeriodType.Name = "Time Period Type";
		TimePeriodType.SetTimePeriodType(TIME_PERIOD_LENGTH_UNIT_MINUTES);

		TimePeriodLength.Name = "Time Period Length";
		TimePeriodLength.SetInt(60);
		TimePeriodLength.SetIntLimits(1, 7*MINUTES_PER_DAY);

		UseCurrentPeriod.Name = "Use Current Period";
		UseCurrentPeriod.SetYesNo(false);


		MinimumRequiredTP.Name = "Minimum Required Time Period as %";
		MinimumRequiredTP.SetFloat(5.0f);
		MinimumRequiredTP.SetFloatLimits(1.0f, 100.0f);

		DisplayDebuggingOutput.Name = "Display Debugging Output (slows study calculations)";
		DisplayDebuggingOutput.SetYesNo(0);

		ForwardProjectLines.Name ="Forward Project OHLC Lines";
		ForwardProjectLines.SetYesNo(0);

		NumberOfDaysToCalculate.Name = "Number Of Days To Calculate";
		NumberOfDaysToCalculate.SetInt(400);

		AutoSkipPeriodOfNoTrading.Name = "Auto Skip Period Of No Trading";
		AutoSkipPeriodOfNoTrading.SetYesNo(false);

		return;
	}


	int PeriodLength = TimePeriodLength.GetInt();
	SCDateTime FirstBarDateTime = sc.BaseDateTimeIn[0];

	SCDateTime ChartStartTime = sc.StartTime1;
	if (sc.UseSecondStartEndTimes)
		ChartStartTime = sc.StartTime2;


	int NumberOfForwardBars = 0;

	if (ForwardProjectLines.GetYesNo())
	{
		NumberOfForwardBars = 10;

		if(sc.UpdateStartIndex == 0)
		{
			for (int i = SC_OPEN; i <= SC_HL; i++)
				sc.Subgraph[i].ExtendedArrayElementsToGraph = NumberOfForwardBars;
		}
	}

	if (NumberOfDaysToCalculate.GetInt() < 1)
		NumberOfDaysToCalculate.SetInt(1000);

	int LastDateInChart = sc.BaseDateTimeIn[sc.ArraySize - 1].GetDate();
	int FirstDateToCalculate = LastDateInChart - NumberOfDaysToCalculate.GetInt() + 1;

	float Open = 0, High = 0, Low = 0, Close = 0, NextOpen = 0;

	SCDateTime BeginOfRefDateTime,EndOfRefDateTime;
	SCDateTime CurrentPeriodBeginDateTime;
	SCDateTime CurrentPeriodEndDateTime;

	SCDateTime PriorCurrentPeriodStartDateTime;

	for (int index = sc.UpdateStartIndex; index < sc.ArraySize+NumberOfForwardBars; index++ )
	{

		SCDateTime CurrentBarDT = sc.BaseDateTimeIn[index];


		if (CurrentBarDT < FirstDateToCalculate)
			continue;

		bool GetReferenceData = true;

		CurrentPeriodBeginDateTime = sc.GetStartOfPeriodForDateTime(CurrentBarDT, TimePeriodType.GetTimePeriodType(),  PeriodLength, 0);

		if(	CurrentPeriodBeginDateTime == PriorCurrentPeriodStartDateTime)
			GetReferenceData  = false;


		PriorCurrentPeriodStartDateTime =	CurrentPeriodBeginDateTime;

		SCDateTime TimeIncrement = sc.TimePeriodSpan(TimePeriodType.GetTimePeriodType(),  PeriodLength);

		//If there are insufficient bars for a reference period, then we will walk back 1 block/period at a time. 

		if(GetReferenceData)
		{
			BeginOfRefDateTime= sc.GetStartOfPeriodForDateTime( 	CurrentPeriodBeginDateTime, TimePeriodType.GetTimePeriodType(), PeriodLength, -1);
			EndOfRefDateTime=  	CurrentPeriodBeginDateTime - (1*SECONDS);

			CurrentPeriodEndDateTime=sc.GetStartOfPeriodForDateTime( 	CurrentPeriodBeginDateTime, TimePeriodType.GetTimePeriodType(),  PeriodLength, 1) - (1*SECONDS);

			if (DisplayDebuggingOutput.GetYesNo() != 0)
			{
				SCString Message;

				Message.Format("Current Bar: %s, BeginOfRefDateTime: %s, EndOfRefDateTime: %s, CurrentPeriodBeginDateTime: %s, CurrentPeriodEndDateTime: %s",
					sc.FormatDateTime(CurrentBarDT).GetChars(),sc.FormatDateTime(BeginOfRefDateTime).GetChars(),
					sc.FormatDateTime(EndOfRefDateTime).GetChars(),sc.FormatDateTime(CurrentPeriodBeginDateTime).GetChars(),
					sc.FormatDateTime(CurrentPeriodEndDateTime).GetChars());

				sc.AddMessageToLog(Message,0);

			}
			int MaxPeriodsToGoBack = 1;

			if(AutoSkipPeriodOfNoTrading.GetYesNo())
				MaxPeriodsToGoBack = 32;

			for (int WalkBack = 0; WalkBack < MaxPeriodsToGoBack; WalkBack++)
			{
				if (!UseCurrentPeriod.GetYesNo() && WalkBack >= 1) //Walk back 1 period.
				{
					SCDateTime  PriorBeginOfRefDateTime = BeginOfRefDateTime;

					BeginOfRefDateTime= sc.GetStartOfPeriodForDateTime(BeginOfRefDateTime, TimePeriodType.GetTimePeriodType(),  PeriodLength, -1);
					EndOfRefDateTime=  PriorBeginOfRefDateTime- (1*SECONDS);

					if (DisplayDebuggingOutput.GetYesNo() != 0)
					{
						SCString Message;

						Message.Format("Moving back 1 period. BeginOfRefDateTime: %s, EndOfRefDateTime: %s.",
							sc.FormatDateTime(BeginOfRefDateTime).GetChars(),sc.FormatDateTime(EndOfRefDateTime).GetChars());

						sc.AddMessageToLog(Message,0);
					}
				}

				int NumberOfBars = 0;
				SCDateTime TotalTimeSpan;
				int Result = 0;
				if (UseCurrentPeriod.GetYesNo())
					Result = sc.GetOHLCOfTimePeriod(CurrentPeriodBeginDateTime, CurrentPeriodEndDateTime, Open, High, Low, Close, NextOpen, NumberOfBars, TotalTimeSpan);
				else
					Result = sc.GetOHLCOfTimePeriod(BeginOfRefDateTime, EndOfRefDateTime, Open, High, Low, Close, NextOpen, NumberOfBars, TotalTimeSpan);



				if (DisplayDebuggingOutput.GetYesNo() != 0)
				{
					SCString Message;
					Message.Format("Number of Bars: %d, Total Time Span In Minutes: %d",NumberOfBars,(int)(TotalTimeSpan/MINUTES+0.5));
					sc.AddMessageToLog(Message,0);

					Message.Format("RefOpen %f,RefHigh %f,RefLow %f,RefClose %f,RefNextOpen %f.",Open, High, Low, Close, NextOpen);
					sc.AddMessageToLog(Message,0);
				}

				if (UseCurrentPeriod.GetYesNo())
					break;

				if (!Result)
					continue;

				SCDateTime MinimumTimeSpan = (TimeIncrement * MinimumRequiredTP.GetFloat()/100.0f);
				if (TotalTimeSpan >= MinimumTimeSpan)
					break;
			}
		}

		sc.Subgraph[SC_OPEN][index] = Open;
		sc.Subgraph[SC_HIGH][index] = High;
		sc.Subgraph[SC_LOW][index] = Low;
		sc.Subgraph[SC_LAST][index] = Close;
		sc.CalculateOHLCAverages(index);
	} //for
}


/*==========================================================================*/

SCSFExport scsf_RenkoChart(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open = sc.Subgraph[0];
	SCSubgraphRef High = sc.Subgraph[1];
	SCSubgraphRef Low = sc.Subgraph[2];
	SCSubgraphRef Last = sc.Subgraph[3];
	SCSubgraphRef Volume = sc.Subgraph[4];
	SCSubgraphRef NumTrades = sc.Subgraph[5];
	SCSubgraphRef OHLCAvg = sc.Subgraph[6];
	SCSubgraphRef HLCAvg = sc.Subgraph[7];
	SCSubgraphRef HLAvg = sc.Subgraph[8];
	SCSubgraphRef BidVol = sc.Subgraph[9];
	SCSubgraphRef AskVol = sc.Subgraph[10];
	SCSubgraphRef RenkoUpDownTrend = sc.Subgraph[11];
	SCSubgraphRef RenkoLast = sc.Subgraph[12];

	SCInputRef RenkoBoxSize = sc.Input[0];
	SCInputRef UseHighLowInsteadOfLast = sc.Input[1];
	SCInputRef IgnoreLatestBarUntilClose = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Renko Chart";

		sc.GraphRegion = 0;
		sc.StandardChartHeader = 1;
		sc.IsCustomChart = 1;
		sc.GraphDrawType = GDT_CANDLESTICK_BODY_ONLY;


		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0, 255, 0);
		Open.DrawZeros = false;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(0, 127, 0);
		High.DrawZeros = false;

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(255, 0, 0);
		Low.DrawZeros = false;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(127, 0, 0);
		Last.DrawZeros = false;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_IGNORE;
		Volume.PrimaryColor = RGB(255,255,255);
		Volume.DrawZeros = false;

		NumTrades.Name = "# of Trades / OI";
		NumTrades.DrawStyle = DRAWSTYLE_IGNORE;
		NumTrades.PrimaryColor = RGB(255,255,255);
		NumTrades.DrawZeros = false;

		OHLCAvg.Name = "OHLC Avg";
		OHLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		OHLCAvg.PrimaryColor = RGB(255,255,255);
		OHLCAvg.DrawZeros = false;

		HLCAvg.Name = "HLC Avg";
		HLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLCAvg.PrimaryColor = RGB(255,255,255);
		HLCAvg.DrawZeros = false;

		HLAvg.Name = "HL Avg";
		HLAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLAvg.PrimaryColor = RGB(255,255,255);
		HLAvg.DrawZeros = false;

		BidVol.Name = "Bid Vol";
		BidVol.DrawStyle = DRAWSTYLE_IGNORE;
		BidVol.PrimaryColor = RGB(255,255,255);
		BidVol.DrawZeros = false;

		AskVol.Name = "Ask Vol";
		AskVol.DrawStyle = DRAWSTYLE_IGNORE;
		AskVol.PrimaryColor = RGB(255,255,255);
		AskVol.DrawZeros = false;

		RenkoBoxSize.Name = "Renko Box Size";
		RenkoBoxSize.SetFloat(0.0f);
		RenkoBoxSize.SetFloatLimits(0.0f,(float)MAX_STUDY_LENGTH);

		UseHighLowInsteadOfLast.Name = "Use High/Low Values instead of Last Value";
		UseHighLowInsteadOfLast.SetYesNo(0);

		IgnoreLatestBarUntilClose.Name = "Ignore Latest Underlying Bar until Close";
		IgnoreLatestBarUntilClose.SetYesNo(0);

		return;
	}

	

	int& FeatureMessageAdded   = sc.PersistVars->Integers [0];

	if (!sc.AdvancedFeatures)
	{
		if (FeatureMessageAdded == 0)
		{
			sc.AddMessageToLog(SPECIAL_FEATURES, 1);
			sc.AddMessageToLog("Renko Chart study: To restore the chart, remove the Renko chart study.", 1);
			sc.ResizeArrays(1);
			FeatureMessageAdded = 1;
		}
		return;
	}

	sc.ValueFormat = sc.BaseGraphValueFormat;
	unsigned long ValueFormat = sc.BaseGraphValueFormat;

	if (RenkoBoxSize.GetFloat() == 0.0f)
	{
		RenkoBoxSize.SetFloat(sc.TickSize*4);
	}

	sc.GraphName.Format("%s Renko - BoxSize: %s", 
		sc.GetStudyName(0).GetChars(), 
		sc.FormatGraphValue(RenkoBoxSize.GetFloat(), ValueFormat).GetChars());

	int BaseGraphIndex = sc.UpdateStartIndex;

	if (IgnoreLatestBarUntilClose.GetYesNo() != false && (sc.UpdateStartIndex == sc.ArraySize - 1))
		return; // No need to process last underlying bar.  However, during a full recalculation, the last bar will be processed. This is OK.

	float UpperBox = 0;
	float LowerBox = 0;
	float CumulativeVolume = 0;
	float CumulativeOIOrNT = 0;
	float CumulativeAskVolume = 0;
	float CumulativeBidVolume = 0;

	if (BaseGraphIndex == 0)
	{

		sc.ResizeArrays(0);

		if (!sc.AddElements(1))
			return;

		if (sc.ArraySize < 1)
			return; // nothing to display

		UpperBox = sc.Close[BaseGraphIndex];
		LowerBox = sc.Close[BaseGraphIndex];

		sc.DateTimeOut[0] = sc.BaseDateTimeIn[0];

		RenkoLast[0] = sc.Close[BaseGraphIndex];
		Open[0] = sc.Close[BaseGraphIndex];
		Last[0] = sc.Close[BaseGraphIndex];
		High[0] = sc.Close[BaseGraphIndex];
		Low[0] = sc.Close[BaseGraphIndex];
	}
	else
	{
		int RenkoIndex = sc.OutArraySize - 1;

		// When new data is updated within the same bar
		if (sc.DateTimeOut[RenkoIndex] == sc.BaseDateTimeIn[BaseGraphIndex])
		{
			// Decrement BaseGraphIndex to the first matching date time
			for (; BaseGraphIndex >= 0; BaseGraphIndex--)
			{
				if (sc.DateTimeOut[RenkoIndex] != sc.BaseDateTimeIn[BaseGraphIndex])
				{
					BaseGraphIndex++;
					break;
				}
			}

			int LastIndex = RenkoIndex;
			// Delete the Renko Elements with that date time
			for (; RenkoIndex >= 0; RenkoIndex--)
			{
				if (sc.DateTimeOut[LastIndex] != sc.DateTimeOut[RenkoIndex])
				{
					RenkoIndex++;
					break;
				}
			}
			sc.ResizeArrays(RenkoIndex);
			RenkoIndex = sc.OutArraySize - 1;
		}

		if (RenkoUpDownTrend[RenkoIndex] > 0) // Uptrend Box
		{
			UpperBox = RenkoLast[RenkoIndex];
			LowerBox = UpperBox - RenkoBoxSize.GetFloat();
			LowerBox = (float)sc.RoundToTickSize(LowerBox, RenkoBoxSize.GetFloat());
		}
		else
		{
			LowerBox = RenkoLast[RenkoIndex];
			UpperBox = LowerBox + RenkoBoxSize.GetFloat();
			UpperBox = (float)sc.RoundToTickSize(UpperBox, RenkoBoxSize.GetFloat());
		}

		// Calculate Cumulative Volume/OpenInterest/Number Of Trades for current index
		// Must calculate from Base Graph since current index could 
		// potentially have grown in volume since last calculation.

		int i = BaseGraphIndex;
		for (; i >= 0; i--)
		{
			if (sc.BaseDateTimeIn[i] <= sc.DateTimeOut[RenkoIndex])
				break;
		}

		for (int n = i; n < BaseGraphIndex; n++)
		{
			CumulativeVolume += sc.Volume[n];
			CumulativeOIOrNT += sc.OpenInterest[n];
			CumulativeAskVolume += sc.AskVolume[n];
			CumulativeBidVolume += sc.BidVolume[n];
		}

	}

	int OriginalRenkoIndex = sc.OutArraySize - 1;
	int BaseGraphEnd = sc.ArraySize;
	if (IgnoreLatestBarUntilClose.GetYesNo() != 0)
		BaseGraphEnd--;

	for (;BaseGraphIndex < BaseGraphEnd; BaseGraphIndex++)
	{
		CumulativeVolume += sc.Volume[BaseGraphIndex];
		CumulativeOIOrNT += sc.OpenInterest[BaseGraphIndex];
		CumulativeAskVolume += sc.AskVolume[BaseGraphIndex];
		CumulativeBidVolume += sc.BidVolume[BaseGraphIndex];

		int NewEntry = 0;
		int UpperOrLowerTrend = 0;
		float HigherPrice = sc.Close[BaseGraphIndex];
		float LowerPrice = sc.Close[BaseGraphIndex];

		if (UseHighLowInsteadOfLast.GetYesNo() != 0)
		{
			HigherPrice = sc.High[BaseGraphIndex];
			LowerPrice = sc.Low[BaseGraphIndex];
		}

		if (sc.FormattedEvaluateUsingDoubles((double)HigherPrice, ValueFormat, GREATER_EQUAL_OPERATOR, (double)(UpperBox + RenkoBoxSize.GetFloat()), ValueFormat))
		{
			NewEntry = 1;
			UpperOrLowerTrend = 1;
		}
		else if (sc.FormattedEvaluateUsingDoubles((double)LowerPrice, ValueFormat, LESS_EQUAL_OPERATOR, (double)(LowerBox - RenkoBoxSize.GetFloat()), ValueFormat))
		{
			NewEntry = 1;
			UpperOrLowerTrend = -1;
		}

		int NewEntryCounter = 0;

		while (NewEntry != 0)
		{
			NewEntryCounter++;
			if (NewEntryCounter >= 500)
			{
				sc.AddMessageToLog("Renko Chart study: Renko error due to either data error or Box Size too small. Stopping processing and skipping until real time updates. Try increasing the Box Size input.", 1);
				return;
			}

			sc.CalculateOHLCAverages(sc.OutArraySize - 1);

			NewEntry = 0;
			sc.AddElements(1);
			int NewBoxIndex = sc.OutArraySize - 1;
			sc.DateTimeOut[NewBoxIndex] = sc.BaseDateTimeIn[BaseGraphIndex];

			Volume[NewBoxIndex-1] = CumulativeVolume - sc.Volume[BaseGraphIndex];
			NumTrades[NewBoxIndex-1] = CumulativeOIOrNT - sc.OpenInterest[BaseGraphIndex];
			AskVol[NewBoxIndex-1] = CumulativeAskVolume - sc.AskVolume[BaseGraphIndex];
			BidVol[NewBoxIndex-1] = CumulativeBidVolume - sc.BidVolume[BaseGraphIndex];

			CumulativeVolume = sc.Volume[BaseGraphIndex];
			CumulativeOIOrNT = sc.OpenInterest[BaseGraphIndex];
			CumulativeAskVolume = sc.AskVolume[BaseGraphIndex];
			CumulativeBidVolume = sc.BidVolume[BaseGraphIndex];

			float NewLast = 0;
			float NewOtherBound = 0;

			if (UpperOrLowerTrend == 1) // Upper Trend
			{
				LowerBox = UpperBox;
				UpperBox += RenkoBoxSize.GetFloat();
				UpperBox = (float)sc.RoundToTickSize(UpperBox, RenkoBoxSize.GetFloat());

				NewLast = UpperBox;
				NewOtherBound = LowerBox;

				if (sc.FormattedEvaluateUsingDoubles((double)HigherPrice, ValueFormat, GREATER_EQUAL_OPERATOR, (double)(UpperBox + RenkoBoxSize.GetFloat()), ValueFormat))
					NewEntry = 1;
			}
			else
			{
				UpperBox = LowerBox;
				LowerBox -= RenkoBoxSize.GetFloat();
				LowerBox = (float)sc.RoundToTickSize(LowerBox, RenkoBoxSize.GetFloat());

				NewLast = LowerBox;
				NewOtherBound = UpperBox;

				if (sc.FormattedEvaluateUsingDoubles((double)LowerPrice, ValueFormat, LESS_EQUAL_OPERATOR, (double)(LowerBox - RenkoBoxSize.GetFloat()), ValueFormat))
					NewEntry = 1;
			}

			Low[NewBoxIndex] = LowerBox;
			High[NewBoxIndex] = UpperBox;
			RenkoLast[NewBoxIndex] = NewLast;
			RenkoUpDownTrend[NewBoxIndex] = (float)UpperOrLowerTrend;
			if (UpperOrLowerTrend == 1)
			{
				Open[NewBoxIndex] = LowerBox;
				Last[NewBoxIndex] = UpperBox;
			}
			else
			{
				Open[NewBoxIndex] = UpperBox;
				Last[NewBoxIndex] = LowerBox;
			}
		}

		sc.CalculateOHLCAverages(sc.OutArraySize - 1);
	}

	int RenkoIndex = sc.OutArraySize - 1;
	if (RenkoIndex >= 0)
	{
		Volume[RenkoIndex] = CumulativeVolume;
		NumTrades[RenkoIndex] = CumulativeOIOrNT;
		AskVol[RenkoIndex] = CumulativeAskVolume;
		BidVol[RenkoIndex] = CumulativeBidVolume;
	}
}

/*==========================================================================*/
SCSFExport scsf_MillisecondsExample(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Milliseconds Example";

		sc.AutoLoop = 1;

		sc.FreeDLL = 0;

		return;
	}
	//When working with milliseconds it is necessary to define variables of type SCDateTimeMS
	SCDateTimeMS DateTimeMS , DateTimeMSRefChart;

	// Get the milliseconds for the current bar.  This will be 0 if the chart bars are based upon a specific time by using the 'Days-Minutes-Seconds' Bar Period Type since the start time for a bar is always evenly aligned to a second based upon the Session Start Time and the time period of the bar. Otherwise, the milliseconds can be obtained for other Bar Period Types. 
	DateTimeMS = sc.BaseDateTimeIn[sc.Index];
	int MilliSecond = DateTimeMS.GetMilliSecond();


	//Get the Date-Times from another chart.
	SCDateTimeArray RefChartDateTime;
	sc.GetChartDateTimeArray(2, RefChartDateTime );

	if(RefChartDateTime.GetArraySize() != 0)
	{
		//Get milliseconds for last bar in the other chart being referenced
		DateTimeMSRefChart = RefChartDateTime[RefChartDateTime.GetArraySize() - 1];
		int MilliSecond = DateTimeMSRefChart.GetMilliSecond();

	}


	//Compare the Date-Times from the 2 charts with millisecond precision
	if(DateTimeMS ==DateTimeMSRefChart)
	{
		//There is a match down to the millisecond
	}

}


/*==========================================================================*/
SCSFExport scsf_RelativeVigorIndex(SCStudyInterfaceRef sc)
{

	SCSubgraphRef RVI=sc.Subgraph[0];
	SCInputRef   MovingAverageLength = sc.Input  [0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Relative Vigor Index";

		sc.AutoLoop = 1;

		sc.FreeDLL = 0;

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		RVI.Name = "RVI";
		RVI.DrawStyle = DRAWSTYLE_LINE;
		RVI.PrimaryColor = RGB(0,255,0);
		RVI.DrawZeros = true;

		MovingAverageLength.Name= "Moving Average Length";
		MovingAverageLength.SetInt(10);
		MovingAverageLength.SetIntLimits(1,MAX_STUDY_LENGTH);;

		return;
	}

	RVI.Arrays[0][sc.Index]= (sc.Close[sc.Index]- sc.Open[sc.Index] )/ (sc.High[sc.Index] - sc.Low[sc.Index]);


	sc.ExponentialMovAvg  (RVI.Arrays[0],RVI,MovingAverageLength.GetInt());
}

/*============================================================================
	Add function description here.
----------------------------------------------------------------------------*/
SCSFExport scsf_ImmediateCallExample(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Immediate Call Example";
		sc.AutoLoop = 1;

		//Increases CPU load. Not recommended. Use only for special purposes.
		sc.OnExternalDataImmediateStudyCall= true;

		sc.FreeDLL = 0;
		
		return;
	}
	
	if(sc.Index == sc.ArraySize - 1)
	{
		// Log the current time
		SCString DateTimeString = sc.DateTimeToString(sc.CurrentSystemDateTime,FLAG_DT_COMPLETE_DATETIME_MS);

		sc.AddMessageToLog(DateTimeString, 0);
	}
}
