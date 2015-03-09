function [pb alpha] = fourgen(BF)
%FOURGEN Generates frequency matrix and scaled learning rates for the
%        Fourier Basis function approximation. See also four
% 
% [PB ALPHA] = FOURGEN(BF) Calculates all possible integer frequency 
% combinations of states using the Fourier Basis approximation scheme, the 
% learning rate vector and the scaling of each state.
%
% INPUTS:   BF.N: order of basis functions (typically 3-9)
%           BF.a: learning rate: 1x1 scalar
%           BF.r: matrix containing domain of x-values
%           BF.
%
% OUTPUTS:  PB: matrix containing all possible integer frequency combinations of
%           states for the Fourier basis function approximation
%           ALPHA: matrix or vector containing learning rates for each
%           basis function

% Calculate number of state variables
[BF.n ~]  = size(BF.r);

% Construct vector containing frequencies
c = zeros(BF.N+1,1);
for i=1:BF.N
    c(i+1) = i;
end

% Compute all possible integer frequency combinations for the size of the
% state vector
nobf = (BF.N+1)^BF.n;
pb = zeros(nobf,BF.n);
[sets{1:BF.n}] = ndgrid(c(1):c(end));
for i=1:BF.n
    for j=1:nobf
        pb(j,i) = sets{i}(j);
    end
end

% Compute the 2-norm of all frequency combinations
norm_pb         = zeros(nobf,1);
for i=1:nobf
    norm_pb(i) = norm(pb(i,:));
end

% Set the first norm non-zero to avoid singularities
norm_pb(1)              = BF.a(1);

% Calculate learning rate vectors based on frequency vector norms
alpha                   = BF.a./norm_pb;
alpha(1)                = BF.a;










