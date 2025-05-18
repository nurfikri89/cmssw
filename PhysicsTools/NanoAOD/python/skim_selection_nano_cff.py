import FWCore.ParameterSet.Config as cms

def SetupSkim_HLTDimuon(process):
  import HLTrigger.HLTfilters.hltHighLevel_cfi as hlt
  process.HLTDimuonFilter = hlt.hltHighLevel.clone()
  process.HLTDimuonFilter.TriggerResultsTag = cms.InputTag( "TriggerResults", "", "HLT" )
  process.HLTDimuonFilter.HLTPaths = cms.vstring(
    'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_v*',
    'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v*'
  )
  process.HLTDimuonFilter.throw = cms.bool( False )
  process.skimHLTDimuonSequence = cms.Sequence(process.HLTDimuonFilter)
  process.SKIMHLTDimuon = cms.Path(process.skimHLTDimuonSequence)
  process.schedule.insert(0, process.SKIMHLTDimuon)
  if hasattr(process,"NANOEDMAODoutput") or hasattr(process,"NANOAODoutput"):
    process.NANOAODoutput.SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('SKIMHLTDimuon'))
  elif hasattr(process,"NANOEDMAODSIMoutput") or hasattr(process,"NANOAODSIMoutput"):
    process.NANOAODSIMoutput.SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('SKIMHLTDimuon'))

  return process

def SetupSkim_HLTJet(process):
  import HLTrigger.HLTfilters.hltHighLevel_cfi as hlt
  process.HLTJetFilter = hlt.hltHighLevel.clone()
  process.HLTJetFilter.TriggerResultsTag = cms.InputTag( "TriggerResults", "", "HLT" )
  process.HLTJetFilter.HLTPaths = cms.vstring(
  "HLT_PFJet40_v*",
  "HLT_PFJet60_v*",
  "HLT_PFJet80_v*",
  "HLT_PFJet140_v*",
  "HLT_PFJet200_v*",
  "HLT_PFJet260_v*",
  "HLT_PFJet320_v*",
  "HLT_PFJet400_v*",
  "HLT_PFJet450_v*",
  "HLT_PFJet500_v*",
  "HLT_PFJetFwd40_v*",
  "HLT_PFJetFwd60_v*",
  "HLT_PFJetFwd80_v*",
  "HLT_PFJetFwd140_v*",
  "HLT_PFJetFwd200_v*",
  "HLT_PFJetFwd260_v*",
  "HLT_PFJetFwd320_v*",
  "HLT_PFJetFwd400_v*",
  "HLT_PFJetFwd450_v*",
  "HLT_PFJetFwd500_v*",
  "HLT_DiPFJetAve40_v*",
  "HLT_DiPFJetAve60_v*",
  "HLT_DiPFJetAve80_v*",
  "HLT_DiPFJetAve140_v*",
  "HLT_DiPFJetAve200_v*",
  "HLT_DiPFJetAve260_v*",
  "HLT_DiPFJetAve320_v*",
  "HLT_DiPFJetAve400_v*",
  "HLT_DiPFJetAve500_v*",
  "HLT_DiPFJetAve60_HFJEC_v*",
  "HLT_DiPFJetAve80_HFJEC_v*",
  "HLT_DiPFJetAve100_HFJEC_v*",
  "HLT_DiPFJetAve160_HFJEC_v*",
  "HLT_DiPFJetAve220_HFJEC_v*",
  "HLT_DiPFJetAve300_HFJEC_v*",
  "HLT_AK8PFJetFwd40_v*",
  "HLT_AK8PFJetFwd60_v*",
  "HLT_AK8PFJetFwd80_v*",
  "HLT_AK8PFJetFwd140_v*",
  "HLT_AK8PFJetFwd200_v*",
  "HLT_AK8PFJetFwd260_v*",
  "HLT_AK8PFJetFwd320_v*",
  "HLT_AK8PFJetFwd400_v*",
  "HLT_AK8PFJetFwd450_v*",
  "HLT_AK8PFJetFwd500_v*",
  "HLT_AK8PFJet40_v*",
  "HLT_AK8PFJet60_v*",
  "HLT_AK8PFJet80_v*",
  "HLT_AK8PFJet140_v*",
  "HLT_AK8PFJet200_v*",
  "HLT_AK8PFJet260_v*",
  "HLT_AK8PFJet320_v*",
  "HLT_AK8PFJet400_v*",
  "HLT_AK8PFJet450_v*",
  "HLT_AK8PFJet500_v*",
  "HLT_AK8PFJet550_v*",
  "HLT_AK8PFJet380_SoftDropMass30_v*",
  "HLT_AK8PFJet400_SoftDropMass30_v*",
  "HLT_AK8PFJet425_SoftDropMass30_v*",
  "HLT_AK8PFJet450_SoftDropMass30_v*",
  "HLT_AK8DiPFJet250_250_SoftDropMass40_v*",
  "HLT_AK8DiPFJet250_250_SoftDropMass50_v*",
  "HLT_AK8DiPFJet260_260_SoftDropMass30_v*",
  "HLT_AK8DiPFJet260_260_SoftDropMass40_v*",
  "HLT_AK8DiPFJet270_270_SoftDropMass30_v*",
  "HLT_AK8DiPFJet280_280_SoftDropMass30_v*",
  "HLT_AK8DiPFJet290_290_SoftDropMass30_v*",
  "HLT_CaloJet500_NoJetID_v*",
  "HLT_PFHT1050_v*",
  )
  process.HLTJetFilter.throw = cms.bool( False )
  process.skimHLTJetSequence = cms.Sequence(process.HLTJetFilter)
  process.SKIMHLTJet = cms.Path(process.skimHLTJetSequence)
  process.schedule.insert(0, process.SKIMHLTJet)
  if hasattr(process,"NANOEDMAODoutput") or hasattr(process,"NANOAODoutput"):
    process.NANOAODoutput.SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('SKIMHLTJet'))
  elif hasattr(process,"NANOEDMAODSIMoutput") or hasattr(process,"NANOAODSIMoutput"):
    process.NANOAODSIMoutput.SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('SKIMHLTJet'))

  return process

def SetupSkim_HLTMET(process):
  import HLTrigger.HLTfilters.hltHighLevel_cfi as hlt
  process.HLTMETFilter = hlt.hltHighLevel.clone()
  process.HLTMETFilter.TriggerResultsTag = cms.InputTag( "TriggerResults", "", "HLT" )
  process.HLTMETFilter.HLTPaths = cms.vstring(
    "HLT_PFMET120_PFMHT120_IDTight_v*",
    "HLT_PFMET130_PFMHT130_IDTight_v*",
    "HLT_PFMET140_PFMHT140_IDTight_v*",
    "HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_v*",
    "HLT_PFMETNoMu130_PFMHTNoMu130_IDTight_v*",
    "HLT_PFMETNoMu140_PFMHTNoMu140_IDTight_v*",
    "HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_FilterHF_v*",
    "HLT_PFMETNoMu130_PFMHTNoMu130_IDTight_FilterHF_v*",
    "HLT_PFMETNoMu140_PFMHTNoMu140_IDTight_FilterHF_v*",
    "HLT_MET105_IsoTrk50_v*",
    "HLT_PFMET105_IsoTrk50_v*",
  )
  process.HLTMETFilter.throw = cms.bool( False )
  process.skimHLTMETSequence = cms.Sequence(process.HLTMETFilter)
  process.SKIMHLTMET = cms.Path(process.skimHLTMETSequence)
  process.schedule.insert(0, process.SKIMHLTMET)
  if hasattr(process,"NANOEDMAODoutput") or hasattr(process,"NANOAODoutput"):
    process.NANOAODoutput.SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('SKIMHLTMET'))
  elif hasattr(process,"NANOEDMAODSIMoutput") or hasattr(process,"NANOAODSIMoutput"):
    process.NANOAODSIMoutput.SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('SKIMHLTMET'))

  return process

def SetupSkim_HLTSinglePhoton(process):
  import HLTrigger.HLTfilters.hltHighLevel_cfi as hlt
  process.HLTSinglePhotonFilter = hlt.hltHighLevel.clone()
  process.HLTSinglePhotonFilter.TriggerResultsTag = cms.InputTag( "TriggerResults", "", "HLT" )
  process.HLTSinglePhotonFilter.HLTPaths = cms.vstring(
    'HLT_Photon30EB_TightID_TightIso_v*', #Prescaled in 2024
    'HLT_Photon40EB_TightID_TightIso_v*', #Switched off in 2024, available in 2025
    'HLT_Photon45EB_TightID_TightIso_v*',
    'HLT_Photon50EB_TightID_TightIso_v*',
    'HLT_Photon75EB_TightID_TightIso_v*',
    'HLT_Photon90EB_TightID_TightIso_v*',
    'HLT_Photon110EB_TightID_TightIso_v*',
    'HLT_Photon20_HoverELoose_v*',#Prescaled in 2024
    'HLT_Photon30_HoverELoose_v*',#Prescaled in 2024
    'HLT_Photon33_v*',#Prescaled in 2024
    'HLT_Photon200_v*',
    'HLT_Photon300_NoHE_v*',
  )
  process.HLTSinglePhotonFilter.throw = cms.bool( False )
  process.skimHLTSinglePhotonSequence = cms.Sequence(process.HLTSinglePhotonFilter)
  process.SKIMHLTSinglePhoton = cms.Path(process.skimHLTSinglePhotonSequence)
  process.schedule.insert(0, process.SKIMHLTSinglePhoton)
  if hasattr(process,"NANOEDMAODoutput") or hasattr(process,"NANOAODoutput"):
    process.NANOAODoutput.SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('SKIMHLTSinglePhoton'))
  elif hasattr(process,"NANOEDMAODSIMoutput") or hasattr(process,"NANOAODSIMoutput"):
    process.NANOAODSIMoutput.SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('SKIMHLTSinglePhoton'))

  return process

def SetupSkimForMC_AlwaysRunWeightsTable(process):
  process.genWeightsTableSequence = cms.Sequence(process.genWeightsTable)
  process.genWeightsTablePath = cms.Path(process.genWeightsTableSequence)
  process.schedule.insert(0, process.genWeightsTablePath)
  return process

