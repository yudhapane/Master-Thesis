function [phi dphidx] = four(x,BF)
%FOUR  Generate feature values for use in RL approximation
%
% [PHI DPHIDX] = FOUR(X, BF) calculates the feature value of vector x, using
% (N+1)^n Fourier Basis functions (if sine or cosine) or 2(N+1)^n when 
% using both.
% 
% INPUTS:   X: n-dimensional input vector
%           
%           BF.r: [n x 2] matrix containing domain of x-values
%           
%           BF.pb: matrix containing combinations of frequencies
%              See also fourgen
%            
%           BF.rn: matrix containing scaling factors
%             rn = [x(1)_min x(1)_max; x(2)_min x(2)_max; ...; x(n)_min
%             x(n)_max]
%           
%           BF.T: Scalar period (equal for sine & cosine)
%           
%           BF.f: Use sine (0), cosine (1) or both (2) for function
%           approximation
%            
% OUTPUTS:  PHI:    feature vector
%           DPHIDX: derivative to x of feature vector

if BF.f == 0
    % Define feature vectors
    phi     = zeros(length(BF.pb),1);
    dphidx  = zeros(length(BF.pb),length(x));

    % Scale state matrices to arbitrary domain BF.rn
    for i=1:length(x)
        x(i)  = ((x(i)-BF.r(i,1))/(BF.r(i,2)-BF.r(i,1)))*(BF.rn(i,2)-BF.rn(i,1)) + BF.rn(i,1);
    end

    % Calculate feature vector.
    for i=1:length(BF.pb)
        phi(i)                  = sin(2*(pi/BF.T)*BF.pb(i,:)*x);
        dphidx(i,:)             = 2*(pi/BF.T)*BF.pb(i,:)*cos(2*(pi/BF.T)*BF.pb(i,:)*x);
    end
elseif BF.f == 1
    % Define feature vectors
    phi     = zeros(length(BF.pb),1);
    dphidx  = zeros(length(BF.pb),length(x));

    % Scale state matrices to arbitrary domain BF.rn
    for i=1:length(x)
        x(i)  = ((x(i)-BF.r(i,1))/(BF.r(i,2)-BF.r(i,1)))*(BF.rn(i,2)-BF.rn(i,1)) + BF.rn(i,1);
    end

    % Calculate feature vector.
    for i=1:length(BF.pb)
        phi(i)                  = cos(2*(pi/BF.T)*BF.pb(i,:)*x);
        dphidx(i,:)             = -BF.pb(i,:)*sin(2*(pi/BF.T)*BF.pb(i,:)*x);
    end

elseif BF.f == 2
    % Define feature vectors
    phi     = zeros(length(BF.pb)*2,1);
    dphidx  = zeros(length(BF.pb)*2,length(x));

    % Scale state matrices to arbitrary domain BF.rn
    for i=1:length(x)
        x(i)  = ((x(i)-BF.r(i,1))/(BF.r(i,2)-BF.r(i,1)))*(BF.rn(i,2)-BF.rn(i,1)) + BF.rn(i,1);
    end

    % Calculate feature vector.
    for i=1:length(BF.pb)
        phi(i)                      = cos(2*(pi/BF.T)*BF.pb(i,:)*x);
        phi(i+length(BF.pb))        = sin(2*(pi/BF.T)*BF.pb(i,:)*x);
        dphidx(i,:)                 = -2*(pi/BF.T)*BF.pb(i,:)*sin(2*(pi/BF.T)*BF.pb(i,:)*x);
        dphidx(i+length(BF.pb),:)   = 2*(pi/BF.T)*BF.pb(i,:)*cos(2*(pi/BF.T)*BF.pb(i,:)*x);
    end
else
    error('Choose between sine (0), cosine (1) or both (2) approximation');   
end
