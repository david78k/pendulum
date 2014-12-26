function initial_state = pendulum_initialize_state(simulator)
  
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Copyright 2000-2002 
%
% Michail G. Lagoudakis (mgl@cs.duke.edu)
% Ronald Parr (parr@cs.duke.edu)
%
% Department of Computer Science
% Box 90129
% Duke University
% Durham, NC 27708
% 
%
% initial_state = chain_initialize_state(simulator)
%
% Initializes the state of the chain
% It picks one state uniformly in random
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
  
  initial_state = feval(simulator);
  noise = (2*rand(1,2)-1) .* [ 0.2   0.2 ] ;
  initial_state = initial_state + noise;
  
  return
