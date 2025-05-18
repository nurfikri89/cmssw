import FWCore.ParameterSet.Config as cms

from PhysicsTools.NanoAOD.nano_eras_cff import *
from PhysicsTools.NanoAOD.common_cff import *
from PhysicsTools.NanoAOD.simplePATJetFlatTableProducer_cfi import simplePATJetFlatTableProducer

##################### User floats producers, selectors ##########################

from  PhysicsTools.PatAlgos.recoLayer0.jetCorrFactors_cfi import *
# Note: Safe to always add 'L2L3Residual' as MC contains dummy L2L3Residual corrections (always set to 1)
#      (cf. https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#CMSSW_7_6_4_and_above )
jetCorrFactorsNano = patJetCorrFactors.clone(src='slimmedJets',
    levels = cms.vstring('L1FastJet',
        'L2Relative',
        'L3Absolute',
        'L2L3Residual'),
    primaryVertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
)

from  PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cfi import *
updatedJets = updatedPatJets.clone(
    addBTagInfo=False,
    jetSource='slimmedJets',
    jetCorrFactorsSource=cms.VInputTag(cms.InputTag("jetCorrFactorsNano") ),
)

jercVars = cms.EDProducer("BetaStarPackedCandidateVarProducer",
    srcJet = cms.InputTag("updatedJets"),
    srcPF = cms.InputTag("packedPFCandidates"),
    maxDR = cms.double(0.4)
)

jetCaloEnergy = cms.EDProducer("PatJetPFConstituentCaloEnergyProducer",
    jets = cms.InputTag("updatedJets"),
    packedPFCandidates = cms.InputTag("packedPFCandidates"),
)

updatedJetsWithUserData = cms.EDProducer("PATJetUserDataEmbedder",
    src = cms.InputTag("updatedJets"),
    userFloats = cms.PSet(
        qgl = cms.InputTag('qgtagger:qgLikelihood'),
        puIdNanoDisc = cms.InputTag('pileupJetIdNano:fullDiscriminant'),
        chFPV0EF = cms.InputTag("jercVars:chargedFromPV0EnergyFraction"),
        ecalEnergy = cms.InputTag('jetCaloEnergy:ecalEnergy'),
        hcalEnergy = cms.InputTag('jetCaloEnergy:hcalEnergy'),
        hcalDepth1Energy = cms.InputTag('jetCaloEnergy:hcalDepth1Energy'),
        hcalDepth2Energy = cms.InputTag('jetCaloEnergy:hcalDepth2Energy'),
        hcalDepth3Energy = cms.InputTag('jetCaloEnergy:hcalDepth3Energy'),
        hcalDepth4Energy = cms.InputTag('jetCaloEnergy:hcalDepth4Energy'),
        hcalDepth5Energy = cms.InputTag('jetCaloEnergy:hcalDepth5Energy'),
        hcalDepth6Energy = cms.InputTag('jetCaloEnergy:hcalDepth6Energy'),
        hcalDepth7Energy = cms.InputTag('jetCaloEnergy:hcalDepth7Energy'),
        hoEnergy = cms.InputTag('jetCaloEnergy:hoEnergy'),
        ps1Energy = cms.InputTag('jetCaloEnergy:ps1Energy'),
        ps2Energy = cms.InputTag('jetCaloEnergy:ps2Energy'),
        ecalEnergyRaw = cms.InputTag('jetCaloEnergy:ecalEnergyRaw'),
        hcalEnergyRaw = cms.InputTag('jetCaloEnergy:hcalEnergyRaw'),
        hcalDepth1EnergyRaw = cms.InputTag('jetCaloEnergy:hcalDepth1EnergyRaw'),
        hcalDepth2EnergyRaw = cms.InputTag('jetCaloEnergy:hcalDepth2EnergyRaw'),
        hcalDepth3EnergyRaw = cms.InputTag('jetCaloEnergy:hcalDepth3EnergyRaw'),
        hcalDepth4EnergyRaw = cms.InputTag('jetCaloEnergy:hcalDepth4EnergyRaw'),
        hcalDepth5EnergyRaw = cms.InputTag('jetCaloEnergy:hcalDepth5EnergyRaw'),
        hcalDepth6EnergyRaw = cms.InputTag('jetCaloEnergy:hcalDepth6EnergyRaw'),
        hcalDepth7EnergyRaw = cms.InputTag('jetCaloEnergy:hcalDepth7EnergyRaw'),
        hfHadEnergy = cms.InputTag('jetCaloEnergy:hfHadEnergy'),
        hfEmEnergy = cms.InputTag('jetCaloEnergy:hfEmEnergy'),
        hfShortEnergy = cms.InputTag('jetCaloEnergy:hfShortEnergy'),
        hfLongEnergy = cms.InputTag('jetCaloEnergy:hfLongEnergy'),
        ecalEnergyFromChgHad          = cms.InputTag('jetCaloEnergy:ecalEnergyFromChgHad'),
        hcalEnergyFromChgHad          = cms.InputTag('jetCaloEnergy:hcalEnergyFromChgHad'),
        hoEnergyFromChgHad            = cms.InputTag('jetCaloEnergy:hoEnergyFromChgHad'),
        ecalEnergyRawFromChgHad       = cms.InputTag('jetCaloEnergy:ecalEnergyRawFromChgHad'),
        hcalEnergyRawFromChgHad       = cms.InputTag('jetCaloEnergy:hcalEnergyRawFromChgHad'),
        hcalDepth1EnergyRawFromChgHad = cms.InputTag('jetCaloEnergy:hcalDepth1EnergyRawFromChgHad'),
        hcalDepth2EnergyRawFromChgHad = cms.InputTag('jetCaloEnergy:hcalDepth2EnergyRawFromChgHad'),
        hcalDepth3EnergyRawFromChgHad = cms.InputTag('jetCaloEnergy:hcalDepth3EnergyRawFromChgHad'),
        hcalDepth4EnergyRawFromChgHad = cms.InputTag('jetCaloEnergy:hcalDepth4EnergyRawFromChgHad'),
        hcalDepth5EnergyRawFromChgHad = cms.InputTag('jetCaloEnergy:hcalDepth5EnergyRawFromChgHad'),
        hcalDepth6EnergyRawFromChgHad = cms.InputTag('jetCaloEnergy:hcalDepth6EnergyRawFromChgHad'),
        hcalDepth7EnergyRawFromChgHad = cms.InputTag('jetCaloEnergy:hcalDepth7EnergyRawFromChgHad'),
        hcalEnergyFromNeuHad          = cms.InputTag('jetCaloEnergy:hcalEnergyFromNeuHad'),
        hoEnergyFromNeuHad            = cms.InputTag('jetCaloEnergy:hoEnergyFromNeuHad'),
        hcalEnergyRawFromNeuHad       = cms.InputTag('jetCaloEnergy:hcalEnergyRawFromNeuHad'),
        hcalDepth1EnergyRawFromNeuHad = cms.InputTag('jetCaloEnergy:hcalDepth1EnergyRawFromNeuHad'),
        hcalDepth2EnergyRawFromNeuHad = cms.InputTag('jetCaloEnergy:hcalDepth2EnergyRawFromNeuHad'),
        hcalDepth3EnergyRawFromNeuHad = cms.InputTag('jetCaloEnergy:hcalDepth3EnergyRawFromNeuHad'),
        hcalDepth4EnergyRawFromNeuHad = cms.InputTag('jetCaloEnergy:hcalDepth4EnergyRawFromNeuHad'),
        hcalDepth5EnergyRawFromNeuHad = cms.InputTag('jetCaloEnergy:hcalDepth5EnergyRawFromNeuHad'),
        hcalDepth6EnergyRawFromNeuHad = cms.InputTag('jetCaloEnergy:hcalDepth6EnergyRawFromNeuHad'),
        hcalDepth7EnergyRawFromNeuHad = cms.InputTag('jetCaloEnergy:hcalDepth7EnergyRawFromNeuHad'),
        ecalEnergyFromPhoton          = cms.InputTag('jetCaloEnergy:ecalEnergyFromPhoton'),
        ecalEnergyRawFromPhoton       = cms.InputTag('jetCaloEnergy:ecalEnergyRawFromPhoton'),
        ecalEnergyFromElectron        = cms.InputTag('jetCaloEnergy:ecalEnergyFromElectron'),
        ecalEnergyRawFromElectron     = cms.InputTag('jetCaloEnergy:ecalEnergyRawFromElectron'),
        ecalEnergyFromMuon            = cms.InputTag('jetCaloEnergy:ecalEnergyFromMuon'),
        hcalEnergyFromMuon            = cms.InputTag('jetCaloEnergy:hcalEnergyFromMuon'),
        hoEnergyFromMuon              = cms.InputTag('jetCaloEnergy:hoEnergyFromMuon'),
        ecalEnergyRawFromMuon         = cms.InputTag('jetCaloEnergy:ecalEnergyRawFromMuon'),
        hcalEnergyRawFromMuon         = cms.InputTag('jetCaloEnergy:hcalEnergyRawFromMuon'),
        hcalDepth1EnergyRawFromMuon   = cms.InputTag('jetCaloEnergy:hcalDepth1EnergyRawFromMuon'),
        hcalDepth2EnergyRawFromMuon   = cms.InputTag('jetCaloEnergy:hcalDepth2EnergyRawFromMuon'),
        hcalDepth3EnergyRawFromMuon   = cms.InputTag('jetCaloEnergy:hcalDepth3EnergyRawFromMuon'),
        hcalDepth4EnergyRawFromMuon   = cms.InputTag('jetCaloEnergy:hcalDepth4EnergyRawFromMuon'),
        hcalDepth5EnergyRawFromMuon   = cms.InputTag('jetCaloEnergy:hcalDepth5EnergyRawFromMuon'),
        hcalDepth6EnergyRawFromMuon   = cms.InputTag('jetCaloEnergy:hcalDepth6EnergyRawFromMuon'),
        hcalDepth7EnergyRawFromMuon   = cms.InputTag('jetCaloEnergy:hcalDepth7EnergyRawFromMuon'),
    ),
    userInts = cms.PSet(
        puIdNanoId = cms.InputTag('pileupJetIdNano:fullId'),
    ),
)


finalJets = cms.EDFilter("PATJetRefSelector",
    src = cms.InputTag("updatedJetsWithUserData"),
    cut = cms.string("pt > 0")
)


##################### Tables for final output and docs ##########################

jetTable = simplePATJetFlatTableProducer.clone(
    src = cms.InputTag("finalJets"),
    name = cms.string("JetCHS"),
    doc  = cms.string("slimmedJets, i.e. ak4 PFJets CHS with JECs applied, after basic selection (" + finalJets.cut.value()+")"),
    externalVariables = cms.PSet(),
    variables = cms.PSet(P4Vars,
        area = Var("jetArea()", float, doc="jet catchment area, for JECs",precision=10),
        btagPNetB = Var("?bDiscriminator('pfParticleNetFromMiniAODAK4CHSCentralDiscriminatorsJetTags:BvsAll')>0?bDiscriminator('pfParticleNetFromMiniAODAK4CHSCentralDiscriminatorsJetTags:BvsAll'):-1",float,precision=10,doc="ParticleNet b vs. udscg"),
        btagPNetCvL = Var("?bDiscriminator('pfParticleNetFromMiniAODAK4CHSCentralDiscriminatorsJetTags:CvsL')>0?bDiscriminator('pfParticleNetFromMiniAODAK4CHSCentralDiscriminatorsJetTags:CvsL'):-1",float,precision=10,doc="ParticleNet c vs. udsg"),
        btagPNetCvB = Var("?bDiscriminator('pfParticleNetFromMiniAODAK4CHSCentralDiscriminatorsJetTags:CvsB')>0?bDiscriminator('pfParticleNetFromMiniAODAK4CHSCentralDiscriminatorsJetTags:CvsB'):-1",float,precision=10,doc="ParticleNet c vs. b"),
        btagPNetCvNotB = Var("?bDiscriminator('pfParticleNetFromMiniAODAK4CHSCentralJetTags:probb')>0?bDiscriminator('pfParticleNetFromMiniAODAK4CHSCentralJetTags:probc')/(1.-bDiscriminator('pfParticleNetFromMiniAODAK4CHSCentralJetTags:probb')):-1",float,precision=10,doc="ParticleNet C vs notB"),
        btagPNetQvG = Var("?abs(eta())<2.5?bDiscriminator('pfParticleNetFromMiniAODAK4CHSCentralDiscriminatorsJetTags:QvsG'):bDiscriminator('pfParticleNetFromMiniAODAK4CHSForwardDiscriminatorsJetTags:QvsG')",float,precision=10,doc="ParticleNet q (udsbc) vs. g"),
        btagPNetTauVJet = Var("?bDiscriminator('pfParticleNetFromMiniAODAK4CHSCentralDiscriminatorsJetTags:TauVsJet')>0?bDiscriminator('pfParticleNetFromMiniAODAK4CHSCentralDiscriminatorsJetTags:TauVsJet'):-1",float,precision=10,doc="ParticleNet tau vs. jet"),
        PNetRegPtRawCorr = Var("?abs(eta())<2.5?bDiscriminator('pfParticleNetFromMiniAODAK4CHSCentralJetTags:ptcorr'):bDiscriminator('pfParticleNetFromMiniAODAK4CHSForwardJetTags:ptcorr')",float,precision=10,doc="ParticleNet universal flavor-aware visible pT regression (no neutrinos), correction relative to raw jet pT"),
        PNetRegPtRawCorrNeutrino = Var("?abs(eta())<2.5?bDiscriminator('pfParticleNetFromMiniAODAK4CHSCentralJetTags:ptnu'):bDiscriminator('pfParticleNetFromMiniAODAK4CHSForwardJetTags:ptnu')",float,precision=10,doc="ParticleNet universal flavor-aware pT regression neutrino correction, relative to visible. To apply full regression, multiply raw jet pT by both PNetRegPtRawCorr and PNetRegPtRawCorrNeutrino."),
        PNetRegPtRawRes = Var("?abs(eta())<2.5?0.5*(bDiscriminator('pfParticleNetFromMiniAODAK4CHSCentralJetTags:ptreshigh')-bDiscriminator('pfParticleNetFromMiniAODAK4CHSCentralJetTags:ptreslow')):0.5*(bDiscriminator('pfParticleNetFromMiniAODAK4CHSForwardJetTags:ptreshigh')-bDiscriminator('pfParticleNetFromMiniAODAK4CHSForwardJetTags:ptreslow'))",float,precision=10,doc="ParticleNet universal flavor-aware jet pT resolution estimator, (q84 - q16)/2"),
        puIdDisc = Var("userFloat('puIdNanoDisc')", float,doc="Pileup ID discriminant with 106X (2018) training",precision=10),
        puId = Var("userInt('puIdNanoId')", "uint8", doc="Pileup ID flags with 106X (2018) training"),
        qgl = Var("?userFloat('qgl')>0?userFloat('qgl'):-1",float,doc="Quark vs Gluon likelihood discriminator",precision=10),
        hfsigmaEtaEta = Var("userFloat('hfJetShowerShape:sigmaEtaEta')",float,doc="sigmaEtaEta for HF jets (noise discriminating variable)",precision=10),
        hfsigmaPhiPhi = Var("userFloat('hfJetShowerShape:sigmaPhiPhi')",float,doc="sigmaPhiPhi for HF jets (noise discriminating variable)",precision=10),
        hfcentralEtaStripSize = Var("userInt('hfJetShowerShape:centralEtaStripSize')", int, doc="eta size of the central tower strip in HF (noise discriminating variable) "),
        hfadjacentEtaStripsSize = Var("userInt('hfJetShowerShape:adjacentEtaStripsSize')", int, doc="eta size of the strips next to the central tower strip in HF (noise discriminating variable) "),
        nConstituents = Var("numberOfDaughters()","uint8",doc="Number of particles in the jet"),
        chMultiplicity = Var("chargedMultiplicity()","uint8",doc="Number of charged particles in the jet"),
        neMultiplicity = Var("neutralMultiplicity()","uint8",doc="Number of neutral particles in the jet"),
        rawFactor = Var("1.-jecFactor('Uncorrected')",float,doc="1 - Factor to get back to raw pT",precision=-1),
        chHEF = Var("chargedHadronEnergyFraction()", float, doc="charged Hadron Energy Fraction", precision=-1),
        neHEF = Var("neutralHadronEnergyFraction()", float, doc="neutral Hadron Energy Fraction", precision=-1),
        chEmEF = Var("chargedEmEnergyFraction()", float, doc="charged Electromagnetic Energy Fraction", precision=-1),
        neEmEF = Var("neutralEmEnergyFraction()", float, doc="neutral Electromagnetic Energy Fraction", precision=-1),
        hfHEF = Var("HFHadronEnergyFraction()",float,doc="hadronic Energy Fraction in HF",precision=-1),
        hfEmEF = Var("HFEMEnergyFraction()",float,doc="electromagnetic Energy Fraction in HF",precision=-1),
        muEF = Var("muonEnergyFraction()", float, doc="muon Energy Fraction", precision=-1),
        chFPV0EF = Var("userFloat('chFPV0EF')", float, doc="charged fromPV==0 Energy Fraction (energy excluded from CHS jets). Previously called betastar.", precision=12),
        chHadMultiplicity = Var("chargedHadronMultiplicity()","int16",doc="number of charged hadrons in the jet"),
        neHadMultiplicity = Var("neutralHadronMultiplicity()","int16",doc="number of neutral hadrons in the jet"),
        hfHadMultiplicity = Var("HFHadronMultiplicity()", "int16",doc="number of HF hadrons in the jet"),
        hfEMMultiplicity  = Var("HFEMMultiplicity()","int16",doc="number of HF EMs in the jet"),
        muMultiplicity    = Var("muonMultiplicity()","int16",doc="number of muons in the jet"),
        elMultiplicity    = Var("electronMultiplicity()","int16",doc="number of electrons in the jet"),
        phoMultiplicity   = Var("photonMultiplicity()","int16",doc="number of photons in the jet"),
        #
        ecalEnergy = Var("userFloat('ecalEnergy')", float, doc="ecalEnergy", precision=-1),
        hcalEnergy = Var("userFloat('hcalEnergy')", float, doc="hcalEnergy", precision=-1),
        hcalDepth1Energy = Var("userFloat('hcalDepth1Energy')", float, doc="hcalDepth1Energy", precision=-1),
        hcalDepth2Energy = Var("userFloat('hcalDepth2Energy')", float, doc="hcalDepth2Energy", precision=-1),
        hcalDepth3Energy = Var("userFloat('hcalDepth3Energy')", float, doc="hcalDepth3Energy", precision=-1),
        hcalDepth4Energy = Var("userFloat('hcalDepth4Energy')", float, doc="hcalDepth4Energy", precision=-1),
        hcalDepth5Energy = Var("userFloat('hcalDepth5Energy')", float, doc="hcalDepth5Energy", precision=-1),
        hcalDepth6Energy = Var("userFloat('hcalDepth6Energy')", float, doc="hcalDepth6Energy", precision=-1),
        hcalDepth7Energy = Var("userFloat('hcalDepth7Energy')", float, doc="hcalDepth7Energy", precision=-1),
        hoEnergy = Var("userFloat('hoEnergy')", float, doc="hoEnergy", precision=-1),
        ps1Energy = Var("userFloat('ps1Energy')", float, doc="ps1Energy", precision=-1),
        ps2Energy = Var("userFloat('ps2Energy')", float, doc="ps2Energy", precision=-1),
        ecalEnergyRaw = Var("userFloat('ecalEnergyRaw')", float, doc="ecalEnergyRaw", precision=-1),
        hcalEnergyRaw = Var("userFloat('hcalEnergyRaw')", float, doc="hcalEnergyRaw", precision=-1),
        hcalDepth1EnergyRaw = Var("userFloat('hcalDepth1EnergyRaw')", float, doc="hcalDepth1EnergyRaw", precision=-1),
        hcalDepth2EnergyRaw = Var("userFloat('hcalDepth2EnergyRaw')", float, doc="hcalDepth2EnergyRaw", precision=-1),
        hcalDepth3EnergyRaw = Var("userFloat('hcalDepth3EnergyRaw')", float, doc="hcalDepth3EnergyRaw", precision=-1),
        hcalDepth4EnergyRaw = Var("userFloat('hcalDepth4EnergyRaw')", float, doc="hcalDepth4EnergyRaw", precision=-1),
        hcalDepth5EnergyRaw = Var("userFloat('hcalDepth5EnergyRaw')", float, doc="hcalDepth5EnergyRaw", precision=-1),
        hcalDepth6EnergyRaw = Var("userFloat('hcalDepth6EnergyRaw')", float, doc="hcalDepth6EnergyRaw", precision=-1),
        hcalDepth7EnergyRaw = Var("userFloat('hcalDepth7EnergyRaw')", float, doc="hcalDepth7EnergyRaw", precision=-1),
        hfHadEnergy = Var("userFloat('hfHadEnergy')", float, doc="hfHadEnergy", precision=-1),
        hfEmEnergy = Var("userFloat('hfEmEnergy')", float, doc="hfEmEnergy", precision=-1),
        hfShortEnergy = Var("userFloat('hfShortEnergy')", float, doc="hfShortEnergy", precision=-1),
        hfLongEnergy = Var("userFloat('hfLongEnergy')", float, doc="hfLongEnergy", precision=-1),
        ecalEnergyFromChgHad = Var("userFloat('ecalEnergyFromChgHad')", float, doc="ecalEnergyFromChgHad", precision=-1),
        hcalEnergyFromChgHad = Var("userFloat('hcalEnergyFromChgHad')", float, doc="hcalEnergyFromChgHad", precision=-1),
        hoEnergyFromChgHad = Var("userFloat('hoEnergyFromChgHad')", float, doc="hoEnergyFromChgHad", precision=-1),
        ecalEnergyRawFromChgHad = Var("userFloat('ecalEnergyRawFromChgHad')", float, doc="ecalEnergyRawFromChgHad", precision=-1),
        hcalEnergyRawFromChgHad = Var("userFloat('hcalEnergyRawFromChgHad')", float, doc="hcalEnergyRawFromChgHad", precision=-1),
        hcalDepth1EnergyRawFromChgHad = Var("userFloat('hcalDepth1EnergyRawFromChgHad')", float, doc="hcalDepth1EnergyRawFromChgHad", precision=-1),
        hcalDepth2EnergyRawFromChgHad = Var("userFloat('hcalDepth2EnergyRawFromChgHad')", float, doc="hcalDepth2EnergyRawFromChgHad", precision=-1),
        hcalDepth3EnergyRawFromChgHad = Var("userFloat('hcalDepth3EnergyRawFromChgHad')", float, doc="hcalDepth3EnergyRawFromChgHad", precision=-1),
        hcalDepth4EnergyRawFromChgHad = Var("userFloat('hcalDepth4EnergyRawFromChgHad')", float, doc="hcalDepth4EnergyRawFromChgHad", precision=-1),
        hcalDepth5EnergyRawFromChgHad = Var("userFloat('hcalDepth5EnergyRawFromChgHad')", float, doc="hcalDepth5EnergyRawFromChgHad", precision=-1),
        hcalDepth6EnergyRawFromChgHad = Var("userFloat('hcalDepth6EnergyRawFromChgHad')", float, doc="hcalDepth6EnergyRawFromChgHad", precision=-1),
        hcalDepth7EnergyRawFromChgHad = Var("userFloat('hcalDepth7EnergyRawFromChgHad')", float, doc="hcalDepth7EnergyRawFromChgHad", precision=-1),
        hcalEnergyFromNeuHad = Var("userFloat('hcalEnergyFromNeuHad')", float, doc="hcalEnergyFromNeuHad", precision=-1),
        hoEnergyFromNeuHad = Var("userFloat('hoEnergyFromNeuHad')", float, doc="hoEnergyFromNeuHad", precision=-1),
        hcalEnergyRawFromNeuHad = Var("userFloat('hcalEnergyRawFromNeuHad')", float, doc="hcalEnergyRawFromNeuHad", precision=-1),
        hcalDepth1EnergyRawFromNeuHad = Var("userFloat('hcalDepth1EnergyRawFromNeuHad')", float, doc="hcalDepth1EnergyRawFromNeuHad", precision=-1),
        hcalDepth2EnergyRawFromNeuHad = Var("userFloat('hcalDepth2EnergyRawFromNeuHad')", float, doc="hcalDepth2EnergyRawFromNeuHad", precision=-1),
        hcalDepth3EnergyRawFromNeuHad = Var("userFloat('hcalDepth3EnergyRawFromNeuHad')", float, doc="hcalDepth3EnergyRawFromNeuHad", precision=-1),
        hcalDepth4EnergyRawFromNeuHad = Var("userFloat('hcalDepth4EnergyRawFromNeuHad')", float, doc="hcalDepth4EnergyRawFromNeuHad", precision=-1),
        hcalDepth5EnergyRawFromNeuHad = Var("userFloat('hcalDepth5EnergyRawFromNeuHad')", float, doc="hcalDepth5EnergyRawFromNeuHad", precision=-1),
        hcalDepth6EnergyRawFromNeuHad = Var("userFloat('hcalDepth6EnergyRawFromNeuHad')", float, doc="hcalDepth6EnergyRawFromNeuHad", precision=-1),
        hcalDepth7EnergyRawFromNeuHad = Var("userFloat('hcalDepth7EnergyRawFromNeuHad')", float, doc="hcalDepth7EnergyRawFromNeuHad", precision=-1),
        ecalEnergyFromPhoton = Var("userFloat('ecalEnergyFromPhoton')", float, doc="ecalEnergyFromPhoton", precision=-1),
        ecalEnergyRawFromPhoton = Var("userFloat('ecalEnergyRawFromPhoton')", float, doc="ecalEnergyRawFromPhoton", precision=-1),
        ecalEnergyFromElectron = Var("userFloat('ecalEnergyFromElectron')", float, doc="ecalEnergyFromElectron", precision=-1),
        ecalEnergyRawFromElectron = Var("userFloat('ecalEnergyRawFromElectron')", float, doc="ecalEnergyRawFromElectron", precision=-1),
        ecalEnergyFromMuon = Var("userFloat('ecalEnergyFromMuon')", float, doc="ecalEnergyFromMuon", precision=-1),
        hcalEnergyFromMuon = Var("userFloat('hcalEnergyFromMuon')", float, doc="hcalEnergyFromMuon", precision=-1),
        hoEnergyFromMuon = Var("userFloat('hoEnergyFromMuon')", float, doc="hoEnergyFromMuon", precision=-1),
        ecalEnergyRawFromMuon = Var("userFloat('ecalEnergyRawFromMuon')", float, doc="ecalEnergyRawFromMuon", precision=-1),
        hcalEnergyRawFromMuon = Var("userFloat('hcalEnergyRawFromMuon')", float, doc="hcalEnergyRawFromMuon", precision=-1),
        hcalDepth1EnergyRawFromMuon = Var("userFloat('hcalDepth1EnergyRawFromMuon')", float, doc="hcalDepth1EnergyRawFromMuon", precision=-1),
        hcalDepth2EnergyRawFromMuon = Var("userFloat('hcalDepth2EnergyRawFromMuon')", float, doc="hcalDepth2EnergyRawFromMuon", precision=-1),
        hcalDepth3EnergyRawFromMuon = Var("userFloat('hcalDepth3EnergyRawFromMuon')", float, doc="hcalDepth3EnergyRawFromMuon", precision=-1),
        hcalDepth4EnergyRawFromMuon = Var("userFloat('hcalDepth4EnergyRawFromMuon')", float, doc="hcalDepth4EnergyRawFromMuon", precision=-1),
        hcalDepth5EnergyRawFromMuon = Var("userFloat('hcalDepth5EnergyRawFromMuon')", float, doc="hcalDepth5EnergyRawFromMuon", precision=-1),
        hcalDepth6EnergyRawFromMuon = Var("userFloat('hcalDepth6EnergyRawFromMuon')", float, doc="hcalDepth6EnergyRawFromMuon", precision=-1),
        hcalDepth7EnergyRawFromMuon = Var("userFloat('hcalDepth7EnergyRawFromMuon')", float, doc="hcalDepth7EnergyRawFromMuon", precision=-1),
    )
)

#jets are not as precise as muons
jetTable.variables.pt.precision=-1
jetTable.variables.eta.precision=-1
jetTable.variables.phi.precision=-1
jetTable.variables.mass.precision=-1

#
# Quark-Gluon Likelihood (QGL)
#
from RecoJets.JetProducers.QGTagger_cfi import  QGTagger
qgtagger=QGTagger.clone(srcJets="updatedJets",srcVertexCollection="offlineSlimmedPrimaryVertices")

#
# PileUp ID
#
from RecoJets.JetProducers.PileupJetID_cfi import pileupJetId, _chsalgos_106X_UL18
pileupJetIdNano=pileupJetId.clone(jets="updatedJets",algos = cms.VPSet(_chsalgos_106X_UL18),inputIsCorrected=True,applyJec=False,vertexes="offlineSlimmedPrimaryVertices")

################################################################################
# JETS FOR MET type1
################################################################################
basicJetsForMetForT1METNano = cms.EDProducer("PATJetCleanerForType1MET",
    src = updatedJetsWithUserData.src,
    jetCorrEtaMax = cms.double(9.9),
    jetCorrLabel = cms.InputTag("L3Absolute"),
    jetCorrLabelRes = cms.InputTag("L2L3Residual"),
    offsetCorrLabel = cms.InputTag("L1FastJet"),
    skipEM = cms.bool(False),
    skipEMfractionThreshold = cms.double(0.9),
    skipMuonSelection = cms.string('isGlobalMuon | isStandAloneMuon'),
    skipMuons = cms.bool(True),
    type1JetPtThreshold = cms.double(0.0),
    calcMuonSubtrRawPtAsValueMap = cms.bool(True)
)

updatedJetsWithUserData.userFloats.muonSubtrRawPt = cms.InputTag("basicJetsForMetForT1METNano:MuonSubtrRawPt")
updatedJetsWithUserData.userFloats.muonSubtrRawEta = cms.InputTag("basicJetsForMetForT1METNano:MuonSubtrRawEta")
updatedJetsWithUserData.userFloats.muonSubtrRawPhi = cms.InputTag("basicJetsForMetForT1METNano:MuonSubtrRawPhi")

corrT1METJetTable = simplePATJetFlatTableProducer.clone(
    src = finalJets.src,
    cut = cms.string("pt<15 && abs(eta)<9.9"),
    name = cms.string("CorrT1METJetCHS"),
    doc  = cms.string("Additional low-pt ak4 CHS jets for Type-1 MET re-correction"),
    variables = cms.PSet(
        rawPt = Var("pt()*jecFactor('Uncorrected')",float,precision=-1),
        rawMass = Var("mass()*jecFactor('Uncorrected')",float,precision=-1),
        eta  = Var("eta",  float,precision=-1),
        phi = Var("phi", float, precision=-1),
        area = Var("jetArea()", float, doc="jet catchment area, for JECs",precision=10),
        EmEF = Var("chargedEmEnergyFraction()+neutralEmEnergyFraction()", float, doc="charged+neutral Electromagnetic Energy Fraction", precision=-1),
    )
)

corrT1METJetTable.variables.muonSubtrFactor = Var("1-userFloat('muonSubtrRawPt')/(pt()*jecFactor('Uncorrected'))",float,doc="1-(muon-subtracted raw pt)/(raw pt)",precision=-1)
corrT1METJetTable.variables.muonSubtrDeltaEta = Var("userFloat('muonSubtrRawEta') - eta()",float,doc="muon-subtracted raw eta - eta",precision=-1)
corrT1METJetTable.variables.muonSubtrDeltaPhi = Var("userFloat('muonSubtrRawPhi') - phi()",float,doc="muon-subtracted raw phi - phi",precision=-1)
jetTable.variables.muonSubtrFactor = Var("1-userFloat('muonSubtrRawPt')/(pt()*jecFactor('Uncorrected'))",float,doc="1-(muon-subtracted raw pt)/(raw pt)",precision=-1)
jetTable.variables.muonSubtrDeltaEta = Var("userFloat('muonSubtrRawEta') - eta()",float,doc="muon-subtracted raw eta - eta",precision=-1)
jetTable.variables.muonSubtrDeltaPhi = Var("userFloat('muonSubtrRawPhi') - phi()",float,doc="muon-subtracted raw phi - phi",precision=-1)

jetForMETTask =  cms.Task(basicJetsForMetForT1METNano,corrT1METJetTable)

#before cross linking
jetUserDataTask = cms.Task(qgtagger,jercVars,jetCaloEnergy,pileupJetIdNano)

#before cross linking
jetTask = cms.Task(jetCorrFactorsNano,updatedJets,jetUserDataTask,updatedJetsWithUserData,finalJets)

#after cross linkining
jetTablesTask = cms.Task(jetTable)
