#include "CommonTools/UtilAlgos/interface/StringCutObjectSelector.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

class PatTauConstituentSelector : public edm::stream::EDProducer<> {
public:
  PatTauConstituentSelector(edm::ParameterSet const& params)
      : srcToken_{consumes<edm::View<pat::Tau>>(params.getParameter<edm::InputTag>("src"))},
        selector_{params.getParameter<std::string>("cut")} {
    produces<std::vector<pat::Tau>>();
    produces<std::vector<edm::Ptr<pat::PackedCandidate>>>("constituents");
  }

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("src", edm::InputTag(""))->setComment("InputTag used for retrieving taus in event.");
    desc.add<std::string>("cut", "")->setComment("Cut used by which to select taus.");
    descriptions.addWithDefaultLabel(desc);
  }

  edm::Ptr<pat::PackedCandidate> const initptr(edm::Ptr<reco::Candidate> const& dau) const {
    edm::Ptr<pat::PackedCandidate> retval(dau);
    return retval;
  }

  void produce(edm::Event& iEvent, edm::EventSetup const& iSetup) override {
    auto taus = std::make_unique<std::vector<pat::Tau>>();
    auto candsOut = std::make_unique<std::vector<edm::Ptr<pat::PackedCandidate>>>();

    edm::Handle<edm::View<pat::Tau>> h_taus;
    iEvent.getByToken(srcToken_, h_taus);

    // Now set the Ptrs with the orphan handles.
    for (auto const& tau : *h_taus) {
      // Check the selection
      if (selector_(tau)) {
        // Add the jets that pass to the output collection
        taus->push_back(tau);

        for (unsigned int ida = 0; ida < tau.signalCands().size(); ++ida) {
          candsOut->emplace_back(initptr(tau.signalCands()[ida]));
        }
        for (unsigned int ida = 0; ida < tau.isolationCands().size(); ++ida) {
          candsOut->emplace_back(initptr(tau.isolationCands()[ida]));
        }
      }
    }
    iEvent.put(std::move(taus));
    iEvent.put(std::move(candsOut), "constituents");
  }

private:
  edm::EDGetTokenT<edm::View<pat::Tau>> const srcToken_;
  StringCutObjectSelector<pat::Tau> const selector_;
};


DEFINE_FWK_MODULE(PatTauConstituentSelector);