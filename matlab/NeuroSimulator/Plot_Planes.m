function Plot_Planes(ind, allV, allM, allN, allH, ...
    Q_applied, Q0, typeStimuli, tit)
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

global E_Na
global E_K
global E_L
global E_rest
global g_Na
global g_K
global g_L

figure; hold on;
set(gcf, 'Color', [1 1 1]);

% Compute the fractions of open subunits in ion channel at t=infinity
Vs = [E_K:0.1:E_Na];
m_inf = Alpha_m(Vs) ./ (Alpha_m(Vs) + Beta_m(Vs));
h_inf = Alpha_h(Vs) ./ (Alpha_h(Vs) + Beta_h(Vs));
n_inf = Alpha_n(Vs) ./ (Alpha_n(Vs) + Beta_n(Vs));

% Solve for f(n, v) = 0 for each v
indF = 1:10:length(Vs);
f_N = zeros(1, length(indF));
manyN = [0:0.001:1];
for j = 1:length(indF)
    k = indF(j);
    thisV = Vs(k);
    thisM = m_inf(k);
    thisH = m_inf(k);
    f = abs(g_Na * thisM^3 .* (1 - manyN) * (thisV - E_Na) + ...
        g_K * manyN.^4 * (thisV - E_K) + g_L * (thisV - E_L)');
    [val, indmin] = min(f);
    f_N(j) = manyN(indmin);
end

% Plot the curves in the (v,s) planes
plot(allV(ind, :), allM(ind, :), 'r-');
plot(allV(ind, :), allH(ind, :), 'm-');
plot(allV(ind, :), allN(ind, :), 'g-');
plot(Vs, m_inf, 'r:', 'LineWidth', 2);
plot(Vs, h_inf, 'm:', 'LineWidth', 2);
plot(Vs, n_inf, 'g:', 'LineWidth', 2);

% Plot the curve f(n,v)=0
plot(Vs(indF), f_N, 'k.');

% Set the graph limits
set(gca, 'XTick', [-100 E_K E_L 0 E_Na 100:100:1000]);
m_frac = min([allM(ind, :) allN(ind, :) allH(ind, :)]);
M_frac = max([allM(ind, :) allN(ind, :) allH(ind, :)]);
set(gca, 'YLim', [m_frac M_frac]);

% Labels, legend and title
leg = legend({'m (fast, Na)', 'h (slow, Na)', 'n (slow, K)', ...
    'm_{inf}', 'h_{inf}', 'n_{inf}', 'f(n,v)=0'});
pos = get(leg, 'Position');
pos(1) = 0.8;
pos(2) = 0.1;
set(leg, 'Position', pos);
xlabel('Voltage in mV');
ylabel('Fraction of ion channel subunits in open state');
stimuli = find(Q_applied(ind, :));
str = sprintf('Experience "%s": neuron %d', tit, ind);
if length(stimuli) > 0
    str = sprintf('%s (stimulated with Q_0=%g \\muA "%s")', ...
        str, Q0, typeStimuli);
end
title(str);

% Save the figure
saveas(gcf, [tit '_Neuron' num2str(ind) '_VSPlane.pdf']);

