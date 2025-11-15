import FWCore.ParameterSet.Config as cms

# nEvents = 30000
nEvents = -1

filename = "root://eosuser.cern.ch//eos/user/n/nbinnorj/Samples/Mini/store/data/Run2024F/MuonEG/MINIAOD/MINIv6NANOv15-v2/2810000/b3b8dd58-c30a-4f1f-876d-1b45af03bb14.root"

process = cms.Process("SelectElMuPlusJets")

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('DQMServices.Core.DQMStoreNonLegacy_cff')
process.load('DQMOffline.Configuration.DQMOfflineMC_cff')
process.load('Configuration.EventContent.EventContent_cff')

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '150X_dataRun3_v6', '')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(nEvents))

process.source = cms.Source("PoolSource",
fileNames = cms.untracked.vstring(filename))

#
# HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v
# HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v
#


#
# Define Loose and Signal Muons. Count them and filter events
#
process.looseMuonsBTVDQM = cms.EDFilter("PATMuonSelector",
    src = cms.InputTag("slimmedMuons"),
    cut = cms.string("(pt > 20) && (abs(eta)<2.4) && (passed('CutBasedIdLoose'))"),
    filter = cms.bool(False)
)
process.signalMuonsBTVDQM = cms.EDFilter("PATMuonRefSelector",
    src = cms.InputTag("looseMuonsBTVDQM"),
    cut = cms.string("(pt > 30) && passed('CutBasedIdMediumPrompt')"),
    filter = cms.bool(False)
)
process.nLooseMuonsCountBTVDQM = cms.EDFilter("PATCandViewCountFilter",
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(1),
    src = cms.InputTag("looseMuonsBTVDQM")
)
process.nSignalMuonsCountBTVDQM = cms.EDFilter("PATCandViewCountFilter",
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(1),
    src = cms.InputTag("signalMuonsBTVDQM")
)


#
# Define Loose and Signal Electrons. Count them and filter events
#
process.looseElectronsBTVDQM = cms.EDFilter("PATElectronSelector",
    src = cms.InputTag("slimmedElectrons"),
    cut = cms.string(
        "(pt > 20) "
        "&& ((abs(superCluster().eta())<2.4) && (abs(superCluster().eta())<1.4442) || (abs(superCluster().eta())>1.5660)) "
        "&& (electronID('cutBasedElectronID-RunIIIWinter22-V1-loose')) "
    ),
    filter = cms.bool(False)
)
process.signalElectronsBTVDQM = cms.EDFilter("PATElectronRefSelector",
    src = cms.InputTag("looseElectronsBTVDQM"),
    cut = cms.string("(pt > 30) && electronID('cutBasedElectronID-RunIIIWinter22-V1-tight')"),
    filter = cms.bool(False)
)
process.nLooseElectronsCountBTVDQM = cms.EDFilter("PATCandViewCountFilter",
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(1),
    src = cms.InputTag("looseElectronsBTVDQM")
)
process.nSignalElectronsCountBTVDQM = cms.EDFilter("PATCandViewCountFilter",
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(1),
    src = cms.InputTag("signalElectronsBTVDQM")
)


process.diLepElMuCandsBTVDQM = cms.EDProducer("CandViewShallowCloneCombiner",
    decay = cms.string("signalMuonsBTVDQM@+ signalElectronsBTVDQM@-"),
    # cut = cms.string("(mass > 20) && (charge=0)")
    cut = cms.string("")
)
process.diLepElMuCandsFilter = cms.EDFilter("CandViewCountFilter",
    src = cms.InputTag("diLepElMuCandsBTVDQM"),
    minNumber = cms.uint32(1),
)


#
# merge signal muons and signal electrons
# Use it for overlap removal of signal jets
#
process.signalLeptonsBTVDQM = cms.EDProducer("CandViewMerger",
  src = cms.VInputTag("signalMuonsBTVDQM", "signalElectronsBTVDQM")
)

#
#
#
from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection
from RecoBTag.ONNXRuntime.pfUnifiedParticleTransformerAK4_cff import _pfUnifiedParticleTransformerAK4JetTagsAll as pfUnifiedParticleTransformerAK4JetTagsAll

_btagDiscriminators = []
_btagDiscriminators += pfUnifiedParticleTransformerAK4JetTagsAll

updateJetCollection(
    process,
    jetSource=cms.InputTag('slimmedJetsPuppi'),
    jetCorrections=('AK4PFPuppi',cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute','L2L3Residual']), 'None'),
    btagDiscriminators=_btagDiscriminators,
    postfix='PuppiWithDeepInfo',
)

process.selectedJetsBTVDQM = cms.EDFilter("PATJetSelector",
    src = cms.InputTag("selectedUpdatedPatJetsPuppiWithDeepInfo"),
    cut = cms.string(
        "(pt > 30) && (abs(eta) < 2.5)"
    ),
    filter = cms.bool(False)
)
process.signalJetsBTVDQM = cms.EDFilter("PATJetDeltaROverlapExclusionSelector",
   src = cms.InputTag('selectedJetsBTVDQM'),
   overlap = cms.InputTag('signalLeptonsBTVDQM'),
   maxDeltaR = cms.double(0.4),
)
process.nSignalJetsBTVDQM = cms.EDFilter("PATCandViewCountFilter",
    minNumber = cms.uint32(2),
    maxNumber = cms.uint32(9999),
    src = cms.InputTag("signalJetsBTVDQM")
)
process.twoHighestPtSignalJetsBTVDQM = cms.EDFilter('PATJetSelector',
    src = cms.InputTag("signalJetsBTVDQM"),
    cutLoose = cms.string("pt>0"),
    nLoose = cms.uint32(2),
    filter = cms.bool(False)
)

from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer
process.patJetsPuppiBarrelTagInfoAnalyzerDQM = DQMEDAnalyzer('MiniAODTagInfoAnalyzer', cms.PSet(
    jets = cms.InputTag('twoHighestPtSignalJetsBTVDQM'),
    jetTagInfos = cms.vstring(
        "pfUnifiedParticleTransformerAK4TagInfosPuppiWithDeepInfo",
    ),
    ptMin = cms.double(30.),
    absEtaMin = cms.double(0.0),
    absEtaMax = cms.double(1.3),
    jetPartonFlavour = cms.int32(-1),#Inclusive flavour since DQM is for data
    jetCutStr = cms.string("absEta0p0To1p3")
   )
)

process.patJetsPuppiEndcapOuterTagInfoAnalyzerDQM = process.patJetsPuppiBarrelTagInfoAnalyzerDQM.clone(
    absEtaMin = cms.double(1.3),
    absEtaMax = cms.double(2.0),
    jetCutStr = cms.string("absEta1p3To2p0")
)

process.patJetsPuppiEndcapInnerTagInfoAnalyzerDQM = process.patJetsPuppiBarrelTagInfoAnalyzerDQM.clone(
    absEtaMin = cms.double(2.0),
    absEtaMax = cms.double(2.5),
    jetCutStr = cms.string("absEta2p0To2p5")
)

#
# This will dump a new MiniAOD, with the same content as input MiniAOD plus with the new collections added
#
# process.out = cms.OutputModule("PoolOutputModule",
#     fileName = cms.untracked.string("test_out.root"),
#     SelectEvents = cms.untracked.PSet(
#         SelectEvents = cms.vstring('selectPath')
#     ),
# )


#
# Quick histogramming
#
process.TFileService = cms.Service("TFileService",
    fileName = cms.string("histo.root")
)
# process.histsSignalMuonsBTVDQM = cms.EDAnalyzer("CandViewHistoAnalyzer",
#     src = cms.InputTag("signalMuonsBTVDQM"),
#     histograms = cms.VPSet(
#         cms.PSet(
#            min = cms.untracked.double(0.0),
#            max = cms.untracked.double(300.0),
#            nbins = cms.untracked.int32(30),
#            description = cms.untracked.string('muon transverse momentum [GeV]'),
#            name = cms.untracked.string('muonPt'),
#            plotquantity = cms.untracked.string('pt'),
#         ),
#         cms.PSet(
#            min = cms.untracked.double(-2.0),
#            max = cms.untracked.double(2.0),
#            nbins = cms.untracked.int32(40),
#            description = cms.untracked.string('muon pseudo rapidity'),
#            name = cms.untracked.string('muonEta'),
#            plotquantity = cms.untracked.string('eta'),
#         ),
#     )
# )
# process.histsSignalElectronsBTVDQM = cms.EDAnalyzer("CandViewHistoAnalyzer",
#     src = cms.InputTag("signalElectronsBTVDQM"),
#     histograms = cms.VPSet(
#         cms.PSet(
#            min = cms.untracked.double(0.0),
#            max = cms.untracked.double(300.0),
#            nbins = cms.untracked.int32(30),
#            description = cms.untracked.string('elec transverse momentum [GeV]'),
#            name = cms.untracked.string('elecPt'),
#            plotquantity = cms.untracked.string('pt'),
#         ),
#         cms.PSet(
#            min = cms.untracked.double(-2.0),
#            max = cms.untracked.double(2.0),
#            nbins = cms.untracked.int32(40),
#            description = cms.untracked.string('elec pseudo rapidity'),
#            name = cms.untracked.string('elecEta'),
#            plotquantity = cms.untracked.string('eta'),
#         ),
#     )
# )
# process.histsDiLepElMuCandsBTVDQM = cms.EDAnalyzer("CandViewHistoAnalyzer",
#     src = cms.InputTag("diLepElMuCandsBTVDQM"),
#     histograms = cms.VPSet(
#         cms.PSet(
#            min = cms.untracked.double(0.0),
#            max = cms.untracked.double(200.0),
#            nbins = cms.untracked.int32(20),
#            description = cms.untracked.string('mass Dilep'),
#            name = cms.untracked.string('mass'),
#            plotquantity = cms.untracked.string('mass'),
#         ),
#     )
# )
# process.histsTwoHighestPtSignalJetsBTVDQM = cms.EDAnalyzer("CandViewHistoAnalyzer",
#     src = cms.InputTag("twoHighestPtSignalJetsBTVDQM"),#selectedJetsBTVDQM
#     # src = cms.InputTag("selectedJetsBTVDQM"),#selectedJetsBTVDQM
#     # src = cms.InputTag("signalJetsBTVDQM"),#selectedJetsBTVDQM
#     histograms = cms.VPSet(
#         cms.PSet(
#            min = cms.untracked.double(0.0),
#            max = cms.untracked.double(300.0),
#            nbins = cms.untracked.int32(30),
#            description = cms.untracked.string('jet %d transverse momentum [GeV]'),
#            name = cms.untracked.string('jet_%d_pt'),
#            plotquantity = cms.untracked.string('pt'),
#            itemsToPlot = cms.untracked.int32(3),
#         ),
#         cms.PSet(
#            min = cms.untracked.double(-2.5),
#            max = cms.untracked.double(2.5),
#            nbins = cms.untracked.int32(50),
#            description = cms.untracked.string('jet %d pseudo rapidity'),
#            name = cms.untracked.string('jet_%d_eta'),
#            plotquantity = cms.untracked.string('eta'),
#            itemsToPlot = cms.untracked.int32(3),
#         ),
#     )
# )


#
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideHistogramUtilities
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideEDMNtuples
#

# process.printEventNumber = cms.OutputModule("AsciiOutputModule")

from PhysicsTools.PatAlgos.tools.helpers  import getPatAlgosToolsTask, addToProcessAndTask
process.patAlgosToolsTask = getPatAlgosToolsTask(process)
process.pathApplyPatAlgos = cms.Path(process.patAlgosToolsTask)

process.DQMoutput = cms.OutputModule("DQMRootOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('DQMIO'),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string('output.root'),
    outputCommands = process.DQMEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

process.selectPath = cms.Path(
    process.looseMuonsBTVDQM
    +process.signalMuonsBTVDQM
    +process.looseElectronsBTVDQM
    +process.signalElectronsBTVDQM
    +process.nLooseMuonsCountBTVDQM
    +process.nLooseElectronsCountBTVDQM
    +process.nSignalMuonsCountBTVDQM
    +process.nSignalElectronsCountBTVDQM
    +process.signalLeptonsBTVDQM
    +process.diLepElMuCandsBTVDQM
    +process.diLepElMuCandsFilter
    +process.selectedJetsBTVDQM
    +process.signalJetsBTVDQM
    +process.nSignalJetsBTVDQM
    +process.twoHighestPtSignalJetsBTVDQM
    +process.patJetsPuppiBarrelTagInfoAnalyzerDQM
    +process.patJetsPuppiEndcapOuterTagInfoAnalyzerDQM
    +process.patJetsPuppiEndcapInnerTagInfoAnalyzerDQM
    # +process.histsTwoHighestPtSignalJetsBTVDQM
)

process.DQMoutput_step = cms.EndPath(
    process.DQMoutput
)
