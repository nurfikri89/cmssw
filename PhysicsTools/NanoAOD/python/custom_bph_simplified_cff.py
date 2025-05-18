import FWCore.ParameterSet.Config as cms
from PhysicsTools.BPHNano.common_cff import *
from PhysicsTools.NanoAOD.simplePATMuonFlatTableProducer_cfi import simplePATMuonFlatTableProducer

def nanoAOD_customize_KshortLL_LambdaLL_PhiKK(process):
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
        process.LambdaToProtonPiTable
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
