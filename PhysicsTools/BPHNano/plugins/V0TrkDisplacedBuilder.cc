#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"

#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include <vector>
#include <memory>
#include <map>
#include <string>
#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"
#include "DataFormats/Candidate/interface/VertexCompositePtrCandidateFwd.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "CommonTools/Statistics/interface/ChiSquaredProbability.h"
#include "helper.h"
#include <limits>
#include <algorithm>
#include "KinVtxFitter.h"

typedef edm::Ptr<pat::PackedCandidate> PackedCandidatePtr;
typedef std::vector<PackedCandidatePtr> PackedCandidatePtrCollection;

using namespace std;

class V0TrkDisplacedBuilder : public edm::global::EDProducer<> {
  // perhaps we need better structure here (begin run etc)
public:
  typedef std::vector<reco::TransientTrack> TransientTrackCollection;
  typedef std::vector<reco::VertexCompositePtrCandidate> V0Collection;

  explicit V0TrkDisplacedBuilder(const edm::ParameterSet &cfg)
      :
        theB_(esConsumes(edm::ESInputTag{"", "TransientTrackBuilder"})),
        bFieldToken_(esConsumes<MagneticField, IdealMagneticFieldRecord>()),
        // selections
        pre_vtx_selection_{cfg.getParameter<std::string>("preVtxSelection")},
        post_vtx_selection_{cfg.getParameter<std::string>("postVtxSelection")},
        trk_selection_{cfg.getParameter<std::string>("trkSelection")},
        //inputs
        V0s_ttracks_{consumes<TransientTrackCollection>(cfg.getParameter<edm::InputTag>("V0s_ttracks"))},
        V0s_{consumes<pat::CompositeCandidateCollection>(cfg.getParameter<edm::InputTag>("V0s"))},
        v0sOriginal_{consumes<V0Collection>(cfg.getParameter<edm::InputTag>("V0sOriginal"))},
        packedPFCandidatesToken_{consumes<pat::PackedCandidateCollection>(cfg.getParameter<edm::InputTag>("packedPFCandidates"))},
        beamspot_{consumes<reco::BeamSpot>(cfg.getParameter<edm::InputTag>("beamSpot"))},
        vertex_src_{consumes<reco::VertexCollection>(cfg.getParameter<edm::InputTag>("offlinePrimaryVertexSrc"))},
        isXi_{cfg.getParameter<bool>("isXi")}{
    //output
    produces<pat::CompositeCandidateCollection>("SelectedV0TrkDisplaced");
    produces<PackedCandidatePtrCollection>("PackedPFCandidatePtrCollection");
  }

  ~V0TrkDisplacedBuilder() override {}

  void produce(edm::StreamID, edm::Event &, const edm::EventSetup &) const override;

  static void fillDescriptions(edm::ConfigurationDescriptions &descriptions) {}

private:
  const edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord> theB_;
  const edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> bFieldToken_;
  // selections
  const StringCutObjectSelector<pat::CompositeCandidate> pre_vtx_selection_;
  const StringCutObjectSelector<pat::CompositeCandidate> post_vtx_selection_;
  const StringCutObjectSelector<pat::PackedCandidate> trk_selection_;

  const edm::EDGetTokenT<TransientTrackCollection> V0s_ttracks_;
  const edm::EDGetTokenT<pat::CompositeCandidateCollection> V0s_;
  const edm::EDGetTokenT<V0Collection> v0sOriginal_;

  const edm::EDGetTokenT<pat::PackedCandidateCollection> packedPFCandidatesToken_;

  const edm::EDGetTokenT<reco::BeamSpot> beamspot_;
  const edm::EDGetTokenT<reco::VertexCollection> vertex_src_;

  const bool isXi_;
};

void V0TrkDisplacedBuilder::produce(edm::StreamID, edm::Event &evt, edm::EventSetup const &iSetup) const {
  // input
  auto const theB = &iSetup.getData(theB_);
  const auto &bField = iSetup.getData(bFieldToken_);

  edm::Handle<pat::CompositeCandidateCollection> V0s;
  evt.getByToken(V0s_, V0s);

  edm::Handle<TransientTrackCollection> V0s_ttracks;
  evt.getByToken(V0s_ttracks_, V0s_ttracks);

  edm::Handle<V0Collection> V0sOriginal;
  evt.getByToken(v0sOriginal_, V0sOriginal);

  edm::Handle<reco::BeamSpot> beamspot;
  evt.getByToken(beamspot_, beamspot);

  edm::Handle<reco::VertexCollection> pvtxs;
  evt.getByToken(vertex_src_, pvtxs);

  edm::Handle<pat::PackedCandidateCollection> packedPFCandidates;
  evt.getByToken(packedPFCandidatesToken_, packedPFCandidates);

  unsigned int nTracks = packedPFCandidates->size();

  edm::ESHandle<MagneticField> fieldHandle;
  const MagneticField *fMagneticField = fieldHandle.product();
  AnalyticalImpactPointExtrapolator extrapolator(fMagneticField);

  std::vector<int> used_lep1_id, used_lep2_id, used_pi_id, used_V0_id;

  // output
  std::unique_ptr<pat::CompositeCandidateCollection> ret_val(new pat::CompositeCandidateCollection());
  std::unique_ptr<PackedCandidatePtrCollection> ptr_out(new PackedCandidatePtrCollection);

  //
  // Loop over V0 candidates
  //
  for (size_t V0_idx = 0; V0_idx < V0s->size(); ++V0_idx) {

    edm::Ptr<pat::CompositeCandidate> V0_ptr(V0s, V0_idx);

    math::PtEtaPhiMLorentzVector V0_p4(V0_ptr->userFloat("fitted_pt"),V0_ptr->userFloat("fitted_eta"),V0_ptr->userFloat("fitted_phi"),V0_ptr->userFloat("fitted_mass"));

    auto v0Original = V0sOriginal->at(V0_ptr->userInt("OriginalV0Idx"));

    PackedCandidatePtr v0_trk1_ptr(packedPFCandidates,V0_ptr->userInt("prefit_trk1_keyPacked"));
    PackedCandidatePtr v0_trk2_ptr(packedPFCandidates,V0_ptr->userInt("prefit_trk2_keyPacked"));

    // auto v0Original_trk1_transientIdx = V0_ptr->userInt("trk1_transientColl_Idx");
    // auto v0Original_trk2_transientIdx = V0_ptr->userInt("trk2_transientColl_Idx");

    //
    // Loop over tracks
    //
    for (unsigned int iTrk = 0; iTrk < nTracks; ++iTrk) {
      const pat::PackedCandidate &trk = (*packedPFCandidates)[iTrk];

      // arranging cuts for speed
      if (!trk.hasTrackDetails())
        continue;
      if (abs(trk.pdgId()) != 211)
        continue;  // do we want also to keep muons?
      if (!trk_selection_(trk))
        continue;

      PackedCandidatePtr trk_ptr(packedPFCandidates, iTrk);

      //
      // Skip tracks that are the same ones used for this V0 candidate
      //
      if (v0_trk1_ptr == trk_ptr || v0_trk2_ptr == trk_ptr)
        continue;

      float trk_mass  = (isXi_) ? bph::PI_MASS  : bph::K_MASS;
      float trk_sigma = (isXi_) ? bph::PI_SIGMA : bph::K_SIGMA;

      math::PtEtaPhiMLorentzVector trk_p4(trk_ptr->pt(), trk_ptr->eta(), trk_ptr->phi(), trk_mass);

      //
      // V0+track candidate
      //
      pat::CompositeCandidate cand;

      cand.setP4(trk_ptr->p4() + V0_p4);

      cand.addUserInt("V0_idx", V0_idx);
      cand.addUserCand("trk", trk_ptr);
      cand.addUserCand("V0", V0_ptr);

      cand.addUserInt("trk_keyPacked",iTrk);

      float dr = deltaR(trk_ptr->eta(), trk_ptr->phi(), V0_ptr->userFloat("fitted_eta"), V0_ptr->userFloat("fitted_phi"));
      cand.addUserFloat("V0trk_dr", dr);

      cand.addUserFloat("V0_pt",   V0_ptr->pt());
      cand.addUserFloat("V0_eta",  V0_ptr->eta());
      cand.addUserFloat("V0_phi",  V0_ptr->phi());
      cand.addUserFloat("V0_mass", V0_ptr->mass());
      cand.addUserFloat("V0_prefit_mass", V0_ptr->userFloat("prefit_mass"));
      cand.addUserFloat("V0_fittedmass",V0_ptr->userFloat("fitted_mass"));

      cand.addUserFloat("V0_trk1_pt",   V0_ptr->userFloat("trk1_pt"));
      cand.addUserFloat("V0_trk1_eta",  V0_ptr->userFloat("trk1_eta"));
      cand.addUserFloat("V0_trk1_phi",  V0_ptr->userFloat("trk1_phi"));
      cand.addUserFloat("V0_trk1_p",    V0_ptr->userFloat("trk1_p"));
      cand.addUserInt("V0_trk1_keyPacked", V0_ptr->userInt("trk1_keyPacked"));

      cand.addUserFloat("V0_trk2_pt",   V0_ptr->userFloat("trk2_pt"));
      cand.addUserFloat("V0_trk2_eta",  V0_ptr->userFloat("trk2_eta"));
      cand.addUserFloat("V0_trk2_phi",  V0_ptr->userFloat("trk2_phi"));
      cand.addUserFloat("V0_trk2_p",    V0_ptr->userFloat("trk2_p"));
      cand.addUserInt("V0_trk2_keyPacked", V0_ptr->userInt("trk2_keyPacked"));

      cand.addUserFloat("V0_prefit_trk1_pt",   V0_ptr->userFloat("prefit_trk1_pt"));
      cand.addUserFloat("V0_prefit_trk1_eta",  V0_ptr->userFloat("prefit_trk1_eta"));
      cand.addUserFloat("V0_prefit_trk1_phi",  V0_ptr->userFloat("prefit_trk1_phi"));
      cand.addUserFloat("V0_prefit_trk1_p",    V0_ptr->userFloat("prefit_trk1_p"));
      cand.addUserInt("V0_prefit_trk1_keyPacked", V0_ptr->userInt("prefit_trk1_keyPacked"));

      cand.addUserFloat("V0_prefit_trk2_pt",   V0_ptr->userFloat("prefit_trk2_pt"));
      cand.addUserFloat("V0_prefit_trk2_eta",  V0_ptr->userFloat("prefit_trk2_eta"));
      cand.addUserFloat("V0_prefit_trk2_phi",  V0_ptr->userFloat("prefit_trk2_phi"));
      cand.addUserFloat("V0_prefit_trk2_p",    V0_ptr->userFloat("prefit_trk2_p"));
      cand.addUserInt("V0_prefit_trk2_keyPacked", V0_ptr->userInt("prefit_trk2_keyPacked"));

      auto V0_p4 = V0_ptr->polarP4();
      trk_p4.SetM(trk_mass);
      V0_p4.SetM(V0_ptr->mass());

      cand.setP4(trk_p4 + V0_p4);

      cand.setCharge(V0_ptr->userInt("prefit_trk1_charge") + V0_ptr->userInt("prefit_trk2_charge") + trk_ptr->charge());

      cand.addUserFloat("prefit_trk_pt",   trk_p4.pt());
      cand.addUserFloat("prefit_trk_eta",  trk_p4.eta());
      cand.addUserFloat("prefit_trk_phi",  trk_p4.phi());
      cand.addUserFloat("prefit_trk_mass", trk_p4.mass());

      cand.addUserFloat("prefit_pt",    (trk_p4+V0_p4).pt());
      cand.addUserFloat("prefit_eta",   (trk_p4+V0_p4).eta());
      cand.addUserFloat("prefit_phi",   (trk_p4+V0_p4).phi());
      cand.addUserFloat("prefit_mass",  (trk_p4+V0_p4).mass());

      auto V0_dr_info = bph::min_max_dr({v0_trk1_ptr, v0_trk2_ptr, trk_ptr});
      cand.addUserFloat("V0_trk_min_dr", V0_dr_info.first);
      cand.addUserFloat("V0_trk_max_dr", V0_dr_info.second);

      auto trk_v0trks_dr_info = bph::min_max_dr({v0_trk1_ptr, v0_trk2_ptr, trk_ptr});
      cand.addUserFloat("tracks_min_dr", trk_v0trks_dr_info.first);
      cand.addUserFloat("tracks_max_dr", trk_v0trks_dr_info.second);

      if (!pre_vtx_selection_(cand))
        continue;

      const reco::TransientTrack trackTT((*trk.bestTrack()), &bField);

      KinVtxFitter v0trkfitter(
        {trackTT,  V0s_ttracks->at(V0_idx)},
        {trk_mass, V0_ptr->mass()},
        {trk_sigma,V0_ptr->userFloat("massErr")}
      );

      if (!v0trkfitter.success())
        continue;

      const auto &V0TrkTT = v0trkfitter.fitted_candidate_ttrk();

      float v0trkmass = v0trkfitter.fitted_candidate().mass();
      float v0trkmassErr = sqrt(v0trkfitter.fitted_candidate().kinematicParametersError().matrix()(6, 6));

      cand.addUserFloat("sv_chi2", v0trkfitter.chi2());
      cand.addUserFloat("sv_ndof", v0trkfitter.dof());
      cand.addUserFloat("sv_prob", v0trkfitter.prob());

      cand.addUserFloat("vtx_x", v0trkfitter.fitted_vtx().x());
      cand.addUserFloat("vtx_y", v0trkfitter.fitted_vtx().y());
      cand.addUserFloat("vtx_z", v0trkfitter.fitted_vtx().z());

      cand.addUserFloat("vtx_ex", sqrt(v0trkfitter.fitted_vtx_uncertainty().cxx()));
      cand.addUserFloat("vtx_ey", sqrt(v0trkfitter.fitted_vtx_uncertainty().cyy()));
      cand.addUserFloat("vtx_ez", sqrt(v0trkfitter.fitted_vtx_uncertainty().czz()));

      cand.addUserFloat("fitted_trk_pt",  v0trkfitter.daughter_p4(0).pt());
      cand.addUserFloat("fitted_trk_eta", v0trkfitter.daughter_p4(0).eta());
      cand.addUserFloat("fitted_trk_phi", v0trkfitter.daughter_p4(0).phi());
      cand.addUserFloat("fitted_trk_mass", v0trkfitter.daughter_p4(0).mass());

      cand.addUserFloat("fitted_V0_pt",   v0trkfitter.daughter_p4(1).pt());
      cand.addUserFloat("fitted_V0_eta",  v0trkfitter.daughter_p4(1).eta());
      cand.addUserFloat("fitted_V0_phi",  v0trkfitter.daughter_p4(1).phi());
      cand.addUserFloat("fitted_V0_mass", v0trkfitter.daughter_p4(1).mass());

      cand.addUserFloat("fitted_mass",     v0trkmass);
      cand.addUserFloat("fitted_massErr",  v0trkmassErr);

      auto V0Trk_fit_p4 = v0trkfitter.fitted_p4();
      cand.addUserFloat("fitted_cos_theta_2D", bph::cos_theta_2D(v0trkfitter, *beamspot, V0Trk_fit_p4));

      auto V0Trk_lxy = bph::l_xy(v0trkfitter, *beamspot);
      cand.addUserFloat("l_xy",     V0Trk_lxy.value());
      cand.addUserFloat("l_xy_unc", V0Trk_lxy.error());


      const reco::BeamSpot &beamSpot = *beamspot;
      TrajectoryStateClosestToPoint theDCAXBS = V0TrkTT.trajectoryStateClosestToPoint(GlobalPoint(beamSpot.position().x(), beamSpot.position().y(), beamSpot.position().z()));
      double DCAB0BS = -99.;
      double DCAB0BSErr = -99.;

      if (theDCAXBS.isValid() == true) {
        DCAB0BS = theDCAXBS.perigeeParameters().transverseImpactParameter();
        DCAB0BSErr = theDCAXBS.perigeeError().transverseImpactParameterError();
      }
      cand.addUserFloat("dca", DCAB0BS);
      cand.addUserFloat("dcaErr", DCAB0BSErr);

      TrajectoryStateOnSurface V0tsos = extrapolator.extrapolate(V0s_ttracks->at(V0_idx).impactPointState(), v0trkfitter.fitted_vtx());
      cand.addUserFloat("V0_dz", V0s_ttracks->at(V0_idx).track().vz() - v0trkfitter.fitted_vtx().z());  //
      cand.addUserFloat("V0_x",  V0s_ttracks->at(V0_idx).track().vx());         //pitsos.globalPosition().x());
      cand.addUserFloat("V0_y",  V0s_ttracks->at(V0_idx).track().vy());         //pitsos.globalPosition().y());
      cand.addUserFloat("V0_z",  V0s_ttracks->at(V0_idx).track().vz());         //pitsos.globalPosition().z());
                                                                                // vertex vars

      std::pair<bool, Measurement1D> cur2DIP =  bph::signedTransverseImpactParameter(V0tsos, v0trkfitter.fitted_refvtx(), *beamspot);
      std::pair<bool, Measurement1D> cur3DIP =  bph::signedImpactParameter3D(V0tsos, v0trkfitter.fitted_refvtx(), *beamspot, (*pvtxs)[0].position().z());
      cand.addUserFloat("svip2d",     cur2DIP.second.value());
      cand.addUserFloat("svip2d_err", cur2DIP.second.error());
      cand.addUserFloat("svip3d",     cur3DIP.second.value());
      cand.addUserFloat("svip3d_err", cur3DIP.second.error());

      if (!post_vtx_selection_(cand))
        continue;

      ret_val->push_back(cand);
      ptr_out->push_back(trk_ptr);
    }  //   for(size_t pi_idx = 0; pi_idx < pions->size(); ++V0_idx)
  }  // for(size_t V0_idx = 0; V0_idx < V0s->size(); ++V0_idx)

  evt.put(std::move(ret_val), "SelectedV0TrkDisplaced");
  evt.put(std::move(ptr_out), "PackedPFCandidatePtrCollection");
}
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(V0TrkDisplacedBuilder);

