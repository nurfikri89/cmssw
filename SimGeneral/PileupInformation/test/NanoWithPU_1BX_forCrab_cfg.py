import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Run3_2023_cff import Run3_2023

process = cms.Process('NANO',Run3_2023)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('PhysicsTools.NanoAOD.nano_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10000),
    # input = cms.untracked.int32(10),
    output = cms.optional.untracked.allowed(cms.int32,cms.PSet)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring( (
        # '/store/mc/Run3Winter24MiniAOD/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/MINIAODSIM/133X_mcRun3_2024_realistic_v10_ext2-v2/80000/9a1687c0-242b-4aa7-8628-d9f4bb12c451.root',
     ) ),
    secondaryFileNames = cms.untracked.vstring( (
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/6b693249-55b6-4f05-9bdd-d49462edbc9e.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/abb2d4a9-699b-4719-920b-eefcacc6bae0.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/b884543e-8339-4a76-b5ec-2ed9a9c617e0.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/ca94e552-c53c-4e41-8a05-aa6ace4c6dc6.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/cdb50915-c3c1-4684-81bf-7db3be11086b.root",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/2b530c27-0d52-413f-83b6-084bab467984.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/8342aaa8-a436-4195-9d0f-d2fb86bc0b92.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/916122e4-dd72-4a7d-a11c-a2432f41eb99.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/afcfcc08-140c-480e-b01b-35358626a316.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/f9f3cb5a-6ffe-431c-ba93-b724b27cc1d0.root",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/1d5cc645-3c24-4689-8dd9-93751c477602.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/2899c6b5-7ac7-4f7f-89e2-4ef720bc64c9.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/2ad11f1b-01ff-4755-88a9-13717f1e3cd2.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/2c8af2b7-1f97-4a8b-b1e7-942ec97eae25.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/83ac4d84-3506-4900-872d-c2fcbe06fa28.root",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/528fc17a-82aa-4dbb-b7b3-2ee22188b784.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/8722c61d-268e-4bb5-9506-46664949e6c2.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/a250338b-e636-405c-a125-e49c6d4672e0.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/a2d19427-d1c7-4612-994d-17c716d9cb8d.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/f905c641-02de-498a-9645-44c1a08011eb.root",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/167bb217-4a7b-4e68-8019-a80f8c935d91.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/4def0c0b-2e60-4421-a699-d5db3364b4ed.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/77408d00-9ecf-4d07-91b3-5649b30e25df.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/9b20c5aa-2573-4262-bfea-470938f6a04f.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/c686fc76-416c-466d-ac77-d0e947bb80cc.root",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/637e419f-9e06-4e57-a5ac-b441f658277d.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/b97e6648-3838-4025-bf66-9a3ae13dd532.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/bc5af6b5-de55-4018-bb4d-f69e7ba616cf.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/c21904ad-169d-4e7f-9d8f-7c0febfe4471.root ",
        # "/store/mc/Run3Winter24Digi/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v10_ext2-v2/80002/fe7b0d03-4621-4560-aa02-9db927d7f584.root",
     ) )
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
    fileName = cms.untracked.string('file:JMEPUNano.root'),
    outputCommands = process.NANOAODSIMEventContent.outputCommands
)

# Additional output definition
#### process.options.numberOfThreads = 4
#### process.options.numberOfStreams = 0

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '133X_mcRun3_2024_realistic_v10', '')

# Path and EndPath definitions
process.nanoAOD_step = cms.Path(process.nanoSequenceMC)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.NANOAODSIMoutput_step = cms.EndPath(process.NANOAODSIMoutput)

# Schedule definition
process.schedule = cms.Schedule(process.nanoAOD_step,process.endjob_step,process.NANOAODSIMoutput_step)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)

# customisation of the process.

# Automatic addition of the customisation function from Configuration.DataProcessing.Utils
from Configuration.DataProcessing.Utils import addMonitoring 

#call to customisation function addMonitoring imported from Configuration.DataProcessing.Utils
process = addMonitoring(process)

# Automatic addition of the customisation function from PhysicsTools.NanoAOD.nano_cff
from PhysicsTools.NanoAOD.nano_cff import nanoAOD_customizeCommon 

#call to customisation function nanoAOD_customizeCommon imported from PhysicsTools.NanoAOD.nano_cff
process = nanoAOD_customizeCommon(process)

from PhysicsTools.NanoAOD.custom_jme_cff import PrepJMECustomNanoAOD_MC 
process = PrepJMECustomNanoAOD_MC(process)
#

from PhysicsTools.NanoAOD.common_cff import *

process.finalJetsPuppi.cut = "(pt >= 5) || ((pt < 5) && (genJetFwdRef().backRef().isNonnull()))"
jetPuppiTableDoc = "AK4 PF Puppi jets with JECs applied. Jets with pt >= 5 GeV are stored."
jetPuppiTableDoc += "For jets with pt < 5 GeV, only those matched to AK4 Gen jets are stored."
process.jetPuppiTable.doc = jetPuppiTableDoc

process.finalJets.cut = "(pt >= 5) || ((pt < 5) && (genJetFwdRef().backRef().isNonnull()))"
jetTableDoc = "AK4 PF CHS jets with JECs applied. Jets with pt >= 5 GeV are stored."
jetTableDoc += "For jets with pt < 5 GeV, only those matched to AK4 Gen jets are stored."
process.jetTable.doc = jetTableDoc

process.jetMCTable.variables.genJetIdx = Var("?genJetFwdRef().backRef().isNonnull()?genJetFwdRef().backRef().key():-1", "int16", doc="index of matched gen jet")

process.genJetTable.variables.chargedHadronEnergy = Var("chargedHadronEnergy()", float,doc="chargedHadronEnergy()",precision=12)
process.genJetTable.variables.neutralHadronEnergy = Var("neutralHadronEnergy()", float,doc="neutralHadronEnergy()",precision=12)
process.genJetTable.variables.chargedEmEnergy = Var("chargedEmEnergy()", float,doc="chargedEmEnergy()",precision=12)
process.genJetTable.variables.neutralEmEnergy = Var("neutralEmEnergy()", float,doc="neutralEmEnergy()",precision=12)
process.genJetTable.variables.muonEnergy = Var("muonEnergy()", float,doc="muonEnergy()",precision=12)
process.genJetTable.variables.chargedHadronMultiplicity = Var("chargedHadronMultiplicity()", "uint8", doc="chargedHadronMultiplicity()")
process.genJetTable.variables.neutralHadronMultiplicity = Var("neutralHadronMultiplicity()", "uint8", doc="neutralHadronMultiplicity()")
process.genJetTable.variables.chargedEmMultiplicity = Var("chargedEmMultiplicity()", "uint8", doc="chargedEmMultiplicity()")
process.genJetTable.variables.neutralEmMultiplicity = Var("neutralEmMultiplicity()", "uint8", doc="neutralEmMultiplicity()")
process.genJetTable.variables.muonMultiplicity = Var("muonMultiplicity()", "uint8", doc="muonMultiplicity()")

process.jetPuppiTable.variables.hoEF = Var("hoEnergyFraction()", float, doc="HO Energy Fraction", precision=12)
process.jetTable.variables.hoEF = Var("hoEnergyFraction()", float, doc="HO Energy Fraction", precision=12)

# End of customisation functions

process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService",
  playBack = cms.PSet(
    engineName = cms.untracked.string('MixMaxRng'),
    initialSeed = cms.untracked.uint32(12345)
  ),
)


from SimGeneral.PileupInformation.inputFilesForMixing_Winter24_cfi import fileNamesMinBias_Winter24
process.GenPUJetExtractor = cms.EDProducer("GenPUJetExtractor",
  bunchIdx = cms.int32(0),
  input = cms.SecSource("EmbeddedRootSource",
    type = cms.string('fixed'),
    nbPileupEvents = cms.PSet(
      averageNumber = cms.double(1.0)
    ),
    fileNames = fileNamesMinBias_Winter24
  )
)

from PhysicsTools.NanoAOD.nanogen_cff import nanoMetadata as _nanoMetadata
from PhysicsTools.NanoAOD.jetMC_cff import genJetTable as _genJetTable
from PhysicsTools.NanoAOD.genVertex_cff import genVertexTable as _genVertexTable
from PhysicsTools.NanoAOD.genVertex_cff import genVertexT0Table as _genVertexT0Table
from PhysicsTools.NanoAOD.simpleXYZPointFlatTableProducerV2_cfi import simpleXYZPointFlatTableProducerV2 as _genPUVertexTable

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


process.nanoPileUpInfoTask = cms.Task(
    process.GenPUJetExtractor, process.genJetPileUpTable, process.genPUVertexTable
)
process.nanoSequenceMC += (cms.Sequence(process.nanoPileUpInfoTask))

# Customisation from command line

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion
