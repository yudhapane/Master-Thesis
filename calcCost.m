function r = calcCost(state, input, params)
%calcCost calculate immediate cost
%
%   r = calcCost(state, input, params) calculates immediate cost/reward
%       for applying input at state. The matrices are defined in params
% 
% Copyright 2015 Yudha Pane
    ref = params.ref;
    r = -(state-ref)'*params.Q*(state-ref) - input*params.R*input;