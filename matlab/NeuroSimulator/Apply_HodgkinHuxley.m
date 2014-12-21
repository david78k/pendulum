function [V, dV, g, E, m, h, n, G_syn] = ...
    Apply_HodgkinHuxley(V, dV, g, E, m, h, n, Q_applied, G_syn)
%
% Internal function used by NEUROSIMULATOR.
% Computer simulation of a network of conductance-based single-compartment
% neurophysiological model neurons, under either deterministic
% or stochastic external current stimulation, or deterministic synaptic
% transmission, using the Hodgkin-Huxley models.
%
% Version 1.0, New York, 20/12/2006
% (c) 2006, Piotr Mirowski,
%     Ph.D. candidate at the Courant Institute of Mathematical Sciences
%     Computer Science Department
%     New York University
%     719 Broadway, 12th Floor, New York, NY 10034, USA.
%     email: mirowski [AT] cs [DOT] nyu [DOT] edu
%
% References:
%     Charles Peskin,
%     Lecture notes in Mathematical Aspects of Neurophysiology
%     Courant Institute of Mathematical Sciences, New York University
%     http://math.nyu.edu/faculty/peskin/
%
%     A. L. Hodkin, A. F. Huxley,
%     "A Quantitative Description of Membrane Current and its Application
%     to Conduction and Excitation in Nerve",
%     Journal of Physiology, vol. 117, pp.500-544, 1952.
%
% This program is free software; you can redistribute it and/or modify it
% under the terms of the GNU General Public License as published by
% the Free Software Foundation. This program is distributed in the hope
% that it will be useful, but WITHOUT ANY WARRANTY; without even the
% implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
% See the GNU General Public License for more details. You should have 
% received a copy of the GNU General Public License along with this 
% program; if not, write to the Free Software Foundation, Inc., 
% 59 Temple Place, Suite 330, Boston, MA 02111 USA.
%
% Please report bugs and ask for other details to:
% mirowski [AT] cs [DOT] nyu [DOT] edu
% For my personal webpage at New York University, and other projects and
% software developed, please have a look at: www.mirowski.info .

global C
global dT

% Update G_syn at time t+1/2 (or t+1) from dV at time t
G_syn = f_syn(dV/dT, G_syn);

% Update m, h, n at time t+1/2 from m, h, n at time t-1/2
m = UpdateFractionAlphaBeta(m, Alpha_m(V), Beta_m(V));
h = UpdateFractionAlphaBeta(h, Alpha_h(V), Beta_h(V));
n = UpdateFractionAlphaBeta(n, Alpha_n(V), Beta_n(V));

% Use updated G_syn, m, h, n to update g, E at time t+1/2
[g, E] = UpdateGE(m, h, n, G_syn, V);

% Compute dV/dt at time t+1 and update V(t+1)
% We use g(t+1/2), E(t+1/2) and V(t)
gdT = g .* dT;
V_next = ((C - gdT / 2) .* V + gdT .* E + Q_applied) ./ (C + gdT / 2);

% Update dV and V at time t+1
dV = V_next - V;
V = V_next;

% -------------------------------------------------------------------------
function s = UpdateFractionAlphaBeta(s, alpha, beta)
global dT
alphaPlusBetaDT2 = (alpha + beta) * dT / 2;
s = (s .* (1 - alphaPlusBetaDT2) + alpha * dT) ./ (1 + alphaPlusBetaDT2);

% -------------------------------------------------------------------------
function s = UpdateFractionTauInf(s, tau, infval)
global dT
s = s + dT / tau * (infval - s);

% -------------------------------------------------------------------------
function G_syn = f_syn(dV, G_syn)
global dT
% We simplify the synaptic conductance by a sigmoid function that
% takes value 1 at -infinity and 0 at +infinity, crossing 1/2 at
% a negative value
% We impose it to be smaller than 1e-3 at 0, i.e. we shift by 5
% (knowing that dV/dT has a steeper slope than -5 during the end of the AP)
G_new = -(tanh(dV+5)/2 - 0.5);
% Since at the very end of the AP, there is a strong second order
% derivative, and in order to avoid G_syn to have steep variations,
% we add to it a condensator-like behaviour with empirical coefficients
G_syn = G_syn + (1 - G_syn) .* (G_new - G_syn) * dT / 6;
