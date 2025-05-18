/////////////////////////////// BToV0LLBuilder ///////////////////////////////
/// original authors: G Karathanasis (CERN),  G Melachroinos (NKUA)
// takes V0 cabds from CMSSW and creates useful V0

#include <algorithm>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "CommonTools/Statistics/interface/ChiSquaredProbability.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/Candidate/interface/VertexCompositeCandidate.h"
#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"
#include "DataFormats/Candidate/interface/VertexCompositePtrCandidateFwd.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "KinVtxFitter.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "RecoVertex/KinematicFit/interface/KinematicConstrainedVertexFitter.h"
#include "RecoVertex/KinematicFit/interface/TwoTrackMassKinematicConstraint.h"
#include "RecoVertex/KinematicFitPrimitives/interface/KinematicParticleFactoryFromTransientTrack.h"
#include "RecoVertex/KinematicFitPrimitives/interface/MultiTrackKinematicConstraint.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "helper.h"

typedef edm::Ptr<pat::PackedCandidate> PackedCandidatePtr;
typedef std::vector<PackedCandidatePtr> PackedCandidatePtrCollection;

class V0ReBuilderV2 : public edm::global::EDProducer<> {
  // perhaps we need better structure here (begin run etc)
public:
  typedef std::vector<reco::TransientTrack> TransientTrackCollection;
  typedef std::vector<reco::VertexCompositePtrCandidate> V0Collection;

  explicit V0ReBuilderV2(const edm::ParameterSet &cfg)
      : theB_(esConsumes(edm::ESInputTag{"", "TransientTrackBuilder"})),
        trk_selection_{cfg.getParameter<std::string>("trkSelection")},
        pre_vtx_selection_{cfg.getParameter<std::string>("V0Selection")},
        post_vtx_selection_{cfg.getParameter<std::string>("postVtxSelection")},
        v0s_{consumes<V0Collection>(cfg.getParameter<edm::InputTag>("V0s"))},
        beamspot_{consumes<reco::BeamSpot>(cfg.getParameter<edm::InputTag>("beamSpot"))},
        packedPFCandidatesToken_{consumes<pat::PackedCandidateCollection>(cfg.getParameter<edm::InputTag>("packedPFCandidates"))},
        isLambda_{cfg.getParameter<bool>("isLambda")} {
    produces<pat::CompositeCandidateCollection>("SelectedV0Collection");
    produces<TransientTrackCollection>("SelectedV0TransientCollection");
    produces<PackedCandidatePtrCollection>("PackedPFCandidatePtrCollection");
  }

  ~V0ReBuilderV2() override {}

  void produce(edm::StreamID, edm::Event &, const edm::EventSetup &) const override;

private:
  const edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord> theB_;
  const StringCutObjectSelector<pat::PackedCandidate> trk_selection_;
  const StringCutObjectSelector<reco::VertexCompositePtrCandidate> pre_vtx_selection_;
  const StringCutObjectSelector<pat::CompositeCandidate> post_vtx_selection_;
  const edm::EDGetTokenT<V0Collection> v0s_;
  const edm::EDGetTokenT<reco::BeamSpot> beamspot_;
  const edm::EDGetTokenT<pat::PackedCandidateCollection> packedPFCandidatesToken_;
  const bool isLambda_;
};

void V0ReBuilderV2::produce(edm::StreamID, edm::Event &evt, edm::EventSetup const &iSetup) const {
  // input
  auto const theB = &iSetup.getData(theB_);
  edm::Handle<V0Collection> V0s;
  evt.getByToken(v0s_, V0s);
  edm::Handle<reco::BeamSpot> beamspot;
  evt.getByToken(beamspot_, beamspot);
  edm::Handle<pat::PackedCandidateCollection> packedPFCandidates;
  evt.getByToken(packedPFCandidatesToken_, packedPFCandidates);

  // output
  std::unique_ptr<pat::CompositeCandidateCollection> ret_val(new pat::CompositeCandidateCollection());
  std::unique_ptr<TransientTrackCollection> trans_out(new TransientTrackCollection);

  std::unique_ptr<PackedCandidatePtrCollection> ptr_out(new PackedCandidatePtrCollection);

  for (reco::VertexCompositePtrCandidateCollection::const_iterator v0 = V0s->begin(); v0 != V0s->end(); v0++) {
    // selection on V0s
    if (v0->numberOfDaughters() != 2)
      continue;
    if (!pre_vtx_selection_(*v0))
      continue;

    pat::PackedCandidate v0daughter1 = *(dynamic_cast<const pat::PackedCandidate *>(v0->daughter(0)));
    pat::PackedCandidate v0daughter2 = *(dynamic_cast<const pat::PackedCandidate *>(v0->daughter(1)));

    if (!v0daughter1.hasTrackDetails())
      continue;
    if (!v0daughter2.hasTrackDetails())
      continue;

    if (abs(v0daughter1.pdgId()) != 211)
      continue;  // This cut do not affect the Lambda->proton pion candidates
    if (abs(v0daughter2.pdgId()) != 211)
      continue;  // This cut do not affect the Lambda->proton pion candidates

    if (!trk_selection_(v0daughter1) || !trk_selection_(v0daughter2))
      continue;

    reco::TransientTrack v0daughter1_ttrack;  // 1st daughter, leading daughter to be assigned.
                                              // Proton mass will be assigned for the
                                              // Lambda->Proton Pion mode, Pion mass will be
                                              // assigned for the Kshort->PionPion mode.
    reco::TransientTrack v0daughter2_ttrack;  // 2nd daughter, subleading daughter to be
                                              // assigned. It hass always the pion mass

    // auto trk1_ptr = v0->daughterPtr(0);
    // auto trk2_ptr = v0->daughterPtr(1);

    int   prefit_trk1_key = -1;
    float prefit_trk1_pt = -1.f;
    float prefit_trk1_p = -1.f;
    float prefit_trk1_eta = -9.f;
    float prefit_trk1_phi = -9.f;

    int   prefit_trk2_key = -1;
    float prefit_trk2_pt = -1.f;
    float prefit_trk2_p = -1.f;
    float prefit_trk2_eta = -9.f;
    float prefit_trk2_phi = -9.f;

    if (v0daughter1.p() > v0daughter2.p()) {
      v0daughter1_ttrack = theB->build(v0daughter1.bestTrack());
      v0daughter2_ttrack = theB->build(v0daughter2.bestTrack());
      prefit_trk1_key = v0->daughterPtr(0).key();
      prefit_trk2_key = v0->daughterPtr(1).key();
      prefit_trk1_pt  = v0daughter1.bestTrack()->pt();
      prefit_trk2_pt  = v0daughter2.bestTrack()->pt();
      prefit_trk1_p   = v0daughter1.bestTrack()->p();
      prefit_trk2_p   = v0daughter2.bestTrack()->p();
      prefit_trk1_eta = v0daughter1.bestTrack()->eta();
      prefit_trk2_eta = v0daughter2.bestTrack()->eta();
      prefit_trk1_phi = v0daughter1.bestTrack()->phi();
      prefit_trk2_phi = v0daughter2.bestTrack()->phi();

    } else {
      v0daughter1_ttrack = theB->build(v0daughter2.bestTrack());
      v0daughter2_ttrack = theB->build(v0daughter1.bestTrack());
      prefit_trk1_key = v0->daughterPtr(1).key();
      prefit_trk2_key = v0->daughterPtr(0).key();
      prefit_trk1_pt  = v0daughter2.bestTrack()->pt();
      prefit_trk2_pt  = v0daughter1.bestTrack()->pt();
      prefit_trk1_p   = v0daughter2.bestTrack()->p();
      prefit_trk2_p   = v0daughter1.bestTrack()->p();
      prefit_trk1_eta = v0daughter2.bestTrack()->eta();
      prefit_trk2_eta = v0daughter1.bestTrack()->eta();
      prefit_trk1_phi = v0daughter2.bestTrack()->phi();
      prefit_trk2_phi = v0daughter1.bestTrack()->phi();
    }

    float Track1_mass = (isLambda_) ? bph::PROT_MASS : bph::PI_MASS;
    float Track1_sigma = bph::PI_SIGMA;
    float Track2_mass = bph::PI_MASS;
    float Track2_sigma = bph::PI_SIGMA;
    // create V0 vertex
    KinVtxFitter fitter(
        {v0daughter1_ttrack, v0daughter2_ttrack}, {Track1_mass, Track2_mass}, {Track1_sigma, Track2_sigma});

    if (!fitter.success())
      continue;

    pat::CompositeCandidate cand;
    cand.setVertex(reco::Candidate::Point(fitter.fitted_vtx().x(), fitter.fitted_vtx().y(), fitter.fitted_vtx().z()));
    auto fit_p4 = fitter.fitted_p4();
    cand.setP4(fit_p4);

    cand.setCharge(v0daughter1.charge() + v0daughter2.charge());
    cand.addUserFloat("sv_chi2", fitter.chi2());
    cand.addUserFloat("sv_prob", fitter.prob());
    cand.addUserFloat("fitted_mass", fitter.fitted_candidate().mass());
    cand.addUserFloat("massErr", sqrt(fitter.fitted_candidate().kinematicParametersError().matrix()(6, 6)));
    cand.addUserFloat("cos_theta_2D", bph::cos_theta_2D(fitter, *beamspot, cand.p4()));
    cand.addUserFloat("fitted_cos_theta_2D", bph::cos_theta_2D(fitter, *beamspot, fit_p4));
    auto lxy = bph::l_xy(fitter, *beamspot);
    cand.addUserFloat("l_xy", lxy.value());
    cand.addUserFloat("l_xy_unc", lxy.error());

    if (!post_vtx_selection_(cand))
      continue;

    cand.addUserFloat("vtx_x", cand.vx());
    cand.addUserFloat("vtx_y", cand.vy());
    cand.addUserFloat("vtx_z", cand.vz());

    const auto &covMatrix = fitter.fitted_vtx_uncertainty();
    cand.addUserFloat("vtx_cxx", covMatrix.cxx());
    cand.addUserFloat("vtx_cyy", covMatrix.cyy());
    cand.addUserFloat("vtx_czz", covMatrix.czz());
    cand.addUserFloat("vtx_cyx", covMatrix.cyx());
    cand.addUserFloat("vtx_czx", covMatrix.czx());
    cand.addUserFloat("vtx_czy", covMatrix.czy());

    cand.addUserFloat("prefit_mass", v0->mass());
    int trk1 = 0;
    int trk2 = 1;

    int postfit_trk1_key = prefit_trk1_key;
    int postfit_trk2_key = prefit_trk2_key;
    if (fitter.daughter_p4(0).pt() < fitter.daughter_p4(1).pt()) {
      trk1 = 1;
      trk2 = 0;
      postfit_trk1_key = prefit_trk2_key;
      postfit_trk2_key = prefit_trk1_key;
    }
    cand.addUserFloat("trk1_pt",  fitter.daughter_p4(trk1).pt());
    cand.addUserFloat("trk1_eta", fitter.daughter_p4(trk1).eta());
    cand.addUserFloat("trk1_phi", fitter.daughter_p4(trk1).phi());
    cand.addUserFloat("trk1_p",   fitter.daughter_p4(trk1).P());

    cand.addUserFloat("trk2_pt", fitter.daughter_p4(trk2).pt());
    cand.addUserFloat("trk2_eta",fitter.daughter_p4(trk2).eta());
    cand.addUserFloat("trk2_phi",fitter.daughter_p4(trk2).phi());
    cand.addUserFloat("trk2_p",  fitter.daughter_p4(trk2).P());

    cand.addUserInt("trk1_keyPacked", postfit_trk1_key);
    cand.addUserInt("trk2_keyPacked", postfit_trk2_key);

    cand.addUserFloat("prefit_trk1_pt",  prefit_trk1_pt);
    cand.addUserFloat("prefit_trk1_p",   prefit_trk1_p);
    cand.addUserFloat("prefit_trk1_eta", prefit_trk1_eta);
    cand.addUserFloat("prefit_trk1_phi", prefit_trk1_phi);

    cand.addUserFloat("prefit_trk2_pt",  prefit_trk2_pt);
    cand.addUserFloat("prefit_trk2_p",   prefit_trk2_p);
    cand.addUserFloat("prefit_trk2_eta", prefit_trk2_eta);
    cand.addUserFloat("prefit_trk2_phi", prefit_trk2_phi);

    cand.addUserInt("prefit_trk1_keyPacked", prefit_trk1_key);
    cand.addUserInt("prefit_trk2_keyPacked", prefit_trk2_key);

    ptr_out->push_back(PackedCandidatePtr(packedPFCandidates,prefit_trk1_key));
    ptr_out->push_back(PackedCandidatePtr(packedPFCandidates,prefit_trk2_key));

    // save
    ret_val->push_back(cand);
    auto V0TT = fitter.fitted_candidate_ttrk();
    trans_out->emplace_back(V0TT);
  }

  evt.put(std::move(ret_val),   "SelectedV0Collection");
  evt.put(std::move(trans_out), "SelectedV0TransientCollection");
  evt.put(std::move(ptr_out),   "PackedPFCandidatePtrCollection");
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(V0ReBuilderV2);
