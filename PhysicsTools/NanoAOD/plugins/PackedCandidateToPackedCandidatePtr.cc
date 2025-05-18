#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

class PackedCandidateToPackedCandidatePtr : public edm::stream::EDProducer<> {
public:

  PackedCandidateToPackedCandidatePtr(edm::ParameterSet const& params)
      : srcToken_{consumes<std::vector<pat::PackedCandidate>>(params.getParameter<edm::InputTag>("src"))}{
    produces<std::vector<edm::Ptr<pat::PackedCandidate>>>();
  }

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("src", edm::InputTag(""))->setComment("InputTag used for retrieving jets in event.");
    descriptions.addWithDefaultLabel(desc);
  }

  void produce(edm::Event& iEvent, edm::EventSetup const& iSetup) override {
    auto candsOut = std::make_unique<std::vector<edm::Ptr<pat::PackedCandidate>>>();

    edm::Handle<std::vector<pat::PackedCandidate>> packedCandHandle;
    iEvent.getByToken(srcToken_, packedCandHandle);

    for (size_t objNr = 0; objNr < packedCandHandle->size(); objNr++) {
      candsOut->emplace_back(edm::Ptr<pat::PackedCandidate>(packedCandHandle, objNr));
    }
    iEvent.put(std::move(candsOut));
  }
private:
  edm::EDGetTokenT<std::vector<pat::PackedCandidate>> const srcToken_;
};

DEFINE_FWK_MODULE(PackedCandidateToPackedCandidatePtr);

