import FWCore.ParameterSet.Config as cms
from PhysicsTools.BPHNano.common_cff import *
from PhysicsTools.NanoAOD.simplePATMuonFlatTableProducer_cfi import simplePATMuonFlatTableProducer

def nanoAOD_customize_KshortLL_LambdaLL(process):
    """
    # Takes slimmedMuons, apply basic preselection.
    process.muonBPH = cms.EDProducer("MuonNoTriggerSelector",
        muonCollection = cms.InputTag("slimmedMuons"),
        muonSelection  = cms.string("pt > 2 && abs(eta) < 2.4"), # Preselection
    )

    process.BPHMuonTable = simplePATMuonFlatTableProducer.clone(
        src  = cms.InputTag("muonBPH:SelectedMuons"),
        cut  = cms.string(""), # We should not filter on cross linked collections
        name = cms.string("BPHMuon"),
        doc  = cms.string("slimmedMuons after basic selection"),
        singleton = cms.bool(False), # the number of entries is variable
        extension = cms.bool(False), # this is the main table for the muons
        variables = cms.PSet(
            CandVars,
            ptErr   = Var("bestTrack().ptError()", float, doc="ptError of the muon track", precision=10),
            covQopQop = Var("bestTrack().covariance(0, 0)", float, doc="Cov of q/p with q/p", precision=10),
            covLamLam = Var("bestTrack().covariance(1, 1)", float, doc="Cov of lambda with lambda", precision=10),
            covPhiPhi = Var("bestTrack().covariance(2, 2)", float, doc="Cov of phi with phi", precision=10),
            covQopLam = Var("bestTrack().covariance(0, 1)", float, doc="Cov of q/p with lambda", precision=10),
            covQopPhi = Var("bestTrack().covariance(0, 2)", float, doc="Cov of q/p with phi", precision=10),
            covLamPhi = Var("bestTrack().covariance(1, 2)", float, doc="Cov of lambda with phi", precision=10),
            dz      = Var("dB('PVDZ')", float, doc="dz (with sign) wrt PV[0] [cm]", precision=10),
            dzErr   = Var("abs(edB('PVDZ'))", float, doc="dz uncertainty [cm]", precision=10),
            dxy     = Var("dB('PV2D')", float, doc="dxy (with sign) wrt PV[0] [cm]", precision=10),
            dxyErr  = Var("edB('PV2D')", float, doc="dxy uncertainty [cm]", precision=10),
            ip3d    = Var("abs(dB('PV3D'))", float, doc="3D impact parameter wrt PV[0] [cm]", precision=10),
            sip3d   = Var("abs(dB('PV3D')/edB('PV3D'))", float, doc="3D impact parameter significance wrt PV[0]", precision=10),
            pfRelIso03_all = Var("(pfIsolationR03().sumChargedHadronPt + max(pfIsolationR03().sumNeutralHadronEt + pfIsolationR03().sumPhotonEt - pfIsolationR03().sumPUPt/2,0.0))/pt", float, doc="PF relative isolation dR=0.3, total (deltaBeta corrections)", precision=10),
            pfRelIso04_all = Var("(pfIsolationR04().sumChargedHadronPt + max(pfIsolationR04().sumNeutralHadronEt + pfIsolationR04().sumPhotonEt - pfIsolationR04().sumPUPt/2,0.0))/pt", float, doc="PF relative isolation dR=0.4, total (deltaBeta corrections)", precision=10),
            isPFcand    = Var("isPFMuon", bool, doc="muon is PF candidate"),
            isGlobal    = Var("isGlobalMuon", bool, doc="muon is global muon"),
            isTracker   = Var("isTrackerMuon", bool, doc="muon is tracker muon"),
            looseId     = Var("passed('CutBasedIdLoose')", bool, doc="cut-based ID, medium WP"),
            mediumId    = Var("passed('CutBasedIdMedium')", bool, doc="cut-based ID, medium WP"),
            triggerIdLoose  = Var("passed('TriggerIdLoose')", bool, doc="TriggerIdLoose ID"),
            softId = Var("passed('SoftCutBasedId')",bool,doc="soft cut-based ID"),
            softMvaId = Var("passed('SoftMvaId')",bool,doc="soft MVA ID"),
            softMva = Var("softMvaValue()",float,doc="soft MVA ID score",precision=6),
            softMvaRun3 = Var("softMvaRun3Value()",float,doc="soft MVA Run3 ID score",precision=6),
        ),
    )

    process.MuMu = cms.EDProducer('DiMuonBuilder',
        src = cms.InputTag('muonBPH', 'SelectedMuons'),# FIKRI:Use all muons, not the triggered ones
        transientTracksSrc = cms.InputTag('muonBPH', 'SelectedTransientMuons'),
        lep1Selection = cms.string('pt > 4.0 && abs(eta) < 2.4 && isLooseMuon && isGlobalMuon'),
        lep2Selection = cms.string('pt > 3.0 && abs(eta) < 2.4 && isLooseMuon && isGlobalMuon'),
        beamSpot = cms.InputTag("offlineBeamSpot"),
        preVtxSelection  = cms.string('abs(userCand("l1").vz - userCand("l2").vz) <= 1.'
                               '&& 0 < mass() && mass() < 15.0 '
                               '&& charge() == 0'
                               '&& userFloat("lep_deltaR") > 0.03'),
        postVtxSelection = cms.string('0 < userFloat("fitted_mass") && userFloat("fitted_mass") < 15.0'
                               '&& userFloat("sv_prob") > 0.001')
    )

    process.MuMuTable = cms.EDProducer("SimpleCompositeCandidateFlatTableProducer",
        src = cms.InputTag("MuMu:SelectedDiLeptons"),
        cut = cms.string(""), #we should not filter on cross linked collections
        name = cms.string("MuMu"),
        doc  = cms.string("Dilepton collections"),
        singleton = cms.bool(False), # the number of entries is variable
        extension = cms.bool(False), # this is the main table for the muons
        variables = cms.PSet(CandVars,
            l1_idx = Var("userInt('l1_idx')", int, doc = "leading muon index to the BPHMuon collection"),
            l2_idx = Var("userInt('l2_idx')", int, doc = "subleading muon index to the BPHMuon collection"),
            fit_mass = Var("userFloat('fitted_mass')", float, doc="Fitted dilepton mass", precision=10),
            fit_massErr = Var("userFloat('fitted_massErr')", float, doc = "post-fit uncertainty of the mass of the B candidate", precision=10),
            svprob = Var("userFloat('sv_prob')", float, doc="Vtx fit probability", precision=10),
            l_xy     = Var("userFloat('l_xy')", float, doc = "post-fit vertex displacement on transverse plane wrt beamspot", precision=10),
            l_xy_unc = Var("userFloat('l_xy_unc')", float, doc = "post-fit vertex uncertainty of displacement on transverse plane wrt beamspot", precision=10),
            vtx_x =Var("userFloat('vtx_x')", float, doc="Vtx position in x", precision=10),
            vtx_y = Var("userFloat('vtx_y')", float, doc="Vtx position in y", precision=10),
            vtx_z = Var("userFloat('vtx_z')", float, doc="Vtx position in y", precision=10),
            cos2D     = Var("userFloat('cos_theta_2D')", float, doc = "cos 2D of pre-fit candidate wrt beamspot", precision=10),
            fit_cos2D = Var("userFloat('fitted_cos_theta_2D')", float, doc = "cos 2D of fitted vertex wrt beamspot", precision=10),
        )
    )

    process.tracksBPH = cms.EDProducer("BPHTrackMerger",
        beamSpot        = cms.InputTag("offlineBeamSpot"),
        dileptons       = cms.InputTag("MuMu:SelectedDiLeptons"),
        tracks          = cms.InputTag("packedPFCandidates"),
        lostTracks      = cms.InputTag("lostTracks"),
        trackSelection  = cms.string("pt>0.7 && abs(eta)<3.0"),  # We need all tracks for tagging, no cuts here for now
        muons           = cms.InputTag("slimmedMuons"),
        electrons       = cms.InputTag("slimmedElectrons"),
        pvSrc           = cms.InputTag("offlineSlimmedPrimaryVertices"),
        maxDzDilep      = cms.double(1.0),
        dcaSig          = cms.double(-100000),
        doDileptonCheck = cms.bool(False),
    )

    process.trackBPHTable = cms.EDProducer("SimpleCompositeCandidateFlatTableProducer",
        src  = cms.InputTag("tracksBPH:SelectedTracks"),
        cut  = cms.string(""),
        name = cms.string("Track"),
        doc  = cms.string("track collection"),
        singleton = cms.bool(False),
        extension = cms.bool(False),
        variables = cms.PSet(
            CandVars,
            vx = Var("vx()", float, doc="x coordinate of of ref point [cm]", precision=6),
            vy = Var("vy()", float, doc="y coordinate of of ref point [cm]", precision=6),
            vz = Var("vz()", float, doc="z coordinate of of ref point [cm]", precision=6),
            # User variables defined in plugins/BPHTrackMerger.cc
            isPacked  = Var("userInt('isPacked')", bool, doc="track from packedCandidate collection"),
            isLostTrk = Var("userInt('isLostTrk')", bool, doc="track from lostTrack collection"),
            keyPacked  = Var("userInt('keyPacked')", int, doc="key in packedPFCandidates collection"),
            dz      = Var("userFloat('dz')", float, doc="dz signed wrt PV[0] [cm]", precision=6),
            dxy     = Var("userFloat('dxy')", float, doc="dxy (with sign) wrt PV associated with the track [cm]", precision=6),
            dzS     = Var("userFloat('dzS')", float, doc="dz/err (with sign) wrt PV[0] [cm]", precision=6),
            dxyS    = Var("userFloat('dxyS')", float, doc="dxy/err (with sign) wrt PV associated with the track [cm]", precision=6),
            DCASig  = Var("userFloat('DCASig')", float, doc="significance of xy-distance of closest approach wrt beamspot", precision=6),
            dzTrg   = Var("userFloat('dzTrg')", float, doc="dz from the corresponding trigger muon [cm]", precision=6),
            isMatchedToMuon = Var("userInt('isMatchedToMuon')", bool, doc="track was used to build a muon"),
            isMatchedToEle  = Var("userInt('isMatchedToEle')", bool, doc="track was used to build a PF ele"),
            nValidHits      = Var("userInt('nValidHits')", "uint8", doc="Number of valid hits"),
            # Covariance matrix elements for helix parameters for decay time uncertainty
            covQopQop = Var("userFloat('covQopQop')", float, doc="Cov. of q/p with q/p", precision=10),
            covQopLam = Var("userFloat('covQopLam')", float, doc="Cov. of q/p with lambda", precision=10),
            covQopPhi = Var("userFloat('covQopPhi')", float, doc="Cov. of q/p with phi", precision=10),
            covLamLam = Var("userFloat('covLamLam')", float, doc="Cov. of lambda with lambda", precision=10),
            covLamPhi = Var("userFloat('covLamPhi')", float, doc="Cov. of lambda with phi", precision=10),
            covPhiPhi = Var("userFloat('covPhiPhi')", float, doc="Cov. of phi with phi", precision=10),
            # Additional track parameters for tagging
            ptErr      = Var("userFloat('ptErr')", float, doc="Pt uncertainty", precision=6),
            normChi2   = Var("userFloat('normChi2')", float, doc="Track fit chi-squared divided by n.d.o.f.", precision=6),
            nValidPixelHits = Var("userInt('nValidPixelHits')", "uint8", doc="Number of pixel hits"),
            # TODO: check impact parameters
        ),
    )

    process.DiTrack = cms.EDProducer('DiTrackBuilder',
        tracks = cms.InputTag('tracksBPH', 'SelectedTracks'),
        transientTracks = cms.InputTag('tracksBPH', 'SelectedTransientTracks'),
        trk1Selection   = cms.string(''),
        trk2Selection   = cms.string(''),
        trk1Mass = cms.double(0.139),
        trk2Mass = cms.double(0.494),
        beamSpot = cms.InputTag("offlineBeamSpot"),
        preVtxSelection = cms.string('(userFloat("unfitted_mass_KK")>0.95 && userFloat("unfitted_mass_KK")<1.12) && charge() == 0'),
        postVtxSelection =  cms.string('(userFloat("fitted_mass_KK")>0.95 && userFloat("fitted_mass_KK")<1.12)   && userFloat("sv_prob") > 0.001'),
        # preVtxSelection = cms.string('((userFloat("unfitted_mass_KK")>0.95 && userFloat("unfitted_mass_KK")<1.12) || (userFloat("unfitted_mass_Kpi")>0.6 && userFloat("unfitted_mass_Kpi")<1.2) || (userFloat("unfitted_mass_piK")>0.6 && userFloat("unfitted_mass_piK")<1.2)) && charge() == 0'),
        # postVtxSelection =  cms.string('((userFloat("fitted_mass_KK")>0.95 && userFloat("fitted_mass_KK")<1.12) || (userFloat("fitted_mass_Kpi")>0.6 && userFloat("fitted_mass_Kpi")<1.2)  || (userFloat("fitted_mass_piK")>0.6 && userFloat("fitted_mass_piK")<1.2)) && userFloat("sv_prob") > 0.001')
    )

    process.DiTrackTable = cms.EDProducer("SimpleCompositeCandidateFlatTableProducer",
        src  = cms.InputTag("DiTrack"),
        cut  = cms.string(""), #we should not filter on cross linked collections
        name = cms.string("DiTrack"),
        doc  = cms.string("slimmedDiTrack for BPark after basic selection"),
        singleton = cms.bool(False), # the number of entries is variable
        extension = cms.bool(False), # this is the main table for the muons
        variables = cms.PSet(
            CandVars,
            fit_mass_KK = Var("userFloat('fitted_mass_KK')", float, doc="", precision=10),
            fit_mass_Kpi = Var("userFloat('fitted_mass_Kpi')", float, doc="", precision=10),
            fit_mass_piK = Var("userFloat('fitted_mass_piK')", float, doc="", precision=10),
            fit_pt   = Var("userFloat('fitted_pt')", float, doc="", precision=10),
            fit_eta  = Var("userFloat('fitted_eta')", float, doc="", precision=10),
            fit_phi  = Var("userFloat('fitted_phi')", float, doc="", precision=10),
            svprob      = Var("userFloat('sv_prob')", float, doc="", precision=10),
            trk1_idx    = Var("userInt('trk1_idx')", int, doc=""),
            trk2_idx    = Var("userInt('trk2_idx')", int, doc=""),
            #
            trk1_isPacked = Var("userCand('trk1').userInt('isPacked')", int,   doc="",lazyEval=True),
            trk2_isPacked = Var("userCand('trk2').userInt('isPacked')", int,   doc="",lazyEval=True),
            trk1_keyPacked = Var("userCand('trk1').userInt('keyPacked')", int, doc="",lazyEval=True),
            trk2_keyPacked = Var("userCand('trk2').userInt('keyPacked')", int, doc="",lazyEval=True),
            #
            vtx_x       = Var("userFloat('vtx_x')", float, doc="", precision=10),
            vtx_y       = Var("userFloat('vtx_y')", float, doc="", precision=10),
            vtx_z       = Var("userFloat('vtx_z')", float, doc="", precision=10),
            l_xy        = Var("userFloat('l_xy')", float, doc="", precision=10),
            l_xy_unc        = Var("userFloat('l_xy_unc')", float, doc="", precision=10),
            cos_theta_2D    = Var("userFloat('fitted_cos_theta_2D')", float, doc="", precision=10),
            sv_prob         = Var("userFloat('sv_prob')", float, doc="", precision=10),
            sv_ndof         = Var("userFloat('sv_ndof')", float, doc="", precision=10),
            sv_chi2         = Var("userFloat('sv_chi2')", float, doc="", precision=10),
            vtx_cxx = Var("userFloat('vtx_cxx')", float, doc="", precision=10),
            vtx_cyy = Var("userFloat('vtx_cyy')", float, doc="", precision=10),
            vtx_czz = Var("userFloat('vtx_czz')", float, doc="", precision=10),
            vtx_cyx = Var("userFloat('vtx_cyx')", float, doc="", precision=10),
            vtx_czx = Var("userFloat('vtx_czx')", float, doc="", precision=10),
            vtx_czy = Var("userFloat('vtx_czy')", float, doc="", precision=10)

        )
    )
    """

    process.tracksBPH = cms.EDProducer("BPHTrackMerger",
        beamSpot        = cms.InputTag("offlineBeamSpot"),
        dileptons       = cms.InputTag("MuMu:SelectedDiLeptons"),
        tracks          = cms.InputTag("packedPFCandidates"),
        lostTracks      = cms.InputTag("lostTracks"),
        trackSelection  = cms.string("pt>0.7 && abs(eta)<3.0"),  # We need all tracks for tagging, no cuts here for now
        muons           = cms.InputTag("slimmedMuons"),
        electrons       = cms.InputTag("slimmedElectrons"),
        pvSrc           = cms.InputTag("offlineSlimmedPrimaryVertices"),
        maxDzDilep      = cms.double(1.0),
        dcaSig          = cms.double(-100000),
    )

    ########################### Selections ###########################

    process.KshortToPiPi = cms.EDProducer(
        'V0ReBuilderV2',
        V0s = cms.InputTag('slimmedKshortVertices'),
        trkSelection = cms.string('pt > 0.35 && abs(eta) < 3.0 && trackHighPurity()'),
        V0Selection = cms.string('0.3 < mass && mass < 0.7'),
        postVtxSelection = cms.string('0.3 < mass && mass < 0.7'
                                      '&& userFloat("sv_prob") > 0.0001'),
        beamSpot = cms.InputTag("offlineBeamSpot"),
        packedPFCandidates = cms.InputTag("packedPFCandidates"),
        isLambda = cms.bool(False)
    )

    process.LambdaToProtonPi = cms.EDProducer(
        'V0ReBuilderV2',
        V0s = cms.InputTag('slimmedLambdaVertices'),
        trkSelection = cms.string('pt > 0.35 && abs(eta) < 3.0 && trackHighPurity()'),
        V0Selection = cms.string('1 < mass && mass < 1.2'),
        postVtxSelection = cms.string('1 < mass && mass < 1.17'
                                      '&& userFloat("sv_prob") > 0.0001'),
        beamSpot = cms.InputTag("offlineBeamSpot"),
        packedPFCandidates = cms.InputTag("packedPFCandidates"),
        isLambda = cms.bool(True)
    )


    ########################### Tables ###########################
    process.KshortToPiPiTable = cms.EDProducer('SimpleCompositeCandidateFlatTableProducer',
        src = cms.InputTag('KshortToPiPi','SelectedV0Collection'),
        cut = cms.string(""),
        name = cms.string("Kshort"),
        doc = cms.string("Kshort Variables"),
        singleton=cms.bool(False),
        extension=cms.bool(False),
        variables = cms.PSet(
            # pre-fit quantities=
            pt     = Var("pt",  float, precision=-1),
            phi    = Var("phi", float, precision=-1),
            eta    = Var("eta",  float,precision=-1),
            mass   = Var("mass", float,precision=-1),
            pdgId  = Var("pdgId", int, doc="PDG code assigned by the event reconstruction (not by MC truth)"),
            charge = Var("charge", int, doc="electric charge"),
            #
            prefit_trk1_pt  = Var("userFloat('prefit_trk1_pt')",  float, doc = "pre-fit pt of the leading (momentum) track",     precision=-1),
            prefit_trk1_eta = Var("userFloat('prefit_trk1_eta')", float, doc = "pre-fit eta of the leading (momentum) track",    precision=-1),
            prefit_trk1_phi = Var("userFloat('prefit_trk1_phi')", float, doc = "pre-fit phi of the leading (momentum) track",    precision=-1),
            prefit_trk1_p   = Var("userFloat('prefit_trk1_p')",   float, doc = "pre-fit p of the leading (momentum) track",      precision=-1),
            prefit_trk2_pt  = Var("userFloat('prefit_trk2_pt')",  float, doc = "pre-fit pt of the subleading (momentum) track",  precision=-1),
            prefit_trk2_eta = Var("userFloat('prefit_trk2_eta')", float, doc = "pre-fit eta of the subleading (momentum) track", precision=-1),
            prefit_trk2_phi = Var("userFloat('prefit_trk2_phi')", float, doc = "pre-fit phi of the subleading (momentum) track", precision=-1),
            prefit_trk2_p   = Var("userFloat('prefit_trk2_p')",   float, doc = "pre-fit p of the subleading (momentum) track",   precision=-1),
            prefit_trk1_keyPacked = Var("userInt('prefit_trk1_keyPacked')", int, doc=""),
            prefit_trk2_keyPacked = Var("userInt('prefit_trk2_keyPacked')", int, doc=""),
            # fit and vtx info=
            chi2 = Var("userFloat('sv_chi2')", float, doc = "chi2 of fitted vertex", precision=-1),
            svprob = Var("userFloat('sv_prob')", float, doc = "vertex probability of fitted vertex", precision=-1),
            l_xy = Var("userFloat('l_xy')", float, doc = "post-fit vertex displacement on transverse plane", precision=-1),
            l_xy_unc = Var("userFloat('l_xy_unc')", float, doc = "post-fit vertex uncertainty of the diplacement on the transverse plane", precision=-1),
            prefit_mass = Var("userFloat('prefit_mass')", float, doc = "pre-fit mass of the vertex", precision=-1),
            vtx_x = Var("userFloat('vtx_x')", float, doc = "x position of fitted vertex",   precision=-1),
            vtx_y = Var("userFloat('vtx_y')", float, doc = "y position of fitted vertex",   precision=-1),
            vtx_z = Var("userFloat('vtx_z')", float, doc = "z position of fitted vertex",   precision=-1),
            vtx_cxx = Var("userFloat('vtx_cxx')", float, doc = "error x of fitted vertex",  precision=-1),
            vtx_cyy = Var("userFloat('vtx_cyy')", float, doc = "error y of fitted vertex",  precision=-1),
            vtx_czz = Var("userFloat('vtx_czz')", float, doc = "error z of fitted vertex",  precision=-1),
            vtx_cyx = Var("userFloat('vtx_cyx')", float, doc = "error yx of fitted vertex", precision=-1),
            vtx_czx = Var("userFloat('vtx_czx')", float, doc = "error zx of fitted vertex", precision=-1),
            vtx_czy = Var("userFloat('vtx_czy')", float, doc = "error zy of fitted vertex", precision=-1),
            fit_cos_theta_2D = Var("userFloat('fitted_cos_theta_2D')", float, doc = "cos 2D of fitted vertex wrt beamspot", precision=-1),
            # post-fit momentum
            fit_mass     = Var("userFloat('fitted_mass')", float, doc = "post-fit mass of the vertex",          precision=-1),
            fit_massErr  = Var("userFloat('massErr')",     float, doc = "post-fit mass error",                  precision=-1),
            fit_trk1_pt  = Var("userFloat('trk1_pt')",     float, doc = "post-fit pt of the leading (pt) track",     precision=-1),
            fit_trk1_p   = Var("userFloat('trk1_p')",      float, doc = "post-fit p of the leading (pt) track",      precision=-1),
            fit_trk1_eta = Var("userFloat('trk1_eta')",    float, doc = "post-fit eta of the leading (pt) track",    precision=-1),
            fit_trk1_phi = Var("userFloat('trk1_phi')",    float, doc = "post-fit phi of the leading (pt) track",    precision=-1),
            fit_trk2_pt  = Var("userFloat('trk2_pt')",     float, doc = "post-fit pt of the subleading (pt) track",  precision=-1),
            fit_trk2_eta = Var("userFloat('trk2_eta')",    float, doc = "post-fit eta of the subleading (pt) track", precision=-1),
            fit_trk2_phi = Var("userFloat('trk2_phi')",    float, doc = "post-fit phi of the subleading (pt) track", precision=-1),
            fit_trk2_p   = Var("userFloat('trk2_p')",      float, doc = "post-fit p of the subleading (pt) track",   precision=-1),
            fit_trk1_keyPacked = Var("userInt('trk1_keyPacked')", int, doc=""),
            fit_trk2_keyPacked = Var("userInt('trk2_keyPacked')", int, doc=""),
        )
    )

    process.LambdaToProtonPiTable = process.KshortToPiPiTable.clone(
        src = cms.InputTag('LambdaToProtonPi','SelectedV0Collection'),
        name = cms.string("Lambda"),
        doc = cms.string("Lambda Variable")
    )

    # process.bphNanoSimpleSequence = cms.Sequence(
    #     # process.muonBPH+
    #     # process.BPHMuonTable+
    #     # process.MuMu+
    #     # process.MuMuTable+
    #     # process.tracksBPH+
    #     # process.trackBPHTable+
    #     # process.DiTrack+
    #     # process.DiTrackTable+
    #     process.KshortToPiPi+
    #     process.LambdaToProtonPi+
    #     process.KshortToPiPiTable+
    #     process.LambdaToProtonPiTable
    # )
    # if hasattr(process,"NANOEDMAODoutput") or hasattr(process,"NANOAODoutput"):
    #     process.nanoSequence *= process.bphNanoSimpleSequence
    # elif hasattr(process,"NANOEDMAODSIMoutput") or hasattr(process,"NANOAODSIMoutput"):
    #     process.nanoSequenceMC *= process.bphNanoSimpleSequence

    process.bphNanoSimpleTask = cms.Task(
        process.KshortToPiPi,
        process.LambdaToProtonPi,
        process.KshortToPiPiTable,
        process.LambdaToProtonPiTable,
    )
    if hasattr(process,"NANOEDMAODoutput") or hasattr(process,"NANOAODoutput"):
        process.nanoTableTaskCommon.add(process.bphNanoSimpleTask)
    elif hasattr(process,"NANOEDMAODSIMoutput") or hasattr(process,"NANOAODSIMoutput"):
        process.nanoTableTaskCommon.add(process.bphNanoSimpleTask)

    process.finalPFCandidates.src += cms.VInputTag(
        cms.InputTag("KshortToPiPi","PackedPFCandidatePtrCollection"),
        cms.InputTag("LambdaToProtonPi","PackedPFCandidatePtrCollection"),
    )

    return process


def nanoAOD_customize_BToKMuMu_BToTrkTrkMuMu(process):
    # /store/data/Run2024I/ParkingDoubleMuonLowMass4/AOD/PromptReco-v2/000/386/694/00000/002f827d-6d7f-4e2e-a0e6-f92fb0996683.root
    #
    # Remove CHS jets
    #
    # del process.jetTask
    # del process.jetForMETTask
    # del process.jetTablesTask
    # print(process.jetTask)
    # print(process.jetForMETTask)
    # print(process.jetTablesTask)
    del process.corrT1METJetTable
    del process.jetTable

    del process.jetPuppiTable.variables.hcalDepth1Energy
    del process.jetPuppiTable.variables.hcalDepth2Energy
    del process.jetPuppiTable.variables.hcalDepth3Energy
    del process.jetPuppiTable.variables.hcalDepth4Energy
    del process.jetPuppiTable.variables.hcalDepth5Energy
    del process.jetPuppiTable.variables.hcalDepth6Energy
    del process.jetPuppiTable.variables.hcalDepth7Energy
    del process.jetPuppiTable.variables.ecalEnergyFromChgHad
    del process.jetPuppiTable.variables.hcalEnergyFromChgHad
    del process.jetPuppiTable.variables.hoEnergyFromChgHad
    del process.jetPuppiTable.variables.ecalEnergyRawFromChgHad
    del process.jetPuppiTable.variables.hcalEnergyRawFromChgHad
    del process.jetPuppiTable.variables.hcalDepth1EnergyRawFromChgHad
    del process.jetPuppiTable.variables.hcalDepth2EnergyRawFromChgHad
    del process.jetPuppiTable.variables.hcalDepth3EnergyRawFromChgHad
    del process.jetPuppiTable.variables.hcalDepth4EnergyRawFromChgHad
    del process.jetPuppiTable.variables.hcalDepth5EnergyRawFromChgHad
    del process.jetPuppiTable.variables.hcalDepth6EnergyRawFromChgHad
    del process.jetPuppiTable.variables.hcalDepth7EnergyRawFromChgHad
    del process.jetPuppiTable.variables.hcalEnergyFromNeuHad
    del process.jetPuppiTable.variables.hoEnergyFromNeuHad
    del process.jetPuppiTable.variables.hcalEnergyRawFromNeuHad
    del process.jetPuppiTable.variables.hcalDepth1EnergyRawFromNeuHad
    del process.jetPuppiTable.variables.hcalDepth2EnergyRawFromNeuHad
    del process.jetPuppiTable.variables.hcalDepth3EnergyRawFromNeuHad
    del process.jetPuppiTable.variables.hcalDepth4EnergyRawFromNeuHad
    del process.jetPuppiTable.variables.hcalDepth5EnergyRawFromNeuHad
    del process.jetPuppiTable.variables.hcalDepth6EnergyRawFromNeuHad
    del process.jetPuppiTable.variables.hcalDepth7EnergyRawFromNeuHad
    del process.jetPuppiTable.variables.ecalEnergyFromPhoton
    del process.jetPuppiTable.variables.ecalEnergyRawFromPhoton
    del process.jetPuppiTable.variables.ecalEnergyFromElectron
    del process.jetPuppiTable.variables.ecalEnergyRawFromElectron
    del process.jetPuppiTable.variables.ecalEnergyFromMuon
    del process.jetPuppiTable.variables.hcalEnergyFromMuon
    del process.jetPuppiTable.variables.hoEnergyFromMuon
    del process.jetPuppiTable.variables.ecalEnergyRawFromMuon
    del process.jetPuppiTable.variables.hcalEnergyRawFromMuon
    del process.jetPuppiTable.variables.hcalDepth1EnergyRawFromMuon
    del process.jetPuppiTable.variables.hcalDepth2EnergyRawFromMuon
    del process.jetPuppiTable.variables.hcalDepth3EnergyRawFromMuon
    del process.jetPuppiTable.variables.hcalDepth4EnergyRawFromMuon
    del process.jetPuppiTable.variables.hcalDepth5EnergyRawFromMuon
    del process.jetPuppiTable.variables.hcalDepth6EnergyRawFromMuon
    del process.jetPuppiTable.variables.hcalDepth7EnergyRawFromMuon

    #
    # Reinstate jet cuts
    #
    process.selectedPatJetsPuppi.cut = "pt > 10"# revert back to original mini cut
    process.finalJetsPuppi.cut = "pt > 15"# revert back to original nanoAOD cut

    #
    #
    #
    del process.finalJetsAK8PFConstituents
    del process.selectedFinalJetsAK8PFConstituents
    del process.finalJetsAK8ConstituentsTable
    del process.finalJetsAK4PFConstituents
    del process.selectedFinalJetsAK4PFConstituents
    del process.finalJetsAK4ConstituentsTable
    #
    del process.customTauConstituentsTable
    del process.customElectronPFCandsTable
    del process.customPhotonPFCandsTable
    del process.customMuonPFCandsTable
    del process.finalTausConstituents
    del process.finalElectronsPFCandsConstituents
    del process.finalPhotonsPFCandsConstituents
    del process.finalMuonsPFCandsConstituents

    Path=["HLT_DoubleMu4_3_LowMass"]

    # Takes slimmedMuons, apply basic preselection and trigger match. Devides muon in
    # SelectedMuons (trigger matched) and AllMuons collections
    process.muonBPH = cms.EDProducer("MuonTriggerSelector",
        muonCollection = cms.InputTag("slimmedMuons"),
        bits           = cms.InputTag("TriggerResults", "", "HLT"),
        prescales      = cms.InputTag("patTrigger"),
        objects        = cms.InputTag("slimmedPatTrigger"),
        maxdR_matching = cms.double(0.3), # For the output trigger matched collection
        muonSelection  = cms.string("pt > 2 && abs(eta) < 2.4"), # Preselection
        HLTPaths       = cms.vstring(Path)
    )

    from PhysicsTools.NanoAOD.simplePATMuonFlatTableProducer_cfi import simplePATMuonFlatTableProducer
    # Table containing triggering muons, to interface with signal reconstruction
    # Contains variable mostly used for signal reconstruction and analysis
    process.TrgMatchMuonTable = simplePATMuonFlatTableProducer.clone(
        src  = cms.InputTag("muonBPH:SelectedMuons"), # Could this be removed and simply select userInt('isTriggering')==1
        cut  = cms.string(""), # We should not filter on cross linked collections
        name = cms.string("TrgMatchMuon"),
        doc  = cms.string("slimmedMuons after basic selection and trigger match"),
        singleton = cms.bool(False), # the number of entries is variable
        extension = cms.bool(False), # this is the main table for the muons
        variables = cms.PSet(
            CandVars,
            ptErr   = Var("bestTrack().ptError()", float, doc="ptError of the muon track", precision=10),
            covQopQop = Var("bestTrack().covariance(0, 0)", float, doc="Cov of q/p with q/p", precision=10),
            covLamLam = Var("bestTrack().covariance(1, 1)", float, doc="Cov of lambda with lambda", precision=10),
            covPhiPhi = Var("bestTrack().covariance(2, 2)", float, doc="Cov of phi with phi", precision=10),
            covQopLam = Var("bestTrack().covariance(0, 1)", float, doc="Cov of q/p with lambda", precision=10),
            covQopPhi = Var("bestTrack().covariance(0, 2)", float, doc="Cov of q/p with phi", precision=10),
            covLamPhi = Var("bestTrack().covariance(1, 2)", float, doc="Cov of lambda with phi", precision=10),
            dz      = Var("dB('PVDZ')", float, doc="dz (with sign) wrt PV[0] [cm]", precision=10),
            dzErr   = Var("abs(edB('PVDZ'))", float, doc="dz uncertainty [cm]", precision=10),
            dxy     = Var("dB('PV2D')", float, doc="dxy (with sign) wrt PV[0] [cm]", precision=10),
            dxyErr  = Var("edB('PV2D')", float, doc="dxy uncertainty [cm]", precision=10),
            ip3d    = Var("abs(dB('PV3D'))", float, doc="3D impact parameter wrt PV[0] [cm]", precision=10),
            sip3d   = Var("abs(dB('PV3D')/edB('PV3D'))", float, doc="3D impact parameter significance wrt PV[0]", precision=10),
            pfRelIso03_all = Var("(pfIsolationR03().sumChargedHadronPt + max(pfIsolationR03().sumNeutralHadronEt + pfIsolationR03().sumPhotonEt - pfIsolationR03().sumPUPt/2,0.0))/pt", float, doc="PF relative isolation dR=0.3, total (deltaBeta corrections)", precision=10),
            pfRelIso04_all = Var("(pfIsolationR04().sumChargedHadronPt + max(pfIsolationR04().sumNeutralHadronEt + pfIsolationR04().sumPhotonEt - pfIsolationR04().sumPUPt/2,0.0))/pt", float, doc="PF relative isolation dR=0.4, total (deltaBeta corrections)", precision=10),
            isPFcand    = Var("isPFMuon", bool, doc="muon is PF candidate"),
            isGlobal    = Var("isGlobalMuon", bool, doc="muon is global muon"),
            isTracker   = Var("isTrackerMuon", bool, doc="muon is tracker muon"),
            looseId     = Var("passed('CutBasedIdLoose')", bool, doc="cut-based ID, medium WP"),
            mediumId    = Var("passed('CutBasedIdMedium')", bool, doc="cut-based ID, medium WP"),
            # tightId     = Var("passed('CutBasedIdTight')", bool, doc="cut-based ID, tight WP"),
            triggerIdLoose  = Var("passed('TriggerIdLoose')", bool, doc="TriggerIdLoose ID"),
            softId = Var("passed('SoftCutBasedId')",bool,doc="soft cut-based ID"),
            softMvaId = Var("passed('SoftMvaId')",bool,doc="soft MVA ID"),
            softMva = Var("softMvaValue()",float,doc="soft MVA ID score",precision=6),
            softMvaRun3 = Var("softMvaRun3Value()",float,doc="soft MVA Run3 ID score",precision=6),
            isTriggering    = Var("userInt('isTriggering')", bool, doc="flag the reco muon if matched to HLT object"),
            matched_dr      = Var("userFloat('trgDR')", float, doc="dr with the matched triggering muon", precision=10),
            matched_dpt     = Var("userFloat('trgDPT')", float, doc="dpt/pt with the matched triggering muon", precision=10), #comma
            # fired_HLT_DoubleMu4_3_LowMass = Var("userInt('HLT_DoubleMu4_3_LowMass')", int, doc="reco muon fired this trigger"),
            # fired_HLT_DoubleMu4_LowMass_Displaced = Var("userInt('HLT_DoubleMu4_LowMass_Displaced')", int, doc="reco muon fired this trigger")
        ),
    )

    process.MuMu = cms.EDProducer('DiMuonBuilder',
        src = cms.InputTag('muonBPH', 'SelectedMuons'),
        transientTracksSrc = cms.InputTag('muonBPH', 'SelectedTransientMuons'),
        lep1Selection = cms.string('pt > 4.0 && abs(eta) < 2.4 && isLooseMuon && isGlobalMuon'),
        lep2Selection = cms.string('pt > 3.0 && abs(eta) < 2.4 && isLooseMuon && isGlobalMuon'),
        beamSpot = cms.InputTag("offlineBeamSpot"),
        preVtxSelection  = cms.string('abs(userCand("l1").vz - userCand("l2").vz) <= 1.'
                                      '&& 0 < mass() && mass() < 15.0 '
                                      '&& charge() == 0'
                                      '&& userFloat("lep_deltaR") > 0.03'),
        postVtxSelection = cms.string('0 < userFloat("fitted_mass") && userFloat("fitted_mass") < 15.0'
                                      '&& userFloat("sv_prob") > 0.001')
    )

    process.MuMuTable = cms.EDProducer("SimpleCompositeCandidateFlatTableProducer",
        src = cms.InputTag("MuMu:SelectedDiLeptons"),
        cut = cms.string(""), #we should not filter on cross linked collections
        name = cms.string("MuMu"),
        doc  = cms.string("Dilepton collections"),
        singleton = cms.bool(False), # the number of entries is variable
        extension = cms.bool(False), # this is the main table for the muons
        variables = cms.PSet(CandVars,
            l1_idx = Var("userInt('l1_idx')", int, doc = "leading muon index to the BPH muon collection"),
            l2_idx = Var("userInt('l2_idx')", int, doc = "subleading muon index to the BPH muon collection"),
            fit_mass = Var("userFloat('fitted_mass')", float, doc="Fitted dilepton mass", precision=10),
            fit_massErr = Var("userFloat('fitted_massErr')", float, doc = "post-fit uncertainty of the mass of the B candidate", precision=10),
            svprob = Var("userFloat('sv_prob')", float, doc="Vtx fit probability", precision=10),
            l_xy     = Var("userFloat('l_xy')", float, doc = "post-fit vertex displacement on transverse plane wrt beamspot", precision=10),
            l_xy_unc = Var("userFloat('l_xy_unc')", float, doc = "post-fit vertex uncertainty of displacement on transverse plane wrt beamspot", precision=10),
            vtx_x =Var("userFloat('vtx_x')", float, doc="Vtx position in x", precision=10),
            vtx_y = Var("userFloat('vtx_y')", float, doc="Vtx position in y", precision=10),
            vtx_z = Var("userFloat('vtx_z')", float, doc="Vtx position in y", precision=10),
            cos2D     = Var("userFloat('cos_theta_2D')", float, doc = "cos 2D of pre-fit candidate wrt beamspot", precision=10),
            fit_cos2D = Var("userFloat('fitted_cos_theta_2D')", float, doc = "cos 2D of fitted vertex wrt beamspot", precision=10),

        )
    )



    process.trackBPHTable = cms.EDProducer("SimpleCompositeCandidateFlatTableProducer",
        src  = cms.InputTag("tracksBPH:SelectedTracks"),
        cut  = cms.string(""),
        name = cms.string("Track"),
        doc  = cms.string("track collection"),
        singleton = cms.bool(False),
        extension = cms.bool(False),
        variables = cms.PSet(
            CandVars,
            vx = Var("vx()", float, doc="x coordinate of of ref point [cm]", precision=6),
            vy = Var("vy()", float, doc="y coordinate of of ref point [cm]", precision=6),
            vz = Var("vz()", float, doc="z coordinate of of ref point [cm]", precision=6),
            # User variables defined in plugins/BPHTrackMerger.cc
            isPacked  = Var("userInt('isPacked')", bool, doc="track from packedCandidate collection"),
            isLostTrk = Var("userInt('isLostTrk')", bool, doc="track from lostTrack collection"),
            dz      = Var("userFloat('dz')", float, doc="dz signed wrt PV[0] [cm]", precision=6),
            dxy     = Var("userFloat('dxy')", float, doc="dxy (with sign) wrt PV associated with the track [cm]", precision=6),
            dzS     = Var("userFloat('dzS')", float, doc="dz/err (with sign) wrt PV[0] [cm]", precision=6),
            dxyS    = Var("userFloat('dxyS')", float, doc="dxy/err (with sign) wrt PV associated with the track [cm]", precision=6),
            DCASig  = Var("userFloat('DCASig')", float, doc="significance of xy-distance of closest approach wrt beamspot", precision=6),
            dzTrg   = Var("userFloat('dzTrg')", float, doc="dz from the corresponding trigger muon [cm]", precision=6),
            isMatchedToMuon = Var("userInt('isMatchedToMuon')", bool, doc="track was used to build a muon"),
            isMatchedToEle  = Var("userInt('isMatchedToEle')", bool, doc="track was used to build a PF ele"),
            nValidHits      = Var("userInt('nValidHits')", "uint8", doc="Number of valid hits"),
            # Covariance matrix elements for helix parameters for decay time uncertainty
            covQopQop = Var("userFloat('covQopQop')", float, doc="Cov. of q/p with q/p", precision=10),
            covQopLam = Var("userFloat('covQopLam')", float, doc="Cov. of q/p with lambda", precision=10),
            covQopPhi = Var("userFloat('covQopPhi')", float, doc="Cov. of q/p with phi", precision=10),
            covLamLam = Var("userFloat('covLamLam')", float, doc="Cov. of lambda with lambda", precision=10),
            covLamPhi = Var("userFloat('covLamPhi')", float, doc="Cov. of lambda with phi", precision=10),
            covPhiPhi = Var("userFloat('covPhiPhi')", float, doc="Cov. of phi with phi", precision=10),
            # Additional track parameters for tagging
            ptErr      = Var("userFloat('ptErr')", float, doc="Pt uncertainty", precision=6),
            normChi2   = Var("userFloat('normChi2')", float, doc="Track fit chi-squared divided by n.d.o.f.", precision=6),
            nValidPixelHits = Var("userInt('nValidPixelHits')", "uint8", doc="Number of pixel hits"),
            # TODO: check impact parameters
            ),
    )

    process.BToKmumu = cms.EDProducer('BToTrkLLBuilderV2',
        dileptons = cms.InputTag("MuMu:SelectedDiLeptons"),
        leptonTransientTracks = cms.InputTag('muonBPH', 'SelectedTransientMuons'),
        dileptonKinVtxs = cms.InputTag('MuMu:SelectedDiLeptonKinVtxs'),
        kaons = cms.InputTag('tracksBPH', 'SelectedTracks'),
        kaonsTransientTracks = cms.InputTag('tracksBPH', 'SelectedTransientTracks'),
        trackMass = cms.double(493.677),
        beamSpot = cms.InputTag("offlineBeamSpot"),
        PUtracks = cms.InputTag('tracksBPH', 'SelectedTracks'),
        packedPFCandidates = cms.InputTag("packedPFCandidates"),
        preVtxSelection  = cms.string('pt > 5.0'
                                      '&& 4.5 < mass && mass < 6.'
                                      '&& userFloat("min_dr") > 0.03'),
        postVtxSelection = cms.string('4.7 < userFloat("fitted_mass") && userFloat("fitted_mass") < 6.' 
                                      '&& userFloat("sv_prob") > 0.001 && -0.045<userFloat("k_svip2d") && userFloat("k_svip2d")<0.045'
                                      '&& userFloat("fitted_cos_theta_2D") > 0.9'),
        dileptonMassContraint = cms.bool(True)
    )
    process.BToKmumuTable = cms.EDProducer('SimpleCompositeCandidateFlatTableProducer',
        src       = cms.InputTag("BToKmumu"),
        cut       = cms.string(""),
        name      = cms.string("BToKMuMu"),
        doc       = cms.string("BToKMuMu Variable"),
        singleton = cms.bool(False),
        extension = cms.bool(False),
        variables = cms.PSet(
        # pre-fit quantities
        CandVars,
        l1_idx = Var("userInt('l1_idx')", int, doc = "leading muon index to the BPH muon collection"),
        l2_idx = Var("userInt('l2_idx')", int, doc = "subleading muon index to the BPH muon collection"),
        k_idx  = Var("userInt('trk_idx')", int, doc = "track index to the BPH track collection"),
        k_isPacked  = Var("userInt('trk_isPacked')", int, doc = "isFrom packedPFCandidates"),
        k_isLostTrk  = Var("userInt('trk_isLostTrk')", int, doc = "isFrom lostTrack"),
        k_keyPacked  = Var("userInt('trk_keyPacked')", int, doc = "key in packedPFCandidates"),
        minDR  = Var("userFloat('min_dr')", float, doc = "minimum DeltaR between the kaon and the two muons", precision=12),
        maxDR  = Var("userFloat('max_dr')", float, doc = "maximum DeltaR between the kaon and the two muons", precision=12),
        # fit and vtx info
        svprob   = Var("userFloat('sv_prob')", float, doc = "vertex probability of the B candidate", precision=12),
        l_xy     = Var("userFloat('l_xy')", float, doc = "post-fit vertex displacement on transverse plane wrt beamspot", precision=12),
        l_xy_unc = Var("userFloat('l_xy_unc')", float, doc = "post-fit vertex uncertainty of displacement on transverse plane wrt beamspot", precision=12),
        vtx_x   = Var("userFloat('vtx_x')", float, doc = "position x of fitted vertex", precision=12),
        vtx_y   = Var("userFloat('vtx_y')", float, doc = "position y of fitted vertex", precision=12),
        vtx_z   = Var("userFloat('vtx_z')", float, doc = "position z of fitted vertex", precision=12),
        vtx_cxx = Var("userFloat('vtx_cxx')", float, doc = "error x of fitted vertex", precision=12),
        vtx_cyy = Var("userFloat('vtx_cyy')", float, doc = "error y of fitted vertex", precision=12),
        vtx_czz = Var("userFloat('vtx_czz')", float, doc = "error z of fitted vertex", precision=12),
        vtx_cyx = Var("userFloat('vtx_cyx')", float, doc = "error yx of fitted vertex", precision=12),
        vtx_czx = Var("userFloat('vtx_czx')", float, doc = "error zx of fitted vertex", precision=12),
        vtx_czy = Var("userFloat('vtx_czy')", float, doc = "error zy of fitted vertex", precision=12),
        # Mll
        mll_fullfit = Var("userFloat('fitted_mll')", float, doc = "post-fit mass of the two muons", precision=12),
        # Cos(theta)
        cos2D     = Var("userFloat('cos_theta_2D')", float, doc = "cos 2D of pre-fit candidate wrt beamspot", precision=12),
        fit_cos2D = Var("userFloat('fitted_cos_theta_2D')", float, doc = "cos 2D of fitted vertex wrt beamspot", precision=10),
        # post-fit momentum
        fit_mass    = Var("userFloat('fitted_mass')", float, doc = "post-fit mass of the B candidate", precision=12),
        fit_massErr = Var("userFloat('fitted_massErr')", float, doc = "post-fit uncertainty of the mass of the B candidate", precision=12),
        fit_pt      = Var("userFloat('fitted_pt')", float, doc = "post-fit B pT", precision=12),
        fit_eta     = Var("userFloat('fitted_eta')", float, doc = "post-fit B eta", precision=12),
        fit_phi     = Var("userFloat('fitted_phi')", float, doc = "post-fit B phi", precision=12),
        fit_l1_pt   = Var("userFloat('fitted_l1_pt')", float, doc = "post-fit leading mu pT", precision=12),
        fit_l1_eta  = Var("userFloat('fitted_l1_eta')", float, doc = "post-fit leading mu eta", precision=12),
        fit_l1_phi  = Var("userFloat('fitted_l1_phi')", float, doc = "post-fit leading mu phi", precision=12),
        fit_l2_pt   = Var("userFloat('fitted_l2_pt')", float, doc = "post-fit subleading mu pT", precision=12),
        fit_l2_eta  = Var("userFloat('fitted_l2_eta')", float, doc = "post-fit subleading mu eta", precision=12),
        fit_l2_phi  = Var("userFloat('fitted_l2_phi')", float, doc = "post-fit subleading mu phi", precision=12),
        fit_k_pt    = Var("userFloat('fitted_trk_pt')", float, doc = "post-fit track pT", precision=12),
        fit_k_eta   = Var("userFloat('fitted_trk_eta')", float, doc = "post-fit track eta", precision=12),
        fit_k_phi   = Var("userFloat('fitted_trk_phi')", float, doc = "post-fit track phi", precision=12),
        k_svip2d    = Var("userFloat('k_svip2d')", float, doc = "2D IP of the track wrt the dimuon vertex", precision=12),
        k_svip2d_err = Var("userFloat('k_svip2d_err')", float, doc = "uncertainty of 2D IP of the track wrt the dimuon vertex", precision=12),
        l1_iso04    = Var("userFloat('l1_iso04')", float, doc = "leading mu isolation DR<0.4", precision=12),
        l2_iso04    = Var("userFloat('l2_iso04')", float, doc = "subleading mu isolation DR<0.4", precision=12),
        k_iso04     = Var("userFloat('trk_iso04')", float, doc = "track isolation DR<0.4", precision=12),
        constraint_sv_prob     = Var("userFloat('constraint_sv_prob')", float, doc = "B vertex probability after the dimuon mass constraint", precision=12),
        constraint_pt   = Var("userFloat('constraint_pt')", float, doc = "B pt after the dimuon mass constraint", precision=12),
        constraint_eta  = Var("userFloat('constraint_eta')", float, doc = "B eta after the dimuon mass constraint", precision=12),
        constraint_phi  = Var("userFloat('constraint_phi')", float, doc = "B phi after the dimuon mass constraint", precision=12),
        constraint_mass = Var("userFloat('constraint_mass')", float, doc = "B mass after the dimuon mass constraint", precision=12),
        constraint_massErr  = Var("userFloat('constraint_massErr')", float, doc = "mass uncertainty of the dimuon mass constraint", precision=12),
        constraint_mll  = Var("userFloat('constraint_mll')", float, doc = "dimuon mass after the dimuon mass constraint", precision=12),
        )
    )

    #
    #
    #
    process.DiTrack = cms.EDProducer('DiTrackBuilderV2',
        tracks = cms.InputTag('tracksBPH', 'SelectedTracks'),
        transientTracks = cms.InputTag('tracksBPH', 'SelectedTransientTracks'),
        trk1Selection   = cms.string(''),
        trk2Selection   = cms.string(''),
        trk1Mass = cms.double(0.139),
        trk2Mass = cms.double(0.494),
        packedPFCandidates = cms.InputTag("packedPFCandidates"),
        beamSpot = cms.InputTag("offlineBeamSpot"),
        preVtxSelection = cms.string('((userFloat("unfitted_mass_KK")>0.95 && userFloat("unfitted_mass_KK")<1.12) || (userFloat("unfitted_mass_Kpi")>0.6 && userFloat("unfitted_mass_Kpi")<1.2) || (userFloat("unfitted_mass_piK")>0.6 && userFloat("unfitted_mass_piK")<1.2)) && charge() == 0'),
        postVtxSelection =  cms.string('((userFloat("fitted_mass_KK")>0.95 && userFloat("fitted_mass_KK")<1.12) || (userFloat("fitted_mass_Kpi")>0.6 && userFloat("fitted_mass_Kpi")<1.2)  || (userFloat("fitted_mass_piK")>0.6 && userFloat("fitted_mass_piK")<1.2)) && userFloat("sv_prob") > 0.001')
    )
    process.DiTrackTable = cms.EDProducer("SimpleCompositeCandidateFlatTableProducer",
        src  = cms.InputTag("BToTrkTrkMuMu:SelectedTrkTrk"),
        cut  = cms.string(""), #we should not filter on cross linked collections
        name = cms.string("DiTrack"),
        doc  = cms.string("slimmedDiTrack for BPark after basic selection"),
        singleton = cms.bool(False), # the number of entries is variable
        extension = cms.bool(False), # this is the main table for the muons
        variables = cms.PSet(
            CandVars,
            fit_mass_KK = Var("userFloat('fitted_mass_KK')", float, doc="", precision=10),
            fit_mass_Kpi = Var("userFloat('fitted_mass_Kpi')", float, doc="", precision=10),
            fit_mass_piK = Var("userFloat('fitted_mass_piK')", float, doc="", precision=10),
            fit_pt   = Var("userFloat('fitted_pt')", float, doc="", precision=10),
            fit_eta  = Var("userFloat('fitted_eta')", float, doc="", precision=10),
            fit_phi  = Var("userFloat('fitted_phi')", float, doc="", precision=10),
            svprob      = Var("userFloat('sv_prob')", float, doc="", precision=10),
            trk1_idx    = Var("userInt('trk1_idx')", int, doc=""),
            trk2_idx    = Var("userInt('trk2_idx')", int, doc=""),
            vtx_x       = Var("userFloat('vtx_x')", float, doc="", precision=10),
            vtx_y       = Var("userFloat('vtx_y')", float, doc="", precision=10),
            vtx_z       = Var("userFloat('vtx_z')", float, doc="", precision=10),
            l_xy        = Var("userFloat('l_xy')", float, doc="", precision=10),
            l_xy_unc        = Var("userFloat('l_xy_unc')", float, doc="", precision=10),
            cos_theta_2D    = Var("userFloat('fitted_cos_theta_2D')", float, doc="", precision=10),
            sv_prob         = Var("userFloat('sv_prob')", float, doc="", precision=10),
            sv_ndof         = Var("userFloat('sv_ndof')", float, doc="", precision=10),
            sv_chi2         = Var("userFloat('sv_chi2')", float, doc="", precision=10),
            vtx_cxx = Var("userFloat('vtx_cxx')", float, doc="", precision=10),
            vtx_cyy = Var("userFloat('vtx_cyy')", float, doc="", precision=10),
            vtx_czz = Var("userFloat('vtx_czz')", float, doc="", precision=10),
            vtx_cyx = Var("userFloat('vtx_cyx')", float, doc="", precision=10),
            vtx_czx = Var("userFloat('vtx_czx')", float, doc="", precision=10),
            vtx_czy = Var("userFloat('vtx_czy')", float, doc="", precision=10)
        )
    )
    process.BToTrkTrkMuMu = cms.EDProducer('BToTrkTrkLLBuilderV2',
        dileptons = cms.InputTag("MuMu:SelectedDiLeptons"),
        leptonTransientTracks = cms.InputTag('muonBPH', 'SelectedTransientMuons'),
        ditracks = cms.InputTag('DiTrack'),
        transientTracks = cms.InputTag('tracksBPH', 'SelectedTransientTracks'),
        PUtracks = cms.InputTag('tracksBPH', 'SelectedTracks'),
        packedPFCandidates = cms.InputTag("packedPFCandidates"),
        beamSpot = cms.InputTag("offlineBeamSpot"),
        preVtxSelection = cms.string('userFloat("min_dr") > 0.03 && ((4.5<userFloat("unfitted_B_mass_KK") && userFloat("unfitted_B_mass_KK")<6.0 ) || (4.5<userFloat("unfitted_B_mass_Kpi") && userFloat("unfitted_B_mass_Kpi")<6.0 ) || (4.5<userFloat("unfitted_B_mass_piK") && userFloat("unfitted_B_mass_piK")<6.0))'),
        postVtxSelection = cms.string('userFloat("sv_prob") > 1.e-3 && userFloat("fitted_cos_theta_2D") >= 0.90 && ( (4.6<userFloat("fitted_mass_KK") && userFloat("fitted_mass_KK")<5.8) || (4.6<userFloat("fitted_mass_Kpi") && userFloat("fitted_mass_Kpi")<5.8 ) || (4.6<userFloat("fitted_mass_piK") && userFloat("fitted_mass_piK")<5.8)) && -0.045<userFloat("trk1_svip2d") && userFloat("trk1_svip2d")<0.045 && -0.045<userFloat("trk2_svip2d") && userFloat("trk2_svip2d")<0.045'),
        dileptonMassContraint = cms.bool(True)
    )

    process.BToTrkTrkMuMuTable = cms.EDProducer(
        'SimpleCompositeCandidateFlatTableProducer',
        src       = cms.InputTag("BToTrkTrkMuMu:SelectedBToTrkTrkMuMu"),
        cut       = cms.string(""),
        name      = cms.string("BToTrkTrkMuMu"),
        doc       = cms.string("BToTrkTrkMuMu Variables"),
        singleton = cms.bool(False),
        extension = cms.bool(False),
        variables = cms.PSet(
            # pre-fit quantities
            CandVars,
            l1_idx      = Var("userInt('l1_idx')", int, doc = "leading muon index to the BPH muon collection"),
            l2_idx      = Var("userInt('l2_idx')", int, doc = "subleading muon index to the BPH muon collection"),
            trk1_idx    = Var("userInt('trk1_idx')", int, doc = "leading track index to the BPH track collection"),
            trk2_idx    = Var("userInt('trk2_idx')", int, doc = "subleading track index to the BPH track collection"),
            trk1_isPacked  = Var("userInt('trk1_isPacked')",  int, doc = "leading track is from packedPFCandidates"),
            trk2_isPacked  = Var("userInt('trk2_isPacked')",  int, doc = "subleading track is from packedPFCandidates"),
            trk1_isLostTrk = Var("userInt('trk1_isLostTrk')", int, doc = "leading track is from lostTracks"),
            trk2_isLostTrk = Var("userInt('trk2_isLostTrk')", int, doc = "subleading track is from lostTracks"),
            trk1_keyPacked = Var("userInt('trk1_keyPacked')", int, doc = "leading track packed key"),
            trk2_keyPacked = Var("userInt('trk2_keyPacked')", int, doc = "subleading track packed key"),
            ditrack_idx = Var("userInt('ditrack_idx')", int, doc = "Ditrack pair index to the BPH DiTrack collection"),
            fit_mass_KK = Var("userFloat('fitted_mass_KK')", float, doc = "mass of the B candidate for the leading trk->Kaon subleading trk->kaon mass hypothesis", precision=10),
            fit_mass_Kpi = Var("userFloat('fitted_mass_Kpi')", float, doc = "mass of the B candidate for the leading trk->Kaon subleading trk->pion mass hypothesis", precision=10),
            fit_mass_piK = Var("userFloat('fitted_mass_piK')", float, doc = "mass of the B candidate for the leading trk->pion subleading trk->kaon mass hypothesis", precision=10),
            fit_massErr_KK = Var("userFloat('fitted_massErr_KK')", float, doc = "uncertainty of the mass of the B candidate for the leading trk->Kaon subleading trk->pion mass hypothesis", precision=10),
            fit_massErr_Kpi = Var("userFloat('fitted_massErr_Kpi')", float, doc = "uncertainty of the mass of the B candidate for the leading trk->Kaon subleading trk->pion mass hypothesis", precision=10),
            fit_massErr_piK = Var("userFloat('fitted_massErr_piK')", float, doc = "uncertainty mass of the B candidate for the leading trk->Kaon subleading trk->pion mass hypothesis", precision=10),
            fit_ditrack_mass_KK = Var("userFloat('fitted_ditrack_mass_KK')", float, doc = "mass of the two track system for the leading trk->Kaon subleading trk->kaon mass hypothesis", precision=10),
            fit_ditrack_mass_Kpi = Var("userFloat('fitted_ditrack_mass_Kpi')", float, doc = "mass of the two track system for the leading trk->Kaon subleading trk->pion mass hypothesis", precision=10),
            fit_ditrack_mass_piK = Var("userFloat('fitted_ditrack_mass_piK')", float, doc = "mass of the two track system for the leading trk->pion subleading trk->kaon mass hypothesis", precision=10),
            min_dr      = Var("userFloat('min_dr')", float, doc = "min DR of the two tracks with the two muons", precision=10),
            max_dr      = Var("userFloat('max_dr')", float, doc = "max DR of the two tracks with the two muons", precision=10),
            # vtx info
            chi2      = Var("userFloat('sv_chi2')", float, doc = "vertex chi^2 of the B candidate", precision=10),
            svprob    = Var("userFloat('sv_prob')", float, doc = "vertex probability of the B candidate", precision=10),
            cos2D     = Var("userFloat('cos_theta_2D')", float, doc = "cos 2D of pre-fit candidate wrt beamspot", precision=10),
            fit_cos2D = Var("userFloat('fitted_cos_theta_2D')", float, doc = "cos 2D of fitted vertex wrt beamspot", precision=10),
            l_xy      = Var("userFloat('l_xy')", float, doc = "post-fit vertex displacement on transverse plane wrt beamspot", precision=10),
            l_xy_unc  = Var("userFloat('l_xy_unc')", float, doc = "post-fit uncertainty of the vertex displacement on transverse plane wrt beamspot", precision=10),
            # post-fit momentum /masses
            mll_fullfit    = Var("userFloat('fitted_mll')", float, doc = "post-fit mass of the two muons", precision=10),
            fit_pt         = Var("userFloat('fitted_pt')", float, doc = "post-fit B pT", precision=10),
            fit_eta        = Var("userFloat('fitted_eta')", float, doc = "post-fit B eta", precision=10),
            fit_phi        = Var("userFloat('fitted_phi')", float, doc = "post-fit B phi", precision=10),
            # vertex
            vtx_x   = Var("userFloat('vtx_x')", float, doc = "position x of fitted vertex", precision=10),
            vtx_y   = Var("userFloat('vtx_y')", float, doc = "position y of fitted vertex", precision=10),
            vtx_z   = Var("userFloat('vtx_z')", float, doc = "position z of fitted vertex", precision=10),
            vtx_cxx = Var("userFloat('vtx_cxx')", float, doc = "error x of fitted vertex", precision=10),
            vtx_cyy = Var("userFloat('vtx_cyy')", float, doc = "error y of fitted vertex", precision=10),
            vtx_czz = Var("userFloat('vtx_czz')", float, doc = "error z of fitted vertex", precision=10),
            vtx_cyx = Var("userFloat('vtx_cyx')", float, doc = "error yx of fitted vertex", precision=10),
            vtx_czx = Var("userFloat('vtx_czx')", float, doc = "error zx of fitted vertex", precision=10),
            vtx_czy = Var("userFloat('vtx_czy')", float, doc = "error zy of fitted vertex", precision=10),
            # post-fit tracks/leptons
            #l1
            fit_l1_pt  = Var("userFloat('fitted_l1_pt')", float, doc = "post-fit leading mu pT", precision=10),
            fit_l1_eta = Var("userFloat('fitted_l1_eta')", float, doc = "post-fit leading mu eta", precision=10),
            fit_l1_phi = Var("userFloat('fitted_l1_phi')", float, doc = "post-fit leading mu phi", precision=10),
            #l2
            fit_l2_pt  = Var("userFloat('fitted_l2_pt')", float, doc = "post-fit subleading mu pT", precision=10),
            fit_l2_eta = Var("userFloat('fitted_l2_eta')", float, doc = "post-fit subleading mu eta", precision=10),
            fit_l2_phi = Var("userFloat('fitted_l2_phi')", float, doc = "post-fit subleading mu phi", precision=10),
            #trk1
            fit_trk1_pt  = Var("userFloat('fitted_trk1_pt')", float, doc = "post-fit leading track pT", precision=10),
            fit_trk1_eta = Var("userFloat('fitted_trk1_eta')", float, doc = "post-fit leading track eta", precision=10),
            fit_trk1_phi = Var("userFloat('fitted_trk1_phi')", float, doc = "post-fit leading track phi", precision=10),
            #trk2
            fit_trk2_pt  = Var("userFloat('fitted_trk2_pt')", float, doc = "post-fit subleading track pT", precision=10),
            fit_trk2_eta = Var("userFloat('fitted_trk2_eta')", float, doc = "post-fit subleading track eta", precision=10),
            fit_trk2_phi = Var("userFloat('fitted_trk2_phi')", float, doc = "post-fit subleading track phi", precision=10),
            # isolation
            l1_iso04   = Var("userFloat('l1_iso04')", float, doc = "leading mu isolation DR<0.4", precision=10),
            l2_iso04   = Var("userFloat('l2_iso04')", float, doc = "subleading mu isolation DR<0.4", precision=10),
            trk1_iso04 = Var("userFloat('trk1_iso04')", float, doc = "leading track isolation DR<0.4", precision=10),
            trk2_iso04 = Var("userFloat('trk2_iso04')", float, doc = "subleading track isolation DR<0.4", precision=10),
            trk1_svip2d     = Var("userFloat('trk1_svip2d')", float, doc = "2D IP of the leading track wrt the dimuon vertex", precision=10),
            trk1_svip2d_err = Var("userFloat('trk1_svip2d_err')", float, doc = "uncertainty of 2D IP of the leading track wrt the dimuon vertex", precision=10),
            trk2_svip2d     = Var("userFloat('trk2_svip2d')", float, doc = "2D IP of the subleading track wrt the dimuon vertex", precision=10),
            trk2_svip2d_err = Var("userFloat('trk2_svip2d_err')", float, doc = "uncertainty of 2D IP of the subleading track wrt the dimuon vertex", precision=10),
            constraint_sv_prob     = Var("userFloat('constraint_sv_prob')", float, doc = "B vertex probability after the dimuon mass constraint", precision=10),
            constraint_pt   = Var("userFloat('constraint_pt')", float, doc = "B pt after the dimuon mass constraint", precision=10),
            constraint_eta  = Var("userFloat('constraint_eta')", float, doc = "B eta after the dimuon mass constraint", precision=10),
            constraint_phi  = Var("userFloat('constraint_phi')", float, doc = "B phi after the dimuon mass constraint", precision=10),
            constraint_mass_KK = Var("userFloat('constraint_mass_KK')", float, doc = "B mass with KK mass hypothesis after the dimuon mass constraint", precision=10),
            constraint_massErr_KK  = Var("userFloat('constraint_massErr_KK')", float, doc = "mass uncertainty for the KK mass hypothesis of the dimuon mass constraint", precision=10),
            constraint_mass_Kpi = Var("userFloat('constraint_mass_Kpi')", float, doc = "B mass with Kpi mass hypothesis after the dimuon mass constraint", precision=10),
            constraint_massErr_Kpi  = Var("userFloat('constraint_massErr_Kpi')", float, doc = "mass uncertainty for the Kpi mass hypothesis of the dimuon mass constraint", precision=10),
            constraint_mass_piK = Var("userFloat('constraint_mass_piK')", float, doc = "B mass with piK mass hypothesis after the dimuon mass constraint", precision=10),
            constraint_massErr_piK  = Var("userFloat('constraint_massErr_piK')", float, doc = "mass uncertainty for the piK mass hypothesis of the dimuon mass constraint", precision=10),
            constraint_mll  = Var("userFloat('constraint_mll')", float, doc = "dimuon mass after the dimuon mass constraint", precision=10),
        )
    )

    process.XiToLambdaPion = cms.EDProducer('V0TrkDisplacedBuilder',
        V0s_ttracks = cms.InputTag('LambdaToProtonPi','SelectedV0TransientCollection'),
        V0s = cms.InputTag('LambdaToProtonPi','SelectedV0Collection'),
        packedPFCandidates = cms.InputTag("packedPFCandidates"),
        V0sOriginal = cms.InputTag('slimmedLambdaVertices'),
        beamSpot = cms.InputTag("offlineBeamSpot"),
        offlinePrimaryVertexSrc = cms.InputTag('offlineSlimmedPrimaryVertices'),
        isXi = cms.bool(True),
        preVtxSelection = cms.string(
            'userFloat("V0_trk_min_dr")>0.03 && '
            'abs(charge)==1 && '
            '1.2 < mass && mass < 1.5 &&'
            'userFloat("V0trk_dr")<1.6'
        ),
        postVtxSelection = cms.string(
            'userFloat("sv_prob") > 1.e-3 '
            ' && userFloat("fitted_cos_theta_2D") >= 0.90 && '
           ' (1.240 < userFloat("fitted_mass") && userFloat("fitted_mass") < 1.450 )'
        ),
        trkSelection  = cms.string("pt>0.7 && abs(eta)<3.0"),
    )

    process.XiToLambdaPionTable = cms.EDProducer(
        'SimpleCompositeCandidateFlatTableProducer',
        src       = cms.InputTag("XiToLambdaPion","SelectedV0TrkDisplaced"),
        cut       = cms.string(""),
        name      = cms.string("XiToLambdaPion"),
        doc       = cms.string("XiToLambdaPion Variables"),
        singleton = cms.bool(False),
        extension = cms.bool(False),
        variables = cms.PSet(
            # pre-fit quantities
            CandVars,
            V0_idx            = Var("userInt('V0_idx')", int, doc = "V0 index in the LambdaToProtonPi collection"),
            V0_pt             = Var("userFloat('V0_pt')",float, doc = "V0 track pt"),
            V0_eta            = Var("userFloat('V0_eta')",float, doc = "V0 track eta"),
            V0_phi            = Var("userFloat('V0_phi')",float, doc = "V0 track phi"),
            V0_p              = Var("userFloat('V0_mass')",float, doc = "V0 mass"),
            V0_prefit_mass    = Var("userFloat('V0_prefit_mass')", float, doc = "V0 prefit mass"),
            V0_fittedmass     = Var("userFloat('V0_fittedmass')", float, doc = "V0 fittedmass"),
            V0_trk1_pt        = Var("userFloat('V0_trk1_pt')",  float, doc = "leading V0 track pt"),
            V0_trk1_eta       = Var("userFloat('V0_trk1_eta')", float, doc = "leading V0 track eta"),
            V0_trk1_phi       = Var("userFloat('V0_trk1_phi')", float, doc = "leading V0 track phi"),
            V0_trk1_p         = Var("userFloat('V0_trk1_p')",   float, doc = "leading V0 track momentum"),
            V0_trk1_keyPacked = Var("userInt('V0_trk1_keyPacked')", int, doc = "leading V0 track packed key"),
            V0_trk2_pt        = Var("userFloat('V0_trk2_pt')",  float, doc = "subleading V0 track pt"),
            V0_trk2_eta       = Var("userFloat('V0_trk2_eta')", float, doc = "subleading V0 track eta"),
            V0_trk2_phi       = Var("userFloat('V0_trk2_phi')", float, doc = "subleading V0 track phi"),
            V0_trk2_p         = Var("userFloat('V0_trk2_p')",   float, doc = "subleading V0 track momentum"),
            V0_trk2_keyPacked = Var("userInt('V0_trk2_keyPacked')", int, doc = "leading V0 track packed key"),
            trk_keyPacked           = Var("userInt('trk_keyPacked')", int, doc = "track index to the BPH track collection"),
            trk_pt            = Var("userFloat('prefit_trk_pt')",  float, doc = " track pt"),
            trk_eta           = Var("userFloat('prefit_trk_eta')", float, doc = " track eta"),
            trk_phi           = Var("userFloat('prefit_trk_phi')", float, doc = " track phi"),
            trk_mass          = Var("userFloat('prefit_trk_mass')", float, doc = "track mass"),
            V0_trk_min_dr     = Var("userFloat('V0_trk_min_dr')", float, doc = "min DR of the track with V0", precision=10),
            V0_trk_max_dr     = Var("userFloat('V0_trk_max_dr')", float, doc = "max DR of the track with V0", precision=10),
            fitted_trk_pt     = Var("userFloat('fitted_trk_pt')",   float, doc = "fitted_trk_pt"),
            fitted_trk_eta    = Var("userFloat('fitted_trk_eta')",  float, doc = "fitted_trk_eta"),
            fitted_trk_phi    = Var("userFloat('fitted_trk_phi')",  float, doc = "fitted_trk_phi"),
            fitted_trk_mass   = Var("userFloat('fitted_trk_mass')", float, doc = "fitted_trk_mass"),
            fitted_V0_pt      = Var("userFloat('fitted_V0_pt')",    float, doc = "fitted_V0_pt"),
            fitted_V0_eta     = Var("userFloat('fitted_V0_eta')",   float, doc = "fitted_V0_eta"),
            fitted_V0_phi     = Var("userFloat('fitted_V0_phi')",   float, doc = "fitted_V0_phi"),
            fitted_V0_mass    = Var("userFloat('fitted_V0_mass')",  float, doc = "fitted_V0_mass"),
            fitted_mass       = Var("userFloat('fitted_mass')",     float, doc = "fitted_mass"),
            fitted_massErr    = Var("userFloat('fitted_massErr')",  float, doc = "fitted_massErr"),
            # vtx info
            chi2      = Var("userFloat('sv_chi2')", float, doc = "vertex chi^2 of the lambda+track candidate", precision=10),
            ndof      = Var("userFloat('sv_ndof')", float, doc = "vertex ndof of the lambda+track candidate", precision=10),
            svprob    = Var("userFloat('sv_prob')", float, doc = "vertex probability of the lambda+track candidate", precision=10),
            fit_cos2D = Var("userFloat('fitted_cos_theta_2D')", float, doc = "cos 2D of fitted vertex wrt beamspot", precision=10),
            l_xy      = Var("userFloat('l_xy')", float, doc = "post-fit vertex displacement on transverse plane wrt beamspot", precision=10),
            l_xy_unc  = Var("userFloat('l_xy_unc')", float, doc = "post-fit uncertainty of the vertex displacement on transverse plane wrt beamspot", precision=10),
            # vertex
            vtx_x   = Var("userFloat('vtx_x')", float, doc = "position x of fitted vertex", precision=10),
            vtx_y   = Var("userFloat('vtx_y')", float, doc = "position y of fitted vertex", precision=10),
            vtx_z   = Var("userFloat('vtx_z')", float, doc = "position z of fitted vertex", precision=10),
            #
            dca       = Var("userFloat('dca')", float, doc = "dca",     precision=15),
            dca_err   = Var("userFloat('dcaErr')", float, doc = "dca_err", precision=15),
            V0_dz  = Var("userFloat('V0_dz')", float, doc = "V0_dz", precision=15),
            V0_x   = Var("userFloat('V0_x')", float,  doc = "V0_x",  precision=15),
            V0_y   = Var("userFloat('V0_y')", float,  doc = "V0_y",  precision=15),
            V0_z   = Var("userFloat('V0_z')", float,  doc = "V0_z",  precision=15),
            svip2d      = Var("userFloat('svip2d')", float,      doc = "svip2d",  precision=15),
            svip2d_err  = Var("userFloat('svip2d_err')", float,  doc = "svip2d_err",  precision=15),
            svip3d      = Var("userFloat('svip3d')", float,      doc = "svip3d",  precision=15),
            svip3d_err  = Var("userFloat('svip3d_err')", float,  doc = "svip3d_err",  precision=15),
        )
    )

    process.OmegaToLambdaKaon = cms.EDProducer('V0TrkDisplacedBuilder',
        V0s_ttracks = cms.InputTag('LambdaToProtonPi','SelectedV0TransientCollection'),
        V0s = cms.InputTag('LambdaToProtonPi','SelectedV0Collection'),
        packedPFCandidates = cms.InputTag("packedPFCandidates"),
        V0sOriginal = cms.InputTag('slimmedLambdaVertices'),
        beamSpot = cms.InputTag("offlineBeamSpot"),
        offlinePrimaryVertexSrc = cms.InputTag('offlineSlimmedPrimaryVertices'),
        isXi = cms.bool(False),
        preVtxSelection = cms.string(
            'userFloat("V0_trk_min_dr")>0.03 && '
            'abs(charge)==1 && '
            '1.5 < mass && mass < 1.8 && '
            'userFloat("V0trk_dr")<1.6'
        ),
        postVtxSelection = cms.string(
            'userFloat("sv_prob") > 1.e-3 '
            ' && userFloat("fitted_cos_theta_2D") >= 0.90 && '
           ' (1.5 < userFloat("fitted_mass") && userFloat("fitted_mass") < 1.8 )'
        ),
        trkSelection  = cms.string("pt>0.7 && abs(eta)<3.0"),
    )
    process.OmegaToLambdaKaonTable = process.XiToLambdaPionTable.clone(
        src       = cms.InputTag("OmegaToLambdaKaon","SelectedV0TrkDisplaced"),
        cut       = cms.string(""),
        name      = cms.string("OmegaToLambdaKaon"),
        doc       = cms.string("OmegaToLambdaKaon Variables"),
    )

    process.pVertexTable = cms.EDProducer("PVertexBPHTable",
        pvSrc = cms.InputTag("offlineSlimmedPrimaryVertices"),
        goodPvCut = cms.string("!isFake && ndof > 4 && abs(z) <= 24 && position.Rho <= 2"),
        pvName = cms.string("PVtx")
    )

    process.BToKmumuTrackIsoCands = cms.EDProducer("PackedPFCandIsoProducer",
        packedPFCandidatesSelected = cms.InputTag("BToKmumu","PackedPFCandidatePtrCollection"),
        packedPFCandidates = cms.InputTag("packedPFCandidates"),
        maxDeltaR = cms.double(0.2)
    )

    process.BToTrkTrkMuMuTrackIsoCands = cms.EDProducer("PackedPFCandIsoProducer",
        packedPFCandidatesSelected = cms.InputTag("BToTrkTrkMuMu","PackedPFCandidatePtrCollection"),
        packedPFCandidates = cms.InputTag("packedPFCandidates"),
        maxDeltaR = cms.double(0.2)
    )

    process.KShortToPiPiTrackIsoCands = cms.EDProducer("PackedPFCandIsoProducer",
        packedPFCandidatesSelected = cms.InputTag("KshortToPiPi","PackedPFCandidatePtrCollection"),
        packedPFCandidates = cms.InputTag("packedPFCandidates"),
        maxDeltaR = cms.double(0.2)
    )

    process.LambdaToProtonPiTrackIsoCands = cms.EDProducer("PackedPFCandIsoProducer",
        packedPFCandidatesSelected = cms.InputTag("LambdaToProtonPi","PackedPFCandidatePtrCollection"),
        packedPFCandidates = cms.InputTag("packedPFCandidates"),
        maxDeltaR = cms.double(0.2)
    )

    process.XiToLambdaPionTrackIsoCands = cms.EDProducer("PackedPFCandIsoProducer",
        packedPFCandidatesSelected = cms.InputTag("XiToLambdaPion","PackedPFCandidatePtrCollection"),
        packedPFCandidates = cms.InputTag("packedPFCandidates"),
        maxDeltaR = cms.double(0.2)
    )

    process.OmegaToLambdaKaonTrackIsoCands = cms.EDProducer("PackedPFCandIsoProducer",
        packedPFCandidatesSelected = cms.InputTag("OmegaToLambdaKaon","PackedPFCandidatePtrCollection"),
        packedPFCandidates = cms.InputTag("packedPFCandidates"),
        maxDeltaR = cms.double(0.2)
    )

    process.pfChargedHadronSelectedIsoCands.maxDeltaR=0.2

    #
    #
    #
    process.finalPFCandidates.src = cms.VInputTag(
        cms.InputTag("BToKmumu","PackedPFCandidatePtrCollection"),
        cms.InputTag("BToKmumuTrackIsoCands"),
        cms.InputTag("BToTrkTrkMuMu","PackedPFCandidatePtrCollection"),
        cms.InputTag("BToTrkTrkMuMuTrackIsoCands"),
        cms.InputTag("KshortToPiPi","PackedPFCandidatePtrCollection"),
        cms.InputTag("KShortToPiPiTrackIsoCands"),
        cms.InputTag("LambdaToProtonPi","PackedPFCandidatePtrCollection"),
        cms.InputTag("LambdaToProtonPiTrackIsoCands"),
        cms.InputTag("pfChargedHadronSelected"),
        cms.InputTag("pfChargedHadronSelectedIsoCands"),
        cms.InputTag("XiToLambdaPion","PackedPFCandidatePtrCollection"),
        cms.InputTag("XiToLambdaPionTrackIsoCands"),
        cms.InputTag("OmegaToLambdaKaon","PackedPFCandidatePtrCollection"),
        cms.InputTag("OmegaToLambdaKaonTrackIsoCands"),
    )

    process.bphNanoBDecayTask = cms.Task(
        process.muonBPH,
        process.TrgMatchMuonTable,
        process.MuMu,
        process.MuMuTable,
        process.tracksBPH,
        # process.trackBPHTable,
        process.BToKmumu,
        process.BToKmumuTable,
        process.DiTrack,
        # process.DiTrackTable,
        process.BToTrkTrkMuMu,
        process.BToTrkTrkMuMuTable,
        process.BToKmumuTrackIsoCands,
        process.BToTrkTrkMuMuTrackIsoCands,
        #
        process.XiToLambdaPion,
        process.XiToLambdaPionTable,
        process.OmegaToLambdaKaon,
        process.OmegaToLambdaKaonTable,
        #
        process.pVertexTable,
    )

    process.bphNanoSimpleTask.add(process.KShortToPiPiTrackIsoCands)
    process.bphNanoSimpleTask.add(process.LambdaToProtonPiTrackIsoCands)
    process.bphNanoSimpleTask.add(process.XiToLambdaPionTrackIsoCands)
    process.bphNanoSimpleTask.add(process.OmegaToLambdaKaonTrackIsoCands)

    if hasattr(process,"NANOEDMAODoutput") or hasattr(process,"NANOAODoutput"):
        process.nanoTableTaskCommon.add(process.bphNanoBDecayTask)
    elif hasattr(process,"NANOEDMAODSIMoutput") or hasattr(process,"NANOAODSIMoutput"):
        process.nanoTableTaskCommon.add(process.bphNanoBDecayTask)

    # process.CountBToKmumu = cms.EDFilter("PATCandViewCountFilter",
    #     minNumber = cms.uint32(1),
    #     maxNumber = cms.uint32(999999),
    #     src       = cms.InputTag("BToKmumu")
    # )
    # process.CountBToTrkTrkMuMu = cms.EDFilter("PATCandViewCountFilter",
    #     minNumber = cms.uint32(1),
    #     maxNumber = cms.uint32(999999),
    #     src       = cms.InputTag("BToTrkTrkMuMu")
    # )

    return process

