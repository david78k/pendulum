function res = Beta_m(V)
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

res = 4 * exp(-(V + 70)/18);
