import argparse
from CRABClient.UserUtilities import config
from CRABAPI.RawCommand import crabCommand

prod_name = "AODToPFNANO"
prod_version = "v0p1"
# workArea = "/afs/cern.ch/work/n/nbinnorj/private/crab_projects_AODToPFNANO/"
# workArea = "/afs/cern.ch/work/n/nbinnorj/private/crab_projects_AODToPFNANO_2/"
# workArea = "/afs/cern.ch/work/n/nbinnorj/private/crab_projects_AODToPFNANO_3/"
workArea = "/afs/cern.ch/work/n/nbinnorj/private/crab_projects_AODToPFNANO_4/"

readInputFileList=False

##########################################################################################
#
#
##########################################################################################
# /Muon0/Run2024F-PromptReco-v1/AOD
# /Muon0/Run2024G-PromptReco-v1/AOD
# /Muon0/Run2024H-PromptReco-v1/AOD
# /Muon0/Run2024I-PromptReco-v1/AOD
# /Muon0/Run2024I-PromptReco-v2/AOD
# /Muon1/Run2024F-PromptReco-v1/AOD
# /Muon1/Run2024G-PromptReco-v1/AOD
# /Muon1/Run2024H-PromptReco-v1/AOD
# /Muon1/Run2024I-PromptReco-v1/AOD
# /Muon1/Run2024I-PromptReco-v2/AOD
# /EGamma0/Run2024F-PromptReco-v1/AOD
# /EGamma0/Run2024G-PromptReco-v1/AOD
# /EGamma0/Run2024H-PromptReco-v1/AOD
# /EGamma0/Run2024I-PromptReco-v1/AOD
# /EGamma0/Run2024I-PromptReco-v2/AOD
# /EGamma1/Run2024F-PromptReco-v1/AOD
# /EGamma1/Run2024G-PromptReco-v1/AOD
# /EGamma1/Run2024H-PromptReco-v1/AOD
# /EGamma1/Run2024I-PromptReco-v1/AOD
# /EGamma1/Run2024I-PromptReco-v2/AOD
# /JetMET0/Run2024F-PromptReco-v1/AOD
# /JetMET0/Run2024G-PromptReco-v1/AOD
# /JetMET0/Run2024H-PromptReco-v1/AOD
# /JetMET0/Run2024I-PromptReco-v1/AOD
# /JetMET0/Run2024I-PromptReco-v2/AOD
# /JetMET1/Run2024F-PromptReco-v1/AOD
# /JetMET1/Run2024G-PromptReco-v1/AOD
# /JetMET1/Run2024H-PromptReco-v1/AOD
# /JetMET1/Run2024I-PromptReco-v1/AOD
# /JetMET1/Run2024I-PromptReco-v2/AOD
# /ZeroBias/Run2024F-PromptReco-v1/AOD
# /ZeroBias/Run2024G-PromptReco-v1/AOD
# /ZeroBias/Run2024H-PromptReco-v1/AOD
# /ZeroBias/Run2024I-PromptReco-v1/AOD
# /ZeroBias/Run2024I-PromptReco-v2/AOD

# Data24_Prompt_AODtoPFNano.py
# Data24_Prompt_SkimDiMuonHLT_AODtoPFNano.py
# Data24_Prompt_SkimJetHLT_AODtoPFNano.py

# psetName = './configs/Data24_Prompt_AODtoPFNano.py'
# tag = "Prompt"

# outputDatasetTag = f'Run2024I_PromptRecoNano_v2_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/ZeroBias/Run2024I-PromptReco-v2/AOD"

# outputDatasetTag = f'Run2024I_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/ZeroBias/Run2024I-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024H_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/ZeroBias/Run2024H-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024G_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/ZeroBias/Run2024G-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024F_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/ZeroBias/Run2024F-PromptReco-v1/AOD"

# psetName = './configs/Data24_Prompt_SkimSinglePhotonHLT_AODtoPFNano.py'
# tag = "Prompt_SkimSinglePhotonHLT"

# outputDatasetTag = f'Run2024I_PromptRecoNano_v2_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/EGamma0/Run2024I-PromptReco-v2/AOD"

# outputDatasetTag = f'Run2024I_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/EGamma0/Run2024I-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024H_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/EGamma0/Run2024H-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024G_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/EGamma0/Run2024G-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024F_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/EGamma0/Run2024F-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024I_PromptRecoNano_v2_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/EGamma1/Run2024I-PromptReco-v2/AOD"

# outputDatasetTag = f'Run2024I_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/EGamma1/Run2024I-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024H_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/EGamma1/Run2024H-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024G_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/EGamma1/Run2024G-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024F_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/EGamma1/Run2024F-PromptReco-v1/AOD"

# psetName = './configs/Data24_Prompt_SkimDiMuonHLT_AODtoPFNano.py'
# tag = "Prompt_SkimDiMuonHLT"

# outputDatasetTag = f'Run2024I_PromptRecoNano_v2_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/Muon0/Run2024I-PromptReco-v2/AOD"

# outputDatasetTag = f'Run2024I_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/Muon0/Run2024I-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024H_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/Muon0/Run2024H-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024G_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/Muon0/Run2024G-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024F_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/Muon0/Run2024F-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024I_PromptRecoNano_v2_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/Muon1/Run2024I-PromptReco-v2/AOD"

# outputDatasetTag = f'Run2024I_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/Muon1/Run2024I-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024H_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/Muon1/Run2024H-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024G_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/Muon1/Run2024G-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024F_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/Muon1/Run2024F-PromptReco-v1/AOD"

# psetName = './configs/Data24_Prompt_SkimJetHLT_AODtoPFNano.py'
# tag = "Prompt_SkimJetHLT"

# outputDatasetTag = f'Run2024I_PromptRecoNano_v2_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/JetMET0/Run2024I-PromptReco-v2/AOD"

# outputDatasetTag = f'Run2024I_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/JetMET0/Run2024I-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024H_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/JetMET0/Run2024H-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024G_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/JetMET0/Run2024G-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024F_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/JetMET0/Run2024F-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024I_PromptRecoNano_v2_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/JetMET1/Run2024I-PromptReco-v2/AOD"

# outputDatasetTag = f'Run2024I_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/JetMET1/Run2024I-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024H_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/JetMET1/Run2024H-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024G_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/JetMET1/Run2024G-PromptReco-v1/AOD"

# outputDatasetTag = f'Run2024F_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/JetMET1/Run2024F-PromptReco-v1/AOD"


# /ZeroBias/Run2024C-2024CDEReprocessing-v1/AOD
# /ZeroBias/Run2024D-2024CDEReprocessing-v1/AOD
# /ZeroBias/Run2024E-2024CDEReprocessing-v1/AOD
# /EGamma0/Run2024C-2024CDEReprocessing-v1/AOD
# /EGamma0/Run2024D-2024CDEReprocessing-v1/AOD
# /EGamma0/Run2024E-2024CDEReprocessing-v1/AOD
# /EGamma1/Run2024C-2024CDEReprocessing-v1/AOD
# /EGamma1/Run2024D-2024CDEReprocessing-v1/AOD
# /EGamma1/Run2024E-2024CDEReprocessing-v1/AOD
# /Muon0/Run2024C-2024CDEReprocessing-v1/AOD
# /Muon0/Run2024D-2024CDEReprocessing-v1/AOD
# /Muon0/Run2024E-2024CDEReprocessing-v1/AOD
# /Muon1/Run2024C-2024CDEReprocessing-v1/AOD
# /Muon1/Run2024D-2024CDEReprocessing-v1/AOD
# /Muon1/Run2024E-2024CDEReprocessing-v1/AOD
# /JetMET0/Run2024C-2024CDEReprocessing-v1/AOD
# /JetMET0/Run2024D-2024CDEReprocessing-v1/AOD
# /JetMET0/Run2024E-2024CDEReprocessing-v1/AOD
# /JetMET1/Run2024C-2024CDEReprocessing-v2/AOD
# /JetMET1/Run2024D-2024CDEReprocessing-v1/AOD
# /JetMET1/Run2024E-2024CDEReprocessing-v1/AOD

# psetName = './configs/Data24_CDEReprocessing_AODtoPFNano.py'
# tag = "CDEReprocessing"

# outputDatasetTag = f'Run2024C_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/ZeroBias/Run2024C-2024CDEReprocessing-v1/AOD"

# outputDatasetTag = f'Run2024D_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/ZeroBias/Run2024D-2024CDEReprocessing-v1/AOD"

# outputDatasetTag = f'Run2024E_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/ZeroBias/Run2024E-2024CDEReprocessing-v1/AOD"

# psetName = './configs/Data24_CDEReprocessing_SkimSinglePhotonHLT_AODtoPFNano.py'
# tag = "CDEReprocessing"

# outputDatasetTag = f'Run2024C_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/EGamma0/Run2024C-2024CDEReprocessing-v1/AOD"

# outputDatasetTag = f'Run2024D_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/EGamma0/Run2024D-2024CDEReprocessing-v1/AOD"

# outputDatasetTag = f'Run2024E_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/EGamma0/Run2024E-2024CDEReprocessing-v1/AOD"

# outputDatasetTag = f'Run2024C_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/EGamma1/Run2024C-2024CDEReprocessing-v1/AOD"

# outputDatasetTag = f'Run2024D_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/EGamma1/Run2024D-2024CDEReprocessing-v1/AOD"

# outputDatasetTag = f'Run2024E_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/EGamma1/Run2024E-2024CDEReprocessing-v1/AOD"

psetName = './configs/Data24_CDEReprocessing_SkimDiMuonHLT_AODtoPFNano.py'
tag = "CDEReprocessing"

# outputDatasetTag = f'Run2024C_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/Muon0/Run2024C-2024CDEReprocessing-v1/AOD"

# outputDatasetTag = f'Run2024D_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/Muon0/Run2024D-2024CDEReprocessing-v1/AOD"

# outputDatasetTag = f'Run2024E_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/Muon0/Run2024E-2024CDEReprocessing-v1/AOD"

# outputDatasetTag = f'Run2024C_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/Muon1/Run2024C-2024CDEReprocessing-v1/AOD"

# outputDatasetTag = f'Run2024D_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/Muon1/Run2024D-2024CDEReprocessing-v1/AOD"

# outputDatasetTag = f'Run2024E_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/Muon1/Run2024E-2024CDEReprocessing-v1/AOD"

# psetName = './configs/Data24_CDEReprocessing_SkimJetHLT_AODtoPFNano.py'
# tag = "CDEReprocessing_SkimJetHLT"

# outputDatasetTag = f'Run2024C_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/JetMET0/Run2024C-2024CDEReprocessing-v1/AOD"

# outputDatasetTag = f'Run2024D_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/JetMET0/Run2024D-2024CDEReprocessing-v1/AOD"

# outputDatasetTag = f'Run2024E_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/JetMET0/Run2024E-2024CDEReprocessing-v1/AOD"

# outputDatasetTag = f'Run2024C_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/JetMET1/Run2024C-2024CDEReprocessing-v2/AOD"

# outputDatasetTag = f'Run2024D_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/JetMET1/Run2024D-2024CDEReprocessing-v1/AOD"

# outputDatasetTag = f'Run2024E_PromptRecoNano_v1_{tag}_{prod_name}_{prod_version}'
# inputDataset = "/JetMET1/Run2024E-2024CDEReprocessing-v1/AOD"

## /SingleNeutrino_Par-E-10_gun/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM
## /GJ-4Jets-2NLO2LO_Bin-PTG-25_Par-BiasedPTG_TuneSherpaDef_13p6TeV_sherpaMEPS/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM

# psetName = './configs/Summer24_AODSIMtoPFNano.py'
# outputDatasetTag = f'RunIII2024Summer24NanoAOD_{prod_name}_{prod_version}'
# inputDataset = "/SingleNeutrino_Par-E-10_gun/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"

# psetName = './configs/Summer24_SkimSinglePhotonHLT_AODSIMtoPFNano.py'
# outputDatasetTag = f'RunIII2024Summer24NanoAOD_SkimSinglePhotonHLT_{prod_name}_{prod_version}'
# outputDatasetTag = f'RunIII2024Summer24NanoAOD_SkimSinglePhotonHLT_{prod_name}_{prod_version}_prodv3'

# inputDataset = "/GJ-4Jets_Bin-HT-40to100-PTG-10to100_Par-dRGJ-0p25_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"
# inputDataset = "/GJ-4Jets_Bin-HT-100to200-PTG-10to100_Par-dRGJ-0p25_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"
# inputDataset = "/GJ-4Jets_Bin-HT-200to400-PTG-10to100_Par-dRGJ-0p25_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"

#
# Special case. Read from input file because AODSIM still in PRODUCTION
#
# inputDataset = "/GJ-4Jets_Bin-HT-400to600-PTG-10to100_Par-dRGJ-0p25_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v3/AODSIM"
# userInputFilesTxt = './SampleFileList/GJ-4Jets_Bin-HT-400to600-PTG-10to100_Par-dRGJ-0p25_TuneCP5_13p6TeV_madgraphMLM-pythia8_RunIII2024Summer24DRPremix_AODSIM.txt'
# readInputFileList = True
# outputPrimaryDatasetName = inputDataset.split('/')[1]

# inputDataset = "/GJ-4Jets_Bin-HT-600to1000-PTG-10to100_Par-dRGJ-0p25_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"
# inputDataset = "/GJ-4Jets_Bin-HT-1000-PTG-10to100_Par-dRGJ-0p25_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"

# inputDataset = "/GJ-4Jets_Bin-HT-40to200-PTG-100to200_Par-dRGJ-0p25_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"
# inputDataset = "/GJ-4Jets_Bin-HT-200to400-PTG-100to200_Par-dRGJ-0p25_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"
# inputDataset = "/GJ-4Jets_Bin-HT-400to600-PTG-100to200_Par-dRGJ-0p25_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"
# inputDataset = "/GJ-4Jets_Bin-HT-600to1000-PTG-100to200_Par-dRGJ-0p25_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"
# inputDataset = "/GJ-4Jets_Bin-HT-1000-PTG-100to200_Par-dRGJ-0p25_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"

# inputDataset = "/GJ-4Jets_Bin-HT-40to400-PTG-200_Par-dRGJ-0p25_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"
# inputDataset = "/GJ-4Jets_Bin-HT-400to600-PTG-200_Par-dRGJ-0p25_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"
# inputDataset = "/GJ-4Jets_Bin-HT-600to1000-PTG-200_Par-dRGJ-0p25_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"
# inputDataset = "/GJ-4Jets_Bin-HT-1000-PTG-200_Par-dRGJ-0p25_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"

# inputDataset = "/GJ-4Jets-2NLO2LO_Bin-PTG-25_Par-BiasedPTG_TuneSherpaDef_13p6TeV_sherpaMEPS/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"

#
# Special case. Read from input file because AODSIM still in PRODUCTION
#
#psetName = './configs/Summer24_SkimDiMuonHLT_AODSIMtoPFNano.py'
#outputDatasetTag = f'RunIII2024Summer24NanoAOD_SkimDiMuonHLT_{prod_name}_{prod_version}_prodv4'
#inputDataset = "/DYto2Mu-4Jets_Bin-MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v3/AODSIM"
## userInputFilesTxt = './SampleFileList/DYto2Mu-4Jets_Bin-MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8_RunIII2024Summer24DRPremix_AODSIM.txt'
## readInputFileList = True
#outputPrimaryDatasetName = inputDataset.split('/')[1]

# psetName = './configs/Summer24_AODSIMtoPFNano.py'
# outputDatasetTag = f'RunIII2024Summer24NanoAOD_{prod_name}_{prod_version}_prodv3'

# psetName = './configs/Summer24_SkimJetHLT_AODSIMtoPFNano.py'
# outputDatasetTag = f'RunIII2024Summer24NanoAOD_SkimJetHLT_{prod_name}_{prod_version}'

# inputDataset = "/QCD-4Jets_Bin-HT-40to70_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"
# inputDataset = "/QCD-4Jets_Bin-HT-70to100_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"
# inputDataset = "/QCD-4Jets_Bin-HT-100to200_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"
# inputDataset = "/QCD-4Jets_Bin-HT-200to400_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"
# inputDataset = "/QCD-4Jets_Bin-HT-400to600_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"
# inputDataset = "/QCD-4Jets_Bin-HT-600to800_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"
# inputDataset = "/QCD-4Jets_Bin-HT-800to1000_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"
# inputDataset = "/QCD-4Jets_Bin-HT-1000to1200_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"
# inputDataset = "/QCD-4Jets_Bin-HT-1200to1500_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"
# inputDataset = "/QCD-4Jets_Bin-HT-1500to2000_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"
# inputDataset = "/QCD-4Jets_Bin-HT-2000_TuneCP5_13p6TeV_madgraphMLM-pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"
# inputDataset = "/QCD_Bin-PT-15to7000_Par-PT-flat2022_TuneCP5_13p6TeV_pythia8/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM"

# psetName = './configs/Summer24_AODSIMtoPFNano.py'
# outputDatasetTag = f'RunIII2024Summer24NanoAOD_NoPU_{prod_name}_{prod_version}_prodv3'
# inputDataset = "/QCD_Bin-PT-15to7000_Par-PT-flat2022_TuneCP5_13p6TeV_pythia8/RunIII2024Summer24DR-NoPU_140X_mcRun3_2024_realistic_v26-v2/AODSIM"

# psetName = './configs/Summer24_AODSIMtoPFNano.py'
# outputDatasetTag = f'RunIII2024Summer24NanoAOD_EpsilonPU_{prod_name}_{prod_version}_prodv3'
# inputDataset = "/QCD_Bin-PT-15to7000_Par-PT-flat2022_TuneCP5_13p6TeV_pythia8/RunIII2024Summer24DR-EpsilonPU_140X_mcRun3_2024_realistic_v26-v2/AODSIM"

# psetName = './configs/Summer24_AODSIMtoPFNano.py'
# outputDatasetTag = f'RunIII2024Summer24NanoAOD_FlatPU0to120_{prod_name}_{prod_version}_prodv3'
# inputDataset = "/QCD_Bin-PT-15to7000_Par-PT-flat2022_TuneCP5_13p6TeV_pythia8/RunIII2024Summer24DR-FlatPU0to120_140X_mcRun3_2024_realistic_v26-v4/AODSIM"

outputPrimaryDataset = inputDataset.split('/')[1]
outputPrimaryDataset = outputPrimaryDataset.replace("_Bin-PTG-25_Par-BiasedPTG_TuneSherpaDef_13p6TeV","")

outputPrimaryDataset = outputPrimaryDataset.replace("madgraph","mg")
outputPrimaryDataset = outputPrimaryDataset.replace("pythia","py")
outputPrimaryDataset = outputPrimaryDataset.replace("_Par-dRGJ-0p25","")
outputPrimaryDataset = outputPrimaryDataset.replace("_TuneCP5_13p6TeV","")
outputPrimaryDataset = outputPrimaryDataset.replace("Bin-","")

crab_config = config()
crab_config.General.requestName = f'{outputPrimaryDataset}_{outputDatasetTag}'.replace(f"_{prod_name}_{prod_version}","").replace(f"RunIII2024Summer24NanoAOD_","")
crab_config.General.workArea = workArea
crab_config.General.transferOutputs = True
crab_config.General.transferLogs = False

crab_config.JobType.pluginName = 'Analysis'
crab_config.JobType.psetName = psetName
crab_config.JobType.allowUndistributedCMSSW = True
crab_config.JobType.numCores = 1
crab_config.JobType.maxJobRuntimeMin = 990
crab_config.JobType.maxMemoryMB = 4000

if readInputFileList:
  listOfFiles = open(userInputFilesTxt).readlines()
  crab_config.Data.outputPrimaryDataset = outputPrimaryDatasetName
  crab_config.Data.userInputFiles = listOfFiles
else:
  crab_config.Data.inputDataset = inputDataset

# crab_config.Data.inputDBS = 'phys03' # Since we are using USER samples (CHECK)
crab_config.Data.splitting   = 'FileBased'
# crab_config.Data.unitsPerJob = 10 #Data
crab_config.Data.unitsPerJob = 1 #Data
# crab_config.Data.unitsPerJob = 8 #MC
# crab_config.Data.unitsPerJob = 4 #MC
# crab_config.Data.unitsPerJob = 1 #MC

crab_config.Data.lumiMask = "./Cert_Collisions2024_378981_386951_Golden.json" #Data only

# CHECK! For MC
crab_config.Data.allowNonValidInputDataset = True

crab_config.Data.partialDataset = True

crab_config.Data.publication = True
crab_config.Data.outputDatasetTag = outputDatasetTag
crab_config.Data.ignoreLocality = True

outLFNDirBase = f"/store/user/nbinnorj/{prod_name}_{prod_version}/CRABOUTPUT/"
storageSite = "T2_FI_HIP"
crab_config.Data.outLFNDirBase = outLFNDirBase
crab_config.Site.storageSite = storageSite

whitelist_sites=[
'T1_ES_*',
'T1_IT_*',
'T1_FR_*',
'T1_DE_*',
'T1_US_*',
'T2_UK_*',
'T2_RU_*',
'T2_DE_*',
'T2_FR_*',
'T2_FI_*',
'T2_IT_*',
'T2_ES_*',
'T2_HU_*',
'T2_BE_*',
'T2_EE_*',
'T2_CH_*',
'T2_US_*',
]
crab_config.Site.whitelist = whitelist_sites

print("requestName = "+crab_config.General.requestName)
crabCommand('submit', config = crab_config)
