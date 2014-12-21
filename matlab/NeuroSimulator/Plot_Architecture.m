function fig = Plot_Architecture(nX, nY, synapses, stimuli, tit)
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

global E_reverse
global E_rest

fig = figure; hold on;
plot(-1e3, -1e3, 'go', 'LineWidth', 2);
plot(-1e3, -1e3, 'ro', 'LineWidth', 2);
plot(-1e3, -1e3, 'b*');

nNeurons = nX * nY;
for y1 = 1:nY
    for x1 = 1:nX
        i = x1 + (y1 - 1) * nX;
        for y2 = 1:nY
            for x2 = 1:nX
                j = x2 + (y2 - 1) * nX;
                s = synapses(j, i) * E_reverse(i);
                DrawSynapse(x1, y1, x2, y2, s);
            end
        end
    end
end
for y = 1:nY
    for x = 1:nX
        i = x + (y - 1) * nX;
        if E_reverse(i) > E_rest
            plot(x, y, 'go', 'MarkerSize', 32, 'LineWidth', 2);
        else
            plot(x, y, 'ro', 'MarkerSize', 32, 'LineWidth', 2);
        end
        if ~isempty(find(stimuli(i, :)))
            plot(x, y, 'b*', 'MarkerSize', 12);
        end
        text(x, y, num2str(i), 'FontSize', 12, 'FontWeight', 'bold');
    end
end
set(gcf, 'Color', [1 1 1]);
axis equal;
set(gca, 'XLim', [0.5, nX+0.5]);
set(gca, 'YLim', [0.5, nY+0.5]);
set(gca, 'XTick', []);
set(gca, 'YTick', []);
title(['Experience "' tit '": neural architecture']);
h = legend({'Excitatory neurons', 'Inhibitory neurons', 'Stimulated neurons'});
pos = get(h, 'Position');
pos(1) = 0.7;
pos(2) = 0.1;
set(h, 'Position', pos);

% Save the figure
saveas(gcf, [tit '_Architecture.pdf']);

% -------------------------------------------------------------------------
function DrawSynapse(x1, y1, x2, y2, dir)

global E_rest
if (dir == 0), return;
else
    if (dir > E_rest), col = 'g:';
    else, col = 'r--';
    end
end
plot([x1 x2], [y1 y2], col, 'LineWidth', 2);
plot(x2 - (x2 - x1)/7, y2 - (y2 - y1)/7, [col '.'], ...
    'MarkerSize', 32, 'LineWidth', 6);
