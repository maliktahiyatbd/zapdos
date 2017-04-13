#### import the simple module from the paraview
from paraview.simple import *
import glob, re, os

def CreateCellSelection(ids):
    source = IDSelectionSource()
    source.FieldType = "CELL"
    sids = []
    for i in ids:
        sids.append(0) #proc-id
        sids.append(i) #cell-id
    source.IDs = sids
    return source

def ProcessCycleData(fileBase):
	reader = ExodusIIReader(FileName=fileBase + '.e')
	reader.GenerateObjectIdCellArray = 1
	reader.GenerateGlobalElementIdArray = 1
	reader.ElementVariables = reader.ElementVariables.Available
	reader.PointVariables = reader.PointVariables.Available
	reader.ElementBlocks = reader.ElementBlocks.Available
	reader.ApplyDisplacements = 1
	reader.DisplacementMagnitude = 1.0
	
	# create a new 'Integrate Variables'
	integrateVariables = IntegrateVariables(Input=reader)
	
	# create a new 'Plot Selection Over Time'
	selection = CreateCellSelection(ids=[0])
	#selection = SelectCells(query="id==0", proxy=integrateVariables)
	integrateVariablesOverTime = PlotSelectionOverTime(Input=integrateVariables, Selection=selection)
	integrateVariablesOverTime.OnlyReportSelectionStatistics = 0
		
	# create a new 'Plot Selection Over Time'
	#selection = SelectPoints(query="id==0", proxy=reader)
	cathodeProbeOverTime = PlotSelectionOverTime(Input=reader, Selection=selection)
	cathodeProbeOverTime.OnlyReportSelectionStatistics = 0
	
	writer = CreateWriter(fileBase + '_integrated.csv', integrateVariablesOverTime, Precision=12, UseScientificNotation=1, WriteAllTimeSteps=1)
	writer.UpdatePipeline()
	del writer
	
	writer = CreateWriter(fileBase + '_cathode.csv', cathodeProbeOverTime, Precision=12, UseScientificNotation=1, WriteAllTimeSteps=1)
	writer.UpdatePipeline()
	del writer
	
	Delete(integrateVariablesOverTime)
	del integrateVariablesOverTime
	
	Delete(cathodeProbeOverTime)
	del cathodeProbeOverTime
	
	Delete(integrateVariables)
	del integrateVariables
	
	Delete(reader)
	del reader
	
	Delete(selection)
	del selection

fileBase = os.getcwd() + '/SteadyState_out'
ProcessCycleData(fileBase)

