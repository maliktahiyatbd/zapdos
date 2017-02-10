from paraview.simple import *
import os
import sys
import numpy as np

path = os.getcwd() + "/"
file_name = sys.argv[1] + ".e"
temp = "temp.csv"

reader = ExodusIIReader(FileName=path+file_name)
tsteps = reader.TimestepValues

writer = CreateWriter(path + temp , reader)
writer.FieldAssociation = "Cells" # or "Points"

if len(tsteps) == 0:
	writer.UpdatePipeline()
else:
	if offset >= 0:
		writer.UpdatePipeline(time=tsteps[offset])
	else:
		writer.UpdatePipeline(time=tsteps[len(tsteps) + offset])
	
del writer