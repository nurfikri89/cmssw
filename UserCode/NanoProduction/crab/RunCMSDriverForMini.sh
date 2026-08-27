cmsDriver.py --python_filename MiniAODv4.py \
--eventcontent MINIAODSIM --customise Configuration/DataProcessing/Utils.addMonitoring \
--datatier MINIAODSIM \
--fileout file:MINIAODSIM.root \
--conditions 140X_mcRun3_2024_realistic_v26 \
--step PAT --geometry DB:Extended \
--filein file:root://eosuser.cern.ch//eos/user/n/nbinnorj/Samples/AOD/store/mc/RunIII2024Summer24DRPremix/TTto4Q_TuneCP5_13p6TeV_powheg-pythia8/AODSIM/140X_mcRun3_2024_realistic_v26-v2/100000/0038d723-82cb-40d5-9261-919e3ae91dc3.root \
--era Run3_2024 \
--no_exec \
--mc -n 50