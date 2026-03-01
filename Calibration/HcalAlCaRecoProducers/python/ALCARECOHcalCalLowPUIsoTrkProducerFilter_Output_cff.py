import FWCore.ParameterSet.Config as cms

# output block for alcastream HCAL LowPU Isotrk
# output module 
#  module alcastreamHcalIsotrkOutput = PoolOutputModule
OutALCARECOHcalCalLowPUIsoTrkProducerFilter_noDrop = cms.PSet(
    # use this in case of filter available
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOHcalCalLowPUIsoTrkProducerFilter')
        ),
    outputCommands = cms.untracked.vstring( 
        'keep *_alcaHcalLowPUIsotrkProducer_HcalIsoTrack_*',
        'keep *_alcaHcalLowPUIsotrkProducer_HcalIsoTrackEvent_*',
        'keep *_genParticles_*_*',
        )
)

import copy
OutALCARECOHcalCalLowPUIsoTrkProducerFilter=copy.deepcopy(OutALCARECOHcalCalLowPUIsoTrkProducerFilter_noDrop)
OutALCARECOHcalCalLowPUIsoTrkProducerFilter.outputCommands.insert(0, "drop *")
