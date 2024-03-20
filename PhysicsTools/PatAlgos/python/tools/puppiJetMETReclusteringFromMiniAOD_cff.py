import FWCore.ParameterSet.Config as cms
from PhysicsTools.PatAlgos.tools.puppiJetMETReclusteringTools import puppiAK4METReclusterFromMiniAOD
from PhysicsTools.PatAlgos.tools.puppiJetMETReclusteringTools import puppiAK8ReclusterFromMiniAOD

def puppiJetMETReclusterFromMiniAOD(process, runOnMC, useExistingWeights=False):

  #
  # AK4 and MET
  #
  from RecoBTag.ONNXRuntime.pfParticleNetFromMiniAODAK4_cff import _pfParticleNetFromMiniAODAK4PuppiCentralJetTagsAll as pfParticleNetFromMiniAODAK4PuppiCentralJetTagsAll
  from RecoBTag.ONNXRuntime.pfParticleNetFromMiniAODAK4_cff import _pfParticleNetFromMiniAODAK4PuppiForwardJetTagsAll as pfParticleNetFromMiniAODAK4PuppiForwardJetTagsAll
  from RecoBTag.ONNXRuntime.pfParticleTransformerAK4_cff import _pfParticleTransformerAK4JetTagsAll as pfParticleTransformerAK4JetTagsAll

  bTagDeepCSV  = ['pfDeepCSVJetTags:probb','pfDeepCSVJetTags:probbb','pfDeepCSVJetTags:probc','pfDeepCSVJetTags:probudsg']

  btagDiscriminatorsAK4 = cms.PSet(
    names=cms.vstring(
      'pfDeepFlavourJetTags:probb',
      'pfDeepFlavourJetTags:probbb',
      'pfDeepFlavourJetTags:problepb',
      'pfDeepFlavourJetTags:probc',
      'pfDeepFlavourJetTags:probuds',
      'pfDeepFlavourJetTags:probg',
    )
    + pfParticleNetFromMiniAODAK4PuppiCentralJetTagsAll
    + pfParticleNetFromMiniAODAK4PuppiForwardJetTagsAll
    + pfParticleTransformerAK4JetTagsAll
    + bTagDeepCSV
  )

  process = puppiAK4METReclusterFromMiniAOD(process, runOnMC,
    useExistingWeights=useExistingWeights,
    btagDiscriminatorsAK4=btagDiscriminatorsAK4
  )

  #
  # AK8
  #
  from RecoBTag.ONNXRuntime.pfDeepBoostedJet_cff import _pfDeepBoostedJetTagsAll as pfDeepBoostedJetTagsAll
  from RecoBTag.ONNXRuntime.pfHiggsInteractionNet_cff import _pfHiggsInteractionNetTagsProbs as pfHiggsInteractionNetTagsProbs
  from RecoBTag.ONNXRuntime.pfParticleNet_cff import _pfParticleNetMassCorrelatedJetTagsAll as pfParticleNetMassCorrelatedJetTagsAll
  from RecoBTag.ONNXRuntime.pfParticleNet_cff import _pfParticleNetMassRegressionOutputs
  from RecoBTag.ONNXRuntime.pfParticleNetFromMiniAODAK8_cff import _pfParticleNetFromMiniAODAK8JetTagsAll as pfParticleNetFromMiniAODAK8JetTagsAll

  btagDiscriminatorsAK8 = cms.PSet(names = cms.vstring(
    'pfMassIndependentDeepDoubleBvLV2JetTags:probQCD',
    'pfMassIndependentDeepDoubleBvLV2JetTags:probHbb',
    'pfMassIndependentDeepDoubleCvLV2JetTags:probQCD',
    'pfMassIndependentDeepDoubleCvLV2JetTags:probHcc',
    'pfMassIndependentDeepDoubleCvBV2JetTags:probHbb',
    'pfMassIndependentDeepDoubleCvBV2JetTags:probHcc',
    )
    + pfParticleNetMassCorrelatedJetTagsAll
    + pfHiggsInteractionNetTagsProbs
    + pfParticleNetFromMiniAODAK8JetTagsAll
  )

  btagDiscriminatorsAK8Subjets = cms.PSet(names = cms.vstring(
    'pfDeepCSVJetTags:probb',
    'pfDeepCSVJetTags:probbb',
    )
  )
  process = puppiAK8ReclusterFromMiniAOD(process, runOnMC,
    useExistingWeights=useExistingWeights,
    btagDiscriminatorsAK8=btagDiscriminatorsAK8,
    btagDiscriminatorsAK8Subjets=btagDiscriminatorsAK8Subjets
  )

  return process

def puppiJetMETReclusterFromMiniAOD_MC(process):
  process = puppiJetMETReclusterFromMiniAOD(process, runOnMC=True)
  return process

def puppiJetMETReclusterFromMiniAOD_Data(process):
  process = puppiJetMETReclusterFromMiniAOD(process, runOnMC=False)
  return process