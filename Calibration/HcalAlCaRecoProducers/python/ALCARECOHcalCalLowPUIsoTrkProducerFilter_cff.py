import FWCore.ParameterSet.Config as cms

#------------------------------------------------
#AlCaReco filtering for HCAL LowPU isotrk
#------------------------------------------------

from Calibration.HcalAlCaRecoProducers.alcaHcalIsotrkProducer_cff import alcaHcalIsotrkProducer as _alcaHcalIsotrkProducer
from Calibration.HcalAlCaRecoProducers.alcaHcalIsotrkFilter_cfi import alcaHcalIsotrkFilter as _alcaHcalIsotrkFilter

alcaHcalLowPUIsotrkProducer = _alcaHcalIsotrkProducer.clone(
  momentumLow = 20,
  momentumHigh = 100,
)

alcaHcalLowPUIsotrkFilter = _alcaHcalIsotrkFilter.clone(
  isoTrackLabel = ('alcaHcalLowPUIsotrkProducer', 'HcalIsoTrack'),
)

from RecoLocalCalo.EcalRecAlgos.EcalSeverityLevelESProducer_cfi import *

seqALCARECOHcalCalLowPUIsoTrkProducerFilter = cms.Sequence(alcaHcalLowPUIsotrkProducer * alcaHcalLowPUIsotrkFilter)
