function Plot_Neuron(ind, allV, allE, allM, allN, allH, allG, time, ...
    Q_applied, Q0, typeStimuli, spikes, tit)
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
global E_reverse

nIter = size(allV, 2);

figure;
set(gcf, 'Color', [1 1 1]);
subplot(4, 1, 1); hold on;

% Plot the neuron voltage and stimuli
plot(time, allE(ind,:), 'y:');
plot(time, allV(ind,:), 'k-');
plot(time, (E_reverse(ind) - E_rest) * allG(ind,:), 'Color', [0.8 0.4 0.7]);
stimuli = find(Q_applied(ind, :));
plot(time([1 nIter]), E_Na * [1 1], 'r:');
plot(time([1 nIter]), E_K * [1 1], 'g:');
plot(time([1 nIter]), E_L * [1 1], 'b:');
plot(time([1 nIter]), E_rest * [1 1], 'c:');
plot(time([1 nIter]), [0 0], 'k:');

% Plot the spikes
for k = 1:length(spikes{ind})
    plot(time(spikes{ind}(k)) * [1 1], [-100 1000], 'r--');
end

% Set the graph limits
set(gca, 'XLim', time([1 nIter]));
set(gca, 'YTick', [-100 E_K E_L 0 E_Na 100:100:1000]);
set(gca, 'YLim', [-100 max([100, max(allV(ind, :))])]);

% Title, legend and labels
xlabel('Time in msec');
ylabel('Internal neuron voltage in mV');
str = sprintf('Experience "%s": neuron %d', tit, ind);
if length(stimuli) > 0
    str = sprintf('%s (stimulated with Q_0=%g \\muA "%s")', ...
        str, Q0, typeStimuli);
end
leg = legend({'E', 'V', 'E_{syn}', 'E_{Na}', 'E_K', 'E_L', 'E_{rest}'});
pos = get(leg, 'Position');
pos(1) = 0.85;
pos(2) = 0.65;
set(leg, 'Position', pos);
title(str);

subplot(4, 1, 2); hold on;

% Plot the m, h, n fractions of ion channel open subunits
plot(time, allM(ind,:), 'r-');
plot(time, allN(ind,:), 'g-');
plot(time, allH(ind,:), 'b-');
plot(time([1 nIter]), [0 0], 'k:'); plot(time([1 nIter]), [1 1], 'k:');

% Set the graph limits
set(gca, 'XLim', time([1 nIter]));
m_frac = min([allM(ind, :) allN(ind, :) allH(ind, :)]);
M_frac = max([allM(ind, :) allN(ind, :) allH(ind, :)]);
set(gca, 'YLim', [m_frac M_frac]);

% Title, legend and labels
leg = legend({'m', 'n', 'h'});
pos = get(leg, 'Position');
pos(1) = 0.85;
set(leg, 'Position', pos);
xlabel('Time in msec');

subplot(4, 1, 3); hold on;

% Plot the rate of firing (in Hz)
M = 0;
x1 = 0; i1 = 1;
for k = 1:length(spikes{ind})
    i0 = i1;
    x0 = x1;
    i1 = spikes{ind}(k);
    x1 = time(i1);
    f = 1000/(x1 - x0);
    plot([x0 x1], f * [1 1], 'k-');
    if k > 1
        M = max(M, f);
    end
end

% Set the graph limits
set(gca, 'XLim', time([1 nIter]));
if (M == 0), M = 1; end
set(gca, 'YLim', [0 M]);

% Labels
xlabel('Time in msec');
ylabel('Rate of neuron firings in Hz');

subplot(4, 1, 4); hold on;

indStimuli = find(Q_applied(ind, :));
if length(indStimuli) > 0
    plot(time(indStimuli), Q_applied(ind, indStimuli), 'r.');
    set(gca, 'XLim', time([1 nIter]));
else
    set(gca, 'XTick', []);
    set(gca, 'YTick', []);    
end
ylabel('Stimuli');
xlabel('Time in msec');

% Save the figure
orient landscape
saveas(gcf, [tit '_Neuron' num2str(ind) '_TimePlot.pdf']);
