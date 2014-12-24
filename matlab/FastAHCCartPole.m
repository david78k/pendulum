disp('Begining.... RLS_TD_CartPole_Final');
%--------------learning parameter definition------------
beta = 0.5; %learning rate of actor
gamma = 0.95;  %discount rate
lambda = 0.6;  %td lambda
mu = 1;     %fogetting rate of RLS
eta = 0.2;  %to initialize P0 = eta * I
k1 = 0.4;   k2 = 0.5;   %param for devariance of normal distribution
%-------------CartPole system definition--------------------------
g=9.8;	length=0.5;
m_cart=1.0;		m_pend=0.1;
total_m = m_cart + m_pend;
momt_pend = m_pend*length;
mu_c = 0.0005;	mu_p = 0.000002;    %friction coefficiency
num_state = 4;
tstep = 0.02;   %time step for simulation

initrange = 0.05;
deg2rad = pi / 180;     rad2deg = 180 / pi; %unit conversion
lower_range = [-12*deg2rad -50*deg2rad -2.4 -1];
higher_range = [12*deg2rad 50*deg2rad 2.4 1];
Max_Theta = 12*deg2rad; Max_Pos = 2.4;
%--------------CMAC network custimization---------------
num_c_tiling = 4;	num_c_partition = 7;	num_c_input = 4; 	num_c_memory = 30; %critic
num_a_tiling = 4;	num_a_partition = 7;	num_a_input = 4;	num_a_memory = 80; %actor
%-----------------Initialization of CMAC network---------------------------
c_interval = (higher_range - lower_range) / num_c_partition;    %intervals for a tiling
c_offset = (0:(num_c_tiling-1)) / num_c_tiling;
tempmx1 = [];   tempmx2 = [];   %temporary matrix variable
for ii = 1:num_c_input
    tempmx1 = [tempmx1 c_offset'];
end
for ii = 1:num_c_tiling
    tempmx2 = [tempmx2; c_interval];
end
c_interval = tempmx2;   %intervals for all tilings
c_offset = tempmx1 .* c_interval;  %finish defining the CMAC tiling's offset
c_num_tiles_per_tiling = num_c_partition^(num_c_input); %number of unit of a tiling

a_interval = (higher_range - lower_range) / num_a_partition;
a_offset = (0:(num_a_tiling-1)) / num_a_tiling;
tempmx1 = [];   tempmx2 = [];   %temporary matrix variable
for ii = 1:num_a_input
    tempmx1 = [tempmx1 a_offset'];
end
for ii = 1:num_a_tiling
    tempmx2 = [tempmx2; a_interval];
end
a_interval = tempmx2;
a_offset = tempmx1 .* a_interval;  %finish defining the CMAC tiling's offset
a_num_tiles_per_tiling = num_a_partition^(num_a_input);

%----------------Allocate the matrix of variables-----------------
c_state = zeros(1,num_c_input);
c_active = zeros(num_c_tiling, num_c_input);
a_state = zeros(1,num_a_input);
a_active = zeros(num_a_tiling, num_a_input);

MaxRuns = 1;
for run = 1:MaxRuns
%-----------------Initialization of variables for each run----------------
    c_weight = zeros(num_c_memory,1);
    a_weight = zeros(num_a_memory,1);
    zt = zeros(num_c_memory,1);     %eligibility trace of critic
    P = eta*eye(num_c_memory);      %RLS variable,  variance matrix
	%initialize System state
    s = rand(1,num_state)*initrange;
	theta = s(1);	theta_vel = s(2);	pos = s(3);		vel = s(4);
	
    trials = 1;
    steps = 0;
    %for recording 
        xx = [];
        ff = [];
    MaxTrials = 200;
	while trials <= MaxTrials
		%----------Calculation of actor output---------------
		a_state = [theta theta_vel pos vel] - lower_range;        
        tempmx1 = [];
        for ii = 1:num_a_tiling
            tempmx1 = [tempmx1; a_state];
        end
		a_active = ceil((tempmx1 - a_offset) ./ a_interval);    %active tile in each tiling
        %Hashing of CMAC network
		a_feature = zeros(num_a_memory,1);
		aa = 0;
		for i = 1:num_a_tiling
            aa = (i-1) * a_num_tiles_per_tiling;
			for j = 1:num_a_input
                if a_active(i,j) <= num_a_partition & a_active(i,j) > 0
    				aa = aa + a_active(i,j) - 1 + num_a_partition^(num_a_input - j);
                end
			end
			aa = rem(aa, num_a_memory) + 1;
			a_feature(aa) = 1;
		end
		y_hat = a_feature' * a_weight;
		
		%------------------Calculation of V(s(t))---------------------------------------
		c_state = [theta theta_vel pos vel] - lower_range;
		tempmx1 = [];
        for ii = 1:num_c_tiling
            tempmx1 = [tempmx1; c_state];
        end
		c_active = ceil((tempmx1 - c_offset) ./ c_interval);
		c_feature = zeros(num_c_memory,1);
		aa = 0;
		for i = 1:num_c_tiling
            aa = (i-1) * c_num_tiles_per_tiling;
			for j = 1:num_c_input
                if c_active(i,j) <= num_c_partition & c_active(i,j) > 0
    				aa = aa + c_active(i,j) - 1 + num_c_partition^(num_c_input - j);
                end
			end
			aa = rem(aa, num_c_memory) + 1;
			c_feature(aa) = 1;
        end		
        
        
		V_t = c_feature' * c_weight;
		
		%calculate according to distribution
		sigma = k1 / (1 + exp(k2*V_t));
		y_t = randn(1,1)*sigma + y_hat;		%actual control action
		
        %---------------Simulation of CartPole system, using Euler method-------------------
        force = y_t * 10;
		if force > 10
			force = 10;
		elseif force < -10
		    force = -10;
        end
		hforce = momt_pend * (theta_vel^2) * sin(theta);
		part_num = -force - hforce + mu_c * sign(vel);
		denom_ang_vel = length * (4/3 - m_pend * ((cos(theta))^2) / total_m);
		num_ang_vel = g*sin(theta) + cos(theta) * part_num / total_m - mu_p * theta_vel / momt_pend;		
		dxdt1 = theta_vel;
		dxdt2 = num_ang_vel / denom_ang_vel;
		num_vel= force + hforce - momt_pend * dxdt2 * cos(theta) - mu_c * sign(vel);		
		dxdt3 = vel;
		dxdt4 = num_vel / total_m;        
		theta = theta + tstep * dxdt1;
		theta_vel = theta_vel + tstep * dxdt2;
		pos = pos + tstep * dxdt3;
		vel = vel + tstep * dxdt4;
        
        %-------------Generate the reward---------------------------------------------------
        if abs(theta) > Max_Theta | abs(pos) > Max_Pos
            rt = -1;
            failed = 1;
        else
            rt = 0;
            failed = 0;
        end
        %--------------Calculation of V(s(t+1))--------------------------------------------
        if failed == 0
			c_state = [theta theta_vel pos vel] - lower_range;			
            tempmx1 = [];
            for ii = 1:num_c_tiling
                tempmx1 = [tempmx1; c_state];
            end
			c_active = ceil((tempmx1 - c_offset) ./ c_interval);
			c_feature_new = zeros(num_c_memory,1);
			aa = 0;
			for i = 1:num_c_tiling
	            aa = (i-1) * c_num_tiles_per_tiling;
				for j = 1:num_c_input
	                if c_active(i,j) <= num_c_partition & c_active(i,j) > 0
	    				aa = aa + c_active(i,j) - 1 + num_c_partition^(num_c_input - j);
	                end
				end
				aa = rem(aa, num_c_memory) + 1;
				c_feature_new(aa) = 1;
	        end		
			V_t_new = c_feature_new' * c_weight;
		else    %if fails, V(s) = 0
			V_t_new = 0;
			c_feature_new = zeros(num_c_memory,1);
		end
        
        %----------Eligibility trace update -------------------------
        zt = gamma*lambda*zt + c_feature;
        %----------TD error------------------------------------------
        r_hat = rt + gamma * V_t_new - V_t;
        %----------Critic update, using RLS_TD(lambda) method--------
        FeatureVector = c_feature' - gamma*c_feature_new';
        tt2 = FeatureVector * P * zt;
        K = P * zt / (mu + tt2);
        c_weight = c_weight + K * r_hat;
        P = P -K*FeatureVector*P;
        P = P / mu;        
        % ---------Critic update, using Kernel LS-TD
        
        
        
        %----------Actor update, using policy gradient
        policygradient = r_hat * (y_t - y_hat) / sigma;
        a_weight = a_weight + beta * policygradient * a_feature / num_a_tiling;
        %----------Next state---------------
        steps = steps + 1;
        ss = [theta theta_vel pos vel];
        xx = [xx; ss];
        ff = [ff; force];
        
        %if failed
        if failed == 1
            if trials == MaxTrials
                figure
                subplot(5,1,1);
                plot(xx(:,1)*rad2deg);
                subplot(5,1,2);
                plot(xx(:,2)*rad2deg);
                subplot(5,1,3);
                plot(xx(:,3));
                subplot(5,1,4);
                plot(xx(:,4));
                subplot(5,1,5);
                plot(ff);
            end
            %------Re-initilization of CartPole state
            s = rand(1,num_state)*initrange;
        	theta = s(1);	theta_vel = s(2);
        	pos = s(3);		vel = s(4);            
            zt = zt*0;  %clear eligibility trace
            xx = [s];
            ff = [];
            if MaxRuns > 1
                if rem(trials,20) == 0 
                    disp(['Trial #' num2str(trials) ' has ' num2str(steps) ' steps...']);
                end
            else
                disp(['Trial #' num2str(trials) ' has ' num2str(steps) ' steps...']);
            end
            steps = 0;
            trials = trials + 1;
        elseif steps >= 12000
            disp(['Success: Trial #' num2str(trials) ' has ' num2str(steps) ' steps...']);
            figure
            subplot(5,1,1);
            plot(xx(:,1)*rad2deg);
            ylabel('theta-degrees');
            subplot(5,1,2);
            plot(xx(:,2)*rad2deg);
            ylabel('theta-dot');
            subplot(5,1,3);
            plot(xx(:,3));
            ylabel('position');
            subplot(5,1,4);
            plot(xx(:,4));
            ylabel('position-dot');
            subplot(5,1,5);
            plot(ff);
            ylabel('force');
            break;      %end training
        elseif rem(steps,10000) == 0
            disp(['It is ' num2str(steps) ' step now...']);
        end
    end %end of "while "
end    