import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Run3_cff import Run3
from Configuration.Eras.Modifier_run3_egamma_2023_cff import run3_egamma_2023
from Configuration.Eras.Modifier_run3_scouting_nanoAOD_2023_cff import run3_scouting_nanoAOD_2023
from Configuration.Eras.Modifier_run3_repuppi_2022_cff import run3_repuppi_2022

Run3_2023 = cms.ModifierChain(Run3.copyAndExclude([run3_repuppi_2022]), run3_egamma_2023, run3_scouting_nanoAOD_2023)
