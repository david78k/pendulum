function D = pdistc(X, Y, distance)

nx = size(X, 1);
ny = size(Y, 1);
D = zeros(nx, ny);

if ((nx > 0) && (ny > 0))
  for i = 1 : nx
    x = X(i, :);
    D(i, :) = ...
      sum(abs(ones(ny, 1) * x - Y) .^ distance, 2) .^ (1 / distance);
  end
end
