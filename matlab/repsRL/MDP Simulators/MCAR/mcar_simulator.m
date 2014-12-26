function  [nexts, reward, endsim] = mcar_simulator(state,action)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Copyright 2005
%
% Sridhar Mahadevan (mahadeva@cs.umass.edu) 
% 
%
% Department of Computer Science
% Univ of Massachusetts, Amherst
% 
%
% [nextstate, reward, absorb] = mcar_simulator(state, action)
%
% A simulator for the Mountain car domain.#define GRAVITY -0.0025  // acceleration due to gravity 

GOAL = 0.5;  % above this value means goal reached 
GRAVITY = -0.0025;  % acceleration due to gravity 

POS_RANGE = [-1.2, 0.5]; 
VEL_RANGE =  [-0.07,0.07]; 

if nargin<1

    % Initialize

    posvals = [-1.2:0.02:0.5];
    posvals2 = [-0.6:01:-0.4];
    velvals = [-0.07:0.01:0.07];

%             nexts = [ 0 0 ];%
%       nexts = [ -0.5 0 ];


      nexts = [-0.5, velvals(random('unid',length(velvals)))];
%         nexts = [posvals(random('unid',length(posvals))), velvals(random('unid',length(velvals)))]; 
     
    reward = 0;
    endsim = 0;
    
%     fprintf('initial state is %f pos %f vel\n', nexts(1), nexts(2)); 
    
    return
    
  elseif nargin<2
    
    % Set state
    
    nexts = state;
    reward = 0;
    endsim = 0;
    return
    
end
  
% if (action == 0)  % go backwards 
% 	 aval = -1; 
% else  aval =  action;  
% end; 
% 

if (action == 1)  % coast
    aval = 0;
elseif (action == 2) % forward
    aval = 1;
else aval =  -1;  %  backward
end;


%update equation for velocity 

oldv = state(2); 
oldp = state(1); 

newv = oldv + (0.001 * aval) + (GRAVITY * cos(3 * oldp));

if (newv < VEL_RANGE(1))  % clip velocity if necessary to keep it within range
    newv = VEL_RANGE(1);
else if (newv > VEL_RANGE(2))
        newv = VEL_RANGE(2);
    end;
end; 
			
newp = oldp + newv;  % update equation for position

if (newp < POS_RANGE(1)) % clip position and velocity if necessary to keep it within range
    newp = POS_RANGE(1);
    newv = 0;  % reduce velocity to 0 if position was out of bounds
elseif (newp > POS_RANGE(2))
        newp = POS_RANGE(2);
        newv = 0;
end;

nexts = [newp,newv];    % update state to new values 

% fprintf('position: %f vel: %f \n', newp, newv); 

if (nexts(1) >= GOAL)  %over the hill!
    endsim = 1;
%      disp('reached the top!');
    reward = 0;
else
    reward = -1;
    endsim = 0;
end;
        
return
  
