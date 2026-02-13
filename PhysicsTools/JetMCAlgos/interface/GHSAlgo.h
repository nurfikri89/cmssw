// $Id: GHSAlgo.hh 1507 2025-01-30 10:33:36Z gstagn $
//
// Copyright (c) 2025, Rhorry Gauld, Alexander Huss, Giovanni Stagnitto
//
// based on initial version by Fabrizio Caola, Radoslaw Grabarczyk,
// Maxwell Hutt, Gavin P. Salam, Ludovic Scyboz, and Jesse Thaler
//
//----------------------------------------------------------------------
// This file is part of FastJet contrib.
//
// It is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2 of the License, or (at
// your option) any later version.
//
// It is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this code. If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------

#ifndef PhysicsTools_JetMCAlgos_GHSAlgo_H
#define PhysicsTools_JetMCAlgos_GHSAlgo_H

#include <fastjet/internal/base.hh>

#include <iostream>
#include <iomanip>

#ifndef __FJC_FLAVINFO_USEFJCORE__
#include "fastjet/NNH.hh"
#include "fastjet/PseudoJet.hh"
#endif

#include "fastjet/contrib/FlavInfo.hh"

using fastjet::PseudoJet;
using fastjet::NNH;
using fastjet::contrib::FlavRecombiner;
using fastjet::contrib::FlavInfo;
using fastjet::contrib::FlavHistory;
using std::vector;
using std::numeric_limits;
using std::min;
using std::max;
using std::abs;
using std::log1p;
using std::sqrt;
using std::setw;

struct GHSInfo {
    double alpha;
    double omega;
    unsigned njets;
    vector<PseudoJet> jets;
    FlavRecombiner flav_recombiner;
  };

class GHSBriefJet {
 public:
  void init(const PseudoJet &jet, GHSInfo *info) {
    _jet = jet;
    _info = info;
    _pt2 = jet.pt2();
    double pt = sqrt(_pt2);
    _nx = jet.px() / pt;
    _ny = jet.py() / pt;
    _rap = jet.rap();
    _phi = jet.phi();
    constexpr double rap_transition = 0.1;
    if (fabs(_rap) < rap_transition) {
      // for moderately small rapidities switch to special rapidity formula
      //
      // rap = 1/2 log[(E+pz)/(E-pz)]
      //     = 1/2 log[1 + 2pz/(E-pz)]
      //
      // and use log1p for the latter
      _rap = 0.5 * log1p(2 * jet.pz() / (jet.E() - jet.pz()));
    }
    // now, evalute the beam distance for the PseudoJet
    if (is_jet()) {
      // no beam distance
      _diB = numeric_limits<double>::max();
    } else if (is_particle()) {
      // beam distance is defined only if the particle has no assigned jet
      if (associated_jet() == -1) {
        double ptBp = 0, ptBm = 0;
        for (const auto &j : _info->jets) {
          // _info->jets are PJs not BriefJets (as they must be to have an
          // associated cs) so we must recalculate the precise rapidity here.
          constexpr double rap_transition = 0.1;
          double jrap = j.rap();
          if (fabs(jrap) < rap_transition) {
            jrap = 0.5 * log1p(2 * j.pz() / (j.E() - j.pz()));
          }
          double jetrap = jet.rap();
          if (fabs(jetrap) < rap_transition) {
            jetrap = 0.5 * log1p(2 * jet.pz() / (jet.E() - jet.pz()));
          }
          double dyj = jrap - jetrap;
          ptBp += j.pt() * exp(min(0.0, -dyj));
          ptBm += j.pt() * exp(min(0.0, dyj));
        }
        double alpha = _info->alpha;
        double diBp = max(pow(jet.pt(), alpha), pow(ptBp, alpha)) *
                      min(pow(jet.pt(), 2 - alpha), pow(ptBp, 2 - alpha));
        double diBm = max(pow(jet.pt(), alpha), pow(ptBm, alpha)) *
                      min(pow(jet.pt(), 2 - alpha), pow(ptBm, 2 - alpha));
        _diB = min(diBp, diBm);
      } else {
        _diB = numeric_limits<double>::max();
      }
    } else {
      assert(false &&
             "the PseudoJet should either be a particle or a jet, "
             "but was neither");
    }
  }
  //> idx in jet if associated [0,...,jets.size()];  -1 if unassociated; 0 if
  // jet
  int associated_jet() const { return abs(_jet.user_index()) - 1; }
  bool is_jet() const { return _jet.user_index() == 1; }
  bool is_particle() const { return _jet.user_index() <= 0; }
  static double geometrical_distance_squared(const GHSBriefJet *first,
                                             const GHSBriefJet *other) {
    // do straight rapidity difference, because we already took
    // care of making the rapidity as accurate as possible
    double delta_y = first->_rap - other->_rap;
    double delta_phi = std::fabs(first->_phi - other->_phi);
    if (delta_phi > fastjet::pi) delta_phi = fastjet::twopi - delta_phi;
    // transition is somewhat arbitrary, but should be such that
    // we are in a region where arcsin() is unambiguous; can be
    // O(1), but must be < pi/2
    constexpr double phi_transition = 0.1;
    if (delta_phi < phi_transition) {
      // take a cross product of the n's (normalised), which
      // is simply equal to sin(delta_phi)
      double cross = first->_nx * other->_ny - other->_nx * first->_ny;
      // the sign can come out negative, but this isn't an issue
      // because we will use it in a context where the sign
      // disappears
      delta_phi = asin(cross);
    }
    // omega = 0: we use deltaR^2 explicitly
    double omega = first->_info->omega;
    if (omega == 0.0) {
      return delta_y * delta_y + delta_phi * delta_phi;
    } else {
      double deltaR2 = delta_y * delta_y + delta_phi * delta_phi;
      if (deltaR2 > _deltaR2_handover)
        return 2 * ((cosh(omega * delta_y) - 1) / (omega * omega) -
                    (cos(delta_phi) - 1));
      else
        return deltaR2;
    }
  }
  double distance(const GHSBriefJet *other_in) {
    // make sure that if at least one of them is a particle, then
    // the first is always a particle
    const GHSBriefJet *first = this;
    const GHSBriefJet *other = other_in;
    if (other->is_particle()) std::swap(first, other);
    // this can only happen if both are jets
    if (first->is_jet()) return numeric_limits<double>::max();
    // return the particle-particle distance (REVISIT FLAVOUR SUM?)
    if (other->is_particle()) {
      // give a flav dependence (only opposite flavours can annihilate)
      FlavInfo flavA = this->_jet.user_info<FlavHistory>().current_flavour();
      FlavInfo flavB =
          other_in->_jet.user_info<FlavHistory>().current_flavour();
      int assA = this->associated_jet();
      int assB = other_in->associated_jet();
      if ((flavA.is_flavourless() || flavB.is_flavourless()) &&
          (assA != assB)) {
        return numeric_limits<double>::max();
      }
      FlavInfo flavsum = flavA + flavB;
      _info->flav_recombiner.apply_summation_choice(flavsum);
      if (!flavA.is_flavourless() && !flavB.is_flavourless() &&
          !flavsum.is_flavourless()) {
        return numeric_limits<double>::max();
      } else {
        return dij(first, other);
      }
    } else {
      // ---- other must be a jet -----
      // first check to see if this particle is associated with any jet at all
      // (if not there is no distance)
      if (first->associated_jet() < 0) return numeric_limits<double>::max();
      // then see if it's associated with specifically with other
      if (_info->jets[first->associated_jet()].cluster_hist_index() ==
          other->_jet.cluster_hist_index()) {
        return dij(first, other);
      } else {
        return numeric_limits<double>::max();
      }
    }
  }
  double beam_distance() const { return _diB; }
  static double dij(const GHSBriefJet *first, const GHSBriefJet *other) {
    double alpha = first->_info->alpha;
    double ptf = sqrt(first->_pt2);
    double pto = sqrt(other->_pt2);
    return geometrical_distance_squared(first, other) *
           max(pow(ptf, alpha), pow(pto, alpha)) *
           min(pow(ptf, 2 - alpha), pow(pto, 2 - alpha));
  }
 private:
  PseudoJet _jet;
  GHSInfo *_info;
  static constexpr double _deltaR2_handover =
      pow(std::numeric_limits<double>::epsilon(), 0.5);
  double _diB, _pt2, _rap, _phi, _nx, _ny;
};

// void print_PJ(std::ostream * ostr, const PseudoJet &p, unsigned precision,
//                 bool short_version, bool final_flav) {
//     (*ostr).precision(precision);
//     (*ostr) << setw(precision + 8) << p.px() << setw(precision + 8) << p.py()
//             << setw(precision + 8) << p.pz() << setw(precision + 8) << p.E()
//             << setw(precision + 8) << p.pt() << setw(precision + 8) << p.rap()
//             << setw(precision + 8) << p.phi() << setw(precision + 8) << p.m()
//             << setw(6) << p.user_index();
// #ifdef PRINTMASSFLAVINFO
//     (*ostr) << setw(6) << p.has_user_info<MassFlavHistory>();
// #else
//     (*ostr) << setw(6) << "";
// #endif
//     if (p.has_user_info<FlavHistory>()) {
//       if (final_flav)
//         (*ostr) << setw(14) << FlavHistory::current_flavour_of(p).description();
//       else
//         (*ostr) << setw(14) << FlavHistory::initial_flavour_of(p).description();
//       // cout << json(FlavHistory::current_flavour_of(p)) << endl;
//     }
//     if (!short_version) {
//       (*ostr) << setw(6) << p.cluster_hist_index();
//     }
//   }

/// given a list of base-jets (before applying a hardness cut) in
/// the event (jets_base), return the jets with "dressed" flavour information
///
/// @param jets_base: the input base-jets; the full list of event
/// particles and associated flavours will be deduced from
/// ClusterSequence associated with these jets.
///
/// @param ptcut: this parameter applies a hardness cut on the base-jets
/// and should be chosen to match the fiducial jet definition
///
/// @param alpha: power of (ktmax/ktmin) used in the flavour-kt distance
///
/// @param omega: relative weighting of rapidity separation
///
/// @returns the list of hard jets dressed with their flavour
std::vector<PseudoJet> run_GHS(
    const std::vector<PseudoJet> &jets_base, double ptcut, double alpha = 1.,
    double omega = 2.,
    const FlavRecombiner &flav_recombiner = FlavRecombiner());


#endif  // __FASTJET_CONTRIB_GHSPLUGIN_HH__
