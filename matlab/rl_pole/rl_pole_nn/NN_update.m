function [w, v, e, xbar] = nn_update(N_BOXES, LAMBDAw, LAMBDAv, ALPHA, BETA, w, v, e, xbar, rhat, failed)

for i=1:N_BOXES,
    %Update all weights.
    w(i) = w(i)+ ALPHA * rhat * e(i);
    v(i) = v(i)+ BETA * rhat * xbar(i);
    
    if (v(i) < -1.0)
        v(i) = v(i);
    end
    
    if (failed)
        %If failure, zero all traces.
        e(i) = 0.;
        xbar(i) = 0.;
        
    else
        e(i)=e(i) * LAMBDAw;
        xbar(i) =xbar(i)* LAMBDAv;
    end
    
end