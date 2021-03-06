function y = rbf(x, params)
%RBF evaluate a radial basis function 
%
%   y = rbf(x, params) evaluate a certain state x for radial basis functions with
%       paramaters defined in params
% 
% Copyright 2015 Yudha Pane
if ~isvector(x)
        error('the input must be a vector');
    end
    if isrow(x)
        x = x';
    end
    
    % get parameters
    c = params.c;                       % mean
    B = params.B;                       % variance matrix
    N = params.NrbfX*params.NrbfY;      % no of rbfs
    
    % pre-process input and parameter
    x = repmat(x, [1, N]);
    
    % efficient matrix multiplication of (x-c)'inv(B)(x-c)
    temp = (x-c)'/B;
    temp2 = temp.*transpose(x-c);
    y = exp(-0.5*sum(temp2,2));
    sumY = sum(y);
    y = y/sumY;
    