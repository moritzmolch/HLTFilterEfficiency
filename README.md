# TauTriggerNtuples

Tools for studying the efficiency of single filters within a HLT path.
 

## Installation

First, ``CMSSW``  has to be installed. The recommended release is ``CMSSW_10_6_28``, which is the version that these tools have been tested on.

```bash
source /cvmfs/cms.cern.ch/cmsset_default.sh
scramv1 project CMSSW CMSSW_10_6_28
cd CMSSW_10_6_28/src
eval "$(scramv1 runtime -sh)"
git clone --recursive git@github.com:moritzmolch/TauTriggerNtuples.git TauAnalysis/TauTriggerNtuples
scramv1 build
scramv1 build python
```
