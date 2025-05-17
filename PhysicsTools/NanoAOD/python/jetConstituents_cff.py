import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import Var, CandVars

from PhysicsTools.NanoAOD.common_cff import *
from PhysicsTools.NanoAOD.jetsAK8_cff import fatJetTable as _fatJetTable
from PhysicsTools.NanoAOD.jetsAK4_Puppi_cff import jetPuppiTable as _jetPuppiTable

from PhysicsTools.NanoAOD.muons_cff     import muonTable as _muonTable
from PhysicsTools.NanoAOD.electrons_cff import electronTable as _electronTable
from PhysicsTools.NanoAOD.photons_cff   import photonTable as _photonTable
from PhysicsTools.NanoAOD.taus_cff      import tauTable as _tauTable


from CommonTools.ParticleFlow.pfCHS_cff import pfCHS

##############################################################
# Take AK8 jets and collect their PF constituents
###############################################################
finalJetsAK8PFConstituents = cms.EDProducer("PatJetConstituentPtrSelector",
    src = _fatJetTable.src,
    cut = cms.string("abs(eta) <= 2.5")
)
selectedFinalJetsAK8PFConstituents = cms.EDFilter("PATPackedCandidatePtrSelector",
    src = cms.InputTag("finalJetsAK8PFConstituents", "constituents"),
    cut = cms.string("")
)
##############################################################
# Take AK4 jets and collect their PF constituents
###############################################################
finalJetsAK4PFConstituents = cms.EDProducer("PatJetConstituentPtrSelector",
    src = _jetPuppiTable.src,
    cut = cms.string("")
)

selectedFinalJetsAK4PFConstituents = cms.EDFilter("PATPackedCandidatePtrSelector",
    src = cms.InputTag("finalJetsAK4PFConstituents", "constituents"),
    cut = cms.string("")
)


##############################################################
# Collect Tau signalCands and isolationCands
###############################################################
finalTausConstituents = cms.EDProducer("PatTauConstituentSelector",
src = _tauTable.src,
cut = cms.string("") # Store all PF candidates for all taus
)

##############################################################
# Electrons, Photons and Muons
###############################################################
finalElectronsPFCandsConstituents = cms.EDProducer("PatElectronPFCandSelector",
src = _electronTable.src,
cut = cms.string("") # Store all PF candidates
)

finalPhotonsPFCandsConstituents = cms.EDProducer("PatPhotonPFCandSelector",
src = _photonTable.src,
cut = cms.string("") # Store all PF candidates
)

finalMuonsPFCandsConstituents = cms.EDProducer("PatMuonPFCandSelector",
src = _muonTable.src,
cut = cms.string("") # Store all PF candidates
)


###############################################################################
# Take packedPFCandidates,changed to Ptr and choose IsolatedChargedHadron()
##############################################################################
packedPFCandidatesPtr =  cms.EDProducer("PackedCandidateToPackedCandidatePtr",
    src = cms.InputTag("packedPFCandidates"),
)

pfChargedHadronSelected = cms.EDFilter("PATPackedCandidatePtrSelector",
  src = cms.InputTag("packedPFCandidatesPtr"),
  cut = cms.string("isIsolatedChargedHadron()"),
)

pfChargedHadronSelectedIsoCands = cms.EDProducer("PackedPFCandIsoProducer",
  packedPFCandidatesSelected = cms.InputTag("pfChargedHadronSelected"),
  packedPFCandidates = cms.InputTag("packedPFCandidates"),
  maxDeltaR = cms.double(0.5)
)

##############################################################
# Setup PF candidates table
##############################################################
finalPFCandidates = cms.EDProducer("PackedCandidatePtrMerger",
    src = cms.VInputTag(
        cms.InputTag("selectedFinalJetsAK8PFConstituents"),
        cms.InputTag("selectedFinalJetsAK4PFConstituents"),
        cms.InputTag("pfChargedHadronSelected"),
        cms.InputTag("pfChargedHadronSelectedIsoCands"),
        cms.InputTag("finalTausConstituents", "constituents"),
        cms.InputTag("finalElectronsPFCandsConstituents", "constituents"),
        cms.InputTag("finalPhotonsPFCandsConstituents", "constituents"),
        cms.InputTag("finalMuonsPFCandsConstituents", "constituents")
        # cms.InputTag("packedPFCandidatesPtr"),
    ),
    skipNulls = cms.bool(True),
    warnOnSkip = cms.bool(True)
)

pfCandidatesTable = cms.EDProducer("SimplePATCandidateFlatTableProducer",
    src = cms.InputTag("finalPFCandidates"),
    cut = cms.string(""),
    name = cms.string("PFCand"),
    doc = cms.string("PF candidate constituents of AK8 puppi jets (FatJet) with |eta| <= 2.5, AK4 puppi jets (Jets) and packedCands with isIsolatedChargedHadron()"),
    singleton = cms.bool(False),
    extension = cms.bool(False),
    variables = cms.PSet(CandVars,
      energy = Var("energy()", float, doc="energy",precision=-1),
      puppiWeight = Var("puppiWeight()", float, doc="Puppi weight",precision=-1),
      puppiWeightNoLep = Var("puppiWeightNoLep()", float, doc="Puppi weight removing leptons",precision=-1),
      passCHS = Var(pfCHS.cut.value(), bool, doc=pfCHS.cut.value()),
      isIsolatedChargedHadron = Var("isIsolatedChargedHadron()", bool, doc="isIsolatedChargedHadron()"),
      trkQuality = Var("?hasTrackDetails()?bestTrack().qualityMask():0", int, doc="track quality mask"),
      trkHighPurity = Var("trackHighPurity()", bool, doc="is trackHighPurity"),
      trkAlgo = Var("?hasTrackDetails()?bestTrack().algo():-1", int, doc="track algorithm"),
      trkP = Var("?hasTrackDetails()?bestTrack().p():-1", float, doc="track momemtum", precision=-1),
      trkPt = Var("?hasTrackDetails()?bestTrack().pt():-1", float, doc="track pt", precision=-1),
      trkEta = Var("?hasTrackDetails()?bestTrack().eta():-1", float, doc="track eta", precision=-1),
      trkPhi = Var("?hasTrackDetails()?bestTrack().phi():-1", float, doc="track phi", precision=-1),
      dz = Var("?hasTrackDetails()?dz():-1", float, doc="dz", precision=15),
      dzErr = Var("?hasTrackDetails()?dzError():-1", float, doc="dz err",precision=15),
      d0 = Var("?hasTrackDetails()?dxy():-1", float, doc="dxy", precision=15),
      d0Err = Var("?hasTrackDetails()?dxyError():-1", float, doc="dxy err", precision=15),
      vx = Var("?hasTrackDetails()?vx():-1", float, doc="vx", precision=15),
      vy = Var("?hasTrackDetails()?vy():-1", float, doc="vy", precision=15),
      vz = Var("?hasTrackDetails()?vy():-1", float, doc="vz", precision=15),
      nHits = Var("numberOfHits()", int, doc="numberOfHits()"),
      nPixelHits = Var("numberOfPixelHits()", int, doc="numberOfPixelHits()"),
      lostInnerHits = Var("lostInnerHits()", int, doc="lost inner hits. -1: validHitInFirstPixelBarrelLayer, 0: noLostInnerHits, 1: oneLostInnerHit, 2: moreLostInnerHits"),
      lostOuterHits = Var("?hasTrackDetails()?bestTrack().hitPattern().numberOfLostHits('MISSING_OUTER_HITS'):0", int, doc="lost outer hits"),
      pixelLayersWithMeasurement = Var("pixelLayersWithMeasurement()", int, doc="pixelLayersWithMeasurement()"),
      stripLayersWithMeasurement = Var("stripLayersWithMeasurement()", int, doc="stripLayersWithMeasurement()"),
      trkChi2 = Var("?hasTrackDetails()?bestTrack().normalizedChi2():-1", float, doc="normalized trk chi2", precision=15),
      pvAssocQuality = Var("pvAssociationQuality()", int, doc="primary vertex association quality (NotReconstructedPrimary = 0, OtherDeltaZ = 1, CompatibilityBTag = 4, CompatibilityDz = 5, UsedInFitLoose = 6, UsedInFitTight = 7)"),
      vertexRef = Var("?vertexRef().isNonnull()?vertexRef().key():-1", int, doc="vertexRef().key()"),
      fromPV0 = Var("fromPV()", int, doc="PV0 association (NoPV = 0, PVLoose = 1, PVTight = 2, PVUsedInFit = 3)"),
      vtxChi2 = Var("?hasTrackDetails()?vertexChi2():-1", float, doc="vertex chi2",precision=15),
      isStandAloneMuon = Var("isStandAloneMuon()", bool, doc="isStandAloneMuon()"),
      isGlobalMuon = Var("isGlobalMuon()", bool, doc="isGlobalMuon()"),
      isGoodEgamma = Var("isGoodEgamma()", bool, doc="isGoodEgamma()"),
  )
)

pfCandidatesTable.variables.pt.precision = -1
pfCandidatesTable.variables.eta.precision = -1
pfCandidatesTable.variables.phi.precision = -1
pfCandidatesTable.variables.mass.precision = -1
pfCandidatesTable.variables.pdgId.doc = "PF candidate type (+/-211 = ChgHad, 130 = NeuHad, 22 = Photon, +/-11 = Electron, +/-13 = Muon, 1 = HFHad, 2 = HFEM)"

#
# PackedCandidateExtTableProducer is currently setup for
# AOD->Nano workflow because we want reference to original reco::PFCandidate
#
pfCandidatesExtTable = cms.EDProducer("PackedCandidateExtTableProducer",
    srcPFCandidates = pfCandidatesTable.src,
    packedPFCandidates = cms.InputTag("packedPFCandidates"),
    # PFClustersHCAL = cms.InputTag("particleFlowClusterHCAL"),
    # PFRecHitsHBHE = cms.InputTag("particleFlowRecHitHBHE"),
    # PFClustersECAL = cms.InputTag("particleFlowClusterECAL"),
    # PFClustersPS = cms.InputTag("particleFlowClusterPS"),
    # savePFClustersHCAL = cms.bool(True),
    # savePFRecHitsHBHE = cms.bool(True),
    # savePFClustersECAL = cms.bool(True),
    # savePFClustersPS = cms.bool(True),
    name = pfCandidatesTable.name,
    srcWeightsV = cms.VInputTag(),
    weightNamesV = cms.vstring(),
    weightDocsV = cms.vstring(),
    weightPrecision = cms.int32(-1),
    saveFromPVvertexRef = cms.bool(True)
)

customPFChargedHadronCandidateTable =  cms.EDProducer("SimplePATCandidateFlatTableProducer",
    src = cms.InputTag("pfChargedHadronSelected"),
    cut = cms.string(""), #we should not filter
    name = cms.string("IsoChHadPFCand"),
    doc = cms.string("Isolated Charged Hadron PF candidates"),
    singleton = cms.bool(False), # the number of entries is variable
    extension = cms.bool(False), # this is the extension table for the AK8 constituents
    variables = cms.PSet()
)

customPFChargedHadronCandidateExtTable = cms.EDProducer("SimpleSelectedCandidateTableProducer",
    name = cms.string("IsoChHadPFCand"),
    candIdxName = cms.string("pfCandIdx"),
    candIdxDoc = cms.string("Index in PFCand table"),
    candidatesMain = pfCandidatesTable.src,
    candidatesSelected = customPFChargedHadronCandidateTable.src,
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
  jetConstCut = selectedFinalJetsAK8PFConstituents.cut
)

finalJetsAK4ConstituentsTable = cms.EDProducer("SimplePatJetConstituentTableProducer",
  name = cms.string(_jetPuppiTable.name.value()+"PFCand"),
  candIdxName = cms.string("pfCandIdx"),
  candIdxDoc = cms.string("Index in the PFCand table"),
  candidates = pfCandidatesTable.src,
  jets = _jetPuppiTable.src,
  jetCut = _jetPuppiTable.cut,
  jetConstCut = selectedFinalJetsAK4PFConstituents.cut
)

##############################################################
# Setup Tau constituents table
##############################################################
customTauConstituentsTable = cms.EDProducer("SimplePatTauConstituentTableProducer",
  name = cms.string(f"{_tauTable.name.value()}PFCand"),
  candIdxName = cms.string("pfCandIdx"),
  candIdxDoc = cms.string("Index in the PFCand table"),
  taus = _tauTable.src,
  candidates = pfCandidatesTable.src,
  tauCut = cms.string("") # No need to apply cut here.
)


##############################################################
# Electrons, Photons and Muons PFCands tables
##############################################################
customElectronPFCandsTable = cms.EDProducer("SimplePatElectronPFCandTableProducer",
  name = cms.string(f"{_electronTable.name.value()}PFCand"),
  candIdxName = cms.string("pfCandIdx"),
  candIdxDoc = cms.string("Index in the PFCand table"),
  objects = _electronTable.src,
  candidates = pfCandidatesTable.src,
  objectCut = cms.string("") # No need to apply cut here.
)

customPhotonPFCandsTable = cms.EDProducer("SimplePatPhotonPFCandTableProducer",
    name = cms.string(f"{_photonTable.name.value()}PFCand"),
    candIdxName = cms.string("pfCandIdx"),
    candIdxDoc = cms.string("Index in the PFCand table"),
    objects = _photonTable.src,
    candidates = pfCandidatesTable.src,
    objectCut = cms.string("") # No need to apply cut here.
)

customMuonPFCandsTable = cms.EDProducer("SimplePatMuonPFCandTableProducer",
    name = cms.string(f"{_muonTable.name.value()}PFCand"),
    candIdxName = cms.string("pfCandIdx"),
    candIdxDoc = cms.string("Index in the PFCand table"),
    objects = _muonTable.src,
    candidates = pfCandidatesTable.src,
    objectCut = cms.string("") # No need to apply cut here.
)

jetConstituentsTask = cms.Task(
    finalJetsAK8PFConstituents,
    selectedFinalJetsAK8PFConstituents,
    finalJetsAK4PFConstituents,
    selectedFinalJetsAK4PFConstituents,
    packedPFCandidatesPtr,
    pfChargedHadronSelected,
    pfChargedHadronSelectedIsoCands,
    finalTausConstituents,
    finalElectronsPFCandsConstituents,
    finalPhotonsPFCandsConstituents,
    finalMuonsPFCandsConstituents
)
jetConstituentsTablesTask = cms.Task(
    finalPFCandidates,
    pfCandidatesTable,
    pfCandidatesExtTable,
    finalJetsAK8ConstituentsTable,
    finalJetsAK4ConstituentsTable,
    customPFChargedHadronCandidateTable,
    customPFChargedHadronCandidateExtTable,
    customTauConstituentsTable,
    customElectronPFCandsTable,
    customPhotonPFCandsTable,
    customMuonPFCandsTable
)

def SaveAK4JetConstituents(process, jetCut="", jetConstCut=""):
    """
    This function can be used as a cmsDriver customization
    function to add AK4 jet constituents, on top of the AK8
    jet constituents.
    """
    process.finalJetsPuppiPFConstituents = process.finalJetsAK8PFConstituents.clone(
        src = process.jetPuppiTable.src,
        cut = jetCut
    )
    process.jetConstituentsTask.add(process.finalJetsPuppiPFConstituents)

    process.selectedFinalJetsPuppiPFConstituents = process.selectedFinalJetsAK8PFConstituents.clone(
        src = cms.InputTag("finalJetsPuppiPFConstituents", "constituents"),
        cut = jetConstCut
    )
    process.jetConstituentsTask.add(process.selectedFinalJetsPuppiPFConstituents)

    process.finalPFCandidates.src += ["selectedFinalJetsPuppiPFConstituents"]
    process.pfCandidatesTable.doc = pfCandidatesTable.doc.value()+" and AK4 puppi jets (Jet)"

    process.finalJetsPuppiConstituentsTable = process.finalJetsAK8ConstituentsTable.clone(
        name = process.jetPuppiTable.name.value()+"PFCand",
        jets = process.jetPuppiTable.src,
        jetCut = process.jetPuppiTable.cut,
        jetConstCut = process.selectedFinalJetsPuppiPFConstituents.cut
    )
    process.jetConstituentsTablesTask.add(process.finalJetsPuppiConstituentsTable)

    return process

def SaveGenJetConstituents(process, addGenJetConst, addGenJetAK8Const, genJetConstCut="",genJetAK8ConstCut=""):
    """
    This function can be used as a cmsDriver
    customization function to add gen jet
    constituents.
    """
    process.genjetConstituentsTask = cms.Task()
    process.genjetConstituentsTableTask = cms.Task()

    if addGenJetConst:
        process.genJetConstituents = cms.EDProducer("GenJetPackedConstituentPtrSelector",
            src = process.genJetTable.src,
            cut = process.genJetTable.cut,
        )
        process.genjetConstituentsTask.add(process.genJetConstituents)

        process.selectedGenJetConstituents = cms.EDFilter("PATPackedGenParticlePtrSelector",
            src = cms.InputTag("genJetConstituents", "constituents"),
            cut = cms.string(genJetConstCut)
        )
        process.genjetConstituentsTask.add(process.selectedGenJetConstituents)

    if addGenJetAK8Const:
        process.genJetAK8Constituents = cms.EDProducer("GenJetPackedConstituentPtrSelector",
            src = process.genJetAK8Table.src,
            cut = process.genJetAK8Table.cut,
        )
        process.genjetConstituentsTask.add(process.genJetAK8Constituents)

        process.selectedGenJetAK8Constituents = cms.EDFilter("PATPackedGenParticlePtrSelector",
            src = cms.InputTag("genJetAK8Constituents", "constituents"),
            cut = cms.string(genJetAK8ConstCut)
        )
        process.genjetConstituentsTask.add(process.selectedGenJetAK8Constituents)

    if addGenJetConst or addGenJetAK8Const:
        process.finalGenPartCandidates = cms.EDProducer("PackedGenParticlePtrMerger",
            src = cms.VInputTag(),
            skipNulls = cms.bool(True),
            warnOnSkip = cms.bool(True)
        )
        process.genjetConstituentsTableTask.add(process.finalGenPartCandidates)

        process.genPartCandidatesTable = cms.EDProducer("SimplePATGenParticleFlatTableProducer",
            src = cms.InputTag("finalGenPartCandidates"),
            cut = cms.string(""),
            name = cms.string("GenPartCand"),
            doc = cms.string("Gen particle constituents:"),
            singleton = cms.bool(False),
            extension = cms.bool(False),
            variables = cms.PSet(P4Vars,
                pdgId = Var("pdgId", int, doc="pdgId")
          )
        )
        process.genjetConstituentsTableTask.add(process.genPartCandidatesTable)
        process.genPartCandidatesTable.variables.pt.precision=10
        process.genPartCandidatesTable.variables.mass.precision=10

        if addGenJetConst:
            process.finalGenPartCandidates.src += ["selectedGenJetConstituents"]
            process.genPartCandidatesTable.doc = process.genPartCandidatesTable.doc.value()+" AK4 Gen jets (GenJet) "

            process.genJetConstituentsTable = cms.EDProducer("SimpleGenJetConstituentTableProducer",
              name = cms.string(process.genJetTable.name.value()+"GenPartCand"),
              candIdxName = cms.string("genPartCandIdx"),
              candIdxDoc = cms.string("Index in the GenPartCand table"),
              candidates = process.genPartCandidatesTable.src,
              jets = process.genJetTable.src,
              jetCut = process.genJetTable.cut,
              jetConstCut = process.selectedGenJetConstituents.cut
            )
            process.genjetConstituentsTableTask.add(process.genJetConstituentsTable)

        if addGenJetAK8Const:
            process.finalGenPartCandidates.src += ["selectedGenJetAK8Constituents"]
            process.genPartCandidatesTable.doc = process.genPartCandidatesTable.doc.value()+" AK8 Gen jets (GenJetAK8)"

            process.genJetAK8ConstituentsTable = cms.EDProducer("SimpleGenJetConstituentTableProducer",
              name = cms.string(process.genJetAK8Table.name.value()+"GenPartCand"),
              candIdxName = cms.string("genPartCandIdx"),
              candIdxDoc = cms.string("Index in the GenPartCand table"),
              candidates = process.genPartCandidatesTable.src,
              jets = process.genJetAK8Table.src,
              jetCut = process.genJetAK8Table.cut,
              jetConstCut = process.selectedGenJetAK8Constituents.cut
            )
            process.genjetConstituentsTableTask.add(process.genJetAK8ConstituentsTable)

        process.nanoTableTaskFS.add(process.genjetConstituentsTask)
        process.nanoTableTaskFS.add(process.genjetConstituentsTableTask)

    return process

def SaveGenJetAK4Constituents(process):
    process = SaveGenJetConstituents(process,addGenJetConst=True,addGenJetAK8Const=False)
    return process
def SaveGenJetAK8Constituents(process):
    process = SaveGenJetConstituents(process,addGenJetConst=False,addGenJetAK8Const=True)
    return process
def SaveGenJetAK4AK8Constituents(process):
    process = SaveGenJetConstituents(process,addGenJetConst=True,addGenJetAK8Const=True)
    return process

