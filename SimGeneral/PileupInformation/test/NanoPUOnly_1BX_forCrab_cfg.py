import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Run3_2023_cff import Run3_2023

process = cms.Process('NANOPU',Run3_2023)

process.maxEvents = cms.untracked.PSet(
    # input = cms.untracked.int32(10000),
    # input = cms.untracked.int32(10),
    input = cms.untracked.int32(-1),
    output = cms.optional.untracked.allowed(cms.int32,cms.PSet)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring((
      # '/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/6b693249-55b6-4f05-9bdd-d49462edbc9e.root',
    )),
)

process.options = cms.untracked.PSet(
    IgnoreCompletely = cms.untracked.vstring(),
    Rethrow = cms.untracked.vstring(),
    TryToContinue = cms.untracked.vstring(),
    accelerators = cms.untracked.vstring('*'),
    allowUnscheduled = cms.obsolete.untracked.bool,
    canDeleteEarly = cms.untracked.vstring(),
    deleteNonConsumedUnscheduledModules = cms.untracked.bool(True),
    dumpOptions = cms.untracked.bool(False),
    emptyRunLumiMode = cms.obsolete.untracked.string,
    eventSetup = cms.untracked.PSet(
        forceNumberOfConcurrentIOVs = cms.untracked.PSet(
            allowAnyLabel_=cms.required.untracked.uint32
        ),
        numberOfConcurrentIOVs = cms.untracked.uint32(0)
    ),
    fileMode = cms.untracked.string('FULLMERGE'),
    forceEventSetupCacheClearOnNewRun = cms.untracked.bool(False),
    holdsReferencesToDeleteEarly = cms.untracked.VPSet(),
    makeTriggerResults = cms.obsolete.untracked.bool,
    modulesToCallForTryToContinue = cms.untracked.vstring(),
    modulesToIgnoreForDeleteEarly = cms.untracked.vstring(),
    numberOfConcurrentLuminosityBlocks = cms.untracked.uint32(0),
    numberOfConcurrentRuns = cms.untracked.uint32(1),
    numberOfStreams = cms.untracked.uint32(0),
    numberOfThreads = cms.untracked.uint32(1),
    printDependencies = cms.untracked.bool(False),
    sizeOfStackForThreadsInKB = cms.optional.untracked.uint32,
    throwIfIllegalParameter = cms.untracked.bool(True),
    wantSummary = cms.untracked.bool(False)
)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('--python_filename nevts:1000'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition

process.NANOAODSIMoutput = cms.OutputModule("NanoAODOutputModule",
    compressionAlgorithm = cms.untracked.string('LZMA'),
    compressionLevel = cms.untracked.int32(9),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('NANOAODSIM'),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string('file:PUNano.root'),
    outputCommands = cms.untracked.vstring(
         'drop *',
         "keep nanoaodFlatTable_*Table_*_*",     # event data
         "keep String_*_genModel_*",  # generator model data
         "keep nanoaodMergeableCounterTable_*Table_*_*", # accumulated per/run or per/lumi data
         "keep nanoaodUniqueString_nanoMetadata_*_*",   # basic metadata
    )
)

# Additional output definition

process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService",
  playBack = cms.PSet(
    engineName = cms.untracked.string('MixMaxRng'),
    initialSeed = cms.untracked.uint32(12345)
  ),
)


from SimGeneral.PileupInformation.inputFilesForMixing_Winter24_cfi import fileNamesMinBias_Winter24
process.GenPUJetExtractor = cms.EDProducer("GenPUJetExtractor",
  bunchIdx = cms.int32(0),
  # saveGenParticles = cms.bool(True),
  saveGenParticles = cms.bool(False),
  input = cms.SecSource("EmbeddedRootSource",
    type = cms.string('fixed'),
    nbPileupEvents = cms.PSet(
      averageNumber = cms.double(1.0)
    ),
    fileNames = fileNamesMinBias_Winter24
  )
)

from PhysicsTools.NanoAOD.common_cff import *
from PhysicsTools.NanoAOD.nanogen_cff import nanoMetadata as _nanoMetadata
from PhysicsTools.NanoAOD.jetMC_cff import genJetTable as _genJetTable
from PhysicsTools.NanoAOD.genVertex_cff import genVertexTable as _genVertexTable
from PhysicsTools.NanoAOD.genVertex_cff import genVertexT0Table as _genVertexT0Table
from PhysicsTools.NanoAOD.simpleXYZPointFlatTableProducerV2_cfi import simpleXYZPointFlatTableProducerV2 as _genPUVertexTable
from PhysicsTools.NanoAOD.genparticles_cff import genParticleTable as _genParticleTable


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

#
# BX0
#
process.genJetPileUpTable = _genJetTableTemp.clone()
process.genJetPileUpTable.name = f"PileUpGenJet"
process.genJetPileUpTable.src = cms.InputTag("GenPUJetExtractor","ak4GenJetsNoNuFromPU")
process.genJetPileUpTable.cut = "(pt >= 5)"
process.genJetPileUpTable.externalVariables = cms.PSet(
  PUEventIdx = ExtVar(cms.InputTag("GenPUJetExtractor","ak4GenJetsNoNuFromPUEventIdx"), int,  doc="PUEventIdx"),
)

process.genPUVertexTable = _genPUVertexTable.clone()
process.genPUVertexTable.name = "PileUpGenVtx"
process.genPUVertexTable.src  = cms.InputTag("GenPUJetExtractor","PUEventXYZ")
process.genPUVertexTable.doc  = "Pileup GenVtx"
process.genPUVertexTable.variables = cms.PSet(
   x = Var("X", float, doc="gen vertex x", precision=10),
   y = Var("Y", float, doc="gen vertex y", precision=10),
   z = Var("Z", float, doc="gen vertex z", precision=16),
)
process.genPUVertexTable.externalVariables = cms.PSet(
  t = ExtVar(cms.InputTag("GenPUJetExtractor", "PUEventT"), float,  doc="t", precision=16),
)


# process.genParticlePileUpTable = _genParticleTable.clone()
# process.genParticlePileUpTable.name = f"PileUpGenPart"
# process.genParticlePileUpTable.src = cms.InputTag("GenPUJetExtractor","genParticlesStatusOneFromPU")
# process.genParticlePileUpTable.cut = ""
# process.genParticlePileUpTable.externalVariables = cms.PSet(
#   PUEventIdx = ExtVar(cms.InputTag("GenPUJetExtractor","genParticlesStatusOneFromPUEventIdx"), int,  doc="PUEventIdx"),
# )
# process.genParticlePileUpTable.variables.pt.precision = 15
# process.genParticlePileUpTable.variables.eta.precision = 15
# process.genParticlePileUpTable.variables.phi.precision = 15
# process.genParticlePileUpTable.variables.mass.precision = 15

# del process.genParticlePileUpTable.variables.statusFlags
# del process.genParticlePileUpTable.variables.genPartIdxMother
# del process.genParticlePileUpTable.variables.status

#
#
#
process.nanoPileUpInfoTask = cms.Task(
  process.nanoMetadata, 
  process.GenPUJetExtractor, 
  process.genJetPileUpTable, 
  process.genPUVertexTable, 
  # process.genParticlePileUpTable
)
process.nanoSequenceMC = cms.Sequence(process.nanoPileUpInfoTask)

#
# Path and EndPath definitions
#
process.nanoAOD_step = cms.Path(process.nanoSequenceMC)
process.NANOAODSIMoutput_step = cms.EndPath(process.NANOAODSIMoutput)
process.schedule = cms.Schedule(process.nanoAOD_step,process.NANOAODSIMoutput_step)


# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion
