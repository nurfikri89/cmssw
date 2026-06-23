import FWCore.ParameterSet.Config as cms

particleFlowRecHitHBHEAbsIEta1To26 = cms.EDProducer("PFRecHitHBHEFilter",
    src = cms.InputTag("particleFlowRecHitHBHE"),

    minEnergy = cms.double(0.0),
    useAbsEnergy = cms.bool(False),

    filterLayer = cms.bool(False),
    requiredLayer = cms.int32(5),

    filterTime = cms.bool(False),
    minTime = cms.double(-9999.0),
    maxTime = cms.double( 9999.0),

    filterIeta = cms.bool(True),
    minAbsIeta = cms.int32(1),
    maxAbsIeta = cms.int32(26),

    filterIphi = cms.bool(False),
    allowedIphi = cms.vint32(),

    filterDepth = cms.bool(False),
    allowedDepth = cms.vint32(),

    verbose = cms.bool(False),
)

particleFlowRecHitHBHEAbsIEta27To29 = particleFlowRecHitHBHEAbsIEta1To26.clone(
    minAbsIeta = 27,
    maxAbsIeta = 29,
)
