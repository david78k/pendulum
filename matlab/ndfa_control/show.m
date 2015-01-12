function frames = show(history, export),
% SHOW  Plots a visual presentation of the cart-pole system.
%
%  SHOW(history, export)
%
%  Animates the cart-pole system. History is the history returned
%  by CONTROL, either in the current structure format or the legacy
%  five-row format.
%


% Copyright (C) 2004-2005 Matti Tornio
%
% This package comes with ABSOLUTELY NO WARRANTY; for details
% see License.txt in the program package.  This is free software,
% and you are welcome to redistribute it under certain conditions;
% see License.txt for details.

% By default figure is not prepared for exporting
if nargin < 2,
  export = false;
end

frames = [];

% Extract data from the history
if isstruct(history),
  xv = history.data(1,:);
  thv = history.data(3,:);
  F = history.control;
else
  % Support for data from legacy versions
  xv = history(1,1:end-1);
  thv = history(3,1:end-1);
  F = history(5,1:end-1);
end

xr = 1:size(xv, 2);
figure(12), clf, hold on;
axis([1 xr(end) -10 10]);
plot(xr, xv, 'kx-', xr, thv-pi, 'k.-', xr, F, 'k-');
plot(xr, 0, 'k--');
xlabel('time t'); % ylabel('observations x(t)');
legend('y (m)', '\phi (rad)', 'F (N)', 0);
legend boxoff;
hold off;

if export == 2,
  fps = 20;
end

figure(13), clf, hold on;
for i=1:size(xv,2),
  if export ~= 1,
    clf;
  end
  % Set suitable axis for the cart-pole simulation
  axis([-3.2 3.2 -1.5 1.5]);
  fill([-3.2 3.2], [-1.5 1.5], 'w');
  axis off;
  hold on;

  % Extract current sample
  x = xv(i);
  th = thv(i);

  % Plot the cart
  cartx = ones(5,1) * x + [-.2;-.2;.2;.2;-.2];
  carty = [-.1;.1;.1;-.1;-.1];
  plot(cartx, carty, 'k')

  % Plot the ground
  plot([-3.2 -3.2 3.2 3.2], [.1 -.1 -.1 .1], 'k');

  % Plot the pole
  plot([x x+sin(th+pi)], [0 cos(th+pi)], 'k');

  % Plot the force
  if ~isempty(F) & export ~= 1,
    plot([x x+F(i)/10], [0 0], 'r');
  end
  
  % Plot the time
  if mod(i, 5) == 0 && export ~= 2,
    text(x+1.1*sin(th+pi), 1.1*cos(th+pi), int2str(i));
  end
    
  if export == 2,
    frames(i) = getframe;
  end
  
  pause(.05);
end

if export == 2,
  for i = 1:size(xv,2),
    M(:,:,1,i) = uint8(sum(frames(i).cdata, 3) / 3);
  end
  imwrite(M, 'test.gif', 'Delaytime', .05);
end

hold off;
