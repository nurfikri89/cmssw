import FWCore.ParameterSet.Config as cms
from PhysicsTools.PatAlgos.tools.puppiJetMETReclusteringTools import puppiAK4METReclusterFromMiniAOD, puppiAK8ReclusterFromMiniAOD

def puppiJetMETReclusterFromMiniAOD(process, runOnMC, useExistingWeights=False):

  #
  # AK4 and MET
  #
  from RecoBTag.ONNXRuntime.pfParticleNetAK4_cff import _pfParticleNetAK4JetTagsAll as pfParticleNetAK4JetTagsAll

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
    + pfParticleNetAK4JetTagsAll
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
  from RecoBTag.ONNXRuntime.pfParticleNet_cff import _pfParticleNetJetTagsAll as pfParticleNetJetTagsAll
  from RecoBTag.ONNXRuntime.pfParticleNet_cff import _pfParticleNetMassRegressionOutputs

  btagDiscriminatorsAK8 = cms.PSet(names = cms.vstring(
    # 'pfCombinedSecondaryVertexV2BJetTags',
    # 'pfCombinedInclusiveSecondaryVertexV2BJetTags',
    # 'pfCombinedMVAV2BJetTags',
    'pfDeepCSVJetTags:probb',
    'pfDeepCSVJetTags:probc',
    'pfDeepCSVJetTags:probudsg',
    'pfDeepCSVJetTags:probbb',
    'pfBoostedDoubleSecondaryVertexAK8BJetTags',
    'pfMassIndependentDeepDoubleBvLV2JetTags:probQCD',
    'pfMassIndependentDeepDoubleBvLV2JetTags:probHbb',
    'pfMassIndependentDeepDoubleCvLV2JetTags:probQCD',
    'pfMassIndependentDeepDoubleCvLV2JetTags:probHcc',
    'pfMassIndependentDeepDoubleCvBV2JetTags:probHbb',
    'pfMassIndependentDeepDoubleCvBV2JetTags:probHcc',
    )
    + pfDeepBoostedJetTagsAll
    + pfParticleNetJetTagsAll
    + pfHiggsInteractionNetTagsProbs
    + _pfParticleNetMassRegressionOutputs
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

  #
  # Ensure that the PuppiProducers will recompute the puppi weights
  # just in case they were overridden beforehand.
  #
  process.packedpuppi.useExistingWeights = False
  process.packedpuppiNoLep.useExistingWeights = False

  return process

def puppiJetMETReclusterFromMiniAOD_MC(process):
  process = puppiJetMETReclusterFromMiniAOD(process, runOnMC=True)
  return process

def puppiJetMETReclusterFromMiniAOD_Data(process):
  process = puppiJetMETReclusterFromMiniAOD(process, runOnMC=False)
  return process
