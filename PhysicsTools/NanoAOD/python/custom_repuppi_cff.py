
from PhysicsTools.PatAlgos.tools.puppiJetMETReclusteringFromMiniAOD_cff import puppiJetMETReclusterFromMiniAOD
def nanoAOD_puppiRecluster(process, runOnMC, useExistingWeights=False):
    process = puppiJetMETReclusterFromMiniAOD(process, runOnMC, useExistingWeights)
    # Set the flag to recompute puppi weights to ensure that the PuppiProducers will do what we want
    # just in case they were overridden beforehand.
    process.packedpuppi.useExistingWeights = useExistingWeights
    process.packedpuppiNoLep.useExistingWeights = useExistingWeights
    return process

def nanoPuppiReclusterCustomize_MC(process):
    process = nanoAOD_puppiRecluster(process, runOnMC=True)
    return process

def nanoPuppiReclusterCustomize_Data(process):
    process = nanoAOD_puppiRecluster(process, runOnMC=False)
    return process

def nanoPuppiReclusterCustomize(process):
    if hasattr(process,"NANOEDMAODoutput") or hasattr(process,"NANOAODoutput"):
        process = nanoPuppiReclusterCustomize_Data(process)
    else:
        process = nanoPuppiReclusterCustomize_MC(process)
    return process
