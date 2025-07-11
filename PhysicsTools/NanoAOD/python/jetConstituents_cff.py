import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import Var, CandVars

from PhysicsTools.NanoAOD.common_cff import *
from PhysicsTools.NanoAOD.jetsAK8_cff import fatJetTable as _fatJetTable
from PhysicsTools.NanoAOD.jetsAK4_Puppi_cff import jetPuppiTable as _jetPuppiTable
from PhysicsTools.NanoAOD.muons_cff import muonTable as _muonTable

##############################################################
# Take AK8 jets and collect their PF constituents
###############################################################
finalJetsAK8PFConstituents = cms.EDProducer("PatJetConstituentPtrSelector",
    src = _fatJetTable.src,
    cut = cms.string("abs(eta) <= 2.5")
)

finalJetsAK4PFConstituents = cms.EDProducer("PatJetConstituentPtrSelector",
    src = _jetPuppiTable.src,
    cut = cms.string("")
)

##############################################################
# Setup PF candidates table
##############################################################
finalPFCandidates = cms.EDProducer("PackedCandidatePtrMerger",
    src = cms.VInputTag(
        cms.InputTag("finalJetsAK8PFConstituents","constituents"),
        cms.InputTag("finalJetsAK4PFConstituents","constituents"),
    ),
    skipNulls = cms.bool(True),
    warnOnSkip = cms.bool(True)
)

pfCandidatesTable = cms.EDProducer("SimplePATCandidateFlatTableProducer",
    src = cms.InputTag("finalPFCandidates"),
    cut = cms.string(""), #we should not filter
    name = cms.string("PFCand"),
    doc = cms.string("PF candidate constituents of AK8 puppi jets (FatJet) with |eta| <= 2.5 and AK4 puppi jets (Jet)"),
    singleton = cms.bool(False), # the number of entries is variable
    extension = cms.bool(False), # this is the extension table for the AK8 constituents
    variables = cms.PSet(CandVars,
      energy = Var("energy()", float, doc="energy",precision=-1),
      puppiWeight = Var("puppiWeight()", float, doc="Puppi weight",precision=-1),
      puppiWeightNoLep = Var("puppiWeightNoLep()", float, doc="Puppi weight removing leptons",precision=-1),
      isIsolatedChargedHadron = Var("isIsolatedChargedHadron()", bool, doc="isIsolatedChargedHadron()"),
      trkQuality = Var("?hasTrackDetails()?bestTrack().qualityMask():0", int, doc="track quality mask"),
      trkHighPurity = Var("trackHighPurity()", bool, doc="is trackHighPurity"),
      trkAlgo = Var("?hasTrackDetails()?bestTrack().algo():-1", int, doc="track algorithm"),
      trkP = Var("?hasTrackDetails()?bestTrack().p():-1", float, doc="track momemtum", precision=-1),
      trkPt = Var("?hasTrackDetails()?bestTrack().pt():-1", float, doc="track pt", precision=-1),
      trkEta = Var("?hasTrackDetails()?bestTrack().eta():-1", float, doc="track eta", precision=-1),
      trkPhi = Var("?hasTrackDetails()?bestTrack().phi():-1", float, doc="track phi", precision=-1),
      dz = Var("?hasTrackDetails()?dz():-1", float, doc="dz", precision=15),
      dzErr = Var("?hasTrackDetails()?dzError():-1", float, doc="dz err", precision=15),
      d0 = Var("?hasTrackDetails()?dxy():-1", float, doc="dxy", precision=15),
      d0Err = Var("?hasTrackDetails()?dxyError():-1", float, doc="dxy err", precision=15),
      nHits = Var("numberOfHits()", int, doc="numberOfHits()"),
      nPixelHits = Var("numberOfPixelHits()", int, doc="numberOfPixelHits()"),
      lostInnerHits = Var("lostInnerHits()", int, doc="lost inner hits. -1: validHitInFirstPixelBarrelLayer, 0: noLostInnerHits, 1: oneLostInnerHit, 2: moreLostInnerHits"),
      lostOuterHits = Var("?hasTrackDetails()?bestTrack().hitPattern().numberOfLostHits('MISSING_OUTER_HITS'):0", int, doc="lost outer hits"),
      trkChi2 = Var("?hasTrackDetails()?bestTrack().normalizedChi2():-1", float, doc="normalized trk chi2", precision=15),
      pvAssocQuality = Var("pvAssociationQuality()", int, doc="primary vertex association quality (NotReconstructedPrimary = 0, OtherDeltaZ = 1, CompatibilityBTag = 4, CompatibilityDz = 5, UsedInFitLoose = 6, UsedInFitTight = 7)"),
      vertexRef = Var("?vertexRef().isNonnull()?vertexRef().key():-1", int, doc="vertexRef().key()"),
      fromPV0 = Var("fromPV()", int, doc="PV0 association (NoPV = 0, PVLoose = 1, PVTight = 2, PVUsedInFit = 3)"),
      vtxChi2 = Var("?hasTrackDetails()?vertexChi2():-1", float, doc="vertex chi2",precision=15),
      # caloFraction = Var("caloFraction()", float, doc="(EcalE+HcalE)/candE", precision=15),
      # hcalFraction = Var("hcalFraction()", float, doc="HcalE/(EcalE+HcalE)", precision=15),
      # rawCaloFraction = Var("rawCaloFraction()", float, doc="(rawEcalE+rawHcalE)/candE. Only for isolated charged hadron", precision=15),
      # rawHcalFraction = Var("rawHcalFraction()", float, doc="rawHcalE/(rawEcalE+rawHcalE). Only for isolated charged hadrons", precision=15),
    )
)
pfCandidatesTable.variables.pt.precision = -1
pfCandidatesTable.variables.eta.precision = -1
pfCandidatesTable.variables.phi.precision = -1
pfCandidatesTable.variables.mass.precision = -1


#
# PackedCandidateExtTableProducer is currently setup for
# AOD->Nano workflow because we want reference to origina reco::PFCandidate
#
pfCandidatesExtTable = cms.EDProducer("PackedCandidateExtTableProducer",
    srcPFCandidates = pfCandidatesTable.src,
    srcJets = _jetPuppiTable.src,
    srcMuons = _muonTable.src,
    packedPFCandidates = cms.InputTag("packedPFCandidates"),
    PFClustersHCAL = cms.InputTag("particleFlowClusterHCAL"),
    PFRecHitsHBHE = cms.InputTag("particleFlowRecHitHBHE"),
    PFClustersECAL = cms.InputTag("particleFlowClusterECAL"),
    PFClustersPS = cms.InputTag("particleFlowClusterPS"),
    savePFClustersHCAL = cms.bool(True),
    savePFRecHitsHBHE = cms.bool(True),
    savePFClustersECAL = cms.bool(True),
    savePFClustersPS = cms.bool(True),
    matchMuonsWithPFRecHitsHBHE = cms.bool(False),
    name = pfCandidatesTable.name,
    srcWeightsV = cms.VInputTag(),
    weightNamesV = cms.vstring(),
    weightDocsV = cms.vstring(),
    weightPrecision = cms.int32(-1),
    saveFromPVvertexRef = cms.bool(True)
)

##############################################################
# Setup AK8 jet constituents table
##############################################################
finalJetsAK8ConstituentsTable = cms.EDProducer("SimplePatJetConstituentTableProducer",
  name = cms.string(_fatJetTable.name.value()+"PFCand"),
  candIdxName = cms.string("pfCandIdx"),
  candIdxDoc = cms.string("Index in the PFCand table"),
  candidates = pfCandidatesTable.src,
  jets = _fatJetTable.src,
  jetCut = _fatJetTable.cut,
  jetConstCut = cms.string("")
)

finalJetsAK4ConstituentsTable = cms.EDProducer("SimplePatJetConstituentTableProducer",
  name = cms.string(_jetPuppiTable.name.value()+"PFCand"),
  candIdxName = cms.string("pfCandIdx"),
  candIdxDoc = cms.string("Index in the PFCand table"),
  candidates = pfCandidatesTable.src,
  jets = _jetPuppiTable.src,
  jetCut = _jetPuppiTable.cut,
  jetConstCut = cms.string("")
)

jetConstituentsTask = cms.Task(finalJetsAK8PFConstituents,finalJetsAK4PFConstituents)
jetConstituentsTablesTask = cms.Task(finalPFCandidates,pfCandidatesTable,pfCandidatesExtTable,finalJetsAK8ConstituentsTable,finalJetsAK4ConstituentsTable)
