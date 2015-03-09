function nze = ebpbc_pde(J,R,g)
%EBPBC_PDE Calculates on which states energy can be shaped
%
% NZE = EBPBC_PDE(J,R,G) Calculates the partial differential equation (PDE) used
% in Energ-Balancing Passivity-Based Control for given system matrices J, R
% and G and returns the index of states where energy can be shaped
%
% INPUTS:   J: [n x n] matrix containing the interconnection structure for
%           which it holds that J(x) = -J(x)^T
%           
%           R: [n x n] matrix containing the damping matrix for
%           which it holds that R(x) = R(x)^T > 0
%
%           G: [n x m] input matrix
%
% OUTPUTS:  NZE: number of zero elements in null space of PDE, corresponding to
%           the states for which the energy can be shaped.
%

% Calculate full-rank left annihilator g^perp
% THIS IS NOT WORKING PROPERLY YET
[n, m]      = size(g);
g_perp      = eye(n-m,n);

[row, ~]    = find(g_perp*g ~= zeros(n-m,m));

zero_col    =  sum(g_perp) == 0;
if ~isempty(row)
    for i=1:length(row)
        g_perp(row(i),row(i)) = 0;       
        g_perp(row(i),zero_col) = 1;
    end
end

% Calculate null-space basis for PDE
Z       = null([g_perp*(J-R)'; g']);

% Find non-zero elements
nze     = find(sum(Z,2) ~= 0);

