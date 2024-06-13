import FWCore.ParameterSet.Config as cms

process = cms.Process("PLAYBACK")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(5),
    # input = cms.untracked.int32(-1),
)

process.source = cms.Source("PoolSource",
  # fileNames = cms.untracked.vstring('file:/afs/cern.ch/work/n/nbinnorj/TestMixing/samples/Run3Winter24Digi/QCD_PT-15to7000_TuneCP5_Flat2022_13p6TeV_pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10-v2/2550001/44425c49-648b-42b9-a117-45ee3a4b6735.root')
  fileNames = cms.untracked.vstring('file:/afs/cern.ch/work/n/nbinnorj/TestMixing/samples/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/2520010/9b0e6012-35c5-4500-b6e7-ecc23d5b3dd5.root')

)

process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService",
  playBack = cms.PSet(
    engineName = cms.untracked.string('MixMaxRng'),
    initialSeed = cms.untracked.uint32(12345)
  ),
)

from test_playback_helpers import fileNamesMinBias
# process.playBack = cms.EDAnalyzer("TestPlayback",
#   input = cms.SecSource("EmbeddedRootSource",
#     type = cms.string('fixed'),
#     nbPileupEvents = cms.PSet(
#       averageNumber = cms.double(1.0)
#     ),
#     fileNames = fileNamesMinBias
#   )
# )
# process.p = cms.Path(process.playBack)

process.GenPUJetExtractor = cms.EDProducer("GenPUJetExtractor",
  input = cms.SecSource("EmbeddedRootSource",
    type = cms.string('fixed'),
    nbPileupEvents = cms.PSet(
      averageNumber = cms.double(1.0)
    ),
    fileNames = fileNamesMinBias
  )
)
# process.p = cms.Path(process.GenPUJetExtractor)



from PhysicsTools.NanoAOD.common_cff import *
from PhysicsTools.NanoAOD.nanogen_cff import nanoMetadata as _nanoMetadata
from PhysicsTools.NanoAOD.jetMC_cff import genJetTable as _genJetTable
from PhysicsTools.NanoAOD.genVertex_cff import genVertexTable as _genVertexTable
from PhysicsTools.NanoAOD.genVertex_cff import genVertexT0Table as _genVertexT0Table

process.nanoMetadata = _nanoMetadata.clone()

_genJetTableTemp = _genJetTable.clone()
_genJetTableTemp.variables.chargedHadronEnergy = Var("chargedHadronEnergy()", float,doc="chargedHadronEnergy()",precision=12)
_genJetTableTemp.variables.neutralHadronEnergy = Var("neutralHadronEnergy()", float,doc="neutralHadronEnergy()",precision=12)
_genJetTableTemp.variables.chargedEmEnergy     = Var("chargedEmEnergy()", float,doc="chargedEmEnergy()",precision=12)
_genJetTableTemp.variables.neutralEmEnergy     = Var("neutralEmEnergy()", float,doc="neutralEmEnergy()",precision=12)
_genJetTableTemp.variables.muonEnergy          = Var("muonEnergy()", float,doc="muonEnergy()",precision=12)
_genJetTableTemp.variables.chargedHadronMultiplicity = Var("chargedHadronMultiplicity()", "uint8", doc="chargedHadronMultiplicity()")
_genJetTableTemp.variables.neutralHadronMultiplicity = Var("neutralHadronMultiplicity()", "uint8", doc="neutralHadronMultiplicity()")
_genJetTableTemp.variables.chargedEmMultiplicity = Var("chargedEmMultiplicity()", "uint8", doc="chargedEmMultiplicity()")
_genJetTableTemp.variables.neutralEmMultiplicity = Var("neutralEmMultiplicity()", "uint8", doc="neutralEmMultiplicity()")
_genJetTableTemp.variables.muonMultiplicity = Var("muonMultiplicity()", "uint8", doc="muonMultiplicity()")

# nMaxPUEvent=100
# for iPU in range(0,nMaxPUEvent):
#   setattr(process,f"genJet{iPU}Table", _genJetTableTemp.clone())
#   getattr(process,f"genJet{iPU}Table").name=f"PUEvt{iPU}GenJet"
#   getattr(process,f"genJet{iPU}Table").src= cms.InputTag("GenPUJetExtractor",f"ak4GenJetsNoNuPUEvent{iPU}")
#   getattr(process,f"genJet{iPU}Table").cut=""
#   setattr(process,f"genVertex{iPU}Table", _genVertexTable.clone())
#   getattr(process,f"genVertex{iPU}Table").name=f"PUEvt{iPU}GenVtx"
#   getattr(process,f"genVertex{iPU}Table").src= cms.InputTag("GenPUJetExtractor",f"xyz0PUEvent{iPU}")
#   setattr(process,f"genVertex{iPU}T0Table", _genVertexT0Table.clone())
#   getattr(process,f"genVertex{iPU}T0Table").name=f"PUEvt{iPU}GenVtx"
#   getattr(process,f"genVertex{iPU}T0Table").variables = cms.PSet(
#      t = ExtVar(cms.InputTag("GenPUJetExtractor",f"t0PUEvent{iPU}"), "float", doc = "gen vertex t0", precision=12)
#   )

process.genJetPileUpTable = _genJetTableTemp.clone()
process.genJetPileUpTable.name = f"PileUpGenJet"
process.genJetPileUpTable.src = cms.InputTag("GenPUJetExtractor","ak4GenJetsNoNuFromPU")
process.genJetPileUpTable.cut = "(pt >= 5)"
process.genJetPileUpTable.externalVariables = cms.PSet(
  PUEventIdx = ExtVar(cms.InputTag("GenPUJetExtractor","ak4GenJetsNoNuFromPUEventIdx"), int,  doc="PUEventIdx"),
)

from PhysicsTools.NanoAOD.simpleXYZPointFlatTableProducerV2_cfi import simpleXYZPointFlatTableProducerV2 as _genPUVertexTable
process.genPUVertexTable = _genPUVertexTable.clone()
process.genPUVertexTable.name = "PileUpGenVtx"
process.genPUVertexTable.src  = cms.InputTag("GenPUJetExtractor","xyz0PUEvent")
process.genPUVertexTable.doc  = "Pileup GenVtx"
process.genPUVertexTable.variables = cms.PSet(
   x = Var("X", float, doc="gen vertex x", precision=10),
   y = Var("Y", float, doc="gen vertex y", precision=10),
   z = Var("Z", float, doc="gen vertex z", precision=16),
)
process.genPUVertexTable.externalVariables = cms.PSet(
  t = ExtVar(cms.InputTag("GenPUJetExtractor", "t0PUEvent"), float,  doc="t", precision=16),
)

process.nanogenSequence = cms.Sequence(
  process.nanoMetadata +
  process.GenPUJetExtractor
)
# for iPU in range(0,nMaxPUEvent):
#   process.nanogenSequence += getattr(process,f"genJet{iPU}Table")
#   process.nanogenSequence += getattr(process,f"genVertex{iPU}Table")
#   process.nanogenSequence += getattr(process,f"genVertex{iPU}T0Table")

process.nanogenSequence += process.genJetPileUpTable
process.nanogenSequence += process.genPUVertexTable

process.nanoAOD_step = cms.Path(process.nanogenSequence)


from PhysicsTools.NanoAOD.NanoAODEDMEventContent_cff import NANOAODGENEventContent
process.NANOAODGENoutput = cms.OutputModule("NanoAODOutputModule",
  SelectEvents = cms.untracked.PSet(),
  compressionAlgorithm = cms.untracked.string('LZMA'),
  compressionLevel = cms.untracked.int32(9),
  dataset = cms.untracked.PSet(
      dataTier = cms.untracked.string('NANOAOD'),
      filterName = cms.untracked.string('')
  ),
  # fileName = cms.untracked.string('file:NanoGen_QCD_Flat2022.root'),
  fileName = cms.untracked.string('file:NanoGen_DYto2L.root'),
  outputCommands = NANOAODGENEventContent.outputCommands
)
process.NANOAODGENoutput_step = cms.EndPath(process.nanogenSequence+process.NANOAODGENoutput)




