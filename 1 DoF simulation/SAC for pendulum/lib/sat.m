function  [uc ducdu] = sat(u, par, Delta_u)
%SAT Generates saturated (clipped) input UC
%
% [UC DUCDU] = SAT(U, PAR, Delta_u) Calculates the saturation of an input scalar U
% between the bounds PAR.MAX and PAR.MIN for a given saturation type
% PAR.SATTYPE subject to a perturbation Delta_u
%
% INPUTS:   U: input (scalar)
%           PAR: structure containing necessary bounds
%               par.max: max-bound
%               par.min: min-bound
%               par.skew: skewness of saturation. (for 'atan' and
%               'tanh')
%               par.sattype: type of saturation. Choose from 'plain', 'atan' or
%               'tanh'.
%           Delta_u: exploratory action
%
% OUTPUT:   UC: clipped control action.
%
%           DUCDU: Policy gradient saturation part. Please note that the 
%           policy gradient should be zero when in the saturated
%           region. Hence, the term ducdu is introduced as the chain-rule derivative
%           part coming from the saturation function applied to the policy. The 
%           exploratory action is NOT included in this partial derivative. This
%           is because we need to evaluate the policy gradient and its derivative -
%           also with respect to the saturation function - at the
%           calculated deterministic control policy pi, and not at the
%           perturbed input u.

if strcmp(par.sattype,'plain')
    % Plain saturation, i.e. u_min <= uc <= u_max
    uc = max(par.min, min(par.max, u + Delta_u));
    % Policy gradient saturation part
    if u <= par.max && u >= par.min
        ducdu = 1;
    else
        ducdu = 0;
    end
elseif strcmp(par.sattype,'atan')
    % Arctangent saturation.
    uc = (par.max/(0.5*pi)*atan(par.skew*(u+Delta_u)));
    % Policy gradient saturation part
    ducdu = ((par.max/(0.5*pi))*par.skew)/(par.skew^2*u^2+1);
elseif strcmp(par.sattype,'tanh')
    % Tangent hyperbolic saturation.
    uc = par.max*tanh(par.skew*(u+Delta_u));
    % Policy gradient saturation part
    ducdu = par.max*par.skew-par.max*par.skew*tanh(par.skew*u)^2;
else
    error('Choose from "plain", "atan" or "tanh"')
end