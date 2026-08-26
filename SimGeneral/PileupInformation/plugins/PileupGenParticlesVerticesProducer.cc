#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "FWCore/Utilities/interface/EDMException.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <vector>
#include <string>
#include <unordered_map>

namespace {

  class ConvertPileupParticle {
  public:
    static constexpr int PDGCacheMax = 32768;
    static constexpr double mmToCm = 0.1;
    static constexpr double mmToNs = 1.0 / 299792458e-6;

    ConvertPileupParticle()
        : abortOnUnknownPDGCode_(true), initialized_(false), chargeP_(PDGCacheMax, 0), chargeM_(PDGCacheMax, 0) {}

    ConvertPileupParticle(bool abortOnUnknownPDGCode)
        : abortOnUnknownPDGCode_(abortOnUnknownPDGCode),
          initialized_(false),
          chargeP_(PDGCacheMax, 0),
          chargeM_(PDGCacheMax, 0) {}

    ~ConvertPileupParticle() {}

    bool initialized() const { return initialized_; }

    void init(HepPDT::ParticleDataTable const& pdt) {
      if (!initialized_) {
        for (HepPDT::ParticleDataTable::const_iterator p = pdt.begin(); p != pdt.end(); ++p) {
          HepPDT::ParticleID const& id = p->first;
          int pdgId = id.pid(), apdgId = std::abs(pdgId);
          int q3 = id.threeCharge();
          if (apdgId < PDGCacheMax && pdgId > 0) {
            chargeP_[apdgId] = q3;
            chargeM_[apdgId] = -q3;
          } else if (apdgId < PDGCacheMax) {
            chargeP_[apdgId] = -q3;
            chargeM_[apdgId] = q3;
          } else {
            chargeMap_.emplace(pdgId, q3);
            chargeMap_.emplace(-pdgId, -q3);
          }
        }
        initialized_ = true;
      }
    }

    bool operator()(reco::GenParticle& cand, HepMC::GenParticle const* part) const {
      reco::Candidate::LorentzVector p4(part->momentum());
      int pdgId = part->pdg_id();
      cand.setThreeCharge(chargeTimesThree(pdgId));
      cand.setPdgId(pdgId);
      cand.setStatus(part->status());
      cand.setP4(p4);
      cand.setCollisionId(0);
      HepMC::GenVertex const* v = part->production_vertex();
      if (v != nullptr) {
        HepMC::ThreeVector vtx = v->point3d();
        reco::Candidate::Point vertex(vtx.x() * mmToCm, vtx.y() * mmToCm, vtx.z() * mmToCm);
        cand.setVertex(vertex);
      } else {
        cand.setVertex(reco::Candidate::Point(0, 0, 0));
      }
      return true;
    }

  private:
    bool abortOnUnknownPDGCode_;
    bool initialized_;
    std::vector<int> chargeP_, chargeM_;
    std::unordered_map<int, int> chargeMap_;

    int chargeTimesThree(int id) const {
      if (std::abs(id) < PDGCacheMax)
        return id > 0 ? chargeP_[id] : chargeM_[-id];

      auto f = chargeMap_.find(id);
      if (f == chargeMap_.end()) {
        if (abortOnUnknownPDGCode_)
          throw edm::Exception(edm::errors::LogicError) << "invalid PDG id: " << id << std::endl;
        else
          return HepPDT::ParticleID(id).threeCharge();
      }
      return f->second;
    }
  };

  class SelectStableParticle {
  public:
    bool operator()(HepMC::GenParticle const* part) const {
      bool selection = part->status() == 1;
      return selection;
    }
  };

}  // Anonymous namespace

#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/ESGetToken.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

#include "SimDataFormats/CrossingFrame/interface/CrossingFrame.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"

namespace edm {
  class ParameterSet;
}

class PileupGenParticlesVerticesProducer : public edm::global::EDProducer<edm::RunCache<ConvertPileupParticle> > {
public:
  PileupGenParticlesVerticesProducer(const edm::ParameterSet&);
  ~PileupGenParticlesVerticesProducer() override;

  void produce(edm::StreamID, edm::Event& e, const edm::EventSetup&) const override;
  std::shared_ptr<ConvertPileupParticle> globalBeginRun(const edm::Run&, const edm::EventSetup&) const override;
  void globalEndRun(edm::Run const&, edm::EventSetup const&) const override {}

private:
  edm::EDGetTokenT<CrossingFrame<edm::HepMCProduct> > mixToken_;
  edm::ESGetToken<HepPDT::ParticleDataTable, edm::DefaultRecord> pdtToken_;
  bool abortOnUnknownPDGCode_;
  int nMaxPUEvent_;
  SelectStableParticle select_;

  static constexpr double mmToCm = 0.1;
  static constexpr double mmToNs = 1.0 / 299792458e-6;
};

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/EDMException.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "SimDataFormats/CrossingFrame/interface/MixCollection.h"

#include <algorithm>

using namespace edm;
using namespace reco;
using namespace std;
using namespace HepMC;

PileupGenParticlesVerticesProducer::PileupGenParticlesVerticesProducer(const ParameterSet& cfg)
    : abortOnUnknownPDGCode_(cfg.getUntrackedParameter<bool>("abortOnUnknownPDGCode", true)),
      nMaxPUEvent_(cfg.getParameter<int>("nMaxPUEvent")) {

  for(int iPU=0; iPU < nMaxPUEvent_; iPU++){
    produces<GenParticleCollection>("genParticlesStatusOneBX0PUEvent"+std::to_string(iPU));
    produces<math::XYZPointF>("xyz0BX0PUEvent"+std::to_string(iPU));
    produces<float>("t0BX0PUEvent"+std::to_string(iPU));
  }

  mixToken_ = consumes<CrossingFrame<HepMCProduct> >(InputTag(cfg.getParameter<std::string>("mix"), "generatorSmeared"));
  pdtToken_ = esConsumes<HepPDT::ParticleDataTable, edm::DefaultRecord, edm::Transition::BeginRun>();
}

PileupGenParticlesVerticesProducer::~PileupGenParticlesVerticesProducer() {}

std::shared_ptr<ConvertPileupParticle> PileupGenParticlesVerticesProducer::globalBeginRun(const Run&, const EventSetup& es) const {
  ESHandle<HepPDT::ParticleDataTable> pdt = es.getHandle(pdtToken_);
  auto convert_ptr = std::make_shared<ConvertPileupParticle>(abortOnUnknownPDGCode_);
  if (!convert_ptr->initialized())
    convert_ptr->init(*pdt);

  return convert_ptr;
}

void PileupGenParticlesVerticesProducer::produce(StreamID, Event& evt, const EventSetup& es) const {
  size_t totalSize = 0;
  size_t npiles = 1;

  std::vector<std::unique_ptr<GenParticleCollection>> vecOutGenParts_;
  std::vector<std::unique_ptr<math::XYZPointF>>       vecOutXYZ0_;
  std::vector<std::unique_ptr<float>>                 vecOutt0_;
  for(int iPU = 0; iPU < nMaxPUEvent_; iPU++){
    vecOutGenParts_.emplace_back(std::make_unique<reco::GenParticleCollection>());
    vecOutXYZ0_.emplace_back(std::make_unique<math::XYZPointF>());
    vecOutt0_.emplace_back(std::make_unique<float>());
  }

  Handle<CrossingFrame<HepMCProduct> > cf;
  evt.getByToken(mixToken_, cf);
  std::unique_ptr<MixCollection<HepMCProduct> > cfhepmcprod(new MixCollection<HepMCProduct>(cf.product()));
  npiles = cfhepmcprod->size();

  LogDebug("PileupGenParticlesVerticesProducer") << " Number of pile-up events : " << npiles << endl;

  for (size_t icf = 0; icf < npiles; ++icf) {
    LogDebug("PileupGenParticlesVerticesProducer") << "CF " << icf << " size : " << cfhepmcprod->getObject(icf).GetEvent()->particles_size() << endl;
    totalSize += cfhepmcprod->getObject(icf).GetEvent()->particles_size();
  }
  LogDebug("PileupGenParticlesVerticesProducer") << "Total size : " << totalSize << endl;

  // Loop over pile-up events
  ConvertPileupParticle const& convertParticle_ = *runCache(evt.getRun().index());

  MixCollection<HepMCProduct>::MixItr mixHepMC_itr;
  unsigned int total_number_of_stable_particles = 0;
  size_t idx_mix = 0;

  // Fill collection

  int PUEventNumber = 0;
  for (mixHepMC_itr = cfhepmcprod->begin(); mixHepMC_itr != cfhepmcprod->end(); ++mixHepMC_itr, ++idx_mix) {
    int bunch = mixHepMC_itr.bunch();
    // Choose BX0 only
    if (bunch != 0) continue;

    // Get PUEventNumber in this bunch crossing
    ////// int PUEventNumber = mixHepMC_itr.getPileupEventNr();
    
    // Get the HEPMC event
    auto event = (*mixHepMC_itr).GetEvent();
    //
    // Get production vertex
    //
    auto origin = (*event->vertices_begin())->position();
    vecOutXYZ0_[PUEventNumber]->SetXYZ(origin.x() * mmToCm, origin.y() * mmToCm, origin.z() * mmToCm);
    *vecOutt0_[PUEventNumber] = origin.t() * mmToNs;
    //
    // Fill gen particles
    //
    unsigned int number_of_stable_particles = 0;
    for (auto p = event->particles_begin(); p != event->particles_end(); ++p) {
      HepMC::GenParticle const* part = *p;
      if (select_(part)) {
        reco::GenParticle cand;
        convertParticle_(cand, part);
        ++number_of_stable_particles;
        vecOutGenParts_[PUEventNumber]->push_back(cand);
      }
    }
    LogDebug("PileupGenParticlesVerticesProducer") << "Idx : " << idx_mix << " Bunch : " << bunch
                                    << " Number of particles : " << event->particles_size()
                                    << " Number of stable_particles : " << number_of_stable_particles << endl;

    total_number_of_stable_particles += number_of_stable_particles;
    PUEventNumber+=1;
  }
  LogDebug("PileupGenParticlesVerticesProducer") << "Total number of stable_particles : " << total_number_of_stable_particles << endl;

  for(int iPU = 0; iPU < nMaxPUEvent_; iPU++){
    evt.put(std::move(vecOutGenParts_[iPU]),"genParticlesStatusOneBX0PUEvent"+std::to_string(iPU));
    evt.put(std::move(vecOutXYZ0_[iPU]),    "xyz0BX0PUEvent"+std::to_string(iPU));
    evt.put(std::move(vecOutt0_[iPU]),      "t0BX0PUEvent"+std::to_string(iPU));
  }
}

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(PileupGenParticlesVerticesProducer);
