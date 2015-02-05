function [e, xbar] = nn_update_traces(LAMBDAw, LAMBDAv, e, xbar, push, box)

    %Update traces.
    e(box)= e(box) + (1.0 - LAMBDAw) * (push - 0.5);
    xbar(box) =xbar(box)+ (1.0 - LAMBDAv);