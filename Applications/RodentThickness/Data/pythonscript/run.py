#!/usr/bin/python


from optparse import OptionParser
import csv
import os,os.path,re

'''
This python script accepts .csv data set, .bms configurations, and output directory then produces statistical analysis results from the processed data
'''

'''
concatToColumns.py
'''
def countLines(files):
  lines = []
  for file in files:
    fin = open(file)
    lines.append(len(fin.readlines()))
    fin.close()
  return lines

def readAllFiles(files):
  lines = []
  for file in files:
    fin = open(file)
    lines.append(fin.readlines())
    fin.close()
  return lines

def file2string(f):
  fin = open(f)
  lines = fin.readlines()
  lines = map(lambda x: x.strip(), lines)
  return lines
   

def concatColumnsToFile(input, output):
  if (input != ""):
    args = file2string(input)
  else:
    return
  lines = countLines(args)
  if (len(lines) == 0):
    print "Empty files"
    return
  if (min(lines) != max(lines)):
    print "Some of input files have different line numbers: ", lines
    return
  lineStrings = readAllFiles(args)
  if (output != ""):
    fout = open(output, "w")
  else:
    fout = sys.stdout
  for i in range(0,lines[0]):
    tempOutputLine = [ x[i].strip() for x in lineStrings]
    outputLineString = "\t".join(tempOutputLine)
    fout.write(outputLineString)
    fout.write("\n")
  fout.close()

def initializeConfig(bmsfile):
  config = {}
  reg = re.compile("[ ]*[Ss][Ee][Tt][ ]*\([ ]*([A-Za-z0-9_]*)[ ]+(.*)\)")
  with open(bmsfile) as f:
    bmslines = f.readlines()
  for line in bmslines:
    bmsmatch = reg.match(line)
    name = bmsmatch.group(1)
    path = bmsmatch.group(2).strip()
    config.update({name:path})
  return config

'''
execute MeshPointsIntensitySampling
'''
def performAnalysis(cvsdata, config, outputDir, opt="initial_dense"):
  ids = []
  groups = []
  for (id, labelmap, group) in cvsdata:
    ids.append(id)
    groups.append(group)
    
  meshDir = "%s/Processing/1.MeasurementandSPHARM" % (outputDir)
  shapeworksDir = "%s/Processing/2.shapeworks" % (outputDir)
  sampleDir = "%s/Processing/3.meshintensity" % (outputDir)
  statDir = "%s/Statistical" % (outputDir)
  pathKmesh = config["kmeshPath"]

  if (opt == "initial_dense"):
    samplingCmd = config["MeshPointIntensitySamplingPath"] + " --workDir %s --inputAsPhysicalCoord --inputPointFlip --distanceVector %s -i nn -a %s -m %s --smoothedAttributeOutput %s --originalMeshOutput %s %s %s"
  else:
    samplingCmd = config["MeshPointIntensitySamplingPath"] + " --workDir %s --inputAsPhysicalCoord --distanceVector %s -i nn -a %s -m %s --smoothedAttributeOutput %s --originalMeshOutput %s %s %s"



  # depending on options, choose different meshes
  # loop over ids
  for id in ids:
    distanceVector = "%s/%s.distanceVector.nrrd" % (sampleDir, id)
    inputMeasurement = "%s/%s_measurementoutput.nrrd" % (meshDir, id)
    inputMeasurement = "%s/%s.thickness.mha" % (meshDir, id)
    if (opt == "initial_dense"):
      samplingTxt = "%s/%s.initialDenseSampling.txt" % (sampleDir, id)
      smoothSamplingTxt = "%s/%s.smoothInitialDenseSampling.txt" % (sampleDir, id)
      samplingMeshOutput = "%s/%s.initialDenseSampling.vtk" % (sampleDir, id)
      originalMeshOutput = "%s/%s.initialDenseOriginal.vtk" % (sampleDir, id)
      inputMesh = "%s/%s.subj.SPHARM.vtk" % (meshDir, id)
    elif (opt == "correspondence"):
      samplingTxt = "%s/%s.sampling.txt" % (sampleDir, id)
      smoothSamplingTxt = "%s/%s.smoothSampling.txt" % (sampleDir, id)
      samplingMeshOutput = "%s/%s.sampling.vtk" % (sampleDir, id)
      originalMeshOutput = "%s/%s.original.vtk" % (sampleDir, id)
      inputMesh = "%s/%s.correspondence.vtk" % (shapeworksDir, id)
    elif (opt == "dense_correspondence"):
      samplingTxt = "%s/%s.denseSampling.txt" % (sampleDir, id)
      smoothSamplingTxt = "%s/%s.smoothDenseSampling.txt" % (sampleDir, id)
      samplingMeshOutput = "%s/%s.denseSampling.vtk" % (sampleDir, id)
      originalMeshOutput = "%s/%s.denseOriginal.vtk" % (sampleDir, id)
      inputMesh = "%s/%s.dense_correspondence.vtk" % (shapeworksDir, id)
    elif (opt == "spharm_sampling"):
      samplingTxt = "%s/%s.spharmSampling.txt" % (sampleDir, id)
      smoothSamplingTxt = "%s/%s.smoothSpharmSampling.txt" % (sampleDir, id)
      samplingMeshOutput = "%s/%s.spharmSampling.vtk" % (sampleDir, id)
      originalMeshOutput = "%s/%s.spharmOriginal.vtk" % (sampleDir, id)
      inputMesh = "%s/%s.subj.SPHARM.vtk" % (meshDir, id)

    # execute sampling
    # exeCmd = samplingCmd % (outputDir, distanceVector, samplingTxt, samplingMeshOutput, smoothSamplingTxt, originalMeshOutput, inputMeasurement, inputMesh)
    # new sampling command
    useStreamLine = True and opt == "spharm_sampling"
    if (not useStreamLine):
      exeCmd = "%s -sampleImage %s %s %s -outputScalarName Thickness -zrotate" % (pathKmesh, inputMeasurement, inputMesh, samplingMeshOutput)
      os.system(exeCmd)

      exeCmd = "%s -smoothScalars %s %s -scalarName Thickness -outputScalarName smoothThickness -iter 3" % (pathKmesh, samplingMeshOutput, samplingMeshOutput)
      os.system(exeCmd)

      exeCmd = "%s -exportScalars %s %s -scalarName Thickness " % (pathKmesh, samplingMeshOutput, samplingTxt)
      os.system(exeCmd)

      exeCmd = "%s -exportScalars %s %s -scalarName smoothThickness " % (pathKmesh, samplingMeshOutput, smoothSamplingTxt)
      os.system(exeCmd)
    else:
      gradientFile = "%s/Processing/1.MeasurementandSPHARM/%s/laplacianGradient.nrrd" % (outputDir, id)
      gradientVTIFile = "%s/Processing/1.MeasurementandSPHARM/%s/laplacianGradient.vti" % (outputDir, id)
      streamLineFile = "%s/Processing/1.MeasurementandSPHARM/%s/stream_lines.vtp" % (outputDir, id)

      # vti image creation
      exeCmd = "%s -vti %s %s -attrDim 3" % (pathKmesh, gradientFile, gradientVTIFile)
      os.system(exeCmd)

      # RK45 integration
      exeCmd = "%s -traceStream %s %s %s %s -zrotate -traceDirection backward" % (pathKmesh, gradientVTIFile, inputMesh, streamLineFile, samplingMeshOutput)
      os.system(exeCmd)

      # scalar export
      exeCmd = "%s -exportScalars %s %s -scalarName Length " % (pathKmesh, samplingMeshOutput, smoothSamplingTxt)
      os.system(exeCmd)



  # aggregate sampling files into a single file
  groupSet = set(groups)
  datafiles = []
  for group in groupSet:
    # setup output filename template
    if (opt == "initial_dense"):
      datafilename = "%s/data_initialDenseSampling_%s.txt" % (statDir, group)
      outfilename = "%s/list_initialDenseSampling_%s.txt" % (statDir, group)
    elif (opt == "correspondence"):
      datafilename = "%s/data_%s.txt" % (statDir, group)
      outfilename = "%s/list_%s.txt" % (statDir, group)
    elif (opt == "dense_correspondence"):
      datafilename = "%s/data_dense_%s.txt" % (statDir, group)
      outfilename = "%s/list_dense_%s.txt" % (statDir, group)
    elif (opt == "spharm_sampling"):
      datafilename = "%s/data_spharm_%s.txt" % (statDir, group)
      outfilename = "%s/list_spharm_%s.txt" % (statDir, group)


    # process and write the aggregated data
    outfile = open(outfilename, 'w')
    for (id, g) in zip(ids, groups):
      if (g != group):
        continue
      if (opt == "initial_dense"):
        samplingTxt = "%s/%s.smoothInitialDenseSampling.txt" % (sampleDir, id)
      elif (opt == "correspondence"):
        samplingTxt = "%s/%s.smoothSampling.txt" % (sampleDir, id)
      elif (opt == "dense_correspondence"):
        samplingTxt = "%s/%s.smoothDenseSampling.txt" % (sampleDir, id)
      elif (opt == "spharm_sampling"):
        samplingTxt = "%s/%s.smoothSpharmSampling.txt" % (sampleDir, id)
      if (os.path.isfile(samplingTxt)):
        outfile.write(samplingTxt)
        outfile.write("\n")
      else:
        print "can't find", samplingTxt
    outfile.close()

    # concatenate those attribute files into a file
    concatColumnsToFile(outfilename, datafilename)
    datafiles.append((group, datafilename))

  # perform statistical analysis
  rscriptExePath = config["RScriptPath"]
  rscriptPath = config["RScriptRodentThicknessPath"]
  if (opt == "initial_dense"):
    outputfile = "%s/stats.initialDenseSampling.txt" % statDir
    outputVTK = "%s/stats.initialDenseSampling.vtk" % (statDir)
    inputVTK = "%s/%s.subj.SPHARM.vtk" % (meshDir, ids[0])
  elif (opt == "correspondence"):
    outputfile = "%s/stats.correspondence.txt" % statDir
    outputVTK = "%s/stats.correspondence.vtk" % (statDir)
    inputVTK = "%s/%s.correspondence.vtk" % (shapeworksDir, ids[0])
  elif (opt == "dense_correspondence"):
    outputfile = "%s/stats.denseCorrespondence.txt" % statDir
    outputVTK = "%s/stats.denseCorrespondence.vtk" % (statDir)
    inputVTK = "%s/%s.dense_correspondence.vtk" % (shapeworksDir, ids[0])
  elif (opt == "spharm_sampling"):
    outputfile = "%s/stats.spharmCorrespondence.txt" % statDir
    outputVTK = "%s/stats.spharmCorrespondence.vtk" % (statDir)
    inputVTK = "%s/%s.subj.SPHARM.vtk" % (meshDir, ids[0])

  pythonPath = config["PythonPath"]
  pythonScriptPath = config["vtkPointAttributesPythonScriptPath"]

  datafilelist = ""
  datagroups = ""
  for (group, file) in datafiles:
    datafilelist = datafilelist + " " + file
    datagroups = datagroups + " " + group
  runCmd = "%s %s %s %s %s" % (rscriptExePath, rscriptPath, datafilelist, outputfile, datagroups)
  os.system(runCmd)

  visCmd = "%s %s %s %s -i %s -t" % (pythonPath, pythonScriptPath, inputVTK, outputVTK, outputfile)
  os.system(visCmd)



'''
Label map processing for thickness measurement
def labelprocessing(config, csvdata, outputdir, labelNCleft, labelNCright, labelOB):
  workdir = "%s/Processing/1.MeasurementandSPHARM"
  for (id, labelmap, group):
    brain_nc_left = "%s/%s_brain_left.nrrd" % (workdir, id)
    brain_nc_left = "%s/%s_brain_left.nrrd" % (workdir, id)
    brain_nc_left = "%s/%s_brain_left.nrrd" % (workdir, id)
    brain_nc_left = "%s/%s_brain_left.nrrd" % (workdir, id)
    brain_nc_left = "%s/%s_brain_left.nrrd" % (workdir, id)
'''

# run thickness measurement
# if the measurement file exists, skip it
def computeThickness(config, data, outputdir, ids, idl, idh):
  pathTool = config["measureThicknessFilterPath"]
  for (id, labelmap, group) in data:
    workdir = "%s/Processing/1.MeasurementandSPHARM/%s" % (outputdir, id)
    measurementoutput = "%s/Processing/1.MeasurementandSPHARM/%s_measurementoutput.nrrd" % (outputdir, id)

    # temporary
    workdir = "%s/Processing/1.MeasurementandSPHARM/%s_2" % (outputdir, id)
    labelmap = "%s/Processing/1.MeasurementandSPHARM/%s.boundaryMap.mha" % (outputdir, id)
    measurementoutput = "%s/Processing/1.MeasurementandSPHARM/%s.thickness.mha" % (outputdir, id)

    if (not os.path.exists(workdir)):
      os.makedirs(workdir)
    if (not os.path.isfile(measurementoutput) or True):
      cmd = "%s --mr --sbt --workdir %s --ids %s --idl %s --idh %s --ttrns 500 %s %s" % (pathTool, workdir, ids, idl, idh, labelmap, measurementoutput)
      print cmd
      os.system(cmd)
    else:
      print "Skipping", measurementoutput

def precorrespondence(config, data, outputDir, ids):
  pathGenParaMesh = config["GenParaMeshCLPPath"]
  pathParaToSPHARM = config["ParaToSPHARMMeshCLPPath"]
  pathImageMath = config["ImageMathPath"]
  pathResampleVolume = config["ResampleVolume2Path"]

  inputSurfaceFiles = open("%s/Processing/2.shapeworks/inputsurfacemodels.txt" % (outputDir), 'w')
  inputImageFiles = open("%s/Processing/2.shapeworks/inputimage.txt" % (outputDir), 'w')
  outputCorrespondenceFiles = open("%s/Processing/2.shapeworks/outputCorrespondence.txt" % (outputDir), 'w')
  outputWarpedFiles = open("%s/Processing/2.shapeworks/outputWarped.txt" % (outputDir), 'w')

  prevId = ""
  for (idx, item) in enumerate(data):
    (id, labelmap, group) = item

    workdir = "%s/Processing/1.MeasurementandSPHARM" % (outputDir)
    eulerName = "%s/EulerFile/Euler_%s.txt" % (workdir, id)
    parafile = "%s/%s_para.vtk" % (workdir, id)
    surffile = "%s/%s_surf.vtk" % (workdir, id)
    initialMesh = "%s/%s.ip.SPHARM.vtk" % (workdir, id)
    surfaceMesh = "%s/%s.subj.SPHARM.vtk" % (workdir, id)

    if (not os.path.isfile(parafile)):
      cmd = "%s --EulerFile --outEulerName %s %s --label %s %s %s" % (pathGenParaMesh, eulerName, labelmap, ids, parafile, surffile)
      print cmd
      os.system(cmd)

    if (not os.path.isfile(initialMesh) or os.stat(initialMesh).st_size == 0):
      if (prevId == ""):
        cmd = "%s %s %s --subdivLevel 10 --spharmDegree 20  %s/%s.ip." % (pathParaToSPHARM, parafile, surffile, workdir, id)
      else:
        cmd = "%s %s %s --subdivLevel 10 --spharmDegree 20  %s/%s.ip. --flipTemplateOn --flipTemplate %s/%s.ip.SPHARM.coef" % (pathParaToSPHARM, parafile, surffile, workdir, id, workdir, prevId)
      print cmd
      os.system(cmd)

    if (not os.path.isfile(surfaceMesh) or os.stat(surfaceMesh).st_size == 0):
      if (prevId == ""):
        cmd = "%s %s %s --subdivLevel 50 --spharmDegree 20  %s/%s.subj." % (pathParaToSPHARM, parafile, surffile, workdir, id)
      else:
        cmd = "%s %s %s --subdivLevel 50 --spharmDegree 20  %s/%s.subj. --flipTemplateOn --flipTemplate %s/%s.subj.SPHARM.coef" % (pathParaToSPHARM, parafile, surffile, workdir, id, workdir, prevId)
      print cmd
      os.system(cmd)
      

    labelMapNC = "%s/%s.labelMapNCsegmentationOriginal.nrrd" % (workdir, id)
    labelMapNCResampled = "%s/%s.labelMapNCsegmentation.nrrd" % (workdir, id)
    if (not os.path.isfile(labelMapNC)):

      # create binary level set with same dimension
      if (prevId == ""):
        # for the first subject, do not create resampled version of NCsegmentation
        cmd = "%s %s -extractLabel %s -outfile %s" % (pathImageMath, labelmap, ids, labelMapNCResampled)
        os.system(cmd)
      else:
        cmd = "%s %s -extractLabel %s -outfile %s" % (pathImageMath, labelmap, ids, labelMapNC)
        os.system(cmd)
        cmd = "%s %s %s -R %s -i nn" % (pathResampleVolume, labelMapNC, labelMapNCResampled, "%s/%s.labelMapNCsegmentation.nrrd" % (workdir, prevId))
        os.system(cmd)
      
    prevId = id
    inputSurfaceFiles.write("%s/%s.ip.SPHARM.vtk\n" % (workdir, id))
    inputImageFiles.write(labelMapNCResampled + "\n")
    outputCorrespondenceFiles.write("%s/Processing/2.shapeworks/%s.correspondence.vtk\n" % (outputdir, id))
    outputWarpedFiles.write("%s/Processing/2.shapeworks/%s.warped.vtk\n" % (outputdir, id))

  inputSurfaceFiles.close()
  inputImageFiles.close()
  outputCorrespondenceFiles.close()
  outputWarpedFiles.close()

# @brief compute consistent boundary conditions from surface maps
def computeConsistentBoundaryConditions(data, config, outputdir):
  pathKmesh = config["kmeshPath"]
  pathKcalc = config["kcalcPath"]
  workdir = "%s/Processing/1.MeasurementandSPHARM" % (outputdir)

  for (idx, item) in enumerate(data):
    tag = item[0]
    labelMap = item[1]
    group = item[2]

    labelOutput = workdir + "/" + tag + "/labelmap_zeroSolution.nrrd"
    surfaceInput = workdir + "/" + tag + ".subj.SPHARM.vtk"
    surfaceLabels = workdir + "/" + tag + ".labels.vtp"
    voronoiImage = workdir + "/" + tag + ".voronoi.mha"
    surfaceImage = workdir + "/" + tag + ".solution.mha"
    boundaryMap = workdir + "/" + tag + ".boundaryMap.mha"

    cmd = "%s -e 'A==3?0:A' -o %s %s" % (pathKcalc, labelOutput, labelMap)
    print cmd
    os.system(cmd)


    cmd = "%s -sampleImage -zrotate %s %s %s -outputScalarName labels" % (pathKmesh, labelOutput, surfaceInput, surfaceLabels)
    print cmd
    os.system(cmd)

  cmd = "%s -averageScalars -threshold 1.8 -scalarName labels -outputScalarName meanLabels" % (pathKmesh)
  for (tag, labelmap, group) in data:
    cmd += " " + workdir + "/" + tag + ".labels.vtp"
  print cmd
  os.system(cmd)

  for (idx, item) in enumerate(data):
    tag = item[0]
    labelMap = item[1]
    group = item[2]

    labelOutput = workdir + "/" + tag + "/labelmap_zeroSolution.nrrd"
    surfaceInput = workdir + "/" + tag + ".subj.SPHARM.vtk"
    surfaceLabels = workdir + "/" + tag + ".labels.vtp"
    voronoiImage = workdir + "/" + tag + ".voronoi.mha"
    surfaceImage = workdir + "/" + tag + ".solution.mha"
    boundaryMap = workdir + "/" + tag + ".boundaryMap.mha"

    cmd = "%s -voronoiImage -zrotate %s %s %s -scalarName meanLabels" % (pathKmesh, labelOutput, surfaceLabels, voronoiImage)
    print cmd
    os.system(cmd)

    cmd = "%s -scanConversion -zrotate %s %s %s" % (pathKmesh, surfaceLabels, labelOutput, surfaceImage)
    print cmd
    os.system(cmd)

    cmd = "%s -e 'B>0?3:A' -o %s %s %s" % (pathKcalc, boundaryMap, voronoiImage, surfaceImage)
    print cmd
    os.system(cmd)

def runcorrespondence(bmsfile, config, data, outputdir, ids):
  pathPython = config["PythonPath"]
  pathShapeWorksScript = config["ShapeWorksPythonScriptPath"]
  pathShapeWorksRun = config["ShapeWorksRunPath"]
  pathImageMath = config["ImageMathPath"]
  pathShapeWorksGroom = config["ShapeWorksGroomPath"]
  pathBinaryToDistanceMap = config["BinaryToDistanceMapPath"]

  workdir = "%s/Processing/2.shapeworks" % (outputdir)

  # generate binary distance map for each label map
  # cmd = "%s %s %s/inputimage.txt %s/inputsurfacemodels.txt -c %s/outputCorrespondence.txt -w %s/outputWarped.txt --workingDir %s --pathShapeWorksRun %s --pathShapeWorksGroom %s --pathImageMath %s --pathBinaryToDistanceMap %s" % (pathPython, pathShapeWorksScript, workdir, workdir, workdir, workdir, workdir, pathShapeWorksRun, pathShapeWorksGroom, pathImageMath, pathBinaryToDistanceMap)
  cmd = "%s %s %s/inputimage.txt %s/inputsurfacemodels.txt -c %s/outputCorrespondence.txt -w %s/outputWarped.txt --workingDir %s --configbms %s" % (pathPython, pathShapeWorksScript, workdir, workdir, workdir, workdir, workdir, bmsfile)
  print cmd
  os.system(cmd)

if (__name__ == "__main__"):
  parser = OptionParser(usage="usage: %prog dataset.csv config.bms output-directory")
#  parser.add_option("--labelprocessing", help="preprocessing for label map generation", action="store_true", dest="labelprocessing")
  parser.add_option("--precorrespondence", help="preprocessing for particle correspondence", action="store_true", dest="precorrespondence")
  parser.add_option("--runcorrespondence", help="run particle correspondence", action="store_true", dest="runcorrespondence")
  parser.add_option("--computeThickness", help="compute thickness for a given label", action="store_true", dest="computeThickness")
  parser.add_option("--runstats", help="run statistical analysis", action="store_true", dest="runstats")
  parser.add_option("--boundaryCorrection", dest="boundaryCorrection", help="Correct boundary conditions more consistent", action="store_true")
  parser.add_option("--ids", help="solution label", dest="ids", default="3")
  parser.add_option("--idl", help="low boundary label", dest="idl", default="2")
  parser.add_option("--idh", help="high boundary label", dest="idh", default="1")

  parser.add_option("--labelNCleft", help="label id for NC left", dest="labelNCleft", default="30")
  parser.add_option("--labelNCright", help="label id for NC right", dest="labelNCright", default="14")
  parser.add_option("--labelOB", help="label id for Olfactory Bulb", dest="labelOB", default="16")

  (opts, args) = parser.parse_args()

  if (len(args) < 3):
    parser.print_help()
  else:
    csvfile = args[0]
    bmsfile = args[1]
    outputdir = args[2]


    if (not os.path.isdir("%s/Processing/1.MeasurementandSPHARM" % (outputdir))):
      os.makedirs("%s/Processing/1.MeasurementandSPHARM" % (outputdir))

    if (not os.path.isdir("%s/Processing/2.shapeworks" % (outputdir))):
      os.makedirs("%s/Processing/2.shapeworks" % (outputdir))

    if (not os.path.isdir("%s/Processing/3.meshintensity" % (outputdir))):
      os.makedirs("%s/Processing/3.meshintensity" % (outputdir))

    if (not os.path.isdir("%s/Statistical" % (outputdir))):
      os.makedirs("%s/Statistical" % (outputdir))

    config = initializeConfig(bmsfile)

    csvreader = csv.reader(open(csvfile, "r"))
    csvheader = csvreader.next()

    csvdata = []
    dataIds = []
    dataGroups = []

    for csvline in csvreader:
      dataIds.append(csvline[0])
      dataGroups.append(csvline[2])
      csvdata.append(csvline) 

    if (opts.precorrespondence):
      precorrespondence(config, csvdata, outputdir, opts.ids)
    if (opts.boundaryCorrection):
      computeConsistentBoundaryConditions(csvdata, config, outputdir)
    if (opts.computeThickness):
      computeThickness(config, csvdata, outputdir, opts.ids, opts.idl, opts.idh)
    if (opts.runcorrespondence):
      runcorrespondence(bmsfile, config, csvdata, outputdir, opts.ids)
    if (opts.runstats):
      performAnalysis(csvdata, config, outputdir, "spharm_sampling")
#      performAnalysis(csvdata, config, outputdir, "initial_dense")
#      performAnalysis(csvdata, config, outputdir, "correspondence")

