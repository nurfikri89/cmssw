#include "CommonTools/UtilAlgos/interface/StringCutObjectSelector.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

template<typename T>
class PatObjectPFCandSelector : public edm::stream::EDProducer<> {

public:
  PatObjectPFCandSelector(edm::ParameterSet const& params)
      : srcToken_{consumes<edm::View<T>>(params.getParameter<edm::InputTag>("src"))},
        srcPackedPFCandsToken_{consumes<std::vector<pat::PackedCandidate>>(params.getParameter<edm::InputTag>("srcPackedPFCands"))},
        selector_{params.getParameter<std::string>("cut")}{
    produces<std::vector<T>>();
    produces<std::vector<edm::Ptr<pat::PackedCandidate>>>("constituents");
  }

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("src", edm::InputTag(""))->setComment("InputTag used for retrieving pat object in event.");
    desc.add<edm::InputTag>("srcPackedPFCands", edm::InputTag("packedPFCandidates"));
    desc.add<std::string>("cut", "")->setComment("Cut used by which to select pat object.");
    descriptions.addWithDefaultLabel(desc);
  }

  edm::Ptr<pat::PackedCandidate> const initptr(edm::Ptr<reco::Candidate> const& dau) const {
    edm::Ptr<pat::PackedCandidate> retval(dau);
    return retval;
  }

  void produce(edm::Event& iEvent, edm::EventSetup const& iSetup) override {
    auto patobjsOut = std::make_unique<std::vector<T>>();
    auto candsOut = std::make_unique<std::vector<edm::Ptr<pat::PackedCandidate>>>();

    edm::Handle<std::vector<pat::PackedCandidate>> h_packedPFCands;
    iEvent.getByToken(srcPackedPFCandsToken_, h_packedPFCands);

    edm::Handle<edm::View<T>> h_patObjs;
    iEvent.getByToken(srcToken_, h_patObjs);

    // Now set the Ptrs with the orphan handles.
    for (auto const& patObj : *h_patObjs) {
      // Check the selection
      if (selector_(patObj)){
        //
        //
        //
        patobjsOut->push_back(patObj);
        for(size_t ipf=0; ipf < patObj.numberOfSourceCandidatePtrs(); ipf++){
          // NOTE: Why not take directly from sourceCandidatePtr()? Answer: Seems to not work for some patobjects.
          candsOut->emplace_back(edm::Ptr<pat::PackedCandidate>(h_packedPFCands,patObj.sourceCandidatePtr(ipf).key()));
        }
        // if constexpr (std::is_same<T,pat::Electron>::value || std::is_same<T,pat::Photon>::value){
        //   for (const auto &pf : patObj.associatedPackedPFCandidates()) {
        //     candsOut.push_back(pf);
        //   }
        // }
      }
    }
    iEvent.put(std::move(patobjsOut));
    iEvent.put(std::move(candsOut), "constituents");
  }

private:
  edm::EDGetTokenT<edm::View<T>> const srcToken_;
  edm::EDGetTokenT<std::vector<pat::PackedCandidate>> const srcPackedPFCandsToken_;
  StringCutObjectSelector<T> const selector_;

};

typedef PatObjectPFCandSelector<pat::Electron> PatElectronPFCandSelector;
typedef PatObjectPFCandSelector<pat::Photon> PatPhotonPFCandSelector;
typedef PatObjectPFCandSelector<pat::Muon> PatMuonPFCandSelector;

DEFINE_FWK_MODULE(PatElectronPFCandSelector);
DEFINE_FWK_MODULE(PatPhotonPFCandSelector);
DEFINE_FWK_MODULE(PatMuonPFCandSelector);
