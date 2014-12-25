function m = meanNoNaN(cols)
% meanNoNan.  Like mean, but disregards NaN's.
mask = ~isnan(cols);

m = [];
for i = 1:size(cols,2)
 m = [m mean(cols(mask(:,i),i))];
end
