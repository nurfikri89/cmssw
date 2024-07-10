############################################################
# Part 1
# - Setup common crab job configurations
#
############################################################
import CRABClient
from CRABClient.UserUtilities import config

def GetSampleList(file):
  samplelist = file.readlines()
  samplelist = [x.strip() for x in samplelist] 
  samplelist = [x for x in samplelist if x] # Choose lines that are not empty
  samplelist = [x for x in samplelist if not(x.startswith("#"))] # Choose lines that do not start with #
  return samplelist

crab_config = config()
#
# Set request name prefx
#
reqNamePrefix = "JMENano12p5"
#
# Set version number (CHECK)
#
version = "v1p0"
#
# Set a non-empty string if we want to remake a sample but save in a new USER dataset
#
prodversion=""
# prodversion="-prodv2"
#
# Change this PATH where the crab directories are stored
# Example: config.General.workArea = '/afs/cern.ch/work/n/nbinnorj/private/crab_projects/'
#
crab_config.General.workArea = '/afs/cern.ch/work/n/nbinnorj/private/crab_projects_JMENano/'

#
crab_config.JobType.pluginName = 'Analysis'

#
crab_config.Data.publication = True
crab_config.Data.allowNonValidInputDataset = True
crab_config.JobType.allowUndistributedCMSSW = True
#
# Specify the outLFNDirBase and your storage site
# JetMET CMS EOS space at CERN
#
# crab_config.Data.outLFNDirBase  = '/store/group/phys_jetmet/'+reqNamePrefix+'_'+version+'/CRABOUTPUT/'
# crab_config.Site.storageSite    = 'T2_CH_CERN'
crab_config.Data.outLFNDirBase  = '/store/user/nbinnorj/'+reqNamePrefix+'_'+version+'/CRABOUTPUT/'
crab_config.Site.storageSite    = 'T2_FI_HIP'
#
#
crab_config.Data.ignoreLocality = True
whitelist_sites=[
'T1_US_*',
'T1_ES_*',
'T1_IT_*',
'T1_FR_*',
'T1_RU_*',
'T1_DE_*',
'T2_US_*',
'T2_UK_*',
'T2_RU_*',
'T2_DE_*',
'T2_FR_*',
'T2_CH_*',
'T2_IT_*',
'T2_IN_*',
'T2_ES_*',
'T2_HU_*',
'T2_BE_*',
'T2_BR_*',
'T2_CN_*',
'T2_EE_*',
'T2_TW_*',
'T2_FI_*',
]
crab_config.Site.whitelist = whitelist_sites

############################################################
# Part 2
# -  Loop over list of samples. Send to Grid
#
############################################################
import sys
import helpers
from CRABAPI.RawCommand import crabCommand

# runTime_data = 360
# runTime_mc   = 360

runTime_data = 720
runTime_mc   = 1800

fileSplit_data = 1
fileSplit_mc   = 1

#
# Read in txt file with list of samples
#
f = open(sys.argv[1]) 
samplelist =  helpers.GetSampleList(f)

#
# Print out the list of samples
#
print("Will send crab jobs for the following samples:")
for dataset in samplelist:
  print(dataset)
print("\n\n")

#
# For each sample, set crab job configuration and then send to the Grid
#
for i, dataset in enumerate(samplelist):
  print("%d/%d:Sending CRAB job: %s" % (i+1,len(samplelist), dataset))
  #
  # Specify input dataset
  #
  crab_config.Data.inputDataset = dataset
  #
  # FileBased split
  #
  crab_config.Data.splitting    = 'FileBased'
  #
  # Check if Data or MC
  #
  isData = helpers.IsSampleData(dataset)
  if isData:
    # crab_config.JobType.psetName  = 'configs/%s_%s/CustomJMEPFNano_Data22_cfg.py'%(version)
    if "Run2022" in dataset and "22Sep2023":
      crab_config.JobType.psetName  = 'configs/%s_%s/CustomPFNano_Data22_PFNano_PFCandGenPartsInAK4AK8_IsoChHad.py'%(reqNamePrefix,version)
    elif "Run2023" in dataset and "22Sep2023":
      crab_config.JobType.psetName  = 'configs/%s_%s/CustomPFNano_Data23_PFNano_PFCandGenPartsInAK4AK8_IsoChHad.py'%(reqNamePrefix,version)
    crab_config.JobType.maxJobRuntimeMin = runTime_data
    crab_config.Data.unitsPerJob = fileSplit_data
    # Have to make unique requestName.
    # Sample naming convention is a bit dumb and makes this more difficult.
    #
    primaryName   = dataset.split('/')[1]
    secondaryName = helpers.TrimSecondaryNameForData(dataset)
  else:
    if   "Run3Summer22MiniAODv4" in dataset:     crab_config.JobType.psetName  = 'prod_Summer22JMENano_cfg.py'
    elif "Run3Summer22EEMiniAODv4" in dataset:   crab_config.JobType.psetName  = 'prod_Summer22JMENano_cfg.py'
    elif "Run3Summer23MiniAODv4" in dataset:     crab_config.JobType.psetName  = 'prod_Summer23JMENano_cfg.py'
    elif "Run3Summer23BPixMiniAODv4" in dataset: crab_config.JobType.psetName  = 'prod_Summer23BPixJMENano_cfg.py'

    crab_config.JobType.maxJobRuntimeMin = runTime_mc
    crab_config.Data.unitsPerJob = fileSplit_mc
    #
    # Have to make unique requestName.
    #
    primaryName   = helpers.TrimPrimaryNameForMC(dataset)
    secondaryName = helpers.TrimSecondaryNameForMC(dataset)

  requestName = reqNamePrefix + "_" + version + "_" + primaryName + "_" + secondaryName
  if prodversion != "":
    requestName += prodversion
  crab_config.General.requestName = requestName

  outputDatasetTag = reqNamePrefix + "_" + version + "_" + secondaryName
  if prodversion != "":
    outputDatasetTag += prodversion
  crab_config.Data.outputDatasetTag = outputDatasetTag

  print("requestName: "+crab_config.General.requestName)
  print("outputDatasetTag: "+crab_config.Data.outputDatasetTag)
  print(crab_config.Data.splitting)
  print(crab_config.Data.unitsPerJob)
  print(crab_config.JobType.maxJobRuntimeMin)
  #####print(crab_config.Data.lumiMask)
  crabCommand('submit', config = crab_config)
  print("")