#Info about branch

This branch is made specifically to produce custom NanoAODv14 for validation of UParT v2.
It's based from `CMSSW_14_0_19_patch2` and then added UParT v2 changes from Alexandre de Moor's 
PR (https://github.com/cms-sw/cmssw/pull/47173) through `git-cms-cherry-pick-pr 47173`.
The branch cannot be used as is as you need the latest UParT model so must checkout 
from Alexandre's cms-data fork. You can do this by doing the following starting from 
your CMSSW release workspace src directory (`${CMSSW_BASE}/src`),

```bash
git cms-addpkg RecoBTag/Combined
cd RecoBTag/Combined
git@github.com:AlexDeMoor/RecoBTag-Combined.git data
```
