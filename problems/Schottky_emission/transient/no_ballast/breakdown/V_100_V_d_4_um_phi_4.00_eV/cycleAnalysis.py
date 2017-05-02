#### import the simple module from the paraview
from paraview.simple import *
import glob, re, os

def CreatePointSelection(ids):
  source = IDSelectionSource()
  source.FieldType = "POINT"
  sids = []
  for i in ids:
    sids.append(0) #proc-id
    sids.append(i) #cell-id
  source.IDs = sids
  return source

def selectElectrodeData(data, voi):
  dataOverTime = PlotSelectionOverTime(Input=data, Selection=selection)
  dataOverTime.OnlyReportSelectionStatistics = 0
  
  dataOverTimeT = TransposeTable(Input=dataOverTime, VariablesofInterest=voi)
  dataOverTimeT.UpdatePipeline()
  dataOverTimeTT = TransposeTable(Input=dataOverTimeT)
  
  dataOverTimeTT.Usethecolumnwithoriginalcolumnsname = 1
  dataOverTimeTT.Addacolumnwithoriginalcolumnsname = 0
  dataOverTimeTT.UpdatePipeline()
  
  return dataOverTimeTT

path = os.getcwd() + "/"
fileBase = "SteadyState_out"

## Define selection ##
selection = CreatePointSelection(ids=[0])

## Get reader data ##
reader = ExodusIIReader(FileName=path+fileBase + '.e')
reader.GenerateObjectIdCellArray = 1
reader.GenerateGlobalElementIdArray = 1
reader.ElementVariables = reader.ElementVariables.Available
reader.PointVariables = reader.PointVariables.Available
reader.ElementBlocks = reader.ElementBlocks.Available
reader.ApplyDisplacements = 1
reader.DisplacementMagnitude = 1.0

## Get cathode and anode coordinates
calc = Calculator(Input=reader)
calc.ResultArrayName = 'coords'
calc.Function = 'coords'
calc.UpdatePipeline()

coordRange = calc.PointData['coords'].GetRange()
minX = coordRange[0]
maxX = coordRange[1]

Delete(calc)
del calc

## Prepare to extract electrode data ##
electrodeData = []
VariablesofInterest = ['Time', 'Voltage', 'Current_Arp', 'Current_em', 'tot_gas_current', 'Emission_energy_flux']

## Extract cathode data ##
cathodeValues = ExtractLocation(Input=reader)
cathodeValues.Location = [minX,0,0]
cathodeValues.Mode = 'Interpolate At Location'

electrodeData.append(selectElectrodeData(cathodeValues, VariablesofInterest))

## Extract anode data ##
anodeValues = ExtractLocation(Input=reader)
anodeValues.Location = [maxX,0,0]
anodeValues.Mode = 'Interpolate At Location'

electrodeData.append(selectElectrodeData(anodeValues, VariablesofInterest))

## Calculate average powers and efficiency ##
PowerAndEfficiency = ProgrammableFilter(Input=electrodeData)
PowerAndEfficiency.Script = """
from numpy import trapz
for c, a, outTable in zip(inputs[0], inputs[1], output):
  time = c.RowData['Time']
  period = max(time) - min(time)
  
  j = abs(a.RowData['tot_gas_current'])
  plasmaPower = abs(j * ( c.RowData['Voltage'] - a.RowData['Voltage'] ))
  EmitEnergy = c.RowData['Emission_energy_flux']
  
  outTable.RowData.append(trapz(j**2, x=time) / period, 'j2')
  outTable.RowData.append(trapz(EmitEnergy, x=time) / period, 'emit_energy')
  outTable.RowData.append(trapz(plasmaPower, x=time) / period, 'plasma_power')
  
  outTable.RowData.append(trapz( (j**2) / (EmitEnergy + plasmaPower), x=time) / period, 'electronic_efficiency')
"""

PowerAndEfficiency.UpdatePipeline()

writer = CreateWriter(path + 'PowerAndEfficiency.csv', PowerAndEfficiency, Precision=12, UseScientificNotation=1)
writer.UpdatePipeline()

#writer = CreateWriter(path + 'AnodeData.csv', anodeValuesOverTime3, Precision=12, UseScientificNotation=1)
#writer.UpdatePipeline()
#Delete(writer)
#del writer



for f in GetSources().values():
    Delete(f)
    del f

