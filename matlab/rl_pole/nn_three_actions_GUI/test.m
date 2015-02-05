function test

% clear all;

xpoints = [];
ypoints = [];

for tv = 1:5,
subplot(2,1,2);
xpoints(tv) = tv;
ypoints(tv) = tv;
plot(xpoints,ypoints)
% disp(['success'])
end