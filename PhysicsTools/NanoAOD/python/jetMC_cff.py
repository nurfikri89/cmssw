import FWCore.ParameterSet.Config as cms

from PhysicsTools.NanoAOD.common_cff import *
from PhysicsTools.NanoAOD.simpleGenJetFlatTableProducer_cfi import simpleGenJetFlatTableProducer
from PhysicsTools.NanoAOD.simplePATJetFlatTableProducer_cfi import simplePATJetFlatTableProducer
from PhysicsTools.NanoAOD.jetsAK8_cff import fatJetTable as _fatJetTable
from PhysicsTools.NanoAOD.jetsAK8_cff import subJetTable as _subJetTable
from RecoJets.JetProducers.ak4GenJets_cfi import ak4GenJets

jetMCTable = simplePATJetFlatTableProducer.clone(
    src = cms.InputTag("linkedObjects","jets"),
    name = cms.string("Jet"),
    extension = cms.bool(True), # this is an extension  table for the jets
    variables = cms.PSet(
        partonFlavour = Var("partonFlavour()", "int16", doc="flavour from parton matching"),
        hadronFlavour = Var("hadronFlavour()", "uint8", doc="flavour from hadron ghost clustering"),
        # cut should follow genJetTable.cut
        genJetIdx = Var("?genJetFwdRef().backRef().isNonnull() && genJetFwdRef().backRef().pt() > 10.?genJetFwdRef().backRef().key():-1", "int16", doc="index of matched gen jet"),
    )
)
genJetTable = simpleGenJetFlatTableProducer.clone(
    src = cms.InputTag("slimmedGenJets"),
    cut = cms.string("pt > 10"),
    name = cms.string("GenJet"),
    doc  = cms.string("slimmedGenJets, i.e. ak4 Jets made with visible genparticles"),
    variables = cms.PSet(P4Vars,
    #anything else?
    )
)

patJetPartonsNano = cms.EDProducer('HadronAndPartonSelector',
    src = cms.InputTag("generator"),
    particles = cms.InputTag("prunedGenParticles"),
    partonMode = cms.string("Auto"),
    fullChainPhysPartons = cms.bool(True)
)

genJetFlavourAssociation = cms.EDProducer("JetFlavourClustering",
    jets = genJetTable.src,
    bHadrons = cms.InputTag("patJetPartonsNano","bHadrons"),
    cHadrons = cms.InputTag("patJetPartonsNano","cHadrons"),
    partons = cms.InputTag("patJetPartonsNano","algorithmicPartons"),
    finalPartons = cms.InputTag("patJetPartonsNano","algorithmicPartons"),
    leptons = cms.InputTag("patJetPartonsNano","leptons"),
    jetAlgorithm = cms.string("AntiKt"),
    rParam = cms.double(0.4),
    ghostRescaling = cms.double(1e-18),
    hadronFlavourHasPriority = cms.bool(False),
    # ========================================================================
    # GHS Algorithm Configuration (nested ParameterSet)
    # ========================================================================
    ghsAlgorithm = cms.PSet(
        # Enable/disable the GHS algorithm
        enabled = cms.bool(True),
        
        # GHS algorithm alpha parameter (default: 1.0)
        # Controls the distance metric weight
        alpha = cms.double(1.0),
        
        # GHS algorithm omega parameter (default: 2.0)
        # Controls the angular distance metric
        omega = cms.double(2.0),
        
        # Minimum pT threshold for GHS jets (default: very small)
        ptMin = cms.double(0.0),
        
        # Flavour summation scheme
        # Options: "net_flav", "mod2_flav", "any_flav"
        #   - net_flav: Net flavour (algebraic sum)
        #   - mod2_flav: Modulo 2 flavour (parity)
        #   - any_flav: Any absolute flavour
        flavSummationScheme = cms.string("net_flav")
    ),
    ifnAlgorithm = cms.PSet(
        # Enable/disable the IFN algorithm
        enabled = cms.bool(True),
        alpha = cms.double(2.0),  # IFN alpha parameter (default: 1.0)
        omega = cms.double(1.0),  # IFN omega parameter (default: 2.0)
        flavSummationScheme = cms.string("net_flav")  # Flavour summation scheme for IFN (same options as GHS)
    )
)

genJetFlavourTable = cms.EDProducer("GenJetFlavourTableProducer",
    name = genJetTable.name,
    src = genJetTable.src,
    cut = genJetTable.cut,
    deltaR = cms.double(0.1),
    flavourAlgorithms = cms.vstring("GHS", "IFN"),  # List of flavour algorithms to include in the table
    # jetFlavourInfos = cms.InputTag("slimmedGenJetsFlavourInfos"),
    jetFlavourInfos = cms.InputTag("genJetFlavourAssociation"),
)

genJetAK8Table = simpleGenJetFlatTableProducer.clone(
    src = cms.InputTag("slimmedGenJetsAK8"),
    cut = cms.string("pt > 100."),
    name = cms.string("GenJetAK8"),
    doc  = cms.string("slimmedGenJetsAK8, i.e. ak8 Jets made with visible genparticles"),
    variables = cms.PSet(P4Vars,
    #anything else?
    )
)

genJetAK8FlavourAssociation = cms.EDProducer("JetFlavourClustering",
    jets = genJetAK8Table.src,
    bHadrons = cms.InputTag("patJetPartonsNano","bHadrons"),
    cHadrons = cms.InputTag("patJetPartonsNano","cHadrons"),
    partons = cms.InputTag("patJetPartonsNano","algorithmicPartons"),
    finalPartons = cms.InputTag("patJetPartonsNano","algorithmicPartons"),
    leptons = cms.InputTag("patJetPartonsNano","leptons"),
    jetAlgorithm = cms.string("AntiKt"),
    rParam = cms.double(0.8),
    ghostRescaling = cms.double(1e-18),
    hadronFlavourHasPriority = cms.bool(False),
    ifnAlgorithm = cms.PSet(
        enabled = cms.bool(True),
        alpha = cms.double(2.0),
        omega = cms.double(1.0),
        flavSummationScheme = cms.string("net_flav")
    ),
    ghsAlgorithm = cms.PSet(
        enabled = cms.bool(True),
        alpha = cms.double(1.0),
        omega = cms.double(2.0),
        ptMin = cms.double(5.0),
        flavSummationScheme = cms.string("net_flav")
    )
)

genJetAK8FlavourTable = cms.EDProducer("GenJetFlavourTableProducer",
    name = genJetAK8Table.name,
    src = genJetAK8Table.src,
    cut = genJetAK8Table.cut,
    deltaR = cms.double(0.1),
    jetFlavourInfos = cms.InputTag("genJetAK8FlavourAssociation"),
    flavourAlgorithms = cms.vstring("GHS", "IFN")
)
fatJetMCTable = simplePATJetFlatTableProducer.clone(
    src = _fatJetTable.src,
    cut = _fatJetTable.cut,
    name = _fatJetTable.name,
    extension = cms.bool(True),
    variables = cms.PSet(
        hadronFlavour = Var("hadronFlavour()", "uint8", doc="flavour from hadron ghost clustering"),
        # cut should follow genJetAK8Table.cut
        genJetAK8Idx = Var("?genJetFwdRef().backRef().isNonnull() && genJetFwdRef().backRef().pt() > 100.?genJetFwdRef().backRef().key():-1", "int16", doc="index of matched gen AK8 jet"),
    )
)

genSubJetAK8Table = simpleGenJetFlatTableProducer.clone(
    src = cms.InputTag("slimmedGenJetsAK8SoftDropSubJets"),
    name = cms.string("SubGenJetAK8"),
    doc  = cms.string("slimmedGenJetsAK8SoftDropSubJets, i.e. subjets of ak8 Jets made with visible genparticles"),
    variables = cms.PSet(P4Vars,
    #anything else?
    )
)
subjetMCTable = simplePATJetFlatTableProducer.clone(
    src = _subJetTable.src,
    cut = _subJetTable.cut,
    name = _subJetTable.name,
    extension = cms.bool(True),
    variables = cms.PSet(
        nBHadrons = Var("jetFlavourInfo().getbHadrons().size()", "uint8", doc="number of b-hadrons"),
        nCHadrons = Var("jetFlavourInfo().getcHadrons().size()", "uint8", doc="number of c-hadrons"),
        hadronFlavour = Var("hadronFlavour()", "uint8", doc="flavour from hadron ghost clustering"),
        subGenJetAK8Idx = Var("?genJetFwdRef().backRef().isNonnull()?genJetFwdRef().backRef().key():-1", "int16", doc="index of matched gen subjet in SubGenJetAK8")
    )
)

genParticlesForJetsCharged = cms.EDFilter("CandPtrSelector",
    src = cms.InputTag("prunedGenParticles"),  # or "packedGenParticles" if available
    cut = cms.string("charge != 0 && pt > 0.3 && status == 1 && abs(pdgId) != 12 && abs(pdgId) != 14 && abs(pdgId) != 16")
)

ak4GenJetsChargedOnly = ak4GenJets.clone(
    src = cms.InputTag("genParticlesForJetsCharged"),
    rParam = cms.double(0.4),
    jetAlgorithm=cms.string("AntiKt"),
    doAreaFastjet = False,
    jetPtMin=1)


trackGenJetAK4Table = genJetTable.clone(
    src = cms.InputTag("ak4GenJetsChargedOnly"),
    cut = cms.string("pt > 1"),
    name = cms.string("TrackGenJetAK4"),
    doc = cms.string("AK4 GenJets made with charged particles only"),
    variables = cms.PSet(P3Vars)
)

trackGenJetAK4Table.variables.pt.precision = 10
trackGenJetAK4Table.variables.eta.precision = 8
trackGenJetAK4Table.variables.phi.precision = 8

jetMCTaskak4 = cms.Task(
    jetMCTable,
    genJetFlavourAssociation,
    genJetTable,
    patJetPartonsNano,
    genJetFlavourTable,
    genParticlesForJetsCharged,
    ak4GenJetsChargedOnly,
    trackGenJetAK4Table
    )
jetMCTaskak8 = cms.Task(
    genJetAK8Table,
    genJetAK8FlavourAssociation,
    genJetAK8FlavourTable,
    fatJetMCTable,
    genSubJetAK8Table,
    subjetMCTable
)
jetMCTask = jetMCTaskak4.copyAndAdd(jetMCTaskak8)
