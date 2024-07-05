############################################################
# Part 1
# - Setup common crab job configurations
#
############################################################
import CRABClient
from CRABClient.UserUtilities import config

crab_config = config()
#
# Set request name prefx
#
# reqNamePrefix = "JMEPUNano"
reqNamePrefix = "PUNano"
#
# Set version number (CHECK)
#
# version = "v0p2_Evt10k"
version = "v0p2"
#
# Set a non-empty string if we want to remake a sample but save in a new USER dataset
#
prodversion=""
# prodversion="-prodv2"
#
# Change this PATH where the crab directories are stored
# Example: config.General.workArea = '/afs/cern.ch/work/n/nbinnorj/private/crab_projects/'
#
crab_config.General.workArea = '/afs/cern.ch/work/n/nbinnorj/private/crab_projects_jmenanowithpileup_4/'

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

# runTime_mc = 1800
# fileSplit_mc = 1
# psetName ='NanoWithPU_1BX_forCrab_cfg.py'
# samplelist = [
#   "/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/Run3Winter24MiniAOD-133X_mcRun3_2024_realistic_v10_ext2-v2/MINIAODSIM",
#   "/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/Run3Winter24MiniAOD-133X_mcRun3_2024_realistic_v10_ext1-v2/MINIAODSIM",
#   "/QCD_PT-15to7000_TuneCP5_Flat2022_13p6TeV_pythia8/Run3Winter24MiniAOD-133X_mcRun3_2024_realistic_v10-v2/MINIAODSIM",
# ]
# parentlist = [
#   "/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/Run3Winter24Digi-133X_mcRun3_2024_realistic_v10_ext2-v2/GEN-SIM-RAW",
#   "/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/Run3Winter24Digi-133X_mcRun3_2024_realistic_v10_ext1-v2/GEN-SIM-RAW",
#   "/QCD_PT-15to7000_TuneCP5_Flat2022_13p6TeV_pythia8/Run3Winter24Digi-133X_mcRun3_2024_realistic_v10-v2/GEN-SIM-RAW",
# ]

runTime_mc = 2500
fileSplit_mc = 8
psetName ='NanoPUOnly_1BX_forCrab_cfg.py'
samplelist = [
  "/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/Run3Winter24Digi-133X_mcRun3_2024_realistic_v10_ext2-v2/GEN-SIM-RAW",
  "/QCD_PT-15to7000_TuneCP5_Flat2022_13p6TeV_pythia8/Run3Winter24Digi-133X_mcRun3_2024_realistic_v10-v2/GEN-SIM-RAW",
]
parentlist = []

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
  # crab_config.Data.useParent = True
  if len(parentlist) != 0:
    crab_config.Data.secondaryInputDataset = parentlist[i]
  #
  # FileBased split
  #
  crab_config.Data.splitting = 'FileBased'
  crab_config.JobType.psetName  = psetName
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

  print("requestName: " + crab_config.General.requestName)
  print("outputDatasetTag: " + crab_config.Data.outputDatasetTag)
  if len(parentlist) != 0:
    print("secondaryInputDataset: "+ crab_config.Data.secondaryInputDataset)
  print(crab_config.Data.splitting)
  print(crab_config.Data.unitsPerJob)
  print(crab_config.JobType.maxJobRuntimeMin)
  #####print(crab_config.Data.lumiMask)
  crabCommand('submit', config = crab_config)
  print("")

